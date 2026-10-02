#include "ggml-impl.h"

#include <cstdlib>
#include <exception>

#if defined(__linux__) || (defined(__APPLE__) && !TARGET_OS_TV && !TARGET_OS_WATCH)
#include <csignal>
#include <cstring>
#include <unistd.h>

extern volatile sig_atomic_t ggml_backtrace_printed;
#endif

static std::terminate_handler previous_terminate_handler;

GGML_NORETURN static void ggml_uncaught_exception() {
    ggml_print_backtrace();
#if defined(__linux__) || (defined(__APPLE__) && !TARGET_OS_TV && !TARGET_OS_WATCH)
    ggml_backtrace_printed = 1;
#endif
    if (previous_terminate_handler) {
        previous_terminate_handler();
    }
    abort(); // unreachable unless previous_terminate_handler was nullptr
}

static bool ggml_uncaught_exception_init = []{
    const char * GGML_NO_BACKTRACE = getenv("GGML_NO_BACKTRACE");
    if (GGML_NO_BACKTRACE) {
        return false;
    }
    const auto prev{std::get_terminate()};
    GGML_ASSERT(prev != ggml_uncaught_exception);
    previous_terminate_handler = prev;
    std::set_terminate(ggml_uncaught_exception);
    return true;
}();

#if defined(__linux__) || (defined(__APPLE__) && !TARGET_OS_TV && !TARGET_OS_WATCH)

// A library or driver can call abort() directly. That throws no C++ exception, so the terminate handler above never runs and nothing is printed before the process dies.
// ggml_abort and ggml_uncaught_exception already print a backtrace, so this handler only prints when nothing else has.
volatile sig_atomic_t ggml_backtrace_printed = 0;

GGML_NORETURN static void ggml_fatal_signal_handler(int sig) {
    static volatile sig_atomic_t active = 0;
    if (active) {
        _exit(128 + sig);
    }
    active = 1;

    if (!ggml_backtrace_printed) {
        ggml_print_backtrace_signals();
    }

    // Unblock before re-raising. The signal stays blocked while this handler runs, so without the unblock it is only delivered on return and the _exit below wins.
    signal(sig, SIG_DFL);

    sigset_t set;
    sigemptyset(&set);
    sigaddset(&set, sig);
    sigprocmask(SIG_UNBLOCK, &set, nullptr);

    raise(sig);
    _exit(128 + sig); // only reached if re-raise failed
}

static bool ggml_fatal_signal_handler_init = []{
    if (getenv("GGML_NO_BACKTRACE")) {
        return false;
    }

    // A stack overflow raises SIGSEGV with no stack left for a handler, so use an alternate stack. Fixed size because SIGSTKSZ is not a constant on glibc 2.34 and later.
    static char alt_stack[64 * 1024];
    stack_t ss;
    memset(&ss, 0, sizeof(ss));
    ss.ss_sp = alt_stack;
    ss.ss_size = sizeof(alt_stack);
    sigaltstack(&ss, nullptr);

    struct sigaction sa;
    memset(&sa, 0, sizeof(sa));
    sa.sa_handler = ggml_fatal_signal_handler;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = SA_RESETHAND | SA_NODEFER | SA_ONSTACK;

    sigaction(SIGABRT, &sa, nullptr);
    sigaction(SIGSEGV, &sa, nullptr);
#ifdef SIGBUS
    sigaction(SIGBUS, &sa, nullptr);
#endif
    return true;
}();

#else

void ggml_print_backtrace_signals(void) {
    // platform not supported
}

#endif

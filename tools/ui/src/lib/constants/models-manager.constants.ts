/** Constants of the models manager table. */

/** What a repo group of the table folds, which decides its label. */
export const ModelGroupKind = {
	PROVIDERS: 'providers',
	QUANTS: 'quants',
	VARIANTS: 'variants'
} as const;

export type ModelGroupKind = (typeof ModelGroupKind)[keyof typeof ModelGroupKind];

/** Kind of one collapsible block of the table. */
export const ModelsTableGroupKind = {
	FAVORITES: 'favorites',
	HIDDEN: 'hidden',
	LOADED: 'loaded',
	LOCAL: 'local'
} as const;

export type ModelsTableGroupKind = (typeof ModelsTableGroupKind)[keyof typeof ModelsTableGroupKind];

/** Kinds a provider block carries on top of the manager's own sections. */
export const ModelsTableProviderKind = {
	COMPAT: 'compat',
	PROVIDER: 'provider'
} as const;

export type ModelsTableProviderKind =
	(typeof ModelsTableProviderKind)[keyof typeof ModelsTableProviderKind];

/** Header label of each manager section. */
export const MODELS_TABLE_GROUP_LABELS: Record<ModelsTableGroupKind, string> = {
	[ModelsTableGroupKind.FAVORITES]: 'Favorites',
	[ModelsTableGroupKind.HIDDEN]: 'Hidden models',
	[ModelsTableGroupKind.LOADED]: 'Loaded models',
	[ModelsTableGroupKind.LOCAL]: 'Local models'
};

/** Column the manager's table can be ordered by. */
export const ModelsTableSortKey = {
	CONTEXT: 'context',
	NAME: 'name',
	STATUS: 'status'
} as const;

export type ModelsTableSortKey = (typeof ModelsTableSortKey)[keyof typeof ModelsTableSortKey];

/** Panel the models dialog shows. */
export const MODELS_DIALOG_VIEW = {
	DISCOVER: 'discover',
	MANAGE: 'manage',
	PROVIDERS: 'providers'
} as const;

export type ModelsDialogView = (typeof MODELS_DIALOG_VIEW)[keyof typeof MODELS_DIALOG_VIEW];

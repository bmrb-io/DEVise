#include "types.h"
#include "catalog.h"
#include "RelationManager.h"

const DteEnvVars DTE_ENV_VARS;

const ISchema STRING_SCHEMA("1 string name");

const NewStat DEFAULT_STAT = NewStat();

const Catalog ROOT_CATALOG(DTE_ENV_VARS.rootCatalog);

const Directory MINMAX_DIR(DTE_ENV_VARS.minmaxCatalog);

RelationManager RELATION_MNGR;


#include "types.h"
#include "catalog.h"
#include "RelationManager.h"
#include "DTE/mql/MqlSession.h"

const DteEnvVars DTE_ENV_VARS;

//const ISchema DIR_SCHEMA("2 string name interface interf");
const ISchema DIR_SCHEMA("3 string(100) name string(50) type eolstring(500,';') args");

//not used const ISchema INDEX_SCHEMA("3 string(100) table string(100) name eolstring(500) descriptor");

//not used: const ISchema STAT_SCHEMA("2 string(100) table statdesc descriptor");
const ISchema STRING_SCHEMA("1 string(500) name");

const ISchema EMPTY_SCHEMA("0");

const ISchema SCHEMA_SCHEMA("1 string(500) attrs");

const NewStat DEFAULT_STAT = NewStat();

const Catalog ROOT_CATALOG(DTE_ENV_VARS.rootCatalog);

const Directory MINMAX_DIR(DTE_ENV_VARS.minmaxCatalog);

RelationManager RELATION_MNGR;

MqlSession mqlSession;

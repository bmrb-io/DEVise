#include "MinMax.h"
#include "myopt.h"
#include "catalog.h"

#include <string>

extern const Directory MINMAX_DIR;

bool MinMax::isApplicable(const vector<OptExpr*>& selList){
	vector<OptExpr*>::const_iterator it;
	for(it = selList.begin(); it != selList.end(); ++it){
		
		OptExpr* curr = *it;

		if(curr->getExprType() == OptExpr::FUNCTION_ID){
			OptFunction* function = (OptFunction*) curr;
			OptExprList& args = function->getArgs();
                        if( args.size() == 1 ) {
                          const string& name = function->getName();
                          if( name != "min" && name != "max" ) {
                            return false;
                          }
                        }
		}
		else{
			return false;
		}
	}
	return true;
}

TableAlias* MinMax::createReplacement(TableAlias* table){
	string tableName = table->getTable().toString();
	cerr << "looking for replacement " << tableName << endl;
	Interface* interf = MINMAX_DIR.createInterface(tableName);
	CATCH(cerr << "Warning: "; cerr << currExcept->toString() << endl;);
	if(interf){
		return new TableAlias(interf, table->getAlias());
	}
        return NULL;
}

void MinMax::replace(const string& entry, const Interface* interf){
	MINMAX_DIR.replace(entry, interf);	
}

string MinMax::getPathName(const string& tableName){
	string dirName = DTE_ENV_VARS.minmaxDir;
	dirName += "/" + tableName + "_mm";
	return dirName;
}

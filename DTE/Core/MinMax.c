#include "MinMax.h"
#include "myopt.h"
#include "catalog.h"

#include <string>

extern const Directory MINMAX_DIR;

bool MinMax::isApplicable(List<BaseSelection*>* selList){
	if(!selList){
		return false;
	}
	for(selList->rewind(); !selList->atEnd(); selList->step()){
		
		BaseSelection* curr = selList->get();

		if(curr->isGlobal() && curr->selectID() == METHOD_ID){
			Method* function = (Method*) curr;
			List<BaseSelection*>* args = function->getArgs();
			int numArgs = args->cardinality();
			const string* name = function->getName();
			args->rewind();
			if(*name == "min" && numArgs == 1){
			}
			else if(*name == "max" && numArgs == 1){
			}
			else{
				return false;
			}
		}
		else{
			return false;
		}
	}
	return true;
}

TableAlias* MinMax::createReplacement(TableAlias* table){
	string* alias = new string(*table->getAlias());
	string tableName = table->getTable()->toString();
	cerr << "looking for replacement " << tableName << endl;
	Interface* interf = MINMAX_DIR.createInterface(tableName);
	CATCH(cerr << "Warning: "; currExcept->display(cerr); cerr << endl;);
	if(interf){
		return new QuoteAlias(interf, alias);
	}
	else{
		delete alias;
		return NULL;
	}
}

void MinMax::replace(const string& entry, const Interface* interf){
	MINMAX_DIR.replace(entry, interf);	
}

string MinMax::getPathName(const string& tableName){
	string dirName = DTE_ENV_VARS.minmaxDir;
	dirName += "/" + tableName + "_mm";
	return dirName;
}

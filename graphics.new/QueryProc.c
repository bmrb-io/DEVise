/* queryProc.c */
#include "QueryProc.h"
#include "DispQueryProc.h"
#include "Exit.h"

QueryProc *QueryProc::_queryProc = NULL;
int QueryProc::_numQueryProcs =0; /* number of query processors */
QPRec QueryProc::_qpArray[MAX_QUERYPROCS];


void QueryProc::Register(char *name, QueryProcGen *gen){
	if (_numQueryProcs == MAX_QUERYPROCS){
		fprintf(stderr,"too many query processors\n");
		Exit::DoExit(1);
	}
	_qpArray[_numQueryProcs].name = name;
	_qpArray[_numQueryProcs++].gen = gen;
}

QueryProc *QueryProc::Instance(){
	if (_queryProc == NULL){
		/* return new DispQueryProcSimple(); */
		char *qpName = Init::QueryProc();
		if (strcmp(qpName,"default") == 0){
			_queryProc = new DispQueryProcFull();
		} else {
			int i;
			Boolean found = false;
			for (i=0; i < _numQueryProcs; i++){
				if (strcmp(_qpArray[i].name,qpName) == 0 ){
					_queryProc = (*_qpArray[i].gen)();
					found = true;
					break;
				}
			}
			if (!found){
				fprintf(stderr,"can't find query proc %s\n",qpName);
				Exit::DoExit(1);
			}
		}
	}
	return _queryProc;
}

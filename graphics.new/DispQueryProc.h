/*
  $Id$

  $Log$*/

/* query processor under the control of the dispatcher */

#ifndef DispQueryProc_h
#define DispQueryProc_h
#include "Dispatcher.h"
#include "QueryProcSimple.h"
#include "QueryProcFull.h"

class DispQueryProcSimple: public QueryProcSimple, private DispatcherCallback {
public:
	DispQueryProcSimple();

private:
	/* from DispatcerhCallback */
	virtual char *DispatchedName() {
		return "DispQueryProcSimple";
	}
	void Run();
	void Cleanup(){
		PrintStat();
	}

};

class DispQueryProcFull: public QueryProcFull, private DispatcherCallback {
public:
	DispQueryProcFull();

private:
	/* from DispatcerhCallback */
	char *DispatchedName(){
		return "DispQueryProcFull";
	}
	void Run();
	void Cleanup(){
		/*
		PrintStat();
		*/
	}
};
#endif

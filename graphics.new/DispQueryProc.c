/* DispQueryProc.c */
#include "DispQueryProc.h"

/******************************************************************
constructor
*********************************************************************/
DispQueryProcSimple::DispQueryProcSimple(){
	Dispatcher::Current()->Register(this, 20);
}

/********************************************************************8
Called by dispatcher when we can run
**********************************************************************/
void DispQueryProcSimple::Run(){
	ProcessQuery();
}

/******************************************************************
constructor
*********************************************************************/
DispQueryProcFull::DispQueryProcFull(){
	Dispatcher::Current()->Register(this,20);
}

/********************************************************************8
Called by dispatcher when we can run
**********************************************************************/
void DispQueryProcFull::Run(){
	ProcessQuery();
}

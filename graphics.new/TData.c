/*
  $Id$

  $Log$*/

#include <stdio.h>
#include "TData.h"
#include "Exit.h"

TData::~TData(){
/*
printf("TData destructor\n");
*/
}

/* For writing records. Default: not implemented. */
void TData::WriteRecs(RecId startId, int numRecs, void *buf){
	fprintf(stderr,"TData::WRiteRecs: not implemented\n");
	Exit::DoExit(2);
}


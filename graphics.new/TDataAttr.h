/*
  $Id$

  $Log$*/

/* TData user defined attributes */

#ifndef TDataAttr_h
#define TDataAttr_h
#include "TData.h"

const int GDataAttrNum = 0;
inline IsGData(TData *tdata){
	return tdata->UserAttr(GDataAttrNum) != -1;
}
#endif

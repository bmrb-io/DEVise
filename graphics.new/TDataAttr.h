/*
  ========================================================================
  DEVise Data Visualization Software
  (c) Copyright 1992-1995
  By the DEVise Development Group
  Madison, Wisconsin
  All Rights Reserved.
  ========================================================================

  Under no circumstances is this software to be copied, distributed,
  or altered in any way without prior permission from the DEVise
  Development Group.
*/

/*
  $Id$

  $Log$
  Revision 1.2  1995/09/05 22:15:53  jussi
  Added CVS header.
*/

/* TData user defined attributes */

#ifndef TDataAttr_h
#define TDataAttr_h

#include "TData.h"

const int GDataAttrNum = 0;
inline int IsGData(TData *tdata)
{
  return tdata->UserAttr(GDataAttrNum) != -1;
}

#endif

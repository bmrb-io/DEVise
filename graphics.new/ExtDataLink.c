/*
  ========================================================================
  DEVise Data Visualization Software
  (c) Copyright 2001
  By the DEVise Development Group
  Madison, Wisconsin
  All Rights Reserved.
  ========================================================================

  Under no circumstances is this software to be copied, distributed,
  or altered in any way without prior permission from the DEVise
  Development Group.
*/

/*
  Implementation of the ExtDataLink class.
 */

/*
  $Id$

  $Log$
 */

#define _ExtDataLink_C_

#include <stdio.h>

#include "ExtDataLink.h"
#include "ViewGraph.h"
#include "TDataMap.h"
#include "TData.h"

#define DEBUG 0

//-----------------------------------------------------------------------------
// Constructor.
ExtDataLink::ExtDataLink(const char *name) :
    MasterSlaveLink(name, VISUAL_EXTDATA)
{
#if (DEBUG >= 1)
  printf("ExtDataLink::ExtDataLink(%s)\n", name);
#endif
}

//-----------------------------------------------------------------------------
// Destructor.
ExtDataLink::~ExtDataLink()
{
#if (DEBUG >= 1)
  printf("ExtDataLink(%s)::~ExtDataLink()\n", GetName());
#endif
}

//-----------------------------------------------------------------------------
// descriptionTEMPTEMP
void
ExtDataLink::Initialize()
{
#if (DEBUG >= 1)
  printf("ExtDataLink(%s)::Initialize()\n", GetName());
#endif
}

//-----------------------------------------------------------------------------
// Propagates updates when the leader view is done.
void
ExtDataLink::Done()
{
#if (DEBUG >= 1)
  printf("ExtDataLink(%s)::Done()\n", GetName());
#endif

  // Refresh the views of this link (invalidate their TDatas).
  int index = InitIterator();
  while(More(index)) {
    ViewGraph *view = Next(index);
#if defined(DEBUG)
    printf("Refreshing follower view %s\n", view->GetName());
#endif
    view->SetAutoScale(false);
    TDataMap *tdMap = view->GetFirstMap();
    TData *tdata = tdMap->GetPhysTData();
    tdata->InvalidateTData();
  }
  DoneIterator(index);
}

//-----------------------------------------------------------------------------
// descriptionTEMPTEMP
void
ExtDataLink::Abort()
{
#if (DEBUG >= 1)
  printf("ExtDataLink(%s)::Abort()\n", GetName());
#endif
}

/*===========================================================================*/

/*
  ========================================================================
  DEVise Data Visualization Software
  (c) Copyright 1992-1996
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
  Revision 1.3  1996/01/25 20:23:17  jussi
  Added copyright notice and improved code.

  Revision 1.2  1995/09/05 22:14:44  jussi
  Added CVS header.
*/

#include "DispQueryProc.h"

DispQueryProcSimple::DispQueryProcSimple()
{
  Dispatcher::Current()->Register(this, 20);
}

/********************************************************************8
Called by dispatcher when we can run
**********************************************************************/

void DispQueryProcSimple::Run()
{
  ProcessQuery();
}

DispQueryProcFull::DispQueryProcFull()
{

  Dispatcher::CreateMarker(readFd,writeFd);

  Dispatcher::Current()->Register(this, 20,GoState,false,readFd);

  Dispatcher::InsertMarker(writeFd);

}

/********************************************************************8
Called by dispatcher when we can run
**********************************************************************/

void DispQueryProcFull::Run()
{
  ProcessQuery();
}

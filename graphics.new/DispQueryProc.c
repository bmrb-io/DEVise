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
  Revision 1.5  1996/06/23 20:32:49  jussi
  Cleaned up.

  Revision 1.4  1996/04/20 19:56:36  kmurli
  QueryProcFull now uses the Marker calls of Dispatcher class to call
  itself when needed instead of being continuosly polled by the Dispatcher.

  Revision 1.3  1996/01/25 20:23:17  jussi
  Added copyright notice and improved code.

  Revision 1.2  1995/09/05 22:14:44  jussi
  Added CVS header.
*/

#include "DispQueryProc.h"

DispQueryProcSimple::DispQueryProcSimple()
{
  Dispatcher::CreateMarker(readFd, writeFd);
  Dispatcher::Current()->Register(this, 20, GoState, false, readFd);
  Dispatcher::InsertMarker(writeFd);
  Timer::Queue(QP_TIMER_INTERVAL, this, 0);
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
  Dispatcher::CreateMarker(readFd, writeFd);
  Dispatcher::Current()->Register(this, 20, GoState, false, readFd);
  Dispatcher::InsertMarker(writeFd);
  Timer::Queue(QP_TIMER_INTERVAL, this, 0);
}

void DispQueryProcFull::Run()
{
  ProcessQuery();
}

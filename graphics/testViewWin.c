/*
  ========================================================================
  DEVise Data Visualization Software
  (c) Copyright 1992-1998
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
  Revision 1.2  1995/09/05 21:13:52  jussi
  Added/update CVS header.
*/

#include "Dispatcher.h"
#include "ViewWinVer.h"
#include "ViewWinHor.h"

main()
{
  ViewWin *vw1 = new ViewWinVer("Vertical");
  ViewWin *vw2 = new ViewWinVer("sub1", vw1);
  ViewWin *vw3 = new ViewWinVer("sub2", vw1);
  
  ViewWin *vw4 = new ViewWinHor("Horizontal");
  ViewWin *vw5 = new ViewWinHor("sub5", vw4);
  ViewWin *vw6 = new ViewWinHor("sub6", vw4);
  Dispatcher::RunNoReturn();
}

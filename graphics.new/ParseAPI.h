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
  Revision 1.4  1998/02/12 17:17:05  wenger
  Merged through collab_br_2; updated version number to 1.5.1.

  Revision 1.3.2.1  1998/01/28 22:43:59  taodb
  Added support for group communicatoin

  Revision 1.3  1997/11/24 23:15:12  weaver
  Changes for the new ColorManager.

  Revision 1.2.10.1  1997/05/21 20:40:42  weaver
  Changes for new ColorManager

  Revision 1.2  1996/05/13 18:14:37  jussi
  Changed definition of "flag" values: API_CMD, API_ACK, API_NAK,
  API_CTL.

  Revision 1.1  1996/05/11 17:27:22  jussi
  Initial revision. Moved all API parsing to ParseAPI.C so that
  ServerAPI.c and TkControl.c would not have to duplicate it.
*/

#ifndef ParseAPI_h
#define ParseAPI_h

class ControlPanel;
class MapInterpClassInfo;

#define API_CMD 0
#define API_ACK 1
#define API_NAK 2
#define API_CTL 3
#define API_GRP 4

extern int	ParseAPIColorCommands(int argc, char** argv, ControlPanel* control);
//******************************************************************************
#endif

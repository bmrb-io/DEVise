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

extern int	ParseAPI(int argc, char** argv, ControlPanel* control);
extern int	ParseAPIColorCommands(int argc, char** argv, ControlPanel* control);

//******************************************************************************
#endif

/*
  ========================================================================
  DEVise Data Visualization Software
  (c) Copyright 1992-1997
  By the DEVise Development Group
  Madison, Wisconsin
  All Rights Reserved.
  ========================================================================

  Under no circumstances is this software to be copied, distributed,
  or altered in any way without prior permission from the DEVise
  Development Group.
*/

/*
  This is a subclass of the ControlPanel class to be used for test programs
  (specifically, testWindowSpeed).  Its main purpose is to eliminate
  unnecessary dependecies on various parts of the Devise code.
 */

/*
  $Id$

  $Log$
 */

#ifndef _TestControl_h_
#define _TestControl_h_


#include "Control.h"

class TestControlPanel: public ControlPanel
{
public:
  /* Most of these functions are just dummies that should never be called.
   * We just need them so we don't have any pure virtual functions. */
  virtual void SelectView(View *view) {}
  virtual void SetBusy() {}
  virtual void SetIdle() {}
  virtual Boolean IsBusy() { return false; }
  virtual int ReturnVal(u_short flag, char *result) { return 0; }
  virtual int ReturnVal(int argc, char **argv) { return 0; }
  virtual GroupDir *GetGroupDir() { return NULL; }
  virtual MapInterpClassInfo *GetInterpProto() { return NULL; }
  virtual int AddReplica(char *hostName, int port) { return 1; }
  virtual int RemoveReplica(char *hostName, int port) { return 1; }
  virtual void OpenDataChannel(int port) {}
  virtual int getFd() { return -1; }

protected:
  virtual void SubclassInsertDisplay(DeviseDisplay *disp,
                                     Coord x, Coord y,
                                     Coord w, Coord h) {}
  virtual void SubclassDoInit() {}
};

#endif /* _TestControl_h_ */

/*============================================================================*/

/*
  ========================================================================
  DEVise Data Visualization Software
  (c) Copyright 1999
  By the DEVise Development Group
  Madison, Wisconsin
  All Rights Reserved.
  ========================================================================

  Under no circumstances is this software to be copied, distributed,
  or altered in any way without prior permission from the DEVise
  Development Group.
*/

/*
  Description of module.
 */

/*
  $Id$

  $Log$
  Revision 1.1  1999/11/22 18:11:47  wenger
  Fixed 'command buffer conflict' errors, other command-related cleanup.

 */

#ifndef _ControlPanelSimple_h_
#define _ControlPanelSimple_h_

#include <sys/types.h>
#include <string>

#include "DeviseTypes.h"
#include "Control.h"

// Note: we're defining this class so we have a ControlPanel * to pass
// to ParseAPI so it can return its results.
// What we really need here is an _object_ that inherits all of the methods
// except ReturnVal() from the ControlPanel::Instance() object and also has
// its own interpreter...
// Anyhow, the important thing is that all methods of the ControlPanel class
// that are called from ParseAPI() are implemented here (unless they're
// static).
// RKW Oct. 2, 1997.

class ControlPanelSimple : public ControlPanel {
public:
  // These member functions are unique to this class.
  ControlPanelSimple() {
	_result = "";
  }
  virtual ~ControlPanelSimple() {
  }

  virtual int ReturnVal(u_short flag, char *result) {
    _valueReturned = true;
	_result = result;
    return 1;
  }
  virtual int ReturnVal(int argc, char **argv) {
    _valueReturned = true;
	_result = "";
    for(int i = 0; i < argc; i++) {
	  if (i > 0) _result += " ";
	  Boolean needsBraces = (strlen(argv[i]) == 0) ||
	      (strchr(argv[i], ' ') != NULL) || (strchr(argv[i], '\t') != NULL) ||
		  (strchr(argv[i], '$') != NULL) || (strchr(argv[i], '"') != NULL);
	  if (needsBraces) _result += "{";
	  _result += argv[i];
	  if (needsBraces) _result += "}";
	}
    return 1;
  }
  const char *GetResult() { return _result.c_str(); }

  // These member functions are called in ParseAPI(), and therefore need
  // to call the appropriate function in the "real" ControlPanel object.
  virtual Mode GetMode() { return ControlPanel::Instance()->GetMode(); }
  virtual void SetMode(Mode mode) { ControlPanel::Instance()->SetMode(mode); }

  virtual void SetBusy() { ControlPanel::Instance()->SetBusy(); }
  virtual void SetIdle() { ControlPanel::Instance()->SetIdle(); }

  virtual void SyncNotify() {}
  virtual void Raise() {}
  virtual void NotifyFrontEnd(const char *script) {}

  virtual void DestroySessionData() {
      ControlPanel::Instance()->DestroySessionData(); }
  virtual void RestartSession() { ControlPanel::Instance()->RestartSession(); }

  virtual void SetBatchMode(Boolean mode) {
      ControlPanel::Instance()->SetBatchMode(mode); }

  virtual void SetSyncNotify() { ControlPanel::Instance()->SetSyncNotify(); }

  virtual GroupDir *GetGroupDir() {
      return ControlPanel::Instance()-> GetGroupDir(); }

  virtual MapInterpClassInfo *GetInterpProto() {
      return ControlPanel::Instance()->GetInterpProto(); }

  virtual void OpenDataChannel(int port) {
      ControlPanel::Instance()->OpenDataChannel(port); }
  virtual int getFd() { return ControlPanel::Instance()->getFd(); }

  // The following member functions are needed just because they are pure
  // virtual in the base class.
  virtual void SelectView(View *view) {}
  virtual void ShowMouseLocation(const char *dataX, const char *dataY) {}
  virtual Boolean IsBusy() { return false; }
  virtual void SubclassInsertDisplay(DeviseDisplay *disp, Coord x, Coord y,
      Coord w, Coord h) {}
  virtual void SubclassDoInit() {}

  // This is the interpreter used for opening a session.
  string _result;
};

#endif // _ControlPanelSimple_h_

/*============================================================================*/

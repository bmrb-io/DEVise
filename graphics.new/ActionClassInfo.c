/*
  ========================================================================
  DEVise Data Visualization Software
  (c) Copyright 1992-2000
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
  Revision 1.4  1999/11/30 22:28:18  wenger
  Temporarily added extra debug logging to figure out Omer's problems;
  other debug logging improvements; better error checking in setViewGeometry
  command and related code; added setOpeningSession command so Omer can add
  data sources to the temporary catalog; added removeViewFromPile (the start
  of allowing piling of only some views in a window).

  Revision 1.3  1998/01/14 16:39:14  wenger
  Merged cleanup_1_4_7_br_6 thru cleanup_1_4_7_br_7.

  Revision 1.2.12.1  1998/01/12 20:34:11  wenger
  Fixed duplicate frees in multi that caused core dump on Linux.

  Revision 1.2  1997/03/28 16:09:39  wenger
  Added headers to all source files that didn't have them; updated
  solaris, solsparc, and hp dependencies.

  Revision 1.1  1996/05/07 16:35:15  jussi
  Moved files from graphics directory.

  Revision 1.3  1995/09/05 21:12:21  jussi
  Added/update CVS header.

  Revision 1.2  1995/09/05 20:39:19  jussi
  Added CVS header.
*/

#include <stdio.h>
#include "ActionClassInfo.h"
#include "Action.h"
#include "View.h"
#include "Util.h"

ActionClassInfo::ActionClassInfo(const char *className, GenAction *gen){
	_gen = gen;
	_className = className;
	_instName = NULL;
	_action = NULL;
}

ActionClassInfo::ActionClassInfo(const char *className, const char *instName, 
	Action *action) {
	_gen = NULL;
	_className = className;
	_instName = instName;
	_action = action;
}

ActionClassInfo::~ActionClassInfo(){
	delete _action;
}

const char *ActionClassInfo::ClassName(){
	return _className;
}

static const char *args[2];
static char buf1[80], buf2[80];
void ActionClassInfo::ParamNames(int &argc, const char **&argv){
	argc = 1;
	argv = args;
	if (_instName != NULL){
		args[0] = buf1;
		sprintf(buf1,"name %s",_instName);
	}
	else args[0] = "name";
}

ClassInfo *ActionClassInfo::CreateWithParams(int argc, const char * const *argv) {
	if (argc != 1){
		reportErrNosys("ActionClassInfo::CreateWithParams: need 1 params\n");
		Exit::DoExit(2);
	}
	char *name = CopyString(argv[0]);
	Action *action = _gen->MakeAction(name);

	return new ActionClassInfo(_className, name, action);
}


/* Set default parameters */
void ActionClassInfo::SetDefaultParams(int argc, const char * const *argv){}

/* Get default parameters */
void ActionClassInfo::GetDefaultParams(int &argc, const char **&argv){
	argc = 0;
	argv = NULL;
}

const char *ActionClassInfo::InstanceName(){
	return _instName;
}

void *ActionClassInfo::GetInstance(){
	return _action;
}

/* Get parameters that can be used to re-create this instance */
void ActionClassInfo::CreateParams(int &argc, const char **&argv) {
	argc = 1;
	argv= args;
	args[0] = buf1;
	sprintf(buf1,"%s",_instName);
}

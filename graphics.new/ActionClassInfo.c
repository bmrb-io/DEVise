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
  $Id$

  $Log$
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

ActionClassInfo::ActionClassInfo(char *className, GenAction *gen){
	_gen = gen;
	_className = className;
	_instName = NULL;
	_action = NULL;
}

ActionClassInfo::ActionClassInfo(char *className, char *instName, 
	Action *action) {
	_className = className;
	_instName = instName;
	_action = action;
}

ActionClassInfo::~ActionClassInfo(){
	if (_action != NULL)
		delete _action;
}

char *ActionClassInfo::ClassName(){
	return _className;
}

char *args[2];
char buf1[80], buf2[80];
void ActionClassInfo::ParamNames(int &argc, char **&argv){
	argc = 1;
	argv = args;
	if (_instName != NULL){
		args[0] = buf1;
		sprintf(buf1,"name %s",_instName);
	}
	else args[0] = "name";
}

ClassInfo *ActionClassInfo::CreateWithParams(int argc, char **argv) {
	if (argc != 1){
		fprintf(stderr,"ActionClassInfo::CreateWithParams: need 1 params\n");
		Exit::DoExit(2);
	}
	char *name = CopyString(argv[0]);
	Action *action = _gen->MakeAction(name);

	return new ActionClassInfo(_className, name, action);
}


/* Set default parameters */
void ActionClassInfo::SetDefaultParams(int argc, char **argv){}

/* Get default parameters */
void ActionClassInfo::GetDefaultParams(int &argc, char **&argv){
	argc = 0;
	argv = NULL;
}

char *ActionClassInfo::InstanceName(){
	return _instName;
}

void *ActionClassInfo::GetInstance(){
	return _action;
}

/* Get parameters that can be used to re-create this instance */
void ActionClassInfo::CreateParams(int &argc, char **&argv) {
	argc = 1;
	argv= args;
	args[0] = buf1;
	sprintf(buf1,"%s",_instName);
}

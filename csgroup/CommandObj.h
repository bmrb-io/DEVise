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
  Declaration of Server class.
 */

/*
  $Id$

  $Log$
  Revision 1.1  1998/02/26 20:35:09  taodb
  Removed ParaseAPI() interface, and added CommandObject interface

 */

#ifndef _CommandObj_h_
#define _CommandObj_h_

#include "Control.h"
#include "View.h"
#include "codec.h"
#include "Exit.h"
#include "Server.h"
#include "DeviseServer.h"
#if !defined(SGI)
#include <sys/varargs.h>
#else
#include <stdarg.h>
#endif

#define MAX_ARGS 20
class Server;
class CommandObj
{
	private:
		DeviseServer *_server;
		int		argc;
		char*	argv[MAX_ARGS];
	public:
		CommandObj(Server*);
		~CommandObj();
		bool filterCmd(int, int argc, char **argv, char *&errmsg); 
		bool SetVisualFilter(View   *view,  VisualFilter *filter);
		int Serialize (int args, ...);
}; 
extern CommandObj* GetCommandObj();
#endif


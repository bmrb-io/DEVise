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
  Revision 1.3  1998/03/11 18:25:06  wenger
  Got DEVise 1.5.2 to compile and link on Linux; includes drastically
  reducing include dependencies between csgroup code and the rest of
  the code, and within the csgroup code.  (Note: running collaboration
  doesn't work yet.)

  Revision 1.2  1998/02/27 20:47:46  wenger
  More SGI compile fixes.

  Revision 1.1  1998/02/26 20:35:09  taodb
  Removed ParaseAPI() interface, and added CommandObject interface

 */

#ifndef _CommandObj_h_
#define _CommandObj_h_

#if !defined(SGI) && !defined(LINUX)
#include <sys/varargs.h>
#else
#include <stdarg.h>
#endif

#define MAX_ARGS 20

class Server;
class DeviseServer;
class View;
class VisualFilter;

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
		bool Run(int argc, char** argv);
		int Serialize (int args, ...);
}; 
extern CommandObj* GetCommandObj();
#endif


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
  Declaration of CommandObj class.
 */

/*
  $Id$

  $Log$
  Revision 1.6  1998/07/29 14:19:39  wenger
  Mods to compile DEVise on Alpha/OSF again (partially successful); mods to
  allow static linking on Linux.

  Revision 1.5  1998/05/02 08:38:55  taodb
  Added command logging and playing support
  Added communication support for JAVA Screen

  Revision 1.4  1998/03/12 02:09:05  wenger
  Fixed dynamic memory errors in collaboration code that caused core dump
  on Linux; collaboration code now tolerates interruption of accept() and
  select() in some cases; fixed excessive CPU usage by collaborator
  (select timeout now non-zero); fixed some other collaboration bugs.

  Revision 1.3  1998/03/11 18:25:06  wenger
  Got DEVise 1.5.2 to compile and link on Linux; includes drastically
  reducing include dependencies between csgroup code and the rest of
  the code, and within the csgroup code.  (Note: running collaboration
  doesn't work yet.)

 */

#include "CommandObj.h"
#include "CmdDescriptor.h"
#include "Csprotocols.h"
#include "codec.h"
#include "devise_varargs.h"
#include "ServerAPI.h"
#include <stdio.h>

#include "DeviseServer.h"
#include "View.h"

CommandObj::CommandObj(Server* server)
{
	int	i;

	_server = (DeviseServer*)server;
	argc	= 0;
	for (i=0; i< MAX_ARGS; ++i) {
		argv[i] = NULL;
    }
}

CommandObj::~CommandObj()
{
	int	i;
	for (i=0; i< MAX_ARGS; ++i) {
		free(argv[i]);
    }
}

bool
CommandObj::filterCmd(int cid, int ac, char**av, char*& errmsg)
{
	    bool        success = true;

		cerr << "****should not come here"<<endl;
/*
		// argv[0], holds the NULL groupkey, reserved for future use
		if ((_server->_clients[cid].gname != NULL)&&
			(_server->_clients[cid].active))
			success = _server->GroupCast(_server->_clients[cid].gname, 
								_server->_clients[cid].cname,
								SS_CSS_Cmd,
								ac-1, &av[1], errmsg);
		else
			success = false;

		if ((_server->_clients[cid].gname != NULL)&&
			!(_server->_clients[cid].active))
		{
			success = false;
			errmsg = "Non-leader should not operate!";
			_server->ReturnVal(cid, (u_short)API_NAK, errmsg);
		}
		else
		{
			// process the command locally
			_server->ProcessCmd(cid,ac -1, &av[1]);
		}
*/
		return success;
}

bool
CommandObj::SetVisualFilter(View* view, VisualFilter * filterp)
{
	// implement serilization
	bool	success;
	Serialize(6,
		TYP_STRING, "setFilter",
		TYP_STRING, view->GetName(),
		TYP_DOUBLE, &filterp->xLow,
		TYP_DOUBLE, &filterp->yLow,
		TYP_DOUBLE, &filterp->xHigh,
		TYP_DOUBLE, &filterp->yHigh);
	success = Run(argc, argv);
	return success;
}


bool
CommandObj::Run(int ac, char** av)
{
	// propagate the serilized results
	// route the command to the Command Container object
	CmdSource cmdSrc(CmdSource::USER, CLIENT_INVALID);
	CmdDescriptor cmdDes(cmdSrc, CmdDescriptor::FOR_SERVER);

	_server->RunCmd(ac, av, cmdDes);
	return cmdDes.isSuccess();
}

int
CommandObj::Serialize(int args,...)
{
	va_list	pvar;
	int		index = 0;
	int		*intArg = NULL;
	char	*strArg = NULL;
	double	*doubleArg = NULL;
	char	buf[200];

	va_start(pvar, args);
	while (args >0)
	{
		int	type;	
		args --;
		type = va_arg(pvar, int);
		switch (type)
		{
			case TYP_INTEGER:
				intArg = va_arg(pvar, int*);
				sprintf(buf, "%d", *intArg);
				if (argv[index] != NULL) {
					free(argv[index]);
				}
				argv[index++] = strdup(buf);
				break;
			case TYP_STRING:
				strArg = va_arg(pvar, char*);
				if (argv[index] != NULL) {
					free(argv[index]);
				}
				argv[index++] = strdup(strArg);
				break;
			case TYP_DOUBLE:
				doubleArg = va_arg(pvar, double*);
				if (argv[index] != NULL) {
					free(argv[index]);
				}
				sprintf(buf, "%g", *doubleArg);
				argv[index++] = strdup(buf);
				break;
		}
		if (index > MAX_ARGS)
		{
			fprintf(stderr, "CommandObj: too many arguments\n");
			Exit::DoExit(1);
		}
	}
	argc = index;
	va_end(pvar);
	return args;
}

CommandObj*
GetCommandObj()
{
	return ((ServerAPI*)ControlPanel::_controlPanel)->_server->cmdObj;
}


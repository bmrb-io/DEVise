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
  Implementation of Client (and CompDate) classes.
 */

/*
  $Id$

  $Log$
  Revision 1.9  1998/08/21 22:16:01  wenger
  Got DEVise 1.5.4 to compile on SPARC/SunOS (sundance) -- to make statically-
  linked DEVise for distribution.

  Revision 1.8  1998/08/17 21:03:37  wenger
  Changed deviseb and devisec to work properly with current devised
  (clients don't expect to be sent slot number at startup).

  Revision 1.7  1998/07/29 14:19:38  wenger
  Mods to compile DEVise on Alpha/OSF again (partially successful); mods to
  allow static linking on Linux.

  Revision 1.6  1998/05/02 08:38:50  taodb
  Added command logging and playing support
  Added communication support for JAVA Screen

  Revision 1.5  1998/03/11 18:25:05  wenger
  Got DEVise 1.5.2 to compile and link on Linux; includes drastically
  reducing include dependencies between csgroup code and the rest of
  the code, and within the csgroup code.  (Note: running collaboration
  doesn't work yet.)

  Revision 1.4  1998/02/26 20:35:08  taodb
  Removed ParaseAPI() interface, and added CommandObject interface

  Revision 1.3  1998/02/26 18:54:04  wenger
  Got everything to compile on haha -- still have a link problem, though.

  Revision 1.2  1998/02/12 17:14:31  wenger
  Merged through collab_br_2; updated version number to 1.5.1.

  Revision 1.1.2.3  1998/02/02 07:47:43  liping
  Fixed bugs related to: queue algorithms, bucket expansion, iterator reset

  Revision 1.1.2.2  1998/01/30 08:52:43  taodb
  Reduced run-time debug messages, fixed memory leaks caused by message headers
  and also fixed a bug in master-slave handshaking

  Revision 1.1.2.1  1998/01/28 22:42:10  taodb
  Added support for group communicatoin

 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include "devise_varargs.h"
#if defined(SGI) || defined(LINUX) || defined(SUN)
#include <sys/param.h> // for MAXHOSTNAMELEN
#endif

#include "Client.h"
#include "ClientAPI.h"
#include "Csprotocols.h"
#include "keys.h"

//#define DEBUG
#undef DEBUG
#define DOASSERT(c,r) {if (!(c)) DoAbort(r); } 
static char* errBase[Client::MAX_ERRORS]=
{
	"Try to specify more than one group",
	"Create failure",
	"Join failure",
	"No group was specified",
	"Fail to get the token",
	"Cannot give up the token",
	"Fail to send back token",
	"Fail to leave the group",
	"Fail to give up the token",
	"Already have the token"
};

char *CompDate::Get()
{
  return __DATE__;
}

Client::Client(char *name, char *hostname, int port, char* initStr)
{
	int retval;
	
	_name = strdup(name);
	_hostname = strdup(hostname);
	DOASSERT(_name && _hostname, "Out of memory");
	_port = port;
	csk = NULL;
	_result = NULL;
	_cmd = NULL;
	 panelMajorCmd = NULL;
	 panelSubCmd = NULL;
	 panelInfo = NULL;
	_serverSlot = -1;

#ifdef DEBUG
	printf("Connecting to server %s:%d.\n", _hostname, _port);
#endif
	
	_serverFd = NetworkModedOpen(_hostname, _port, CONNECT_ALWAYS, 3);
	
	//probe the server with client's current host name
	char localhostname[MAXHOSTNAMELEN+1];
	gethostname(localhostname, MAXHOSTNAMELEN);
	
	retval = SendServerCmd(3, localhostname, CS_Init_Req, initStr);
	if (retval <0)
	{
		fprintf (stderr,"Server does not respond, quit\n");
		exit(1);
	}
	_groupname = NULL;
	_grpstat = EMPTY;
	if (_serverFd < 0)
	{
		fprintf (stderr,"Cannot connect to the server\n");
		exit(1);
	}
}

Client::~Client()
{
  (void)NetworkClose(_serverFd);
  free( _name);
  free(_hostname);
  delete _result;
  delete _cmd;
  delete csk;
}

int
Client::readInteger(int fd, int&num)
{
	char buf[SLOTNUMSIZE+1];
	int retval;

	num = -1;
	int nbytes = read(fd, buf, SLOTNUMSIZE);
	if (nbytes == SLOTNUMSIZE)
	{
		buf[SLOTNUMSIZE] = 0;
		num = atoi(buf);
		retval = 1;
	}
	else
		retval = -1;

	return retval;
}


void
Client::SendPanelErr(char* err)
{
	char	info[128] = "ERROR: ";

	strcat(info, err);
	SetPanelMajorCmd("StatusLine");
	SetPanelSubCmd("hilight");
	SetPanelInfo(info);
	SendPanelCmd();
}
void 
Client::SendPanelCmd()
{
	char	*panelArgv[8];
	panelArgv[0] =  panelMajorCmd;
	panelArgv[1] =  panelSubCmd;
	panelArgv[2] = "{";
	panelArgv[3] =  panelInfo;
	panelArgv[4] = "}";
	if (_groupname == NULL)
		panelArgv[5] = "{ No Group }";
	else
		panelArgv[5] = _groupname;

	if (_grpstat & ACTIVE)
		panelArgv[6] = "1";
	else
		panelArgv[6] = "0";

	if (_grpstat & REQUESTED)
		panelArgv[7] = "1";
	else
		panelArgv[7] = "0";

	ControlCmd(8, panelArgv);
}
void
Client::SetPanelMajorCmd(char* cmd)
{
	DOASSERT((cmd!=NULL), "SetPanelMajorCmd has a NULL parameter");
	if (panelMajorCmd != NULL)
		free(panelMajorCmd);
	panelMajorCmd = strdup(cmd);
}
void
Client::SetPanelSubCmd(char* cmd)
{
	DOASSERT((cmd!=NULL), "SetPanelSubCmd has a NULL parameter");
	if (panelSubCmd != NULL)
		free(panelSubCmd);
	panelSubCmd = strdup(cmd);
}
void
Client::SetPanelInfo(char* cmd)
{
	DOASSERT((cmd!=NULL), "SetPanelInfo has a NULL parameter");
	if (panelInfo != NULL)
		free(panelInfo);
	panelInfo = strdup(cmd);
}
// call back virtual function that can be overloaded
int
Client::NotifyGrabbed()
{
	char info[128];
	_grpstat|= ACTIVE;


	if (_groupname != NULL)
	{
		sprintf(info, "Leader for group:%s ",_groupname);
		SetPanelSubCmd("normal");
		SetPanelMajorCmd("StatusLine");
		SetPanelInfo(info);
		SendPanelCmd();
	}
	else
		SendPanelErr("Group not defined");
		

	return _grpstat;
}

// call back virtual function that can be overloaded
int 
Client::RequestRelinquish()
{
	char info[128];
	DOASSERT(((_grpstat & ACTIVE)!=0), "Inactive client was requested!");
	_grpstat |= REQUESTED;

	if (_groupname !=NULL)
	{
		sprintf(info, "Leadership for group:%s is requested!", _groupname);
		SetPanelSubCmd("normal");
		SetPanelMajorCmd("StatusLine");
		SetPanelInfo(info);
		SendPanelCmd();
	}
	else
		SendPanelErr("Group not defined");

	return _grpstat;
}

void Client::DoAbort(char *reason)
{
  fprintf(stderr, "%s\n", reason);
  (void)NetworkClose(_serverFd);
  exit(1);
}

// the following four functions are blocking and require one-time handshake
int Client::CreateGroup(char *groupname, char* passwd, const char*& errmsg)
{
	int retval;
	errmsg = NULL;

	if (_groupname!= NULL)
	{
		errmsg = errBase[DOUBLE_GROUP];
		retval = -1;
	}
	else
	{
		// generate a compressed key
		if (csk != NULL)
		{
			delete csk;
		}
		csk = new CSgroupKey(groupname, passwd);

		retval = SendServerCmd(2,csk->toStr(), CS_Creat_Req);
		if (retval <0)
		{
			errmsg = errBase[CREAT_FAIL];
			retval = -1;
		}
		else
		{
			_groupname = strdup(groupname);
			_grpstat = ACTIVE;
		}
	}
	return retval;
}

int Client::JoinGroup(char *groupname,char* passwd, const char*& errmsg)
{
	int retval;
	errmsg = NULL;
	if (_groupname!= NULL)
	{
		errmsg = errBase[DOUBLE_GROUP];
		retval = -1;
	}
	else
	{
		// generate a compressed key
		if (csk != NULL)
		{
			delete csk;
		}
		csk = new CSgroupKey(groupname, passwd);
		retval = SendServerCmd(2,csk->toStr(), CS_Join_Req);
		if (retval <0)
		{
			errmsg = errBase[JOIN_FAIL];
			retval = -1;
		}
		else
		{
			_groupname = strdup(groupname);
 			_grpstat |= PASSIVE;
		}
	}
	return retval;
}
//
// to lead a group, you must already be a passive member of some group
int Client::QueryGroup(char*& groups)
{
	int retval;

	retval = SendServerCmd(1, CS_Query_Req);
	groups = _result;
	return retval;
}
int Client::LeadGroup(const char *& errmsg)
{
	if (_groupname == NULL)
	{
		errmsg = errBase[NONE_GROUP];
		return -1;
	}

	// once in a group, you must either be ACTIVE or be PASSIVE
	DOASSERT(((_grpstat&ACTIVE)||(_grpstat&PASSIVE)),
		"The client is neither active nor inactive");

	if (_grpstat&ACTIVE)
	{
		errmsg = errBase[TOKEN_ALREADY];
		return -1;
	}

	int retval;
	DOASSERT((csk != NULL),"Before issuing leadership request, the group is not defined");
	retval = SendServerCmd(2,csk->toStr(), CS_Lead_Req);

	if (retval <0)
	{
		errmsg = errBase[LEAD_FAIL];
		retval = -1;
	}
	/*
	else
	{
		_grpstat &= ~PASSIVE;
		_grpstat |= ACTIVE;
	}
	*/
	return retval;
}
// to give up a token, you must already have the token and 
// somebody has requested for it. We enforce a lazy policy
int Client::ListenGroup(const char *&errmsg)
{
	int retval = -1;
	if (_groupname == NULL)
	{
		errmsg = errBase[NONE_GROUP];
		return -1;
	}
	if ((_grpstat & ACTIVE)==0)
	{
		errmsg = errBase[NO_TOKEN];
	}
	else
	{
		DOASSERT((csk!= NULL),"Group not defined while giving up token");
		retval = SendServerCmd (2,csk->toStr(), CS_Relinquish_Not);
		if (retval <0)
		{
			errmsg = errBase[LISTEN_FAIL];
		}
		else
		{
			_grpstat &= ~ACTIVE;
			_grpstat &= ~REQUESTED;
			_grpstat |= PASSIVE;
		}
	}
	return retval;
}

//
// blocking call
int Client::LeaveGroup(const char *& errmsg)
{
	int retval = -1;
	if (_groupname == NULL)
	{
		errmsg = errBase[NONE_GROUP];
		return -1;
	}
	DOASSERT((csk!= NULL),"Group not defined while leaving a group");
	retval = SendServerCmd(2,csk->toStr(), CS_Leave_Req);
	if (retval <0)
	{
		errmsg = errBase[LEAVE_FAIL];
	}
	else
	{
		_grpstat = EMPTY;
		free( _groupname);
		_groupname = NULL;
	}
	return retval;
}

int 
Client::SendServerCmd(int args, ...)
{
	va_list	pvar;
	int		i;
	char	**argv;

	argv = new (char*)[args];
	va_start(pvar, args);
	for (i=0; i<args; ++i)
	{
		argv[i] = va_arg(pvar, char*);
	}
	va_end(pvar);

	if (NetworkSend(_serverFd, API_GRP, 0, args, argv) < 0) {
		fprintf(stderr, "Server has terminated. Client exits.\n");
		exit(1);
	}
	delete []argv;

	u_short flag;
	int rargc;
	char **rargv;

#if defined(USE_START_PROTOCOL)
	// retrieve the slotno from the server
	if (!strcmp(argv[1], CS_Init_Req))
	{
		readInteger(_serverFd, _serverSlot);
	}
#endif // USE_START_PROTOCOL

	// the client will potentially hangs, if he does not receive 
	// any acknowledgement information from the server
	do {
		if (NetworkReceive(_serverFd, 1, flag, rargc, rargv) <= 0) {
			fprintf(stderr, "Server has terminated. Client exits.\n");
			exit(1);
		}
		if (flag == API_CTL)
			ControlCmd(rargc, rargv);
	} while ((flag != API_ACK)&&(flag != API_NAK));

	delete _result ;
	_result = NULL;
	if (rargc>0)
	{
		_result = NetworkPaste(rargc, &rargv[0]);
#ifdef DEBUG
	printf("Received reply: \"%s\"\n", _result);
#endif
	}
	if (flag == API_ACK) {
		return 1;
	} else {
		return -1;
	}
}

int 
Client::ServerCmd(int argc, char **argv)
{
	// filtering commands realted to group control
	int		retval = 0;
	if (!strcmp(argv[0],"GroupCmd"))
	{
		char	*errmsg = NULL;

		if (!strcmp(argv[1],"QueryGroup"))
		{
			char* groups;
			retval = QueryGroup(groups);
			if (retval >0)
			{
				TokenList tk(groups);

				char *argv[2];
				argv[0] = "SetGroupList";
				tk.toTclList(argv[1]);
				ControlCmd(2, argv);
				delete argv[1];
			}
			else
				SendPanelErr("Collaborator is down");
		}
		else if (!strcmp(argv[1],"CreateGroup"))
		{
			retval = CreateGroup(argv[2],argv[3], errmsg); 
			char	info[128];

			sprintf(info, "Group:%s created successfully", argv[2]);
			if (retval <0)
			{
				SendPanelErr(errmsg);
			}
			else
			{
				SetPanelMajorCmd("StatusLine");
				SetPanelSubCmd("normal");
				SetPanelInfo(info);
				SendPanelCmd();
			}
		}
		else if (!strcmp(argv[1],"JoinGroup"))
		{
			retval = JoinGroup(argv[2],argv[3], errmsg); 
			char	info[128];

			sprintf(info, "Joined group:%s", argv[2]);
			if (retval <0)
			{
				SendPanelErr(errmsg);
			}
			else
			{
				SetPanelInfo(info);
				SetPanelMajorCmd("StatusLine");
				SetPanelSubCmd("normal");
				SendPanelCmd();
			}
		}
		else if (!strcmp(argv[1],"LeadGroup"))
		{
			char	info[128];	

			if (_groupname != NULL)
			{
				retval = LeadGroup(errmsg); 
				sprintf(info, "Requested leadership for group:%s", _groupname);
				if (retval <0)
				{
					SendPanelErr(errmsg);
				}
				else
				{
					SetPanelInfo(info);
					SetPanelMajorCmd("StatusLine");
					SetPanelSubCmd("normal");
					SendPanelCmd();
				}
			}
			else
			{
				SendPanelErr(errBase[NONE_GROUP]);
			}
		}
		else if (!strcmp(argv[1],"ListenGroup"))
		{
			char	info[128];	

			if (_groupname != NULL)
			{
				retval = ListenGroup(errmsg); 
				sprintf(info, "Gave up leadership for group:%s", _groupname);
				if (retval <0)
				{
					SendPanelErr(errmsg);
				}
				else
				{
					SetPanelInfo(info);
					SetPanelMajorCmd("StatusLine");
					SetPanelSubCmd("normal");
					SendPanelCmd();
				}
			}
			else
				SendPanelErr(errBase[NONE_GROUP]);
		}
		else if (!strcmp(argv[1],"LeaveGroup")||
				 !strcmp(argv[1],"ExitGroupMode"))
		{

			bool	exitmode = false;
			if (!strcmp(argv[1],"ExitGroupMode"))
			{
				exitmode = true;
			}

			retval = LeaveGroup(errmsg); 
			if (retval <0)
			{
				SendPanelErr(errmsg);
			}
			else
			if (exitmode)
				return retval;
			else
			{
				SetPanelInfo("Running in standalone mode");
				SetPanelMajorCmd("StatusLine");
				SetPanelSubCmd("normal");
				SendPanelCmd();
			}
		}
	}
	else
	{
		// other DEVise commands
		char 	**nargv = new (char*)[argc+1];
		int		i;

		for (i=0; i< argc; ++i)
			nargv[i+1] = argv[i];
	
		if (csk == NULL)
		{
			// new a null key
			csk = new CSgroupKey();
		}
		nargv[0] = csk->toStr();		
	
		if (NetworkSend(_serverFd, API_CMD, 0, argc+1, nargv) < 0) {
			fprintf(stderr, "Server has terminated. Client exits.\n");
			exit(1);
		}
		delete nargv;
	
		u_short flag;
		int rargc;
		char **rargv;
		do{
			if (NetworkReceive(_serverFd, 1, flag, rargc, rargv) <= 0) {
				fprintf(stderr, "Server has terminated. Client exits.\n");
				return -1;
			}
			if (flag == API_CTL)
				ControlCmd(rargc, rargv);
		}while (flag == API_CTL);
	
		delete _result;
		_result = NetworkPaste(rargc, rargv);
		DOASSERT(_result, "Out of memory");
	
	#ifdef DEBUG
		printf("Received reply: \"%s\"\n", _result);
	#endif
	
		if (flag == API_ACK)
			retval = 1;
		else
			retval = -1;
	}
	return retval;
}

void Client::ReadServer()
{
	u_short flag;
	int argc;
	char **argv;
	if (NetworkReceive(_serverFd, 1, flag, argc, argv) <= 0)
	{
		exit(-1);
	}

	if (flag == API_CTL) 
	{
		ControlCmd(argc, argv);
	}
	else if (flag == API_GRP)
	{
		if (!strcmp(argv[0], SC_Relinquish_Req))
		{
			this->RequestRelinquish();
		}
		else if (!strcmp(argv[0], SC_Grabbed_Not))
		{
			this->NotifyGrabbed();
		}
		else
			fprintf(stderr,"Unkown GRP command from server:%s\n", argv[1]);
	}
	else
	{
		// general server to client commands were received
		this->GeneralServerCmd(flag, argc, argv);
	}
}
void Client::ControlCmd(int argc, char **argv)
{
  delete _cmd;
  _cmd = NetworkPaste(argc, argv);
  DOASSERT(_cmd, "Out of memory");
#if defined(DEBUG)
  printf("Received command from server: \"%s\"\n", _cmd);
#endif
}

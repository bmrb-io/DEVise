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
  Implementation of Server class.
 */

/*
  $Id$

  $Log$
  Revision 1.20  1998/08/21 22:16:05  wenger
  Got DEVise 1.5.4 to compile on SPARC/SunOS (sundance) -- to make statically-
  linked DEVise for distribution.

  Revision 1.19  1998/08/17 21:03:38  wenger
  Changed deviseb and devisec to work properly with current devised
  (clients don't expect to be sent slot number at startup).

  Revision 1.18  1998/08/13 18:14:34  wenger
  More updates to JavaScreen support.

  Revision 1.17  1998/08/11 18:06:29  wenger
  Switched over to simplified JavaScreen startup protocol; added isDir
  and priority args to session list.

  Revision 1.16  1998/08/11 13:42:03  wenger
  Implemented new JavaScreen startup protocol (currently runs old one until
  Hongyu commits new JavaScreen code).

  Revision 1.15  1998/07/29 14:19:41  wenger
  Mods to compile DEVise on Alpha/OSF again (partially successful); mods to
  allow static linking on Linux.

  Revision 1.14  1998/06/23 17:51:29  wenger
  Added client timeout to Devise -- quits if no commands from client
  within specified period.

  Revision 1.13  1998/06/11 20:45:35  wenger
  Added -quit command line argument for devised to quit when client disconnects.

  Revision 1.12  1998/05/14 18:20:50  wenger
  New protocol for JavaScreen opening sessions works (sending "real" GIF)
  except for the problem of spaces in view and window names.

  Revision 1.11  1998/05/13 13:36:45  wenger
  Fixed some dynamic memory errors in the csgroup code; cleaned up
  DeviseCommand class somewhat -- simplified the calling of Run()
  functions in subclasses.

  Revision 1.10  1998/05/02 08:38:58  taodb
  Added command logging and playing support
  Added communication support for JAVA Screen

  Revision 1.9  1998/03/30 22:32:51  wenger
  Merged fixes from collab_debug_br through collab_debug_br2 (not all
  changes from branch were merged -- some were for debug only)
  (committed stuff includes conditionaled-out debug code).

  Revision 1.8.2.2  1998/03/25 23:04:56  wenger
  Removed all stuff setting internet address to INADDR_ANY (not needed).

  Revision 1.8.2.1  1998/03/25 15:56:43  wenger
  Committing debug version of collaboration code.

  Revision 1.8  1998/03/12 02:09:06  wenger
  Fixed dynamic memory errors in collaboration code that caused core dump
  on Linux; collaboration code now tolerates interruption of accept() and
  select() in some cases; fixed excessive CPU usage by collaborator
  (select timeout now non-zero); fixed some other collaboration bugs.

  Revision 1.7  1998/03/11 18:25:08  wenger
  Got DEVise 1.5.2 to compile and link on Linux; includes drastically
  reducing include dependencies between csgroup code and the rest of
  the code, and within the csgroup code.  (Note: running collaboration
  doesn't work yet.)

  Revision 1.6  1998/03/05 17:10:30  taodb
  Temporarily disabled multiple-client connect, and enabled mutiple pair of
  C/Ss on the same machine

  Revision 1.5  1998/03/04 05:54:27  taodb
  Removed bugs that prevent multiple connection

  Revision 1.4  1998/02/26 20:35:10  taodb
  Removed ParaseAPI() interface, and added CommandObject interface

  Revision 1.3  1998/02/26 18:54:08  wenger
  Got everything to compile on haha -- still have a link problem, though.

  Revision 1.2  1998/02/12 17:14:40  wenger
  Merged through collab_br_2; updated version number to 1.5.1.

  Revision 1.1.2.3  1998/02/12 05:02:22  taodb
  Made code proceed even if DEVISE_COLLABORATOR environmental variable is not defined

  Revision 1.1.2.2  1998/01/30 08:52:45  taodb
  Reduced run-time debug messages, fixed memory leaks caused by message headers
  and also fixed a bug in master-slave handshaking

  Revision 1.1.2.1  1998/01/28 22:42:21  taodb
  Added support for group communicatoin

 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <math.h>
#include <signal.h>
#if defined(SGI)
#include <stdarg.h>
#endif
#if defined(SGI) || defined(OSF) || defined(SUN)
#include <sys/param.h> // for MAXHOSTNAMELEN
#endif
#if defined(OSF)
#   include <rpcsvc/showfh.h> // for MAXNAMELEN
#endif
#include "devise_varargs.h"
#if defined(LINUX) || defined(SUN)
#   define MAXNAMELEN 1024
#endif

#if defined(LINUX) || defined(SUN)
#   include <sys/time.h>
#endif
#if defined(SUN)
#  include "machdep.h"
#endif

#include "JavaScreenCmd.h"
#include "ClientAPI.h"
#include "Server.h"
#include "Control.h"
#include "Dispatcher.h"
#include "callBks.h"
#include "rcvMsg.h"
#include "CommandObj.h"
#include "CmdDescriptor.h"
#include "serverInterface.h"
#include "keys.h"
#include "Csprotocols.h"
#include "Util.h"
#include "Init.h"

#if !defined(USE_START_PROTOCOL)
int _clientSlot = CLIENT_INVALID;
#endif

Server* _ThisServer;

int RequestRelinquish(char *groupname, ConnectInfo *cinfo)
{
	return _ThisServer->_RequestRelinquish(groupname, cinfo);
}
int NotifyGrabbed(char *groupname, ConnectInfo *cinfo)
{
	return _ThisServer->_NotifyGrabbed(groupname, cinfo);
}
int RequestCheckpoint(char *fname, ConnectInfo *cinfo)
{
	return _ThisServer->_RequestCheckpoint(fname, cinfo);
}
int ExecCheckpoint(char *fname, ConnectInfo *cinfo)
{
	return _ThisServer->_ExecCheckpoint(fname, cinfo);
}

//#define DEBUG

char *NoError = "NONE";

#define DO_ASSERT(c,r) {if (!(c)) DoAbort(r); }
Server::Server(char *name,int image_port, 
	int swt_port, int clnt_port, char* swtname,
	int maxClients)
{
    char hostname[MAXNAMELEN];
	struct hostent *hst;

	signal(SIGPIPE, SIG_IGN);

	// initialize object variables
	serverstate = STANDALONE;
	_ThisServer = this;

	_name = strdup(name);
	DO_ASSERT(_name, "Out of memory");
								 
	// record the last client who try to create/join a group
	collabCid = CLIENT_INVALID;

	switchname = strdup(swtname);
									   
	bzero((char *) &switchaddr, sizeof(switchaddr));
	gethostname(hostname, MAXNAMELEN);

    hst = gethostbyname(hostname);

#if defined(DEBUG)
    printf("h_name = %s\n", hst->h_name);
    printf("h_addrtype = %s\n", hst->h_addrtype == AF_INET ? "AF_INET" :
      "other");
    printf("h_length = %d\n", hst->h_length);

    char **p;
    for (p = hst->h_addr_list; *p != 0; p++) {
        struct in_addr in;
        char **q;

        (void) memcpy(&in.s_addr, *p, sizeof (in.s_addr));
        (void) printf("%s\t%s", inet_ntoa(in), hst->h_name);
        for (q = hst->h_aliases; *q != 0; q++)
            (void) printf(" %s", *q);
        (void) putchar('\n');
    }
#endif
	 
	switchaddr.sin_family = AF_INET;
	switchaddr.sin_addr   = *(struct in_addr *) (hst->h_addr_list[0]);
	switchaddr.sin_port = htons(swt_port);
				  
	_port = clnt_port;                   // port for switch, _port for clients
	_imageport = image_port;             // port for switch, _port for clients
								   
	CBakInstall((int)ceil(log(CTRL_RELINQUISH)/log(2)),
		(CallBackHandler)&RequestRelinquish);
	CBakInstall((int)ceil(log(CTRL_GRABBED)/log(2)),
		(CallBackHandler)&NotifyGrabbed);
	CBakInstall((int)ceil(log(CTRL_EXECCKPT)/log(2)),
		(CallBackHandler)&ExecCheckpoint);
	CBakInstall((int)ceil(log(CTRL_CKPTSERVER)/log(2)),
		(CallBackHandler)&RequestCheckpoint);

	// register the listening fd for the switch
	_maxClients = maxClients;
    _clients = new ClientInfo[_maxClients];
	_listenFd = -1;
	_listenImageFd = -1;
	_numClients = 0;
    _cmd = NULL;
	_listenSwtFd = RPCInit(switchname, switchaddr);
	if (_listenSwtFd >0) {
		(void)Dispatcher::Current()->Register(this, 10, AllState, 
		true, _listenSwtFd);
	}
	if (_listenSwtFd >0) {
	    _channel = new ControlChannel();
    }

	// initialize the command object
	cmdObj = new CommandObj(this);
}


int 
Server::_RequestRelinquish(char *groupname, ConnectInfo *cinfo)
{
	int	fd;
	fd = GetClientFd(groupname);
    DO_ASSERT(fd>0, "Invalid fd in request relinquish ");
	return SendClientCmd(fd, API_GRP, 1, SC_Relinquish_Req);
}
int 
Server::_NotifyGrabbed(char *groupname, ConnectInfo *cinfo)
{
	int	fd;
	int i;
	for (i=0; i<_maxClients; ++i)
	{
		if (_clients[i].gname != NULL)
		{
			if (!strcmp(_clients[i].gname->grpName, groupname))
				_clients[i].active = true;
			break;
		}
	}

	fd = GetClientFd(groupname);
    DO_ASSERT(fd>0, "Invalid fd in request relinquish ");
	return SendClientCmd(fd, API_GRP, 1, SC_Grabbed_Not );
}
int 
Server::_RequestCheckpoint(char *fname, ConnectInfo *cinfo)
{
	FILE	*fp;
	fp = fopen(fname,"w");
	if (fp ==NULL)
	{
		fprintf(stderr, "*** the checkpoint file does not exist\n");
	}
	else
	{
		fprintf(fp,"HAHA\n");
		fclose(fp);
	}
	return 0;
}
int 
Server::_ExecCheckpoint(char *fname, ConnectInfo *cinfo)
{
	FILE	*fp;
	fp = fopen(fname,"r");
	if (fp==NULL)
	{
		fprintf(stderr, "*** the checkpoint file does not exist\n");
	}
	else
	{
		char	buf[128];
		fscanf(fp,"%s",buf);
		fprintf(stderr,"First line of the checkpoint file:\n%s\n", buf);
		fclose(fp);
	}
	return 0;
}
int
Server::GetClientFd(char* grpname)
{
	int	i;
	int	retfd = -1;
	for (i=0;(retfd == -1)&&(i< _maxClients); ++i )
	{
		if (_clients[i].valid)
			retfd = _clients[i].GetFd(grpname);
	}
	return retfd;
}
Server::~Server()
{
    for (int i = 0; i < _maxClients; i++)
    {
		CloseClient(i);
    }
    delete [] _clients;
	_clients = NULL;
    free( _name);
	_name = NULL;
    delete _cmd;
	_cmd = NULL;
}

void Server::DoAbort(char *reason)
{
    fprintf(stderr, "%s\n", reason);
    char *args[] = { "AbortProgram", reason };
    Server::SendControl(API_CTL, 2, args, true);
    fprintf(stderr, "Server aborts.\n");
    exit(0);
}

void Server::MainLoop()
{
    while (1) {
        WaitForConnection();
        while (_numClients > 0)
        {
	    	SingleStep();
        }
    }
}

void Server::SingleStep()
{
    ReadCmd();
}

void Server::InitializeListenFd(int port, int& listenFd)
{
    DO_ASSERT(listenFd < 0, "Invalid socket");
    listenFd = socket(AF_INET, SOCK_STREAM, 0);
    if (listenFd < 0)
    {
		perror("socket() failed");
    }
    DO_ASSERT(listenFd >= 0, "Cannot create socket");
    
    // allow listening port number to be reused
    int enable = 1;
    int result = setsockopt(listenFd, SOL_SOCKET, SO_REUSEADDR,
			    (char *)&enable, sizeof enable);
    if (result < 0)
    {
		perror("setsockopt() failed");
    }
    DO_ASSERT(result >= 0, "Cannot set socket options");
    
    // Now bind these to the address..
    struct sockaddr_in servAddr;
    memset(&servAddr, 0, sizeof servAddr);
    servAddr.sin_family = AF_INET;
    servAddr.sin_port = htons(port);

#if defined(DEBUG)
    printf("Server listening at port %u....\n", port);
#endif
    servAddr.sin_addr.s_addr = htonl(INADDR_ANY);
    
    result = bind(listenFd, (struct sockaddr *)&servAddr,
		  sizeof(struct sockaddr));
    if (result < 0)
    {
		perror("bind() failed");
    }
    DO_ASSERT(result >= 0, "Cannot bind to address");
    
    result = listen(listenFd, 5);
    if (result < 0)
    {
		perror("listen() failed");
    }
    DO_ASSERT(result >= 0, "Cannot listen");
    
    printf("Listening fd = %d has been initialized\n", listenFd);
}

int Server::FindIdleClientSlot()
{
    for (int i = 0; i < _maxClients; i++)
    {
		if (!_clients[i].valid)
		{
	    	return i;
		}
    }
    return -1;
}

void Server::WaitForConnection()
{
    int clientfd;
    if (_listenFd < 0)
    {
		InitializeListenFd(_port, _listenFd);
    }
	if (_listenImageFd <0)
	{
		InitializeListenFd(_imageport, _listenImageFd);
	}

    printf("%s server waiting for client connection on port %d\n",
      _name, _port);

    if (Init::ClientTimeout() > 0) {
		fd_set fdread;
		FD_ZERO(&fdread);
		FD_SET(_listenFd, &fdread);

		struct timeval timeout;
		timeout.tv_sec = Init::ClientTimeout() * 60;
		timeout.tv_usec = 0;

		int fdReadyCount = select(_listenFd + 1, &fdread, NULL, NULL, &timeout);
		if (fdReadyCount < 0) {
			perror("select failed: ");
			return;
		} else if (fdReadyCount == 0) {
		    printf("Last client command more than %d minutes ago; "
			  "server exiting\n", Init::ClientTimeout());
			Exit::DoExit(0);
		}
	}

    struct sockaddr_in tempaddr;
    int len = sizeof(tempaddr);
    clientfd = accept(_listenFd, (struct sockaddr *)&tempaddr, &len);
    if (clientfd < 0)
    {
        fprintf(stderr, "Warning: ");
		perror("accept() failed");
		return;
    }
    int slot = FindIdleClientSlot();
	if (slot >=0)
	{
    	_clients[slot].valid = true;
    	_clients[slot].fd = clientfd;
    	_numClients++;
	}

#if defined(USE_START_PROTOCOL)
	// echo back the "slot" number as a handle for the client
	// a JAVA client will later use this handle to identify a connection
	int nbytes = writeInteger(clientfd, slot);
#else
	_clientSlot = slot;
#endif
    if (slot < 0)
    {
		fprintf(stderr, "WARNING: Too many clients. Connection denied\n");
		close(clientfd);
    }
#if defined(USE_START_PROTOCOL)
	if (nbytes <0)
	{
		printf("Failed to send back slot number to the client\n");
		if (slot >=0)
			CloseClient(slot);
	}
#endif
#if defined(USE_START_PROTOCOL)
	if ((nbytes>0)&&(slot >=0))
#else
	if (slot >= 0)
#endif
	{
    	printf("Connection established to client %d\n", slot);
    	BeginConnection(slot);
	}
}

int
Server::readInteger(int fd, int&num)
{
	char buf[SLOTNUMSIZE+1];
	int	retval;

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

int
Server::writeInteger(int fd, int num)
{
	char buf[SLOTNUMSIZE+1];
	char format[128];
	int	retval;

	sprintf(format, "%%-%dd",SLOTNUMSIZE);

	sprintf(buf,format,num);
	int nbytes = write(fd, buf, SLOTNUMSIZE);
	if (nbytes == SLOTNUMSIZE)
		retval = 1;
	else
		retval = -1;

	return retval;
}

void
Server::CloseImageConnection(ClientID cid)
{
	if (_clients[cid].imagefd >0)
	{
		close(_clients[cid].imagefd);
		_clients[cid].imagefd = -1;
	}
	else
	{
		fprintf(stderr, "Invalid image socket for client:%d\n", cid);
	}
}

void 
Server::WaitForImageportConnection()
{
#if defined(DEBUG)
    printf("Server::WaitForImageportConnection()\n");
#endif

	int		imagefd;
    struct 	sockaddr_in tempaddr;
    int 	len = sizeof(tempaddr);
	int	 	slotno = -1;

    imagefd = accept(_listenImageFd, (struct sockaddr *)&tempaddr, &len);
    if (imagefd < 0)
    {
        fprintf(stderr, "Warning: ");
		perror("accept() failed");
		return;
    }
	/*
	if (NetworkNonBlockMode(imagefd) <0)
	{
		fprintf(stderr, "Fail to set non-blocking mode for image fd\n");
		return;
	}
	*/

#if defined(USE_START_PROTOCOL)
	//
	// this blocking read should be moved to scheduler later
	// because we cannot have the server blocking for a malicious client
	char buf[SLOTNUMSIZE +1];

	if (readInteger(imagefd, slotno) <0)
	{
		fprintf(stderr, "Invalid image connection, slot no expected\n");
		writeInteger(imagefd, -1);
		close(imagefd);
	}
	else
	{
#else
        slotno = _clientSlot;
		_clientSlot = CLIENT_INVALID;
#endif // USE_START_PROTOCOL
		if ((slotno <0)||(slotno >= _maxClients))
		{
			fprintf(stderr, "Invalid slot no\n");
			writeInteger(imagefd, -1);
			close(imagefd);
		}
		else
		if ((!_clients[slotno].valid)||(!_clients[slotno].fd <0))
		{
			fprintf(stderr, "Client for slot:%d is down\n", slotno);
			writeInteger(imagefd, -1);
			CloseClient(slotno);
		}
		else
		{
			// setup the imagefd
			_clients[slotno].imagefd = imagefd;
			char*	retval=
				JavaScreenCmd::JavaScreenCmdName(JavaScreenCmd::DONE);

#if defined(USE_START_PROTOCOL)
			// send confirmation to client's image port
			writeInteger(imagefd, slotno);

			//send confirmation to JAVA Client
			char buf[128];
			char* argv[1]={buf};
			sprintf(buf, "%s", retval);
			ReturnVal(slotno, API_JAVACMD, 1, &argv[0], false);
#endif // USE_START_PROTOCOL
		}
#if defined(USE_START_PROTOCOL)
	}
#endif // USE_START_PROTOCOL
}

void Server::ProcessCmd(ClientID clientID, int argc, char **argv)
{
}
void Server::CloseClient(ClientID clientID)
{
    if (!_clients[clientID].valid)
    {
		return;
    }
    if (_clients[clientID].fd >= 0)
    {
		EndConnection(clientID);
		printf("Closing client connection.\n");
		(void)NetworkClose(_clients[clientID].fd);
    }
	if (_clients[clientID].imagefd >=0)
	{
		close(_clients[clientID].imagefd);
	}
	_clients[clientID].imagefd = -1;
    _clients[clientID].fd = -1;
    _clients[clientID].valid = false;
	delete [] _clients[clientID].cname;
	_clients[clientID].cname = NULL;

	// clean the group associated with this client, if any
	CloseClientGroup(clientID);
    _numClients--;

    if (Init::QuitOnDisconnect()) {
		Exit::DoExit();
	}
}

int Server::CloseClientGroup(ClientID clientID)
{
	int	retval = -1;

	if (_clients[clientID].dispID != NULL)
	{
		(void)Dispatcher::Current()->Unregister(_clients[clientID].dispID);
		_clients[clientID].dispID = NULL;
	}

	if (_clients[clientID].gname != NULL)
	{
		retval = DelGroupFd(_clients[clientID].gname);
		delete _clients[clientID].gname;
		_clients[clientID].gname = NULL;
		_clients[clientID].active = false;
	}
	return retval;
}


//
// Reading client commands when MULTIPLE clients are allowed
//
void Server::ReadCmd()
{
    fd_set fdset;
    int maxFdCheck;
    int numFds;
    bool error = false;

    // Initialize the fd set
    memset(&fdset, 0, sizeof fdset);
	maxFdCheck = 0;

	if (_listenFd >0) {
    	FD_SET(_listenFd, &fdset);
		if (_listenFd > maxFdCheck) maxFdCheck = _listenFd;
	}
	if (_listenImageFd >0) {
    	FD_SET(_listenImageFd, &fdset);
		if (_listenImageFd > maxFdCheck) maxFdCheck = _listenImageFd;
	}

	if (_listenSwtFd >0) {
    	FD_SET(_listenSwtFd, &fdset);
		if (_listenSwtFd > maxFdCheck) maxFdCheck = _listenSwtFd;
	}

	// add the fds of all groups
	int	maxfd = SetGroupFdset(&fdset);
	if (maxfd > maxFdCheck) maxFdCheck = maxfd;

	// add the fds of all local clients
    for (int i = 0; i < _maxClients; i++)
    {
		if (_clients[i].valid)
		{
	    	FD_SET(_clients[i].fd, &fdset);
	    	if (_clients[i].fd > maxFdCheck)
	    	{
				maxFdCheck = _clients[i].fd;
	    	}
		}
    }

    // select()
    numFds = select(maxFdCheck + 1, &fdset, 0, 0, 0);
    if (numFds < 0)
    {
		char errBuf[MAXPATHLEN + 256];
		sprintf(errBuf, "select() failed at %s: %d", __FILE__, __LINE__);
		perror(errBuf);
		return;
    }

    //
    // Handle a new connection request
    if (FD_ISSET(_listenFd, &fdset))
    {
		WaitForConnection();
    }
    if (FD_ISSET(_listenImageFd, &fdset))
    {
		WaitForImageportConnection();
    }
	else
	if ((_listenSwtFd >0)&&FD_ISSET(_listenSwtFd, &fdset))
	{
		//switch-port available	, re-connect request from switch
		fprintf(stderr, "Re-connect request received from the switch\n");
		ReConnect();
	}
	if (ExecSwitchCmds(&fdset) == false)
	{
    	ExecClientCmds(&fdset);
	}
}
bool Server::ExecSwitchCmds(fd_set *fdsetp)
{
	int	 readyfd;

	readyfd = GetGroupReadyFd(fdsetp);
	if (readyfd >0)
	{
		char recBuffer[1000];
		int argc;
		char **argv;
		int tsize;
		int retval;

		if ((retval=RecvMsg(readyfd, recBuffer, 1000))>0)
		{
			ServerServerProt	*prot;
			int	controlvalue;

			prot = new ServerServerProt(retval, recBuffer);
			prot->getArguments(argc, argv);

#if defined(DEBUG)
			fprintf(stderr, "Message from the collaborator:\n"
							"From server = %s\n"
							"and client  = %s\n",
							prot->getServerName(), prot->getClientName());	
#endif
			controlvalue = prot->getControl();

			// Route this command to the command container object
			if (controlvalue == ServerServerProt::SSC_CMD)
			{
				// a command from the switch for the clients
				//Server::SendControl(API_CTL, argc, argv, true);
				CmdSource cmdSrc(CmdSource::NETWORK, collabCid);
				CmdDescriptor cmdDes(cmdSrc, CmdDescriptor::FOR_CLIENT);
				RunCmd(argc, argv, cmdDes);
			}
			else if (controlvalue == ServerServerProt::CSS_CMD)
			{
				// a command from the switch for the server
				//ProcessCmd(CLIENT_INVALID, argc, argv);
				CmdSource cmdSrc(CmdSource::NETWORK, collabCid); 
				CmdDescriptor cmdDes(cmdSrc, CmdDescriptor::FOR_SERVER);
				RunCmd(argc, argv, cmdDes);
			}
			else
				DO_ASSERT(0, "control value invalid");
			delete prot;
		}
		else if (retval <0)
		{
			// error conditions, may be the collaborator is down
			// or the message format is not right
		}
	}
	return (readyfd >0);
}

int Server::SendClientCmd(int fd,int flag, int args, ...)
{
	va_list	pvar;
    int		retval;
    char	**argv;
	int		i;

	DO_ASSERT((args>=1), "Insufficient parameters to SendClientCmd");
	argv = new (char*)[args];
	va_start(pvar, args);
	for (i=0; i< args; ++i)
	{
		argv[i] = va_arg(pvar, char*);
	}
	va_end(pvar);

    retval= NetworkSend(fd, flag, 0, args, argv);
	delete []argv;
    return retval;
}
void Server::ProcessGroupControl(ClientID cid, int argc, char** argv)
{
	bool 	success = false;
	char	*errmsg = "NULL";
	int		mode = 0;
	GroupKey *gkp = NULL;
	CSgroupKey *key = NULL;
	char	*grouplist = NULL;	

	if (!strcmp(argv[1], CS_Init_Req))
	{
		// initialize the server according to the client
		success = ExecInitServer(cid, argv[2], errmsg);
		if (_clients[cid].cname != NULL)
		{
			free( _clients[cid].cname);
		}
		_clients[cid].cname = strdup(argv[2]);
	}
	else if (!strcmp(argv[0], CS_Query_Req))
	{
		if (_channel->QueryGroupName(grouplist) >0)
			success = true;
		else
		{
			success = false;
			errmsg = "Fail to obtain registered groups";
		}
	}
	else
	{
		key = new CSgroupKey(argv[0]);
		gkp = key->toGroupKey();
	
		if (!strcmp(argv[1], CS_Creat_Req)||
			!strcmp(argv[1], CS_Join_Req))
		{
			collabCid = cid;
			if (!strcmp(argv[1], CS_Creat_Req))
			{
				mode = (CRM_LEADER|CRM_CREATE);
			}
			else
				mode = CRM_FOLLOWER;
	
			int groupfd;
			if ((groupfd = Register(gkp, switchaddr, mode)) <0)
			{
				if (errcode == ER_RPCTIMEDOUT)
				{
					CloseClientGroup(cid);
					errmsg = "The collaborator is down!";
				}
				else
				{
					fprintf(stderr, "Register failure, errcode =%d\n",errcode);
					errmsg = "Unkown errors!";
				}
				success = false;
			}
			else
			{
				if (groupfd !=0)
				{
					// install fd attached to each group
					_clients[cid].dispID =
						Dispatcher::Current()->Register(this, 10, AllState, 
						true, groupfd);
				}
				if (mode == CRM_FOLLOWER)
				{
					// here we are going to do the sync operation
					success = AfterPassiveJoin(gkp,errmsg);
					_clients[cid].active = false;
				}
				else
				{
					success = true;
					_clients[cid].active = true;
				}
				if (success)
				{
					// record its groupkey
					if (_clients[cid].gname!= NULL)
						delete _clients[cid].gname;
						_clients[cid].gname = NULL;
					_clients[cid].gname = new GroupKey;
					memcpy((char*)_clients[cid].gname,(char*)gkp,
						sizeof(GroupKey));
				}
			}
		}
		else if (!strcmp(argv[1], CS_Lead_Req))
		{
			success = false;
			if (_clients[cid].gname == NULL)
				errmsg = "No group registered yet!";
			else if (_clients[cid].active == true)
				errmsg = "Already the leader of your current group!";
			else
			{
				success = ( Leadership(gkp, switchaddr, CRM_GRAB) == 0);
				if (success)
				{
					//_clients[cid].active = true;
				}
				else
				{
					if (errcode == ER_RPCTIMEDOUT)
					{
						errmsg = "Collaborator is down";
					}
					else
					{
						fprintf(stderr, "CRM_GRAB failure, with errocde=%d",
							errcode);
						errmsg = "Unknown error from the collaborator";	
					}
				}
			}
		}
		else if (!strcmp(argv[1], CS_Relinquish_Not)||!strcmp(argv[1], CS_Leave_Req))
		{
			success = false;
			if (_clients[cid].gname == NULL)
				errmsg = "No group registered yet!";
			else
			if (_clients[cid].active == true)
			{
				success = (Leadership(gkp, switchaddr, CRM_RELINQUISH) ==0);
				if (success)
				{
					_clients[cid].active = false;

					// the server does not cache the token
					if (!strcmp(argv[1], CS_Leave_Req))
					{
						DeRegister(gkp, switchaddr, CRM_DELETE);
						CloseClientGroup(cid);
					}
				}
				else
				{
					if (errcode == ER_RPCTIMEDOUT)
						errmsg = "Collaborator is down";
					else
					{
						fprintf(stderr, "CRM_RELINQUISH failure,"
										" with errocde=%d", errcode);
						errmsg = "Unkown error from the collaborator";
					}
				}
			}
			else
			{
				// we do not want to hear anything from the collaborator
				if (!strcmp(argv[1], CS_Leave_Req))
				{
					DeRegister(gkp, switchaddr, CRM_DELETE);
					CloseClientGroup(cid);
					success = true;	
				}
				else
				{
					errmsg = "No token to relinquish";
					success = false;
				}
			}
		}
	}

	// send a message back to the client
	if (success)
	{
		if (grouplist != NULL)
		{
			SendClientCmd(_clients[cid].fd, API_ACK, 1, grouplist);
			delete grouplist;
		}
		else
		{
			SendClientCmd(_clients[cid].fd, API_ACK, 1, errmsg);
		}
	}
	else
	{
		SendClientCmd(_clients[cid].fd, API_NAK, 1, errmsg);
	}
}


bool Server::AfterPassiveJoin(GroupKey *gkp, char*&errmsg)
{
	bool	success;
	success =(GroupSync(gkp, switchaddr) ==0);
	if (!success)
		if (errcode == ER_RPCTIMEDOUT)
		{
			errmsg = "Collaborator is down";
		}
		else
		{
			errmsg = "GroupSync failed";
			fprintf(stderr, "GroupSync error with errcode = %d\n",
				errcode);
		}
	return success;
}

void Server::ExecClientCmds(fd_set *fdset)
{
    u_short flag;
    int result;
    int argc;
    char **argv;
    for (int i = 0; i < _maxClients; i++)
    {
		if (!_clients[i].valid || !FD_ISSET(_clients[i].fd, fdset))
		{
	    	continue;
		}
		result = NetworkReceive(_clients[i].fd, 1, flag, argc, argv);
		if (result < 0)
		{
	    	CloseClient(i);
		}
		else if (!result)
		{
#if defined(DEBUG)
	    	printf("End of client data.\n");
#endif
	    	CloseClient(i);
		}
		else if (flag == API_GRP)
		{
			// process group control messages from the client
			ProcessGroupControl(i, argc, argv);
		}
		else if (flag == API_JAVACMD)
		{
			// specify a client
			if (_clients[i].ctype == ClientInfo::NOT_DEFINED)
			{
				_clients[i].ctype = ClientInfo::JAVA_SCREEN;
			}
			setCurrentClient(i);
			CmdSource cmdSrc(CmdSource::JAVACLIENT, i);
			CmdDescriptor cmdDes(cmdSrc, CmdDescriptor::FOR_CLIENT);
			RunCmd(argc, &argv[0], cmdDes);

			bool success;
			success = cmdDes.isSuccess();
			// set server free
			setCurrentClient(CLIENT_INVALID);
#if defined(DEBUG)
	    	printf("Done executing JAVA command\n");
#endif
		}
		else if (flag == API_CMD)
		{
			// API_CMD or API_JAVACMD
			char*	errmsg = NoError;
			bool	success;
#if defined(DEBUG)
	    	printf("Executing command\n");
#endif
			if (_clients[i].ctype == ClientInfo::NOT_DEFINED)
			{
				_clients[i].ctype = ClientInfo::TCL_CLIENT;
			}
			setCurrentClient(i);
        	// route the command to the Command Container object
			CmdSource cmdSrc(CmdSource::CLIENT, i);
			CmdDescriptor cmdDes(cmdSrc, CmdDescriptor::FOR_CLIENT);
			RunCmd(argc-1, &argv[1], cmdDes);
			success = cmdDes.isSuccess();
			setCurrentClient(CLIENT_INVALID);
#if defined(DEBUG)
	    	printf("Done executing command\n");
#endif
		}
		else 
		{
	    	fprintf(stderr, "Received unexpected type of message: %u\n", flag);
	    	CloseClient(i);
		}
    }
}

bool 
Server::GroupCast(GroupKey* gname, char* cname, char* control,
	int argc, char** argv, char*&errmsg)
{
	char		*recBuffer;
	int         msgsize;
	int         mode;
	CSgroupKey  *gkp;
	bool		success = true;
	char 		serverhostname[MAXHOSTNAMELEN+1];
	ServerServerProt	*prot;

	gkp = new CSgroupKey(gname);
	gethostname(serverhostname, MAXHOSTNAMELEN);

	prot = new ServerServerProt(argc, argv);
	prot->setServerName(serverhostname);
	prot->setClientName(cname);
	prot->setControl(1, control);

	msgsize = prot->getMsg(&recBuffer);

	if (GroupSend(gkp->toGroupKey(),switchaddr,recBuffer,msgsize)<0)
	{
		if (errcode == ER_RPCTIMEDOUT)
		{
			errmsg = "Collaborator is down";
		}
		else
		{
			fprintf(stderr, "GroupSend failure," 
				" with errocde=%d", errcode);
			errmsg = "Unknown error in GroupSend";
		}
		success = false;
	}
	else
		success = true;
	
	return success;
}

void
Server::GetCmd(int argc, char **argv)
{
    delete _cmd;
    _cmd = NetworkPaste(argc, argv);
    DO_ASSERT(_cmd, "Out of memory");
    //printf("Received command from client: \"%s\"\n", _cmd);
}

int
Server::ReturnVal(ClientID clientID, u_short flag, int argc, char **argv,
    int addBraces)
{
#if defined(DEBUG)
    printf("Server::ReturnVal(");
	printf("%d, %d, ", clientID, flag);
	PrintArgs(stdout, argc, argv, false);
	printf(", %d", addBraces);
	printf(")\n");
	fflush(stdout);
#endif

	if (clientID == CLIENT_INVALID)
		return -1;

    if (flag != API_ACK && flag != API_NAK&& flag != API_JAVACMD) {
        fprintf(stderr, "Is this really a return value?\n");
    }

    int status = NetworkSend(_clients[clientID].fd, flag, addBraces, argc,
      argv);

    if (status < 0) {
		fprintf(stderr, "Client error.\n");
		CloseClient(clientID);
    }

    return status;
}

bool
Server::ServerClientCmd(u_short flag, int argc, char** argv)
{
	bool	success = true;
	int		i;
	char	*errmsg = NoError;

	// if we allows a server to server multiple groups,
	// there will be problems for the server to identify the current group

	for (i=0; i<_maxClients; ++i)
	{
		if ((_clients[i].gname != NULL)&&(_clients[i].active))
			success = GroupCast(_clients[i].gname, "SERVER",
					SS_SSC_Cmd,
					argc, argv, errmsg);
	}
	return success;
}

int
Server::SendControl(u_short flag, int argc, char **argv, int addBraces)
{
#if defined(DEBUG)
    printf("SendControl(");
    int num;
    for (num = 0; num < argc; num++) {
      printf("<%s> ", argv[num]);
    }
    printf(")\n");
#endif

    int status = 1;

    if (flag != API_CMD && flag != API_CTL) {
        fprintf(stderr, "Is this really a command?\n");
    }

    ClientID clientID;
    for (clientID = 0; clientID < _maxClients; clientID++) {
		if ((_clients[clientID].valid)&&
			(_clients[clientID].ctype == ClientInfo::TCL_CLIENT)) {

#if defined(DEBUG)
            printf("  sending control command to client %d\n", clientID);
#endif
    		int tmpStatus = NetworkSend(_clients[clientID].fd, flag, addBraces,
	        	argc, argv);
    		if (tmpStatus < 0) {
       			fprintf(stderr, "Client error.\n");
       			status = -1;
	   			CloseClient(clientID);
    		}
		}
    }

    return status;
}
int
Server::GetControlChannelFd()
{
	int	fd;
	fd = _channel->GetFd();
	return fd;
}

ControlChannel::ControlChannel()
{
	int	mode;
	_hostname = new (char)[MAXNAMELEN];
	char	buf[MAXNAMELEN];
	_key = NULL;
	pid_t	pid;

	pid = getpid();
	gethostname(_hostname, MAXNAMELEN);
	sprintf(buf,"%s%ld",_hostname,(long)pid);
	_key = new CSgroupKey(buf, SERVERTAG);
	_controlgkp = _key->toGroupKey();
	
	mode = (CRM_LEADER|CRM_CREATE);
	_controlfd = Register(_controlgkp, _ThisServer->switchaddr, mode);
}
ControlChannel::~ControlChannel()
{
	delete _key;
	_key = NULL;
	delete [] _hostname;
	_hostname = NULL;
}
int
ControlChannel::GetFd()
{
	return _controlfd;
}
int
ControlChannel::QueryGroupName(char*& groups)
{
	int	retval;
	retval =QueryColbrGroups(_controlgkp, _ThisServer->switchaddr, groups);
	return retval;
}


/*
  ========================================================================
  DEVise Data Visualization Software
  (c) Copyright 1992-2003
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
  Revision 1.11  2008/01/24 22:08:05  wenger
  Got rid of a bunch of compile warnings.

  Revision 1.10  2005/12/06 20:01:11  wenger
  Merged V1_7b0_br_4 thru V1_7b0_br_5 to trunk.  (This should
  be the end of the V1_7b0_br branch.)

  Revision 1.9.14.2  2005/09/28 17:14:21  wenger
  Fixed a bunch of possible buffer overflows (sprintfs and
  strcats) in DeviseCommand.C and Dispatcher.c; changed a bunch
  of fprintfs() to reportErr*() so the messages go into the
  debug log; various const-ifying of function arguments.

  Revision 1.9.14.1  2003/12/19 18:07:07  wenger
  Merged redhat9_br_0 thru redhat9_br_1 to V1_7b0_br.

  Revision 1.9.32.1  2003/12/17 00:17:42  wenger
  Merged gcc3_br_1 thru gcc3_br_2 to redhat9_br (just fixed conflicts,
  didn't actually get it to work).

  Revision 1.9.30.2  2003/12/16 16:07:57  wenger
  Got DEVise to compile with gcc 3.2.3 (with lots of deprecated-header
  warnings).  It runs on RedHat 7.2, but not on Solaris 2.8 (some kind
  of dynamic library problem).

  Revision 1.9.30.1  2003/04/18 16:10:25  wenger
  Got things to compile and link with gcc 3.2.2 (with lots of warnings),
  but some code is commented out; also may need fixes to be backwards-
  compatible with older gcc versions.

  Revision 1.9  2000/01/13 23:06:43  wenger
  Got DEVise to compile with new (much fussier) compiler (g++ 2.95.2).

  Revision 1.8  1999/11/24 15:44:07  wenger
  Removed (unnecessary) CommandObj class; commands are now logged for the
  monolithic form, not just the client/server form; other command-related
  cleanups; added GUI for playing back command logs.

  Revision 1.7  1998/05/02 08:38:58  taodb
  Added command logging and playing support
  Added communication support for JAVA Screen

  Revision 1.6  1998/04/01 05:21:47  taodb
  Replaced types.h with CollabTypes.h, related source files were updated.

  Revision 1.5  1998/03/30 22:32:52  wenger
  Merged fixes from collab_debug_br through collab_debug_br2 (not all
  changes from branch were merged -- some were for debug only)
  (committed stuff includes conditionaled-out debug code).

  Revision 1.4.2.1  1998/03/25 15:56:44  wenger
  Committing debug version of collaboration code.

  Revision 1.4  1998/03/11 18:25:09  wenger
  Got DEVise 1.5.2 to compile and link on Linux; includes drastically
  reducing include dependencies between csgroup code and the rest of
  the code, and within the csgroup code.  (Note: running collaboration
  doesn't work yet.)

  Revision 1.3  1998/02/26 20:35:11  taodb
  Removed ParaseAPI() interface, and added CommandObject interface

  Revision 1.2  1998/02/12 17:14:40  wenger
  Merged through collab_br_2; updated version number to 1.5.1.

  Revision 1.1.2.1  1998/01/28 22:42:22  taodb
  Added support for group communicatoin

 */

#ifndef _Server_h_
#define _Server_h_

using namespace std;

#include <iostream>

#include "ClientAPI.h"
#include "CollabTypes.h"
#include "Dispatcher.h"
#include "CmdDescriptor.h"

class CSgroupKey;

//
// Each Server can serve multiple clients. Client state is stored
// in a ClientInfo object. A Server instance will have an array of
// ClientInfo objects, indexed by ClientID values.
//
#define CLIENT_INVALID (-1)
class ClientInfo
{
  public:
	typedef enum
	{
		NOT_DEFINED=-1, TCL_CLIENT, JAVA_SCREEN
	}ClientType;

    ClientInfo()
    : ctype(NOT_DEFINED), fd(-1),imagefd(-1), valid(false),active(false),gname(NULL), cname(NULL),dispID(NULL)
    {}
	int GetFd(char * grpname)
	{
		if (gname != NULL)
		{
			if (!strcmp(grpname, gname->grpName))
				return fd;
		}
		return -1;
	}

	ClientType 	ctype;			// client type
    int 		fd;				// command fd
	int			imagefd;		// image fd, this is a write-only fd.
    bool 		valid;
	bool		active;
	GroupKey	*gname;
	char		*cname;
	DispatcherID dispID;
};
typedef int ClientID;

class ControlChannel {
	private:
		char*	 _hostname;
		GroupKey	*_controlgkp;
		int			_controlfd;
		CSgroupKey *_key;
	public:
		ControlChannel();
		~ControlChannel();
		int QueryGroupName(char*& groups );
		int GetFd();
};
class Server:public DispatcherCallback{
friend class ControlChannel;
friend class CmdContainer;
public:
    Server(const char *name,int image_port, 
		int swt_port, int clnt_port, const char* switchname, 
		int maxClients = 10);
    virtual ~Server();
    virtual void MainLoop();      // main loop of server
    virtual void SingleStep();    // once throught the body of MainLoop

    virtual int ReturnVal(		  // send result to client
		ClientID clientID, u_short flag, const char *result) 
	{
		return ReturnVal(clientID, flag, 1, &result, false); 
	}

    virtual int ReturnVal(		  // send result to client
		ClientID clientID, u_short flag, int argc, const char * const *argv,
	    int addBraces = true);

    virtual int SendControl(		  // send command to all clients
		u_short flag, char *command)
	{
	    return SendControl(flag, 1, &command, false);
	}

    virtual int SendControl(		  // send command to all clients
		u_short flag, int argc, const char * const *argv,
	    int addBraces = true);
	virtual void RunCmd(int argc, char** argv, CmdDescriptor& cmdDes)
		{ cerr << "***base class should not be reached" << endl; }
	virtual ClientInfo* getClientInfo(ClientID cid)
	{
		return &_clients[cid];
	}


protected:
    virtual void DoAbort(char *reason);   // print error message and abort
    virtual void WaitForConnection();     // wait for client connection
    virtual void WaitForImageportConnection();     
									    	// wait for client connection
    virtual void CloseImageConnection(ClientID cid);     
    virtual void ReadCmd();               // read client command

    virtual void GetCmd(int argc,	  // get command from client into _cmd
		char **argv);
	virtual void setCurrentClient(ClientID){}

  	enum { STANDALONE = 0, CONNECTED }	serverstate;
    char *_name;                        // name of server
	char *switchname;					// name of the collaborator
	ConnectInfo	switchaddr;				// listen address for the client
    int _port;                          // port number of server
	int	_imageport;						// port number for the image server
    int _listenFd;                      // fd of listening socket
	int _listenSwtFd;					// fd for the switch
	int	_listenImageFd;					// fd for image server
    char *_cmd;							// command from client
	ControlChannel	*_channel;          // chanel for communicating with the
										// switch

private:
	int GetClientFd(char* grpname);		// get the client's fd for a group
    void InitializeListenFd(int port, 
						int& listenFd); // initialize fd for listening
    int FindIdleClientSlot();           // search for slot in client array
	void ProcessGroupControl(ClientID clientID, int argc, char** argv);
										// process group control
protected:
    virtual void BeginConnection(ClientID)
    {}                                     // executed when a connection starts
    virtual void EndConnection(ClientID)
    {}                            		   // executed when a connection ends
    virtual void CloseClient(ClientID);    // close client connection
	virtual int  CloseClientGroup(ClientID);
    ClientInfo *_clients;                  // array of ClientInfo structs
    int _numClients;                       // current number of clients
    int _maxClients;                       // maximum number of clients
	ClientID	collabCid;

// group control functions
protected:
	virtual bool  AfterPassiveJoin(GroupKey* gkp, char*& errmsg);
	virtual bool ExecInitServer(ClientID clientID, char*InitStr, char*&errmsg)
	{	
		errmsg = "NULL";
		return true;
	}					   					//run server initialization 
	virtual bool ExecSwitchCmds(fd_set *); // run incoming switch commands
    virtual void ExecClientCmds(fd_set *); // run incoming client commands

	// right now, it deos not propagate flags, and assume it to be API_CTL
	// it is to be fixed later
	virtual bool GroupCast(GroupKey* gname, 
				char* cname, 
				char* control,
				int argc, 
				const char* const * argv, 
				char*&errmsg);
    virtual void ProcessCmd(ClientID,
			    int argc,
			    char **argv);	 	      // process a single client command
public:
	virtual int _RequestRelinquish(char *groupname, ConnectInfo *cinfo); 
										// requlish control call back
	virtual int _NotifyGrabbed(char *groupname, ConnectInfo *cinfo);
							  			// notify control call back
	virtual int _RequestCheckpoint(char *fname, ConnectInfo *cinfo);
  										// request the active server to 
										// make checkpoint
	virtual int _ExecCheckpoint(char *fname, ConnectInfo *cinfo);
										// inform the passive server that 
										//the checkpoint file is ready
	// server-client side functions
	int SendClientCmd(int fd, int flag, int args, ...);
										// issue notification back: 
										//	SC_OK_Not, SC_Fail_Not
	// server-switch-client
	virtual bool ServerClientCmd(u_short flag, char* result)
	{
		return ServerClientCmd(flag, 1,&result);
	}
	virtual bool ServerClientCmd(u_short flag, int argc,
	  const char* const * argv);
										// propagate commands from active
										// server to the passive server and
										// then to the client
	int writeInteger(int fd, int slotno);
	int readInteger(int fd, int& slotno);
	int	GetControlChannelFd();
};

// export the server object
extern Server* _ThisServer;

// call back functions
int RequestRelinquish(char *groupname, ConnectInfo *cinfo);
int NotifyGrabbed(char *groupname, ConnectInfo *cinfo);
int RequestCheckpoint(char *fname, ConnectInfo *cinfo);
int ExecCheckpoint(char *fname, ConnectInfo *cinfo);
#endif

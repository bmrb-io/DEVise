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
  Revision 1.1.2.1  1998/01/28 22:42:22  taodb
  Added support for group communicatoin

 */

#ifndef _Server_h_
#define _Server_h_

#include "ClientAPI.h"
#include "serverInterface.h"
#include "keys.h"
#include "types.h"
#include "Csprotocols.h"
#include "Dispatcher.h"

//
// Each Server can serve multiple clients. Client state is stored
// in a ClientInfo object. A Server instance will have an array of
// ClientInfo objects, indexed by ClientID values.
//
#define CLIENT_INVALID (-1)
class ClientInfo
{
  public:
    ClientInfo()
    : fd(-1), valid(false),active(false),gname(NULL), cname(NULL),dispID(NULL)
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
    int 		fd;
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

public:
    Server(char *name, int swt_port, int clnt_port, char* switchname, 
		int maxClients = 10);
    virtual ~Server();
    virtual void MainLoop();      // main loop of server
    virtual void SingleStep();    // once throught the body of MainLoop

    virtual int ReturnVal(		  // send result to client
		ClientID clientID, u_short flag, char *result) 
	{
		return ReturnVal(clientID, flag, 1, &result, false); 
	}

    virtual int ReturnVal(		  // send result to client
		ClientID clientID, u_short flag, int argc, char **argv,
	    int addBraces = true);

    virtual int SendControl(		  // send command to all clients
		u_short flag, char *command)
	{
	    return SendControl(flag, 1, &command, false);
	}

    virtual int SendControl(		  // send command to all clients
		u_short flag, int argc, char **argv, int addBraces = true);

protected:
    virtual void DoAbort(char *reason);   // print error message and abort
    virtual void WaitForConnection();     // wait for client connection
    virtual void ReadCmd();               // read client command

    virtual void GetCmd(int argc,	  // get command from client into _cmd
		char **argv);

  	enum { STANDALONE = 0, CONNECTED }	serverstate;
    char *_name;                        // name of server
	char *switchname;					// name of the collaborator
	ConnectInfo address;				// listen address for the server
	ConnectInfo	switchaddr;				// listen address for the client
    int _port;                          // port number of server
    int _listenFd;                      // fd of listening socket
	int _listenSwtFd;					// fd for the switch
    char *_cmd;							// command from client
	ControlChannel	*_channel;          // chanel for communicating with the
										// switch

private:
	int GetClientFd(char* grpname);		// get the client's fd for a group
    void InitializeListenFd();          // initialize fd for listening
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
				char** argv, 
				char*&errmsg);
    virtual bool FilterCmd(ClientID,
			    int argc,
			    char **argv,
				char *&errmsg);	 	      // process a single client command
    virtual void ProcessCmd(ClientID,
			    int argc,
			    char **argv);	 	      // process a single client command
public:
	// Server-Switch side call back functions
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
	virtual bool ServerClientCmd(u_short flag, int argc, char** argv);
										// propagate commands from active
										// server to the passive server and
										// then to the client
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

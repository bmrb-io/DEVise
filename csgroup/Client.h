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
  Declaration of Client (and CompDate) classes.
 */

/*
  $Id$
  $Log$
  Revision 1.2  1998/02/12 17:14:32  wenger
  Merged through collab_br_2; updated version number to 1.5.1.

  Revision 1.1.2.1  1998/01/28 22:42:11  taodb
  Added support for group communicatoin

 */

#ifndef _Client_h_
#define _Client_h_

class CSgroupKey;

class CompDate {
public:
  static char *Get();
};

enum{
	EMPTY        = 0x00,        //un-used slot
	ACTIVE       = 0x02,        //leader of a group
	PASSIVE      = 0x04,        //listener of a group
	REQUESTED    = 0x08         //control is requested
};

class Client {
public:
	enum{
		DOUBLE_GROUP=0,			// specify more than one group
		CREAT_FAIL,				// cannot create a group
		JOIN_FAIL,				// cannot join a group
		NONE_GROUP,				// no group specified
		LEAD_FAIL,				// fail to obtain the token
		NO_TOKEN,				// no token yet
		LISTEN_FAIL, 			// cannot give up the token
		LEAVE_FAIL, 			// fail to leave a group
		NO_REQUEST,				// no one ask for the token
		TOKEN_ALREADY,          // already have the token
		MAX_ERRORS
	};
	// constructor/destrutors etc.
  	Client(char *name, char *hostname, int port, char* initStr);
	virtual ~Client();
	virtual void MainLoop() = 0;        // main loop of client
	CSgroupKey	*csk;					// key to the group

	// call back functions supporting group-communications
	virtual int NotifyGrabbed();		// client clback -notify msg
	virtual int RequestRelinquish();	// client clback-request msg

	// interface for group-communication
	int CreateGroup(char *grpname, 		// create a group
		char* passwd, const char*&);
	int JoinGroup(char *grpname, 		// join a group with sync
		char*passwd, const char*&);
	int LeadGroup(const char*&);		// request leadership
	int ListenGroup(const char*&);		// giveup leadership
	int LeaveGroup(const char*&);		// leave a group w/o leadership
	int QueryGroup(char*&);             // query the available list of groups

protected:
	// functions for communcating with the server
	virtual void DoAbort(char *reason);	// print error message and abort
	virtual int ServerCmd(int argc,
		char **argv);					// send command to server
	virtual int  SendServerCmd			// another way to send cmds to server
		(int args, ...);
	virtual void ReadServer();          // receive command from server
	virtual void ControlCmd(int argc,
		char **argv); 					// process server control command
	virtual void GeneralServerCmd(u_short flag, int argc,
			  char **argv){};			// process server general command
  
	virtual void SendPanelCmd();		// send command to the panel
	virtual void SendPanelErr(char* msg);
	virtual void SetPanelMajorCmd(char* cmd);
	virtual void SetPanelSubCmd(char* cmd);
	virtual void SetPanelInfo(char* cmd);
	char*   panelMajorCmd;
	char*	panelSubCmd;
	char*	panelInfo;

	char *_name;                        // name of client
	char *_hostname;                    // hostname of server
	int _port;                          // port number of server
	int _serverFd;                      // fd of server socket
	char *_result;						// result of command
	char *_cmd;							// command from server
	int	_grpstat;						// group status of the client
	char *_groupname;					// current group name
};

#endif

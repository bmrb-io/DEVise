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
  Revision 1.5.14.1  2005/09/28 22:29:21  wenger
  Various const-ifying to make things compile better on basslet.

  Revision 1.5  2000/01/13 23:06:43  wenger
  Got DEVise to compile with new (much fussier) compiler (g++ 2.95.2).

  Revision 1.4  1998/05/02 08:38:51  taodb
  Added command logging and playing support
  Added communication support for JAVA Screen

  Revision 1.3  1998/03/11 18:25:05  wenger
  Got DEVise 1.5.2 to compile and link on Linux; includes drastically
  reducing include dependencies between csgroup code and the rest of
  the code, and within the csgroup code.  (Note: running collaboration
  doesn't work yet.)

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
  	Client(const char *name, const char *hostname, int port,
	  const char* initStr);
	virtual ~Client();
	virtual void MainLoop() = 0;        // main loop of client
	CSgroupKey	*csk;					// key to the group

	// call back functions supporting group-communications
	virtual int NotifyGrabbed();		// client clback -notify msg
	virtual int RequestRelinquish();	// client clback-request msg

	// interface for group-communication
	int CreateGroup(const char *grpname,// create a group
		const char* passwd, const char*&);
	int JoinGroup(const char *grpname, 	// join a group with sync
		const char*passwd, const char*&);
	int LeadGroup(const char*&);		// request leadership
	int ListenGroup(const char*&);		// giveup leadership
	int LeaveGroup(const char*&);		// leave a group w/o leadership
	int QueryGroup(char*&);             // query the available list of groups

protected:
	// functions for communcating with the server
	virtual void DoAbort(char *reason);	// print error message and abort
	virtual int ServerCmd(int argc,		// send command to server
		const char * const *argv);
	virtual int  SendServerCmd			// another way to send cmds to server
		(int args, ...);
	virtual void ReadServer();          // receive command from server
	virtual void ControlCmd(int argc,	// process server control command
		const char * const *argv);
	virtual void GeneralServerCmd(u_short flag, int argc,
	    const char * const *argv){};	// process server general command
  
	virtual void SendPanelCmd();		// send command to the panel
	virtual void SendPanelErr(const char* msg);
	virtual void SetPanelMajorCmd(const char* cmd);
	virtual void SetPanelSubCmd(const char* cmd);
	virtual void SetPanelInfo(const char* cmd);
	int		readInteger(int,int&);
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
	int	_serverSlot;					// server slot# for current conenction
};

#endif

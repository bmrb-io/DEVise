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
  $Id$

  $Log$
  Revision 1.5  1998/05/02 09:00:38  taodb
  Added support for JAVA Screen and command logging

  Revision 1.4  1998/04/07 14:30:18  wenger
  Reduced unnecessary include dependencies.

  Revision 1.3  1998/03/27 15:08:53  wenger
  Added dumping of logical session description, added GUI for dumping
  logical or physical description; cleaned up some of the command code
  a little.

 */

#ifndef _CmdContainer_h_
#define _CmdContainer_h_

#include <iostream.h>

#include "Control.h"
#include "DeviseCommand.h"
#include "DeviseServer.h"

class ControlPanel;
class DeviseCommand;
class CmdLogRecord;

class CmdContainer 
{
  	friend ostream& operator <<(ostream& os, const CmdContainer& cc);
	public:
		typedef enum {MONOLITHIC=0,CSGROUP} Make;
		CmdContainer(ControlPanel* control, Make make, DeviseServer* server);
		~CmdContainer();

		int	Run(int argc, char** argv, ControlPanel* control, 
			CmdDescriptor& cmdDes);
		int	RunOneCommand(int argc, char** argv, ControlPanel* control);

		void insertCmd(char*, DeviseCommand *,int cmdsize);		
		DeviseCommand* lookupCmd(char*);
		void deleteCmd(char* cmdName);

		Make	getMake(){return make;}
		void	setMake(Make newMake) {make = newMake;}

		char*	getLogName(){ return cmdLogFname;}
		CmdLogRecord* getCmdLog() const 
			{ return cmdLog;}
		DeviseServer* getDeviseServer(){ return _server;}

	private:
		long	logCommand(int argc, char** argv, CmdDescriptor& cmdDes);
		//bool	playCommand(long logId1, long logId2);

		CmdLogRecord	*cmdLog;
		Make	make;
		DeviseServer*	_server;
		char		*cmdLogFname;
};
ostream& operator <<(ostream& os, const CmdContainer& cc);
//TEMP -- this should be changed to a static method.  RKW Aug 17, 1998.
extern CmdContainer*	cmdContainerp;

#endif // _CmdContainer_h_

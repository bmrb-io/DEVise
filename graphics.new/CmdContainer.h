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
  Revision 1.3  1998/03/27 15:08:53  wenger
  Added dumping of logical session description, added GUI for dumping
  logical or physical description; cleaned up some of the command code
  a little.

 */

#ifndef _CmdContainer_h_
#define _CmdContainer_h_

#include <iostream.h>

class ControlPanel;
class DeviseCommand;

class CmdContainer 
{
  	friend ostream& operator <<(ostream& os, const CmdContainer& cc);
	public:
		typedef enum {MONOLITHIC=0,CSGROUP} Make;
		CmdContainer(ControlPanel* control, Make make );
		~CmdContainer();
		int	Run(int argc, char** argv, ControlPanel* control);
		void insertCmd(char*, DeviseCommand *,int cmdsize);		
		DeviseCommand* lookupCmd(char*);
		void deleteCmd(char* cmdName);
		Make	getMake(){return make;}
	private:
		Make	make;
};
ostream& operator <<(ostream& os, const CmdContainer& cc);
extern CmdContainer*	cmdContainerp;

#endif // _CmdContainer_h_

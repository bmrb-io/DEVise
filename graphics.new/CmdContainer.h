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
 */

#ifndef _CmdContainer_h_
#define _CmdContainer_h_

#include <iostream.h>

#include "Control.h"
#include "DeviseCommand.h"

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

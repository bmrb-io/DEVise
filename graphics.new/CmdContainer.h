#ifndef _COMMAND_CONTAINER_H
#define _COMMAND_CONTAINER_H
#include <iostream.h>
#include "Control.h"
#include "DeviseCommand.h"
#include "ParseAPI.h"

#define REGISTER_COMMAND(objType)\
{\
	DeviseCommandOption	cmdOption;\
	cmdOption.setCmdName(#objType);\
	DeviseCommand_##objType *obj= new (DeviseCommand_##objType)(cmdOption);\
	insertCmd(#objType,(DeviseCommand*)obj,sizeof(DeviseCommand_##objType)); \
}

#define REGISTER_COMMAND_WITH_OPTION(objType,cmdOption)\
{\
	cmdOption.setCmdName(#objType);\
	DeviseCommand_##objType *obj= new (DeviseCommand_##objType)(cmdOption);\
	insertCmd(#objType,(DeviseCommand*)obj); \
}

class DeviseCommand;
class CmdContainer 
{
  	friend ostream& operator <<(ostream& os, const CmdContainer& cc);
	public:
		typedef enum {MONOLITHIC=0,CSGROUP} Make;
		CmdContainer(ControlPanel* control, Make make );
		~CmdContainer();
		int	Run(int argc, char** argv);
		void insertCmd(char*, DeviseCommand *,int cmdsize);		
		DeviseCommand* lookupCmd(char*);
		void deleteCmd(char* cmdName);
		Make	getMake(){return make;}
	private:
		Make	make;
		ControlPanel*	control;
};
ostream& operator <<(ostream& os, const CmdContainer& cc);
extern CmdContainer*	cmdContainerp;
#endif

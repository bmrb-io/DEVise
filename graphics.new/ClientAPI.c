/*
  ========================================================================
  DEVise Data Visualization Software
  (c) Copyright 1992-1996
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
  Revision 1.44  1996/04/18 18:14:20  jussi
  The Tcl/Tk file interpreted in batch mode is now batch.tcl.

  Revision 1.43  1996/04/16 19:46:23  jussi
  Added DoAbort() method.

  Revision 1.42  1996/04/15 15:08:11  jussi
  Interface to ViewGraph's mapping iterator has changed. Added another
  version of insertMapping command that takes the label as a parameter.

  Revision 1.41  1996/04/14 00:17:15  jussi
  Removed interpMapClassInfo and clearInterp commands. Added
  createMappingClass which will eventually replace createInterp
  complete. createInterp is retained for backward compatibility.

  Revision 1.40  1996/04/11 17:53:45  jussi
  Added command verbs raiseView and lowerView.

  Revision 1.39  1996/04/10 15:30:07  jussi
  Added removeMapping command verb.

  Revision 1.38  1996/04/09 22:52:25  jussi
  Added getViewOverrideColor and setViewOverrideColor.

  Revision 1.37  1996/03/26 19:09:17  jussi
  Disabled debugging.

  Revision 1.36  1996/03/26 17:20:03  jussi
  Streamlined DEVise getSchema command.

  Revision 1.35  1996/03/25 22:59:46  jussi
  Reverted back to the state where maps are inserted to views but
  views are not inserted to maps. Currently TDataMap only stores
  one view which is not enough.

  Revision 1.34  1996/03/22 18:25:47  jussi
  Fixed problem with insertWindow.

  Revision 1.33  1996/03/07 16:54:39  jussi
  Added association of TDataMap and ViewGraph.

  Revision 1.32  1996/01/27 00:21:22  jussi
  Added support for postscript execution; scripts which are
  executed after query processor has evaluated all queries
  and system is idle.

  Revision 1.31  1996/01/19 18:59:02  jussi
  Added ClearTopGroups.

  Revision 1.30  1996/01/17 20:53:51  jussi
  Added support for additional image export formats.

  Revision 1.29  1996/01/13 20:51:20  jussi
  Added references to www_extract.

  Revision 1.28  1996/01/12 15:25:05  jussi
  Replaced libc.h with stdlib.h.

  Revision 1.27  1996/01/10 18:47:36  jussi
  Attribute hi/lo values are now conditional; a boolean value
  returned from getSchema indicates which values are default
  and which are not.

  Revision 1.26  1996/01/10 00:37:40  jussi
  Added support for hi/lo values defined in schema.

  Revision 1.25  1996/01/09 16:36:26  jussi
  Added references to Seq.c and seq_extract.

  Revision 1.24  1995/12/28 20:01:59  jussi
  Small fix to remove compiler warning.

  Revision 1.23  1995/12/14 18:07:29  jussi
  Small fixes to get rid of g++ -Wall warnings.

  Revision 1.22  1995/12/14 15:37:20  jussi
  Minor fixes.

  Revision 1.21  1995/12/14 15:27:44  jussi
  Changed getViewStatistics to reflect the new scheme of turning
  multiple stats on/off in one setViewStatistics call.

  Revision 1.20  1995/12/08 23:47:12  ravim
  Window name is a parameter to KGraph creation. Previous KGraph not deleted
  when the new one is created.

  Revision 1.19  1995/12/07 02:18:29  ravim
  The set of stats to be displayed is specified as a parameter to
  setViewStatistics.

  Revision 1.18  1995/12/06 05:42:57  ravim
  Added function to display KGraph.

  Revision 1.17  1995/12/05 17:07:00  jussi
  View statistics are now part of ViewGraph and not View, its subclass.

  Revision 1.16  1995/12/04 18:07:35  jussi
  Added getLabel and getViewStatistics. Replaced ToggleStatistics with
  SetViewStatistics.

  Revision 1.15  1995/12/02 21:06:30  jussi
  Added support for TK_WINDOW, added Set Label command, and fixed
  error checking in insertWindow.

  Revision 1.14  1995/11/28 17:01:50  jussi
  Added copyright notice and renamed printWindows to saveWindow which
  now saves a single window image to a given file.

  Revision 1.13  1995/11/28 05:34:18  ravim
  Support for statistics.

  Revision 1.12  1995/11/28 00:02:13  jussi
  Added printWindows command.

  Revision 1.11  1995/11/18 01:52:15  ravim
  defaultGroup removed. Groups associated with schema.

  Revision 1.10  1995/11/15 18:19:34  jussi
  Added definition of crsp_extract command.

  Revision 1.9  1995/09/28 00:00:12  ravim
  Fixed some bugs. Added some new functions for handling groups.

  Revision 1.8  1995/09/26 23:06:22  jussi
  Did some reformatting.

  Revision 1.7  1995/09/19 16:08:32  jussi
  Changed comp_extract from C linkage to C++ linkage.

  Revision 1.6  1995/09/19 15:30:22  jussi
  Added creation of cstat_extract command for Compustat data.

  Revision 1.5  1995/09/13 14:43:39  jussi
  Found one more instance vhere name of view was used without protecting
  braces or quotes.

  Revision 1.4  1995/09/12 16:06:15  jussi
  Modified SelectView() and other view-related TCL calls which
  did not work if view name had spaces in it.

  Revision 1.3  1995/09/06 15:30:29  jussi
  Added creation of extractStocksCmd command for ISSM data

  Revision 1.2  1995/09/05 22:16:09  jussi
  Added CVS header.
*/

#include<stdio.h>
#include<stdlib.h>
#include"ClientAPI.h"
#include<netinet/in.h>
#include<arpa/inet.h>
#include<sys/types.h>
#include<sys/time.h>
#include<sys/socket.h>
#include<netdb.h>
#include<errno.h>
#include<fcntl.h>
#if 0 
extern GroupDir *gdir;
ViewKGraph *vkg = NULL;
extern int extractStocksCmd(ClientData clientData, Tcl_Interp *interp,
			    int argc, char *argv[]);
extern int comp_extract(ClientData clientData, Tcl_Interp *interp,
			int argc, char *argv[]);
extern int crsp_extract(ClientData clientData, Tcl_Interp *interp,
			int argc, char *argv[]);
extern int seq_extract(ClientData clientData, Tcl_Interp *interp,
		       int argc, char *argv[]);
extern int www_extract(ClientData clientData, Tcl_Interp *interp,
		       int argc, char *argv[]);

ControlPanel::Mode ClientControl::_mode = ControlPanel::DisplayMode;
MapInterpClassInfo *ClientControl::_interpProto = NULL;

ControlPanel *GetTkControl()
{
  return new ClientControl();
}

#endif

ClientControl *ClientControl::_currentClient = NULL;

/* bitmap for control panel */
/*
#define idle_width 16
#define idle_height 16
static char idle_bits[] = {
   0xff, 0x01, 0xff, 0x01, 0x1f, 0x00, 0x3f, 0x00, 0x7f, 0x00, 0xfb, 0x00,
   0xf3, 0x01, 0xe3, 0x03, 0xc3, 0x07, 0x80, 0x0f, 0x00, 0x1f, 0x00, 0x3e,
   0x00, 0x7c, 0x00, 0xf8, 0x00, 0xf0, 0x00, 0xe0};
*/

ClientControl::ClientControl()
{
  #if 0 
  --------------------------------
  
  _interpProto = new MapInterpClassInfo();
  
  View::InsertViewCallback(this);
  _busy = false;
  _restoring = false;
  _template = false;

  _fileName = (char *)malloc(1);
  _fileName[0] = '\0';
  _fileAlias = (char *)malloc(1);
  _fileAlias[0] = '\0';

  _winName = (char *)malloc(1);
  _winName[0] = '\0';
  _gdataName = (char *)malloc(1);
  _gdataName[0] = '\0';
  _viewName = (char *)malloc(1);
  _viewName[0] = '\0';
  _sessionName = CopyString(Init::SessionName());
  _argv0 = CopyString(Init::ProgName());


  /* register with dispatcher to be always informed of when to run */
  //Dispatcher::Current()->Register(this, AllState, true);


#endif
  /* Create a new interpreter */
  _interp = Tcl_CreateInterp();
  _mainWindow = Tk_CreateMainWindow(_interp, NULL, "DEVise", "DEVise");
  if (_mainWindow == NULL) {
    fprintf(stderr, "%s\n", _interp->result);
    exit(1);
  }
  Tk_MoveWindow(_mainWindow, 0,0);
  Tk_GeometryRequest(_mainWindow, 100, 200);
  _currentClient = this;

#ifdef TK_WINDOW
  ControlPanelTclInterp = _interp;
  ControlPanelMainWindow = _mainWindow;
#endif

  /* Init tcl and tk */
  if (Tcl_Init(_interp) == TCL_ERROR) {
    fprintf(stderr,"can't init tcl in TkControl.c\n");
    Exit(1);
  }
  if (Tk_Init(_interp) == TCL_ERROR) {
    fprintf(stderr,"can't init tk in TkControl.c\n");
    Exit(1);
  }
#if 0
     if (Tk_DefineBitmap(_interp,
     Tk_GetUid("idle"), idle_bits, idle_width,idle_height) == TCL_ERROR) {
     fprintf(stderr,"ClientControl::can't init bitmap\n");
     Exit(1);
     }
#endif
}

/* start session */
void ClientControl::StartSession()
{
  printf("DEVise Data Visualization Software\n");
  printf("(c) Copyright 1992-1996\n");
  printf("By the DEVise Development Group\n");
  printf("All Rights Reserved.\n");
  printf("\n");

  char *controlFile = "control.tk";
  //if (Init::BatchFile()) 
  //  controlFile = "batch.tcl";
  // NEED TO CHECK THIS LATER..

  char *envPath = getenv("DEVISE_LIB");
  char *control;
  char buf[256];
  if (envPath) {
    sprintf(buf, "%s/%s", envPath, controlFile);
    control = buf;
  } else
    control = controlFile;

  printf("Control panel file is: %s\n", control);

  // Implementation of the client sides..
  // The client first tries to connect to the server and if it fails
  // it does not operate...
  // A TCP connection is opened ..

	char *serverFile= "server.dat";

	envPath = getenv("DEVISE_LIB");
	char servBuf[256];
	char servName[256];
	int portNum;
	if (envPath) 
	  sprintf(servBuf, "%s/%s", envPath, serverFile);

	FILE *fp;
	DOASSERT(((fp = fopen(servBuf,"r"))!= 0),"Server name could not be found.");

	DOASSERT(fscanf(fp,"%s %d",servName,&portNum),"Server name not found.");
	printf("Port no = %d\n",portNum);
	openConnection(servName,portNum);
	
	fclose(fp);
	// Now send the XDisplay name so as to register the Display..
#if 0	
	char * displayName = getenv("DISPLAY");
	DOASSERT((displayName !=  0),"Cannot open display..DISPLAY not defined");

	DOASSERT((send(socketFd,displayName,strlen(displayName)+1,0) >= 0) ,
		"Send on socket failed");
#endif

  Tcl_LinkVar(_interp, "gdataName", (char *)&_gdataName, TCL_LINK_STRING);
  Tcl_LinkVar(_interp, "windowName", (char *) &_winName, TCL_LINK_STRING);
  Tcl_LinkVar(_interp, "viewName", (char *) &_viewName, TCL_LINK_STRING);
  Tcl_LinkVar(_interp, "sessionName", (char *)&_sessionName, TCL_LINK_STRING);
  Tcl_LinkVar(_interp, "argv0", (char *)&_argv0, TCL_LINK_STRING);
  Tcl_LinkVar(_interp, "restoring", (char *)&_restoring, TCL_LINK_INT);
  Tcl_LinkVar(_interp, "template", (char *)&_template, TCL_LINK_INT);

  Tcl_LinkVar(_interp, "fileName", (char *)&_fileName, TCL_LINK_STRING);
  Tcl_LinkVar(_interp, "fileAlias", (char *)&_fileAlias, TCL_LINK_STRING);
  
  /* Create a new tcl command for control panel */
  Tcl_CreateCommand(_interp, "DEVise", ControlCmd, this, NULL);

  /* Create a new tcl command for ISSM stock data */
 // Tcl_CreateCommand(_interp, "issm_extractStocks", extractStocksCmd, 0, 0);

  /* Create a new tcl command for Compustat data */
 // Tcl_CreateCommand(_interp, "cstat_extract", comp_extract, 0, 0);

  /* Create a new tcl command for CRSP data */
  //Tcl_CreateCommand(_interp, "crsp_extract", crsp_extract, 0, 0);

  /* Create a new tcl command for SEQ data */
  //Tcl_CreateCommand(_interp, "seq_extract", seq_extract, 0, 0);

  /* Create a new tcl command for WWW data */
  //Tcl_CreateCommand(_interp, "www_extract", www_extract, 0, 0);

  int code = Tcl_EvalFile(_interp,control);
  if (code != TCL_OK) {
    fprintf(stderr,"%s\n", _interp->result);
    Exit(1);
  }

	if (0){ // MUST CHANGE HERE...
  //if (Init::Restore()) {
    /* restore session */
    _restoring = true;
    int code = Tcl_EvalFile(_interp,_sessionName);
    _restoring = false;
    if (code != TCL_OK) {
      fprintf(stderr,"Can't restore session file %s\n",_sessionName);
      fprintf(stderr,"%s\n", _interp->result);
      Exit(1);
    }
  }
} 

void ClientControl::openConnection(char *servName,int portNum)
{

	
	socketFd = socket(AF_INET,SOCK_STREAM,0);
	DOASSERT(socketFd != -1,"Socket in Client");

	// Now bind these to the address..

	struct hostent *servEnt = gethostbyname(servName);
	DOASSERT(servEnt != NULL , "Address determination error");

	struct in_addr * ptr;
	ptr = 0;
	switch(servEnt->h_addrtype){
		
		case AF_INET: ptr = (struct in_addr * ) *servEnt->h_addr_list;
					  break;
		default:	  DOASSERT(0,"Address type not supported");	
					  exit(0);

	}
	
	struct sockaddr_in servAddr;
	bzero(&servAddr,sizeof(servAddr));
	
	printf("Portnumber inside  = %d\n",portNum);
	u_short ServPort= (u_short)portNum;
	printf("short POrtnumber inside  = %u\n",ServPort);
	servAddr.sin_family = AF_INET;
	servAddr.sin_port   = htons(ServPort);
	servAddr.sin_addr   = * ptr;

	DOASSERT((connect(socketFd,(struct sockaddr *)&servAddr,\
		sizeof(struct sockaddr)) >= 0  )," Cannot connect to client");
	
	// connected so the read etc.. calls can use the socketFd 
	// Now wait for the control connection...
	// *********************//

	controlFd = socket(AF_INET,SOCK_STREAM,0);
	DOASSERT(controlFd != -1,"Socket in client ");

	struct sockaddr_in cliAddr;

    bzero(&cliAddr,sizeof(cliAddr));

    cliAddr.sin_family = AF_INET;
    cliAddr.sin_port   = htons(0);
    cliAddr.sin_addr.s_addr   = htonl(INADDR_ANY);

    DOASSERT((bind(controlFd,(struct sockaddr *)&cliAddr,
          sizeof(struct sockaddr)) >= 0 )," Cannot bind to address");

    // Now make this non - blocking and proceed..
    DOASSERT((listen(controlFd,5)>= 0),"Listen failed");

	// Now send the port numbers and the client name over the net..
	// Now do an accept on the client control...	
	char cliName[30];

	if (gethostname(cliName,30) < 0)
		perror("GethostName");
	struct sockaddr_in tempAddr;	
	bzero(&tempAddr,sizeof(struct sockaddr));
	int len = sizeof(struct sockaddr);

	if (getsockname(controlFd,(sockaddr *)&tempAddr,&len) < 0)
		perror("Get sockname");
	
	u_short port = tempAddr.sin_port;
	u_short size = strlen(servName)+1;
	u_short tempSize = htons(size);

	if (send(socketFd,(char *)&tempSize,sizeof(u_short),0) < 0)	
		perror("Send");
	
	if (send(socketFd,cliName,size,0) < 0)	
		perror("Send");
	
	if (send(socketFd,(char *)&port,sizeof(u_short),0) < 0)	
		perror("Send");
	
	int tempFd;
	bzero(&cliAddr,sizeof(struct sockaddr));
    if ((tempFd = accept(controlFd,(struct sockaddr *)&cliAddr,&len)) < 0 ) {
	
      perror("Error in accept call");
            Exit(1);
    }
	controlFd = tempFd;
	
#ifndef PENTIUM 
    DOASSERT(fcntl(controlFd,F_SETFL,FNDELAY) >= 0 ,"Fcntl failed") ;
#else
    DOASSERT(fcntl(controlFd,F_SETFL,O_NDELAY) >= 0 ,"Fcntl failed") ;
#endif

}
#if 0
char * ClientControl::SessionName()
{
  return _sessionName;
}
void ClientControl::SetSessionName(char *name)
{
  /*XXX: This is a memory leak */
  _sessionName = CopyString(name);
}
inline void MakeReturnVals(Tcl_Interp *interp, int numArgs, char **args)
{
  for (int i=0; i < numArgs; i++) {
    Tcl_AppendElement(interp,args[i]);
  }
}
#endif
int ClientControl::MakeReturnVals(Tcl_Interp *interp, char * errorMsg)
{

  u_short size;
  u_short errorFlag;
  u_short numElements;
  static u_short oldsize = 0;
  if ( recv(socketFd,(char *)&errorFlag,sizeof(u_short),0) < 0 ) {
       Tcl_AppendElement(interp, errorMsg);
       return -1;
  }
	
  errorFlag = ntohs(errorFlag);

  if ( recv(socketFd,(char *)&numElements,sizeof(u_short),0) < 0 ) {
       Tcl_AppendElement(interp, errorMsg);
       return -1;
   }
   numElements = ntohs(numElements);
	
	if (numElements > 0)
   for(int i = 0; i < numElements; i ++){
		
		if ( recv(socketFd,(char *)&size,sizeof(u_short),0) < 0 ) {
		   Tcl_AppendElement(interp, errorMsg);
		   return -1;
		}
		size = ntohs(size);
		static char * buff = 0;
		// Saves time
		if ( oldsize < size ) {
		  if (buff) delete []buff;
		  buff = new char[(int)size];
		  oldsize = size;
		}
		if ( size > 0 ) {
			if (recv(socketFd,buff,(int)size,0)< 0){
		  		Tcl_AppendElement(interp, "Receive failed");
		  		return -1;
			}
			Tcl_AppendElement(interp,buff); // Needed ??? 
		}
		else
			Tcl_AppendElement(interp,NULL);
	}
#if 0
	}	
	else
		strcpy(interp->result,"");
#endif  	

	if ( errorFlag ) 
		return -1;
	else
		return 1;
}
int ClientControl::sendServer(char **argv,int num)
{

	u_short size;

	if (num <= 0 ) return 1;

	size = (u_short ) num;

	if (send(socketFd,(char *)&size,sizeof(u_short),0) < 0)
		return -1;


	for(int i = 1; i <= num; i++){

		size = (u_short) (strlen(argv[i])+1);
		size = htons(size);
		if (send(socketFd,(char *)&size,sizeof(u_short),0) < 0)
			return -1;

		if (send(socketFd,argv[i],size,0) < 0)
		return -1;

	}

	return 1;

}
#if 0
ClientControl::Mode ClientControl::GetMode()
{
  return _mode;
}
Boolean ClientControl::Restoring()
{
  return _restoring;
}

#endif
void ClientControl::DoAbort(char *reason)
{
  char cmd[256];
  sprintf(cmd, "AbortProgram {%s}", reason);
  (void)Tcl_Eval(_interp, cmd);
}
int ClientControl::receive(Tcl_Interp *interp,char *errorMsg)
{
	
	u_short size;
	static u_short oldsize = 0;
	// Flag to know if the result is in error or not.. 1 - error 
	u_short errorFlag;

	if ( recv(socketFd,(char *)&errorFlag,sizeof(u_short),0) < 0 ) {
		sprintf(interp->result,"Receive failed in %s",  errorMsg);
		return -1;
	}
	errorFlag = ntohs(errorFlag);
	if ( recv(socketFd,(char *)&size,sizeof(u_short),0) < 0 ) {
		sprintf(interp->result,"Receive failed in %s",  errorMsg);
		return -1;
	}
	size = ntohs(size);
#if 1
	static char * buff = 0;
#else
	static char buff[1000];
#endif
	
	// Saves time 
	
	if (size > 0 ){

#if 1
		if ( oldsize < size ) {
			if (buff) delete []buff;
			buff = new char[(int)size];
			oldsize = size;
		}
#endif
		if (recv(socketFd,buff,(int)size,0)< 0){
			Tcl_AppendElement(interp, "Receive failed");
			return -1;
		}
		 Tcl_AppendResult(interp,buff,(char *) NULL);
	}
	else
		 Tcl_AppendResult(interp,NULL,(char *) NULL);
		//strcpy(interp->result,"");

	if ( errorFlag ) 
		return -1;
	else
		return 1;

}

int ClientControl::sendReceive(Tcl_Interp *interp,char **argv,int num,char * errorMsg)
{

	if ( sendServer(argv,num) < 0 ) {
		sprintf(interp->result,"Send failed in %s",errorMsg);
		return -1;
	}
	if ( receive(interp,errorMsg) < 0 )
		return -1;

	return 1;
}
int ClientControl::sendReturnVals(Tcl_Interp *interp,char **argv,int num,char * errorMsg)
{
	

	if ( sendServer(argv,num) < 0 ) {
		sprintf(interp->result,"Send failed in %s",errorMsg);
		return -1;
	}
	if ( MakeReturnVals(interp,errorMsg)< 0 )
		return -1;
	return 1;
}

int ClientControl::ControlCmd(ClientData clientData, Tcl_Interp *interp,int argc,char *argv[])
{
	return (_currentClient)->ControlComd(clientData, interp,argc,argv);

}

int ClientControl::ControlComd(ClientData clientData, Tcl_Interp *interp,
			int argc, char *argv[]) {
	ClientControl *control = (ClientControl *)clientData;

	//ClassDir *classDir = control->GetClassDir();
	int code;
	int numArgs; char **args;
	char *name;
	// Contains the actual chars to be transmitted...
	// It is allocated and edalloc everytime..
	char * buff; 
	u_short size;

#if DEBUG	
	printf("Function %s %d args \n" ,argv[1],argc-1);
#endif

	Tcl_ResetResult(interp);
	// It is assumed that in case the server wishes to transmit anything it
	// first sends a u_short value indicating the amount of bytes it is 
	// sending so that the client can allocate enought buffer space..
	
	if (argc == 1) {
		Tcl_AppendElement(interp,"wrong args");
		goto error;
	}
	else if (argc == 2) {
		if (strcmp(argv[1],"setWindowName") == 0){
			argv[2] = new char[strlen(_winName)+1];
			strcpy(argv[2],_winName);
			if (sendServer(argv,2) < 0){
				strcpy(interp->result , "setWindowName failed");
				goto error;
			}
			delete argv[2];
		}
		else if (strcmp(argv[1],"date") == 0 ) {
			if (sendReceive (interp,argv,1,"date ") < 0 ) 
				goto error;
		}	
		else if (strcmp(argv[1],"clearQP") == 0) {
			if (sendServer(argv,1)< 0 ){
				Tcl_AppendElement(interp,"ClearQP failed");
				goto error;
			}
		}
		else if (strcmp(argv[1],"clearTopGroups") == 0) {
			if (sendServer(argv,1)< 0 ){
				interp->result = "Send failed in clear Top GRoups";
				goto error;
			}
		}		
		else if (strcmp(argv[1],"printDispatcher") == 0) {
		    // Needs to be changed later...
			if (sendReceive(interp,argv,1,"Print Dispatcher")< 0 )
				goto error;
		}	
		else if (strcmp(argv[1],"catFiles") == 0) {
			if (sendReturnVals(interp,argv,1,"Cat Files ")  < 0 )
				goto error;
		}	
		else if (strcmp(argv[1],"exit")== 0) {
			if (sendReceive(interp,argv,1,"Exit")< 0 ){
				interp->result = "EXIT failed - server Down??" ;
				// Probably must use Control::Exit or something..
			}
			Exit(0);
		}
		else {
			Tcl_AppendElement(interp,"wrong args");
			goto error;
		}
	}
	else if (strcmp(argv[1], "showkgraph") == 0) {
	 	 if( sendReceive(interp,argv,argc -1,"Show KGraph ") < 0 ) 
			goto error;
	}	
	else if (strcmp(argv[1],"createMappingClass") == 0) {
		if (sendReceive(interp,argv,argc-1,"Create Mapping class")< 0)
			goto error;
	}
	else if (argc == 3){
		if (strcmp(argv[0],"invalidatePixmap") == 0 ) {
		 if( sendReceive(interp,argv,2,"Invalidate Pixel Map") < 0 )
					goto error;
		}
	else if (strcmp(argv[1],"readLine") == 0 ) {
		if ( sendReceive(interp,argv,2,"Read line") < 0 ) 
			goto error;
	}	
	else if (strcmp(argv[1],"close") == 0) {
		if ( sendServer(argv,2)< 0 ) {
			interp->result = "Close failed!!";
			goto error;
		}
	}
	else if (strcmp(argv[1],"isMapped") == 0) {
			if ( sendReceive(interp,argv,2,"is Mapped") < 0 )
				goto error;
	}	
	else if (strcmp(argv[1],"getLabel") == 0){
		if ( sendReceive(interp,argv,2,"get Label") < 0 ) 
			goto error;
	}
	else if (strcmp(argv[1],"tdataFileName") == 0) {
		if ( sendReceive(interp,argv,2,"tData File Name ") < 0 ) 
			goto error;
	}
	else if (strcmp(argv[1],"getViewWin") == 0) {
		if ( sendReceive(interp,argv,2,"get Label") < 0 ) 
			goto error;
	}
	else if (strcmp(argv[1],"clearViewHistory") == 0) {
		if ( sendReceive(interp,argv,2,"clear View history ") < 0 ) 
			goto error;
	}
	else if (strcmp(argv[1],"getCursorViews") == 0) {
		if ( sendReceive(interp,argv,2,"get Cursor Views") < 0 ) 
			goto error;
	}
	else if (strcmp(argv[1],"getMappingTData") == 0) {
		if ( sendReceive(interp,argv,2,"get Mapping TData") < 0 ) 
			goto error;
	}
	else if (strcmp(argv[1],"openSession") == 0 ) {
		(void)Tcl_Eval(interp,"set template 0");
		control->_restoring = true;
		code = Tcl_EvalFile(interp,argv[2]);
		control->_restoring = false;
		if (code != TCL_OK) {
			Tcl_AppendElement(interp, "cant restore session file");
			goto error;
		}
		if ( sendServer(argv,2)< 0 ) {
			Tcl_AppendElement(interp,"Error in sending ");
			goto error;
		}
	}

	else if (strcmp(argv[1],"openTemplate") == 0 ) {
		(void)Tcl_Eval(interp,"set template 1");
            control->_restoring = true;
            code = Tcl_EvalFile(interp,argv[2]);
            control->_restoring = false;
            if (code != TCL_OK) {
                Tcl_AppendElement(interp,"cant restore template file");
                goto error;
            } 
		if ( sendServer(argv,2)< 0 ) {
			Tcl_AppendElement(interp, "Error in sending");
			goto error;
		}
	}

	else if (strcmp(argv[1],"destroy") == 0 ) {
		if ( sendServer(argv,2) < 0 ) {
			Tcl_AppendElement(interp, "Destroy failed");
			goto error;
		}
	}
	else if (strcmp(argv[1],"parseDateFloat") == 0 ) {
		if ( sendReceive(interp,argv,2,"parse Date float ") < 0 ) 
			goto error;
	}
	else if (strcmp(argv[1],"isInterpretedGData") == 0 ) {
		if ( sendReceive(interp,argv,2,"Is Intepreted GData ") < 0 ) 
			goto error;
	}
	else if (strcmp(argv[1],"isInterpreted") == 0 ) {
		if ( sendReceive(interp,argv,2,"Is Intepreted") < 0 ) 
			goto error;
	}
	else if (strcmp(argv[1],"getPixelWidth") == 0 ) {
		if ( sendReceive(interp,argv,2,"get Pixel Width") < 0 ) 
			goto error; // NEED TO CHECK THIS AS THE ORIGINAL USES
		// Tcl_AppendElement
	}
	else if (strcmp(argv[1], "getTopGroups") == 0) {
		//gdir->top_level_groups(interp, argv[2]);
		if ( sendReceive(interp,argv,2,"Get Top groups") < 0 ) 
			goto error;
	}
	else if (strcmp(argv[1], "getWindowLayout") == 0) {
		if ( sendReceive(interp,argv,2,"get Window Layout") < 0 ) 
			goto error;
	}
	else if (strcmp(argv[1],"getSchema") == 0) {
		if ( sendReturnVals(interp,argv,2,"Get Schema") < 0 ) 
			goto error;
	}
	else if (strcmp(argv[1],"getAction") == 0 ) {
		if ( sendReceive (interp,argv,2,"get Action") < 0 ) 
			goto error;
	}
	else if (strcmp(argv[1],"getLinkFlag") == 0 ) {
		if ( sendReceive(interp,argv,2,"Get Link flag") < 0 ) 
			goto error;
	}
	else if (strcmp(argv[1],"importFileType") == 0) {
		if ( sendReceive(interp,argv,2,"Import File type") < 0 ) 
			goto error;
	}
	else if (strcmp(argv[1],"changeableParam") == 0) {
		if ( sendReceive(interp,argv,2,"changeable Param") < 0 ) 
			goto error;
	}
	else if (strcmp(argv[1],"getInstParam") == 0) {
		if(	sendReturnVals(interp, argv,2,"Get instance Param")<0)
			goto error;
	}	
	else if (strcmp(argv[1],"tcheckpoint") == 0) {
		if (sendServer(argv,2) < 0 ){
			Tcl_AppendElement(interp,"Send failed in tcheckpoint");
			goto error;
		}
	}
	else if (strcmp(argv[1],"get") == 0) {
		if(	sendReturnVals(interp, argv,2,"get")<0)
			goto error;
	} 
	else if (strcmp(argv[1],"changeMode") == 0) {
		if (sendServer(argv,2) < 0 ){
			Tcl_AppendElement(interp, "Send failed in ChangeMode");
			goto error;
		}
	}	
	else if (strcmp(argv[1],"exists") == 0) {
		if ( sendReceive(interp,argv,2,"exists") < 0 ) 
			goto error;
	}
	else if (strcmp(argv[1],"removeView") == 0) {
		if ( sendReceive(interp,argv,2,"remove View") < 0 ) 
			goto error;
	}
	else if (strcmp(argv[1],"getViewMappings") == 0) {
		if ( sendReceive(interp,argv,2,"get View Mappings") < 0 ) 
			goto error;
	}
	else if (strcmp(argv[1],"refreshView") == 0) {
		if ( sendReceive(interp,argv,2,"refresh View") < 0 ) 
			goto error;
	}
	else if (strcmp(argv[1],"getWinViews") == 0) {
		if ( sendReceive(interp,argv,2,"Get Win Views") < 0 ) 
			goto error;
	}
	else if (strcmp(argv[1],"getLinkViews") == 0) {
		if ( sendReceive(interp,argv,2,"get Link Views") < 0 ) 
			goto error;
	}
	else if (strcmp(argv[1],"getCurVisualFilter") == 0) {
		if ( sendReceive(interp,argv,2,"get CurVisual Filter") < 0 ) 
			goto error;
	}
	else if (strcmp(argv[1],"getVisualFilters") == 0) {
		if ( sendReceive(interp,argv,2,"get Visual Filters") < 0 ) 
			goto error;
	}
	else if (strcmp(argv[1], "getViewStatistics") == 0) {
		if ( sendReceive(interp,argv,2,"get View Statistics") < 0 ) 
			goto error;
	}
	else if (strcmp(argv[1], "getViewDimensions") == 0) {
		if ( sendReceive(interp,argv,2,"get View Dimensions") < 0 ) 
			goto error;
	}
	else if (strcmp(argv[1],"getViewOverrideColor") == 0) {
		if ( sendReceive(interp,argv,2,"get View Override Color") < 0 ) 
			goto error;
	}
	else if (strcmp(argv[1],"raiseView") == 0) {
		if ( sendReceive(interp,argv,2,"raiseView") < 0 ) 
			goto error;
	}
	else if (strcmp(argv[1],"lowerView") == 0) {
		if ( sendReceive(interp,argv,2,"lowerView") < 0 ) 
			goto error;
	}
	else {
			Tcl_AppendElement(interp,"wrong args");
			goto error;
		}
	}
	else if (argc == 4) {
		
		if (strcmp(argv[1],"writeLine") == 0)	{
			if (sendServer(argv,3) < 0 ){
				Tcl_AppendElement(interp, "Send failed in writeLine");
				goto error;
			}
	}	
	else if (strcmp(argv[1],"open") == 0) {
		if ( sendReceive(interp,argv,3,"open") < 0 ) 
			goto error;
	}
	else if (strcmp(argv[1], "setViewStatistics") == 0) {
		if ( sendReceive(interp,argv,3,"set ViewStatistics") < 0 ) 
			goto error;
	}
	else if (strcmp(argv[1], "setViewDimensions") == 0) {
		if ( sendReceive(interp,argv,3,"set View Dimensions") < 0 ) 
			goto error;
	}
	else if (strcmp(argv[1],"savePixmap") == 0) {
		if ( sendReceive(interp,argv,3,"save Pix Map") < 0 ) 
			goto error;
	}
	else if (strcmp(argv[1],"loadPixmap") == 0) {
		if ( sendReceive(interp,argv,3,"load Pix Map") < 0 ) 
			goto error;
	}	
	else if (strcmp(argv[1],"getAxisDisplay") == 0 ) {
		if ( sendReceive(interp,argv,3,"get Axis Display") < 0 ) 
			goto error;
	}
	else if (strcmp(argv[1],"replaceView") == 0) {
		if ( sendReceive(interp,argv,3,"replace View") < 0 ) 
			goto error;
	}
	else if (strcmp(argv[1],"setCursorSrc") == 0 ) {
		if ( sendReceive(interp,argv,3,"Set Cursor Source") < 0 ) 
			goto error;
	}
	else if (strcmp(argv[1],"setCursorDst") == 0) {
		if ( sendReceive(interp,argv,3,"Set Cursor Destination") < 0 ) 
			goto error;
	}
	else if (strcmp(argv[1],"setPixelWidth") == 0 ) {
		if ( sendReceive(interp,argv,3,"Set Pixel width") < 0 ) 
			goto error;
	}
	else if (strcmp(argv[1],"getAxis") == 0 ) {
		if ( sendReceive(interp,argv,3,"Get axis") < 0 ) 
			goto error;
	}
	else if (strcmp(argv[1],"setAction") == 0) {
		if ( sendReceive(interp,argv,3,"Set Action") < 0 ) 
			goto error;
	}
	else if (strcmp(argv[1],"setLinkFlag") == 0 ) {
		if ( sendReceive(interp,argv,3,"set Link Flag") < 0 ) 
			goto error;
	}
	else if (strcmp(argv[1], "highlightView") == 0) {
		if ( sendReceive(interp,argv,3,"Highlight View") < 0 ) 
			goto error;
	}
	else if (strcmp(argv[1],"get") == 0) {
		if (sendReturnVals(interp, argv,3,"Get")< 0)
			goto error;
	}	
	else if (strcmp(argv[1],"getparam") == 0) {
		if(	sendReturnVals(interp, argv,3,"Get Param")<0)
			goto error;
	}	
	else if (strcmp(argv[1],"insertMapping") == 0) {
		if(	sendReceive(interp, argv,3,"Insert Mapping")<0)
			goto error;
	}	
	else if (strcmp(argv[1],"getMappingLegend") == 0) {
		if ( sendReceive(interp,argv,3,"Get Mapping Legend") < 0 ) 
			goto error;
	}
	else if (strcmp(argv[1],"insertLink") == 0) {
		if ( sendReceive(interp,argv,3,"Insert Link") < 0 ) 
			goto error;
	}
	else if (strcmp(argv[1],"viewInLink") == 0) {
		if ( sendReceive(interp,argv,3,"View in Link") < 0 ) 
			goto error;
	}
	else if (strcmp(argv[1],"unlinkView") == 0) {
		if ( sendReceive(interp,argv,3,"Unlink View") < 0 ) 
			goto error;
	}
	else if (strcmp(argv[1],"insertWindow") == 0) {
		if ( sendReceive(interp,argv,3,"Insert View") < 0 ) 
			goto error;
	}
	else if (strcmp(argv[1],"removeMapping") == 0) {
		if ( sendReceive(interp,argv,3,"Remove Mapping") < 0 ) 
			goto error;
	}
	else {
			Tcl_AppendElement(interp, "wrong args");
			goto error;
		}
	}
	else if (strcmp(argv[1],"swapView") == 0 ) {
		if ( sendReceive(interp,argv,argc-1,"Swap View") < 0 ) 
			goto error;
	}
	else if (strcmp(argv[1],"setAxisDisplay") == 0 ) {
		if ( sendReceive(interp,argv,argc-1,"set Axis Display") < 0 ) 
			goto error;
	}
	else if (strcmp(argv[1],"insertViewHistory") == 0 ) {
		if ( sendReceive(interp,argv,argc-1,"Insert View history") < 0 ) 
			goto error;
	}
	else if (strcmp(argv[1],"markViewFilter") == 0 ) {
		if ( sendReceive(interp,argv,argc-1,"Mark View Filter ") < 0 ) 
			goto error;
	}
	else if (strcmp(argv[1],"setAxis") == 0 ) {
		if (argc != 5) {
			fprintf(stderr,"TkControl:setAxis needs 5 params\n");
			Exit(2);
		}
		if ( sendReceive(interp,argv,5,"set Axis") < 0 ) 
			goto error;
	}
	else if (strcmp(argv[1],"changeParam") == 0) {
		if (sendServer(argv,argc-1) < 0 ){
			Tcl_AppendElement(interp, "Send failed in Change Param");
			goto error;
		}
	}
	else if (strcmp(argv[1],"createInterp") == 0) {
		if ( sendReceive(interp,argv,argc-1,"Create Interpreter") < 0 ) 
			goto error;
	}
	else if (strcmp(argv[1],"create") == 0) {
		if ( sendReceive(interp,argv,argc-1,"Create") < 0 ) 
			goto error;
	}
	else if (strcmp(argv[1],"setDefault") == 0) {
		if (argc < 5) {
			Tcl_AppendElement(interp, "wrong args");
			goto error;
		}
		if (sendServer(argv,argc-1) < 0 ){
			Tcl_AppendElement(interp, "Send failed in Set default");
			goto error;
		}
	}
	else if (argc == 5) {
		if (strcmp(argv[1],"getCreateParam") == 0){
			if(	sendReturnVals(interp, argv,4,"Get Create Params")<0)
				goto error;
		}	
	  else if (strcmp(argv[1], "getItems") == 0) {
		if (sendReceive(interp,argv,4,"get Items") < 0 ){
			Tcl_AppendElement(interp, "Send failed in get Items");
			goto error;
		}
	  }
	  else if (strcmp(argv[1], "setWindowLayout") == 0) {
		if ( sendReceive(interp,argv,4,"Set Window Layout") < 0 ) 
			goto error;
	  }
	  else if (strcmp(argv[1], "saveWindowImage") == 0) {
		if ( sendReceive(interp,argv,4,"Save Window Image") < 0 ) 
			goto error;
	  }
	  else if (strcmp(argv[1], "setViewOverrideColor") == 0) {
		if ( sendReceive(interp,argv,4,"Set View Override Color") < 0 ) 
			goto error;
	  }
	  else if (strcmp(argv[1],"insertMapping") == 0) {
		if ( sendReceive(interp,argv,4,"Insert Mapping") < 0 ) 
			goto error;
	  }
	  else {
	    Tcl_AppendElement(interp, "wrong args");
	    goto error;
	  }
	}
	else if (argc == 6) {
		if (strcmp(argv[1], "setLabel") == 0) {
			if ( sendReceive(interp,argv,5,"Set Label") < 0 ) 
			goto error;
		}
		else if (strcmp(argv[1], "setWindowLayout") == 0) {
			if ( sendReceive(interp,argv,5,"Set Window Layout") < 0 ) 
				goto error;
		}
		else {
			Tcl_AppendElement(interp, "wrong args");
			goto error;
		}
	}
	else if (argc == 7 ) {
		if (strcmp(argv[1],"setFilter") == 0) {
		if ( sendReceive(interp,argv,6,"Set Filter") < 0 ) 
			goto error;
		}
	}
	else {
		Tcl_AppendElement(interp, "wrong args");
		goto error;
	}
	
#if DEBUG
	printf("Result: %s\n",interp->result);
#endif
	return TCL_OK;
error:
#if DEBUG
	printf("ERROR: %s\n",interp->result);
#endif
	return TCL_ERROR;
}

void ClientControl::Run()
{
  while(1){
   Tk_DoOneEvent(TK_X_EVENTS | TK_FILE_EVENTS 
	 | TK_IDLE_EVENTS | TK_DONT_WAIT);
    CheckRead();
 }

}

void ClientControl::CheckRead()
{
	
	static char buff[100];
	u_short size;
	u_short errorFlag;

	// read the command off the pipe and execute it!!
	if (recv(controlFd,(char *)&errorFlag,sizeof(u_short),0) < 0 )
		if (errno == EWOULDBLOCK )
			return ;
		else
			perror("read Failed");
	errorFlag = ntohs(errorFlag);

	if (recv(controlFd,(char *)&size,sizeof(u_short),0) < 0 )
		if (errno == EWOULDBLOCK){
			while(errno == EWOULDBLOCK )
				if (recv(controlFd,(char *)&size,sizeof(u_short),0) >= 0 )
					break;
		}			
		else
			perror("Read failed");	

	size = ntohs(size);

	// Ignoring size for future exapnsion... Use size as a parameter to 
	// initialize the number of characters pointed to by buff.

	if (recv(controlFd,buff,size,0) < 0 )
		if (errno == EWOULDBLOCK){
			while(errno == EWOULDBLOCK )
				if (recv(controlFd,buff,size,0) >= 0 )
					break;
		}
		else
			perror("Read failed");	
	
	if ( errorFlag == 0)	
			(void) Tcl_Eval(_interp,buff);
		
}
		

#if 0
void ClientControl::SetBusy()
{
  if (++_busy == 1)
    (void)Tcl_Eval(_interp, "ChangeStatus 1");
}

void ClientControl::SetIdle()
{
  DOASSERT(_busy > 0, "Control panel unexpectedly busy");

  if (--_busy == 0)
    (void)Tcl_Eval(_interp, "ChangeStatus 0");
}

Boolean ClientControl::IsBusy()
{
  return (_busy > 0);
}

void ClientControl::ExecuteScript(char *script)
{
  char cmd[256];
  sprintf(cmd, "ExecuteScript %s", script);
  (void)Tcl_Eval(_interp, cmd);
}
#endif

void ClientControl::Exit(int num)
{
	close(socketFd);
	//DoAbort("ERROR");
	exit(num);
 }

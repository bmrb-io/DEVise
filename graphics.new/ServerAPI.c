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
//#define SERV_ANYPORT

#include <stdio.h>
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include "ServerAPI.h"
#include "ClassDir.h"
#include "ViewGraph.h"
#include "View.h"
#include "TData.h"
#include "TDataMap.h"
#include "Init.h"
#include "Util.h"
#include "VisualArg.h"
#include "VisualLink.h"
#include "FilterQueue.h"
#include "ParseCat.h"
#include "Action.h"
#include "AttrList.h"
#include "MapInterpClassInfo.h"
#include "Parse.h"
#include "QueryProc.h"
#include "Cursor.h"
#include "Group.h"
#include "GroupDir.h"
#include "ViewLayout.h"
#include "ViewKGraph.h"
#include <sys/socket.h>
#include <netdb.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <arpa/inet.h>
//#include "socket_missing.h"


extern GroupDir *gdir;
ViewKGraph *vkg = NULL;
#if 0
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
#endif
ControlPanel::Mode TkControlPanel::_mode = ControlPanel::DisplayMode;
MapInterpClassInfo *TkControlPanel::_interpProto = NULL;

ControlPanel *GetTkControl()
{
  return new TkControlPanel();
}

TkControlPanel::TkControlPanel(int socketID )
{

	ControlPanel::_controlPanel = (ControlPanel *) this;
  _interpProto = new MapInterpClassInfo();
  
  View::InsertViewCallback(this);
	
   socketFd = socketID;
  _busy = false;
  _restoring = false;
  _template = false;

  _fileName = (char *)malloc(1);
  _fileName[0] = '\0';
  _fileAlias = (char *)malloc(1);
  _fileAlias[0] = '\0';

  _winName = (char *)malloc(20);
  strcpy(_winName,"DEViseWn0");
  _gdataName = (char *)malloc(1);
  _gdataName[0] = '\0';
  _viewName = (char *)malloc(1);
  _viewName[0] = '\0';
  _sessionName = CopyString(Init::SessionName());
  _argv0 = CopyString(Init::ProgName());

   if (socketFd == -1){

		socketFd = socket(AF_INET,SOCK_STREAM,0);
		DOASSERT(socketFd != -1,"Socket in Server ");

		// Now bind these to the address..

		char servName[30];

		struct sockaddr_in servAddr;

		bzero(&servAddr,sizeof(servAddr));

		servAddr.sin_family = AF_INET;
#ifdef SERV_ANYPORT
		servAddr.sin_port   = htons(0);
#else
		servAddr.sin_port   = htons(SERV_PORT_NUM);
#endif
		servAddr.sin_addr.s_addr   = htonl(INADDR_ANY);

		DOASSERT((bind(socketFd,(struct sockaddr *)&servAddr,
				sizeof(struct sockaddr)) >= 0 )," Cannot bind to address");
	
#ifdef SERV_ANYPORT	

		// Write the server data....
		struct sockaddr_in tempAddr;
    	bzero(&tempAddr,sizeof(struct sockaddr));
    	int len = sizeof(struct sockaddr);

   		if (getsockname(socketFd,(sockaddr *)&tempAddr,&len) < 0)
        	perror("Get sockname");
		
		printf(" The port number is %u\n",tempAddr.sin_port);
		// Open the server.dat file and write to it..
		FILE *fp;
		char server[150];
		char *envpath = getenv("DEVISE_LIB");
		if (envpath){
			strcpy(server,envpath);
			strcat(server,"/");
			strcat(server,"server.dat");
		}
		else
			strcpy(server,"server.dat");
		fp = fopen(server,"w");
		DOASSERT(fp ,"server.dat couldnt be created");
		char host[30];	
		DOASSERT(gethostname(host,30)>=0,"Gethostname failed");	
		int temp = ntohs(tempAddr.sin_port);
		if ( fprintf(fp,"%s %d \n", host,(int)temp) < 0)
			perror("Fprintf");	
		fflush(fp);
		fclose(fp);

#endif	
		// Now make this non - blocking and proceed..	
		DOASSERT((listen(socketFd,5)>= 0),"Listen failed");
#ifndef PENTIUM
		DOASSERT(fcntl(socketFd,F_SETFL,FNDELAY) >= 0 ,"Fcntl failed") ;
#else
		DOASSERT(fcntl(socketFd,F_SETFL,O_NDELAY) >= 0 ,"Fcntl failed") ;
#endif
  }
  else
	DOASSERT(fcntl(socketFd,F_SETFL,0) >= 0 ,"Fcntl failed") ;
  /* register with dispatcher to be always informed of when to run */
  //Dispatcher::Current()->Register(this, 10,AllState, true,-1);
  Dispatcher::Current()->Register(this, 10,AllState, true,socketFd);



}
/* start session */
void TkControlPanel::StartSession()
{
  printf("DEVise Data Visualization Software\n");
  printf("(c) Copyright 1992-1996\n");
  printf("By the DEVise Development Group\n");
  printf("All Rights Reserved.\n");
  printf("\n");

#if 0 
  Tcl_LinkVar(_interp, "fileName", (char *)&_fileName, TCL_LINK_STRING);
  Tcl_LinkVar(_interp, "fileAlias", (char *)&_fileAlias, TCL_LINK_STRING);
  Tcl_LinkVar(_interp, "gdataName", (char *)&_gdataName, TCL_LINK_STRING);
  Tcl_LinkVar(_interp, "windowName", (char *) &_winName, TCL_LINK_STRING);
  Tcl_LinkVar(_interp, "viewName", (char *) &_viewName, TCL_LINK_STRING);
  Tcl_LinkVar(_interp, "sessionName", (char *)&_sessionName, TCL_LINK_STRING);
  Tcl_LinkVar(_interp, "argv0", (char *)&_argv0, TCL_LINK_STRING);
  Tcl_LinkVar(_interp, "restoring", (char *)&_restoring, TCL_LINK_INT);
  Tcl_LinkVar(_interp, "template", (char *)&_template, TCL_LINK_INT);

  /* Create a new tcl command for control panel */
  Tcl_CreateCommand(_interp, "DEVise", ControlCmd, this, NULL);

  /* Create a new tcl command for ISSM stock data */
  Tcl_CreateCommand(_interp, "issm_extractStocks", extractStocksCmd, 0, 0);

  /* Create a new tcl command for Compustat data */
  Tcl_CreateCommand(_interp, "cstat_extract", comp_extract, 0, 0);

  /* Create a new tcl command for CRSP data */
  Tcl_CreateCommand(_interp, "crsp_extract", crsp_extract, 0, 0);

  /* Create a new tcl command for SEQ data */
  Tcl_CreateCommand(_interp, "seq_extract", seq_extract, 0, 0);

  /* Create a new tcl command for WWW data */
  Tcl_CreateCommand(_interp, "www_extract", www_extract, 0, 0);

  char *controlFile = "control.tk";
  if (Init::BatchFile()) 
    controlFile = "batch.tcl";

  char *envPath = getenv("DEVISE_LIB");
  char *control;
  char buf[256];
  if (envPath) {
    sprintf(buf, "%s/%s", envPath, controlFile);
    control = buf;
  } else
    control = controlFile;

  printf("Control panel file is: %s\n", control);

  int code = Tcl_EvalFile(_interp,control);
  if (code != TCL_OK) {
    fprintf(stderr,"%s\n", _interp->result);
    Exit::DoExit(1);
  }

  if (Init::Restore()) {
    /* restore session */
    _restoring = true;
    int code = Tcl_EvalFile(_interp,_sessionName);
    _restoring = false;
    if (code != TCL_OK) {
      fprintf(stderr,"Can't restore session file %s\n",_sessionName);
      fprintf(stderr,"%s\n", _interp->result);
      Exit::DoExit(1);
    }
  }
#endif
}

char * TkControlPanel::SessionName()
{
  return _sessionName;
}
void TkControlPanel::SetSessionName(char *name)
{
  /*XXX: This is a memory leak */
  _sessionName = CopyString(name);
}

int ServerControl::SendReturnVals(int numArgs, char **argv)
{
	
  u_short errorFlag = 0; // No error
  u_short size;
  
  errorFlag = htons(errorFlag);
   
  if (send(socketFd,(char *)&errorFlag,sizeof(u_short),0) < 0)
		   return -1;
 
  u_short num = (u_short) numArgs;
  num = htons(num);

  if (send(socketFd,(char *)&num,sizeof(u_short),0) < 0)
		   return -1;
	
	if (numArgs > 0 )
	for(int i = 0 ; i < numArgs; i++){

        if (argv[i] == 0)
			size = 0;
		else
			size = (u_short) (strlen(argv[i])+1);
		if (send(socketFd,(char *)&size,sizeof(u_short),0) < 0)
				return -1;

		if ( size > 0)
			if (send(socketFd,argv[i],size,0) < 0)
				return -1;

	}

	if (errorFlag )
	return -1;
		else
	return 1;


}


int ServerControl::sendClient(enum FLAG flag, char * result)
{

		
  u_short errorFlag = 0; // No error
  u_short size;
  
  if (flag == OK ) 
	errorFlag = htons(0);
  else
	 errorFlag = htons(1);
   
  if (send(socketFd,(char *)&errorFlag,sizeof(u_short),0) < 0)
		   return -1;
 
  if ( result == 0)
	size = 0;
  else
	size = (u_short) (strlen(result)+1);

#if 0
  if (size == 1 ) 
	size = 0;
#endif

  if (send(socketFd,(char *)&size,sizeof(u_short),0) < 0)
		return -1;
	
  if (size > 0)
 	 if (send(socketFd,(char *)result,size,0) < 0)
		return -1;
	
	if (errorFlag )
		return -1;
	else
		return 1;
}
  	
int ServerControl::sendControlClient(enum FLAG flag, char * result)
{

		
  u_short errorFlag = 0; // No error
  u_short size;
  
  if (flag == OK ) 
	errorFlag = htons(0);
  else
	 errorFlag = htons(1);
   
  if (send(controlFd,(char *)&errorFlag,sizeof(u_short),0) < 0)
		   return -1;
 
  size = (u_short) (strlen(result)+1);
  if (size == 1 ) 
	size = 0;

  if (send(controlFd,(char *)&size,sizeof(u_short),0) < 0)
		return -1;
	
  if (size > 0)
 	 if (send(controlFd,result,size,0) < 0)
		return -1;
	
	if (errorFlag )
		return -1;
	else
		return 1;
}
ControlPanel::Mode TkControlPanel::GetMode()
{
  return _mode;
}

Boolean TkControlPanel::Restoring()
{
  return _restoring;
}

void TkControlPanel::DoAbort(char *reason)
{
  char cmd[256];
  sprintf(cmd, "AbortProgram {%s}", reason);
  sendControlClient(OK,reason);
  exit(0);
 // (void)Tcl_Eval(_interp, cmd);
}

int ServerControl::ControlCmd(int argc, char *argv[]) {
	
	ServerControl *control = this;
	ClassDir *classDir = control->GetClassDir();
	int code;
	int numArgs; char **args;
	char *name;
	char result[1000];
	
	result[0] = '\0';
#if DEBUG
	printf("%s (%d)",argv[0],argc);
	for(int i = 0; i < argc; i++)
		printf("<%s>",argv[i]);
	printf("\n");
	
#endif
	if (argc == 0) {
		sendClient(ERROR,"wrong args");
		goto error;
	}
	else if (argc == 1) {
		if (strcmp(argv[0],"date") == 0 ) {
			time_t tm = time((time_t *)0);
			sendClient(OK,DateString(tm));
		}
		else if (strcmp(argv[0],"clearQP") == 0) {
			classDir->DestroyTransientClasses();
			QueryProc::Instance()->ClearQueries();
			ClearCats();
		}
		else if (strcmp(argv[0],"clearTopGroups") == 0) {
			delete gdir;
			gdir = new GroupDir();
		}
		else if (strcmp(argv[0],"printDispatcher") == 0) {
			sendClient(OK,"Dispatcher::Current()->Print()");
		}
		else if (strcmp(argv[0],"catFiles") == 0) {
			CatFiles(numArgs, args);
			SendReturnVals(numArgs, args);
		}
		else if (strcmp(argv[0],"exit")== 0) {
			sendClient(OK,"QueryProc::Instance()->PrintStat()");
			exit(0);
			//control->DoQuit();
		}
		else {
			sendClient(ERROR,"wrong args");
			goto error;
		}
	}
	else if (strcmp(argv[0], "showkgraph") == 0) {
	    if ((atoi(argv[1]) == 1) || (!vkg))  {
		/* Create a new ViewKGraph object */
		/* Dont delete the previous vkg since we still want it to
		   continuously display the previously defined KGraph */
		/* Who eventually deletes it ?? */
		/* delete vkg; */
		vkg = new ViewKGraph();
	      }
	    int i = 0;
	    /* Number of views */
	    int nview = argc - 4;
	    ViewGraph **vlist = (ViewGraph **)malloc(nview*sizeof(ViewGraph *));
	    for (i = 0; i < nview; i++) {
	      vlist[i] = (ViewGraph *)classDir->FindInstance(argv[5+i]);
	      if (vlist[i] == NULL) {
		  sendClient(ERROR,"Cannot find view");
		  goto error;
		}
	    }
	    vkg->Init();
	    /* Add these to the ViewKGraph class and display */
	    if (vkg->AddViews(vlist, nview, argv[3]) == false) {
		sendClient(ERROR,"Could not add views to ViewKGraph");
		goto error;
	      }
		  
	    if (vkg->Display(atoi(argv[2])) == false) {
		sendClient(ERROR,"Could not display the KGraph");
		goto error;
	      }
		  sendClient(OK,"OK");
	    free(vlist);
	  }
	   else if (strcmp(argv[0],"createMappingClass") == 0) {
			 ClassInfo *classInfo =
				 _interpProto->CreateWithParams(argc-1,&argv[1]);
		   if (!classInfo) {
			   sendClient(ERROR,"Can't create mapping class");
			   goto error;
		   }
		   sendClient(OK,classInfo->ClassName());
		 RegisterClass(classInfo, true);
	 }
	else if (argc == 2) {
		if (strcmp(argv[0],"setWindowName") == 0){
			strcpy(_winName,argv[1]);
		}
		else if (strcmp(argv[0],"invalidatePixmap") == 0 ) {
			View *vg = (View *)classDir->FindInstance(argv[1]);
			if (vg == NULL) {
				sendClient(ERROR,"Can't find view");
				goto error;
			}
			vg->InvalidatePixmaps();
			sendClient(OK,"OK");
		}
		else if (strcmp(argv[0],"createMappingClass") == 0) {
		  ClassInfo *classInfo = 
		    _interpProto->CreateWithParams(argc-1,&argv[1]);
		  if (!classInfo) {
		    sendClient(ERROR,"Can't create mapping class");
		    goto error;
		  }
		  sendClient(OK,classInfo->ClassName());
		  RegisterClass(classInfo, true);
		}
		else if (strcmp(argv[0],"readLine") == 0 ) {
			FILE *file = (FILE *)atol(argv[1]);
			(void)fgets(result, 256, file);
			int len = strlen(result);
			if (len > 0 && result[len-1] == '\n')
					result[len-1] = '\0';
			sendClient(OK,result);
		}
		else if (strcmp(argv[0],"close") == 0) {
			FILE *file = (FILE *)atol(argv[1]);
			fclose(file);
		}
		else if (strcmp(argv[0],"isMapped") == 0) {
			View *vg = (View *)classDir->FindInstance(argv[1]);
			if (vg == NULL) {
				sprintf(result,"Can't find view %s in isMapped", argv[1]);
				sendClient(ERROR,result);
				goto error;
			}
			sprintf(result,"%d",(vg->Mapped()? 1: 0 ));
			sendClient(OK,result);
		}
		else if (strcmp(argv[0],"getLabel") == 0) {
			View *vg = (View *)classDir->FindInstance(argv[1]);
			if (vg == NULL) {
			  sprintf(result,"Can't find view %s in getLabelParam", argv[1]);
			  sendClient(ERROR,result);
			  goto error;
			}
			Boolean occupyTop;
			int extent;
			char *name;
			vg->GetLabelParam(occupyTop, extent, name);
			sprintf(result, "%d %d {%s}",
				(occupyTop ? 1 : 0), extent,
				(name ? name : ""));
			sendClient(OK,result);
		}
		else if (strcmp(argv[0],"tdataFileName") == 0) {
			TData *tdata = (TData *)classDir->FindInstance(argv[1]);
			if (tdata == NULL) {
				sendClient(ERROR,"Can't find tdata in tdataFileName");
				goto error;
			}
			sprintf(result,"%s",tdata->GetName());\
			sendClient(OK,result);
		}
		else if (strcmp(argv[0],"getViewWin") == 0) {
			View *view = (View *)classDir->FindInstance(argv[1]);
			if (view == NULL) {
				sendClient(ERROR,"Can't find view in getViewWin");
				goto error;
			}
			ViewWin *viewWin = view->GetParent();
			if (viewWin == NULL)
				strcpy(result, "");
			else sprintf(result,"%s",viewWin->GetName());
			sendClient(OK,result);
		}
		else if (strcmp(argv[0],"clearViewHistory") == 0) {
			View *view = (View *)classDir->FindInstance(argv[1]);
			if (view == NULL) {
				sendClient(ERROR,"Can't find cursor in getCursorViews");
				goto error;
			}
			FilterQueue *queue = view->GetHistory();
			queue->Clear();
			sendClient(OK,"OK");
		}
		else if (strcmp(argv[0],"getCursorViews") == 0) {
			DeviseCursor *cursor = (DeviseCursor *)
				classDir->FindInstance(argv[1]);
			if (cursor == NULL) {
				sendClient(ERROR,"Can't find cursor in getCursorViews");
				goto error;
			}
			View *src = cursor->GetSource();
			View *dst = cursor->GetDst();
			char *name1, *name2;
			if (src != NULL)
				name1 = src->GetName();
			else name1 = "";
			if (dst != NULL)
				name2 = dst->GetName();
			else name2 = "";

			sprintf(result,"{%s} {%s}",name1,name2);
			sendClient(OK,result);
		} 
		else if (strcmp(argv[0],"getMappingTData") == 0) {
			TDataMap *map= (TDataMap *)classDir->FindInstance(argv[1]);
			if (map== NULL) {
				sendClient(ERROR,"Can't find mapping in getPixelWidth");
				goto error;
			}
			TData *tdata = map->GetTData();
			sprintf(result,"%s",classDir->FindInstanceName(tdata));
			sendClient(OK,result);
		}
		else if (strcmp(argv[0],"openSession") == 0 ) {
		/*	(void)Tcl_Eval(interp,"set template 0");
			control->_restoring = true;
			code = Tcl_EvalFile(interp,argv[1]);
			control->_restoring = false;
			if (code != TCL_OK) {
				sendClient(ERROR,"can't restore session file\n");
				goto error;
			}*/
			control->SetSessionName(argv[1]);
		}
		else if (strcmp(argv[0],"openTemplate") == 0 ) {
		/*	(void)Tcl_Eval(interp,"set template 1");
			control->_restoring = true;
			code = Tcl_EvalFile(interp,argv[1]);
			control->_restoring = false;
			if (code != TCL_OK) {
				interp->result = "can't restore template file\n";
				goto error;
			} */
			control->SetSessionName(argv[1]);
		}
		else if (strcmp(argv[0],"destroy") == 0 ) {
			classDir->DestroyInstance(argv[1]);
		}
		else if (strcmp(argv[0],"parseDateFloat") == 0 ) {
			char buf[80];
			double val;
			(void)ParseFloatDate(argv[1], val);
			sprintf(buf,"%f",val);
			sendClient(OK,buf);
		}
		else if (strcmp(argv[0],"isInterpretedGData") == 0 ) {
			TDataMap *map= (TDataMap *)classDir->FindInstance(argv[1]);
			if (map== NULL) {
				sendClient(ERROR,"Can't find mapping in getPixelWidth");
				goto error;
			}
			if (map->IsInterpreted())
				strcpy(result, "1");
			else strcpy(result, "0");
			sendClient(OK,result);
		}
		else if (strcmp(argv[0],"isInterpreted") == 0 ) {
			int *isInterp = (int *)classDir->UserInfo("mapping", argv[1]);
			if (isInterp== NULL) {
				/*
				interp->result = "Can't find mapping in isInterpreted";
				goto error;
				*/
				strcpy(result, "0");
			}
			else {
				if (*isInterp)
					strcpy(result, "1");
				else strcpy(result, "0");
			}
			sendClient(OK,result);
		}
		else if (strcmp(argv[0],"getPixelWidth") == 0 ) {
			TDataMap *map= (TDataMap *)classDir->FindInstance(argv[1]);
			if (map== NULL) {
				sendClient(ERROR,"Can't find mapping in getPixelWidth");
				goto error;
			}
			char buf[80];
			sprintf(buf,"%d", map->GetPixelWidth());
			sendClient(OK,buf);
			//Tcl_AppendElement(interp,buf);
		}
		else if (strcmp(argv[0], "getTopGroups") == 0) {
		  gdir->top_level_groups(result, argv[1]);
		  sendClient(OK,result);
		}
		else if (strcmp(argv[0], "getWindowLayout") == 0) {
		  ViewLayout *layout = (ViewLayout *)classDir->FindInstance(argv[1]);
		  if (!layout) {
		  fprintf(stderr,"TkControl:cmd getWindowLayout can't find window %s\n",
			    argv[1]);
			sendClient(ERROR,"TkControl:cmd getWindowLayout can't find window");
		    Exit::DoExit(2);
		  }
		  int v, h;
		  Boolean stacked;
		  layout->GetPreferredLayout(v, h, stacked);
		  sprintf(result, "%d %d %d", v, h,
			  (stacked ? 1 : 0));
			sendClient(OK,result);
		}
		else if (strcmp(argv[0],"getSchema") == 0) {
	
			char **args;
			TData *tdata = (TData *)classDir->FindInstance(argv[1]);
			if (tdata == NULL) {
			  sendClient(ERROR,"Can't find tdata in getSchema");
			  goto error;
			}
			AttrList *attrList = tdata->GetAttrList();
			if (!attrList) {
			  sendClient(ERROR,"NULL attribute list");
			  goto error;
			}

#ifdef DEBUG
			printf("getSchema: \n");
			attrList->Print();
#endif
			char attrBuf[160];
			int numAttrs = attrList->NumAttrs(); // ********///
			//Tcl_AppendElement(interp,"recId int 1 0 0 0 0");
			//strcpy(result,"");
			//strcpy(result,"recId int 1 0 0 0 0 ");
			args = new (char*) [numAttrs+1];
			args[0] = new char[25];
			strcpy(args[0],"recId int 1 0 0 0 0");

			for(int i = 0; i < numAttrs; i++) {
			  AttrInfo *info = attrList->Get(i);
//#ifdef DEBUG
			  printf("inserting %s\n",info->name);
//#endif
			  switch(info->type) {
			  case FloatAttr:
			    sprintf(attrBuf,"%s float %d %d %g %d %g",
				    info->name, info->isSorted,
				    info->hasHiVal,
				    (info->hasHiVal ? info->hiVal.floatVal : 100),
				    info->hasLoVal,
				    (info->hasLoVal ? info->loVal.floatVal : 0));
			    break;
			  case DoubleAttr:
			    sprintf(attrBuf,"%s double %d %d %g %d %g",
				    info->name, info->isSorted,
				    info->hasHiVal,
				    (info->hasHiVal ? info->hiVal.doubleVal : 100),
				    info->hasLoVal,
				    (info->hasLoVal ? info->loVal.doubleVal : 0));
			    break;
			  case StringAttr:
			    sprintf(attrBuf,"%s string %d 0 0 0 0", info->name,
				    info->isSorted);
			    break;
			  case IntAttr:
			    sprintf(attrBuf,"%s int %d %d %ld %d %ld",
				    info->name, info->isSorted,
				    info->hasHiVal,
				    (long)(info->hasHiVal ? info->hiVal.intVal : 100),
				    info->hasLoVal,
				    (long)(info->hasLoVal ? info->loVal.intVal : 0));
			    break;
			  case DateAttr:
			    sprintf(attrBuf,"%s date %d %d %ld %d %ld",
				    info->name, info->isSorted,
				    info->hasHiVal,
				    (long)(info->hasHiVal ? info->hiVal.dateVal : 100),
				    info->hasLoVal,
				    (long)(info->hasLoVal ? info->loVal.dateVal : 0));
			    break;
			  default:
			    printf("TkControl:unknown attr\n");
			    Exit::DoExit(1);
			  }
			args[i+1] = new char[strlen(attrBuf) + 1];
			strcpy(args[i+1],attrBuf);

			// strcat(result,attrBuf);
			}
			 SendReturnVals(numAttrs + 1, args) ;
			 for(int i = 0; i < numAttrs + 1;i++)
				delete []args;
			delete []args;

			 // sendClient(OK,result);
	        }
		else if (strcmp(argv[0],"getAction") == 0 ) {
			ViewGraph *view = (ViewGraph *)classDir->FindInstance(argv[1]);
			if (view == NULL) {
				sendClient(ERROR,"Can't find view");
				goto error;
			}
			Action *action = view->GetAction();
			if (action == NULL || strcmp(action->GetName(),"") == 0 ||
				strcmp(action->GetName(),"default") == 0)
				strcpy(result , "");
			else strcpy(result,action->GetName());
			sendClient(OK,result);
		}
		else if (strcmp(argv[0],"getLinkFlag") == 0 ) {
			VisualLink *link = (VisualLink *)classDir->FindInstance(argv[1]);
			if (link == NULL) {
				sendClient(ERROR,"can't find link");
				goto error;
			}
			sprintf(result,"%d",link->GetFlag());
			sendClient(OK,result);
		}
		else if (strcmp(argv[0],"importFileType") == 0) {
			if ((name=ParseCat(argv[1])) == NULL) {
				strcpy(result , "");
				sendClient(ERROR,result);
				goto error;
			}
			else strcpy(result, name);
			sendClient(OK,result);
		}
		else if (strcmp(argv[0],"changeableParam") == 0) {
			Boolean changeable = classDir->Changeable(argv[1]);
			if (changeable)
				strcpy(result , "1");
			else strcpy(result , "0");
			sendClient(OK,result);
		}
		else if (strcmp(argv[0],"getInstParam") == 0) {
			/* get current parameters for instance */
			/*printf("getInstParam %s\n", argv[1]); */
			classDir->GetParams(argv[1],numArgs, args);
			/* printf("getInstParam %s: got %d args\n",argv[1], numArgs);*/
			SendReturnVals(numArgs, args);
		}
		else if (strcmp(argv[0],"tcheckpoint") == 0) {
			/* checkpoint all tdata */
			TData *tdata;
			control->SetBusy();
			if ((tdata=(TData *)classDir->FindInstance(argv[1])) != NULL)
				tdata->Checkpoint();
			control->SetIdle();
		}
		else if (strcmp(argv[0],"get") == 0) {
			classDir->ClassNames(argv[1],numArgs, args);
#if 0
			printf("get %s, got %d args\n", argv[1], numArgs);
			for(int ind = 0; ind < numArgs; ind++) {
			  printf("%s\n", args[ind]);
			}
#endif
			SendReturnVals(numArgs, args);
		} else if (strcmp(argv[0],"changeMode") == 0) {
			if (strcmp(argv[1],"0") == 0) {
			  if( _mode != ControlPanel::DisplayMode) {
			    /* Set display mode  and make all views refresh*/
			    _mode = ControlPanel::DisplayMode;
			  ControlPanel::Instance()->ReportModeChange(_mode);
			  }
			} else if(_mode != ControlPanel::LayoutMode) {
			  /* set layout mode */
			  _mode = ControlPanel::LayoutMode;
			  ControlPanel::Instance()->ReportModeChange(_mode);
			}
		}
		else if (strcmp(argv[0],"exists") == 0) {
			void *ptr = classDir->FindInstance(argv[1]);
			if ( ptr == (void *)NULL )
				strcpy(result , "0");
			else strcpy(result , "1");
			sendClient(OK,result);
		} 
		else if (strcmp(argv[0],"removeView") == 0) {
			ViewGraph *view = (ViewGraph *)classDir->FindInstance(argv[1]);
			if (view == NULL) {
				sendClient(ERROR,"can't find view");
				goto error;
			}
			if (!view->Mapped()) {
				sendClient(ERROR,"view not in any window");
				goto error;
			}
			view->DeleteFromParent();
			sendClient(OK,"OK");
		}
		else if (strcmp(argv[0],"getViewMappings") == 0) {
			/*
			printf("finding view '%s'\n",argv[1]);
			*/
			ViewGraph *view = (ViewGraph *)classDir->FindInstance(argv[1]);
			if (view == NULL) {
				sendClient(ERROR,"can't find view");
				goto error;
			}
			/*
			for (view->InitMappingIterator(); view->MoreMapping(); ) {
				TDataMap *map = view->NextMapping()->map;
				strcat(result,map->GetGDataName()); 
			}
			view->DoneMappingIterator();
			sendClient(OK,result);/// ////
			*/

			int index = view->InitMappingIterator();
            while(view->MoreMapping(index)) {
              TDataMap *map = view->NextMapping(index)->map;
             // Tcl_AppendElement(interp,map->GetGDataName());
			strcat(result,map->GetGDataName()); //***//
            }
            view->DoneMappingIterator(index);
			sendClient(OK,result);
		}
		else if (strcmp(argv[0],"refreshView") == 0) {
			/*
			printf("finding view '%s'\n",argv[1]);
			*/
			View *view = (View *)classDir->FindInstance(argv[1]);
			if (view == NULL) {
				sendClient(ERROR,"can't find view");
				goto error;
			}
			view->Refresh();
			sendClient(OK,"OK");
		}
		else if (strcmp(argv[0],"getWinViews") == 0) {
			ViewWin *win = (ViewWin*)classDir->FindInstance(argv[1]);
			if (win == NULL) {
				sendClient(ERROR,"can't find window");
				goto error;
			}
			int index;
			for(index = win->InitIterator(); win->More(index); ) {
				ViewWin *view = (ViewWin *)win->Next(index);
				strcat(result, view->GetName());
			}
			win->DoneIterator(index);
			sendClient(OK,result);
		} else if (strcmp(argv[0],"getLinkViews") == 0) {
			VisualLink *link = (VisualLink *)classDir->FindInstance(argv[1]);
			if (link == NULL) {
				sendClient(ERROR,"can't find link");
				goto error;
			}
			int index;
			for (index=link->InitIterator(); link->More(index); ) {
				ViewWin *view = (ViewWin *)link->Next(index);
				strcat(result, view->GetName());
			}
			link->DoneIterator(index);
			sendClient(OK,result);
		}
		else if (strcmp(argv[0],"getCurVisualFilter") == 0) {
			View *view = (View *)classDir->FindInstance(argv[1]);
			if (view == NULL) {
				sendClient(ERROR,"can't find view");
				goto error;
			}
			VisualFilter *filter = view->GetVisualFilter();
			char buf[256];
			sprintf(buf,"%.2f", filter->xLow);
			strcat(result, buf);

			sprintf(buf,"%.2f", filter->yLow);
			strcat(result, buf);

			sprintf(buf,"%.2f", filter->xHigh);
			strcat(result, buf);

			sprintf(buf,"%.2f", filter->yHigh);
			strcat(result, buf);
			sendClient(OK,"OK");
		}
		else if (strcmp(argv[0],"getVisualFilters") == 0) {
			View *view = (View *)classDir->FindInstance(argv[1]);
			if (view == NULL) {
				sendClient(ERROR,"can't find view");
				goto error;
			}
			FilterQueue *queue ;
			queue = view->GetHistory();
			int i;
			strcpy(result,"");
			for (i=0; i < queue->Size(); i++) {
				VisualFilter filter;
				queue->Get(i,filter);
				char buf[256];
				char xLowBuf[40],xHighBuf[40],yLowBuf[40],yHighBuf[40];
				if (view->GetXAxisAttrType() == DateAttr) {
					sprintf(xLowBuf,"%s",DateString(filter.xLow));
					sprintf(xHighBuf,"%s",DateString(filter.xHigh));
				}
				else {
					sprintf(xLowBuf,"%.2f",filter.xLow);
					sprintf(xHighBuf,"%.2f",filter.xHigh);
				}
				
				if (view->GetYAxisAttrType() == DateAttr) {
					sprintf(yLowBuf,"%s",DateString(filter.yLow));
					sprintf(yHighBuf,"%s",DateString(filter.yHigh));
				}
				else {
					sprintf(yLowBuf,"%.2f",filter.yLow);
					sprintf(yHighBuf,"%.2f",filter.yHigh);
				}

				sprintf(buf,"{{%s} {%s} {%s} {%s} %d} ",xLowBuf, yLowBuf,
					xHighBuf, yHighBuf, filter.marked);
				strcat(result, buf);
			}
			sendClient(OK,result);
		}
		else if (strcmp(argv[0], "getViewStatistics") == 0) {
		    ViewGraph *vg = (ViewGraph *)classDir->FindInstance(argv[1]);
		    if (vg == NULL) {
			sendClient(ERROR,"Can't find view in GetViewStatistics");
			goto error;
		    }
		    /* Return status of statistics display */
		    sendClient(OK,vg->GetDisplayStats());
		}
		else if (strcmp(argv[0], "getViewDimensions") == 0) {
		    View *vg = (View *)classDir->FindInstance(argv[1]);
		    if (vg == NULL) {
			sendClient(ERROR,"Can't find view in GetViewDimensions");
			goto error;
		    }
		    /* Return status of statistics display */
		    sprintf(result, "%d", vg->GetNumDimensions());
			sendClient(OK,result);
		}
		else if (strcmp(argv[0],"getViewOverrideColor") == 0) {
		  View *view = (View *)classDir->FindInstance(argv[1]);
		  if (!view) {
		    sendClient(ERROR,"Can't find view in getViewOverrideColor");
		    goto error;
		  }
		  Boolean active;
		  Color color = view->GetOverrideColor(active);
		  sprintf(result, "%d %d", (active ? 1 : 0),
			  (int)color);
		  sendClient(OK,result);
		}
		else if (strcmp(argv[0],"raiseView") == 0) {
		  View *view = (View *)classDir->FindInstance(argv[1]);
		  if (view == NULL) {
		    sendClient(ERROR,"Can't find view in raiseView");
		    goto error;
		  }
		  view->Raise();
		  sendClient(OK,"oK");
		}
		else if (strcmp(argv[0],"lowerView") == 0) {
		  View *view = (View *)classDir->FindInstance(argv[1]);
		  if (view == NULL) {
		    sendClient(ERROR,"Can't find view in lowerView");
		    goto error;
		  }
		  view->Lower();
		  sendClient(OK,"OK");
		}
		else {
			sendClient(ERROR,"wrong args");
			goto error;
		}
	}
	else if (argc == 3) {
		if (strcmp(argv[0],"writeLine") == 0) {
			FILE *file = (FILE *)atol(argv[2]);
			/*
			printf("writeLIne %s file is %d\n", argv[1], argv[2]);
			*/
			fputs(argv[1],file);
			fputs("\n",file);
		}
		else if (strcmp(argv[0],"open") == 0) {
			FILE *file = fopen(argv[1],argv[2]);
			if (file == NULL) {
				sprintf(result,"can't open file %s\n", argv[1]);
				sendClient(ERROR,result);
				// Need to exit..
			}
			/*
			printf("open %d\n",file);
			*/
			sprintf(result,"%ld",(long)file);
			sendClient(OK,result);
		}
		else if (strcmp(argv[0], "setViewStatistics") == 0) {
		    ViewGraph *vg = (ViewGraph *)classDir->FindInstance(argv[1]);
		    if (vg == NULL) {
			sendClient(ERROR,"Can't find view in SetViewStatistics");
			goto error;
		    }
		    /* Turn on/off display of statistics */
		    vg->SetDisplayStats(argv[2]);
			sendClient(OK,"OK");
		}
		else if (strcmp(argv[0], "setViewDimensions") == 0) {
		    View *vg = (View *)classDir->FindInstance(argv[1]);
		    if (vg == NULL) {
			sendClient(ERROR,"Can't find view in SetViewDimensions");
			goto error;
		    }
		    /* Turn on/off display of statistics */
		    vg->SetNumDimensions(atoi(argv[2]));
			sendClient(OK,"OK");
		}
		else if (strcmp(argv[0],"savePixmap") == 0) {
			/*
			printf("savePixmap %s %s %s\n",argv[0],argv[1],argv[2]);
			*/
			View *vg = (View *)classDir->FindInstance(argv[1]);
			if (vg == NULL) {
				sendClient(ERROR,"Can't find view in savePixmap");
				goto error;
			}
			FILE *file = (FILE *)atol(argv[2]);
			/*
			printf("file is %d\n", (int)file);
			*/
			vg->SavePixmaps(file);
			sendClient(OK,"OK");

		}
		else if (strcmp(argv[0],"loadPixmap") == 0) {
			/*
			printf("loadPixmap %s %s %s\n",argv[0],argv[1],argv[2]);
			*/
			View *vg = (View *)classDir->FindInstance(argv[1]);
			if (vg == NULL) {
				sendClient(ERROR,"Can't find view in loadPixmap");
				goto error;
			}
			FILE *file = (FILE *)atol(argv[2]);
			/*
			printf("file is %d\n", (int)file);
			*/
			vg->LoadPixmaps(file);
			sendClient(OK,"OK");
		}
		else if (strcmp(argv[0],"getAxisDisplay") == 0 ) {
			View *vg = (View *)classDir->FindInstance(argv[1]);
			if (vg == NULL) {
				sendClient(ERROR,"Can't find view in getAxisDisplay");
				goto error;
			}
			Boolean stat, xAxis, yAxis;
			vg->AxisDisplay(xAxis, yAxis);
			if (strcmp(argv[2],"X") == 0) 
			  stat = xAxis;
			else
			  stat = yAxis;
			if (stat)
				strcpy(result , "1");
			else
				strcpy(result , "0");
			sendClient(OK,result);
		}
		else if (strcmp(argv[0],"replaceView") == 0) {
			View *view1 = (View *)classDir->FindInstance(argv[1]);
			View *view2 = (View *)classDir->FindInstance(argv[2]);
			if (view1 == NULL || view2 == NULL) {
				sendClient(ERROR,"TkControl:can't find view for replaceView");
				// Need to exit here..
			}
			ViewWin *win = view1->GetParent();
			if (win == NULL) {
				sendClient(ERROR,"TkControl:can't find window for replaceView");
				//Exit::DoExit(1);
			}
			win->Replace(view1, view2);
			sendClient(OK,"OK");
		} else if (strcmp(argv[0],"setCursorSrc") == 0 ) {
			DeviseCursor *cursor = (DeviseCursor *)
				classDir->FindInstance(argv[1]);
			if (cursor == NULL) {
				sendClient(ERROR,"Can't find cursor");
				goto error;
			}
			View *view = (View *)classDir->FindInstance(argv[2]);
			if (view == NULL) {
				sendClient(ERROR,"Can't find view");
				goto error;
			}
			cursor->SetSource(view);
			sendClient(OK,"OK");
		}
		else if (strcmp(argv[0],"setCursorDst") == 0) {
			DeviseCursor *cursor = (DeviseCursor *)
				classDir->FindInstance(argv[1]);
			if (cursor == NULL) {
				sendClient(ERROR,"Can't find cursor");
				goto error;
			}
			View *view = (View *)classDir->FindInstance(argv[2]);
			if (view == NULL) {
				sendClient(ERROR,"Can't find view");
				goto error;
			}
			cursor->SetDst(view);
			sendClient(OK,"OK");

		} 
		else if (strcmp(argv[0],"setPixelWidth") == 0 ) {
			TDataMap *map= (TDataMap *)classDir->FindInstance(argv[1]);
			if (map== NULL) {
				sendClient(ERROR,"Can't find mapping in setPixelWidth");
				goto error;
			}
			int width = atoi(argv[2]);
			if (width > 0 )
				map->SetPixelWidth(width);

			sendClient(OK,"OK");
		}
		else if (strcmp(argv[0],"getAxis") == 0 ) {
			View *view = (View *)classDir->FindInstance(argv[1]);
			if (view == NULL) {
				sendClient(ERROR,"Can't find view");
				goto error;
			}
			AxisLabel *label;
			if (strcmp(argv[2],"x") == 0) 
				label = view->GetXAxisLabel();
			else label = view->GetYAxisLabel();

			if (label == NULL || strcmp(label->GetName(),"") == 0)
				sendClient(OK,"");
			else sendClient(OK,label->GetName());
		}
		else if (strcmp(argv[0],"setAction") == 0) {
			ViewGraph *view = (ViewGraph *)classDir->FindInstance(argv[1]);
			if (view == NULL) {
				sendClient(ERROR,"can't find view");
				goto error;
			}
			Action *action = (Action*)classDir->FindInstance(argv[2]);
			if (action == NULL) {
				sendClient(ERROR,"can't find action");
				goto error;
			}
			view->SetAction(action);
			sendClient(OK,"OK");

		}
		else if (strcmp(argv[0],"setLinkFlag") == 0 ) {
			VisualLink *link = (VisualLink *)classDir->FindInstance(argv[1]);
			if (link == NULL) {
				sendClient(ERROR, "can't find link");
				goto error;
			}
			VisualFlag flag = atoi(argv[2]);
			link->ChangeFlag(flag);
			sendClient(OK,"OK");
		}
		else if (strcmp(argv[0], "highlightView") == 0) {
			View *view = (View *)classDir->FindInstance(argv[1]);
			if (view == NULL) {
				sprintf(result,"TkControl:Cmd highlightView can't find view %s\n", argv[1]);
				sendClient(ERROR,result);
			//	Exit::DoExit(2);
			}
			view->Highlight(atoi(argv[2]));
			sendClient(OK,"OK");
		}
		else if (strcmp(argv[0],"get") == 0) {
			classDir->InstanceNames(argv[1],argv[2], numArgs, args);
			SendReturnVals(numArgs, args);
		}
		else if (strcmp(argv[0],"getparam") == 0) {
			classDir->GetParams(argv[1],argv[2],numArgs, args);
			SendReturnVals(numArgs, args);
		}
		else if (strcmp(argv[0],"insertMapping") == 0) {
		  ViewGraph *view = (ViewGraph *)classDir->FindInstance(argv[1]);
		  if (!view) {
		    sprintf(result, "Cannot find view %s\n", argv[1]);
			sendClient(ERROR,result);
		    goto error;
		  }
		  TDataMap *map = (TDataMap *)classDir->FindInstance(argv[2]);
		  if (!map) {
		    sprintf(result, "Cannot find mapping %s\n", argv[2]);
			sendClient(ERROR,result);
		    goto error;
		  }
		  view->InsertMapping(map);
		  sendClient(OK,"OK");
		}
		else if (strcmp(argv[0],"getMappingLegend") == 0) {
		  ViewGraph *view = (ViewGraph *)classDir->FindInstance(argv[1]);
		  if (!view) {
		    sprintf(result, "Cannot find view %s\n", argv[1]);
			sendClient(ERROR,result);
		    goto error;
		  }
		  TDataMap *map = (TDataMap *)classDir->FindInstance(argv[2]);
		  if (!map) {
		    sprintf(result, "Cannot find mapping %s\n", argv[2]);
			sendClient(ERROR,result);
		    //interp->result = "";
		    goto error;
		  }
		  sendClient(OK,view->GetMappingLegend(map));
		}
		else if (strcmp(argv[0],"insertLink") == 0) {
		  VisualLink *link = (VisualLink *)classDir->FindInstance(argv[1]);
		  if (!link) {
		    sprintf(result, "TkControl:Cmd insertLink can't find link %s\n",
			    argv[1]);
			sendClient(ERROR,result);
		    goto error;
		  }
		  View *view = (View *)classDir->FindInstance(argv[2]);
		  if (!view) {
		    sprintf(result, "TkControl:Cmd insertLink can't find view %s\n",
			    argv[2]);
			sendClient(ERROR,result);
		    goto error;
		  }
		  link->InsertView(view);
		  sendClient(OK,"OK");
		}
		else if (strcmp(argv[0],"viewInLink") == 0) {
			VisualLink *link = (VisualLink *)classDir->FindInstance(argv[1]);
			if (link == NULL) {
				sprintf(result,"TkControl:Cmd insertLink can't find link %s\n", argv[1]);
				sendClient(ERROR,result);
				//Exit::DoExit(2);
			}
			View *view = (View *)classDir->FindInstance(argv[2]);
			if (view == NULL) {
				sprintf(result,"TkControl:Cmd insertMapping can't find view %s\n", argv[2]);
				sendClient(ERROR,result);
				//Exit::DoExit(2);
			}
			if (link->ViewInLink(view)) {
				strcpy(result , "1");
			}
			else {
				strcpy(result , "0");
			}
			sendClient(OK,result);
		}
		else if (strcmp(argv[0],"unlinkView") == 0) {
			VisualLink *link = (VisualLink *)classDir->FindInstance(argv[1]);
			if (link == NULL) {
				sprintf(result,"TkControl:Cmd insertLink can't find link %s\n", argv[1]);
				sendClient(ERROR,result);
				//Exit::DoExit(2);
			}
			View *view = (View *)classDir->FindInstance(argv[2]);
			if (view == NULL) {
				sprintf(result,"TkControl:Cmd insertLink can't find view %s\n", argv[2]);
				sendClient(ERROR,result);
				//Exit::DoExit(2);
			}
			link->DeleteView(view);
			sendClient(OK,"OK");
		}
		else if (strcmp(argv[0],"insertWindow") == 0) {
			ViewGraph *view = (ViewGraph *)classDir->FindInstance(argv[1]);
			if (!view) {
				sprintf(result,"TkControl:Cmd insertWindow can't find view %s\n", argv[1]);
				sendClient(ERROR,result);
			//	Exit::DoExit(2);
			}
			ViewWin *win = (ViewWin *)classDir->FindInstance(argv[2]);
			if (!win) {
				sprintf(result,"TkControl:Cmd insertWindow can't find window %s\n", argv[2]);
				sendClient(ERROR,result);
				//Exit::DoExit(2);
			}
			view->DeleteFromParent();
			view->AppendToParent(win);
			sendClient(OK,"OK");
		}
		else if (strcmp(argv[0],"removeMapping") == 0) {
			ViewGraph *view = (ViewGraph *)classDir->FindInstance(argv[1]);
			if (!view) {
				strcpy(result , "Cannot find view");
			  sendClient(ERROR,result);
			  goto error;
			}
			TDataMap *map = (TDataMap *)classDir->FindInstance(argv[2]);
			if (!map) {
				strcpy(result , "Cannot find mapping");
			  sendClient(ERROR,result);
			  goto error;
			}
			view->RemoveMapping(map);
			sendClient(OK,"OK");
		}
		else {
			sendClient(ERROR,"wrong args");
			goto error;
		}
	}
	else if (strcmp(argv[0],"swapView") == 0 ) {
		ViewWin *viewWin = (ViewWin *)classDir->FindInstance(argv[1]);
		View *view1 = (View *)classDir->FindInstance(argv[2]);
		View *view2 = (View *)classDir->FindInstance(argv[3]);
		if (viewWin == NULL || view1 == NULL || view2 == NULL) {
			sendClient(ERROR,"Can't find view or window in swapView ");
			goto error;
		}
		if (view1->GetParent() != viewWin || view2->GetParent() != viewWin) {
			sendClient(ERROR,"Views not in same window in swapView\n");
			goto error;
		}
		viewWin->SwapChildren(view1, view2);
		sendClient(OK,"OK");
	}
	else if (strcmp(argv[0],"setAxisDisplay") == 0 ) {
		View *vg = (View *)classDir->FindInstance(argv[1]);
		if (vg == NULL) {
			sendClient(ERROR,"Can't find view in setAxisDisplay");
			goto error;
		}
		Boolean stat = atoi(argv[3]);
		if (strcmp(argv[2],"X") == 0)
		  vg->XAxisDisplayOnOff(stat);
		else
		  vg->YAxisDisplayOnOff(stat);
		sendClient(OK,"OK");
	}
	else if (strcmp(argv[0],"insertViewHistory") == 0 ) {
		View *view = (View *)classDir->FindInstance(argv[1]);
		if (view == NULL) {
			sprintf(result,"TkControl:Cmd insertViewHistory find view %s\n", 
				argv[1]);
			sendClient(ERROR,result);
			//Exit::DoExit(2);
		}
		VisualFilter filter;
		(void)ParseFloatDate(argv[2],filter.xLow); 
		(void)ParseFloatDate(argv[3],filter.yLow);
		(void)ParseFloatDate(argv[4],filter.xHigh); 
		(void)ParseFloatDate(argv[5],filter.yHigh);
		filter.marked = atoi(argv[6]);
		view->InsertHistory(filter);
		sendClient(OK,"OK");
	}
	else if (strcmp(argv[0],"markViewFilter") == 0 ) {
		View *view = (View *)classDir->FindInstance(argv[1]);
		if (view == NULL) {
			sprintf(result,"TkControl:Cmd markViewFiltercan't find view %s\n", 
				argv[1]);
			sendClient(ERROR,result);
			//Exit::DoExit(2);
		}
		int index = atoi(argv[2]);
		Boolean mark = atoi(argv[3]);
		view->Mark(index, mark);
		sendClient(OK,"OK");
	}
	else if (strcmp(argv[0],"setAxis") == 0 ) {
		if (argc != 4) {
			sprintf(result,"TkControl:setAxis needs 5 params\n");
			sendClient(ERROR,result);
			//Exit::DoExit(2);
		}
		View *view = (View *)classDir->FindInstance(argv[1]);
		if (view == NULL) {
			sprintf(result,"TkControl:Cmd setAxis can't find view %s\n", 
				argv[1]);
			sendClient(ERROR,result);
			//Exit::DoExit(2);
		}
		AxisLabel *label = (AxisLabel *)classDir->FindInstance(argv[2]);
		if (label == NULL) {
			sprintf(result,"TkControl:Cmd setAxis can't find label %s\n", 
				argv[2]);
			sendClient(ERROR,result);
			//Exit::DoExit(2);
		}
		if (strcmp(argv[3],"x")== 0)
			view->SetXAxisLabel(label);
		else view->SetYAxisLabel(label);
		sendClient(OK,"OK");
	}
	else if (strcmp(argv[0],"changeParam") == 0) {
		classDir->ChangeParams(argv[1],argc-2,&argv[2]);
	}
	else if (strcmp(argv[0],"createInterp") == 0) {
	  /* This command is supported for backward compatibility only */
		ClassInfo *classInfo = 
			_interpProto->CreateWithParams(argc-1,&argv[1]);
		if (!classInfo) {
			sendClient(ERROR,"Can't create mapping");
			goto error;
		}
		sendClient(OK,classInfo->ClassName());
		RegisterClass(classInfo,true);
	}
	else if (strcmp(argv[0],"create") == 0) {
		control->SetBusy();
		// HACK to provide backward compatibility
		if (!strcmp(argv[2], "WinVertical") ||
		    !strcmp(argv[2], "WinHorizontal"))
		  argv[2] = "TileLayout";
		char *name = classDir->CreateWithParams(argv[1],argv[2],
			argc-3, &argv[3]);
		control->SetIdle();
		if (name == NULL) {
			sendClient(OK,"");
		}
		else {
			sendClient(OK,name);
			//Tcl_AppendResult(interp,name, (char *)NULL);
		}
	}
	else if (strcmp(argv[0],"setDefault") == 0) {
		if (argc < 4) {
			sendClient(ERROR,"wrong args");
			goto error;
		}
		classDir->SetDefault(argv[1],argv[2],argc-3,&argv[3]);
		sendClient(OK,"OK");
	}
	else if (argc == 4) {
	  if (strcmp(argv[0],"getCreateParam") == 0) {
	    /* getCreateParam category class instance: 
	       get parameters used to recreate an instance */
	    classDir->CreateParams(argv[1],argv[2],argv[3],numArgs,args);
	    SendReturnVals(numArgs, args);
	  }
	  else if (strcmp(argv[0], "getItems") == 0) {
	    gdir->get_items(result, argv[1], argv[2], argv[3]);
		sendClient(OK,result);
      }
		////****************/////////////
	  
	  else if (strcmp(argv[0], "setWindowLayout") == 0) {
	    ViewLayout *layout = (ViewLayout *)classDir->FindInstance(argv[1]);
	    if (!layout) {
	      sprintf(result, "TkControl:cmd setWindowLayout can't find window %s\n", argv[1]);
		  sendClient(ERROR,result);
	      //Exit::DoExit(2);
	    }
	    layout->SetPreferredLayout(atoi(argv[2]), atoi(argv[3]));
		sendClient(OK,"OK");
	  }
	  else if (strcmp(argv[0], "saveWindowImage") == 0) {
	    DisplayExportFormat format = POSTSCRIPT;
	    if (!strcmp(argv[1], "eps"))
	      format = EPS;
	    else if (!strcmp(argv[1], "gif"))
	      format = GIF;
	    ViewWin *viewWin = (ViewWin *)classDir->FindInstance(argv[2]);
	    if (!viewWin) {
	      sprintf(result, "TkControl:cmd saveWindowImage can't find window %s\n", argv[1]);
			sendClient(ERROR,result);
              //Exit::DoExit(2);
	    }
	    viewWin->GetWindowRep()->ExportImage(format, argv[3]);
		sendClient(OK,"OK");
	  }
	  else if (strcmp(argv[0], "setViewOverrideColor") == 0) {
	    View *view = (View *)classDir->FindInstance(argv[1]);
	    if (view == NULL) {
	      sprintf(result,"TkControl:Cmd setViewOverrideColor can't find view %s\n", argv[1]);
		sendClient(ERROR,result);
	    //Exit::DoExit(2);
	    }
	    Boolean active = (atoi(argv[2]) == 1);
	    view->SetOverrideColor(atoi(argv[3]), active);
		sendClient(OK,"OK");
	  }
	  else if (strcmp(argv[0],"insertMapping") == 0) {
	    ViewGraph *view = (ViewGraph *)classDir->FindInstance(argv[1]);
	    if (!view) {
	      sprintf(result, "Cannot find view %s\n", argv[1]);
		  sendClient(ERROR,result);
	      goto error;
	    }
	    TDataMap *map = (TDataMap *)classDir->FindInstance(argv[2]);
	    if (!map) {
	      sprintf(result, "Cannot find mapping %s\n", argv[2]);
		  sendClient(ERROR,result);
	      goto error;
	    }
	    view->InsertMapping(map, argv[3]);
		sendClient(OK,"OK");
	  }
	  else {
	    sendClient(ERROR,"wrong args");
	    goto error;
	  }
	}
	else if (argc == 5) {
		if (strcmp(argv[0], "setLabel") == 0) {
			View *view = (View *)classDir->FindInstance(argv[1]);
			if (view == NULL) {
				sendClient(ERROR,"Can't find view in setLabel\n");
				goto error;
			}
			view->SetLabelParam(atoi(argv[2]), atoi(argv[3]),
					    argv[4]);
			sendClient(OK,"OK");
		}
		else if (strcmp(argv[0], "setWindowLayout") == 0) {
		  ViewLayout *layout = (ViewLayout *)classDir->FindInstance(argv[1]);
		  if (!layout) {
		    sprintf(result, "TkControl:cmd setWindowLayout can't find window %s\n", argv[1]);
		   sendClient(ERROR,result);
		  //Exit::DoExit(2);
		  }
		  layout->SetPreferredLayout(atoi(argv[2]), atoi(argv[3]),
					     (atoi(argv[4]) ? true : false));
		   sendClient(OK,"OK");
		}
		else {
			sendClient(ERROR,"wrong args");
			goto error;
		}
	}
	else if (argc == 6 ) {
		if (strcmp(argv[0],"setFilter") == 0) {
			View *view = (View *)classDir->FindInstance(argv[1]);
			if (view == NULL) {
				sprintf(result,"TkControl:Cmd insertWindow can't find view %s\n", argv[1]);
				sendClient(ERROR,result);
				//Exit::DoExit(2);
			}
			VisualFilter filter;
			view->GetVisualFilter(filter);
			if (!ParseFloatDate(argv[2],filter.xLow) ||
				!ParseFloatDate(argv[3], filter.yLow) ||
				!ParseFloatDate(argv[4], filter.xHigh) ||
				!ParseFloatDate(argv[5], filter.yHigh)) {
				sendClient(ERROR,"invalid date or float");
				goto error;
			}
			view->SetVisualFilter(filter);
			sendClient(OK,"OK");
		}
	}
	else {
		sendClient(ERROR,"wrong args");
		goto error;
	}

	/*
	Instance()->SetIdle();
	*/
	return TCL_OK;
error:
	/*
	Instance()->SetIdle();
	*/
	return TCL_ERROR;
}

void TkControlPanel::Run()
{

	// Do an accept and then call the necessary functions...

	int newSockFd;

	struct sockaddr_in cli_addr;
	bzero(&cli_addr,sizeof(cli_addr));
	int len = sizeof(struct sockaddr);

	if ((newSockFd = accept(socketFd,(struct sockaddr *)&cli_addr,&len)) < 0 ) {

		if ( errno == EWOULDBLOCK ) 
			;
		else{
			perror("Error in accept call");
			Exit::DoExit(1);
		}
	}
	else{
	
		// create a new instance of the socket 
		ServerControl *temp = new ServerControl(newSockFd);
		DOASSERT(temp,"Failure to create new client socket");
	}


}

int  TkControlPanel::readSocket()
{

	static u_short maxSize = 0;
	u_short numElements;

	static char ** buff = 0;

	if (recv(socketFd,(char *)&numElements,sizeof(u_short),0) < 0 )
		return -1;
	
	numElements = ntohs(numElements);
	if ( maxSize < numElements){
		if (buff) 
			delete [] buff;
		buff = new (char*) [numElements];
		DOASSERT(buff,"Memory overflow");
		maxSize = numElements;
	}
	u_short size;
	for(int i = 0; i < numElements;i++){
		
		if (recv(socketFd,(char *)&size,sizeof(u_short),0) < 0 )
			return -1;
		size = ntohs(size);
		buff[i] = new char[size];
		DOASSERT(buff[i],"Memory overflow");

		if ( recv(socketFd,buff[i],size,0) < 0 ) 
			return -1;

	}
	if ( ControlCmd(numElements, buff) == TCL_ERROR ) 
		fprintf(stderr,"Error");
	
	for(int i = 0;i < numElements;i++)
		if (buff[i])
			delete buff[i];

		return 1;
}



	


void TkControlPanel::SubclassInsertDisplay(DeviseDisplay * /*disp*/,
					   Coord /*x*/, Coord /*y*/,
					   Coord /*w*/, Coord /*h*/)
{
}

void TkControlPanel::SubclassRegisterView(View *view)
{

}

void TkControlPanel::SubclassUnregisterView(View *view)
{
}

void TkControlPanel::SubclassDoInit()
{
}

void TkControlPanel::SubclassDoViewPerspectiveChanged(View *view,
		Coord , Coord, Coord, Coord)
{
}

void TkControlPanel::SetBusy()
{
  if (++_busy == 1);
  sendControlClient(OK,"ChangeStatus 1");
    //(void)Tcl_Eval(_interp, "ChangeStatus 1");
}

void TkControlPanel::SetIdle()
{
  DOASSERT(_busy > 0, "Control panel unexpectedly busy");

  if (--_busy == 0);
  sendControlClient(OK,"ChangeStatus 0");
   // (void)Tcl_Eval(_interp, "ChangeStatus 0");
}

Boolean TkControlPanel::IsBusy()
{
  return (_busy > 0);
}

void TkControlPanel::ExecuteScript(char *script)
{
  char cmd[256];
  sprintf(cmd, "ExecuteScript %s", script);
  sendControlClient(OK,cmd);
  //(void)Tcl_Eval(_interp, cmd);
}

void TkControlPanel::FilterChanged(View *view, VisualFilter &filter,
				   int flushed)
{
#ifdef DEBUG
  printf("TkControl: filter changed\n");
#endif

  char cmd[256];
  char xLowBuf[80], yLowBuf[80], xHighBuf[80], yHighBuf[80];
  if (view->GetXAxisAttrType() == DateAttr) {
    sprintf(xLowBuf, "%s", DateString(filter.xLow));
    sprintf(xHighBuf, "%s", DateString(filter.xHigh));
  } else {
    sprintf(xLowBuf, "%.2f", filter.xLow);
    sprintf(xHighBuf, "%.2f", filter.xHigh);
  }
				
  if (view->GetYAxisAttrType() == DateAttr) {
    sprintf(yLowBuf, "%s", DateString(filter.yLow));
    sprintf(yHighBuf, "%s", DateString(filter.yHigh));
  } else {
    sprintf(yLowBuf, "%.2f", filter.yLow);
    sprintf(yHighBuf, "%.2f", filter.yHigh);
  }
  
  sprintf(cmd, "ProcessViewFilterChange {%s} %d {%s} {%s} {%s} {%s} 0",
	  view->GetName(), flushed, xLowBuf, yLowBuf, xHighBuf, yHighBuf);
  sendControlClient(OK,cmd);
  //(void)Tcl_Eval(_interp, cmd);
}

void TkControlPanel::ViewCreated(View *view)
{
  char cmd[256];
  sprintf(cmd, "ProcessViewCreated {%s}", view->GetName());
  sendControlClient(OK,cmd);
  //(void)Tcl_Eval(_interp, cmd);
}

void TkControlPanel::ViewDestroyed(View *view)
{
  char cmd[256];
  sprintf(cmd, "ProcessViewDestroyed {%s}", view->GetName());
  sendControlClient(OK,cmd);
  //(void)Tcl_Eval(_interp, cmd);
}

/* Make view the current view int he control panel */

void TkControlPanel::SelectView(View *view)
{
  char cmd[256];
  sprintf(cmd, "ProcessViewSelected {%s}", view->GetName());
  sendControlClient(OK,cmd);
  //(void)Tcl_Eval(_interp, cmd);
}

ServerControl::ServerControl(int socketID):TkControlPanel(socketID)
{
	
	char cliName[30];
	u_short port,size,tempsize,tempport;
	
	// for future use..

	if (recv(socketFd,(char *)&tempsize,sizeof(u_short),0) < 0)
		perror("Recv");
	
	size = ntohs(tempsize);
	if (recv(socketFd,cliName,size,0) < 0)
		perror("Recv");

	if (recv(socketFd,(char *)&tempport,sizeof(u_short),0) < 0)
		perror("Recv");

	port = ntohs(tempport);
	openConnection(cliName,port);
}

void ServerControl::openConnection(char *cliName,u_short port)
{


    controlFd = socket(AF_INET,SOCK_STREAM,0);
    DOASSERT(controlFd != -1,"Socket in Server");

   	// Now bind these to the address..

    struct hostent *servEnt = gethostbyname(cliName);
    DOASSERT(servEnt != NULL , "Address determination error");

    struct in_addr * ptr;
    ptr = 0;
    switch(servEnt->h_addrtype){
       
        case AF_INET: ptr = (struct in_addr * ) *servEnt->h_addr_list;
                      break;
        default:      DOASSERT(0,"Address type not supported");
                      exit(0);

    }
   
    struct sockaddr_in cliAddr;
    bzero(&cliAddr,sizeof(cliAddr));

    cliAddr.sin_family = AF_INET;
    cliAddr.sin_port   = htons(port);
    cliAddr.sin_addr   = * ptr;
    
	printf(" Connect: port num = %d\n",cliAddr.sin_port);
	printf(" Connect: port num = %d\n",ntohs(cliAddr.sin_port));
	
	//DOASSERT((connect(controlFd,(struct sockaddr *)&cliAddr,\
     //   sizeof(struct sockaddr)) >= 0  )," Cannot connect to client");
    if(connect(controlFd,(struct sockaddr *)&cliAddr, sizeof(struct sockaddr)) < 0){
		perror("Connect");
		exit(0);
	}

}
	
ServerControl::~ServerControl()
{

	close(socketFd);
	Dispatcher::Current()->Unregister(this);

}

void ServerControl::Run()
{

	if ( readSocket() < 0 ) {
		//~ServerControl();
	}

}

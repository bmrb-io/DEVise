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
  Description of module.
 */

/*
  $Id$

  $Log$
  Revision 1.7  1998/05/26 14:09:19  wenger
  Now puts curly brackets on arguments to allow spaces in arguments;
  committed with debug code on for Hongyu.

  Revision 1.6  1998/05/21 18:18:49  wenger
  Most code for keeping track of 'dirty' GIFs in place; added 'test'
  command to be used for generic test code that needs to be controlled
  by GUI; added debug code in NetworkSend().

  Revision 1.5  1998/05/14 18:21:43  wenger
  New protocol for JavaScreen opening sessions works (sending "real" GIF)
  except for the problem of spaces in view and window names.

  Revision 1.4  1998/05/08 17:16:23  taodb
  Added stripping functions for "{}"

  Revision 1.3  1998/05/05 17:07:24  wenger
  Minor improvements to JavaScreenCmd.[Ch].

  Revision 1.2  1998/05/02 09:00:44  taodb
  Added support for JAVA Screen and command logging

  Revision 1.1  1998/04/25 05:45:23  taodb
  Initial Revision

 */
#include <sys/types.h>
#include <sys/stat.h>
#if !defined(SGI) && !defined(LINUX)
#include <sys/varargs.h>
#else
#include <stdarg.h>
#endif
#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>
#include <string>
#include <vector>

#include "JavaScreenCmd.h"
#include "DeviseServer.h"
#include "CmdContainer.h"
#include "ClientAPI.h"
#include "Control.h"
#include "Util.h"
#include "Session.h"
#include "WinClassInfo.h"
#include "ViewWin.h"
#include "ViewGraph.h"
#include "Init.h"
#include "Timer.h"
#include "QueryProc.h"

//#define DEBUG

off_t getFileSize(const char* filename);

// Wait for all queries to finish before continuing.
static void
WaitForQueries()
{
    QueryProc *qp = QueryProc::Instance();
    do {
      Dispatcher::SingleStepCurrent();
    } while (!qp->Idle());
}

// Determine whether the given point is within the given rectangle.
static Boolean
PointInRect(int px, int py, int rx1, int ry1, int rx2, int ry2)
{
    if (rx1 > rx2) {
		int tmp = rx1;
		rx1 = rx2;
		rx2 = tmp;
	}

	if (ry1 > ry2) {
		int tmp = ry1;
		ry1 = ry2;
		ry2 = tmp;
	}

	if (px < rx1) {
		return false;
	} else if (px > rx2) {
		return false;
	} else if (py < ry1) {
		return false;
	} else if (py > ry2) {
		return false;
	} else {
		return true;
	}
}

void
JavaScreenCmd::GetSessionList()
{
#if defined (DEBUG)
    printf("JavaScreenCmd::GetSessionList(");
    PrintArgs(stdout, _argc, _argv, false);
    printf(")\n");
	fflush(stdout);
#endif

	// ADD---begin
	char* argv[] =
	{
		_controlCmdName[UPDATESESSIONLIST],
		"/p/devise/session/AAAcolor2.ds",
		"/p/devise/session/AAAcolor3.ds",
		"/p/devise/session/JFK2.ds",
		"/p/devise/session/demo/colors.ds",
		"/p/devise/session/test/js_test.ds",// no spaces in names
		"/p/devise/session/test/js_test2.ds",// no spaces in names
		"dummy_session"
	};



	// ADD---end
	_status = RequestUpdateSessionList(sizeof(argv)/sizeof(argv[0]), argv);	
}

void
JavaScreenCmd::OpenSession()
{
#if defined (DEBUG)
    printf("JavaScreenCmd::OpenSession(");
    PrintArgs(stdout, _argc, _argv, false);
    printf(")\n");
	fflush(stdout);
#endif

	if (_argc != 1)
	{
		errmsg = "{Usage: OpenSession <session name>}";
		_status = ERROR;
		return;
	}

	printf("Session:%s requested!\n", _argv[0]);

	// Set batch mode so the server makes pixmaps instead of windows.
	ControlPanel::Instance()->SetBatchMode(true);

	// Open the session.
    DevStatus result = Session::Open(_argv[0]);
	if (!result.IsComplete())
	{
		errmsg = "{Error opening session}";
		_status = ERROR;
		return;
	}

	// Wait for all queries to finish before continuing.
	WaitForQueries();

	// Dump all window images to files.
	int winIndex = DevWindow::InitIterator();
	while (DevWindow::More(winIndex))
	{
	  ClassInfo *info = DevWindow::Next(winIndex);
	  ViewWin *window = (ViewWin *)info->GetInstance();
	  if (window != NULL)
	  {
#if defined(DEBUG)
        printf("window name: %s\n", window->GetName());
		printf("%d children\n", window->NumChildren());
	    fflush(stdout);
#endif

        JavaViewInfo **views = new JavaViewInfo*[window->NumChildren()];




		int winX, winY;
		unsigned winW, winH;
		window->RealGeometry(winX, winY, winW, winH);
#if defined(DEBUG)
        printf("window RealGeometry = %d, %d, %d, %d\n", winX, winY, winW,
			winH);
#endif

		window->AbsoluteOrigin(winX, winY);
#if defined(DEBUG)
        printf("window AbsoluteOrigin = %d, %d\n", winX, winY);
#endif

		JavaRectangle winRect(0, 0, winW-1, winH-1);
#if defined(DEBUG)
        printf("window JavaRectangle: (%g, %g), (%g, %g)\n", winRect._x1,
			winRect._y1, winRect._x2, winRect._y2);
#endif

        string winName(window->GetName());
        string imageName(Init::TmpDir());
        imageName += "/" + winName + ".gif";
#if defined(DEBUG)
        cout << "imageName = " << imageName << endl;
#endif




	    int viewNum = 0;
		int viewIndex = window->InitIterator();
		while (window->More(viewIndex))
		{
		  ViewWin *view = window->Next(viewIndex);
#if defined(DEBUG)
          printf("  view name: %s\n", view->GetName());
#endif


		  int viewX, viewY;
		  unsigned viewW, viewH;
		  view->RealGeometry(viewX, viewY, viewW, viewH);
#if defined(DEBUG)
          printf("  view RealGeometry = %d, %d, %d, %d\n", viewX, viewY,
			  viewW, viewH);
#endif

		  view->GetWindowRep()->Origin(viewX, viewY);
#if defined(DEBUG)
          printf("  view WindowRep Origin = %d, %d\n", viewX, viewY);
#endif

		  JavaRectangle viewRect(viewX, viewY, viewX + viewW - 1,
			  viewY + viewH - 1);
#if defined(DEBUG)
          printf("  view JavaRectangle: (%g, %g), (%g, %g)\n", viewRect._x1,
			  viewRect._y1, viewRect._x2, viewRect._y2);
#endif

		  string viewName(view->GetName());

		  JavaViewInfo *viewInfo = new JavaViewInfo(viewRect, viewName);
		  views[viewNum] = viewInfo;

		  viewNum++;
		}
		window->DoneIterator(viewIndex);

        JavaWindowInfo winInfo(winRect, winName, imageName,
			window->NumChildren(), views);



	    _status = RequestCreateWindow(winInfo);

        delete [] views;
	  }
	}
	DevWindow::DoneIterator(winIndex);












	// Add---end
}

void
JavaScreenCmd::MouseAction_Click()
{
#if defined (DEBUG)
    printf("JavaScreenCmd::MouseAction_Click(");
    PrintArgs(stdout, _argc, _argv, false);
    printf(")\n");
	fflush(stdout);
#endif

	if (_argc != 3)
	{
		errmsg = "{Usage: MouseAction_Click <view name> <x> <y>}";
		_status = ERROR;
		return;
	}

	// ADD---begin
	TDataVal	tdval = 3.1415926;





	// ADD---end
	_status = RequestUpdateRecordValue(tdval);
}

void
JavaScreenCmd::MouseAction_DoubleClick()
{
#if defined (DEBUG)
    printf("JavaScreenCmd::MouseAction_DoubleClick(");
    PrintArgs(stdout, _argc, _argv, false);
    printf(")\n");
	fflush(stdout);
#endif

	if (_argc != 3)
	{
		errmsg = "{Usage: MouseAction_DoubleClick <view name> <x> <y>}";
		_status = ERROR;
		return;
	}

	// ADD---begin
	GDataVal	gval;






	// ADD---end
	_status = RequestUpdateGData(gval);
}

void
JavaScreenCmd::MouseAction_RubberBand()
{
#if defined (DEBUG)
    printf("JavaScreenCmd::MouseAction_RubberBand(");
    PrintArgs(stdout, _argc, _argv, false);
    printf(")\n");
	fflush(stdout);
#endif

	if (_argc != 5)
	{
		errmsg = "{Usage: MouseAction_RubberBand <window name>}"
				 " <x1> <y1> <x2> <y2>";
		// Note: (x1, y1) is where mouse started; Y is down from the top of
		// the window.  RKW May 28, 1998.
		_status = ERROR;
		return;
	}

	int startX = atoi(_argv[1]);
	int startY = atoi(_argv[2]);
	int endX = atoi(_argv[3]);
	int endY = atoi(_argv[4]);

#if defined(DEBUG)
    printf("Rubberband line from (%d, %d) to (%d, %d) in window %s\n",
	  startX, startY, endX, endY, _argv[0]);
#endif

	//
	// Figure out which view the rubberband line started out in -- if it
	// overlaps more than one view (not allowed in the "regular" DEVise)
	// only zoom in the first view.
	//
    ViewWin *window = (ViewWin *)ControlPanel::FindInstance(_argv[0]);
	DOASSERT(window != NULL && !strcmp(window->GetName(), _argv[0]),
	  "Bad window name");

	int index = window->InitIterator();
	while (window->More(index)) {
		ViewWin *view = window->Next(index);

		int viewX, viewY;
		unsigned viewW, viewH;
		view->Geometry(viewX, viewY, viewW, viewH);
		view->GetWindowRep()->Origin(viewX, viewY);

		// Start and end of rubberband line relative to the view, not the
		// window.
		int viewStartX, viewStartY, viewEndX, viewEndY;

		if (PointInRect(startX, startY, viewX, viewY, viewX+viewW-1,
	      viewY+viewH-1)) {
			viewStartX = startX - viewX;
			viewStartY = startY - viewY;

			// If the rubberband line goes out of the view it starts in,
			// we only include the part within that view.
			if (endX < viewX) {
				viewEndX = 0;
			} else if (endX > viewX + (int)viewW - 1) {
				viewEndX = viewW - 1;
			} else {
				viewEndX = endX - viewX;
			}

			if (endY < viewY) {
				viewEndY = 0;
			} else if (endY > viewY + (int)viewH - 1) {
				viewEndY = viewH - 1;
			} else {
				viewEndY = endY - viewY;
			}

#if defined(DEBUG)
		    printf("Rubberband from (%d, %d) to (%d, %d) in view %s\n",
			  viewStartX, viewStartY, viewEndX, viewEndY, view->GetName());
#endif

			// Update the visual filter of the view that the
			// rubberband line started in.
			VisualFilter filter;
			((ViewGraph *)view)->GetVisualFilter(filter);
			((ViewGraph *)view)->FindWorld(viewStartX, viewStartY, viewEndX,
			  viewEndY, filter.xLow, filter.yLow, filter.xHigh, filter.yHigh);
			((ViewGraph *)view)->SetVisualFilter(filter);

			// Make sure everything has actually been re-drawn before we
			// continue.
			WaitForQueries();


			// Send the updated window image.
			// TEMP: this needs to be changed to send images of all windows
			// that are "dirty".
			char *fileName = "/tmp/window.gif";
			window->ExportImage(GIF, fileName);
	
			int	filesize;
			filesize = getFileSize(fileName);
			if (filesize >0)
			{
				_status = RequestUpdateWindow(_argv[0] ,filesize);
				ControlCmd(_status);
				if (_status == DONE)
				{
					_status = SendWindowImage(fileName, filesize);
				}
		
				// avoid unnecessary JAVAC_Done command, after sending
				// back images
				if (_status == DONE)
					_status = NULL_COMMAND;
			}
		}
	}
	window->DoneIterator(index);
}

void
JavaScreenCmd::CloseCurrentSession()
{
#if defined (DEBUG)
    printf("JavaScreenCmd::CloseCurrentSession(");
    PrintArgs(stdout, _argc, _argv, false);
    printf(")\n");
	fflush(stdout);
#endif

	ControlPanel::Instance()->DestroySessionData();

	_status = DONE;
	return;
}

//====================================================================
JavaScreenCmd::ControlCmdType
JavaScreenCmd::RequestUpdateGData(GDataVal gval)
{
#if defined (DEBUG)
    printf("JavaScreenCmd::RequestUpdateGData()\n");
	fflush(stdout);
#endif

	// Add---begin
	char* argv[7] =
	{
		_controlCmdName[UPDATEGDATA],
		"{name 1}",
		"http://www.test1.edu",
		"{name 2}",
		"http://www.test2.edu",
		"{name 3}",
		"http://www.test3.edu",
	};
	ReturnVal(7, argv);

	// Add---end
	return DONE;
}

JavaScreenCmd::ControlCmdType
JavaScreenCmd::RequestUpdateRecordValue(TDataVal tval)
{
#if defined (DEBUG)
    printf("JavaScreenCmd::RequestUpdateRecordValue()\n");
	fflush(stdout);
#endif

	// Add--begin
	char buf[128];
	sprintf(buf, "%f", tval);
	char* argv[2] = {
		_controlCmdName[UPDATERECORDVALUE],
		buf
	};
	ReturnVal(2, argv);

	// Add---end
	return DONE;
}
//====================================================================
off_t
getFileSize(const char* filename)
{
	off_t filesize;
	int	fd = open(filename, O_RDONLY);
	filesize = lseek(fd, 0, SEEK_END);
	if (filesize <0)
	{
		perror("Error in getting file size:");
	}
	return filesize;
}

JavaWindowInfo::JavaWindowInfo(JavaRectangle& winRec, string& winName,
	string& imageName, int viewCount, JavaViewInfo **views)
{
	_winRec = winRec;
	_winName = winName;
	_imageName = imageName;
	_views 	= viewCount;
	_viewList = new JavaViewInfo[viewCount];

	int i;
	for (i=0; i< viewCount; ++i)
	{
		_viewList[i] = *views[i];
	}
}

JavaWindowInfo::~JavaWindowInfo()
{
	delete []_viewList;
}


//
// be very careful that this order agree with the ControlCmdType definition
char* JavaScreenCmd::_controlCmdName[JavaScreenCmd::CONTROLCMD_NUM]=
{
	"JAVAC_UpdateSessionList",
	"JAVAC_CreateWindow",
	"JAVAC_UpdateRecordValue",
	"JAVAC_UpdateGData",
	"JAVAC_UpdateWindow",
	"JAVAC_Done",
	"JAVAC_Error",
	"JAVAC_Fail"
};

JavaScreenCmd::~JavaScreenCmd()
{
	int	i;
	for (i=0; i< _argc; ++i)
		delete []_argv[i];
	delete []_argv;
}

char* 
JavaScreenCmd::JavaScreenCmdName(JavaScreenCmd::ControlCmdType ctype)
{
	return JavaScreenCmd::_controlCmdName[(int)ctype];
}

JavaScreenCmd::JavaScreenCmd(ControlPanel* control,
	ServiceCmdType ctype, int argc, char** argv)
{
	int	i;
	static	char	leftBrace ='{';
	static  char	rightBrace ='}';


	_control  = control;
	_ctype = ctype;
	_argc = argc;
	_argv = new (char*)[argc](NULL);
	errmsg = NULL;

    for (i=0; i< _argc; ++i)
    {
        int j = 0;
        int arglen = strlen(argv[i]);
        int startPos, endPos;
 
        startPos = -1;
        endPos = arglen;
        _argv[i] = new (char)[arglen+1](0);
        while (argv[i][j]&&(
            (argv[i][j]==' ')||
            (argv[i][j]=='\t')&&
            (argv[i][j]!= leftBrace)))
            ++j;
        if (argv[i][j]==leftBrace)
        {
            startPos = j;
            j = arglen -1;
            while ((j>0)&&(
                (argv[i][j]==' ')||
                (argv[i][j]=='\t')&&
                (argv[i][j]!= rightBrace))
            )
                --j;
            if (j > startPos)
            {
                endPos = j;
            }
            else
            {
                fprintf(stderr, " { expected\n");
                startPos = -1;
            }
        }
        strncpy(_argv[i], argv[i]+startPos +1, endPos - startPos -1);
    }
}

int
JavaScreenCmd::Run()
{
#if defined(DEBUG)
    printf("JavaScreenCmd::Run(%d)\n", _ctype);
	fflush(stdout);
#endif

	_status = DONE;
	if (_ctype == JAVAEXIT)
	{
		CloseJavaConnection();
		return _status;
	}
	switch (_ctype)
	{
		case CLOSECURRENTSESSION:
			CloseCurrentSession();
			break;
		case GETSESSIONLIST:
			GetSessionList();
			break;
		case OPENSESSION:
			OpenSession();
			break;
		case MOUSEACTION_CLICK:
			MouseAction_Click();
			break;
		case MOUSEACTION_DOUBLECLICK:
			MouseAction_DoubleClick();
			break;
		case MOUSEACTION_RUBBERBAND:
			MouseAction_RubberBand();
			break;
		default:
			fprintf(stderr, "Undefined JAVA Screen Command:%d\n", _ctype);
	}
	return ControlCmd(_status);
}

int
JavaScreenCmd::ControlCmd(JavaScreenCmd::ControlCmdType  status)
{
	// return either DONE/ERROR/FAIL to current JAVA client
	if (status == DONE)
	{
		ReturnVal(1,&_controlCmdName[DONE]);
		return 1;
	}
	if (status == FAIL)
	{
		// abort the client 
		ReturnVal(1, &_controlCmdName[FAIL]);
		CloseJavaConnection();
		return -1;
	}
	if (status == ERROR)
	{
		const char *argv[2] = {
			JavaScreenCmd::_controlCmdName[ERROR],
			errmsg
		};	
		if (argv[1] == NULL)
		{
			argv[1]= "Error, but no error message available";
		}
		ReturnVal(2, (char**)&argv[0]);
		return -1;
	}
	if (status != NULL_COMMAND)
	{
		fprintf(stderr, "Return value is not DONE/FAIL/ERROR\n");
		return -1;
	}
	return DONE;
}

void
JavaScreenCmd::FillInt(char** argv, int& pos, const int x)
{
	char buf[128];
	sprintf(buf, "%d", x);
	argv[pos++] = strdup(buf);
}

void
JavaScreenCmd::FillArgv(char** argv, int& pos, const JavaRectangle& jr)
{
	char	buf[128];
	sprintf(buf,"%.0f", jr._x1);
	argv[pos ++] = strdup(buf);
	sprintf(buf,"%.0f", jr._y1);
	argv[pos ++] = strdup(buf);
	sprintf(buf,"%.0f", jr._x2);
	argv[pos ++] = strdup(buf);
	sprintf(buf,"%.0f", jr._y2);
	argv[pos ++] = strdup(buf);
}

JavaScreenCmd::ControlCmdType
JavaScreenCmd::SendWindowImage(const char* fileName, int& filesize)
{
#if defined (DEBUG)
    printf("JavaScreenCmd::SendWindowImage(%s)\n", fileName);
	fflush(stdout);
#endif

	ControlCmdType	status = DONE;

	// send the dumped image file to the JAVA_SCREEN client
	int	fd = open(fileName, O_RDONLY);
	if (fd <0)
	{
		perror("Image file:");
		status = ERROR;
		errmsg = "{Cannot open image file}";
	}
	else
	{
		filesize = 0;
		char buf[2048];
		int	nbytes;
		
		while ((nbytes = read(fd, buf, sizeof(buf)))>0)
		{
			DeviseServer*	server;
			server = cmdContainerp->getDeviseServer();
			if (server->WriteImagePort(buf, nbytes) <0)
			{
				status = FAIL;
				break;
			}
			filesize += nbytes;
		}
		close(fd);
	}

#if defined(DEBUG)
    printf("  done sending window image\n");
	fflush(stdout);
#endif

	return status;
}

JavaScreenCmd::ControlCmdType 
JavaScreenCmd::RequestUpdateSessionList(int argc, char** argv)
{
	ReturnVal(argc, argv);
	return DONE;
}

JavaScreenCmd::ControlCmdType
JavaScreenCmd::RequestCreateWindow(JavaWindowInfo& winInfo)
{
#if defined (DEBUG)
    printf("JavaScreenCmd::RequestCreateWindow(%s)\n",
	winInfo._winName.c_str());
	fflush(stdout);
#endif

	const char*	fileName = winInfo._imageName.c_str();
	ControlCmdType	status = DONE;
	int		filesize = 0;

// TEMP maybe the dumping, getting file size, etc., should be put into
// a single function

    ViewWin *viewWin = (ViewWin *)ControlPanel::FindInstance(
		winInfo._winName.c_str());
    if (!viewWin) {
      reportErrNosys("Cannot find window");
      return ERROR;
    }
    
    viewWin->ExportImage(GIF, fileName);




	filesize = getFileSize(fileName);
#if defined(DEBUG)
        printf("filesize = %d\n", filesize);
#endif
	if (filesize >0)
	{
		//
		// Return one command to  the JAVA Screen
		// This is Okay, sicne JAVA Screen will not react until 
		// it sees DONE/ERROR/FAIL
		int		argc = 
				+1 						// CommandName
				+1 						// WinName
				+4 						// Win coordinates
				+1						// Image Size
				+1						// # Views
				+winInfo._views * 5;	// viewname + coordinaes

		char**	argv = new (char*)[argc](NULL);
		int		i;
		int		pos = 0;

		argv[pos++] = strdup(_controlCmdName[CREATEWINDOW]);
		argv[pos++] = strdup(winInfo._winName.c_str());
		FillArgv(argv, pos, winInfo._winRec);
		FillInt(argv, pos, filesize);
		FillInt(argv, pos, winInfo._views);

		for (i =0; i< winInfo._views; ++i)
		{
			JavaViewInfo	*jv;
			jv = &winInfo._viewList[i];
			argv[pos++] = strdup(jv->_viewName.c_str());
			FillArgv(argv, pos, jv->_jr);
		}
		if (pos != argc)
		{
			fprintf(stderr, "Error in perparing create window command\n");
		}

		// Send a image retrieving command to JAVA_Screen
		ReturnVal(argc, argv);
		ControlCmd(DONE);

		// Send back the window image, and omit return value if successful
		status = SendWindowImage(fileName, filesize);
		if (status == DONE)
			status = NULL_COMMAND;

		// free all ..
		for (i=0; i< argc; ++i)
		{
			free(argv[i]);
		}
		delete []argv;
	}
	return status;
}


JavaScreenCmd::ControlCmdType
JavaScreenCmd::RequestUpdateWindow(char* winName, int imageSize)
{
#if defined (DEBUG)
    printf("JavaScreenCmd::RequestUpdateWindow(%s, %d)\n", winName, imageSize);
	fflush(stdout);
#endif

	char* argv[3];
	int	pos = 0;
	argv[pos++] = _controlCmdName[UPDATEWINDOW];
	argv[pos++] = winName;
	FillInt(argv, pos, imageSize);
	ReturnVal(3, argv);
	free(argv[2]);
	return DONE;
}

void
JavaScreenCmd::ReturnVal(int argc, char** argv)
{
	static	char* buf = NULL;
	static	int bufsize = 0;
	int		eleSize = 0;

	// Append end-of-command marker
	char** nargv = new (char*)[argc](NULL);
	int	i;
	for (i=0; i< argc-1; ++i)
		nargv[i] = argv[i];

	eleSize = strlen(argv[argc-1]);

	if ((buf == NULL)||(eleSize > bufsize))
	{
		if (!buf)
			delete buf;
		buf = new char[strlen(argv[argc-1])+3];
	}

	// We can also send back multiple commands by seperating them with "\n"
	sprintf(buf,"%s",argv[argc-1]);
	nargv[argc-1]= buf;

	// send the command out
	_control->ReturnVal(API_JAVACMD, argc, nargv, true);
	delete []nargv;
}

void
JavaScreenCmd::CloseJavaConnection()
{
	DeviseServer*	server;
	server = cmdContainerp->getDeviseServer();
	server->CloseClient();
}

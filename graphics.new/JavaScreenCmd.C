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
 */
#include <stdlib.h>
#include <string>
#include <vector>
#include "JavaScreenCmd.h"
#include "DeviseServer.h"
#include "ClientAPI.h"

JavaWindowInfo::JavaWindowInfo(JavaRectangle& winRec, string& winName,
	string imageName, int views, ...)
{
	_winRec = winRec;
	_winName = winName;
	_imageName = imageName;
	_views 	= views;
	_viewList = new JavaViewInfo[views];

	va_list		pvar;
	va_start(pvar, views);
	int i;
	for (i=0; i< views; ++i)
	{
		void* cmd;
		cmd = va_arg(pvar, void*);
		_viewList[i] = *(ViewInfo*)cmd;
	}
	va_end(pvar);
}

JavaWindowInfo::~JavaWindowInfo(0
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
JavaScreenCmd::JavaScreenCmd(DeviseServer* control, 
	ServiceCmdType ctype, int argc, char** argv)
{
	_control  = control;
	_ctype = ctype;
	_argc = argc;
	_argv = argv;
	errmsg = NULL;
}

int
JavaScreenCmd::Run()
{
	_status = DONE;
	switch (_ctype)
	{
		case GETSESSIONLIST:
			GetSession();
		case OPENSESSION:
			OpenSession();
		case MOUSEACTION_CLICK:
			MouseAction_Click();
		case MOUSEACTION_DOUBLECLICK:
			MouseAction_DoubleClick();
		case MOUSEACTION_RUBBERBAND:
			MouseAction_RubberBand();
		default:
	}

	// return either DONE/ERROR/FAIL to current JAVA client
	if (_status == DONE)
	{
		_control->ReturnVal(1,&_controlCmdName[DONE]);
		return 1;
	}
	if (_status == FAIL)
	{
		ReturnVal(1, &_controlCmdName[FAIL]);
		return -1;
	}
	if (_status == ERROR)
	{
		const char *argv[2] = {
			&_controlCmdname[ERROR],
			errmsg
		};	
		if (argv[1] == NULL)
		{
			argv[1]= "Error, but no error message available";
		}
		ReturnVal(2, &argv[0]);
		return -1;
	}
}

void
JavaScreenCmd::GetSessionList()
{
	// TODO:
	// export all sessions avialable for JAVA screen
	//
	char* argv[4] =
	{
		_controlCmdName[UPDATESESSIONLIST],
		"/p/devise/session/AAAcolor2.ds",
		"/p/devise/session/AAAcolor3.ds",
		"/p/devise/session/JFK2.ds"
	};
	_status = RequestUpdateSessionList(4, argv);	
}

void
JavaScreenCmd::OpenSession()
{
	if (argc != 1)
	{
		errmsg = "Usage: OpenSession <session name>";
		_status = ERROR;
		return;
	}
	// TODO:
	// open the right session here with session name 
	//
	printf("Session:%s requested!\n", argv[0]);

	// TODO:
	// Dump all window images to files: e.g. window1.gif, window2.gif...
	//
	// 
	// Example of preparing information to send to JAVA_Screen for one window
	// For a real impementation, we will do this for as many window for 
	// a session
	//
	JavaRectangle winRec(0,0,400,300);
	JavaRectangle viewRec1(0,0,200,300);
	JavaRectangle viewRec2(200,0,400,300);

	string	winName("Win");
	string	imageName("window1.gif");
	string	viewName1("View1");
	string	viewName2("View2");

	JavaViewInfo viewInfo1(viewRec1, viweName1);
	JavaViewInfo viewInfo2(viewRec2, viweName2);

	JavaWindowInfo winInfo(
					winName,
					imageName, winRec, 
					2, (void*)&viewInfo1, (void*)&viewInfo2);

	// send image to JAVA_Screen and one command to it
	_status = RequestCreateWindow(winInfo);
	if (_status != DONE)
		return;
	else
	{
		// TODO:
		//.... You can send more window images for a session here.... 
	}
}

void
JavaScreenCmd::FillInt(char** argv, int& pos, const int x)
{
	char buf[128];
	sprintf(buf, "%d", x);
	argv[pos++] = strdup(buf);
}

void
JavaScreenCmd::FillArgv(char** argv, int& pos, const JavaRectange& jr)
{
	char	buf[128];
	sprintf(buf,"%f", jr._x1);
	argv[pos ++] = strdup(buf);
	sprintf(buf,"%f", jr._y1);
	argv[pos ++] = strdup(buf);
	sprintf(buf,"%f", jr._x2);
	argv[pos ++] = strdup(buf);
	sprintf(buf,"%f", jr._y2);
	argv[pos ++] = strdup(buf);
}

ControlCmdType
JavaScreenCmd::SendWindowImage(char* fileName, int& filesize)
{
	ControlCmdType	status = DONE;

	// send the dumped image file to the JAVA_SCREEN client
	int	fd = open(fileName, O_RDONLY);
	if (fd <0)
	{
		perror("Image file:");
		status = ERROR;
		errmsg = "Cannot open image file";
	}
	else
	{
		filesize = 0;
		char buf[1024];
		while (nbytes = read(fd, buf, sizeof(buf)))
		{
			_control->WriteImagePort(buf, nbytes);
			filesize += nbytes;
		}
		close(fd);
	}
	return status;
}

ControlCmdType 
JavaScreenCmd::RequestUpdateSessionList(int argc, char** argv)
{
	ReturnVal(argc, argv);
	return DONE;
}
ControlCmdType
JavaScreenCmd::RequestCreateWindow(JavaWindowInfo& winInfo)
{
	char*	fileName = winInfo._imageName.c_str();
	ControlCmdType	status;
	int		filesize = 0;

	status = SendWindowImage(fileName, filesize);

	if (status == DONE)
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
				+_views * 5 			// viewname + coordinaes

		char**	argv = new argv[argc](NULL);
		int		i;
		int		pos = 0;

		argv[pos++] = strdup(_controlCmdName[CREATEWINDOW]);
		argv[pos++] = strdup(_winName);
		FillArgv(argv, pos, _winRec);
		FillInt(argv, pos, filesize);
		FillInt(argv, pos, views);

		for (i =0; i< views; ++i)
		{
			JavaViewInfo	*jv;
			jv = &_viweList[i];
			argv[pos++] = strdup(jv->_viewName.c_str());
			FillArgv(argv, pos, jv->_jr);
		}
		if (pos != cnt)
		{
			fprintf(stderr, "Error in perparing create window command\n");
		}

		// Send a command to JAVA_Screen
		ReturnVal(argc, argv);

		// free all ..
		for (i=0; i< argc; ++i)
		{
			free argv[i];
		}
		delete []argv;
	}
	return status;
}

void
JavaScreenCmd::MouseAction_Click()
{
	if (argc != 3)
	{
		errmsg = "Usage: MouseAction_Click <view name> <x> <y>";
		_status = ERROR;
		return;
	}

	// TODO:
	// Select a view based on view name and return TData Record Value
	TDataVal	tdval = 3.1415926;
	_status = RequestUpdateRecordValue(tdval);
}

void
JavaScreenCmd::MouseAction_DoubleClick()
{
	if (argc != 3)
	{
		errmsg = "Usage: MouseAction_DoubleClick <view name> <x> <y>";
		_status = ERROR;
		return;
	}

	// TODO:
	// Select a view and return all the gdata associated with the view
	GDataVal	gval;
	_status = RequestUpdateGData(gval);
}

void
JavaScreenCmd::MouseAction_RubberBand()
{
	if (argc != 5)
	{
		errmsg = "Usage: MouseAction_DoubleClick <view name>"
				 " <x1> <y1> <x2> <y2>";
		_status = ERROR;
		return;
	}

	// TODO:
	// select the view, perform rubber band, and generate its GIF for
	// the window it is in
	// EXAMPLE:
	char *fileName = "window2.gif";
	int	filesize;

	_status = SendWindowImage(fileName, filesize);
	if (_status == DONE)
	{
		_status =RequestUpdateWindow(_argv[1] ,filesize);
	}
}

ControlCmdType
JavaScreenCmd::RequestUpdateGData(GDataVal gval)
{
	// TODO:
	// Same problem as TData val, we need to define 
	// GData table format, and serialize it and have 
	// JavaScreen understand its format
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
	return DONE;
}

ControlCmdType
JavaScreenCmd::RequestUpdateRecordValue(TDataVal tval)
{
	//TODO: we need find a good protocols for serializing
	// a TData tuple, and on the other side, JAVA Screen
	// need to de-serialize it
	char buf[128];
	sprintf(buf, "%f", tval);
	char* argv[2] = {
		_controlCmdName[UPDATERECORDVALUE],
		buf
	};
	ReturnVal(2, argv);
	return DONE;
}

ControlCmdType
JavaScreenCmd::RequestUpdateWindow(char* winName, int imageSize)
{
	char* argv[3];
	int	pos = 0;
	argv[pos++] = _controlCmdName[UPDATEWINDOW];
	argv[pos++] = winName;
	FillInt(argv, pos, imageSize);
	ReturnVal(2, argv);
	free(argv[1]);
	return DONE;
}
void
JavaScreenCmd::ReturnVal(int argc, char** argv)
{
	_control->ReturnVal(API_JAVACMD, argc, argv, true);
}

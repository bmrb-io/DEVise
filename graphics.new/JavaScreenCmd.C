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

off_t getFileSize(const char* filename);
void
JavaScreenCmd::GetSessionList()
{
	// ADD---begin
	char* argv[4] =
	{
		_controlCmdName[UPDATESESSIONLIST],
		"/p/devise/session/AAAcolor2.ds",
		"/p/devise/session/AAAcolor3.ds",
		"/p/devise/session/JFK2.ds"
	};



	// ADD---end
	_status = RequestUpdateSessionList(4, argv);	
}

void
JavaScreenCmd::OpenSession()
{
	if (_argc != 1)
	{
		errmsg = "{Usage: OpenSession <session name>}";
		_status = ERROR;
		return;
	}

	// Add---begin



	// Dump all window images to files: e.g. window1.gif, window2.gif...
	printf("Session:%s requested!\n", _argv[0]);
	JavaRectangle winRec(0,0,400,300);
	JavaRectangle viewRec1(0,0,200,300);
	JavaRectangle viewRec2(200,0,400,300);

	string	winName("Testwindow");
	string	imageName("window1.gif");
	string	viewName1("View1");
	string	viewName2("View2");

	JavaViewInfo viewInfo1(viewRec1, viewName1);
	JavaViewInfo viewInfo2(viewRec2, viewName2);

	JavaWindowInfo winInfo(
					winRec,
					winName,
					imageName,
					2, (void*)&viewInfo1, (void*)&viewInfo2);

	// send one image to JAVA_Screen 
	_status = RequestCreateWindow(winInfo);
	if (_status != DONE)
		return;
	else
	{
		//.... You can send more window images here....
	}

	// Add---end
}

void
JavaScreenCmd::MouseAction_Click()
{
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
	if (_argc != 5)
	{
		errmsg = "{Usage: MouseAction_RubberBand <view name>}"
				 " <x1> <y1> <x2> <y2>";
		_status = ERROR;
		return;
	}

	// ADD---begin
	char *fileName = "window2.gif";

	
	
	
	
	// ADD---end
	int	filesize;
	filesize = getFileSize(fileName);
	if (filesize >0)
	{
		_status =RequestUpdateWindow(_argv[1] ,filesize);
		ControlCmd(_status);
		if (_status == DONE)
		{
			_status = SendWindowImage(fileName, filesize);
		}

		// avoid unnecessary JAVAC_Done command, after sending back images
		if (_status == DONE)
			_status = NULL_COMMAND;
	}
}

void
JavaScreenCmd::CloseCurrentSession()
{
	// ADD---Begin






	// ADD---End
	_status = DONE;
	return;
}

//====================================================================
JavaScreenCmd::ControlCmdType
JavaScreenCmd::RequestUpdateGData(GDataVal gval)
{
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
	string& imageName, int views, ...)
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
		_viewList[i] = *(JavaViewInfo*)cmd;
	}
	va_end(pvar);
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
	const char*	fileName = winInfo._imageName.c_str();
	ControlCmdType	status = DONE;
	int		filesize = 0;

	filesize = getFileSize(fileName);
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
	_control->ReturnVal(API_JAVACMD, argc, nargv, false);
	delete []nargv;
}

void
JavaScreenCmd::CloseJavaConnection()
{
	DeviseServer*	server;
	server = cmdContainerp->getDeviseServer();
	server->CloseClient();
}

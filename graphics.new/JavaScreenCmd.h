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
  Revision 1.6  1998/05/29 19:34:40  wenger
  Added JAVAC_SetDisplaySize to allow the JavaScreen to set the display
  size.

  Revision 1.5  1998/05/14 18:21:44  wenger
  New protocol for JavaScreen opening sessions works (sending "real" GIF)
  except for the problem of spaces in view and window names.

  Revision 1.4  1998/05/08 17:16:23  taodb
  Added stripping functions for "{}"

  Revision 1.3  1998/05/05 17:07:25  wenger
  Minor improvements to JavaScreenCmd.[Ch].

  Revision 1.2  1998/05/02 09:00:45  taodb
  Added support for JAVA Screen and command logging

  Revision 1.1  1998/04/25 05:45:34  taodb
  Initial Revision

 */
#ifndef _JAVA_SCREEN_CMD
#define _JAVA_SCREEN_CMD
#include <string>

typedef double TDataVal;
typedef string GDataVal;

class DeviseServer;
class ControlPanel;

class JavaRectangle
{
	public:
		JavaRectangle(){}
		JavaRectangle(double x1, double y1, double x2, double y2):
			_x1(x1),_y1(y1), _x2(x2),_y2(y2){};
		JavaRectangle& operator =(const JavaRectangle& jr)
		{
			_x1 = jr._x1;
			_y1 = jr._y1;
			_x2 = jr._x2;
			_y2 = jr._y2;
			return *this;
		}
		double _x1;
		double _y1;
		double _x2;
		double _y2;
};

class JavaViewInfo
{
	public:
		JavaViewInfo(){}
		JavaViewInfo(JavaRectangle& jr, string& viewName)
		{
			_jr = jr;
			_viewName = viewName;
		}
		JavaViewInfo& operator = (const JavaViewInfo& info)
		{
			_jr = info._jr;
			_viewName = info._viewName;
			return *this;
		}
		JavaRectangle	_jr;
		string			_viewName;
};

class JavaWindowInfo
{
	public:
		JavaWindowInfo(JavaRectangle& winRec, string& winName,
			string& imageName, int viewCount, JavaViewInfo **views);
		~JavaWindowInfo();
		string			_winName;
		string			_imageName;
		JavaRectangle	_winRec;
		int				_views;
		JavaViewInfo*	_viewList;
};

class JavaScreenCmd
{
	public:
		typedef enum 
		{
			GETSESSIONLIST, 
			OPENSESSION, 
			MOUSEACTION_CLICK,
			MOUSEACTION_DOUBLECLICK, 
			MOUSEACTION_RUBBERBAND,
			JAVAEXIT,
			CLOSECURRENTSESSION,
			SETDISPLAYSIZE
		}ServiceCmdType;

		typedef enum
		{
			UPDATESESSIONLIST = 0,
			CREATEWINDOW,
			UPDATERECORDVALUE,
			UPDATEGDATA,
			UPDATEWINDOW,
			DONE,
			ERROR,
			FAIL,
			CONTROLCMD_NUM,
			NULL_COMMAND
		}ControlCmdType;

		// argv does not contain the command name!
		~JavaScreenCmd();
		JavaScreenCmd(ControlPanel* server,
			ServiceCmdType ctype, int argc, char** argv);
		int Run();
		static char* JavaScreenCmdName(JavaScreenCmd::ControlCmdType);
	private:
		static char* _controlCmdName[CONTROLCMD_NUM];
		ControlPanel	*_control;
		int				_ctype;
		int				_argc;
		char** 			_argv;

		ControlCmdType	_status;
		char			*errmsg;

		// JavaScreen->Server Requests
		void GetSessionList();
		void CloseCurrentSession();
		void OpenSession();
		void MouseAction_Click();
		void MouseAction_DoubleClick();
		void MouseAction_RubberBand();
		void SetDisplaySize();

		// Server->JavaScreen Control Commands
		ControlCmdType RequestUpdateSessionList(int argc, char** argv);
		ControlCmdType RequestCreateWindow(JavaWindowInfo& winInfo);
		ControlCmdType RequestUpdateRecordValue(TDataVal tval);
		ControlCmdType RequestUpdateGData(GDataVal gval);
		ControlCmdType RequestUpdateWindow(char* winName, int imageSize);

		// Convenience functions
		void CloseJavaConnection();
		ControlCmdType SendWindowImage(const char* fileName, int& filesize);
		ControlCmdType SendChangedWindows();
		void FillArgv(char** argv, int& pos, const JavaRectangle& jr);
		void FillInt(char** argv, int& pos, int i);
		int  ControlCmd(ControlCmdType  status);
		void ReturnVal(int argc, char** argv);
};
#endif

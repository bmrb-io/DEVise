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
#ifndef _JAVA_SCREEN_CMD
#define _JAVA_SCREEN_CMD
#include <string>
#include <sys/varargs.h>
#else
#include <stdarg.h>
#endif

typedef double TDataVal;
typedef string GDataVal;
class DeviseServer;
class JavaRectangle
{
	public:
		JavaRectangle(double x1, double y1, double x2, double y2):
			_x1(x1),_y1(y1), _x2(x2),_y2(y2){};
		JavaRectangle& operator =(const JavaRectangle* jr)
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
		}
		JavaRectangle	_jr;
		string			_viewName;
};

class JavaWindowInfo
{
	public:
		JavaWindowInfo(JavaRectangle& winRec, string& winName,
			string imageName, int views, ...);
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
			MOUSEACTION_CLIK,
			MOUSEACTION_DOUBLECLICK, 
			MOUSEACTION_RUBERBAND
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
			CONTROLCMD_NUM
		}ControlCmdType;

		// argv does not contain the command name!
		JavaScreenCmd(DeviseServer* control, 
			CmdType ctype, int argc, char** argv);
		int Run();
	private:
		static char* _controlCmdName[CONTROLCMD_NUM];
		DeviseServer*	_control;
		CmdType			_ctype
		int				_argc;
		char** 			_argv;

		ControlCmdType	_status;
		char			*errmsg;

		// JavaScreen->Server Requests
		void GetSessionList();
		void OpenSession();
		void MouseAction_Click();
		void MouseAction_DoubleClick();
		void MouseAction_RubberBand();

		// Server->JavaScreen Control Commands
		ControlCmdType RequestUpdateSessionList(int argc, char** argv);
		ControlCmdType RequestCreateWindow(JavaWindowInfo& winInfo);
		ControlCmdType RequestUpdateRecordValue(TDataVal tval);
		ControlCmdType RequestUpdateGData(GDataVal gval);
		ControlCmdType RequestUpdateWindow(string winName, int imageSize);

		// Convenience functions
		ControlCmdType SendWindowImage(JavaWindowInfo& winInfo);
		void FillArgv(char** argv, int& pos, const JavaRectangle& jr);
		void FillInt(char** argv, int& pos, int i);
		void ReturnVal(int argc, char** argv);
};
#endif

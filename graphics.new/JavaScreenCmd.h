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
  Revision 1.12  1998/08/25 20:56:32  wenger
  Implemented support for JavaScreen cursors (not yet fully tested).

  Revision 1.11  1998/08/24 14:51:33  wenger
  Implemented support for JavaScreen drill-down.

  Revision 1.10  1998/08/13 18:14:46  wenger
  More updates to JavaScreen support.

  Revision 1.9  1998/08/03 18:38:40  wenger
  Implemented JAVAC_ServerExit and JAVAC_SaveSession commands; partly
  implemented several other new commands for the JavaScreen.

  Revision 1.8  1998/06/17 17:20:42  wenger
  Devised now sends "real" session file list to JavaScreen.

  Revision 1.7  1998/06/03 17:09:41  wenger
  Rubberband line in JavaScreen now sends updates of all changed windows
  using the "dirty GIF" flag; updated DEVise version to 1.5.3.

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

#include "DeviseTypes.h"

typedef double TDataVal;
typedef string GDataVal;

class DeviseServer;
class ControlPanel;

class JavaRectangle
{
	public:
		JavaRectangle(){}
		JavaRectangle(double x0, double y0, double width, double height):
			_x0(x0),_y0(y0), _width(width),_height(height){};
		JavaRectangle& operator =(const JavaRectangle& jr)
		{
			_x0 = jr._x0;
			_y0 = jr._y0;
			_width = jr._width;
			_height = jr._height;
			return *this;
		}
		double _x0;
		double _y0;
		double _width;
		double _height;
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

class DeviseCursor;

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
			SETDISPLAYSIZE,
			KEYACTION,
			SAVESESSION,
			SERVEREXIT,
			SERVERCLOSESOCKET,
			IMAGECHANNEL,
			NULL_SVC_CMD
		}ServiceCmdType;

		typedef enum
		{
			UPDATESESSIONLIST = 0,
			CREATEWINDOW,
			UPDATERECORDVALUE,
			UPDATEGDATA,
			UPDATEWINDOW,
			DRAWCURSOR,
			ERASECURSOR,
			DONE,
			ERROR,
			FAIL,
			UPDATEIMAGE,
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

		static Boolean	_openingSession;

		// JavaScreen->Server Requests
		void GetSessionList();
		void CloseCurrentSession();
		void OpenSession();
		void MouseAction_Click();
		void MouseAction_DoubleClick();
		void MouseAction_RubberBand();
		void SetDisplaySize();
		void KeyAction();
		void SaveSession();
		void ServerExit();
		void ServerCloseSocket();
		void ImageChannel();

		// Server->JavaScreen Control Commands
		ControlCmdType RequestUpdateSessionList(int argc, char** argv);
		ControlCmdType RequestCreateWindow(JavaWindowInfo& winInfo);
		ControlCmdType RequestUpdateRecordValue(int argc, char** argv);
		ControlCmdType RequestUpdateGData(GDataVal gval);
		ControlCmdType RequestUpdateWindow(char* winName, int imageSize);

		// Convenience functions
		void CloseJavaConnection();
		ControlCmdType SendWindowImage(const char* fileName, int& filesize);
		ControlCmdType SendChangedWindows();
		static void FillArgv(char** argv, int& pos, const JavaRectangle& jr);
		static void FillInt(char** argv, int& pos, int i);
		int  ControlCmd(ControlCmdType  status);
		void ReturnVal(int argc, char** argv);
		void UpdateSessionList(char *dirName);
		void DrawAllCursors();

	protected:
		friend class View;

		static void DrawCursor(View *view, DeviseCursor *cursor);
		static void EraseCursor(View *view, DeviseCursor *cursor);
};
#endif

/*
  ========================================================================
  DEVise Data Visualization Software
  (c) Copyright 1998-1999
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
  Revision 1.52  1999/03/18 17:30:40  wenger
  Implemented two-color option in HighLow symbols; compensated for
  XFillRectangle() not working the way O'Reilly says it does (subtracted
  one from width and height); test command for Condor User data visualization.

  Revision 1.51  1999/03/16 17:10:03  wenger
  Improved 'view home' configuration: user can select whether home changes
  X, Y, or both parts of visual filter; added explicit option to force Y
  min to 0 in automatic mode; fixed bug 469 (problems with 'home' in
  record link follower views).

  Revision 1.50  1999/03/12 18:46:02  wenger
  Implemented duplicate symbol elimination.

  Revision 1.49  1999/03/10 19:11:05  wenger
  Implemented DataReader schema GUI; made other improvements to schema
  editing GUI, such as sorting the schema lists.

  Revision 1.48  1999/03/04 15:11:10  wenger
  Implemented 'automatic filter update' features: views can be designated
  to have their visual filters automatically updated with the 'Update
  Filters' menu selection; alternatively, a session can be opened with
  the 'Open, Update, and Save' selection, which updates the designated
  filters and saves the updated session.

  Revision 1.47  1999/03/01 17:47:44  wenger
  Implemented grouping/ungrouping of views to allow custom view geometries.

  Revision 1.46  1999/02/23 15:35:07  wenger
  Fixed bugs 446 and 465 (problems with cursors in piles); fixed some
  other pile-related problems.

  Revision 1.45  1999/02/11 21:43:29  wenger
  Avoid inserting views into old-style pile links.

  Revision 1.44  1999/02/11 19:54:55  wenger
  Merged newpile_br through newpile_br_1 (new PileStack class controls
  pile and stacks, allows non-linked piles; various other improvements
  to pile-related code).

  Revision 1.43  1999/01/04 15:33:28  wenger
  Improved View symbol code; removed NEW_LAYOUT and VIEW_SHAPE conditional
  compiles; added code (GUI is currently disabled) to manually set view
  geometry (not yet saved to sessions).

  Revision 1.42.2.2  1999/02/11 18:24:19  wenger
  PileStack objects are now fully working (allowing non-linked piles) except
  for a couple of minor bugs; new PileStack state is saved to session files;
  piles and stacks in old session files are dealt with reasonably well;
  incremented version number; added some debug code.

  Revision 1.42.2.1  1998/12/29 17:25:11  wenger
  First version of new PileStack objects implemented -- allows piles without
  pile links.  Can't be saved or restored in session files yet.

  Revision 1.42  1998/12/22 19:39:27  wenger
  User can now change date format for axis labels; fixed bug 041 (axis
  type not being changed between float and date when attribute is changed);
  got dates to work semi-decently as Y axis labels; view highlight is now
  always drawn by outlining the view; fixed some bugs in drawing the highight.

  Revision 1.41  1998/12/18 22:21:01  wenger
  Removed axis label code, which doesn't seem to have been fully implemented,
  and is not used; added sanity check on visual filter at view creation.

  Revision 1.40  1998/12/15 14:55:11  wenger
  Reduced DEVise memory usage in initialization by about 6 MB: eliminated
  Temp.c (had huge global arrays); eliminated Object3D class and greatly
  simplified Map3D; removed ViewLens class (unused); got rid of large static
  buffers in a number of other source files.

  Revision 1.39  1998/12/10 21:53:25  wenger
  Devised now sends GIFs to JavaScreen on a per-view rather than per-window
  basis; GIF "dirty" flags are now also referenced by view rather than by
  window.

  Revision 1.38  1998/12/07 19:43:38  wenger
  Removed "old" layout manager code.

  Revision 1.37  1998/12/01 20:04:30  wenger
  More reductions of memory usage in DEVise -- basically eliminated the
  histogram capability (this really saves a lot, since there are big
  structures in every ViewGraph for this); made creation of TDatas more
  efficient by bypassing command code.

  Revision 1.36  1998/11/19 21:12:49  wenger
  Implemented non-DTE version of DEVise (new code handles data source catalog
  functions; Tables, SQLViews, etc., are not implemented); changed version to
  1.6.0.

  Revision 1.35  1998/11/16 18:58:43  wenger
  Added options to compile without DTE code (NO_DTE), and to warn whenever
  the DTE is called (DTE_WARN).

  Revision 1.34  1998/11/11 14:30:56  wenger
  Implemented "highlight views" for record links and set links; improved
  ClassDir::DestroyAllInstances() by having it destroy all links before
  it destroys anything else -- this cuts down on propagation problems as
  views are destroyed; added test code for timing a view's query and draw.

  Revision 1.33  1998/11/06 17:59:46  wenger
  Multiple string tables fully working -- allows separate tables for the
  axes in a given view.

  Revision 1.32  1998/11/04 20:33:52  wenger
  Multiple string tables partly working -- loading and saving works, one
  table per mapping works; need multiple tables per mapping, API and GUI,
  saving to session, sorting.

  Revision 1.31  1998/11/02 19:22:42  wenger
  Added "range/MQL" session description capability.

  Revision 1.30  1998/10/28 19:22:27  wenger
  Added code to check all data sources (runs through the catalog and tries
  to open all of them); this includes better error handling in a number of
  data source-related areas of the code; also fixed errors in the propagation
  of command results.

  Revision 1.29  1998/10/21 17:16:39  wenger
  Fixed bug 101 (problems with the '5' (home) key); added "Set X, Y to
  Show All" (go home) button to Query dialog; fixed bug 421 (crash when
  closing set link sessions); fixed bug 423 (saving session file over
  directory).

  Revision 1.28  1998/10/20 19:46:15  wenger
  Mapping dialog now displays the view's TData name; "Next in Pile" button
  in mapping dialog allows user to edit the mappings of all views in a pile
  without actually flipping them; user has the option to show all view names;
  new GUI to display info about all links and cursors; added API and GUI for
  count mappings.

  Revision 1.27  1998/10/13 19:40:42  wenger
  Added SetAttrs() function to TData and its subclasses to allow Liping to
  push projection down to the DTE.

  Revision 1.26  1998/09/30 17:44:42  wenger
  Fixed bug 399 (problems with parsing of UNIXFILE data sources); fixed
  bug 401 (improper saving of window positions).

  Revision 1.25  1998/09/22 17:23:54  wenger
  Devised now returns no image data if there are any problems (as per
  request from Hongyu); added a bunch of debug and test code to try to
  diagnose bug 396 (haven't figured it out yet); made some improvements
  to the Dispatcher to make the main loop more reentrant; added some error
  reporting to the xv window grabbing code; improved command-result
  checking code.

  Revision 1.24  1998/09/21 16:47:42  wenger
  Fixed bug 395 (Condorview GIF dumping problem) (caused by waitForQueries
  not returning a value); made Dispatcher::WaitForQueries a little safer;
  added code in DeviseCommand class to print an error message and return
  a default value if a command does not return a value.

  Revision 1.23  1998/09/08 20:26:15  wenger
  Added option to save which view is selected when saving a session -- for
  JavaScreen client switching support.

  Revision 1.22  1998/08/28 22:01:16  wenger
  Made Dispatcher::WaitForQueries() function -- improved over earlier
  versions because it also checks the callback list (this fixes bug 367);
  fixed other piled-related JavaScreen support problems; JAVAC_OpenSession
  closes any existing session before opening the new one.

  Revision 1.21  1998/08/10 19:08:04  wenger
  Moved command result buffer from DeviseCommand class to ControlPanel
  class -- saves 7 MB of memory!

  Revision 1.20  1998/08/03 18:38:38  wenger
  Implemented JAVAC_ServerExit and JAVAC_SaveSession commands; partly
  implemented several other new commands for the JavaScreen.

  Revision 1.19  1998/07/07 17:59:02  wenger
  Moved #define of PURIFY from DeviseCommand.h to DeviseCommand.C so the
  "outside world" doesn't see it.

  Revision 1.18  1998/07/06 21:06:53  wenger
  More memory leak hunting -- partly tracked down some in the DTE.

  Revision 1.17  1998/06/12 19:55:28  wenger
  Attribute of TAttr/set links can now be changed; GUI has menu of available
  attributes; attribute is set when master view is set instead of at link
  creation; misc. debug code added.

  Revision 1.16  1998/06/09 20:06:12  wenger
  2D OpenGL cursor now drawn as shaded outline plus every-other-point
  "mesh"; OpenGL CursorStore and GLWindowRep on SGI now use color indices
  instead of RGB so that they work the same as the other architectures;
  added user interface to allow changing cursor color (merged through
  cursor_test_br_1).

  Revision 1.15  1998/05/29 19:34:38  wenger
  Added JAVAC_SetDisplaySize to allow the JavaScreen to set the display
  size.

  Revision 1.14  1998/05/21 18:18:47  wenger
  Most code for keeping track of 'dirty' GIFs in place; added 'test'
  command to be used for generic test code that needs to be controlled
  by GUI; added debug code in NetworkSend().

  Revision 1.13  1998/05/13 13:36:35  wenger
  Fixed some dynamic memory errors in the csgroup code; cleaned up
  DeviseCommand class somewhat -- simplified the calling of Run()
  functions in subclasses.

  Revision 1.12  1998/05/05 15:15:14  zhenhai
  Implemented 3D Cursor as a rectangular block in the destination view
  showing left, right, top, bottom, front and back cutting planes of the
  source view.

  Revision 1.11  1998/05/02 09:00:40  taodb
  Added support for JAVA Screen and command logging

  Revision 1.10  1998/04/28 18:02:54  wenger
  Added provision for "logical" and "physical" TDatas to mappings,
  instead of creating new mappings for slave views; other TAttrLink-
  related improvements.

  Revision 1.9  1998/04/27 17:30:25  wenger
  Improvements to TAttrLinks and related code.

  Revision 1.8  1998/04/14 21:03:13  wenger
  TData attribute links (aka set links) are working except for actually
  creating the join table, and some cleanup when unlinking, etc.

 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <ctype.h>
#include <assert.h>

#include "DeviseCommand.h"
#include "ParseAPI.h"
#if !defined(NO_DTE)
  #include "TDataDQLInterp.h"
#endif
#include "ClassDir.h"
#include "Control.h"
#include "ViewKGraph.h"
#include "Util.h"
#include "ParseCat.h"
#include "TData.h"
#include "TDataMap.h"
#include "Parse.h"
#include "GroupDir.h"
#include "ViewLayout.h"
#include "VisualLink.h"
#include "RecordLink.h"
#include "FilterQueue.h"
#include "DataSeg.h"
#include "Version.h"
#include "CompDate.h"
#include "DevFileHeader.h"
#include "Display.h"
#include "TDataAscii.h"
#include "DevError.h"
#include "WinClassInfo.h"
#include "VisualLinkClassInfo.h"
#include "CursorClassInfo.h"
#include "MappingInterp.h"
#include "QueryProc.h"

#include "CatalogComm.h"
#include "SessionDesc.h"
#include "StringStorage.h"
#include "DepMgr.h"
#include "Session.h"
#include "GDataSock.h"
#include "Timer.h"
#include "CmdLog.h"
#include "CmdContainer.h"
#include "JavaScreenCmd.h"
#include "TAttrLink.h"
#include "RangeDesc.h"
#include "DataCatalog.h"
#include "Layout.h"
#include "PileStack.h"
#include "ViewGeom.h"
#include "DRUtils.h"

#include "Color.h"
//#define INLINE_TRACE
#include "debug.h"
#define LINESIZE 1024

//#define DEBUG
#define PURIFY 0

#define IMPLEMENT_COMMAND_BEGIN(command) \
int DeviseCommand_##command::Run(int argc, char** argv)\
{

#define IMPLEMENT_COMMAND_END \
	return true;\
}


static ViewKGraph *vkg = 0;
void ResetVkg()
{
	vkg = 0;
}



#if PURIFY
extern "C" int purify_is_running();
extern "C" int purify_new_leaks();
extern "C" int purify_new_inuse();
#endif

int GetDisplayImageAndSize(ControlPanel *control, int port, char *imageType);
int GetWindowImageAndSize(ControlPanel *control, int port, char *imageType,
	char *windowName);
#if !defined(NO_DTE)
  int ParseAPIDTE(int argc, char** argv, ControlPanel* control);
#endif
int ParseAPIColorCommands(int argc, char** argv, ControlPanel* control);


//**********************************************************************
// DeviseCommand base class definition
//**********************************************************************
ostream&
operator <<(ostream& os, const DeviseCommand& cmd)
{
	return os;
}
ControlPanel* DeviseCommand::defaultControl;
ControlPanel*
DeviseCommand::getDefaultControl()
{
	return defaultControl;
}

void
DeviseCommand::setDefaultControl(ControlPanel* defaultCntl)
{
	defaultControl = defaultCntl;
}

int 
DeviseCommand::Run(int argc, char** argv, ControlPanel* cntl)
{
#if defined(DEBUG)
	printf("DeviseCommand::Run(");
	PrintArgs(stdout, argc, argv, false);
	printf(")\n");
#  if defined(DEBUG_MEM)
    printf("  %s: %d; end of data seg = 0x%p\n", __FILE__, __LINE__, sbrk(0));
#  endif
	fflush(stdout);
#endif

	int	retval;

	DevError::ResetError();

	// reset the control each time you run a command
	control = cntl;
	pushControl(cntl);

	classDir = control->GetClassDir();
	DOASSERT(result != control->resultBuf,
	  "Command result buffer conflict");
	result = control->resultBuf;
	result[0] = '\0';

	control->SetValueReturned(false);
	retval = Run(argc, argv);
	if (!control->GetValueReturned()) {
		char errBuf[1024];
		sprintf(errBuf, "No command value returned from command %s\n",
		  argv[0]);
		reportErrNosys(errBuf);
    	ReturnVal(API_NAK, "No command value returned");
	}

	result = NULL;

	// restore the orignal value to control
	popControl();

#if defined(DEBUG)
    printf("  Done with command %s\n", argv[0]);
#  if defined(DEBUG_MEM)
    printf("  %s: %d; end of data seg = 0x%p\n", __FILE__, __LINE__, sbrk(0));
#  endif
#endif

	return retval;
}

int
DeviseCommand::ReturnVal(u_short flag, char *result)
{
#if defined(DEBUG)
    printf("DeviseCommand::ReturnVal(%d, %s)\n", flag, result);
#endif

    return control->ReturnVal(flag, result);
}

int
DeviseCommand::ReturnVal(int argc, char **argv)
{
#if defined(DEBUG)
    printf("DeviseCommand::ReturnVal(");
	PrintArgs(stdout, argc, argv, false);
	printf(")\n");
#endif

    return control->ReturnVal(argc, argv);
}

//**********************************************************************
// Conventions for Developing Devise Command Objects:
//======================================================================
// 1. Define subclass based on DeviseCommand Object 
//		(see DeviseCommand.h)
// 2. Define Run() of the subclass 
//		(see this file)
// 		.In your definition, always call base class's Run() first
//      .If the return value is false, sub-class Run() should return
//		.I provided two Macros IMPLEMENT_COMMAND_BEGIN and 
//		IMPLEMENT_COMMAND_END to facilitate use
// 3. Register this command object with CmdContainer Object: 
//		(see CmdContainer.C)
//		.REGISTER_COMMANND : use default command option
//		.REGISTER_COMMAND_WITH_OPTION: specify your own option

IMPLEMENT_COMMAND_BEGIN(JAVAC_GetSessionList)
	JavaScreenCmd jc(control,JavaScreenCmd::GETSESSIONLIST,
		argc-1, &argv[1]);
	return jc.Run();
IMPLEMENT_COMMAND_END

IMPLEMENT_COMMAND_BEGIN(JAVAC_Exit)
	JavaScreenCmd jc(control,JavaScreenCmd::JAVAEXIT,
		argc-1, NULL);
	return jc.Run();
IMPLEMENT_COMMAND_END

IMPLEMENT_COMMAND_BEGIN(JAVAC_CloseCurrentSession)
	JavaScreenCmd jc(control,JavaScreenCmd::CLOSECURRENTSESSION,
		argc-1, &argv[1]);
	return jc.Run();
IMPLEMENT_COMMAND_END

IMPLEMENT_COMMAND_BEGIN(JAVAC_OpenSession)
	JavaScreenCmd jc(control,JavaScreenCmd::OPENSESSION,
		argc-1, &argv[1]);
	return jc.Run();
IMPLEMENT_COMMAND_END

IMPLEMENT_COMMAND_BEGIN(JAVAC_MouseAction_Click)
	JavaScreenCmd jc(control,JavaScreenCmd::MOUSEACTION_CLICK,
		argc-1, &argv[1]);
	return jc.Run();
IMPLEMENT_COMMAND_END

IMPLEMENT_COMMAND_BEGIN(JAVAC_MouseAction_DoubleClick)
	JavaScreenCmd jc(control,JavaScreenCmd::MOUSEACTION_DOUBLECLICK,
		argc-1, &argv[1]);
	return jc.Run();
IMPLEMENT_COMMAND_END

IMPLEMENT_COMMAND_BEGIN(JAVAC_MouseAction_RubberBand)
	JavaScreenCmd jc(control,JavaScreenCmd::MOUSEACTION_RUBBERBAND,
		argc-1, &argv[1]);
	return jc.Run();
IMPLEMENT_COMMAND_END

IMPLEMENT_COMMAND_BEGIN(JAVAC_SetDisplaySize)
	JavaScreenCmd jc(control,JavaScreenCmd::SETDISPLAYSIZE,
		argc-1, &argv[1]);
	return jc.Run();
IMPLEMENT_COMMAND_END

IMPLEMENT_COMMAND_BEGIN(JAVAC_KeyAction)
	JavaScreenCmd jc(control,JavaScreenCmd::KEYACTION,
		argc-1, &argv[1]);
	return jc.Run();
IMPLEMENT_COMMAND_END

IMPLEMENT_COMMAND_BEGIN(JAVAC_SaveSession)
	JavaScreenCmd jc(control,JavaScreenCmd::SAVESESSION,
		argc-1, &argv[1]);
	return jc.Run();
IMPLEMENT_COMMAND_END

IMPLEMENT_COMMAND_BEGIN(JAVAC_ServerExit)
	JavaScreenCmd jc(control,JavaScreenCmd::SERVEREXIT,
		argc-1, &argv[1]);
	return jc.Run();
IMPLEMENT_COMMAND_END

IMPLEMENT_COMMAND_BEGIN(JAVAC_ServerCloseSocket)
	JavaScreenCmd jc(control,JavaScreenCmd::SERVERCLOSESOCKET,
		argc-1, &argv[1]);
	return jc.Run();
IMPLEMENT_COMMAND_END

IMPLEMENT_COMMAND_BEGIN(JAVAC_ImageChannel)
	JavaScreenCmd jc(control,JavaScreenCmd::IMAGECHANNEL,
		argc-1, &argv[1]);
	return jc.Run();
IMPLEMENT_COMMAND_END

IMPLEMENT_COMMAND_BEGIN(JAVAC_CursorChanged)
	JavaScreenCmd jc(control,JavaScreenCmd::CURSORCHANGED,
		argc-1, &argv[1]);
	return jc.Run();
IMPLEMENT_COMMAND_END

//**********************************************************************
// DTE Command objects
//**********************************************************************
/*
// alternatively, we can write this, instead of the Macro
int
DeviseCommand_dteImportFileType::Run(int argc, char** argv)
{
*/

IMPLEMENT_COMMAND_BEGIN(dteImportFileType)
		if (argc ==2) 
        {
#if defined(DTE_WARN)
  fprintf(stderr, "Warning: calling DTE at %s: %d\n", __FILE__, __LINE__);
#endif
#if !defined(NO_DTE)
    		 char * name = dteImportFileType(argv[1]);
#else
			 char *name = NULL;
#endif
    		 if (!name){
    		ReturnVal(API_NAK, (char *)DevError::GetLatestError());
    		return -1;
    		 }
    		 ReturnVal(API_ACK, name);
    		 return 1;
    	}
#if defined(DTE_WARN)
  fprintf(stderr, "Warning: calling DTE at %s: %d\n", __FILE__, __LINE__);
#endif
#if !defined(NO_DTE)
		return ParseAPIDTE(argc, argv, control);
#else
        return false;
#endif
IMPLEMENT_COMMAND_END

/*
	// alternatively, we can write this, instead of the Macro
    return true;
}
*/

int
DeviseCommand_dteListAllIndexes::Run(int argc, char** argv)
{
    {
/*TAG
        {
    			char* retVal = dteListAllIndexes(argv[1]);
    			ReturnVal(API_ACK, retVal);
    			return 1;
    		}
TAG*/
#if defined(DTE_WARN)
  fprintf(stderr, "Warning: calling DTE at %s: %d\n", __FILE__, __LINE__);
#endif
#if !defined(NO_DTE)
        return ParseAPIDTE(argc, argv, control);
#else
        return false;
#endif
    }
    return true;
}
int
DeviseCommand_dteDeleteCatalogEntry::Run(int argc, char** argv)
{
    {
/*TAG
        {
          dteDeleteCatalogEntry(argv[1]);
          ReturnVal(API_ACK, "");
          return 1;
        }
TAG*/
#if defined(DTE_WARN)
  fprintf(stderr, "Warning: calling DTE at %s: %d\n", __FILE__, __LINE__);
#endif
#if !defined(NO_DTE)
        return ParseAPIDTE(argc, argv, control);
#else
		int result = DataCatalog::Instance()->DeleteEntry(argv[1]);
		if (result == 0) {
          ReturnVal(API_ACK, "");
    	  return 1;
		} else {
          ReturnVal(API_NAK, (char *)DevError::GetLatestError());
    	  return -1;
		}
#endif
    }
    return true;
}
int
DeviseCommand_dteMaterializeCatalogEntry::Run(int argc, char** argv)
{
    {
/*TAG
        {
          dteMaterializeCatalogEntry(argv[1]);
          ReturnVal(API_ACK, "");
          return 1;
        }
TAG*/
#if defined(DTE_WARN)
  fprintf(stderr, "Warning: calling DTE at %s: %d\n", __FILE__, __LINE__);
#endif
#if !defined(NO_DTE)
        return ParseAPIDTE(argc, argv, control);
#else
        return false;
#endif
    }
    return true;
}
int
DeviseCommand_dteReadSQLFilter::Run(int argc, char** argv)
{
    {
/*TAG
        {
          char* retVal = dteReadSQLFilter(argv[1]);
          ReturnVal(API_ACK, retVal);
          return 1;
        }
TAG*/
#if defined(DTE_WARN)
  fprintf(stderr, "Warning: calling DTE at %s: %d\n", __FILE__, __LINE__);
#endif
#if !defined(NO_DTE)
        return ParseAPIDTE(argc, argv, control);
#else
        return false;
#endif
    }
    return true;
}
int
DeviseCommand_dteShowCatalogEntry::Run(int argc, char** argv)
{
    {
/*TAG
        {
    
    	// TEMP -- Kent, I traced the type 11 error to here...
    
    		char* catEntry = dteShowCatalogEntry(argv[1]);
    		CATCH(
    			string err = currExcept->toString();
    			ReturnVal(API_NAK, (char*) err.c_str());
    			return -1;
    		)
    		ReturnVal(API_ACK, catEntry);
    		delete [] catEntry;
    		return 1;
    	}
TAG*/
#if defined(DTE_WARN)
  fprintf(stderr, "Warning: calling DTE at %s: %d\n", __FILE__, __LINE__);
#endif
#if !defined(NO_DTE)
        return ParseAPIDTE(argc, argv, control);
#else
	char *catEntry = DataCatalog::Instance()->ShowEntry(argv[1]);
	if (catEntry != NULL) {
   	  ReturnVal(API_ACK, catEntry);
   	  delete [] catEntry;
   	  return 1;
	} else {
   	  ReturnVal(API_ACK, "");
   	  return 1;
	}
#endif
    }
    return true;
}
int
DeviseCommand_dteListCatalog::Run(int argc, char** argv)
{
    {
/*TAG
        {
    		int errorCode;	// 0 means OK
    		char* catListing = dteListCatalog(argv[1], errorCode);
    		if(errorCode){
    			ReturnVal(API_NAK, catListing);
    			delete [] catListing;
    			return -1;
    		}
    		else{
    			ReturnVal(API_ACK, catListing);
    			delete [] catListing;
    			return 1;
    		}
    	}
TAG*/
#if defined(DTE_WARN)
  fprintf(stderr, "Warning: calling DTE at %s: %d\n", __FILE__, __LINE__);
#endif
#if !defined(NO_DTE)
        return ParseAPIDTE(argc, argv, control);
#else
    char *catListing = DataCatalog::Instance()->ListCatalog(argv[1]);
	if (catListing != NULL) {
      ReturnVal(API_ACK, catListing);
	  delete [] catListing;
      return 1;
	} else {
      ReturnVal(API_NAK, (char *)DevError::GetLatestError());
      return -1;
	}
#endif
    }
    return true;
}
int
DeviseCommand_dteListQueryAttributes::Run(int argc, char** argv)
{
    {
/*TAG
        {
    		char* attrListing = dteListQueryAttributes(argv[1]);
    		CATCH(
    			string err = "Failed to find available attributes.\n";
    			err += currExcept->toString();
    			ReturnVal(API_NAK, (char*) err.c_str());
    			return -1;
    		)
    		ReturnVal(API_ACK, attrListing);
    		delete [] attrListing;
    		return 1;
    	}
TAG*/
#if defined(DTE_WARN)
  fprintf(stderr, "Warning: calling DTE at %s: %d\n", __FILE__, __LINE__);
#endif
#if !defined(NO_DTE)
        return ParseAPIDTE(argc, argv, control);
#else
        return false;
#endif
    }
    return true;
}
int
DeviseCommand_dteListAttributes::Run(int argc, char** argv)
{
    {
/*TAG
        {
          char* attrListing = dteListAttributes(argv[1]);
          ReturnVal(API_ACK, attrListing);
          return 1;
        }
TAG*/
#if defined(DTE_WARN)
  fprintf(stderr, "Warning: calling DTE at %s: %d\n", __FILE__, __LINE__);
#endif
#if !defined(NO_DTE)
        return ParseAPIDTE(argc, argv, control);
#else
        return false;
#endif
    }
    return true;
}
int
DeviseCommand_dteDeleteIndex::Run(int argc, char** argv)
{
    {
/*TAG
        {
          dteDeleteIndex(argv[1], argv[2]);
          ReturnVal(API_ACK, "");
          return 1;
        }
TAG*/
#if defined(DTE_WARN)
  fprintf(stderr, "Warning: calling DTE at %s: %d\n", __FILE__, __LINE__);
#endif
#if !defined(NO_DTE)
        return ParseAPIDTE(argc, argv, control);
#else
        return false;
#endif
    }
    return true;
}
int
DeviseCommand_dteShowIndexDesc::Run(int argc, char** argv)
{
    {
/*TAG
        {
          char* indexDesc = dteShowIndexDesc(argv[1], argv[2]);
          ReturnVal(API_ACK, indexDesc);
          return 1;
        }
TAG*/
#if defined(DTE_WARN)
  fprintf(stderr, "Warning: calling DTE at %s: %d\n", __FILE__, __LINE__);
#endif
#if !defined(NO_DTE)
        return ParseAPIDTE(argc, argv, control);
#else
        return false;
#endif
    }
    return true;
}
int
DeviseCommand_dteShowAttrNames::Run(int argc, char** argv)
{
    {
/*TAG
        {
          char* attrListing = dteShowAttrNames(argv[1], argv[2]);
          ReturnVal(API_ACK, attrListing);
          return 1;
        }
TAG*/
#if defined(DTE_WARN)
  fprintf(stderr, "Warning: calling DTE at %s: %d\n", __FILE__, __LINE__);
#endif
#if !defined(NO_DTE)
        return ParseAPIDTE(argc, argv, control);
#else
        return false;
#endif
    }
    return true;
}
int
DeviseCommand_dteInsertCatalogEntry::Run(int argc, char** argv)
{
    {
/*TAG
        {
          dteInsertCatalogEntry(argv[1], argv[2]);
          ReturnVal(API_ACK, "");
    	 return 1;
        }
TAG*/
#if defined(DTE_WARN)
  fprintf(stderr, "Warning: calling DTE at %s: %d\n", __FILE__, __LINE__);
#endif
#if !defined(NO_DTE)
		//TEMP -- should check for existing entry with given name
        return ParseAPIDTE(argc, argv, control);
#else
		int result = DataCatalog::Instance()->AddEntry(argv[1], argv[2]);
		if (result == 0) {
          ReturnVal(API_ACK, "");
    	  return 1;
		} else {
          ReturnVal(API_NAK, (char *)DevError::GetLatestError());
    	  return -1;
		}
#endif
    }
    return true;
}
int
DeviseCommand_dteCheckSQLViewEntry::Run(int argc, char** argv)
{
    {
/*TAG
        {
    			string errmsg = dteCheckSQLViewEntry(argv[1], argv[2]);
    			if(errmsg.empty()){
    				ReturnVal(API_ACK, "");
    			}
    			else{
    				// this cast is OK
    				ReturnVal(API_ACK, (char*) errmsg.c_str());
    			}
    			return 1;
    		}
TAG*/
#if defined(DTE_WARN)
  fprintf(stderr, "Warning: calling DTE at %s: %d\n", __FILE__, __LINE__);
#endif
#if !defined(NO_DTE)
        return ParseAPIDTE(argc, argv, control);
#else
        return false;
#endif
    }
    return true;
}
int
DeviseCommand_dteCreateIndex::Run(int argc, char** argv)
{
    {
/*TAG
        {
              dteCreateIndex(argv[1], argv[2], argv[3], argv[4], argv[5]);
              ReturnVal(API_ACK, "");
              return 1;
         }
TAG*/
#if defined(DTE_WARN)
  fprintf(stderr, "Warning: calling DTE at %s: %d\n", __FILE__, __LINE__);
#endif
#if !defined(NO_DTE)
        return ParseAPIDTE(argc, argv, control);
#else
        return false;
#endif
    }
    return true;
}

//**********************************************************************
// User Command objects
//**********************************************************************
int
DeviseCommand_color::Run(int argc, char** argv)
{
	return ParseAPIColorCommands(argc, argv, control);
}
int
DeviseCommand_getAllViews::Run(int argc, char** argv)
{
    {
    	{
    		int		index;
    
    		for(index = View::InitViewIterator(); View::MoreView(index);)
    		{
    			View*	view = View::NextView(index);
    
    			strcat(result, "{");
    			strcat(result, view->GetName());
    			strcat(result, "} ");
    		}
    
    		View::DoneViewIterator(index);
    
    		ReturnVal(API_ACK, result);
    		return 1;
    	}
    }
    return true;
}
#if 0 // Why the hell do we have two *different* versions of the same
	  // command?!?  RKW Feb. 3, 1998.
int
DeviseCommand_getAllViews::Run(int argc, char** argv)
{
    {
    	{
    		int		iargc;
    		char**	iargv;
    
    		classDir->ClassNames("view", iargc, iargv);
    
    		for (int i=0; i<iargc; i++)
    		{
    			classDir->InstanceNames("view", iargv[i], numArgs, args);
    
    			for (int j=0; j<numArgs; j++)
    			{
    				View*	view = (View*)classDir->FindInstance(args[j]);
    
    				if (!view)
    				{
    					ReturnVal(API_NAK, "Cannot find view");
    					return -1;
    				}
    
    				strcat(result, "{");
    				strcat(result, view->GetName());
    				strcat(result, "} ");
    			}
    		}
    
    		ReturnVal(API_ACK, result);
    		return 1;
    	}
    }
    return true;
}
#endif

int
DeviseCommand_changeParam::Run(int argc, char** argv)
{
    {
        {
        classDir->ChangeParams(argv[1], argc - 2, &argv[2]);
        ReturnVal(API_ACK, "done");
        return 1;
      }
    }
    return true;
}
int
DeviseCommand_createInterp::Run(int argc, char** argv)
{
    {
        {
        /* This command is supported for backward compatibility only */
        MapInterpClassInfo *interp = control->GetInterpProto();
        ClassInfo *classInfo = interp->CreateWithParams(argc - 1, &argv[1]);
        if (!classInfo) {
          ReturnVal(API_NAK, "Cannot create mapping");
          return -1;
        }
        ControlPanel::RegisterClass(classInfo, true);
        ReturnVal(API_ACK, classInfo->ClassName());
        return 1;
      }
    }
    return true;
}
int
DeviseCommand_create::Run(int argc, char** argv)
{
    {
        {
        control->SetBusy();
        // HACK to provide backward compatibility
        if (!strcmp(argv[2], "WinVertical") ||
    	!strcmp(argv[2], "WinHorizontal")){
          argv[2] = "TileLayout";
    	}
    #if defined(DEBUG)
        for(int i=0; i<argc; i++) {
        	printf("ParseAPI: argv[%d]=%s \n", i, argv[i]);
        }
    #endif
        char *name = classDir->CreateWithParams(argv[1], argv[2],
    					    argc - 3, &argv[3]);
    #ifdef DEBUG
        printf("Create - return value = %s\n", name);
    #endif
        control->SetIdle();
        if (!name) {
          ReturnVal(API_NAK, "cannot create object");
	  return -1;
        } else {
          ReturnVal(API_ACK, name);
	}
        return 1;
      }
    }
    return true;
}
int
DeviseCommand_getTDataName::Run(int argc, char** argv)
{
    {
        {
        TData *tdata = (TData *)classDir->FindInstance(argv[1]);
        if (!tdata) {
            ReturnVal(API_NAK, "Cannot find TData");
            return -1;
        }
        char *tname = tdata->GetName();
        ReturnVal(API_ACK, tname); 
        return 1;
      }
    }
    return true;
}
int
DeviseCommand_showkgraph::Run(int argc, char** argv)
{
    {
        {
        if (atoi(argv[1]) == 1 || !vkg) {
          /* Create a new ViewKGraph object */
          /*
    	 Don't delete the previous vkg since we still want it to
    	 continuously display the previously defined KGraph
          */
          vkg = new ViewKGraph;
          DOASSERT(vkg, "Out of memory");
        }
        /* Number of views */
        int nview = argc - 4;
        ViewGraph **vlist = new (ViewGraph *) [nview];
        DOASSERT(vlist, "Out of memory");
        for(int i = 0; i < nview; i++) {
          vlist[i] = (ViewGraph *)classDir->FindInstance(argv[4 + i]);
          if (!vlist[i]) {
    	ReturnVal(API_NAK, "Cannot find view");
    	delete vlist;
    	return -1;
          }
        }
        vkg->Init();
        /* Add these to the ViewKGraph class and display */
        if (vkg->AddViews(vlist, nview, argv[3]) == false) {
          ReturnVal(API_NAK, "Could not add views to Kiviat graph");
          delete vlist;
          return -1;
        }
        delete vlist;
        if (!vkg->Display(atoi(argv[2]))) {
          ReturnVal(API_NAK, "Could not display Kiviat graph");
          return -1;
        }
        ReturnVal(API_ACK, "done");
        return 1;
      }
    }
    return true;
}
int
DeviseCommand_createMappingClass::Run(int argc, char** argv)
{
    {
        {
        MapInterpClassInfo *interp = control->GetInterpProto();
        ClassInfo *classInfo = interp->CreateWithParams(argc - 1, &argv[1]);
        if (!classInfo) {
          ReturnVal(API_NAK, "Cannot create mapping class");
          return -1;
        }
        ControlPanel::RegisterClass(classInfo, true);
        ReturnVal(API_ACK, classInfo->ClassName());
        return 1;
      }
    }
    return true;
}
int
DeviseCommand_setDefault::Run(int argc, char** argv)
{
    {
        {
        classDir->SetDefault(argv[1], argv[2], argc - 3, &argv[3]);
        ReturnVal(API_ACK, "done");
        return 1;
      }
    }
    return true;
}

int
DeviseCommand_setHistogram::Run(int argc, char** argv)
{
#if VIEW_MIN_STATS
		printf("Warning: histograms not implemented\n");
        ReturnVal(API_ACK, "done");
        return 1;
#endif
    {
        {
        ViewGraph *view = (ViewGraph *)classDir->FindInstance(argv[1]);
        if (!view) {
          ReturnVal(API_NAK, "Cannot find view");
          return -1;
        }
        double min = atof(argv[2]);
        double max = atof(argv[3]);
        int buckets = atoi(argv[4]);
        if( min >= max ) {
          ReturnVal(API_NAK, "min >= max");
          return -1;
        }
        if( buckets > MAX_HISTOGRAM_BUCKETS ) {
          sprintf(result, "buckets > %d", MAX_HISTOGRAM_BUCKETS);
          ReturnVal(API_NAK, result);
          return -1;
        }
        if( buckets < 1 ) {
          ReturnVal(API_NAK, "buckets < 1");
          return -1;
        }
        view->SetHistogram(atof(argv[2]), atof(argv[3]), atoi(argv[4]));
        ReturnVal(API_ACK, "done");
        return 1;
      }
    }
    return true;
}
int
DeviseCommand_getHistogram::Run(int argc, char** argv)
{
    {
        {
        ViewGraph *view = (ViewGraph *)classDir->FindInstance(argv[1]);
        if (!view) {
          ReturnVal(API_NAK, "Cannot find view");
          return -1;
        }
        sprintf(result, "%g %g %d", view->GetHistogramMin(),
                view->GetHistogramMax(), view->GetHistogramBuckets());
        ReturnVal(API_ACK, result);
        return 1;
      }
    }
    return true;
}

#if defined(KSB_MAYBE_DELETE_THIS_OLD_STATS_STUFF)
int
DeviseCommand_setBuckRefresh::Run(int argc, char** argv)
{
    {
        {
        ViewGraph *view = (ViewGraph *)classDir->FindInstance(argv[1]);
        if (!view) {
    	    ReturnVal(API_NAK, "Cannot find view");
    	    return -1;
        }
    #if defined(DEBUG) || 0
        printf("In setBuckRefresh, %s %s\n", argv[1], argv[2]);
    #endif
        view->AbortQuery();
        view->ResetGStatInMem();
        view->SetHistBucks(atoi(argv[2]));
        view->Refresh();
    //    view->PrepareStatsBuffer(view->GetFirstMap());
        
        ReturnVal(API_ACK, "done");
        return 1;
      }
    }
    return true;
}
#endif

int
DeviseCommand_setHistViewname::Run(int argc, char** argv)
{
    {
        {
        ViewGraph *view = (ViewGraph *)classDir->FindInstance(argv[1]);
        if (!view) {
                ReturnVal(API_NAK, "Cannot find view");
                return -1;
        }
        char *viewName = new char[strlen(argv[2]) + 1];
        strcpy(viewName, argv[2]);
        view->setHistViewname(viewName);
    
        ReturnVal(API_ACK, "done");
        return 1;
      }
    }
    return true;
}
int
DeviseCommand_getHistViewname::Run(int argc, char** argv)
{
    {
        {
        ViewGraph *view = (ViewGraph *)classDir->FindInstance(argv[1]);
        if (!view) {
        	ReturnVal(API_NAK, "Cannot find view");
    	return -1;
        }
        char *name = NULL;
        name = view->getHistViewname();
    
        ReturnVal(API_ACK, name);
        return 1;
      }
    }
    return true;
}
int
DeviseCommand_checkGstat::Run(int argc, char** argv)
{
    {
        {
        ViewGraph *view = (ViewGraph *)classDir->FindInstance(argv[1]);
        if (!view) {
    	ReturnVal(API_NAK, "Cannot find view");
    	return -1;
        }
        if(view->IsGStatInMem()) {
    	printf("GDataStat is in memory\n");
    	strcpy(result, "1");
        } else {
    	printf("GDataStat is NOT in memory\n");
            strcpy(result, "0");
        }
        ReturnVal(API_ACK, result);
    
        return 1;
      }
    }
    return true;
}
int
DeviseCommand_getSourceName::Run(int argc, char** argv)
{
    {
        {
       ViewGraph *view = (ViewGraph *)classDir->FindInstance(argv[1]);
        if (!view) {
            ReturnVal(API_NAK, "Cannot find view");
            return -1;
        }
        TDataMap *map = view->GetFirstMap();
        TData *tdata = map->GetLogTData();
        char *sourceName = tdata->GetTableName();
        ReturnVal(API_ACK, sourceName); 
    
        return 1;
      }
    }
    return true;
}
int
DeviseCommand_isXDateType::Run(int argc, char** argv)
{
    {
        {
        ViewGraph *view = (ViewGraph *)classDir->FindInstance(argv[1]);
        if (!view) {
            ReturnVal(API_NAK, "Cannot find view");
            return -1;
        }
        if(view->IsXDateType()) {
    	printf("X type is date\n");
            strcpy(result, "1");
        } else {
    	printf("X type is NOT date\n");
            strcpy(result, "0");
        }
        ReturnVal(API_ACK, result);
    
        return 1;
      }
    }
    return true;
}
int
DeviseCommand_isYDateType::Run(int argc, char** argv)
{
    {
        {
        ViewGraph *view = (ViewGraph *)classDir->FindInstance(argv[1]);
        if (!view) {
            ReturnVal(API_NAK, "Cannot find view");
            return -1;
        }
        if(view->IsYDateType()) {
    //	printf("Y type is date\n");
            strcpy(result, "1");
        } else {
    //	printf("Y type is NOT date\n");
            strcpy(result, "0");
        }
        ReturnVal(API_ACK, result);
    
        return 1;
      }
    }
    return true;
}
int
DeviseCommand_mapG2TAttr::Run(int argc, char** argv)
{
    {
        {
        ViewGraph *view = (ViewGraph *)classDir->FindInstance(argv[1]);
        if (!view) {
            ReturnVal(API_NAK, "Cannot find view");
    	return -1;
        }
        TDataMap *map = view->GetFirstMap();
        if (!map) {
    	ReturnVal(API_NAK, "Cannot find Mapping");
            return -1;
        }
        AttrInfo *attr = NULL;
        if (!strcasecmp(argv[2], "X")) {
          	attr = map->MapGAttr2TAttr(MappingCmd_X);
        } else if (!strcasecmp(argv[2], "Y")) {
          	attr = map->MapGAttr2TAttr(MappingCmd_Y);
        } else if (!strcasecmp(argv[2], "Z")) {
          	attr = map->MapGAttr2TAttr(MappingCmd_Z);
        } else if (!strcasecmp(argv[2], "Color")) {
            attr = map->MapGAttr2TAttr(MappingCmd_Color);
        } else {
          	fprintf(stderr, "Not implemented yet\n");
          	ReturnVal(API_NAK, "GAttr type not implemented");
    	return -1;
        }
        if (attr) {
          strcpy(result, attr->name);
        } else {
          strcpy(result, "0");
        }
        ReturnVal(API_ACK, result);
        return 1;
      }
    }
    return true;
}
int
DeviseCommand_mapT2GAttr::Run(int argc, char** argv)
{
    {
        {
        char *gname = NULL;
        ViewGraph *view = (ViewGraph *)classDir->FindInstance(argv[1]);
        if (!view) {
            ReturnVal(API_NAK, "Cannot find view");
            return -1;
        }
        TDataMap *map = view->GetFirstMap();
        if (!map) {
            ReturnVal(API_NAK, "Cannot find Mapping");
            return -1;
        }
        printf("argv[2] is %s\n", argv[2]);
        gname = map->MapTAttr2GAttr(argv[2]);
        if (gname == NULL) {
    	strcpy(result, "0");
        } else {
    	strcpy(result, gname);
        }
        ReturnVal(API_ACK, result);
        delete gname; 
        return 1;
      }
    }
    return true;
}

int
DeviseCommand_startLayoutManager::Run(int argc, char** argv)
{
#if 0
    {
        {
          printf("starting Layout Manager\n");
          int childpid = fork();
          if (childpid == 0) {
    	char *args[] = {"/u/s/s/ssl/Work/LM/C++/LM", 0};
    	execvp(args[0], argv);
          } else {
    	return 1;
          }
          LMControl::GetLMControl()->Go();
          return 1;
        }
    }
#endif
    return false;
}
int
DeviseCommand_date::Run(int argc, char** argv)
{
    {
        {
          time_t tm = time((time_t *)0);
          ReturnVal(API_ACK, DateString(tm));
          return 1;
        }
    }
    return true;
}
int
DeviseCommand_printDispatcher::Run(int argc, char** argv)
{
    {
        {
          ReturnVal(API_ACK, "done");
          return 1;
        }
    }
    return true;
}
int
DeviseCommand_catFiles::Run(int argc, char** argv)
{
    {
        {
          CatFiles(numArgs, args);
          ReturnVal(numArgs, args);
          return 1;
        }
    }
    return true;
}
int
DeviseCommand_exit::Run(int argc, char** argv)
{
    {
        {
          ReturnVal(API_ACK, "done");
          control->RestartSession();
          return 1;
        }
    }
    return true;
}
int
DeviseCommand_clearAll::Run(int argc, char** argv)
{
    {
        {
	  Session::Close();
          ReturnVal(API_ACK, "done");
          return 1;
        }
    }
    return true;
}
int
DeviseCommand_sync::Run(int argc, char** argv)
{
    {
        {
          control->SetSyncNotify();
          ReturnVal(API_ACK, "done");
          return 1;
        }
    }
    return true;
}
int
DeviseCommand_version::Run(int argc, char** argv)
{
    {
        {
          ReturnVal(API_ACK, (char *) Version::Get());
          return 1;
        }
    }
    return true;
}
int
DeviseCommand_copyright::Run(int argc, char** argv)
{
    {
        {
          ReturnVal(API_ACK, (char *) Version::GetCopyright());
          return 1;
        }
    }
    return true;
}
int
DeviseCommand_compDate::Run(int argc, char** argv)
{
    {
        {
          ReturnVal(API_ACK, (char *) CompDate::Get());
          return 1;
        }
    }
    return true;
}
int
DeviseCommand_new_leaks::Run(int argc, char** argv)
{
#if PURIFY
    {
        {
          if (purify_is_running()) {
    	purify_new_leaks();
          }
          ReturnVal(API_ACK, "done");
          return 1;
        }
    }
#endif
    return false;
}
int
DeviseCommand_new_inuse::Run(int argc, char** argv)
{
#if PURIFY
    {
        {
          if (purify_is_running()) {
    	purify_new_inuse();
          }
          ReturnVal(API_ACK, "done");
          return 1;
        }
    }
#endif
    return false;
}
int
DeviseCommand_getWinCount::Run(int argc, char** argv)
{
    {
        {
          char buf[100];
          sprintf(buf, "%d", DevWindow::GetCount());
          ReturnVal(API_ACK, buf);
          return 1;
        }
    }
    return true;
}
int
DeviseCommand_getStringCount::Run(int argc, char** argv)
{
    {
        {
          // Returns: <stringCount>
          char buf[100];
          sprintf(buf, "%d", StringStorage::GetTotalCount());
          ReturnVal(API_ACK, buf);
          return 1;
        }
    }
    return true;
}
int
DeviseCommand_waitForQueries::Run(int argc, char** argv)
{
    // Arguments: none
    // Returns: "done"
    Dispatcher::Current()->WaitForQueries();
    ReturnVal(API_ACK, "done");
    return true;
}
int
DeviseCommand_serverExit::Run(int argc, char** argv)
{
    {
        {
          // Arguments: none
          // Returns: "done"
    #if defined(DEBUG)
          printf("serverExit\n");
    #endif
    	  // Only allow this if the client that requested the server to exit
    	  // is the only client.
    	  if (control->NumClients() == 1) {
            ReturnVal(API_ACK, "done");
            printf("Server exiting on command from client\n");
            ControlPanel::Instance()->DoQuit();
    		return 1; // We never get to here.
          } else {
    		char *result = "Server won't exit with more than one client connected.";
    		printf("%s\n", result);
            ReturnVal(API_NAK, result);
    		return -1;
    	  }
        }
    }
    return true;
}
int
DeviseCommand_abortQuery::Run(int argc, char** argv)
{
    {
        {
          View *view = (View *)classDir->FindInstance(argv[1]);
          if (!view) {
    	ReturnVal(API_NAK, "Cannot find view");
    	return -1;
          }
          view->AbortQuery();
          ReturnVal(API_ACK, "done");
          return 1;
        }
    }
    return true;
}
int
DeviseCommand_importFileType::Run(int argc, char** argv)
{
	if (argc ==2)
		return Run_2(argc, argv);
	else if (argc ==4)
		return Run_4(argc, argv);
	else
		fprintf(stderr, "Unknown command:%s\n", argv[0]);
	return 0;
}
int
DeviseCommand_importFileType::Run_2(int argc, char** argv)
{
    {
        {
          char *name = ParseCat(argv[1]);
          if (!name) {
    	ReturnVal(API_NAK, "error parsing UNIXFILE schema");
    	return -1;
          }
          ReturnVal(API_ACK, name);
          return 1;
        }
    }
    return true;
}
int
DeviseCommand_importFileType::Run_4(int argc, char** argv)
{
    {
        {
          char *name = ParseCat(argv[1],argv[2],argv[3]);
          if (!name) {
    	strcpy(result , "");
    	ReturnVal(API_NAK, result);
    	return -1;
          }
          ReturnVal(API_ACK, name);
          return 1;
        }
    }
    return true;
}
int
DeviseCommand_resetLinkMaster::Run(int argc, char** argv)
{
    {
        {
          DeviseLink *link = (DeviseLink *)classDir->FindInstance(argv[1]);
          if (!link) {
    	ReturnVal(API_NAK, "Cannot find link");
    	return -1;
          }
          link->SetMasterView(0);
          ReturnVal(API_ACK, "done");
          return 1;
        }
    }
    return true;
}
int
DeviseCommand_get3DLocation::Run(int argc, char** argv)
{
    {
        {
          ViewGraph *view = (ViewGraph *)classDir->FindInstance(argv[1]);
          if (!view) {
    	ReturnVal(API_NAK, "Cannot find view");
    	return -1;
          }
          Camera c = view->GetCamera();
          sprintf(result, "%s %g %g %g %g %g %g %g %g",
            ViewDir2Char(c.view_dir),
            c.min_x, c.max_x, c.min_y, c.max_y, c.near, c.far,
            c.pan_right, c.pan_up);

          ReturnVal(API_ACK, result);
          return 1;
        }
    }
    return true;
}
int
DeviseCommand_getLinkMaster::Run(int argc, char** argv)
{
    {
        {
          DeviseLink *link = (DeviseLink *)classDir->FindInstance(argv[1]);
          if (!link) {
    	ReturnVal(API_NAK, "Cannot find link");
    	return -1;
          }
          ViewGraph *view = link->GetMasterView();
          ReturnVal(API_ACK, (view ? view->GetName() : ""));
          return 1;
        }
    }
    return true;
}
int
DeviseCommand_getLinkType::Run(int argc, char** argv)
{
    {
        {
          RecordLink *link = (RecordLink *)classDir->FindInstance(argv[1]);
          if (!link) {
    	ReturnVal(API_NAK, "Cannot find link");
    	return -1;
          }
          ReturnVal(API_ACK, (link->GetLinkType() == Positive)? "1" : "0");
          return 1;
        }
    }
    return true;
}
int
DeviseCommand_setBatchMode::Run(int argc, char** argv)
{
    {
        {
          Boolean batch = (atoi(argv[1]) ? true : false);
          control->SetBatchMode(batch);
          ReturnVal(API_ACK, "done");
          return 1;
        }
    }
    return true;
}
int
DeviseCommand_invalidateTData::Run(int argc, char** argv)
{
    {
        {
          TData *tdata = (TData *)classDir->FindInstance(argv[1]);
          if (!tdata) {
    	ReturnVal(API_NAK, "Cannot find TData");
    	return -1;
          }
          tdata->InvalidateTData();
          ReturnVal(API_ACK, "Done");
          return 1;
        }
    }
    return true;
}
int
DeviseCommand_invalidatePixmap::Run(int argc, char** argv)
{
    {
        {
          View *vg = (View *)classDir->FindInstance(argv[1]);
          if (!vg) {
    	ReturnVal(API_NAK, "Cannot find view");
    	return -1;
          }
          vg->InvalidatePixmaps();
          ReturnVal(API_ACK, "done");
          return 1;
        }
    }
    return true;
}
int
DeviseCommand_readLine::Run(int argc, char** argv)
{
    {
        {
          FILE *file = (FILE *)atol(argv[1]);
          (void)fgets(result, 256, file);
          int len = strlen(result);
          if (len > 0 && result[len - 1] == '\n')
    	result[len - 1] = '\0';
          ReturnVal(API_ACK, result);
          return 1;
        }
    }
    return true;
}
int
DeviseCommand_close::Run(int argc, char** argv)
{
    {
        {
          FILE *file = (FILE *)atol(argv[1]);
          fclose(file);
          ReturnVal(API_ACK, "done");
          return 1;
        }
    }
    return true;
}
int
DeviseCommand_isMapped::Run(int argc, char** argv)
{
    {
        {
          View *vg = (View *)classDir->FindInstance(argv[1]);
          if (!vg) {
    	ReturnVal(API_NAK, "Cannot find view");
    	return -1;
          }
          sprintf(result, "%d", (vg->Mapped() ? 1 : 0 ));
          ReturnVal(API_ACK, result);
          return 1;
        }
    }
    return true;
}
int
DeviseCommand_getLabel::Run(int argc, char** argv)
{
    {
        {
          View *vg = (View *)classDir->FindInstance(argv[1]);
          if (!vg) {
    	ReturnVal(API_NAK, "Cannot find view");
    	return -1;
          }
          Boolean occupyTop;
          int extent;
          char *name;
          vg->GetLabelParam(occupyTop, extent, name);
          sprintf(result, "%d %d {%s}", (occupyTop ? 1 : 0), extent,
    	      (name ? name : ""));
          ReturnVal(API_ACK, result);
          return 1;
        }
    }
    return true;
}
int
DeviseCommand_tdataFileName::Run(int argc, char** argv)
{
    {
        {
          TData *tdata = (TData *)classDir->FindInstance(argv[1]);
          if (!tdata) {
    	ReturnVal(API_NAK, "Cannot find tdata");
    	return -1;
          }
          ReturnVal(API_ACK, tdata->GetName());
          return 1;
        }
    }
    return true;
}
int
DeviseCommand_getViewWin::Run(int argc, char** argv)
{
    {
        {
          View *view = (View *)classDir->FindInstance(argv[1]);
          if (!view) {
    	ReturnVal(API_NAK, "Cannot find view");
    	return -1;
          }
          ViewWin *viewWin = view->GetParent();
          if (!viewWin)
    	ReturnVal(API_ACK, "");
          else
    	ReturnVal(API_ACK, viewWin->GetName());
          return 1;
        }
    }
    return true;
}
int
DeviseCommand_clearViewHistory::Run(int argc, char** argv)
{
    {
        {
          View *view = (View *)classDir->FindInstance(argv[1]);
          if (!view) {
    	ReturnVal(API_NAK, "Cannot find view");
    	return -1;
          }
          FilterQueue *queue = view->GetHistory();
          queue->Clear();
          ReturnVal(API_ACK, "done");
          return 1;
        }
    }
    return true;
}
int
DeviseCommand_getCursorViews::Run(int argc, char** argv)
{
    {
        {
          DeviseCursor *cursor = (DeviseCursor *)classDir->FindInstance(argv[1]);
          if (!cursor) {
    	ReturnVal(API_NAK, "Cannot find cursor");
    	return -1;
          }
          View *src = cursor->GetSource();
          View *dst = cursor->GetDst();
          char *name[2];
          if (src)
    	name[0] = src->GetName();
          else
    	name[0] = "";
          if (dst)
    	name[1] = dst->GetName();
          else
    	name[1] = "";
          ReturnVal(2, name);
          return 1;
        } 
    }
    return true;
}
int
DeviseCommand_getMappingTData::Run(int argc, char** argv)
{
    {
        {
          TDataMap *map= (TDataMap *)classDir->FindInstance(argv[1]);
          if (!map) {
    	ReturnVal(API_NAK, "Cannot find mapping");
    	return -1;
          }
          TData *tdata = map->GetLogTData();
          ReturnVal(API_ACK, classDir->FindInstanceName(tdata));
          return 1;
        }
    }
    return true;
}
int
DeviseCommand_destroy::Run(int argc, char** argv)
{
    {
        {
          classDir->DestroyInstance(argv[1]);
          ReturnVal(API_ACK, "done");
          return 1;
        }
    }
    return true;
}
int
DeviseCommand_parseDateFloat::Run(int argc, char** argv)
{
    {
        {
          double val;
          (void)ParseFloatDate(argv[1], val);
          sprintf(result, "%f", val);
          ReturnVal(API_ACK, result);
          return 1;
        }
    }
    return true;
}
int
DeviseCommand_isInterpretedGData::Run(int argc, char** argv)
{
    {
        {
          TDataMap *map= (TDataMap *)classDir->FindInstance(argv[1]);
          if (!map) {
    	ReturnVal(API_NAK, "Cannot find mapping");
    	return -1;
          }
          if (map->IsInterpreted())
    	strcpy(result, "1");
          else
    	strcpy(result, "0");
          ReturnVal(API_ACK, result);
          return 1;
        }
    }
    return true;
}
int
DeviseCommand_isInterpreted::Run(int argc, char** argv)
{
    {
        {
          int *isInterp = (int *)classDir->UserInfo("mapping", argv[1]);
          strcpy(result, "0");
          if (isInterp && *isInterp)
    	strcpy(result, "1");
          ReturnVal(API_ACK, result);
          return 1;
        }
    }
    return true;
}
int
DeviseCommand_getPixelWidth::Run(int argc, char** argv)
{
    {
        {
          TDataMap *map= (TDataMap *)classDir->FindInstance(argv[1]);
          if (!map) {
    	ReturnVal(API_NAK, "Cannot find mapping");
    	return -1;
          }
          sprintf(result, "%d", map->GetPixelWidth());
          ReturnVal(API_ACK, result);
          return 1;
        }
    }
    return true;
}
int
DeviseCommand_getTopGroups::Run(int argc, char** argv)
{
    {
        {
          control->GetGroupDir()->top_level_groups(result, argv[1]);
          ReturnVal(API_ACK, result);
          return 1;
        }
    }
    return true;
}
int
DeviseCommand_getWindowLayout::Run(int argc, char** argv)
{
    {
        {
          ViewLayout *layout = (ViewLayout *)classDir->FindInstance(argv[1]);
          if (!layout) {
    	ReturnVal(API_NAK, "Cannot find window");
    	return -1;
          }
          int v, h;
          Boolean stacked;
          layout->GetPreferredLayout(v, h, stacked);
		  // Set stacked to false here because we no longer stack windows
		  // this way (it's now done via the PileStack class).  RKW 1999-02-11.
		  stacked = false;
          sprintf(result, "%d %d %d", v, h, (stacked ? 1 : 0));
          ReturnVal(API_ACK, result);
          return 1;
        }
    }
    return true;
}
int
DeviseCommand_getSchema::Run(int argc, char** argv)
{
    {
        {
        	
          TData *tdata = (TData *)classDir->FindInstance(argv[1]);
          if (!tdata) {
    	ReturnVal(API_NAK, "Cannot find Tdata");
    	return -1;
          }
          AttrList *attrList = tdata->GetAttrList();
          if (!attrList) {
    	ReturnVal(API_NAK, "Null attribute list");
    	return -1;
          }
          
    #ifdef DEBUG
          printf("getSchema: \n");
          attrList->Print();
    #endif
          int numAttrs = attrList->NumAttrs();
    //    args = new (char *) [numAttrs + 1];
          args = new (char *) [numAttrs];
    	 /*
          args[0] = new char[25];
          assert(args && args[0]);
          strcpy(args[0], "recId int 1 0 0 0 0");
         */ 
          int i;
          for(i = 0; i < numAttrs; i++) {
    	char attrBuf[160];
    	AttrInfo *info = attrList->Get(i);
    #ifdef DEBUG
    	printf("inserting %s\n", info->name);
    #endif
    	switch(info->type) {
    	case FloatAttr:
    	  sprintf(attrBuf, "%s float %d %d %g %d %g",
    		  info->name, info->isSorted,
    		  info->hasHiVal,
    		  (info->hasHiVal ? info->hiVal.floatVal : 100),
    		  info->hasLoVal,
    		  (info->hasLoVal ? info->loVal.floatVal : 0));
    	  break;
    	case DoubleAttr:
    	  sprintf(attrBuf, "%s double %d %d %g %d %g",
    		  info->name, info->isSorted,
    		  info->hasHiVal,
    		  (info->hasHiVal ? info->hiVal.doubleVal : 100),
    		  info->hasLoVal,
    		  (info->hasLoVal ? info->loVal.doubleVal : 0));
    	  break;
    	case StringAttr:
    	  sprintf(attrBuf, "%s string %d 0 0 0 0", info->name,
    		  info->isSorted);
    	  break;
    	case IntAttr:
    	  sprintf(attrBuf, "%s int %d %d %ld %d %ld",
    		  info->name, info->isSorted,
    		  info->hasHiVal,
    		  (long)(info->hasHiVal ? info->hiVal.intVal : 100),
    		  info->hasLoVal,
    		  (long)(info->hasLoVal ? info->loVal.intVal : 0));
    	  break;
    	case DateAttr:
    	  sprintf(attrBuf, "%s date %d %d %ld %d %ld",
    		  info->name, info->isSorted,
    		  info->hasHiVal,
    		  (long)(info->hasHiVal ? info->hiVal.dateVal : 100),
    		  info->hasLoVal,
    		  (long)(info->hasLoVal ? info->loVal.dateVal : 0));
    	  break;
    	default:
    	  printf("Unknown attribute type\n");
    	  Exit::DoExit(1);
    	}
    //	args[i + 1] = new char [strlen(attrBuf) + 1];
      	args[i] = new char [strlen(attrBuf) + 1];
    	assert(args[i]);
    //	strcpy(args[i + 1], attrBuf);
    	strcpy(args[i], attrBuf);
          }
    //    ReturnVal(numAttrs + 1, args) ;
          ReturnVal(numAttrs, args) ;
    	 delete [] args;
          return 1;
        }
    }
    return true;
}
int
DeviseCommand_getAction::Run(int argc, char** argv)
{
    {
        {
          ViewGraph *view = (ViewGraph *)classDir->FindInstance(argv[1]);
          if (!view) {
    	ReturnVal(API_NAK, "Cannot find view");
    	return -1;
          }
          Action *action = view->GetAction();
          if (!action || !strcmp(action->GetName(), "") ||
    	  !strcmp(action->GetName(), "default"))
    	strcpy(result , "");
          else
    	strcpy(result,action->GetName());
          ReturnVal(API_ACK, result);
          return 1;
        }
    }
    return true;
}
int
DeviseCommand_getLinkFlag::Run(int argc, char** argv)
{
    {
        {
          DeviseLink *link = (DeviseLink *)classDir->FindInstance(argv[1]);
          if (!link) {
    	ReturnVal(API_NAK, "Cannot find link");
    	return -1;
          }
          sprintf(result, "%d", link->GetFlag());
          ReturnVal(API_ACK, result);
          return 1;
        }
    }
    return true;
}
int
DeviseCommand_changeableParam::Run(int argc, char** argv)
{
    {
        {
          Boolean changeable = classDir->Changeable(argv[1]);
          if (changeable)
    	strcpy(result, "1");
          else
    	strcpy(result, "0");
          ReturnVal(API_ACK, result);
          return 1;
        }
    }
    return true;
}
int
DeviseCommand_getInstParam::Run(int argc, char** argv)
{
    {
        {
          classDir->GetParams(argv[1], numArgs, args);
          ReturnVal(numArgs, args);
          return 1;
        }
    }
    return true;
}
int
DeviseCommand_tcheckpoint::Run(int argc, char** argv)
{
    {
        {
          TData *tdata;
          control->SetBusy();
          tdata = (TData *)classDir->FindInstance(argv[1]);
          if (tdata)
    	tdata->Checkpoint();
          control->SetIdle();
          ReturnVal(API_ACK, "done");
          return 1;
        }
    }
    return true;
}

int
DeviseCommand_get::Run(int argc, char** argv)
{
	if (argc ==2)
		return Run_2(argc, argv);
	else
	if (argc ==3)
		return Run_3(argc, argv);
	else
		fprintf(stderr, "Unknown command:%s\n", argv[0]);
	return 0;
}
int
DeviseCommand_get::Run_2(int argc, char** argv)
{
    {
        {
          classDir->ClassNames(argv[1], numArgs, args);
          ReturnVal(numArgs, args);
          return 1;
        }
    }
    return true;
}
int
DeviseCommand_get::Run_3(int argc, char** argv)
{
    {
        {
          classDir->InstanceNames(argv[1], argv[2], numArgs, args);
          ReturnVal(numArgs, args);
          return 1;
        }
    }
    return true;
}

int
DeviseCommand_changeMode::Run(int argc, char** argv)
{
    {
	  if (!strcmp(argv[1], "0")) {
		  if(control->GetMode() != ControlPanel::DisplayMode) {
			/* Set display mode  and make all views refresh*/
		 	DepMgr::Current()->RegisterEvent(NULL, DepMgr::EventControlModeCh);
			control->SetMode(ControlPanel::DisplayMode);
			ControlPanel::Instance()->ReportModeChange(control->GetMode());
		  }
	  } else if (control->GetMode() != ControlPanel::LayoutMode) {
			/* set layout mode */
			DepMgr::Current()->RegisterEvent(NULL, DepMgr::EventControlModeCh);
			control->SetMode(ControlPanel::LayoutMode);
			ControlPanel::Instance()->ReportModeChange(control->GetMode());
	  }
	  ReturnVal(API_ACK, "done");
  	  return 1;
    }
    return true;
}

int
DeviseCommand_exists::Run(int argc, char** argv)
{
    {
        {
          if (!classDir->FindInstance(argv[1]))
    	strcpy(result, "0");
          else
    	strcpy(result, "1");
          ReturnVal(API_ACK, result);
          return 1;
        }
    }
    return true;
}
int
DeviseCommand_removeView::Run(int argc, char** argv)
{
    {
        {
          ViewGraph *view = (ViewGraph *)classDir->FindInstance(argv[1]);
          if (!view) {
    	ReturnVal(API_NAK, "Cannot find view");
    	return -1;
          }
          if (!view->Mapped()) {
    	ReturnVal(API_NAK, "View not in any window");
    	return -1;
          }
          view->DeleteFromParent();
          ReturnVal(API_ACK, "done");
          return 1;
        }
    }
    return true;
}
int
DeviseCommand_getViewMappings::Run(int argc, char** argv)
{
    {
        {
          ViewGraph *view = (ViewGraph *)classDir->FindInstance(argv[1]);
          if (!view) {
    	ReturnVal(API_NAK, "Cannot find view");
    	return -1;
          }
          int index = view->InitMappingIterator();
          while(view->MoreMapping(index)) {
    	TDataMap *map = view->NextMapping(index)->map;
    	strcat(result, "{");
    	strcat(result, map->GetGDataName());
    	strcat(result, "} ");
          }
          view->DoneMappingIterator(index);
          ReturnVal(API_ACK, result);
          return 1;
        }
    }
    return true;
}
int
DeviseCommand_refreshView::Run(int argc, char** argv)
{
    {
        {
          View *view = (View *)classDir->FindInstance(argv[1]);
          if (!view) {
    	ReturnVal(API_NAK, "Cannot find view");
    	return -1;
          }
          view->Refresh();
          ReturnVal(API_ACK, "done");
          return 1;
        }
    }
    return true;
}
int
DeviseCommand_getWinGeometry::Run(int argc, char** argv)
{
    {
        {
          ViewWin *win = (ViewWin*)classDir->FindInstance(argv[1]);
          if (!win) {
    	ReturnVal(API_NAK, "Cannot find window");
    	return -1;
          }
          int x, y;
          unsigned h, w;
          int x0, y0;
          win->Geometry(x, y, w, h);
          win->AbsoluteOrigin(x0,y0);
          sprintf (result, "{ %d %d %d %d %u %u }" , x0, y0, x, y, w, h);
          ReturnVal(API_ACK, result);
          return 1;	      
        }
    }
    return true;
}
int
DeviseCommand_getWinViews::Run(int argc, char** argv)
{
    {
        {
          ViewWin *win = (ViewWin*)classDir->FindInstance(argv[1]);
          if (!win) {
    	ReturnVal(API_NAK, "Cannot find window");
    	return -1;
          }
          int index;
          for(index = win->InitIterator(); win->More(index); ) {
    	ViewWin *view = (ViewWin *)win->Next(index);
    	strcat(result, "{");
    	strcat(result, view->GetName());
    	strcat(result, "} ");
          }
          win->DoneIterator(index);
          ReturnVal(API_ACK, result);
    
          return 1;
        }
    }
    return true;
}
int
DeviseCommand_getLinkViews::Run(int argc, char** argv)
{
    {
        {
          DeviseLink *link = (DeviseLink *)classDir->FindInstance(argv[1]);
          if (!link) {
    	ReturnVal(API_NAK, "Cannot find link");
    	return -1;
          }
          int index;
          for(index = link->InitIterator(); link->More(index); ) {
    	ViewGraph *view = (ViewGraph *)link->Next(index);
    	strcat(result, "{");
    	strcat(result, view->GetName());
    	strcat(result, "} ");
          }
          link->DoneIterator(index);
          ReturnVal(API_ACK, result);
          return 1;
        }
    }
    return true;
}
int
DeviseCommand_getCurVisualFilter::Run(int argc, char** argv)
{
    {
        {
          View *view = (View *)classDir->FindInstance(argv[1]);
          if (!view) {
    	ReturnVal(API_NAK, "Cannot find view");
    	return -1;
          }
          VisualFilter *filter = view->GetVisualFilter();
          sprintf(result, "%.2f %.2f %.2f %.2f", filter->xLow, filter->yLow,
    	      filter->xHigh, filter->yHigh);
          ReturnVal(API_ACK, result);
          return 1;
        }
    }
    return true;
}
int
DeviseCommand_getVisualFilters::Run(int argc, char** argv)
{
    {
        {
          View *view = (View *)classDir->FindInstance(argv[1]);
          if (!view) {
    	ReturnVal(API_NAK, "Cannot find view");
    	return -1;
          }
          FilterQueue *queue ;
          queue = view->GetHistory();
          int i;
          for(i = 0; i < queue->Size(); i++) {
    	VisualFilter filter;
    	queue->Get(i,filter);
    	char buf[256];
    	char xLowBuf[40], xHighBuf[40], yLowBuf[40], yHighBuf[40];
    	if (view->GetXAxisAttrType() == DateAttr) {
    	  sprintf(xLowBuf, "%s",DateString(filter.xLow));
    	  sprintf(xHighBuf, "%s",DateString(filter.xHigh));
    	} else {
    	  sprintf(xLowBuf, "%.2f",filter.xLow);
    	  sprintf(xHighBuf, "%.2f",filter.xHigh);
    	}
    	
    	if (view->GetYAxisAttrType() == DateAttr) {
    	  sprintf(yLowBuf, "%s",DateString(filter.yLow));
    	  sprintf(yHighBuf, "%s",DateString(filter.yHigh));
    	} else {
    	  sprintf(yLowBuf, "%.2f",filter.yLow);
    	  sprintf(yHighBuf, "%.2f",filter.yHigh);
    	}
    	sprintf(buf, "{{%s} {%s} {%s} {%s} %d} ",xLowBuf, yLowBuf,
    		xHighBuf, yHighBuf, filter.marked);
    	strcat(result, buf);
          }
          ReturnVal(API_ACK, result);
          return 1;
        }
    }
    return true;
}
int
DeviseCommand_getViewStatistics::Run(int argc, char** argv)
{
    {
        {
          ViewGraph *vg = (ViewGraph *)classDir->FindInstance(argv[1]);
          if (!vg) {
    	ReturnVal(API_NAK, "Cannot find view");
    	return -1;
          }
          /* Return status of statistics display */
          ReturnVal(API_ACK, vg->GetDisplayStats());
          return 1;
        }
    }
    return true;
}
int
DeviseCommand_getAllStats::Run(int argc, char** argv)
{
    {
        {
          ViewGraph *vg = (ViewGraph *)classDir->FindInstance(argv[1]);
          if (!vg) {
            ReturnVal(API_NAK, "Cannot find view");
            return -1;
          }
          /* Return allStats */
          char *buff[4];
          int i;
          for(i = 0; i < 4; i++) {
    	  buff[i] = '\0';
          }
          for (i = 0; i < 4; i++) {
              buff[i] = new char[64];  /*64 bytes is enough for double type */
          }
          BasicStats *allStats = vg->GetStatObj();
          sprintf(buff[0], "%g", allStats->GetStatVal(STAT_MAX));
          sprintf(buff[1], "%g", allStats->GetStatVal(STAT_MEAN));
          sprintf(buff[2], "%g", allStats->GetStatVal(STAT_MIN));
          sprintf(buff[3], "%d", (int)allStats->GetStatVal(STAT_COUNT));
    #if defined(DEBUG) || 0
          printf("buff=%s %s %s %s\n", buff[0],buff[1],buff[2],buff[3]);
    #endif
          ReturnVal(4, buff);
          for (i = 0; i < 4; i++) {
    	  delete buff[i];
          }
          return 1;
        }    	
    }
    return true;
}
int
DeviseCommand_getStatBuffer::Run(int argc, char** argv)
{
    {
        {
           enum {
               STAT_TYPE,
               HIST_TYPE,
               GSTATX_TYPE, 
               GSTATY_TYPE 
    	} type;
            char* viewName = NULL;
            if(!strncmp(argv[1], "Stat:", 5)) {
    	  type = STAT_TYPE;
    	  viewName = &argv[1][5];
            } else if(!strncmp(argv[1], "Hist:", 5)) {
    	  type = HIST_TYPE;
    	  viewName = &argv[1][5];
            } else if(!strncmp(argv[1], "GstatX: ", 8)) {
    	  //e.g. argv[1] = GstatX: View 1
    	  type = GSTATX_TYPE;
    	  viewName = &argv[1][8];
            } else if(!strncmp(argv[1], "GstatY: ", 8)) {
    	  type = GSTATY_TYPE;
    	  viewName = &argv[1][8];
            } else {
    	  ReturnVal(API_NAK, "Invalid stat type");
    	  return -1;
            }
            while( *viewName == ' ' ) {
    	   viewName++;
    	}
            ViewGraph *vg = (ViewGraph *)classDir->FindInstance(viewName);
            if(!vg) {
    	  ReturnVal(API_NAK, "Cannot find view");
    	  return -1;
            }
    
            /* Return statistics buffer */
            DataSourceBuf *buffObj = NULL;
            switch( type ) {
              case STAT_TYPE:
                buffObj = vg->GetViewStatistics();
    	    break;
              case HIST_TYPE:
                buffObj = vg->GetViewHistogram();
    	    break;
              case GSTATX_TYPE:
                buffObj = vg->GetGdataStatisticsX();
    	    break;
              case GSTATY_TYPE:
                buffObj = vg->GetGdataStatisticsY();
    	    break;
    	}
           DOASSERT(buffObj != NULL, "didn't find stats buffer\n");
          
             if (!(buffObj ->Open("r") == StatusOk))
             {
    	     reportError("Cannot open statBuffer object for read", devNoSyserr);
    	     ReturnVal(API_NAK, "statBuffer object");
    	     return -1;
              }
              int k;
              args = new (char *) [MAX_GSTAT+1];
              for(k=0; k < MAX_GSTAT+1; k++) {
    	      args[k]=0;
              }
              char statBuff[LINESIZE];
              k = 0;
              while(buffObj->Fgets(statBuff, LINESIZE) != NULL) {
    	      DOASSERT(k < MAX_GSTAT, "too many stats lines");
    	      int len = strlen(statBuff);
    	      args[k] = new char[len + 1];
    	      strcpy(args[k], statBuff);
    	      k++;
               }
               ReturnVal(k, args);
               for(int j = 0; j < k; j++)
    	       delete args[j];
               delete [] args;
               return 1;
        }
    }
    return true;
}
int
DeviseCommand_getViewDimensions::Run(int argc, char** argv)
{
    {
        {
          View *vg = (View *)classDir->FindInstance(argv[1]);
          if (!vg) {
    	ReturnVal(API_NAK, "Cannot find view");
    	return -1;
          }
          /* Return number of dimensions in view */
          sprintf(result, "%d", vg->GetNumDimensions());
          ReturnVal(API_ACK, result);
          return 1;
        }
    }
    return true;
}
int
DeviseCommand_getViewSolid3D::Run(int argc, char** argv)
{
    {
        {
          View *vg = (View *)classDir->FindInstance(argv[1]);
          if (!vg) {
    	ReturnVal(API_NAK, "Cannot find view");
    	return -1;
          }
          /* Return setting of solid/wireframe 3D objects */
          sprintf(result, "%d", vg->GetSolid3D());
          ReturnVal(API_ACK, result);
          return 1;
        }
    }
    return true;
}
int
DeviseCommand_getViewXYZoom::Run(int argc, char** argv)
{
    {
        {
          View *vg = (View *)classDir->FindInstance(argv[1]);
          if (!vg) {
    	ReturnVal(API_NAK, "Cannot find view");
    	return -1;
          }
          /* Return setting of XY or X/Y zoom */
          sprintf(result, "%d", (vg->IsXYZoom() ? 1 : 0));
          ReturnVal(API_ACK, result);
          return 1;
        }
    }
    return true;
}
int
DeviseCommand_getViewDisplayDataValues::Run(int argc, char** argv)
{
    {
        {
          View *vg = (View *)classDir->FindInstance(argv[1]);
          if (!vg) {
    	ReturnVal(API_NAK, "Cannot find view");
    	return -1;
          }
          /* Return setting of data value display */
          sprintf(result, "%d", (vg->GetDisplayDataValues() ? 1 : 0));
          ReturnVal(API_ACK, result);
          return 1;
        }
    }
    return true;
}
int
DeviseCommand_getViewPileMode::Run(int argc, char** argv)
{
    {
        {
          View *vg = (View *)classDir->FindInstance(argv[1]);
          if (!vg) {
    	ReturnVal(API_NAK, "Cannot find view");
    	return -1;
          }
          /* Return pile mode flag */
          sprintf(result, "%d", (vg->IsInPileMode() ? 1 : 0));
          ReturnVal(API_ACK, result);
          return 1;
        }
    }
    return true;
}
int
DeviseCommand_raiseView::Run(int argc, char** argv)
{
    {
        {
          View *view = (View *)classDir->FindInstance(argv[1]);
          if (!view) {
    	ReturnVal(API_NAK, "Cannot find view");
    	return -1;
          }
          view->Raise();
          ReturnVal(API_ACK, "done");
          return 1;
        }
    }
    return true;
}
int
DeviseCommand_lowerView::Run(int argc, char** argv)
{
    {
        {
          View *view = (View *)classDir->FindInstance(argv[1]);
          if (!view) {
    	ReturnVal(API_NAK, "Cannot find view");
    	return -1;
          }
          view->Lower();
          ReturnVal(API_ACK, "done");
          return 1;
        }
    }
    return true;
}
int
DeviseCommand_getFileHeader::Run(int argc, char** argv)
{
    {
        {
          ReturnVal(API_ACK, DevFileHeader::Get(argv[1]));
          return 1;
        }
    }
    return true;
}

int
DeviseCommand_winGetPrint::Run(int argc, char** argv)
{
    {
        {
          ViewWin *win = (ViewWin *)classDir->FindInstance(argv[1]);
          if (!win) {
            ReturnVal(API_NAK, "Cannot find window");
            return -1;
          }
          Boolean exclude, pixmap;
          exclude = win->GetPrintExclude();
          pixmap = win->GetPrintPixmap();
          char buf[100];
          sprintf(buf, "%d %d", exclude, pixmap);
          ReturnVal(API_ACK, buf);
          return 1;
        }
    }
    return true;
}
int
DeviseCommand_viewGetHome::Run(int argc, char** argv)
{
    {
        {
          // Arguments: <viewName>
          // Returns: <doHomeX> <doHomeY> <mode> <autoYMinZero> <autoXMargin>
		  // <autoYMargin> <manXLo> <manYLo> <manXHi> <manYHi>
    #if defined(DEBUG)
          printf("viewGetHome <%s>\n", argv[1]);
    #endif
          ViewGraph *view = (ViewGraph *)classDir->FindInstance(argv[1]);
          if (!view) {
    	ReturnVal(API_NAK, "Cannot find view");
    	return -1;
          }
          ViewHomeInfo info;
          view->GetHomeInfo(info);
          char buf[100];
          sprintf(buf, "%d %d %d %d %f %f %f %f %f %f", info.homeX, info.homeY,
		    (int)info.mode, info.autoYMinZero,
			info.autoXMargin, info.autoYMargin, info.manXLo,
			info.manYLo, info.manXHi, info.manYHi);
          ReturnVal(API_ACK, buf);
          return 1;
        }
    }
    return true;
}
int
DeviseCommand_viewGetHorPan::Run(int argc, char** argv)
{
    {
        {
          // Arguments: <viewName>
          // Returns: <mode> <relativePan> <absolutePan>
    #if defined(DEBUG)
          printf("viewGetHorPan <%s>\n", argv[1]);
    #endif
          ViewGraph *view = (ViewGraph *)classDir->FindInstance(argv[1]);
          if (!view) {
    	ReturnVal(API_NAK, "Cannot find view");
    	return -1;
          }
          ViewPanInfo info;
          view->GetHorPanInfo(info);
          char buf[100];
          sprintf(buf, "%d %f %f", (int) info.mode, info.relPan, info.absPan);
          ReturnVal(API_ACK, buf);
          return 1;
        }
    }
    return true;
}
int
DeviseCommand_getCursorGrid::Run(int argc, char** argv)
{
    {
        {
          // Arguments: <cursorName>
          // Returns: <useGrid> <gridX> <gridY>
    #if defined(DEBUG)
          printf("getCursorGrid <%s>\n", argv[1]);
    #endif
          DeviseCursor *cursor = (DeviseCursor *) classDir->FindInstance(argv[1]);
          if (!cursor) {
    	ReturnVal(API_NAK, "Cannot find cursor");
    	return -1;
          }
          Boolean useGrid;
          Coord gridX, gridY;
          cursor->GetGrid(useGrid, gridX, gridY);
          char buf[100];
          sprintf(buf, "%d %f %f", useGrid, gridX, gridY);
          ReturnVal(API_ACK, buf);
          return 1;
        }
    }
    return true;
}

IMPLEMENT_COMMAND_BEGIN(writeDesc)
// Arguments: <file name> [<physical>]
// Returns: "done"

#if defined(DEBUG)
	printf("DEVise writeDesc\n");
#endif

	if ((argc == 2) || (argc == 3)) {
		Boolean physical;
		if (argc == 2) {
			physical = true;
		} else {
			physical = atoi(argv[2]);
		}

		if (SessionDesc::Write(argv[1], physical).IsComplete()) {
			ReturnVal(API_ACK, "done");
			return 1;
		} else {
			ReturnVal(API_NAK, "Error saving session description");
			return -1;
		}
	} else {
		ReturnVal(API_NAK,
		  "Wrong number of arguments for DEVise writeDesc");
		return -1;
	}
IMPLEMENT_COMMAND_END

int
DeviseCommand_saveStringSpace::Run(int argc, char** argv)
{
    {
        {
          // Argument: <file name>
    #if defined(DEBUG)
          printf("saveStringSpace <%s>\n", argv[1]);
    #endif
          if (StringStorage::SaveAll(argv[1]) != 0) {
            ReturnVal(API_NAK, "can't save string space");
            return -1;
          }
          ReturnVal(API_ACK, "done");
          return 1;
        }
    }
    return true;
}
int
DeviseCommand_loadStringSpace::Run(int argc, char** argv)
{
    {
        {
          // Argument: <file name>
    #if defined(DEBUG)
          printf("loadStringSpace <%s>\n", argv[1]);
    #endif
          if (StringStorage::LoadAll(argv[1]) != 0) {
            ReturnVal(API_NAK, "can't load string space");
            return -1;
          }
          View::RefreshAll();
          ReturnVal(API_ACK, "done");
          return 1;
        }
    }
    return true;
}
int
DeviseCommand_dumpLinkCursor::Run(int argc, char** argv)
{
    {
        {
          // Argument: <file name>
    #if defined(DEBUG)
          printf("dumpLinkCursor <%s>\n", argv[1]);
    #endif
          FILE *fp = fopen(argv[1], "w");
          if (fp == NULL) {
            ReturnVal(API_NAK, "can't dump link/cursor info");
            return -1;
          }
          char *header = DevFileHeader::Get(FILE_TYPE_LINKDESC);
          fprintf(fp, "%s", header);
          DevLink::Dump(fp);
          DevCursor::Dump(fp);
          (void) fclose(fp);
          ReturnVal(API_ACK, "done");
          return 1;
        }
    }
    return true;
}
int
DeviseCommand_openSession::Run(int argc, char** argv)
{
    {
        {
          // Argument: <file name>
    #if defined(DEBUG)
          printf("openSession <%s>\n", argv[1]);
    #endif
          if (!Session::Open(argv[1]).IsComplete()) {
    	ReturnVal(API_NAK, "Unable to open session file");
    	return -1;
          }
          ReturnVal(API_ACK, "done");
          return 1;
        }
    }
    return true;
}
int
DeviseCommand_createTData::Run(int argc, char** argv)
{
    {
        {
          // Argument: <data source name>
    #if defined(DEBUG)
          printf("createTData <%s>\n", argv[1]);
    #endif
		  TData *tdata = (TData *)classDir->FindInstance(argv[1]);
		  if (!tdata) {
          	if (!Session::CreateTData(argv[1]).IsComplete()) {
           	 	ReturnVal(API_NAK, "Unable to create tdata");
            	return -1;
          	}
		  }
          //TEMP -- may need to return something different here
          ReturnVal(API_ACK, "done");
          return 1;
        }
    }
    return true;
}
int
DeviseCommand_getViewGDS::Run(int argc, char** argv)
{
    {
        {
          // Argument: <view name>
          // Returns: <draw to screen> <send to socket> <port number> <file>
          //  <send text> <separator>
    #if defined(DEBUG)
          printf("getViewGDS <%s>\n", argv[1]);
    #endif
          ViewGraph *view = (ViewGraph *)classDir->FindInstance(argv[1]);
          if (!view) {
    	ReturnVal(API_NAK, "Cannot find view");
    	return -1;
          }
          Boolean drawToScreen = view->GetDrawToScreen();
          Boolean sendToSocket = view->GetSendToSocket();
    
          GDataSock::Params params;
          view->GetSendParams(params);
          if (params.file == NULL) params.file = "";
          char buf[1024];
          sprintf(buf, "%d %d %d \"%s\" %d \"%c\"", drawToScreen, sendToSocket,
    	params.portNum, params.file, params.sendText, params.separator);
          ReturnVal(API_ACK, buf);
          return 1;
        }
    }
    return true;
}
int
DeviseCommand_testDataSock::Run(int argc, char** argv)
{
    {
        {
          // Arguments: <port number>
          // Returns: "done"
    #if defined(DEBUG)
          printf("testDataSock <%s>\n", argv[1]);
    #endif
          int port = atoi(argv[1]);
          control->OpenDataChannel(port);
          int fd = control->getFd();
          if (fd < 0) {
            reportErrSys("Cannot open data channel");
            ReturnVal(API_NAK, "Invalid socket to write");
            return -1;
          }
    
          char *testStr = "abcd0123456789";
          int numBytes = strlen(testStr) + 1;
          if (write(fd, testStr, numBytes) != numBytes) {
            reportErrSys("write error");
            ReturnVal(API_NAK, "write error");
            (void) close(fd);
            return -1;
          }
    
          close(fd);
          ReturnVal(API_ACK, "done");
          return 1;
        }
    }
    return true;
}
int
DeviseCommand_viewGetAlign::Run(int argc, char** argv)
{
    {
        {
          // Argument: <view name>
          // Returns: <alignment value>
    #if defined(DEBUG)
          printf("viewGetAlign <%s>\n", argv[1]);
    #endif
          ViewGraph *view = (ViewGraph *)classDir->FindInstance(argv[1]);
          if (!view) {
    	    ReturnVal(API_NAK, "Cannot find view");
    	    return -1;
          }
          char buf[1024];
          sprintf(buf, "%d", view->GetAlign());
          ReturnVal(API_ACK, buf);
          return 1;
        }
    }
    return true;
}
int
DeviseCommand_setLinkMaster::Run(int argc, char** argv)
{
    {
        {
          DeviseLink *link = (DeviseLink *)classDir->FindInstance(argv[1]);
          if (!link) {
    	ReturnVal(API_NAK, "Cannot find link");
    	return -1;
          }
          ViewGraph *view = (ViewGraph *)classDir->FindInstance(argv[2]);
          if (!view) {
    	ReturnVal(API_NAK, "Cannot find view");
    	return -1;
          }
          link->SetMasterView(view);
          ReturnVal(API_ACK, "done");
          return 1;
        }
    }
    return true;
}
int
DeviseCommand_setLinkType::Run(int argc, char** argv)
{
    {
        {
          RecordLink *link = (RecordLink *)classDir->FindInstance(argv[1]);
          if (!link) {
    	ReturnVal(API_NAK, "Cannot find link");
    	return -1;
          }
          if(atoi(argv[2]) == 0) {
    	link->SetLinkType(Positive);
          } else if(atoi(argv[2]) == 1) {
    	link->SetLinkType(Negative);
          }
          return 1;
        }
    }
    return true;
}
int
DeviseCommand_setScreenSize::Run(int argc, char** argv)
{
    {
        {
          DeviseDisplay::DefaultDisplay()->DesiredScreenWidth() = atoi(argv[1]);
          DeviseDisplay::DefaultDisplay()->DesiredScreenHeight() = atoi(argv[2]);
          ReturnVal(API_ACK, "done");
          return 1;
        }
    }
    return true;
}
int
DeviseCommand_writeLine::Run(int argc, char** argv)
{
    {
        {
          FILE *file = (FILE *)atol(argv[2]);
          fputs(argv[1],file);
          fputs("\n",file);
          ReturnVal(API_ACK, "done");
          return 1;
        }
    }
    return true;
}
int
DeviseCommand_open::Run(int argc, char** argv)
{
    {
        {
          FILE *file = fopen(argv[1],argv[2]);
          if (!file) {
    	ReturnVal(API_NAK, "Cannot open file");
    	return -1;
          }
          sprintf(result, "%ld", (long)file);
          ReturnVal(API_ACK, result);
          return 1;
        }
    }
    return true;
}
int
DeviseCommand_setViewStatistics::Run(int argc, char** argv)
{
    {
        {
          ViewGraph *vg = (ViewGraph *)classDir->FindInstance(argv[1]);
          if (!vg) {
    	ReturnVal(API_NAK, "Cannot find view");
    	return -1;
          }
          /* Turn on/off display of statistics */
          vg->SetDisplayStats(argv[2]);
          ReturnVal(API_ACK, "done");
          return 1;
        }
    }
    return true;
}
int
DeviseCommand_setViewDimensions::Run(int argc, char** argv)
{
    {
        {
          View *vg = (View *)classDir->FindInstance(argv[1]);
          if (!vg) {
    	ReturnVal(API_NAK, "Cannot find view");
    	return -1;
          }
          /* Turn on/off display of statistics */
          vg->SetNumDimensions(atoi(argv[2]));
          ReturnVal(API_ACK, "done");
          return 1;
        }
    }
    return true;
}
int
DeviseCommand_setViewSolid3D::Run(int argc, char** argv)
{
    {
        {
          View *vg = (View *)classDir->FindInstance(argv[1]);
          if (!vg) {
    	ReturnVal(API_NAK, "Cannot find view");
    	return -1;
          }
          /* Set solid or wireframe 3D objects */
          vg->SetSolid3D(atoi(argv[2]));
          ReturnVal(API_ACK, "done");
          return 1;
        }
    }
    return true;
}
int
DeviseCommand_setViewXYZoom::Run(int argc, char** argv)
{
    {
        {
          View *vg = (View *)classDir->FindInstance(argv[1]);
          if (!vg) {
    	ReturnVal(API_NAK, "Cannot find view");
    	return -1;
          }
          /* Set XY or X/Y zoom */
          vg->SetXYZoom(atoi(argv[2]) ? true : false);
          ReturnVal(API_ACK, "done");
          return 1;
        }
    }
    return true;
}
int
DeviseCommand_setViewDisplayDataValues::Run(int argc, char** argv)
{
    {
        {
          View *vg = (View *)classDir->FindInstance(argv[1]);
          if (!vg) {
    	ReturnVal(API_NAK, "Cannot find view");
    	return -1;
          }
          /* Set data value display on/off */
          vg->SetDisplayDataValues(atoi(argv[2]) ? true : false);
          ReturnVal(API_ACK, "done");
          return 1;
        }
    }
    return true;
}
int
DeviseCommand_setViewPileMode::Run(int argc, char** argv)
{
	// This is now a no-op because setting the view's pile mode is now
	// handled by the PileStack class.  The command is left here only
	// for old session files.  RKW 1999-02-10.
	ReturnVal(API_ACK, "done");
	return 1;
}
int
DeviseCommand_savePixmap::Run(int argc, char** argv)
{
    {
        {
          View *vg = (View *)classDir->FindInstance(argv[1]);
          if (!vg) {
    	ReturnVal(API_NAK, "Cannot find view");
    	return -1;
          }
          FILE *file = (FILE *)atol(argv[2]);
          vg->SavePixmaps(file);
          ReturnVal(API_ACK, "done");
          return 1;
        }
    }
    return true;
}
int
DeviseCommand_loadPixmap::Run(int argc, char** argv)
{
    {
        {
          View *vg = (View *)classDir->FindInstance(argv[1]);
          if (!vg) {
    	ReturnVal(API_NAK, "Cannot find view");
    	return -1;
          }
          FILE *file = (FILE *)atol(argv[2]);
          vg->LoadPixmaps(file);
          ReturnVal(API_ACK, "done");
          return 1;
        }
    }
    return true;
}
int
DeviseCommand_getAxisDisplay::Run(int argc, char** argv)
{
    {
        {
          View *vg = (View *)classDir->FindInstance(argv[1]);
          if (!vg) {
    	ReturnVal(API_NAK, "Cannot find view");
    	return -1;
          }
          Boolean stat, xAxis, yAxis;
          vg->AxisDisplay(xAxis, yAxis);
          if (!strcmp(argv[2], "X"))
    	stat = xAxis;
          else
    	stat = yAxis;
          if (stat)
    	strcpy(result , "1");
          else
    	strcpy(result , "0");
          ReturnVal(API_ACK, result);
          return 1;
        }
    }
    return true;
}
int
DeviseCommand_replaceView::Run(int argc, char** argv)
{
    {
        {
          View *view1 = (View *)classDir->FindInstance(argv[1]);
          View *view2 = (View *)classDir->FindInstance(argv[2]);
          if (!view1 || !view2) {
    	ReturnVal(API_NAK, "Cannot find view");
    	return -1;
          }
          ViewWin *win = view1->GetParent();
          if (!win) {
    	ReturnVal(API_NAK, "Cannot find window");
    	return -1;
          }
          win->Replace(view1, view2);
          ReturnVal(API_ACK, "done");
          return 1;
        }
    }
    return true;
}
int
DeviseCommand_setCursorSrc::Run(int argc, char** argv)
{
    {
        {
          DeviseCursor *cursor = (DeviseCursor *)
    	classDir->FindInstance(argv[1]);
          if (!cursor) {
    	ReturnVal(API_NAK, "Cannot find cursor");
    	return -1;
          }
          View *view = (View *)classDir->FindInstance(argv[2]);
          if (!view) {
    	ReturnVal(API_NAK, "Cannot find view");
    	return -1;
          }
          cursor->SetSource(view);
          ReturnVal(API_ACK, "done");
          return 1;
        }
    }
    return true;
}
int
DeviseCommand_setCursorDst::Run(int argc, char** argv)
{
    {
        {
          DeviseCursor *cursor = (DeviseCursor *)
    	classDir->FindInstance(argv[1]);
          if (!cursor) {
    	ReturnVal(API_NAK, "Cannot find cursor");
    	return -1;
          }
          View *view = (View *)classDir->FindInstance(argv[2]);
          if (!view) {
    	ReturnVal(API_NAK, "Cannot find view");
    	return -1;
          }
          cursor->SetDst(view);
          ReturnVal(API_ACK, "done");
          return 1;
        } 
    }
    return true;
}
int
DeviseCommand_setPixelWidth::Run(int argc, char** argv)
{
    {
        {
          TDataMap *map= (TDataMap *)classDir->FindInstance(argv[1]);
          if (!map) {
    	ReturnVal(API_NAK, "Cannot find mapping");
    	return -1;
          }
          int width = atoi(argv[2]);
          if (width > 0 )
    	map->SetPixelWidth(width);
          ReturnVal(API_ACK, "done");
          return 1;
        }
    }
    return true;
}
int
DeviseCommand_setAction::Run(int argc, char** argv)
{
    {
        {
          ViewGraph *view = (ViewGraph *)classDir->FindInstance(argv[1]);
          if (!view) {
    	ReturnVal(API_NAK, "Cannot find view");
    	return -1;
          }
          Action *action = (Action*)classDir->FindInstance(argv[2]);
          if (!action) {
    	ReturnVal(API_NAK, "Cannot find action");
    	return -1;
          }
          view->SetAction(action);
          ReturnVal(API_ACK, "done");
          return 1;
        }
    }
    return true;
}
int
DeviseCommand_setLinkFlag::Run(int argc, char** argv)
{
    {
        {
          DeviseLink *link = (DeviseLink *)classDir->FindInstance(argv[1]);
          if (!link) {
    	ReturnVal(API_NAK, "Cannot find link");
    	return -1;
          }
          VisualFlag flag = atoi(argv[2]);
          link->SetFlag(flag);
          ReturnVal(API_ACK, "done");
          return 1;
        }
    }
    return true;
}
int
DeviseCommand_highlightView::Run(int argc, char** argv)
{
    {
        {
          View *view = (View *)classDir->FindInstance(argv[1]);
          if (!view) {
    	ReturnVal(API_NAK, "Cannot find view");
    	return -1;
          }
          view->DoSelect(atoi(argv[2]));
          ReturnVal(API_ACK, "done");
          return 1;
        }
    }
    return true;
}
int
DeviseCommand_getparam::Run(int argc, char** argv)
{
    {
        {
          classDir->GetParams(argv[1], argv[2], numArgs, args);
          ReturnVal(numArgs, args);
          return 1;
        }
    }
    return true;
}

int
DeviseCommand_insertMapping::Run(int argc, char** argv)
{
	if (argc ==3)
		return Run_3(argc, argv);
	else if (argc ==4)
		return Run_4(argc, argv);
	else
		fprintf(stderr, "Unknown command:%s\n", argv[0]);
	return 0;
}
int
DeviseCommand_insertMapping::Run_3(int argc, char** argv)
{
    {
        {
          ViewGraph *view = (ViewGraph *)classDir->FindInstance(argv[1]);
          if (!view) {
    	ReturnVal(API_NAK, "Cannot find view");
    	return -1;
          }
          TDataMap *map = (TDataMap *)classDir->FindInstance(argv[2]);
          if (!map) {
    	ReturnVal(API_NAK, "Cannot find mapping");
    	return -1;
          }
          view->InsertMapping(map);
          ReturnVal(API_ACK, "done");
          return 1;
        }
    }
    return true;
}
int
DeviseCommand_insertMapping::Run_4(int argc, char** argv)
{
    {
        {
          ViewGraph *view = (ViewGraph *)classDir->FindInstance(argv[1]);
          if (!view) {
    	ReturnVal(API_NAK, "Cannot find view");
    	return -1;
          }
          TDataMap *map = (TDataMap *)classDir->FindInstance(argv[2]);
          if (!map) {
    	ReturnVal(API_NAK, "Cannot find mapping");
    	return -1;
          }
          view->InsertMapping(map, argv[3]);
          ReturnVal(API_ACK, "done");
          return 1;
        }
    }
    return true;
}

int
DeviseCommand_getMappingLegend::Run(int argc, char** argv)
{
    {
        {
          ViewGraph *view = (ViewGraph *)classDir->FindInstance(argv[1]);
          if (!view) {
    	ReturnVal(API_NAK, "Cannot find view");
    	return -1;
          }
          TDataMap *map = (TDataMap *)classDir->FindInstance(argv[2]);
          if (!map) {
    	ReturnVal(API_NAK, "Cannot find mapping");
    	return -1;
          }
          ReturnVal(API_ACK, view->GetMappingLegend(map));
          return 1;
        }
    }
    return true;
}

int
DeviseCommand_insertLink::Run(int argc, char** argv)
{
  // Don't do anything for old-style pile links.
  if (!DeviseLink::IsPileLinkName(argv[1])) {
    DeviseLink *link = (DeviseLink *)classDir->FindInstance(argv[1]);
    if (!link) {
      ReturnVal(API_NAK, "Cannot find link");
      return -1;
    }
    //      printf("insertLink %s %s\n", argv[1], argv[2]);
    ViewGraph *view = (ViewGraph *)classDir->FindInstance(argv[2]);
    if (!view) {
      ReturnVal(API_NAK, "Cannot find view");
      return -1;
    }
    link->InsertView(view);
  }
  ReturnVal(API_ACK, "done");
  return 1;
}

int
DeviseCommand_viewInLink::Run(int argc, char** argv)
{
    {
        {
          DeviseLink *link = (DeviseLink *)classDir->FindInstance(argv[1]);
          if (!link) {
    	ReturnVal(API_NAK, "Cannot find link");
    	return -1;
          }
          ViewGraph *view = (ViewGraph *)classDir->FindInstance(argv[2]);
          if (!view) {
    	ReturnVal(API_NAK, "Cannot find view");
    	return -1;
          }
          if (link->ViewInLink(view))
    	strcpy(result, "1");
          else
    	strcpy(result , "0");
          ReturnVal(API_ACK, result);
          return 1;
        }
    }
    return true;
}
int
DeviseCommand_unlinkView::Run(int argc, char** argv)
{
    {
        {
          DeviseLink *link = (DeviseLink *)classDir->FindInstance(argv[1]);
          if (!link) {
    	ReturnVal(API_NAK, "Cannot find link");
    	return -1;
          }
          printf("unLink %s %s\n", argv[1], argv[2]);
          ViewGraph *view = (ViewGraph *)classDir->FindInstance(argv[2]);
          if (!view) {
    	ReturnVal(API_NAK, "Cannot find view");
    	return -1;
          }
          link->DeleteView(view);
          ReturnVal(API_ACK, "done");
          return 1;
        }
    }
    return true;
}
int
DeviseCommand_insertWindow::Run(int argc, char** argv)
{
    {
        {
          ViewGraph *view = (ViewGraph *)classDir->FindInstance(argv[1]);
          if (!view) {
    	ReturnVal(API_NAK, "Cannot find view");
    	return -1;
          }
          ViewWin *win = (ViewWin *)classDir->FindInstance(argv[2]);
          if (!win) {
    	ReturnVal(API_NAK, "Cannot find window");
    	return -1;
          }
          view->DeleteFromParent();
          view->AppendToParent(win);
          ReturnVal(API_ACK, "done");
          return 1;
        }
    }
    return true;
}
int
DeviseCommand_removeMapping::Run(int argc, char** argv)
{
    {
        {
          ViewGraph *view = (ViewGraph *)classDir->FindInstance(argv[1]);
          if (!view) {
    	ReturnVal(API_NAK, "Cannot find view");
    	return -1;
          }
          TDataMap *map = (TDataMap *)classDir->FindInstance(argv[2]);
          if (!map) {
    	ReturnVal(API_NAK, "Cannot find mapping");
    	return -1;
          }
          view->RemoveMapping(map);
          ReturnVal(API_ACK, "done");
          return 1;
        }
    }
    return true;
}
int
DeviseCommand_saveDisplayImage::Run(int argc, char** argv)
{
    {
        {
          DisplayExportFormat format = POSTSCRIPT;
          if (!strcmp(argv[1], "eps"))
    	format = EPS;
          else if (!strcmp(argv[1], "gif"))
    	format = GIF;
          DeviseDisplay::DefaultDisplay()->ExportImage(format, argv[2]);
          ReturnVal(API_ACK, "done");
          return 1;
        }
    }
    return true;
}
int
DeviseCommand_saveDisplayView::Run(int argc, char** argv)
{
    {
        {
          DisplayExportFormat format = POSTSCRIPT;
          if (!strcmp(argv[1], "eps"))
    	 format = EPS;
          else if (!strcmp(argv[1], "gif"))
    	 format = GIF;
          DeviseDisplay::DefaultDisplay()->ExportView(format, argv[2]);
          ReturnVal(API_ACK, "done");
          return 1;
        }
    }
    return true;
}
int
DeviseCommand_saveTdata::Run(int argc, char** argv)
{
    {
        {
          TData *tdata = (TData *)classDir->FindInstance(argv[1]);
          if (!tdata) {
    	ReturnVal(API_NAK, "Cannot find tdata");
    	return -1;
          }
          if (!(tdata->Save(argv[2]) == StatusFailed)) {
            ReturnVal(API_ACK, "done");
            return 1;
          } else {
    	ReturnVal(API_NAK, "Cannot save tdata");
    	return -1;
          }
        }
    }
    return true;
}
int
DeviseCommand_getDisplayImage::Run(int argc, char** argv)
{
    {
        {
          // Arguments: <port number> <image type>
          // Returns: "done"
          int port = atoi(argv[1]);
          if (strcmp(argv[2], "gif")) {
             ReturnVal(API_NAK, "Can only support GIF now.");
             return -1;
          }
          control->OpenDataChannel(port);
          int fd = control->getFd();
          if (fd < 0) {
    	reportErrSys("Cannot open data channel");
            ReturnVal(API_NAK, "Invalid socket to write");
            return -1;
          }
          FILE *fp = fdopen(control->getFd(), "wb");
          Timer::StopTimer();
          DeviseDisplay::DefaultDisplay()->ExportGIF(fp, 0);
          Timer::StartTimer();
          close(control->getFd());
          ReturnVal(API_ACK, "done");
          return 1;
        }
    }
    return true;
}
int
DeviseCommand_getDisplayImageAndSize::Run(int argc, char** argv)
{
    {
        {
          // Arguments: <port number> <image type>
          // Returns: "done"
          return GetDisplayImageAndSize(control, atoi(argv[1]), argv[2]);
        }
    }
    return true;
}
int
DeviseCommand_getFont::Run(int argc, char** argv)
{
    {
        {
          View *view = View::FindViewByName(argv[1]);
          if (view == NULL) {
            ReturnVal(API_NAK, "Cannot find view");
            return -1;
          }
    
          int family;
          float pointSize;
          Boolean bold;
          Boolean italic;
          view->GetFont(argv[2], family, pointSize, bold, italic);
          sprintf(result, "%d %d %d %d", family, (int) pointSize, bold, italic);
    
          ReturnVal(API_ACK, result);
          return 1;
        }
    }
    return true;
}
int
DeviseCommand_viewSetAlign::Run(int argc, char** argv)
{
    {
        {
          // Argument: <view name> <alignment value>
          // Returns: "done"
    #if defined(DEBUG)
          printf("viewSetAlign <%s> <%s>\n", argv[1], argv[2]);
    #endif
          ViewGraph *view = (ViewGraph *)classDir->FindInstance(argv[1]);
          if (!view) {
    	    ReturnVal(API_NAK, "Cannot find view");
    	    return -1;
          }
          int newAlign = atoi(argv[2]);
          if (view->GetAlign() != newAlign) {
            view->SetAlign(newAlign);
    	view->Refresh();
          }
          ReturnVal(API_ACK, "done");
          return 1;
        }
    }
    return true;
}
int
DeviseCommand_checkTDataForRecLink::Run(int argc, char** argv)
{
    {
        {
          DeviseLink *link = (DeviseLink *)classDir->FindInstance(argv[1]);
          if (!link) {
    	    ReturnVal(API_NAK, "Cannot find link");
    	    return -1;
          } 
          ViewGraph *view = (ViewGraph *)classDir->FindInstance(argv[2]);
          if (!view) {
    	    ReturnVal(API_NAK, "Cannot find view");
    	    return -1;
          }

	  if (link->GetFlag() & VISUAL_RECORD) {
	    RecordLink *recLink = (RecordLink *)link;
            sprintf(result, "%d",
	      (recLink->CheckTData(view, atoi(argv[3]))) ?  1 : 0);
            ReturnVal(API_ACK, result);
            return 1;
	  } else {
            ReturnVal(API_ACK, "1");
            return 1;
	  }
        }      
    }
    return true;
}
int
DeviseCommand_setMappingLegend::Run(int argc, char** argv)
{
    {
        {
          ViewGraph *view = (ViewGraph *)classDir->FindInstance(argv[1]);
          if (!view) {
    	ReturnVal(API_NAK, "Cannot find view");
    	return -1;
          }
          TDataMap *map = (TDataMap *)classDir->FindInstance(argv[2]);
          if (!map) {
    	ReturnVal(API_NAK, "Cannot find mapping");
    	return -1;
          }
          view->SetMappingLegend(map, argv[3]);
          ReturnVal(API_ACK, "done");
          return 1;
        }
    }
    return true;
}
int
DeviseCommand_markViewFilter::Run(int argc, char** argv)
{
    {
        {
          View *view = (View *)classDir->FindInstance(argv[1]);
          if (!view) {
    	ReturnVal(API_NAK, "Cannot find view");
    	return -1;
          }
          int index = atoi(argv[2]);
          Boolean mark = atoi(argv[3]);
          view->Mark(index, mark);
          ReturnVal(API_ACK, "done");
          return 1;
        }
    }
    return true;
}
int
DeviseCommand_getWindowImage::Run(int argc, char** argv)
{
    {
        {
          // Arguments: <port number> <image type> <window name>
          // Returns: "done"
          int port = atoi(argv[1]);
          if (strcmp(argv[2], "gif")) {
             ReturnVal(API_NAK, "Can only support GIF now.");
             return -1;
          }
          ViewWin *viewWin = (ViewWin *)classDir->FindInstance(argv[3]);
          if (!viewWin) {
            ReturnVal(API_NAK, "Cannot find window");
            return -1;
          }
    
          control->OpenDataChannel(port);
          int fd = control->getFd();
          if (fd < 0) {
    	reportErrSys("Cannot open data channel");
            ReturnVal(API_NAK, "Invalid socket to write");
            return -1;
          }
          FILE *fp = fdopen(control->getFd(), "wb");
          Timer::StopTimer();
          viewWin->GetWindowRep()->ExportGIF(fp);
          Timer::StartTimer();
          close(control->getFd());
          ReturnVal(API_ACK, "done");
          return 1;
        }
    }
    return true;
}
int
DeviseCommand_getWindowImageAndSize::Run(int argc, char** argv)
{
    {
        {
          // Arguments: <port number> <image type> <window name>
          // Returns: "done"
          return GetWindowImageAndSize(control, atoi(argv[1]), argv[2], argv[3]);
        }
    }
    return true;
}
int
DeviseCommand_swapView::Run(int argc, char** argv)
{
    {
        {
          ViewWin *viewWin = (ViewWin *)classDir->FindInstance(argv[1]);
          View *view1 = (View *)classDir->FindInstance(argv[2]);
          View *view2 = (View *)classDir->FindInstance(argv[3]);
          if (!viewWin || !view1 || !view2) {
    	ReturnVal(API_NAK, "Cannot find view or window");
    	return -1;
          }
          if (view1->GetParent() != viewWin || view2->GetParent() != viewWin) {
    	ReturnVal(API_NAK, "Views not in same window");
    	return -1;
          }
          viewWin->SwapChildren(view1, view2);
          ReturnVal(API_ACK, "done");
          return 1;
        }
    }
    return true;
}
int
DeviseCommand_setAxisDisplay::Run(int argc, char** argv)
{
    {
        {
          View *vg = (View *)classDir->FindInstance(argv[1]);
          if (!vg) {
    	ReturnVal(API_NAK, "Cannot find view");
    	return -1;
          }
          Boolean stat = atoi(argv[3]);
          if (!strcmp(argv[2], "X"))
    	vg->XAxisDisplayOnOff(stat);
          else
    	vg->YAxisDisplayOnOff(stat);
          ReturnVal(API_ACK, "done");
          return 1;
        }
    }
    return true;
}
int
DeviseCommand_getCreateParam::Run(int argc, char** argv)
{
    {
        {
          classDir->CreateParams(argv[1], argv[2], argv[3], numArgs, args);
          ReturnVal(numArgs, args);
          return 1;
        }
    }
    return true;
}
int
DeviseCommand_getItems::Run(int argc, char** argv)
{
    {
        {
          control->GetGroupDir()->get_items(result, argv[1], argv[2], argv[3]);
          ReturnVal(API_ACK, result);
          return 1;
        }
    }
    return true;
}

int
DeviseCommand_setWindowLayout::Run(int argc, char** argv)
{
	if (argc ==4)
		return Run_4(argc, argv);
	else if (argc ==5)
		return Run_5(argc, argv);
	else
		fprintf(stderr, "Unknown command:%s\n", argv[0]);
	return 0;
}
int
DeviseCommand_setWindowLayout::Run_4(int argc, char** argv)
{
    {
        {
          ViewLayout *layout = (ViewLayout *)classDir->FindInstance(argv[1]);
          if (!layout) {
    	ReturnVal(API_NAK, "Cannot find window");
    	return -1;
          }
          layout->SetPreferredLayout(atoi(argv[2]), atoi(argv[3]));
          ReturnVal(API_ACK, "done");
          return 1;
        }
    }
    return true;
}
int
DeviseCommand_setWindowLayout::Run_5(int argc, char** argv)
{
    {
        {
          ViewLayout *layout = (ViewLayout *)classDir->FindInstance(argv[1]);
          if (!layout) {
    	ReturnVal(API_NAK, "Cannot find window");
    	return -1;
          }
		  if (atoi(argv[4])) {
			//
			// Note: if we're setting the window to stacked mode here, we
			// assume that this is probably a command in a pre-PileStack
			// session file, and therefore we set the PileStack object to
			// PiledLinked mode.  Note that old sessions with stacked
			// views will now end up with the views piled intead of stacked.
			// RKW 1999-02-10.
			//
		    layout->GetPileStack()->SetState(PileStack::PSPiledLinked);
		  } else {
            layout->SetPreferredLayout(atoi(argv[2]), atoi(argv[3]),
    				   (atoi(argv[4]) ? true : false));
		  }
          ReturnVal(API_ACK, "done");
          return 1;
        }
    }
    return true;
}
int
DeviseCommand_saveWindowImage::Run(int argc, char** argv)
{
    {
        {
          DisplayExportFormat format = POSTSCRIPT;
          if (!strcmp(argv[1], "eps"))
    	format = EPS;
          else if (!strcmp(argv[1], "gif"))
    	format = GIF;
          ViewWin *viewWin = (ViewWin *)classDir->FindInstance(argv[2]);
          if (!viewWin) {
    	ReturnVal(API_NAK, "Cannot find window");
    	return -1;
          }
          viewWin->ExportImage(format, argv[3]);
    
          ReturnVal(API_ACK, "done");
          return 1;
        }
    }
    return true;
}
int
DeviseCommand_setViewOverrideColor::Run(int argc, char** argv)
{
    {
        {
    		ReturnVal(API_ACK, "done");
    		return 1;
        }
    }
    return true;
}
int
DeviseCommand_parseSchema::Run(int argc, char** argv)
{
    {
        {
          char *name = ParseSchema(argv[1], argv[2], argv[3]);
          if (!name) {
    	ReturnVal(API_NAK, "Cannot parse schema(s)");
    	return -1;
          }
          ReturnVal(API_ACK, name);
          return 1;
        }
    }
    return true;
}
int
DeviseCommand_winSetPrint::Run(int argc, char** argv)
{
    {
        {
    #if defined(DEBUG)
          printf("winSetPrint %s %s %s\n", argv[1], argv[2], argv[3]);
    #endif
          ViewWin *win = (ViewWin *)classDir->FindInstance(argv[1]);
          if (!win) {
            ReturnVal(API_NAK, "Cannot find window");
            return -1;
          }
          Boolean exclude = atoi(argv[2]);
          Boolean pixmap = atoi(argv[3]);
          win->SetPrintExclude(exclude);
          win->SetPrintPixmap(pixmap);
          ReturnVal(API_ACK, "done");
          return 1;
        }
    }
    return true;
}
int
DeviseCommand_setLabel::Run(int argc, char** argv)
{
    {
        {
          View *view = (View *)classDir->FindInstance(argv[1]);
          if (!view) {
    	ReturnVal(API_NAK, "Cannot find view");
    	return -1;
          }
          view->SetLabelParam(atoi(argv[2]), atoi(argv[3]), argv[4]);
          ReturnVal(API_ACK, "done");
          return 1;
        }
    }
    return true;
}
int
DeviseCommand_dataSegment::Run(int argc, char** argv)
{
    {
        {
          DataSeg::Set(argv[1], argv[2], atoi(argv[3]), atoi(argv[4]));
          ReturnVal(API_ACK, "done");
          return 1;
        }
    }
    return true;
}
int
DeviseCommand_viewSetHorPan::Run(int argc, char** argv)
{
    {
        {
          // Arguments: <viewName> <mode> <relativePan> <absolutePan>
    #if defined(DEBUG)
          printf("viewSetHorPan <%s> <%s> <%s> <%s>\n", argv[1], argv[2], argv[3],
    	argv[4]);
    #endif
          ViewGraph *view = (ViewGraph *)classDir->FindInstance(argv[1]);
          if (!view) {
    	ReturnVal(API_NAK, "Cannot find view");
    	return -1;
          }
          ViewPanInfo info;
          info.mode = (ViewPanMode) atoi(argv[2]);
          info.relPan = atof(argv[3]);
          info.absPan = atof(argv[4]);
          view->SetHorPanInfo(info);
          ReturnVal(API_ACK, "done");
          return 1;
        }
    }
    return true;
}
int
DeviseCommand_setCursorGrid::Run(int argc, char** argv)
{
    {
        {
          // Arguments: <cursorName> <useGrid> <gridX> <gridY>
    #if defined(DEBUG)
          printf("setCursorGrid <%s>\n", argv[1]);
    #endif
          DeviseCursor *cursor = (DeviseCursor *) classDir->FindInstance(argv[1]);
          if (!cursor) {
    	ReturnVal(API_NAK, "Cannot find cursor");
    	return -1;
          }
          Boolean useGrid = atoi(argv[2]);
          Coord gridX = atof(argv[3]);
          Coord gridY = atof(argv[4]);
          cursor->SetGrid(useGrid, gridX, gridY);
          ReturnVal(API_ACK, "done");
          return 1;
        }
    }
    return true;
}
int
DeviseCommand_saveSession::Run(int argc, char** argv)
{
    {
        {
          // Arguments: <file name> <as template> <as export> <with data>
    #if defined(DEBUG)
          printf("saveSession <%s> <%s> <%s> <%s>\n", argv[1], argv[2], argv[3],
    	argv[4]);
    #endif
          if (!Session::Save(argv[1], atoi(argv[2]), atoi(argv[3]),
    	  atoi(argv[4])).IsComplete()) {
    	ReturnVal(API_NAK, "Unable to save session file");
    	return -1;
          }
          ReturnVal(API_ACK, "done");
          return 1;
        }
    }
    return true;
}
int
DeviseCommand_setWinGeometry::Run(int argc, char** argv)
{
    {
        {
          View *view = (View *)classDir->FindInstance(argv[1]);
          if (!view) {
    	ReturnVal(API_NAK, "Cannot find view");
    	return -1;
          }
          int x, y;
          unsigned h, w;
          x = atoi(argv[2]);
          y = atoi(argv[3]);
          w = atoi(argv[4]);
          h = atoi(argv[5]);
          printf("%d %d %u %u\n", x, y, w, h);
          view->SetGeometry(x, y, w, h);
          ReturnVal(API_ACK, "done");
          return 1;
        }  
    }
    return true;
}
int
DeviseCommand_setFilter::Run(int argc, char** argv)
{
    {
        {
          View *view = (View *)classDir->FindInstance(argv[1]);
          if (!view) {
    	ReturnVal(API_NAK, "Cannot find view");
    	return -1;
          }
          VisualFilter filter;
          view->GetVisualFilter(filter);
          if (!ParseFloatDate(argv[2], filter.xLow) ||
    	  !ParseFloatDate(argv[3], filter.yLow) ||
    	  !ParseFloatDate(argv[4], filter.xHigh) ||
    	  !ParseFloatDate(argv[5], filter.yHigh)) {
    	ReturnVal(API_NAK, "invalid date or float");
    	return -1;
          }
          view->SetVisualFilter(filter);
          ReturnVal(API_ACK, "done");
          return 1;
        }
    }
    return true;
}
int
DeviseCommand_saveDisplayImageAndMap::Run(int argc, char** argv)
{
    {
        {
          DisplayExportFormat format = POSTSCRIPT;
          if (!strcmp(argv[1], "eps"))
            format = EPS;
          else if (!strcmp(argv[1], "gif"))
            format = GIF;
          DeviseDisplay::DefaultDisplay()->ExportImageAndMap(format, argv[2], 
    					argv[3], argv[4], argv[5]);
          ReturnVal(API_ACK, "done");
          return 1;
        }
    }
    return true;
}
int
DeviseCommand_insertViewHistory::Run(int argc, char** argv)
{
    {
        {
          View *view = (View *)classDir->FindInstance(argv[1]);
          if (!view) {
    	ReturnVal(API_NAK, "Cannot find view");
    	return -1;
          }
          VisualFilter filter;
          (void)ParseFloatDate(argv[2],filter.xLow); 
          (void)ParseFloatDate(argv[3],filter.yLow);
          (void)ParseFloatDate(argv[4],filter.xHigh); 
          (void)ParseFloatDate(argv[5],filter.yHigh);
          filter.marked = atoi(argv[6]);
          view->InsertHistory(filter);
          ReturnVal(API_ACK, "done");
          return 1;
        }
    }
    return true;
}
int
DeviseCommand_setFont::Run(int argc, char** argv)
{
    {
        {
          View *view = View::FindViewByName(argv[1]);
          if (view == NULL) {
            ReturnVal(API_NAK, "Cannot find view");
            return -1;
          }
    
          view->SetFont(argv[2], atoi(argv[3]), atof(argv[4]), atoi(argv[5]),
    	atoi(argv[6]));
    
          ReturnVal(API_ACK, "done");
          return 1;
        }
    }
    return true;
}
int
DeviseCommand_set3DLocation::Run(int argc, char** argv)
{
    {
        {
          ViewGraph *view = (ViewGraph *)classDir->FindInstance(argv[1]);
          if (!view) {
            ReturnVal(API_NAK, "Cannot find view");
            return -1;
          }
          if (argc ==11)  {

	    ViewDir dir;
            Camera c=view->GetCamera();

            if (Char2ViewDir(dir, argv[2])) {
	      c.view_dir = dir;
              c.min_x = atof(argv[3]);
              c.max_x = atof(argv[4]);
              c.min_y = atof(argv[5]);
              c.max_y = atof(argv[6]);
              c.near = atof(argv[7]);
              c.far = atof(argv[8]);
              c.pan_right = atof(argv[9]);
              c.pan_up = atof(argv[10]);
            }

            view->SetCamera(c);
          }
          ReturnVal(API_ACK, "done");
          return 1;
        }
    }
    return true;
}
int
DeviseCommand_setViewGDS::Run(int argc, char** argv)
{
    {
        {
          // Argument: <view name> <draw to screen> <send to socket>
          //   <port number> <file> <send text> <separator>
          // Returns: "done"
    #if defined(DEBUG)
          printf("setViewGDS <%s>\n", argv[1]);
    #endif
          ViewGraph *view = (ViewGraph *)classDir->FindInstance(argv[1]);
          if (!view) {
    	ReturnVal(API_NAK, "Cannot find view");
    	return -1;
          }
    
          view->SetDrawToScreen(atoi(argv[2]));
          view->SetSendToSocket(atoi(argv[3]));
    
          GDataSock::Params params;
          params.portNum = atoi(argv[4]);
          params.file = argv[5];
          params.sendText = atoi(argv[6]);
          params.separator = argv[7][0];
          view->SetSendParams(params);
    
          ReturnVal(API_ACK, "done");
          return 1;
        }
    }
    return true;
}
int
DeviseCommand_viewSetHome::Run(int argc, char** argv)
{
    // Arguments: <viewName> [<doHomeX> <doHomeY>] <mode> [<autoYMinZero>]
    // <autoXMargin> <autoYMargin> <manXLo> <manYLo> <manXHi> <manYHi>
#if defined(DEBUG)
    PrintArgs(stdout, argc, argv);
#endif
	if (argc == 9 || argc == 12) {
        ViewGraph *view = (ViewGraph *)classDir->FindInstance(argv[1]);
        if (!view) {
    	    ReturnVal(API_NAK, "Cannot find view");
    	    return -1;
        }

        ViewHomeInfo info;
		int argNum = 2;

		// Old version of this command didn't include these.
		if (argc == 12) {
		  info.homeX = atoi(argv[argNum++]);
		  info.homeY = atoi(argv[argNum++]);
		}

        info.mode = (ViewHomeMode) atoi(argv[argNum++]);
		if (argc == 12) {
	      info.autoYMinZero = atoi(argv[argNum++]);
		}
        info.autoXMargin = atof(argv[argNum++]);
        info.autoYMargin = atof(argv[argNum++]);
        info.manXLo = atof(argv[argNum++]);
        info.manYLo = atof(argv[argNum++]);
        info.manXHi = atof(argv[argNum++]);
        info.manYHi = atof(argv[argNum++]);
        view->SetHomeInfo(info);

        ReturnVal(API_ACK, "done");
        return 1;
	} else {
		fprintf(stderr,"Wrong # of arguments: %d in viewSetHome\n", argc);
    	ReturnVal(API_NAK, "Wrong # of arguments");
    	return -1;
	}
}

IMPLEMENT_COMMAND_BEGIN(playLog)
	char	*logFile;
	char	*errmsg = NULL;

	static	long	lastLogId = CmdLogRecord::LOG_BEGIN;
	if (argc != 6)
	{
		fprintf(stderr,"Wrong # of arguments:%d in playLog\n", argc);
    	ReturnVal(API_NAK, "Wrong # of arguments");
    	return -1;
	}
	if (!strcmp(argv[1], "default"))
	{
		logFile = cmdContainerp->getLogName();
	}
	else
	{
		logFile = argv[1];
	}
	FILE* fp= fopen(logFile,"r");
	if (fp == NULL)
	{
		errmsg = "Open log file failure";
		ReturnVal(API_NAK, errmsg);
		fprintf(stderr,"%s\n", errmsg);
		return -1;
	}
	fclose(fp);

	int	by = atoi(argv[2]);
	int pause = atoi(argv[3]);
	long	start = atol(argv[4]);
	long	end = atol(argv[5]);

	// it is up to the last scheduled Run to garbage-collect this object
	CmdLogRecord 	*cmdLog= new CmdLogRecord(logFile);
	bool success = cmdLog->playLog( by, pause, start, end, errmsg);
	if (success)
	{
    	ReturnVal(API_ACK, "done");
		return 1;
	}
	else
	{
		fprintf(stderr, "Play log failure:%s\n",errmsg);
    	ReturnVal(API_NAK, errmsg);
		return -1;
	}
IMPLEMENT_COMMAND_END

IMPLEMENT_COMMAND_BEGIN(test)
// Note: modify this code to do whatever you need to test.
    if (argc == 1) {
        ViewGraph *view = (ViewGraph *)View::FindSelectedView();
		if (view != NULL) {
		  view->NiceifyAxes(true, true);
		}

       	ReturnVal(API_ACK, "done");
		return 1;
	} else {
		fprintf(stderr,"Wrong # of arguments: %d in test\n", argc);
    	ReturnVal(API_NAK, "Wrong # of arguments");
    	return -1;
	}
IMPLEMENT_COMMAND_END

IMPLEMENT_COMMAND_BEGIN(getLinkMasterAttr)
    // Arguments: <linkName>
    // Returns: <attrName>
#if defined(DEBUG)
    PrintArgs(stdout, argc, argv);
#endif

    if (argc == 2) {
        TAttrLink *link = (TAttrLink *)classDir->FindInstance(argv[1]);
        if (!link) {
            ReturnVal(API_NAK, "Cannot find link");
            return -1;
        }
		const char *attrName = link->GetMasterAttrName();
    	ReturnVal(API_ACK, (char *)attrName);
	} else {
		fprintf(stderr,"Wrong # of arguments: %d in getLinkMasterAttr\n", argc);
    	ReturnVal(API_NAK, "Wrong # of arguments");
    	return -1;
	}
IMPLEMENT_COMMAND_END

IMPLEMENT_COMMAND_BEGIN(getLinkSlaveAttr)
    // Arguments: <linkName>
    // Returns: <attrName>
#if defined(DEBUG)
    PrintArgs(stdout, argc, argv);
#endif

    if (argc == 2) {
        TAttrLink *link = (TAttrLink *)classDir->FindInstance(argv[1]);
        if (!link) {
            ReturnVal(API_NAK, "Cannot find link");
            return -1;
        }
		const char *attrName = link->GetSlaveAttrName();
    	ReturnVal(API_ACK, (char *)attrName);
	} else {
		fprintf(stderr,"Wrong # of arguments: %d in getLinkSlaveAttr\n", argc);
    	ReturnVal(API_NAK, "Wrong # of arguments");
    	return -1;
	}
IMPLEMENT_COMMAND_END

IMPLEMENT_COMMAND_BEGIN(setLinkMasterAttr)
    // Arguments: <linkName> <attrName>
    // Returns: "done"
#if defined(DEBUG)
    PrintArgs(stdout, argc, argv);
#endif

    if (argc == 3) {
        TAttrLink *link = (TAttrLink *)classDir->FindInstance(argv[1]);
        if (!link) {
            ReturnVal(API_NAK, "Cannot find link");
            return -1;
        }
		link->SetMasterAttr(argv[2]);
        ReturnVal(API_ACK, "done");
		return 1;
	} else {
		fprintf(stderr,"Wrong # of arguments: %d in setLinkMasterAttr\n", argc);
    	ReturnVal(API_NAK, "Wrong # of arguments");
    	return -1;
	}
IMPLEMENT_COMMAND_END

IMPLEMENT_COMMAND_BEGIN(setLinkSlaveAttr)
    // Arguments: <linkName> <attrName>
    // Returns: "done"
#if defined(DEBUG)
    PrintArgs(stdout, argc, argv);
#endif

    if (argc == 3) {
        TAttrLink *link = (TAttrLink *)classDir->FindInstance(argv[1]);
        if (!link) {
            ReturnVal(API_NAK, "Cannot find link");
            return -1;
        }
		link->SetSlaveAttr(argv[2]);
        ReturnVal(API_ACK, "done");
		return 1;
	} else {
		fprintf(stderr,"Wrong # of arguments: %d in setLinkSlaveAttr\n", argc);
    	ReturnVal(API_NAK, "Wrong # of arguments");
    	return -1;
	}
IMPLEMENT_COMMAND_END

IMPLEMENT_COMMAND_BEGIN(selectView)
    // Arguments: <viewName>
    // Returns: "done"
#if defined(DEBUG)
    PrintArgs(stdout, argc, argv);
#endif

    if (argc == 2) {
        View *view = (View *)classDir->FindInstance(argv[1]);
		if (view != NULL) {
		    view->SelectView();
            ReturnVal(API_ACK, "done");
		    return 1;
		} else {
    	    ReturnVal(API_NAK, "Cannot find view");
    	    return -1;
		}
	} else {
		fprintf(stderr,"Wrong # of arguments: %d in selectView\n", argc);
    	ReturnVal(API_NAK, "Wrong # of arguments");
    	return -1;
	}
IMPLEMENT_COMMAND_END

IMPLEMENT_COMMAND_BEGIN(setShowViewNames)
    // Arguments: <showViewNames (boolean)>
    // Returns: "done"
#if defined(DEBUG)
    PrintArgs(stdout, argc, argv);
#endif

    if (argc == 2) {
		View::SetShowNames(atoi(argv[1]));
        ReturnVal(API_ACK, "done");
	    return 1;
	} else {
		fprintf(stderr,"Wrong # of arguments: %d in setShowViewNames\n", argc);
    	ReturnVal(API_NAK, "Wrong # of arguments");
    	return -1;
	}
IMPLEMENT_COMMAND_END

IMPLEMENT_COMMAND_BEGIN(getShowViewNames)
    // Arguments: none
    // Returns: <showViewNames (boolean)>
#if defined(DEBUG)
    PrintArgs(stdout, argc, argv);
#endif

    if (argc == 1) {
		char buf[128];
		sprintf(buf, "%d", View::GetShowNames());
        ReturnVal(API_ACK, buf);
	    return 1;
	} else {
		fprintf(stderr,"Wrong # of arguments: %d in getShowViewNames\n", argc);
    	ReturnVal(API_NAK, "Wrong # of arguments");
    	return -1;
	}
IMPLEMENT_COMMAND_END

IMPLEMENT_COMMAND_BEGIN(getCountMapping)
    // Arguments: <view name>
    // Returns: <enabled> <count attr> <put attr>
#if defined(DEBUG)
    PrintArgs(stdout, argc, argv);
#endif

    if (argc == 2) {
        ViewGraph *view = (ViewGraph *)classDir->FindInstance(argv[1]);
        if (!view) {
          ReturnVal(API_NAK, "Cannot find view");
          return -1;
        }

		Boolean enabled;
		char *countAttr;
		char *putAttr;
		view->GetCountMapping(enabled, countAttr, putAttr);

		char buf[128];
		sprintf(buf, "%d {%s} {%s}", enabled, countAttr, putAttr);
        ReturnVal(API_ACK, buf);
	    return 1;
	} else {
		fprintf(stderr,"Wrong # of arguments: %d in getCountMapping\n", argc);
    	ReturnVal(API_NAK, "Wrong # of arguments");
    	return -1;
	}
IMPLEMENT_COMMAND_END

IMPLEMENT_COMMAND_BEGIN(setCountMapping)
    // Arguments: <view name> <enabled> <count attr> <put attr>
    // Returns: "done"
#if defined(DEBUG)
    PrintArgs(stdout, argc, argv);
#endif

    if (argc == 5) {
        ViewGraph *view = (ViewGraph *)classDir->FindInstance(argv[1]);
        if (!view) {
          ReturnVal(API_NAK, "Cannot find view");
          return -1;
        }

		Boolean enabled = atoi(argv[2]);
		if (view->SetCountMapping(enabled, argv[3], argv[4]).IsComplete()) {
          ReturnVal(API_ACK, "done");
	      return 1;
		} else {
    	  ReturnVal(API_NAK, "Error setting count mapping");
    	  return -1;
		}
	} else {
		fprintf(stderr,"Wrong # of arguments: %d in setCountMapping\n", argc);
    	ReturnVal(API_NAK, "Wrong # of arguments");
    	return -1;
	}
IMPLEMENT_COMMAND_END

IMPLEMENT_COMMAND_BEGIN(getCursorType)
    // Arguments: <cursor name>
    // Returns: <cursor type>
#if defined(DEBUG)
    PrintArgs(stdout, argc, argv);
#endif

    if (argc == 2) {
        DeviseCursor *cursor = (DeviseCursor *)classDir->FindInstance(argv[1]);
        if (!cursor) {
        	ReturnVal(API_NAK, "Cannot find cursor");
        	return -1;
		}

		VisualFlag flag = cursor->GetFlag();

		char buf[128];
		if (flag == VISUAL_X) {
			sprintf(buf, "VisualX");
		} else if (flag == VISUAL_Y) {
			sprintf(buf, "VisualY");
		} else if (flag == (VISUAL_X | VISUAL_Y)) {
			sprintf(buf, "VisualXY");
		} else {
			sprintf(buf, "unknown");
		}

        ReturnVal(API_ACK, buf);
	    return 1;
	} else {
		fprintf(stderr,"Wrong # of arguments: %d in getCursorType\n", argc);
    	ReturnVal(API_NAK, "Wrong # of arguments");
    	return -1;
	}
IMPLEMENT_COMMAND_END

IMPLEMENT_COMMAND_BEGIN(viewGoHome)
    // Arguments: <view name>
    // Returns: "done"
#if defined(DEBUG)
    PrintArgs(stdout, argc, argv);
#endif

    if (argc == 2) {
        ViewGraph *view = (ViewGraph *)classDir->FindInstance(argv[1]);
        if (!view) {
          ReturnVal(API_NAK, "Cannot find view");
          return -1;
        }

		view->GoHome();
        ReturnVal(API_ACK, "done");
	    return 1;
	} else {
		fprintf(stderr,"Wrong # of arguments: %d in viewGoHome\n", argc);
    	ReturnVal(API_NAK, "Wrong # of arguments");
    	return -1;
	}
IMPLEMENT_COMMAND_END

IMPLEMENT_COMMAND_BEGIN(writeRangeDesc)
    // Arguments: <file name>
    // Returns: "done"
#if defined(DEBUG)
    PrintArgs(stdout, argc, argv);
#endif

    if (argc == 2) {
		if (RangeDesc::Dump(argv[1]).IsComplete()) {
#if 0 // Check for memory leaks.
        printf("%s: %d; end of data seg = 0x%p\n", __FILE__, __LINE__, sbrk(0));
		for (int count = 0; count < 1000; count++) {
		  (void) RangeDesc::Dump(argv[1]);
		}
        printf("%s: %d; end of data seg = 0x%p\n", __FILE__, __LINE__, sbrk(0));
#endif
        	ReturnVal(API_ACK, "done");
			return 1;
		} else {
            ReturnVal(API_NAK, "Error dumping range description");
            return -1;
		}
	} else {
		fprintf(stderr,"Wrong # of arguments: %d in writeRangeDesc\n", argc);
    	ReturnVal(API_NAK, "Wrong # of arguments");
    	return -1;
	}
IMPLEMENT_COMMAND_END

TDataMap::TableType
Name2StringTableType(char *name)
{
  TDataMap::TableType type = TDataMap::TableInvalid;
  if (!strcmp(name, "x")) {
    type = TDataMap::TableX;
  } else if (!strcmp(name, "y")) {
    type = TDataMap::TableY;
  } else if (!strcmp(name, "z")) {
    type = TDataMap::TableZ;
  } else if (!strcmp(name, "gen")) {
    type = TDataMap::TableGen;
  }

  return type;
}

IMPLEMENT_COMMAND_BEGIN(viewSetStringTable)
    // Arguments: <view name> <table type> <table name>
	//   (Table type is "x", "y", "z", or "gen".)
    // Returns: "done"
#if defined(DEBUG)
    PrintArgs(stdout, argc, argv);
#endif

    if (argc == 4) {
        ViewGraph *view = (ViewGraph *)classDir->FindInstance(argv[1]);
        if (!view) {
          ReturnVal(API_NAK, "Cannot find view");
          return -1;
        }

		TDataMap::TableType type = Name2StringTableType(argv[2]);
		if (type == TDataMap::TableInvalid) {
          ReturnVal(API_NAK, "Invalid table type");
	      return -1;
		}
		char *tableName = argv[3];
		if (!strcmp(tableName, "")) tableName = NULL;
		view->SetStringTable(type, tableName);
        ReturnVal(API_ACK, "done");
	    return 1;
	} else {
		fprintf(stderr,"Wrong # of arguments: %d in viewSetStringTable\n",
		  argc);
    	ReturnVal(API_NAK, "Wrong # of arguments");
    	return -1;
	}
IMPLEMENT_COMMAND_END

IMPLEMENT_COMMAND_BEGIN(viewGetStringTable)
    // Arguments: <view name> <table type>
	//   (Table type is "x", "y", "z", or "gen".)
    // Returns: <table name>
#if defined(DEBUG)
    PrintArgs(stdout, argc, argv);
#endif

    if (argc == 3) {
        ViewGraph *view = (ViewGraph *)classDir->FindInstance(argv[1]);
        if (!view) {
          ReturnVal(API_NAK, "Cannot find view");
          return -1;
        }

		TDataMap::TableType type = Name2StringTableType(argv[2]);
		if (type == TDataMap::TableInvalid) {
          ReturnVal(API_NAK, "Invalid table type");
	      return -1;
		}
		char *tableName = view->GetStringTable(type);
		if (tableName == NULL) tableName = "";
        ReturnVal(API_ACK, tableName);
	    return 1;
	} else {
		fprintf(stderr,"Wrong # of arguments: %d in viewGetStringTable\n",
		  argc);
    	ReturnVal(API_NAK, "Wrong # of arguments");
    	return -1;
	}
IMPLEMENT_COMMAND_END

IMPLEMENT_COMMAND_BEGIN(viewSetIsHighlight)
    // Arguments: <view name> <is highlight>
    // Returns: "done"
#if defined(DEBUG)
    PrintArgs(stdout, argc, argv);
#endif

    if (argc == 3) {
        ViewGraph *view = (ViewGraph *)classDir->FindInstance(argv[1]);
        if (!view) {
          ReturnVal(API_NAK, "Cannot find view");
          return -1;
        }
		view->SetHighlightView(atoi(argv[2]));

        ReturnVal(API_ACK, "done");
	    return 1;
	} else {
		fprintf(stderr,"Wrong # of arguments: %d in viewSetIsHighlight\n",
		  argc);
    	ReturnVal(API_NAK, "Wrong # of arguments");
    	return -1;
	}
IMPLEMENT_COMMAND_END

IMPLEMENT_COMMAND_BEGIN(viewGetIsHighlight)
    // Arguments: <view name>
    // Returns: <is highlight>
#if defined(DEBUG)
    PrintArgs(stdout, argc, argv);
#endif

    if (argc == 2) {
        ViewGraph *view = (ViewGraph *)classDir->FindInstance(argv[1]);
        if (!view) {
          ReturnVal(API_NAK, "Cannot find view");
          return -1;
        }
		char buf[128];
		sprintf(buf, "%d", view->IsHighlightView());
        ReturnVal(API_ACK, buf);
	    return 1;
	} else {
		fprintf(stderr,"Wrong # of arguments: %d in viewGetIsHighlight\n",
		  argc);
    	ReturnVal(API_NAK, "Wrong # of arguments");
    	return -1;
	}
IMPLEMENT_COMMAND_END


IMPLEMENT_COMMAND_BEGIN(getXAxisDateFormat)
    // Arguments: <view name>
    // Returns: <date format (cftime() format string)>
#if defined(DEBUG)
    PrintArgs(stdout, argc, argv);
#endif

    if (argc == 2) {
        ViewGraph *view = (ViewGraph *)classDir->FindInstance(argv[1]);
        if (!view) {
          ReturnVal(API_NAK, "Cannot find view");
          return -1;
        }
		const char *format = view->GetXAxisDateFormat();
		if (format == NULL) format = "";
        ReturnVal(API_ACK, (char *)format);
	    return 1;
	} else {
		fprintf(stderr,"Wrong # of arguments: %d in getXAxisDateFormat\n",
		  argc);
    	ReturnVal(API_NAK, "Wrong # of arguments");
    	return -1;
	}
IMPLEMENT_COMMAND_END

IMPLEMENT_COMMAND_BEGIN(getYAxisDateFormat)
    // Arguments: <view name>
    // Returns: <date format (cftime() format string)>
#if defined(DEBUG)
    PrintArgs(stdout, argc, argv);
#endif

    if (argc == 2) {
        ViewGraph *view = (ViewGraph *)classDir->FindInstance(argv[1]);
        if (!view) {
          ReturnVal(API_NAK, "Cannot find view");
          return -1;
        }
		const char *format = view->GetYAxisDateFormat();
		if (format == NULL) format = "";
        ReturnVal(API_ACK, (char *)format);
	    return 1;
	} else {
		fprintf(stderr,"Wrong # of arguments: %d in getYAxisDateFormat\n",
		  argc);
    	ReturnVal(API_NAK, "Wrong # of arguments");
    	return -1;
	}
IMPLEMENT_COMMAND_END

IMPLEMENT_COMMAND_BEGIN(setXAxisDateFormat)
    // Arguments: <view name> <date format (cftime() format string)>
    // Returns: "done"
#if defined(DEBUG)
    PrintArgs(stdout, argc, argv);
#endif

    if (argc == 3) {
        ViewGraph *view = (ViewGraph *)classDir->FindInstance(argv[1]);
        if (!view) {
          ReturnVal(API_NAK, "Cannot find view");
          return -1;
        }
		view->SetXAxisDateFormat(argv[2]);
        ReturnVal(API_ACK, "done");
	    return 1;
	} else {
		fprintf(stderr,"Wrong # of arguments: %d in setXAxisDateFormat\n",
		  argc);
    	ReturnVal(API_NAK, "Wrong # of arguments");
    	return -1;
	}
IMPLEMENT_COMMAND_END

IMPLEMENT_COMMAND_BEGIN(setYAxisDateFormat)
    // Arguments: <view name> <date format (cftime() format string)>
    // Returns: "done"
#if defined(DEBUG)
    PrintArgs(stdout, argc, argv);
#endif

    if (argc == 3) {
        ViewGraph *view = (ViewGraph *)classDir->FindInstance(argv[1]);
        if (!view) {
          ReturnVal(API_NAK, "Cannot find view");
          return -1;
        }
		view->SetYAxisDateFormat(argv[2]);
        ReturnVal(API_ACK, "done");
	    return 1;
	} else {
		fprintf(stderr,"Wrong # of arguments: %d in setYAxisDateFormat\n",
		  argc);
    	ReturnVal(API_NAK, "Wrong # of arguments");
    	return -1;
	}
IMPLEMENT_COMMAND_END

IMPLEMENT_COMMAND_BEGIN(updateAxisTypes)
    // Arguments: <view name>
    // Returns: "done"
#if defined(DEBUG)
    PrintArgs(stdout, argc, argv);
#endif

    if (argc == 2) {
        ViewGraph *view = (ViewGraph *)classDir->FindInstance(argv[1]);
        if (!view) {
          ReturnVal(API_NAK, "Cannot find view");
          return -1;
        }
		view->UpdateAxisTypes();
        ReturnVal(API_ACK, "done");
	    return 1;
	} else {
		fprintf(stderr,"Wrong # of arguments: %d in updateAxisTypes\n",
		  argc);
    	ReturnVal(API_NAK, "Wrong # of arguments");
    	return -1;
	}
IMPLEMENT_COMMAND_END

IMPLEMENT_COMMAND_BEGIN(setViewGeometry)
    // Arguments: <view name> <x> <y> <width> <height>
	//   (x, y, width, and height are expressed as a fraction of the
	//   window size)
    // Returns: "done"
#if defined(DEBUG)
    PrintArgs(stdout, argc, argv);
#endif

    if (argc == 6) {
        ViewGraph *view = (ViewGraph *)classDir->FindInstance(argv[1]);
        if (!view) {
          ReturnVal(API_NAK, "Cannot find view");
          return -1;
        }
		int winX, winY;
		unsigned winWidth, winHeight;
		Layout *win = (Layout *)(view->GetParent());
		win->Geometry(winX, winY, winWidth, winHeight);

		int viewX = (int)(atof(argv[2]) * winWidth);
		int viewY = (int)(atof(argv[3]) * winHeight);
		int viewWidth = (int)(atof(argv[4]) * winWidth);
		int viewHeight = (int)(atof(argv[5]) * winHeight);

		view->MoveResize(viewX, viewY, viewWidth, viewHeight);
		view->SetGeometry(viewX, viewY, viewWidth, viewHeight);
		win->SetLayoutProperties(CUSTOM, 0, 0);
        ReturnVal(API_ACK, "done");
	    return 1;
	} else {
		fprintf(stderr,"Wrong # of arguments: %d in setViewGeom\n",
		  argc);
    	ReturnVal(API_NAK, "Wrong # of arguments");
    	return -1;
	}
IMPLEMENT_COMMAND_END

IMPLEMENT_COMMAND_BEGIN(setPileStackState)
    // Arguments: <window name> <pile/stack state>
    //   see PileStack.h for state enum
    // Returns: "done"
#if defined(DEBUG)
    PrintArgs(stdout, argc, argv);
#endif

    if (argc == 3) {
        ViewWin *window = (ViewWin *)classDir->FindInstance(argv[1]);
        if (!window) {
          ReturnVal(API_NAK, "Cannot find window");
          return -1;
        }
		PileStack *ps = window->GetPileStack();
		if (!ps) {
          ReturnVal(API_NAK, "Cannot find pile/stack object");
          return -1;
		}
		ps->SetState((PileStack::State)atoi(argv[2]));
        ReturnVal(API_ACK, "done");
	    return 1;
	} else {
		fprintf(stderr,"Wrong # of arguments: %d in setPileStackState\n",
		  argc);
    	ReturnVal(API_NAK, "Wrong # of arguments");
    	return -1;
	}
IMPLEMENT_COMMAND_END

IMPLEMENT_COMMAND_BEGIN(getPileStackState)
    // Arguments: <window name>
    // Returns: <pile/stack state> (see PileStack.h for state enum)
#if defined(DEBUG)
    PrintArgs(stdout, argc, argv);
#endif

    if (argc == 2) {
        ViewWin *window = (ViewWin *)classDir->FindInstance(argv[1]);
        if (!window) {
          ReturnVal(API_NAK, "Cannot find window");
          return -1;
        }
		PileStack *ps = window->GetPileStack();
		if (!ps) {
          ReturnVal(API_NAK, "Cannot find pile/stack object");
          return -1;
		}
		char buf[128];
		sprintf(buf, "%d", ps->GetState());
        ReturnVal(API_ACK, buf);
	    return 1;
	} else {
		fprintf(stderr,"Wrong # of arguments: %d in getPileStackState\n",
		  argc);
    	ReturnVal(API_NAK, "Wrong # of arguments");
    	return -1;
	}
IMPLEMENT_COMMAND_END

IMPLEMENT_COMMAND_BEGIN(flipPileStack)
    // Arguments: <window name>
    // Returns: "done"
#if defined(DEBUG)
    PrintArgs(stdout, argc, argv);
#endif

    if (argc == 2) {
        ViewWin *window = (ViewWin *)classDir->FindInstance(argv[1]);
        if (!window) {
          ReturnVal(API_NAK, "Cannot find window");
          return -1;
        }
		PileStack *ps = window->GetPileStack();
		if (!ps) {
          ReturnVal(API_NAK, "Cannot find pile/stack object");
          return -1;
		}
		ps->Flip();
        ReturnVal(API_ACK, "done");
	    return 1;
	} else {
		fprintf(stderr,"Wrong # of arguments: %d in flipPileStack\n",
		  argc);
    	ReturnVal(API_NAK, "Wrong # of arguments");
    	return -1;
	}
IMPLEMENT_COMMAND_END

IMPLEMENT_COMMAND_BEGIN(groupUngroupViews)
    // Arguments: none
    // Returns: "done"
#if defined(DEBUG)
    PrintArgs(stdout, argc, argv);
#endif
    if (argc == 1) {
		ViewGeom *viewGeom = ViewGeom::GetViewGeom();
		if (viewGeom->IsGrouped()) {
		  ViewGeom::GetViewGeom()->Ungroup();
		} else {
		  ViewGeom::GetViewGeom()->Group();
		}
		
       	ReturnVal(API_ACK, "done");
		return 1;
	} else {
		fprintf(stderr,"Wrong # of arguments: %d in groupUngroupViews\n", argc);
    	ReturnVal(API_NAK, "Wrong # of arguments");
    	return -1;
	}
IMPLEMENT_COMMAND_END

IMPLEMENT_COMMAND_BEGIN(updateSession)
    // Arguments: <session file>
    // Returns: "done"
#if defined(DEBUG)
    PrintArgs(stdout, argc, argv);
#endif
    if (argc == 2) {
		if (!Session::Update(argv[1]).IsComplete()) {
    	  ReturnVal(API_NAK, "Unable to update session");
    	  return -1;
		}
		
       	ReturnVal(API_ACK, "done");
		return 1;
	} else {
		fprintf(stderr,"Wrong # of arguments: %d in updateSession\n", argc);
    	ReturnVal(API_NAK, "Wrong # of arguments");
    	return -1;
	}
IMPLEMENT_COMMAND_END

IMPLEMENT_COMMAND_BEGIN(getViewAutoFilter)
    // Arguments: <view name>
    // Returns: <auto filter enabled>
#if defined(DEBUG)
    PrintArgs(stdout, argc, argv);
#endif

    if (argc == 2) {
        ViewGraph *view = (ViewGraph *)classDir->FindInstance(argv[1]);
        if (!view) {
          ReturnVal(API_NAK, "Cannot find view");
          return -1;
        }
		char buf[128];
		sprintf(buf, "%d", view->AutoUpdateFilter());
        ReturnVal(API_ACK, buf);
	    return 1;
	} else {
		fprintf(stderr,"Wrong # of arguments: %d in getViewAutoFilter\n",
		  argc);
    	ReturnVal(API_NAK, "Wrong # of arguments");
    	return -1;
	}
IMPLEMENT_COMMAND_END

IMPLEMENT_COMMAND_BEGIN(setViewAutoFilter)
    // Arguments: <view name> <auto filter enabled>
    // Returns: "done"
#if defined(DEBUG)
    PrintArgs(stdout, argc, argv);
#endif

    if (argc == 3) {
        ViewGraph *view = (ViewGraph *)classDir->FindInstance(argv[1]);
        if (!view) {
          ReturnVal(API_NAK, "Cannot find view");
          return -1;
        }
		view->SetAutoUpdate(atoi(argv[2]));
        ReturnVal(API_ACK, "done");
	    return 1;
	} else {
		fprintf(stderr,"Wrong # of arguments: %d in setViewAutoFilter\n",
		  argc);
    	ReturnVal(API_NAK, "Wrong # of arguments");
    	return -1;
	}
IMPLEMENT_COMMAND_END

IMPLEMENT_COMMAND_BEGIN(updateFilters)
    // Arguments: none
    // Returns: "done"
#if defined(DEBUG)
    PrintArgs(stdout, argc, argv);
#endif
    if (argc == 1) {
		if (!Session::UpdateFilters().IsComplete()) {
    	  ReturnVal(API_NAK, "Unable to update filters");
    	  return -1;
		}
		
       	ReturnVal(API_ACK, "done");
		return 1;
	} else {
		fprintf(stderr,"Wrong # of arguments: %d in updateFilters\n", argc);
    	ReturnVal(API_NAK, "Wrong # of arguments");
    	return -1;
	}
IMPLEMENT_COMMAND_END

IMPLEMENT_COMMAND_BEGIN(parseDRSchema)
    // Arguments: <schema file name>
    // Returns: <schema in Tcl list>
#if defined(DEBUG)
    PrintArgs(stdout, argc, argv);
#endif
    if (argc == 2) {
		char *list = DRSchema2TclList(argv[1]);
		if (list == NULL) {
          ReturnVal(API_NAK, "Unable to parse schema file");
    	  return -1;
		}

       	ReturnVal(API_ACK, list);
		delete list;
		return 1;
	} else {
		fprintf(stderr,"Wrong # of arguments: %d in parseDRSchema\n", argc);
    	ReturnVal(API_NAK, "Wrong # of arguments");
    	return -1;
	}
IMPLEMENT_COMMAND_END

IMPLEMENT_COMMAND_BEGIN(getDupElim)
    // Arguments: <view name>
    // Returns: <duplicate elmination enabled (Boolean)>
#if defined(DEBUG)
    PrintArgs(stdout, argc, argv);
#endif
    if (argc == 2) {
        ViewGraph *view = (ViewGraph *)classDir->FindInstance(argv[1]);
		if (view == NULL) {
          ReturnVal(API_NAK, "Cannot find view");
    	  return -1;
		}

		char buf[256];
		sprintf(buf, "%d", view->GetDupElim());
       	ReturnVal(API_ACK, buf);
		return 1;
	} else {
		fprintf(stderr,"Wrong # of arguments: %d in getDupElim\n", argc);
    	ReturnVal(API_NAK, "Wrong # of arguments");
    	return -1;
	}
IMPLEMENT_COMMAND_END

IMPLEMENT_COMMAND_BEGIN(setDupElim)
    // Arguments: <view name> <enable duplicate elimination (Boolean)>
    // Returns: "done"
#if defined(DEBUG)
    PrintArgs(stdout, argc, argv);
#endif
    if (argc == 3) {
        ViewGraph *view = (ViewGraph *)classDir->FindInstance(argv[1]);
		if (view == NULL) {
          ReturnVal(API_NAK, "Cannot find view");
    	  return -1;
		}

		Boolean enable = atoi(argv[2]);
		view->SetDupElim(enable);
       	ReturnVal(API_ACK, "done");
		return 1;
	} else {
		fprintf(stderr,"Wrong # of arguments: %d in setDupElim\n", argc);
    	ReturnVal(API_NAK, "Wrong # of arguments");
    	return -1;
	}
IMPLEMENT_COMMAND_END

IMPLEMENT_COMMAND_BEGIN(niceifyAxes)
    // Arguments: <view name> <niceify X> <niceify Y>
    // Returns: "done"
#if defined(DEBUG)
    PrintArgs(stdout, argc, argv);
#endif
    if (argc == 4) {
        ViewGraph *view = (ViewGraph *)classDir->FindInstance(argv[1]);
		if (view == NULL) {
          ReturnVal(API_NAK, "Cannot find view");
    	  return -1;
		}

		Boolean doX = atoi(argv[2]);
		Boolean doY = atoi(argv[3]);
		view->NiceifyAxes(doX, doY);
       	ReturnVal(API_ACK, "done");
		return 1;
	} else {
		fprintf(stderr,"Wrong # of arguments: %d in niceifyAxes\n", argc);
    	ReturnVal(API_NAK, "Wrong # of arguments");
    	return -1;
	}
IMPLEMENT_COMMAND_END

IMPLEMENT_COMMAND_BEGIN(getNiceAxes)
    // Arguments: <view name>
    // Returns: <nice X> <nice Y>
#if defined(DEBUG)
    PrintArgs(stdout, argc, argv);
#endif
    if (argc == 2) {
        ViewGraph *view = (ViewGraph *)classDir->FindInstance(argv[1]);
		if (view == NULL) {
          ReturnVal(API_NAK, "Cannot find view");
    	  return -1;
		}

		Boolean niceX, niceY;
		view->GetNiceAxes(niceX, niceY);

		char buf[128];
		sprintf(buf, "%d %d", niceX, niceY);
       	ReturnVal(API_ACK, buf);
		return 1;
	} else {
		fprintf(stderr,"Wrong # of arguments: %d in getNiceAxes\n", argc);
    	ReturnVal(API_NAK, "Wrong # of arguments");
    	return -1;
	}
IMPLEMENT_COMMAND_END

IMPLEMENT_COMMAND_BEGIN(setNiceAxes)
    // Arguments: <view name> <nice x> <nice y>
    // Returns: "done"
#if defined(DEBUG)
    PrintArgs(stdout, argc, argv);
#endif
    if (argc == 4) {
        ViewGraph *view = (ViewGraph *)classDir->FindInstance(argv[1]);
		if (view == NULL) {
          ReturnVal(API_NAK, "Cannot find view");
    	  return -1;
		}

		Boolean niceX = atoi(argv[2]);
		Boolean niceY = atoi(argv[3]);
		view->SetNiceAxes(niceX, niceY);
       	ReturnVal(API_ACK, "done");
		return 1;
	} else {
		fprintf(stderr,"Wrong # of arguments: %d in setNiceAxes\n", argc);
    	ReturnVal(API_NAK, "Wrong # of arguments");
    	return -1;
	}
IMPLEMENT_COMMAND_END

/*============================================================================*/

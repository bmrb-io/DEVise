/*
  ========================================================================
  DEVise Data Visualization Software
  (c) Copyright 1998
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
  Revision 1.8  1998/04/14 21:03:13  wenger
  TData attribute links (aka set links) are working except for actually
  creating the join table, and some cleanup when unlinking, etc.

 */

#include <stdio.h>
#include <ctype.h>
#include <assert.h>

#include "DeviseCommand.h"
#include "ParseAPI.h"
#include "TDataDQLInterp.h"
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
#include "ViewLens.h"
#include "WinClassInfo.h"
#include "VisualLinkClassInfo.h"
#include "CursorClassInfo.h"
#include "MappingInterp.h"
#include "QueryProc.h"

#include "LMControl.h"		// LayoutManager

#include "CatalogComm.h"
#include "SessionDesc.h"
#include "StringStorage.h"
#include "DepMgr.h"
#include "Session.h"
#include "GDataSock.h"
#include "Timer.h"

#include "Color.h"
//#define INLINE_TRACE
#include "debug.h"
#define LINESIZE 1024

#define IMPLEMENT_COMMAND_BEGIN(command) \
int DeviseCommand_##command::Run(int argc, char** argv)\
{\
	int ret_value;\
	ret_value=DeviseCommand::Run(argc, argv);\
	if (ret_value)\
	{
#define IMPLEMENT_COMMAND_END \
	}\
	return ret_value;\
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
int ParseAPIDTE(int argc, char** argv, ControlPanel* control);
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
void
DeviseCommand::setDefaultControl(ControlPanel* defaultCntl)
{
	defaultControl = defaultCntl;
}

int 
DeviseCommand::Run(int argc, char** argv, ControlPanel* cntl)
{
	// reset the control each time you run a command
	int	retval;

	control = cntl;
	pushControl(cntl);
	retval = Run(argc, argv);

	// restore the orignial value to control
	popControl();
	return retval;
}

int 
DeviseCommand::Run(int argc, char** argv)
{
	classDir = control->GetClassDir();
	result[0] = '\0';
#if defined(DEBUG)
	printf("ParseAPI[%ld]: ", (long) getpid());
	for (int i = 0; i < argc; i++)
  	{
		printf("\"%s\" ", argv[i]);
	}
	printf("\n");
#endif
	return (int) true;
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
//**********************************************************************
// DTE Command objects
//**********************************************************************

/*
	// alternatively, we can write this, instead of the Macro
int
DeviseCommand_dteImportFileType::Run(int argc, char** argv)
{
    int ret_value;
    ret_value=DeviseCommand::Run(argc, argv);
    if (ret_value)
    {
*/

IMPLEMENT_COMMAND_BEGIN(dteImportFileType)
		if (argc ==2) 
        {
    		 char * name = dteImportFileType(argv[1]);
    		 if (!name){
    		control->ReturnVal(API_NAK, strdup(""));
    		return -1;
    		 }
    		 control->ReturnVal(API_ACK, name);
    		 return 1;
    	}
		return ParseAPIDTE(argc, argv, control);
IMPLEMENT_COMMAND_END

/*
	// alternatively, we can write this, instead of the Macro
    }
    return ret_value;
}
*/

int
DeviseCommand_dteListAllIndexes::Run(int argc, char** argv)
{
    int ret_value;
    ret_value=DeviseCommand::Run(argc, argv);
    if (ret_value)
    {
/*TAG
        {
    			char* retVal = dteListAllIndexes(argv[1]);
    			control->ReturnVal(API_ACK, retVal);
    			return 1;
    		}
TAG*/
        return ParseAPIDTE(argc, argv, control);
    }
    return ret_value;
}
int
DeviseCommand_dteDeleteCatalogEntry::Run(int argc, char** argv)
{
    int ret_value;
    ret_value=DeviseCommand::Run(argc, argv);
    if (ret_value)
    {
/*TAG
        {
          dteDeleteCatalogEntry(argv[1]);
          control->ReturnVal(API_ACK, "");
          return 1;
        }
TAG*/
        return ParseAPIDTE(argc, argv, control);
    }
    return ret_value;
}
int
DeviseCommand_dteMaterializeCatalogEntry::Run(int argc, char** argv)
{
    int ret_value;
    ret_value=DeviseCommand::Run(argc, argv);
    if (ret_value)
    {
/*TAG
        {
          dteMaterializeCatalogEntry(argv[1]);
          control->ReturnVal(API_ACK, "");
          return 1;
        }
TAG*/
        return ParseAPIDTE(argc, argv, control);
    }
    return ret_value;
}
int
DeviseCommand_dteReadSQLFilter::Run(int argc, char** argv)
{
    int ret_value;
    ret_value=DeviseCommand::Run(argc, argv);
    if (ret_value)
    {
/*TAG
        {
          char* retVal = dteReadSQLFilter(argv[1]);
          control->ReturnVal(API_ACK, retVal);
          return 1;
        }
TAG*/
        return ParseAPIDTE(argc, argv, control);
    }
    return ret_value;
}
int
DeviseCommand_dteShowCatalogEntry::Run(int argc, char** argv)
{
    int ret_value;
    ret_value=DeviseCommand::Run(argc, argv);
    if (ret_value)
    {
/*TAG
        {
    
    	// TEMPTEMP -- Kent, I traced the type 11 error to here...
    
    		char* catEntry = dteShowCatalogEntry(argv[1]);
    		CATCH(
    			string err = currExcept->toString();
    			control->ReturnVal(API_NAK, (char*) err.c_str());
    			return -1;
    		)
    		control->ReturnVal(API_ACK, catEntry);
    		delete [] catEntry;
    		return 1;
    	}
TAG*/
        return ParseAPIDTE(argc, argv, control);
    }
    return ret_value;
}
int
DeviseCommand_dteListCatalog::Run(int argc, char** argv)
{
    int ret_value;
    ret_value=DeviseCommand::Run(argc, argv);
    if (ret_value)
    {
/*TAG
        {
    		int errorCode;	// 0 means OK
    		char* catListing = dteListCatalog(argv[1], errorCode);
    		if(errorCode){
    			control->ReturnVal(API_NAK, catListing);
    			delete [] catListing;
    			return -1;
    		}
    		else{
    			control->ReturnVal(API_ACK, catListing);
    			delete [] catListing;
    			return 1;
    		}
    	}
TAG*/
        return ParseAPIDTE(argc, argv, control);
    }
    return ret_value;
}
int
DeviseCommand_dteListQueryAttributes::Run(int argc, char** argv)
{
    int ret_value;
    ret_value=DeviseCommand::Run(argc, argv);
    if (ret_value)
    {
/*TAG
        {
    		char* attrListing = dteListQueryAttributes(argv[1]);
    		CATCH(
    			string err = "Failed to find available attributes.\n";
    			err += currExcept->toString();
    			control->ReturnVal(API_NAK, (char*) err.c_str());
    			return -1;
    		)
    		control->ReturnVal(API_ACK, attrListing);
    		delete [] attrListing;
    		return 1;
    	}
TAG*/
        return ParseAPIDTE(argc, argv, control);
    }
    return ret_value;
}
int
DeviseCommand_dteListAttributes::Run(int argc, char** argv)
{
    int ret_value;
    ret_value=DeviseCommand::Run(argc, argv);
    if (ret_value)
    {
/*TAG
        {
          char* attrListing = dteListAttributes(argv[1]);
          control->ReturnVal(API_ACK, attrListing);
          return 1;
        }
TAG*/
        return ParseAPIDTE(argc, argv, control);
    }
    return ret_value;
}
int
DeviseCommand_dteDeleteIndex::Run(int argc, char** argv)
{
    int ret_value;
    ret_value=DeviseCommand::Run(argc, argv);
    if (ret_value)
    {
/*TAG
        {
          dteDeleteIndex(argv[1], argv[2]);
          control->ReturnVal(API_ACK, "");
          return 1;
        }
TAG*/
        return ParseAPIDTE(argc, argv, control);
    }
    return ret_value;
}
int
DeviseCommand_dteShowIndexDesc::Run(int argc, char** argv)
{
    int ret_value;
    ret_value=DeviseCommand::Run(argc, argv);
    if (ret_value)
    {
/*TAG
        {
          char* indexDesc = dteShowIndexDesc(argv[1], argv[2]);
          control->ReturnVal(API_ACK, indexDesc);
          return 1;
        }
TAG*/
        return ParseAPIDTE(argc, argv, control);
    }
    return ret_value;
}
int
DeviseCommand_dteShowAttrNames::Run(int argc, char** argv)
{
    int ret_value;
    ret_value=DeviseCommand::Run(argc, argv);
    if (ret_value)
    {
/*TAG
        {
          char* attrListing = dteShowAttrNames(argv[1], argv[2]);
          control->ReturnVal(API_ACK, attrListing);
          return 1;
        }
TAG*/
        return ParseAPIDTE(argc, argv, control);
    }
    return ret_value;
}
int
DeviseCommand_dteInsertCatalogEntry::Run(int argc, char** argv)
{
    int ret_value;
    ret_value=DeviseCommand::Run(argc, argv);
    if (ret_value)
    {
/*TAG
        {
          dteInsertCatalogEntry(argv[1], argv[2]);
          control->ReturnVal(API_ACK, "");
    	 return 1;
        }
TAG*/
        return ParseAPIDTE(argc, argv, control);
    }
    return ret_value;
}
int
DeviseCommand_dteCheckSQLViewEntry::Run(int argc, char** argv)
{
    int ret_value;
    ret_value=DeviseCommand::Run(argc, argv);
    if (ret_value)
    {
/*TAG
        {
    			string errmsg = dteCheckSQLViewEntry(argv[1], argv[2]);
    			if(errmsg.empty()){
    				control->ReturnVal(API_ACK, "");
    			}
    			else{
    				// this cast is OK
    				control->ReturnVal(API_ACK, (char*) errmsg.c_str());
    			}
    			return 1;
    		}
TAG*/
        return ParseAPIDTE(argc, argv, control);
    }
    return ret_value;
}
int
DeviseCommand_dteCreateIndex::Run(int argc, char** argv)
{
    int ret_value;
    ret_value=DeviseCommand::Run(argc, argv);
    if (ret_value)
    {
/*TAG
        {
              dteCreateIndex(argv[1], argv[2], argv[3], argv[4], argv[5]);
              control->ReturnVal(API_ACK, "");
              return 1;
         }
TAG*/
        return ParseAPIDTE(argc, argv, control);
    }
    return ret_value;
}

//**********************************************************************
// User Command objects
//**********************************************************************
int
DeviseCommand_color::Run(int argc, char** argv)
{
    int ret_value;
    ret_value=DeviseCommand::Run(argc, argv);
    if (ret_value)
    {
    		return ParseAPIColorCommands(argc, argv, control);
    }
    return ret_value;
}
int
DeviseCommand_getAllViews::Run(int argc, char** argv)
{
    int ret_value;
    ret_value=DeviseCommand::Run(argc, argv);
    if (ret_value)
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
    
    		control->ReturnVal(API_ACK, result);
    		return 1;
    	}
    }
    return ret_value;
}
#if 0 // Why the hell do we have two *different* versions of the same
	  // command?!?  RKW Feb. 3, 1998.
int
DeviseCommand_getAllViews::Run(int argc, char** argv)
{
    int ret_value;
    ret_value=DeviseCommand::Run(argc, argv);
    if (ret_value)
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
    					control->ReturnVal(API_NAK, "Cannot find view");
    					return -1;
    				}
    
    				strcat(result, "{");
    				strcat(result, view->GetName());
    				strcat(result, "} ");
    			}
    		}
    
    		control->ReturnVal(API_ACK, result);
    		return 1;
    	}
    }
    return ret_value;
}
#endif

int
DeviseCommand_changeParam::Run(int argc, char** argv)
{
    int ret_value;
    ret_value=DeviseCommand::Run(argc, argv);
    if (ret_value)
    {
        {
        classDir->ChangeParams(argv[1], argc - 2, &argv[2]);
        control->ReturnVal(API_ACK, "done");
        return 1;
      }
    }
    return ret_value;
}
int
DeviseCommand_createInterp::Run(int argc, char** argv)
{
    int ret_value;
    ret_value=DeviseCommand::Run(argc, argv);
    if (ret_value)
    {
        {
        /* This command is supported for backward compatibility only */
        MapInterpClassInfo *interp = control->GetInterpProto();
        ClassInfo *classInfo = interp->CreateWithParams(argc - 1, &argv[1]);
        if (!classInfo) {
          control->ReturnVal(API_NAK, "Cannot create mapping");
          return -1;
        }
        ControlPanel::RegisterClass(classInfo, true);
        control->ReturnVal(API_ACK, classInfo->ClassName());
        return 1;
      }
    }
    return ret_value;
}
int
DeviseCommand_create::Run(int argc, char** argv)
{
    int ret_value;
    ret_value=DeviseCommand::Run(argc, argv);
    if (ret_value)
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
        if (!name)
          control->ReturnVal(API_ACK, "");
        else
          control->ReturnVal(API_ACK, name);
        return 1;
      }
    }
    return ret_value;
}
int
DeviseCommand_getTDataName::Run(int argc, char** argv)
{
    int ret_value;
    ret_value=DeviseCommand::Run(argc, argv);
    if (ret_value)
    {
        {
        TData *tdata = (TData *)classDir->FindInstance(argv[1]);
        if (!tdata) {
            control->ReturnVal(API_NAK, "Cannot find TData");
            return -1;
        }
        char *tname = tdata->GetName();
        control->ReturnVal(API_ACK, tname); 
        return 1;
      }
    }
    return ret_value;
}
int
DeviseCommand_showkgraph::Run(int argc, char** argv)
{
    int ret_value;
    ret_value=DeviseCommand::Run(argc, argv);
    if (ret_value)
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
    	control->ReturnVal(API_NAK, "Cannot find view");
    	delete vlist;
    	return -1;
          }
        }
        vkg->Init();
        /* Add these to the ViewKGraph class and display */
        if (vkg->AddViews(vlist, nview, argv[3]) == false) {
          control->ReturnVal(API_NAK, "Could not add views to Kiviat graph");
          delete vlist;
          return -1;
        }
        delete vlist;
        if (!vkg->Display(atoi(argv[2]))) {
          control->ReturnVal(API_NAK, "Could not display Kiviat graph");
          return -1;
        }
        control->ReturnVal(API_ACK, "done");
        return 1;
      }
    }
    return ret_value;
}
int
DeviseCommand_createMappingClass::Run(int argc, char** argv)
{
    int ret_value;
    ret_value=DeviseCommand::Run(argc, argv);
    if (ret_value)
    {
        {
        MapInterpClassInfo *interp = control->GetInterpProto();
        ClassInfo *classInfo = interp->CreateWithParams(argc - 1, &argv[1]);
        if (!classInfo) {
          control->ReturnVal(API_NAK, "Cannot create mapping class");
          return -1;
        }
        ControlPanel::RegisterClass(classInfo, true);
        control->ReturnVal(API_ACK, classInfo->ClassName());
        return 1;
      }
    }
    return ret_value;
}
int
DeviseCommand_setDefault::Run(int argc, char** argv)
{
    int ret_value;
    ret_value=DeviseCommand::Run(argc, argv);
    if (ret_value)
    {
        {
        classDir->SetDefault(argv[1], argv[2], argc - 3, &argv[3]);
        control->ReturnVal(API_ACK, "done");
        return 1;
      }
    }
    return ret_value;
}

int
DeviseCommand_setHistogram::Run(int argc, char** argv)
{
    int ret_value;
    ret_value=DeviseCommand::Run(argc, argv);
    if (ret_value)
    {
        {
        ViewGraph *view = (ViewGraph *)classDir->FindInstance(argv[1]);
        if (!view) {
          control->ReturnVal(API_NAK, "Cannot find view");
          return -1;
        }
        double min = atof(argv[2]);
        double max = atof(argv[3]);
        int buckets = atoi(argv[4]);
        if( min >= max ) {
          control->ReturnVal(API_NAK, "min >= max");
          return -1;
        }
        if( buckets > MAX_HISTOGRAM_BUCKETS ) {
          sprintf(result, "buckets > %d", MAX_HISTOGRAM_BUCKETS);
          control->ReturnVal(API_NAK, result);
          return -1;
        }
        if( buckets < 1 ) {
          control->ReturnVal(API_NAK, "buckets < 1");
          return -1;
        }
        view->SetHistogram(atof(argv[2]), atof(argv[3]), atoi(argv[4]));
        control->ReturnVal(API_ACK, "done");
        return 1;
      }
    }
    return ret_value;
}
int
DeviseCommand_getHistogram::Run(int argc, char** argv)
{
    int ret_value;
    ret_value=DeviseCommand::Run(argc, argv);
    if (ret_value)
    {
        {
        ViewGraph *view = (ViewGraph *)classDir->FindInstance(argv[1]);
        if (!view) {
          control->ReturnVal(API_NAK, "Cannot find view");
          return -1;
        }
        sprintf(result, "%g %g %d", view->GetHistogramMin(),
                view->GetHistogramMax(), view->GetHistogramBuckets());
        control->ReturnVal(API_ACK, result);
        return 1;
      }
    }
    return ret_value;
}

#if defined(KSB_MAYBE_DELETE_THIS_OLD_STATS_STUFF)
int
DeviseCommand_setBuckRefresh::Run(int argc, char** argv)
{
    int ret_value;
    ret_value=DeviseCommand::Run(argc, argv);
    if (ret_value)
    {
        {
        ViewGraph *view = (ViewGraph *)classDir->FindInstance(argv[1]);
        if (!view) {
    	    control->ReturnVal(API_NAK, "Cannot find view");
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
        
        control->ReturnVal(API_ACK, "done");
        return 1;
      }
    }
    return ret_value;
}
#endif

int
DeviseCommand_setHistViewname::Run(int argc, char** argv)
{
    int ret_value;
    ret_value=DeviseCommand::Run(argc, argv);
    if (ret_value)
    {
        {
        ViewGraph *view = (ViewGraph *)classDir->FindInstance(argv[1]);
        if (!view) {
                control->ReturnVal(API_NAK, "Cannot find view");
                return -1;
        }
        char *viewName = new char[strlen(argv[2]) + 1];
        strcpy(viewName, argv[2]);
        view->setHistViewname(viewName);
    
        control->ReturnVal(API_ACK, "done");
        return 1;
      }
    }
    return ret_value;
}
int
DeviseCommand_getHistViewname::Run(int argc, char** argv)
{
    int ret_value;
    ret_value=DeviseCommand::Run(argc, argv);
    if (ret_value)
    {
        {
        ViewGraph *view = (ViewGraph *)classDir->FindInstance(argv[1]);
        if (!view) {
        	control->ReturnVal(API_NAK, "Cannot find view");
    	return -1;
        }
        char *name = NULL;
        name = view->getHistViewname();
    
        control->ReturnVal(API_ACK, name);
        return 1;
      }
    }
    return ret_value;
}
int
DeviseCommand_checkGstat::Run(int argc, char** argv)
{
    int ret_value;
    ret_value=DeviseCommand::Run(argc, argv);
    if (ret_value)
    {
        {
        ViewGraph *view = (ViewGraph *)classDir->FindInstance(argv[1]);
        if (!view) {
    	control->ReturnVal(API_NAK, "Cannot find view");
    	return -1;
        }
        if(view->IsGStatInMem()) {
    	printf("GDataStat is in memory\n");
    	strcpy(result, "1");
        } else {
    	printf("GDataStat is NOT in memory\n");
            strcpy(result, "0");
        }
        control->ReturnVal(API_ACK, result);
    
        return 1;
      }
    }
    return ret_value;
}
int
DeviseCommand_getSourceName::Run(int argc, char** argv)
{
    int ret_value;
    ret_value=DeviseCommand::Run(argc, argv);
    if (ret_value)
    {
        {
       ViewGraph *view = (ViewGraph *)classDir->FindInstance(argv[1]);
        if (!view) {
            control->ReturnVal(API_NAK, "Cannot find view");
            return -1;
        }
        TDataMap *map = view->GetFirstMap();
        TData *tdata = map->GetTData();
        char *sourceName = tdata->GetTableName();
        control->ReturnVal(API_ACK, sourceName); 
    
        return 1;
      }
    }
    return ret_value;
}
int
DeviseCommand_isXDateType::Run(int argc, char** argv)
{
    int ret_value;
    ret_value=DeviseCommand::Run(argc, argv);
    if (ret_value)
    {
        {
        ViewGraph *view = (ViewGraph *)classDir->FindInstance(argv[1]);
        if (!view) {
            control->ReturnVal(API_NAK, "Cannot find view");
            return -1;
        }
        if(view->IsXDateType()) {
    	printf("X type is date\n");
            strcpy(result, "1");
        } else {
    	printf("X type is NOT date\n");
            strcpy(result, "0");
        }
        control->ReturnVal(API_ACK, result);
    
        return 1;
      }
    }
    return ret_value;
}
int
DeviseCommand_isYDateType::Run(int argc, char** argv)
{
    int ret_value;
    ret_value=DeviseCommand::Run(argc, argv);
    if (ret_value)
    {
        {
        ViewGraph *view = (ViewGraph *)classDir->FindInstance(argv[1]);
        if (!view) {
            control->ReturnVal(API_NAK, "Cannot find view");
            return -1;
        }
        if(view->IsYDateType()) {
    //	printf("Y type is date\n");
            strcpy(result, "1");
        } else {
    //	printf("Y type is NOT date\n");
            strcpy(result, "0");
        }
        control->ReturnVal(API_ACK, result);
    
        return 1;
      }
    }
    return ret_value;
}
int
DeviseCommand_mapG2TAttr::Run(int argc, char** argv)
{
    int ret_value;
    ret_value=DeviseCommand::Run(argc, argv);
    if (ret_value)
    {
        {
        ViewGraph *view = (ViewGraph *)classDir->FindInstance(argv[1]);
        if (!view) {
            control->ReturnVal(API_NAK, "Cannot find view");
    	return -1;
        }
        TDataMap *map = view->GetFirstMap();
        if (!map) {
    	control->ReturnVal(API_NAK, "Cannot find Mapping");
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
          	control->ReturnVal(API_NAK, "GAttr type not implemented");
    	return -1;
        }
        if (attr) {
          strcpy(result, attr->name);
        } else {
          strcpy(result, "0");
        }
        control->ReturnVal(API_ACK, result);
        return 1;
      }
    }
    return ret_value;
}
int
DeviseCommand_mapT2GAttr::Run(int argc, char** argv)
{
    int ret_value;
    ret_value=DeviseCommand::Run(argc, argv);
    if (ret_value)
    {
        {
        char *gname = NULL;
        ViewGraph *view = (ViewGraph *)classDir->FindInstance(argv[1]);
        if (!view) {
            control->ReturnVal(API_NAK, "Cannot find view");
            return -1;
        }
        TDataMap *map = view->GetFirstMap();
        if (!map) {
            control->ReturnVal(API_NAK, "Cannot find Mapping");
            return -1;
        }
        printf("argv[2] is %s\n", argv[2]);
        gname = map->MapTAttr2GAttr(argv[2]);
        if (gname == NULL) {
    	strcpy(result, "0");
        } else {
    	strcpy(result, gname);
        }
        control->ReturnVal(API_ACK, result);
        delete gname; 
        return 1;
      }
    }
    return ret_value;
}
int
DeviseCommand_setViewLensParams::Run(int argc, char** argv)
{
    int ret_value;
    ret_value=DeviseCommand::Run(argc, argv);
    if (ret_value)
    {
        {
         // name mode views 
         ViewLens *lens = (ViewLens *)classDir->FindInstance(argv[1]);
         if (!lens) {
           control->ReturnVal(API_NAK, "Cannot find lens");
           return -1;
         }
         lens->SetMode(argv[2]);
         DeviseLink *link = (DeviseLink *)classDir->FindInstance(argv[3]);
         if (!link) {
            control->ReturnVal(API_NAK,"Cannot find link");
            return -1;
         }
         lens->SetLink(link);
    
    	 // Replace constant with a heuristically-chosen color?
         lens->SetBackground(GetPColorID(lensBackColor));
    
         int index = lens->InitViewLensIterator();
         for (int j = 4;  j < argc ; j++) {
             ViewGraph *v = (ViewGraph *)classDir->FindInstance(argv[j]);
             if (!v) {
                control->ReturnVal(API_NAK, "Cannot find view");
                return -1;
             }
             printf("Inserting view %s\n", v->GetName());
             if (lens->MoreViewsInLens(index)) {
                ViewInfo *vinfo = lens->NextViewInfoInLens(index);
                lens->ReplaceView(vinfo,v);
             } else {
                lens->InsertView(v);
             }
         }
         // delete extra views 
         index = lens->DeleteViewListTail(index);
         lens->DoneViewLensIterator(index);
         control->ReturnVal(API_ACK, "done");
         return 1;
      }
    }
    return ret_value;
}
int
DeviseCommand_startLayoutManager::Run(int argc, char** argv)
{
    int ret_value = 0;
#if 0
    ret_value=DeviseCommand::Run(argc, argv);
    if (ret_value)
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
    return ret_value;
}
int
DeviseCommand_date::Run(int argc, char** argv)
{
    int ret_value;
    ret_value=DeviseCommand::Run(argc, argv);
    if (ret_value)
    {
        {
          time_t tm = time((time_t *)0);
          control->ReturnVal(API_ACK, DateString(tm));
          return 1;
        }
    }
    return ret_value;
}
int
DeviseCommand_printDispatcher::Run(int argc, char** argv)
{
    int ret_value;
    ret_value=DeviseCommand::Run(argc, argv);
    if (ret_value)
    {
        {
          control->ReturnVal(API_ACK, "done");
          return 1;
        }
    }
    return ret_value;
}
int
DeviseCommand_catFiles::Run(int argc, char** argv)
{
    int ret_value;
    ret_value=DeviseCommand::Run(argc, argv);
    if (ret_value)
    {
        {
          CatFiles(numArgs, args);
          control->ReturnVal(numArgs, args);
          return 1;
        }
    }
    return ret_value;
}
int
DeviseCommand_exit::Run(int argc, char** argv)
{
    int ret_value;
    ret_value=DeviseCommand::Run(argc, argv);
    if (ret_value)
    {
        {
          control->ReturnVal(API_ACK, "done");
          control->RestartSession();
          return 1;
        }
    }
    return ret_value;
}
int
DeviseCommand_clearAll::Run(int argc, char** argv)
{
    int ret_value;
    ret_value=DeviseCommand::Run(argc, argv);
    if (ret_value)
    {
        {
          control->DestroySessionData();
          control->ReturnVal(API_ACK, "done");
          return 1;
        }
    }
    return ret_value;
}
int
DeviseCommand_sync::Run(int argc, char** argv)
{
    int ret_value;
    ret_value=DeviseCommand::Run(argc, argv);
    if (ret_value)
    {
        {
          control->SetSyncNotify();
          control->ReturnVal(API_ACK, "done");
          return 1;
        }
    }
    return ret_value;
}
int
DeviseCommand_version::Run(int argc, char** argv)
{
    int ret_value;
    ret_value=DeviseCommand::Run(argc, argv);
    if (ret_value)
    {
        {
          control->ReturnVal(API_ACK, (char *) Version::Get());
          return 1;
        }
    }
    return ret_value;
}
int
DeviseCommand_copyright::Run(int argc, char** argv)
{
    int ret_value;
    ret_value=DeviseCommand::Run(argc, argv);
    if (ret_value)
    {
        {
          control->ReturnVal(API_ACK, (char *) Version::GetCopyright());
          return 1;
        }
    }
    return ret_value;
}
int
DeviseCommand_compDate::Run(int argc, char** argv)
{
    int ret_value;
    ret_value=DeviseCommand::Run(argc, argv);
    if (ret_value)
    {
        {
          control->ReturnVal(API_ACK, (char *) CompDate::Get());
          return 1;
        }
    }
    return ret_value;
}
int
DeviseCommand_new_leaks::Run(int argc, char** argv)
{
    int ret_value = 0;
#if PURIFY
    ret_value=DeviseCommand::Run(argc, argv);
    if (ret_value)
    {
        {
          if (purify_is_running()) {
    	purify_new_leaks();
          }
          control->ReturnVal(API_ACK, "done");
          return 1;
        }
    }
#endif
    return ret_value;
}
int
DeviseCommand_new_inuse::Run(int argc, char** argv)
{
    int ret_value;
    ret_value=DeviseCommand::Run(argc, argv);
#if PURIFY
    if (ret_value)
    {
        {
          if (purify_is_running()) {
    	purify_new_inuse();
          }
          control->ReturnVal(API_ACK, "done");
          return 1;
        }
    }
#endif
    return ret_value;
}
int
DeviseCommand_getWinCount::Run(int argc, char** argv)
{
    int ret_value;
    ret_value=DeviseCommand::Run(argc, argv);
    if (ret_value)
    {
        {
          char buf[100];
          sprintf(buf, "%d", DevWindow::GetCount());
          control->ReturnVal(API_ACK, buf);
          return 1;
        }
    }
    return ret_value;
}
int
DeviseCommand_getStringCount::Run(int argc, char** argv)
{
    int ret_value;
    ret_value=DeviseCommand::Run(argc, argv);
    if (ret_value)
    {
        {
          // Returns: <stringCount>
          char buf[100];
          sprintf(buf, "%d", StringStorage::GetCount());
          control->ReturnVal(API_ACK, buf);
          return 1;
        }
    }
    return ret_value;
}
int
DeviseCommand_waitForQueries::Run(int argc, char** argv)
{
    int ret_value;
    ret_value=DeviseCommand::Run(argc, argv);
    if (ret_value)
    {
        {
          // Arguments: none
          // Returns: "done"
    
          // Always go once thru the dispatcher, in case there's a view that
          // has requested a callback but hasn't started a query yet.
          QueryProc *qp = QueryProc::Instance();
          do {
    	Dispatcher::SingleStepCurrent();
          } while (!qp->Idle());
          control->ReturnVal(API_ACK, "done");
          return 1;
        }
    }
    return ret_value;
}
int
DeviseCommand_serverExit::Run(int argc, char** argv)
{
    int ret_value;
    ret_value=DeviseCommand::Run(argc, argv);
    if (ret_value)
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
            control->ReturnVal(API_ACK, "done");
            printf("Server exiting on command from client\n");
            ControlPanel::Instance()->DoQuit();
    		return 1; // We never get to here.
          } else {
    		char *result = "Server won't exit with more than one client connected.";
    		printf("%s\n", result);
            control->ReturnVal(API_NAK, result);
    		return -1;
    	  }
        }
    }
    return ret_value;
}
int
DeviseCommand_abortQuery::Run(int argc, char** argv)
{
    int ret_value;
    ret_value=DeviseCommand::Run(argc, argv);
    if (ret_value)
    {
        {
          View *view = (View *)classDir->FindInstance(argv[1]);
          if (!view) {
    	control->ReturnVal(API_NAK, "Cannot find view");
    	return -1;
          }
          view->AbortQuery();
          control->ReturnVal(API_ACK, "done");
          return 1;
        }
    }
    return ret_value;
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
    int ret_value;
    ret_value=DeviseCommand::Run(argc, argv);
    if (ret_value)
    {
        {
          char *name = ParseCat(argv[1]);
          if (!name) {
    	control->ReturnVal(API_NAK, "error parsing UNIXFILE schema");
    	return -1;
          }
          control->ReturnVal(API_ACK, name);
          return 1;
        }
    }
    return ret_value;
}
int
DeviseCommand_importFileType::Run_4(int argc, char** argv)
{
    int ret_value;
    ret_value=DeviseCommand::Run(argc, argv);
    if (ret_value)
    {
        {
          char *name = ParseCat(argv[1],argv[2],argv[3]);
          if (!name) {
    	strcpy(result , "");
    	control->ReturnVal(API_NAK, result);
    	return -1;
          }
          control->ReturnVal(API_ACK, name);
          return 1;
        }
    }
    return ret_value;
}
int
DeviseCommand_resetLinkMaster::Run(int argc, char** argv)
{
    int ret_value;
    ret_value=DeviseCommand::Run(argc, argv);
    if (ret_value)
    {
        {
          DeviseLink *link = (DeviseLink *)classDir->FindInstance(argv[1]);
          if (!link) {
    	control->ReturnVal(API_NAK, "Cannot find link");
    	return -1;
          }
          link->SetMasterView(0);
          control->ReturnVal(API_ACK, "done");
          return 1;
        }
    }
    return ret_value;
}
int
DeviseCommand_get3DLocation::Run(int argc, char** argv)
{
    int ret_value;
    ret_value=DeviseCommand::Run(argc, argv);
    if (ret_value)
    {
        {
          ViewGraph *view = (ViewGraph *)classDir->FindInstance(argv[1]);
          if (!view) {
    	control->ReturnVal(API_NAK, "Cannot find view");
    	return -1;
          }
          Camera c = view->GetCamera();
          sprintf(result, "%d %g %g %g %g %g %g %g %g %g %d %d",
    	      c.spherical_coord,
    	      c.x_, c.y_, c.z_, c.fx, c.fy, c.fz,
    	      c._theta, c._phi, c._rho,
                  c.fix_focus, c._perspective);
          control->ReturnVal(API_ACK, result);
          return 1;
        }
    }
    return ret_value;
}
int
DeviseCommand_getLinkMaster::Run(int argc, char** argv)
{
    int ret_value;
    ret_value=DeviseCommand::Run(argc, argv);
    if (ret_value)
    {
        {
          DeviseLink *link = (DeviseLink *)classDir->FindInstance(argv[1]);
          if (!link) {
    	control->ReturnVal(API_NAK, "Cannot find link");
    	return -1;
          }
          ViewGraph *view = link->GetMasterView();
          control->ReturnVal(API_ACK, (view ? view->GetName() : ""));
          return 1;
        }
    }
    return ret_value;
}
int
DeviseCommand_getLinkType::Run(int argc, char** argv)
{
    int ret_value;
    ret_value=DeviseCommand::Run(argc, argv);
    if (ret_value)
    {
        {
          RecordLink *link = (RecordLink *)classDir->FindInstance(argv[1]);
          if (!link) {
    	control->ReturnVal(API_NAK, "Cannot find link");
    	return -1;
          }
          control->ReturnVal(API_ACK, (link->GetLinkType() == Positive)? "1" : "0");
          return 1;
        }
    }
    return ret_value;
}
int
DeviseCommand_setBatchMode::Run(int argc, char** argv)
{
    int ret_value;
    ret_value=DeviseCommand::Run(argc, argv);
    if (ret_value)
    {
        {
          Boolean batch = (atoi(argv[1]) ? true : false);
          control->SetBatchMode(batch);
          control->ReturnVal(API_ACK, "done");
          return 1;
        }
    }
    return ret_value;
}
int
DeviseCommand_invalidateTData::Run(int argc, char** argv)
{
    int ret_value;
    ret_value=DeviseCommand::Run(argc, argv);
    if (ret_value)
    {
        {
          TData *tdata = (TData *)classDir->FindInstance(argv[1]);
          if (!tdata) {
    	control->ReturnVal(API_NAK, "Cannot find TData");
    	return -1;
          }
          tdata->InvalidateTData();
          control->ReturnVal(API_ACK, "Done");
          return 1;
        }
    }
    return ret_value;
}
int
DeviseCommand_invalidatePixmap::Run(int argc, char** argv)
{
    int ret_value;
    ret_value=DeviseCommand::Run(argc, argv);
    if (ret_value)
    {
        {
          View *vg = (View *)classDir->FindInstance(argv[1]);
          if (!vg) {
    	control->ReturnVal(API_NAK, "Cannot find view");
    	return -1;
          }
          vg->InvalidatePixmaps();
          control->ReturnVal(API_ACK, "done");
          return 1;
        }
    }
    return ret_value;
}
int
DeviseCommand_readLine::Run(int argc, char** argv)
{
    int ret_value;
    ret_value=DeviseCommand::Run(argc, argv);
    if (ret_value)
    {
        {
          FILE *file = (FILE *)atol(argv[1]);
          (void)fgets(result, 256, file);
          int len = strlen(result);
          if (len > 0 && result[len - 1] == '\n')
    	result[len - 1] = '\0';
          control->ReturnVal(API_ACK, result);
          return 1;
        }
    }
    return ret_value;
}
int
DeviseCommand_close::Run(int argc, char** argv)
{
    int ret_value;
    ret_value=DeviseCommand::Run(argc, argv);
    if (ret_value)
    {
        {
          FILE *file = (FILE *)atol(argv[1]);
          fclose(file);
          control->ReturnVal(API_ACK, "done");
          return 1;
        }
    }
    return ret_value;
}
int
DeviseCommand_isMapped::Run(int argc, char** argv)
{
    int ret_value;
    ret_value=DeviseCommand::Run(argc, argv);
    if (ret_value)
    {
        {
          View *vg = (View *)classDir->FindInstance(argv[1]);
          if (!vg) {
    	control->ReturnVal(API_NAK, "Cannot find view");
    	return -1;
          }
          sprintf(result, "%d", (vg->Mapped() ? 1 : 0 ));
          control->ReturnVal(API_ACK, result);
          return 1;
        }
    }
    return ret_value;
}
int
DeviseCommand_getLabel::Run(int argc, char** argv)
{
    int ret_value;
    ret_value=DeviseCommand::Run(argc, argv);
    if (ret_value)
    {
        {
          View *vg = (View *)classDir->FindInstance(argv[1]);
          if (!vg) {
    	control->ReturnVal(API_NAK, "Cannot find view");
    	return -1;
          }
          Boolean occupyTop;
          int extent;
          char *name;
          vg->GetLabelParam(occupyTop, extent, name);
          sprintf(result, "%d %d {%s}", (occupyTop ? 1 : 0), extent,
    	      (name ? name : ""));
          control->ReturnVal(API_ACK, result);
          return 1;
        }
    }
    return ret_value;
}
int
DeviseCommand_tdataFileName::Run(int argc, char** argv)
{
    int ret_value;
    ret_value=DeviseCommand::Run(argc, argv);
    if (ret_value)
    {
        {
          TData *tdata = (TData *)classDir->FindInstance(argv[1]);
          if (!tdata) {
    	control->ReturnVal(API_NAK, "Cannot find tdata");
    	return -1;
          }
          control->ReturnVal(API_ACK, tdata->GetName());
          return 1;
        }
    }
    return ret_value;
}
int
DeviseCommand_getViewWin::Run(int argc, char** argv)
{
    int ret_value;
    ret_value=DeviseCommand::Run(argc, argv);
    if (ret_value)
    {
        {
          View *view = (View *)classDir->FindInstance(argv[1]);
          if (!view) {
    	control->ReturnVal(API_NAK, "Cannot find view");
    	return -1;
          }
          ViewWin *viewWin = view->GetParent();
          if (!viewWin)
    	control->ReturnVal(API_ACK, "");
          else
    	control->ReturnVal(API_ACK, viewWin->GetName());
          return 1;
        }
    }
    return ret_value;
}
int
DeviseCommand_clearViewHistory::Run(int argc, char** argv)
{
    int ret_value;
    ret_value=DeviseCommand::Run(argc, argv);
    if (ret_value)
    {
        {
          View *view = (View *)classDir->FindInstance(argv[1]);
          if (!view) {
    	control->ReturnVal(API_NAK, "Cannot find cursor");
    	return -1;
          }
          FilterQueue *queue = view->GetHistory();
          queue->Clear();
          control->ReturnVal(API_ACK, "done");
          return 1;
        }
    }
    return ret_value;
}
int
DeviseCommand_getCursorViews::Run(int argc, char** argv)
{
    int ret_value;
    ret_value=DeviseCommand::Run(argc, argv);
    if (ret_value)
    {
        {
          DeviseCursor *cursor = (DeviseCursor *)classDir->FindInstance(argv[1]);
          if (!cursor) {
    	control->ReturnVal(API_NAK, "Cannot find cursor");
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
          control->ReturnVal(2, name);
          return 1;
        } 
    }
    return ret_value;
}
int
DeviseCommand_getMappingTData::Run(int argc, char** argv)
{
    int ret_value;
    ret_value=DeviseCommand::Run(argc, argv);
    if (ret_value)
    {
        {
          TDataMap *map= (TDataMap *)classDir->FindInstance(argv[1]);
          if (!map) {
    	control->ReturnVal(API_NAK, "Cannot find mapping");
    	return -1;
          }
          TData *tdata = map->GetTData();
          control->ReturnVal(API_ACK, classDir->FindInstanceName(tdata));
          return 1;
        }
    }
    return ret_value;
}
int
DeviseCommand_destroy::Run(int argc, char** argv)
{
    int ret_value;
    ret_value=DeviseCommand::Run(argc, argv);
    if (ret_value)
    {
        {
          classDir->DestroyInstance(argv[1]);
          control->ReturnVal(API_ACK, "done");
          return 1;
        }
    }
    return ret_value;
}
int
DeviseCommand_parseDateFloat::Run(int argc, char** argv)
{
    int ret_value;
    ret_value=DeviseCommand::Run(argc, argv);
    if (ret_value)
    {
        {
          double val;
          (void)ParseFloatDate(argv[1], val);
          sprintf(result, "%f", val);
          control->ReturnVal(API_ACK, result);
          return 1;
        }
    }
    return ret_value;
}
int
DeviseCommand_isInterpretedGData::Run(int argc, char** argv)
{
    int ret_value;
    ret_value=DeviseCommand::Run(argc, argv);
    if (ret_value)
    {
        {
          TDataMap *map= (TDataMap *)classDir->FindInstance(argv[1]);
          if (!map) {
    	control->ReturnVal(API_NAK, "Cannot find mapping");
    	return -1;
          }
          if (map->IsInterpreted())
    	strcpy(result, "1");
          else
    	strcpy(result, "0");
          control->ReturnVal(API_ACK, result);
          return 1;
        }
    }
    return ret_value;
}
int
DeviseCommand_isInterpreted::Run(int argc, char** argv)
{
    int ret_value;
    ret_value=DeviseCommand::Run(argc, argv);
    if (ret_value)
    {
        {
          int *isInterp = (int *)classDir->UserInfo("mapping", argv[1]);
          strcpy(result, "0");
          if (isInterp && *isInterp)
    	strcpy(result, "1");
          control->ReturnVal(API_ACK, result);
          return 1;
        }
    }
    return ret_value;
}
int
DeviseCommand_getPixelWidth::Run(int argc, char** argv)
{
    int ret_value;
    ret_value=DeviseCommand::Run(argc, argv);
    if (ret_value)
    {
        {
          TDataMap *map= (TDataMap *)classDir->FindInstance(argv[1]);
          if (!map) {
    	control->ReturnVal(API_NAK, "Cannot find mapping");
    	return -1;
          }
          sprintf(result, "%d", map->GetPixelWidth());
          control->ReturnVal(API_ACK, result);
          return 1;
        }
    }
    return ret_value;
}
int
DeviseCommand_getTopGroups::Run(int argc, char** argv)
{
    int ret_value;
    ret_value=DeviseCommand::Run(argc, argv);
    if (ret_value)
    {
        {
          control->GetGroupDir()->top_level_groups(result, argv[1]);
          control->ReturnVal(API_ACK, result);
          return 1;
        }
    }
    return ret_value;
}
int
DeviseCommand_getWindowLayout::Run(int argc, char** argv)
{
    int ret_value;
    ret_value=DeviseCommand::Run(argc, argv);
    if (ret_value)
    {
        {
          ViewLayout *layout = (ViewLayout *)classDir->FindInstance(argv[1]);
          if (!layout) {
    	control->ReturnVal(API_NAK, "Cannot find window");
    	return -1;
          }
          int v, h;
          Boolean stacked;
          layout->GetPreferredLayout(v, h, stacked);
          sprintf(result, "%d %d %d", v, h, (stacked ? 1 : 0));
          control->ReturnVal(API_ACK, result);
          return 1;
        }
    }
    return ret_value;
}
int
DeviseCommand_getSchema::Run(int argc, char** argv)
{
    int ret_value;
    ret_value=DeviseCommand::Run(argc, argv);
    if (ret_value)
    {
        {
        	
          TData *tdata = (TData *)classDir->FindInstance(argv[1]);
          if (!tdata) {
    	control->ReturnVal(API_NAK, "Cannot find Tdata");
    	return -1;
          }
          AttrList *attrList = tdata->GetAttrList();
          if (!attrList) {
    	control->ReturnVal(API_NAK, "Null attribute list");
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
    //    control->ReturnVal(numAttrs + 1, args) ;
          control->ReturnVal(numAttrs, args) ;
    	 delete [] args;
          return 1;
        }
    }
    return ret_value;
}
int
DeviseCommand_getAction::Run(int argc, char** argv)
{
    int ret_value;
    ret_value=DeviseCommand::Run(argc, argv);
    if (ret_value)
    {
        {
          ViewGraph *view = (ViewGraph *)classDir->FindInstance(argv[1]);
          if (!view) {
    	control->ReturnVal(API_NAK, "Cannot find view");
    	return -1;
          }
          Action *action = view->GetAction();
          if (!action || !strcmp(action->GetName(), "") ||
    	  !strcmp(action->GetName(), "default"))
    	strcpy(result , "");
          else
    	strcpy(result,action->GetName());
          control->ReturnVal(API_ACK, result);
          return 1;
        }
    }
    return ret_value;
}
int
DeviseCommand_getLinkFlag::Run(int argc, char** argv)
{
    int ret_value;
    ret_value=DeviseCommand::Run(argc, argv);
    if (ret_value)
    {
        {
          DeviseLink *link = (DeviseLink *)classDir->FindInstance(argv[1]);
          if (!link) {
    	control->ReturnVal(API_NAK, "Cannot find link");
    	return -1;
          }
          sprintf(result, "%d", link->GetFlag());
          control->ReturnVal(API_ACK, result);
          return 1;
        }
    }
    return ret_value;
}
int
DeviseCommand_changeableParam::Run(int argc, char** argv)
{
    int ret_value;
    ret_value=DeviseCommand::Run(argc, argv);
    if (ret_value)
    {
        {
          Boolean changeable = classDir->Changeable(argv[1]);
          if (changeable)
    	strcpy(result, "1");
          else
    	strcpy(result, "0");
          control->ReturnVal(API_ACK, result);
          return 1;
        }
    }
    return ret_value;
}
int
DeviseCommand_getInstParam::Run(int argc, char** argv)
{
    int ret_value;
    ret_value=DeviseCommand::Run(argc, argv);
    if (ret_value)
    {
        {
          classDir->GetParams(argv[1], numArgs, args);
          control->ReturnVal(numArgs, args);
          return 1;
        }
    }
    return ret_value;
}
int
DeviseCommand_tcheckpoint::Run(int argc, char** argv)
{
    int ret_value;
    ret_value=DeviseCommand::Run(argc, argv);
    if (ret_value)
    {
        {
          TData *tdata;
          control->SetBusy();
          tdata = (TData *)classDir->FindInstance(argv[1]);
          if (tdata)
    	tdata->Checkpoint();
          control->SetIdle();
          control->ReturnVal(API_ACK, "done");
          return 1;
        }
    }
    return ret_value;
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
    int ret_value;
    ret_value=DeviseCommand::Run(argc, argv);
    if (ret_value)
    {
        {
          classDir->ClassNames(argv[1], numArgs, args);
          control->ReturnVal(numArgs, args);
          return 1;
        }
    }
    return ret_value;
}
int
DeviseCommand_get::Run_3(int argc, char** argv)
{
    int ret_value;
    ret_value=DeviseCommand::Run(argc, argv);
    if (ret_value)
    {
        {
          classDir->InstanceNames(argv[1], argv[2], numArgs, args);
          control->ReturnVal(numArgs, args);
          return 1;
        }
    }
    return ret_value;
}

int
DeviseCommand_changeMode::Run(int argc, char** argv)
{
    int ret_value;
    ret_value=DeviseCommand::Run(argc, argv);
    if (ret_value)
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
	  control->ReturnVal(API_ACK, "done");
  	  return 1;
    }
    return ret_value;
}

int
DeviseCommand_exists::Run(int argc, char** argv)
{
    int ret_value;
    ret_value=DeviseCommand::Run(argc, argv);
    if (ret_value)
    {
        {
          if (!classDir->FindInstance(argv[1]))
    	strcpy(result, "0");
          else
    	strcpy(result, "1");
          control->ReturnVal(API_ACK, result);
          return 1;
        }
    }
    return ret_value;
}
int
DeviseCommand_removeView::Run(int argc, char** argv)
{
    int ret_value;
    ret_value=DeviseCommand::Run(argc, argv);
    if (ret_value)
    {
        {
          ViewGraph *view = (ViewGraph *)classDir->FindInstance(argv[1]);
          if (!view) {
    	control->ReturnVal(API_NAK, "Cannot find view");
    	return -1;
          }
          if (!view->Mapped()) {
    	control->ReturnVal(API_NAK, "View not in any window");
    	return -1;
          }
          view->DeleteFromParent();
          control->ReturnVal(API_ACK, "done");
          return 1;
        }
    }
    return ret_value;
}
int
DeviseCommand_getViewMappings::Run(int argc, char** argv)
{
    int ret_value;
    ret_value=DeviseCommand::Run(argc, argv);
    if (ret_value)
    {
        {
          ViewGraph *view = (ViewGraph *)classDir->FindInstance(argv[1]);
          if (!view) {
    	control->ReturnVal(API_NAK, "Cannot find view");
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
          control->ReturnVal(API_ACK, result);
          return 1;
        }
    }
    return ret_value;
}
int
DeviseCommand_refreshView::Run(int argc, char** argv)
{
    int ret_value;
    ret_value=DeviseCommand::Run(argc, argv);
    if (ret_value)
    {
        {
          View *view = (View *)classDir->FindInstance(argv[1]);
          if (!view) {
    	control->ReturnVal(API_NAK, "Cannot find view");
    	return -1;
          }
          view->Refresh();
          control->ReturnVal(API_ACK, "done");
          return 1;
        }
    }
    return ret_value;
}
int
DeviseCommand_getWinGeometry::Run(int argc, char** argv)
{
    int ret_value;
    ret_value=DeviseCommand::Run(argc, argv);
    if (ret_value)
    {
        {
          ViewWin *win = (ViewWin*)classDir->FindInstance(argv[1]);
          if (!win) {
    	control->ReturnVal(API_NAK, "Cannot find window");
    	return -1;
          }
          int x, y;
          unsigned h, w;
          int x0, y0;
          win->Geometry(x, y, w, h);
          win->AbsoluteOrigin(x0,y0);
          sprintf (result, "{ %d %d %d %d %u %u }" , x0, y0, x, y, w, h);
          control->ReturnVal(API_ACK, result);
          return 1;	      
        }
    }
    return ret_value;
}
int
DeviseCommand_getWinViews::Run(int argc, char** argv)
{
    int ret_value;
    ret_value=DeviseCommand::Run(argc, argv);
    if (ret_value)
    {
        {
          ViewWin *win = (ViewWin*)classDir->FindInstance(argv[1]);
          if (!win) {
    	control->ReturnVal(API_NAK, "Cannot find window");
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
          control->ReturnVal(API_ACK, result);
    
          return 1;
        }
    }
    return ret_value;
}
int
DeviseCommand_getLinkViews::Run(int argc, char** argv)
{
    int ret_value;
    ret_value=DeviseCommand::Run(argc, argv);
    if (ret_value)
    {
        {
          DeviseLink *link = (DeviseLink *)classDir->FindInstance(argv[1]);
          if (!link) {
    	control->ReturnVal(API_NAK, "Cannot find link");
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
          control->ReturnVal(API_ACK, result);
          return 1;
        }
    }
    return ret_value;
}
int
DeviseCommand_getCurVisualFilter::Run(int argc, char** argv)
{
    int ret_value;
    ret_value=DeviseCommand::Run(argc, argv);
    if (ret_value)
    {
        {
          View *view = (View *)classDir->FindInstance(argv[1]);
          if (!view) {
    	control->ReturnVal(API_NAK, "Cannot find view");
    	return -1;
          }
          VisualFilter *filter = view->GetVisualFilter();
          sprintf(result, "%.2f %.2f %.2f %.2f", filter->xLow, filter->yLow,
    	      filter->xHigh, filter->yHigh);
          control->ReturnVal(API_ACK, result);
          return 1;
        }
    }
    return ret_value;
}
int
DeviseCommand_getVisualFilters::Run(int argc, char** argv)
{
    int ret_value;
    ret_value=DeviseCommand::Run(argc, argv);
    if (ret_value)
    {
        {
          View *view = (View *)classDir->FindInstance(argv[1]);
          if (!view) {
    	control->ReturnVal(API_NAK, "Cannot find view");
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
          control->ReturnVal(API_ACK, result);
          return 1;
        }
    }
    return ret_value;
}
int
DeviseCommand_getViewStatistics::Run(int argc, char** argv)
{
    int ret_value;
    ret_value=DeviseCommand::Run(argc, argv);
    if (ret_value)
    {
        {
          ViewGraph *vg = (ViewGraph *)classDir->FindInstance(argv[1]);
          if (!vg) {
    	control->ReturnVal(API_NAK, "Cannot find view");
    	return -1;
          }
          /* Return status of statistics display */
          control->ReturnVal(API_ACK, vg->GetDisplayStats());
          return 1;
        }
    }
    return ret_value;
}
int
DeviseCommand_getAllStats::Run(int argc, char** argv)
{
    int ret_value;
    ret_value=DeviseCommand::Run(argc, argv);
    if (ret_value)
    {
        {
          ViewGraph *vg = (ViewGraph *)classDir->FindInstance(argv[1]);
          if (!vg) {
            control->ReturnVal(API_NAK, "Cannot find view");
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
          control->ReturnVal(4, buff);
          for (i = 0; i < 4; i++) {
    	  delete buff[i];
          }
          return 1;
        }    	
    }
    return ret_value;
}
int
DeviseCommand_getStatBuffer::Run(int argc, char** argv)
{
    int ret_value;
    ret_value=DeviseCommand::Run(argc, argv);
    if (ret_value)
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
    	  control->ReturnVal(API_NAK, "Invalid stat type");
    	  return -1;
            }
            while( *viewName == ' ' ) {
    	   viewName++;
    	}
            ViewGraph *vg = (ViewGraph *)classDir->FindInstance(viewName);
            if(!vg) {
    	  control->ReturnVal(API_NAK, "Cannot find view");
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
    	     control->ReturnVal(API_NAK, "statBuffer object");
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
               control->ReturnVal(k, args);
               for(int j = 0; j < k; j++)
    	       delete args[j];
               delete [] args;
               return 1;
        }
    }
    return ret_value;
}
int
DeviseCommand_getViewDimensions::Run(int argc, char** argv)
{
    int ret_value;
    ret_value=DeviseCommand::Run(argc, argv);
    if (ret_value)
    {
        {
          View *vg = (View *)classDir->FindInstance(argv[1]);
          if (!vg) {
    	control->ReturnVal(API_NAK, "Cannot find view");
    	return -1;
          }
          /* Return number of dimensions in view */
          sprintf(result, "%d", vg->GetNumDimensions());
          control->ReturnVal(API_ACK, result);
          return 1;
        }
    }
    return ret_value;
}
int
DeviseCommand_getViewSolid3D::Run(int argc, char** argv)
{
    int ret_value;
    ret_value=DeviseCommand::Run(argc, argv);
    if (ret_value)
    {
        {
          View *vg = (View *)classDir->FindInstance(argv[1]);
          if (!vg) {
    	control->ReturnVal(API_NAK, "Cannot find view");
    	return -1;
          }
          /* Return setting of solid/wireframe 3D objects */
          sprintf(result, "%d", vg->GetSolid3D());
          control->ReturnVal(API_ACK, result);
          return 1;
        }
    }
    return ret_value;
}
int
DeviseCommand_getViewXYZoom::Run(int argc, char** argv)
{
    int ret_value;
    ret_value=DeviseCommand::Run(argc, argv);
    if (ret_value)
    {
        {
          View *vg = (View *)classDir->FindInstance(argv[1]);
          if (!vg) {
    	control->ReturnVal(API_NAK, "Cannot find view");
    	return -1;
          }
          /* Return setting of XY or X/Y zoom */
          sprintf(result, "%d", (vg->IsXYZoom() ? 1 : 0));
          control->ReturnVal(API_ACK, result);
          return 1;
        }
    }
    return ret_value;
}
int
DeviseCommand_getViewDisplayDataValues::Run(int argc, char** argv)
{
    int ret_value;
    ret_value=DeviseCommand::Run(argc, argv);
    if (ret_value)
    {
        {
          View *vg = (View *)classDir->FindInstance(argv[1]);
          if (!vg) {
    	control->ReturnVal(API_NAK, "Cannot find view");
    	return -1;
          }
          /* Return setting of data value display */
          sprintf(result, "%d", (vg->GetDisplayDataValues() ? 1 : 0));
          control->ReturnVal(API_ACK, result);
          return 1;
        }
    }
    return ret_value;
}
int
DeviseCommand_getViewPileMode::Run(int argc, char** argv)
{
    int ret_value;
    ret_value=DeviseCommand::Run(argc, argv);
    if (ret_value)
    {
        {
          View *vg = (View *)classDir->FindInstance(argv[1]);
          if (!vg) {
    	control->ReturnVal(API_NAK, "Cannot find view");
    	return -1;
          }
          /* Return pile mode flag */
          sprintf(result, "%d", (vg->IsInPileMode() ? 1 : 0));
          control->ReturnVal(API_ACK, result);
          return 1;
        }
    }
    return ret_value;
}
int
DeviseCommand_raiseView::Run(int argc, char** argv)
{
    int ret_value;
    ret_value=DeviseCommand::Run(argc, argv);
    if (ret_value)
    {
        {
          View *view = (View *)classDir->FindInstance(argv[1]);
          if (!view) {
    	control->ReturnVal(API_NAK, "Cannot find view");
    	return -1;
          }
          view->Raise();
          control->ReturnVal(API_ACK, "done");
          return 1;
        }
    }
    return ret_value;
}
int
DeviseCommand_lowerView::Run(int argc, char** argv)
{
    int ret_value;
    ret_value=DeviseCommand::Run(argc, argv);
    if (ret_value)
    {
        {
          View *view = (View *)classDir->FindInstance(argv[1]);
          if (!view) {
    	control->ReturnVal(API_NAK, "Cannot find view");
    	return -1;
          }
          view->Lower();
          control->ReturnVal(API_ACK, "done");
          return 1;
        }
    }
    return ret_value;
}
int
DeviseCommand_getFileHeader::Run(int argc, char** argv)
{
    int ret_value;
    ret_value=DeviseCommand::Run(argc, argv);
    if (ret_value)
    {
        {
          control->ReturnVal(API_ACK, DevFileHeader::Get(argv[1]));
          return 1;
        }
    }
    return ret_value;
}
int
DeviseCommand_getViewLensParams::Run(int argc, char** argv)
{
    int ret_value;
    ret_value=DeviseCommand::Run(argc, argv);
    if (ret_value)
    {
        {
          ViewLens *lens = (ViewLens *)classDir->FindInstance(argv[1]);
          if (!lens) {
           control->ReturnVal(API_NAK, "Cannot find lens");
           return -1;
          }
          /* Get mode */
          sprintf(result, "%s %s { ", lens->GetMode(), lens->GetLink()->GetName()); 
          /* get list of views */
          int index = lens->InitViewLensIterator();
          while(lens->MoreViewsInLens(index)) {
               ViewGraph *view = lens->NextViewInLens(index);
               strcat(result, " {");
               strcat(result, view->GetName());
               strcat(result, "} ");
          }
          lens->DoneViewLensIterator(index);
          strcat(result, " } ");
          control->ReturnVal(API_ACK, result);
          return 1;
        }
    }
    return ret_value;
}
int
DeviseCommand_winGetPrint::Run(int argc, char** argv)
{
    int ret_value;
    ret_value=DeviseCommand::Run(argc, argv);
    if (ret_value)
    {
        {
          ViewWin *win = (ViewWin *)classDir->FindInstance(argv[1]);
          if (!win) {
            control->ReturnVal(API_NAK, "Cannot find window");
            return -1;
          }
          Boolean exclude, pixmap;
          exclude = win->GetPrintExclude();
          pixmap = win->GetPrintPixmap();
          char buf[100];
          sprintf(buf, "%d %d", exclude, pixmap);
          control->ReturnVal(API_ACK, buf);
          return 1;
        }
    }
    return ret_value;
}
int
DeviseCommand_viewGetHome::Run(int argc, char** argv)
{
    int ret_value;
    ret_value=DeviseCommand::Run(argc, argv);
    if (ret_value)
    {
        {
          // Arguments: <viewName>
          // Returns: <mode> <autoXMargin> <autoYMargin> <manXLo> <manYLo>
          // <manXHi> <manYHi>
    #if defined(DEBUG)
          printf("viewGetHome <%s>\n", argv[1]);
    #endif
          ViewGraph *view = (ViewGraph *)classDir->FindInstance(argv[1]);
          if (!view) {
    	control->ReturnVal(API_NAK, "Cannot find view");
    	return -1;
          }
          ViewHomeInfo info;
          view->GetHomeInfo(info);
          char buf[100];
          sprintf(buf, "%d %f %f %f %f %f %f", (int) info.mode, info.autoXMargin,
    	info.autoYMargin, info.manXLo, info.manYLo, info.manXHi, info.manYHi);
          control->ReturnVal(API_ACK, buf);
          return 1;
        }
    }
    return ret_value;
}
int
DeviseCommand_viewGetHorPan::Run(int argc, char** argv)
{
    int ret_value;
    ret_value=DeviseCommand::Run(argc, argv);
    if (ret_value)
    {
        {
          // Arguments: <viewName>
          // Returns: <mode> <relativePan> <absolutePan>
    #if defined(DEBUG)
          printf("viewGetHorPan <%s>\n", argv[1]);
    #endif
          ViewGraph *view = (ViewGraph *)classDir->FindInstance(argv[1]);
          if (!view) {
    	control->ReturnVal(API_NAK, "Cannot find view");
    	return -1;
          }
          ViewPanInfo info;
          view->GetHorPanInfo(info);
          char buf[100];
          sprintf(buf, "%d %f %f", (int) info.mode, info.relPan, info.absPan);
          control->ReturnVal(API_ACK, buf);
          return 1;
        }
    }
    return ret_value;
}
int
DeviseCommand_getCursorGrid::Run(int argc, char** argv)
{
    int ret_value;
    ret_value=DeviseCommand::Run(argc, argv);
    if (ret_value)
    {
        {
          // Arguments: <cursorName>
          // Returns: <useGrid> <gridX> <gridY>
    #if defined(DEBUG)
          printf("getCursorGrid <%s>\n", argv[1]);
    #endif
          DeviseCursor *cursor = (DeviseCursor *) classDir->FindInstance(argv[1]);
          if (!cursor) {
    	control->ReturnVal(API_NAK, "Cannot find cursor");
    	return -1;
          }
          Boolean useGrid;
          Coord gridX, gridY;
          cursor->GetGrid(useGrid, gridX, gridY);
          char buf[100];
          sprintf(buf, "%d %f %f", useGrid, gridX, gridY);
          control->ReturnVal(API_ACK, buf);
          return 1;
        }
    }
    return ret_value;
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
			control->ReturnVal(API_ACK, "done");
			return 1;
		} else {
			control->ReturnVal(API_NAK, "Error saving session description");
			return -1;
		}
	} else {
		control->ReturnVal(API_NAK,
		  "Wrong number of arguments for DEVise writeDesc");
		return -1;
	}
IMPLEMENT_COMMAND_END

int
DeviseCommand_saveStringSpace::Run(int argc, char** argv)
{
    int ret_value;
    ret_value=DeviseCommand::Run(argc, argv);
    if (ret_value)
    {
        {
          // Argument: <file name>
    #if defined(DEBUG)
          printf("saveStringSpace <%s>\n", argv[1]);
    #endif
          if (StringStorage::Save(argv[1]) != 0) {
            control->ReturnVal(API_NAK, "can't save string space");
            return -1;
          }
          control->ReturnVal(API_ACK, "done");
          return 1;
        }
    }
    return ret_value;
}
int
DeviseCommand_loadStringSpace::Run(int argc, char** argv)
{
    int ret_value;
    ret_value=DeviseCommand::Run(argc, argv);
    if (ret_value)
    {
        {
          // Argument: <file name>
    #if defined(DEBUG)
          printf("loadStringSpace <%s>\n", argv[1]);
    #endif
          if (StringStorage::Clear() != 0) {
            control->ReturnVal(API_NAK, "can't clear string space");
            return -1;
          }
          if (StringStorage::Load(argv[1]) != 0) {
            control->ReturnVal(API_NAK, "can't load string space");
            return -1;
          }
          View::RefreshAll();
          control->ReturnVal(API_ACK, "done");
          return 1;
        }
    }
    return ret_value;
}
int
DeviseCommand_dumpLinkCursor::Run(int argc, char** argv)
{
    int ret_value;
    ret_value=DeviseCommand::Run(argc, argv);
    if (ret_value)
    {
        {
          // Argument: <file name>
    #if defined(DEBUG)
          printf("dumpLinkCursor <%s>\n", argv[1]);
    #endif
          FILE *fp = fopen(argv[1], "w");
          if (fp == NULL) {
            control->ReturnVal(API_NAK, "can't dump link/cursor info");
            return -1;
          }
          char *header = DevFileHeader::Get(FILE_TYPE_LINKDESC);
          fprintf(fp, "%s", header);
          DevLink::Dump(fp);
          DevCursor::Dump(fp);
          (void) fclose(fp);
          control->ReturnVal(API_ACK, "done");
          return 1;
        }
    }
    return ret_value;
}
int
DeviseCommand_openSession::Run(int argc, char** argv)
{
    int ret_value;
    ret_value=DeviseCommand::Run(argc, argv);
    if (ret_value)
    {
        {
          // Argument: <file name>
    #if defined(DEBUG)
          printf("openSession <%s>\n", argv[1]);
    #endif
          if (!Session::Open(argv[1]).IsComplete()) {
    	control->ReturnVal(API_NAK, "Unable to open session file");
    	return -1;
          }
          control->ReturnVal(API_ACK, "done");
          return 1;
        }
    }
    return ret_value;
}
int
DeviseCommand_createTData::Run(int argc, char** argv)
{
    int ret_value;
    ret_value=DeviseCommand::Run(argc, argv);
    if (ret_value)
    {
        {
          // Argument: <data source name>
    #if defined(DEBUG)
          printf("createTData <%s>\n", argv[1]);
    #endif
		  TData *tdata = (TData *)classDir->FindInstance(argv[1]);
		  if (!tdata) {
          	if (!Session::CreateTData(argv[1]).IsComplete()) {
           	 	control->ReturnVal(API_NAK, "Unable to create tdata");
            	return -1;
          	}
		  }
          //TEMPTEMP -- may need to return something different here
          control->ReturnVal(API_ACK, "done");
          return 1;
        }
    }
    return ret_value;
}
int
DeviseCommand_getViewGDS::Run(int argc, char** argv)
{
    int ret_value;
    ret_value=DeviseCommand::Run(argc, argv);
    if (ret_value)
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
    	control->ReturnVal(API_NAK, "Cannot find view");
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
          control->ReturnVal(API_ACK, buf);
          return 1;
        }
    }
    return ret_value;
}
int
DeviseCommand_testDataSock::Run(int argc, char** argv)
{
    int ret_value;
    ret_value=DeviseCommand::Run(argc, argv);
    if (ret_value)
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
            control->ReturnVal(API_NAK, "Invalid socket to write");
            return -1;
          }
    
          char *testStr = "abcd0123456789";
          int numBytes = strlen(testStr) + 1;
          if (write(fd, testStr, numBytes) != numBytes) {
            reportErrSys("write error");
            control->ReturnVal(API_NAK, "write error");
            (void) close(fd);
            return -1;
          }
    
          close(fd);
          control->ReturnVal(API_ACK, "done");
          return 1;
        }
    }
    return ret_value;
}
int
DeviseCommand_viewGetAlign::Run(int argc, char** argv)
{
    int ret_value;
    ret_value=DeviseCommand::Run(argc, argv);
    if (ret_value)
    {
        {
          // Argument: <view name>
          // Returns: <alignment value>
    #if defined(DEBUG)
          printf("viewGetAlign <%s>\n", argv[1]);
    #endif
          ViewGraph *view = (ViewGraph *)classDir->FindInstance(argv[1]);
          if (!view) {
    	    control->ReturnVal(API_NAK, "Cannot find view");
    	    return -1;
          }
          char buf[1024];
          sprintf(buf, "%d", view->GetAlign());
          control->ReturnVal(API_ACK, buf);
          return 1;
        }
    }
    return ret_value;
}
int
DeviseCommand_setLinkMaster::Run(int argc, char** argv)
{
    int ret_value;
    ret_value=DeviseCommand::Run(argc, argv);
    if (ret_value)
    {
        {
          DeviseLink *link = (DeviseLink *)classDir->FindInstance(argv[1]);
          if (!link) {
    	control->ReturnVal(API_NAK, "Cannot find link");
    	return -1;
          }
          ViewGraph *view = (ViewGraph *)classDir->FindInstance(argv[2]);
          if (!view) {
    	control->ReturnVal(API_NAK, "Cannot find view");
    	return -1;
          }
          link->SetMasterView(view);
          control->ReturnVal(API_ACK, "done");
          return 1;
        }
    }
    return ret_value;
}
int
DeviseCommand_setLinkType::Run(int argc, char** argv)
{
    int ret_value;
    ret_value=DeviseCommand::Run(argc, argv);
    if (ret_value)
    {
        {
          RecordLink *link = (RecordLink *)classDir->FindInstance(argv[1]);
          if (!link) {
    	control->ReturnVal(API_NAK, "Cannot find link");
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
    return ret_value;
}
int
DeviseCommand_setScreenSize::Run(int argc, char** argv)
{
    int ret_value;
    ret_value=DeviseCommand::Run(argc, argv);
    if (ret_value)
    {
        {
          DeviseDisplay::DefaultDisplay()->DesiredScreenWidth() = atoi(argv[1]);
          DeviseDisplay::DefaultDisplay()->DesiredScreenHeight() = atoi(argv[2]);
          control->ReturnVal(API_ACK, "done");
          return 1;
        }
    }
    return ret_value;
}
int
DeviseCommand_writeLine::Run(int argc, char** argv)
{
    int ret_value;
    ret_value=DeviseCommand::Run(argc, argv);
    if (ret_value)
    {
        {
          FILE *file = (FILE *)atol(argv[2]);
          fputs(argv[1],file);
          fputs("\n",file);
          control->ReturnVal(API_ACK, "done");
          return 1;
        }
    }
    return ret_value;
}
int
DeviseCommand_open::Run(int argc, char** argv)
{
    int ret_value;
    ret_value=DeviseCommand::Run(argc, argv);
    if (ret_value)
    {
        {
          FILE *file = fopen(argv[1],argv[2]);
          if (!file) {
    	control->ReturnVal(API_NAK, "Cannot open file");
    	return -1;
          }
          sprintf(result, "%ld", (long)file);
          control->ReturnVal(API_ACK, result);
          return 1;
        }
    }
    return ret_value;
}
int
DeviseCommand_setViewStatistics::Run(int argc, char** argv)
{
    int ret_value;
    ret_value=DeviseCommand::Run(argc, argv);
    if (ret_value)
    {
        {
          ViewGraph *vg = (ViewGraph *)classDir->FindInstance(argv[1]);
          if (!vg) {
    	control->ReturnVal(API_NAK, "Cannot find view");
    	return -1;
          }
          /* Turn on/off display of statistics */
          vg->SetDisplayStats(argv[2]);
          control->ReturnVal(API_ACK, "done");
          return 1;
        }
    }
    return ret_value;
}
int
DeviseCommand_setViewDimensions::Run(int argc, char** argv)
{
    int ret_value;
    ret_value=DeviseCommand::Run(argc, argv);
    if (ret_value)
    {
        {
          View *vg = (View *)classDir->FindInstance(argv[1]);
          if (!vg) {
    	control->ReturnVal(API_NAK, "Cannot find view");
    	return -1;
          }
          /* Turn on/off display of statistics */
          vg->SetNumDimensions(atoi(argv[2]));
          control->ReturnVal(API_ACK, "done");
          return 1;
        }
    }
    return ret_value;
}
int
DeviseCommand_setViewSolid3D::Run(int argc, char** argv)
{
    int ret_value;
    ret_value=DeviseCommand::Run(argc, argv);
    if (ret_value)
    {
        {
          View *vg = (View *)classDir->FindInstance(argv[1]);
          if (!vg) {
    	control->ReturnVal(API_NAK, "Cannot find view");
    	return -1;
          }
          /* Set solid or wireframe 3D objects */
          vg->SetSolid3D(atoi(argv[2]));
          control->ReturnVal(API_ACK, "done");
          return 1;
        }
    }
    return ret_value;
}
int
DeviseCommand_setViewXYZoom::Run(int argc, char** argv)
{
    int ret_value;
    ret_value=DeviseCommand::Run(argc, argv);
    if (ret_value)
    {
        {
          View *vg = (View *)classDir->FindInstance(argv[1]);
          if (!vg) {
    	control->ReturnVal(API_NAK, "Cannot find view");
    	return -1;
          }
          /* Set XY or X/Y zoom */
          vg->SetXYZoom(atoi(argv[2]) ? true : false);
          control->ReturnVal(API_ACK, "done");
          return 1;
        }
    }
    return ret_value;
}
int
DeviseCommand_setViewDisplayDataValues::Run(int argc, char** argv)
{
    int ret_value;
    ret_value=DeviseCommand::Run(argc, argv);
    if (ret_value)
    {
        {
          View *vg = (View *)classDir->FindInstance(argv[1]);
          if (!vg) {
    	control->ReturnVal(API_NAK, "Cannot find view");
    	return -1;
          }
          /* Set data value display on/off */
          vg->SetDisplayDataValues(atoi(argv[2]) ? true : false);
          control->ReturnVal(API_ACK, "done");
          return 1;
        }
    }
    return ret_value;
}
int
DeviseCommand_setViewPileMode::Run(int argc, char** argv)
{
    int ret_value;
    ret_value=DeviseCommand::Run(argc, argv);
    if (ret_value)
    {
        {
          View *vg = (View *)classDir->FindInstance(argv[1]);
          if (!vg) {
    	control->ReturnVal(API_NAK, "Cannot find view");
    	return -1;
          }
          /* Set pile mode flag */
          vg->SetPileMode(atoi(argv[2]) ? true : false);
          control->ReturnVal(API_ACK, "done");
          return 1;
        }
    }
    return ret_value;
}
int
DeviseCommand_savePixmap::Run(int argc, char** argv)
{
    int ret_value;
    ret_value=DeviseCommand::Run(argc, argv);
    if (ret_value)
    {
        {
          View *vg = (View *)classDir->FindInstance(argv[1]);
          if (!vg) {
    	control->ReturnVal(API_NAK, "Cannot find view");
    	return -1;
          }
          FILE *file = (FILE *)atol(argv[2]);
          vg->SavePixmaps(file);
          control->ReturnVal(API_ACK, "done");
          return 1;
        }
    }
    return ret_value;
}
int
DeviseCommand_loadPixmap::Run(int argc, char** argv)
{
    int ret_value;
    ret_value=DeviseCommand::Run(argc, argv);
    if (ret_value)
    {
        {
          View *vg = (View *)classDir->FindInstance(argv[1]);
          if (!vg) {
    	control->ReturnVal(API_NAK, "Cannot find view");
    	return -1;
          }
          FILE *file = (FILE *)atol(argv[2]);
          vg->LoadPixmaps(file);
          control->ReturnVal(API_ACK, "done");
          return 1;
        }
    }
    return ret_value;
}
int
DeviseCommand_getAxisDisplay::Run(int argc, char** argv)
{
    int ret_value;
    ret_value=DeviseCommand::Run(argc, argv);
    if (ret_value)
    {
        {
          View *vg = (View *)classDir->FindInstance(argv[1]);
          if (!vg) {
    	control->ReturnVal(API_NAK, "Cannot find view");
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
          control->ReturnVal(API_ACK, result);
          return 1;
        }
    }
    return ret_value;
}
int
DeviseCommand_replaceView::Run(int argc, char** argv)
{
    int ret_value;
    ret_value=DeviseCommand::Run(argc, argv);
    if (ret_value)
    {
        {
          View *view1 = (View *)classDir->FindInstance(argv[1]);
          View *view2 = (View *)classDir->FindInstance(argv[2]);
          if (!view1 || !view2) {
    	control->ReturnVal(API_NAK, "Cannot find view");
    	return -1;
          }
          ViewWin *win = view1->GetParent();
          if (!win) {
    	control->ReturnVal(API_NAK, "Cannot find window");
    	return -1;
          }
          win->Replace(view1, view2);
          control->ReturnVal(API_ACK, "done");
          return 1;
        }
    }
    return ret_value;
}
int
DeviseCommand_setCursorSrc::Run(int argc, char** argv)
{
    int ret_value;
    ret_value=DeviseCommand::Run(argc, argv);
    if (ret_value)
    {
        {
          DeviseCursor *cursor = (DeviseCursor *)
    	classDir->FindInstance(argv[1]);
          if (!cursor) {
    	control->ReturnVal(API_NAK, "Cannot find cursor");
    	return -1;
          }
          View *view = (View *)classDir->FindInstance(argv[2]);
          if (!view) {
    	control->ReturnVal(API_NAK, "Cannot find view");
    	return -1;
          }
          cursor->SetSource(view);
          control->ReturnVal(API_ACK, "done");
          return 1;
        }
    }
    return ret_value;
}
int
DeviseCommand_setCursorDst::Run(int argc, char** argv)
{
    int ret_value;
    ret_value=DeviseCommand::Run(argc, argv);
    if (ret_value)
    {
        {
          DeviseCursor *cursor = (DeviseCursor *)
    	classDir->FindInstance(argv[1]);
          if (!cursor) {
    	control->ReturnVal(API_NAK, "Cannot find cursor");
    	return -1;
          }
          View *view = (View *)classDir->FindInstance(argv[2]);
          if (!view) {
    	control->ReturnVal(API_NAK, "Cannot find view");
    	return -1;
          }
          cursor->SetDst(view);
          control->ReturnVal(API_ACK, "done");
          return 1;
        } 
    }
    return ret_value;
}
int
DeviseCommand_setPixelWidth::Run(int argc, char** argv)
{
    int ret_value;
    ret_value=DeviseCommand::Run(argc, argv);
    if (ret_value)
    {
        {
          TDataMap *map= (TDataMap *)classDir->FindInstance(argv[1]);
          if (!map) {
    	control->ReturnVal(API_NAK, "Cannot find mapping");
    	return -1;
          }
          int width = atoi(argv[2]);
          if (width > 0 )
    	map->SetPixelWidth(width);
          control->ReturnVal(API_ACK, "done");
          return 1;
        }
    }
    return ret_value;
}
int
DeviseCommand_getAxis::Run(int argc, char** argv)
{
    int ret_value;
    ret_value=DeviseCommand::Run(argc, argv);
    if (ret_value)
    {
        {
          View *view = (View *)classDir->FindInstance(argv[1]);
          if (!view) {
    	control->ReturnVal(API_NAK, "Cannot find view");
    	return -1;
          }
          AxisLabel *label;
          if (!strcmp(argv[2], "x"))
    	label = view->GetXAxisLabel();
          else
    	label = view->GetYAxisLabel();
          
          if (!label)
    	control->ReturnVal(API_ACK, "");
          else
    	control->ReturnVal(API_ACK, label->GetName());
          return 1;
        }
    }
    return ret_value;
}
int
DeviseCommand_setAction::Run(int argc, char** argv)
{
    int ret_value;
    ret_value=DeviseCommand::Run(argc, argv);
    if (ret_value)
    {
        {
          ViewGraph *view = (ViewGraph *)classDir->FindInstance(argv[1]);
          if (!view) {
    	control->ReturnVal(API_NAK, "Cannot find view");
    	return -1;
          }
          Action *action = (Action*)classDir->FindInstance(argv[2]);
          if (!action) {
    	control->ReturnVal(API_NAK, "Cannot find action");
    	return -1;
          }
          view->SetAction(action);
          control->ReturnVal(API_ACK, "done");
          return 1;
        }
    }
    return ret_value;
}
int
DeviseCommand_setLinkFlag::Run(int argc, char** argv)
{
    int ret_value;
    ret_value=DeviseCommand::Run(argc, argv);
    if (ret_value)
    {
        {
          DeviseLink *link = (DeviseLink *)classDir->FindInstance(argv[1]);
          if (!link) {
    	control->ReturnVal(API_NAK, "Cannot find link");
    	return -1;
          }
          VisualFlag flag = atoi(argv[2]);
          link->SetFlag(flag);
          control->ReturnVal(API_ACK, "done");
          return 1;
        }
    }
    return ret_value;
}
int
DeviseCommand_highlightView::Run(int argc, char** argv)
{
    int ret_value;
    ret_value=DeviseCommand::Run(argc, argv);
    if (ret_value)
    {
        {
          View *view = (View *)classDir->FindInstance(argv[1]);
          if (!view) {
    	control->ReturnVal(API_NAK, "Cannot find view");
    	return -1;
          }
          view->Highlight(atoi(argv[2]));
          control->ReturnVal(API_ACK, "done");
          return 1;
        }
    }
    return ret_value;
}
int
DeviseCommand_getparam::Run(int argc, char** argv)
{
    int ret_value;
    ret_value=DeviseCommand::Run(argc, argv);
    if (ret_value)
    {
        {
          classDir->GetParams(argv[1], argv[2], numArgs, args);
          control->ReturnVal(numArgs, args);
          return 1;
        }
    }
    return ret_value;
}
int
DeviseCommand_insertViewInLens::Run(int argc, char** argv)
{
    int ret_value;
    ret_value=DeviseCommand::Run(argc, argv);
    if (ret_value)
    {
        {
          ViewLens *lens = (ViewLens *)classDir->FindInstance(argv[1]);
          if (!lens) {
    	control->ReturnVal(API_NAK, "Cannot find lens");
    	return -1;
          }
          ViewGraph *view = (ViewGraph *)classDir->FindInstance(argv[2]);
          if (!view){
    	control->ReturnVal(API_NAK, "Cannot find view");
    	return -1;
          }
          lens->InsertView(view);
          control->ReturnVal(API_ACK, "done");
          return 1;
        }
    }
    return ret_value;
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
    int ret_value;
    ret_value=DeviseCommand::Run(argc, argv);
    if (ret_value)
    {
        {
          ViewGraph *view = (ViewGraph *)classDir->FindInstance(argv[1]);
          if (!view) {
    	control->ReturnVal(API_NAK, "Cannot find view");
    	return -1;
          }
          TDataMap *map = (TDataMap *)classDir->FindInstance(argv[2]);
          if (!map) {
    	control->ReturnVal(API_NAK, "Cannot find mapping");
    	return -1;
          }
          view->InsertMapping(map);
          control->ReturnVal(API_ACK, "done");
          return 1;
        }
    }
    return ret_value;
}
int
DeviseCommand_insertMapping::Run_4(int argc, char** argv)
{
    int ret_value;
    ret_value=DeviseCommand::Run(argc, argv);
    if (ret_value)
    {
        {
          ViewGraph *view = (ViewGraph *)classDir->FindInstance(argv[1]);
          if (!view) {
    	control->ReturnVal(API_NAK, "Cannot find view");
    	return -1;
          }
          TDataMap *map = (TDataMap *)classDir->FindInstance(argv[2]);
          if (!map) {
    	control->ReturnVal(API_NAK, "Cannot find mapping");
    	return -1;
          }
          view->InsertMapping(map, argv[3]);
          control->ReturnVal(API_ACK, "done");
          return 1;
        }
    }
    return ret_value;
}

int
DeviseCommand_getMappingLegend::Run(int argc, char** argv)
{
    int ret_value;
    ret_value=DeviseCommand::Run(argc, argv);
    if (ret_value)
    {
        {
          ViewGraph *view = (ViewGraph *)classDir->FindInstance(argv[1]);
          if (!view) {
    	control->ReturnVal(API_NAK, "Cannot find view");
    	return -1;
          }
          TDataMap *map = (TDataMap *)classDir->FindInstance(argv[2]);
          if (!map) {
    	control->ReturnVal(API_NAK, "Cannot find mapping");
    	return -1;
          }
          control->ReturnVal(API_ACK, view->GetMappingLegend(map));
          return 1;
        }
    }
    return ret_value;
}
int
DeviseCommand_insertLink::Run(int argc, char** argv)
{
    int ret_value;
    ret_value=DeviseCommand::Run(argc, argv);
    if (ret_value)
    {
        {
          DeviseLink *link = (DeviseLink *)classDir->FindInstance(argv[1]);
          if (!link) {
    	control->ReturnVal(API_NAK, "Cannot find link");
    	return -1;
          }
    //      printf("insertLink %s %s\n", argv[1], argv[2]);
          ViewGraph *view = (ViewGraph *)classDir->FindInstance(argv[2]);
          if (!view) {
    	control->ReturnVal(API_NAK, "Cannot find view");
    	return -1;
          }
          link->InsertView(view);
          control->ReturnVal(API_ACK, "done");
          return 1;
        }
    }
    return ret_value;
}
int
DeviseCommand_viewInLink::Run(int argc, char** argv)
{
    int ret_value;
    ret_value=DeviseCommand::Run(argc, argv);
    if (ret_value)
    {
        {
          DeviseLink *link = (DeviseLink *)classDir->FindInstance(argv[1]);
          if (!link) {
    	control->ReturnVal(API_NAK, "Cannot find link");
    	return -1;
          }
          ViewGraph *view = (ViewGraph *)classDir->FindInstance(argv[2]);
          if (!view) {
    	control->ReturnVal(API_NAK, "Cannot find view");
    	return -1;
          }
          if (link->ViewInLink(view))
    	strcpy(result, "1");
          else
    	strcpy(result , "0");
          control->ReturnVal(API_ACK, result);
          return 1;
        }
    }
    return ret_value;
}
int
DeviseCommand_unlinkView::Run(int argc, char** argv)
{
    int ret_value;
    ret_value=DeviseCommand::Run(argc, argv);
    if (ret_value)
    {
        {
          DeviseLink *link = (DeviseLink *)classDir->FindInstance(argv[1]);
          if (!link) {
    	control->ReturnVal(API_NAK, "Cannot find link");
    	return -1;
          }
          printf("unLink %s %s\n", argv[1], argv[2]);
          ViewGraph *view = (ViewGraph *)classDir->FindInstance(argv[2]);
          if (!view) {
    	control->ReturnVal(API_NAK, "Cannot find view");
    	return -1;
          }
          link->DeleteView(view);
          control->ReturnVal(API_ACK, "done");
          return 1;
        }
    }
    return ret_value;
}
int
DeviseCommand_insertWindow::Run(int argc, char** argv)
{
    int ret_value;
    ret_value=DeviseCommand::Run(argc, argv);
    if (ret_value)
    {
        {
          ViewGraph *view = (ViewGraph *)classDir->FindInstance(argv[1]);
          if (!view) {
    	control->ReturnVal(API_NAK, "Cannot find view");
    	return -1;
          }
          ViewWin *win = (ViewWin *)classDir->FindInstance(argv[2]);
          if (!win) {
    	control->ReturnVal(API_NAK, "Cannot find window");
    	return -1;
          }
          view->DeleteFromParent();
          view->AppendToParent(win);
          control->ReturnVal(API_ACK, "done");
          return 1;
        }
    }
    return ret_value;
}
int
DeviseCommand_removeMapping::Run(int argc, char** argv)
{
    int ret_value;
    ret_value=DeviseCommand::Run(argc, argv);
    if (ret_value)
    {
        {
          ViewGraph *view = (ViewGraph *)classDir->FindInstance(argv[1]);
          if (!view) {
    	control->ReturnVal(API_NAK, "Cannot find view");
    	return -1;
          }
          TDataMap *map = (TDataMap *)classDir->FindInstance(argv[2]);
          if (!map) {
    	control->ReturnVal(API_NAK, "Cannot find mapping");
    	return -1;
          }
          view->RemoveMapping(map);
          control->ReturnVal(API_ACK, "done");
          return 1;
        }
    }
    return ret_value;
}
int
DeviseCommand_saveDisplayImage::Run(int argc, char** argv)
{
    int ret_value;
    ret_value=DeviseCommand::Run(argc, argv);
    if (ret_value)
    {
        {
          DisplayExportFormat format = POSTSCRIPT;
          if (!strcmp(argv[1], "eps"))
    	format = EPS;
          else if (!strcmp(argv[1], "gif"))
    	format = GIF;
          DeviseDisplay::DefaultDisplay()->ExportImage(format, argv[2]);
          control->ReturnVal(API_ACK, "done");
          return 1;
        }
    }
    return ret_value;
}
int
DeviseCommand_saveDisplayView::Run(int argc, char** argv)
{
    int ret_value;
    ret_value=DeviseCommand::Run(argc, argv);
    if (ret_value)
    {
        {
          DisplayExportFormat format = POSTSCRIPT;
          if (!strcmp(argv[1], "eps"))
    	 format = EPS;
          else if (!strcmp(argv[1], "gif"))
    	 format = GIF;
          DeviseDisplay::DefaultDisplay()->ExportView(format, argv[2]);
          control->ReturnVal(API_ACK, "done");
          return 1;
        }
    }
    return ret_value;
}
int
DeviseCommand_saveTdata::Run(int argc, char** argv)
{
    int ret_value;
    ret_value=DeviseCommand::Run(argc, argv);
    if (ret_value)
    {
        {
          TData *tdata = (TData *)classDir->FindInstance(argv[1]);
          if (!tdata) {
    	control->ReturnVal(API_NAK, "Cannot find tdata");
    	return -1;
          }
          if (!(tdata->Save(argv[2]) == StatusFailed)) {
            control->ReturnVal(API_ACK, "done");
            return 1;
          } else {
    	control->ReturnVal(API_NAK, "Cannot save tdata");
    	return -1;
          }
        }
    }
    return ret_value;
}
int
DeviseCommand_getDisplayImage::Run(int argc, char** argv)
{
    int ret_value;
    ret_value=DeviseCommand::Run(argc, argv);
    if (ret_value)
    {
        {
          // Arguments: <port number> <image type>
          // Returns: "done"
          int port = atoi(argv[1]);
          if (strcmp(argv[2], "gif")) {
             control->ReturnVal(API_NAK, "Can only support GIF now.");
             return -1;
          }
          control->OpenDataChannel(port);
          int fd = control->getFd();
          if (fd < 0) {
    	reportErrSys("Cannot open data channel");
            control->ReturnVal(API_NAK, "Invalid socket to write");
            return -1;
          }
          FILE *fp = fdopen(control->getFd(), "wb");
          Timer::StopTimer();
          DeviseDisplay::DefaultDisplay()->ExportGIF(fp, 0);
          Timer::StartTimer();
          close(control->getFd());
          control->ReturnVal(API_ACK, "done");
          return 1;
        }
    }
    return ret_value;
}
int
DeviseCommand_getDisplayImageAndSize::Run(int argc, char** argv)
{
    int ret_value;
    ret_value=DeviseCommand::Run(argc, argv);
    if (ret_value)
    {
        {
          // Arguments: <port number> <image type>
          // Returns: "done"
          return GetDisplayImageAndSize(control, atoi(argv[1]), argv[2]);
        }
    }
    return ret_value;
}
int
DeviseCommand_getFont::Run(int argc, char** argv)
{
    int ret_value;
    ret_value=DeviseCommand::Run(argc, argv);
    if (ret_value)
    {
        {
          View *view = View::FindViewByName(argv[1]);
          if (view == NULL) {
            control->ReturnVal(API_NAK, "Cannot find view");
            return -1;
          }
    
          int family;
          float pointSize;
          Boolean bold;
          Boolean italic;
          view->GetFont(argv[2], family, pointSize, bold, italic);
          sprintf(result, "%d %d %d %d", family, (int) pointSize, bold, italic);
    
          control->ReturnVal(API_ACK, result);
          return 1;
        }
    }
    return ret_value;
}
int
DeviseCommand_viewSetAlign::Run(int argc, char** argv)
{
    int ret_value;
    ret_value=DeviseCommand::Run(argc, argv);
    if (ret_value)
    {
        {
          // Argument: <view name> <alignment value>
          // Returns: "done"
    #if defined(DEBUG)
          printf("viewSetAlign <%s> <%s>\n", argv[1], argv[2]);
    #endif
          ViewGraph *view = (ViewGraph *)classDir->FindInstance(argv[1]);
          if (!view) {
    	    control->ReturnVal(API_NAK, "Cannot find view");
    	    return -1;
          }
          int newAlign = atoi(argv[2]);
          if (view->GetAlign() != newAlign) {
            view->SetAlign(newAlign);
    	view->Refresh();
          }
          control->ReturnVal(API_ACK, "done");
          return 1;
        }
    }
    return ret_value;
}
int
DeviseCommand_checkTDataForRecLink::Run(int argc, char** argv)
{
    int ret_value;
    ret_value=DeviseCommand::Run(argc, argv);
    if (ret_value)
    {
        {
          DeviseLink *link = (DeviseLink *)classDir->FindInstance(argv[1]);
          if (!link) {
    	    control->ReturnVal(API_NAK, "Cannot find link");
    	    return -1;
          } 
          ViewGraph *view = (ViewGraph *)classDir->FindInstance(argv[2]);
          if (!view) {
    	    control->ReturnVal(API_NAK, "Cannot find view");
    	    return -1;
          }

	  if (link->GetFlag() & VISUAL_RECORD) {
	    RecordLink *recLink = (RecordLink *)link;
            sprintf(result, "%d",
	      (recLink->CheckTData(view, atoi(argv[3]))) ?  1 : 0);
            control->ReturnVal(API_ACK, result);
            return 1;
	  } else {
            control->ReturnVal(API_ACK, "1");
            return 1;
	  }
        }      
    }
    return ret_value;
}
int
DeviseCommand_setMappingLegend::Run(int argc, char** argv)
{
    int ret_value;
    ret_value=DeviseCommand::Run(argc, argv);
    if (ret_value)
    {
        {
          ViewGraph *view = (ViewGraph *)classDir->FindInstance(argv[1]);
          if (!view) {
    	control->ReturnVal(API_NAK, "Cannot find view");
    	return -1;
          }
          TDataMap *map = (TDataMap *)classDir->FindInstance(argv[2]);
          if (!map) {
    	control->ReturnVal(API_NAK, "Cannot find mapping");
    	return -1;
          }
          view->SetMappingLegend(map, argv[3]);
          control->ReturnVal(API_ACK, "done");
          return 1;
        }
    }
    return ret_value;
}
int
DeviseCommand_markViewFilter::Run(int argc, char** argv)
{
    int ret_value;
    ret_value=DeviseCommand::Run(argc, argv);
    if (ret_value)
    {
        {
          View *view = (View *)classDir->FindInstance(argv[1]);
          if (!view) {
    	control->ReturnVal(API_NAK, "Cannot find view");
    	return -1;
          }
          int index = atoi(argv[2]);
          Boolean mark = atoi(argv[3]);
          view->Mark(index, mark);
          control->ReturnVal(API_ACK, "done");
          return 1;
        }
    }
    return ret_value;
}
int
DeviseCommand_setAxis::Run(int argc, char** argv)
{
    int ret_value;
    ret_value=DeviseCommand::Run(argc, argv);
    if (ret_value)
    {
        {
          View *view = (View *)classDir->FindInstance(argv[1]);
          if (!view) {
    	control->ReturnVal(API_NAK, "Cannot find view");
    	return -1;
          }
          AxisLabel *label = (AxisLabel *)classDir->FindInstance(argv[2]);
          if (!label) {
    	control->ReturnVal(API_NAK, "Cannot find label");
    	return -1;
          }
          if (!strcmp(argv[3], "x"))
    	view->SetXAxisLabel(label);
          else
    	view->SetYAxisLabel(label);
          control->ReturnVal(API_ACK, "done");
          return 1;
        }
    }
    return ret_value;
}
int
DeviseCommand_getWindowImage::Run(int argc, char** argv)
{
    int ret_value;
    ret_value=DeviseCommand::Run(argc, argv);
    if (ret_value)
    {
        {
          // Arguments: <port number> <image type> <window name>
          // Returns: "done"
          int port = atoi(argv[1]);
          if (strcmp(argv[2], "gif")) {
             control->ReturnVal(API_NAK, "Can only support GIF now.");
             return -1;
          }
          ViewWin *viewWin = (ViewWin *)classDir->FindInstance(argv[3]);
          if (!viewWin) {
            control->ReturnVal(API_NAK, "Cannot find window");
            return -1;
          }
    
          control->OpenDataChannel(port);
          int fd = control->getFd();
          if (fd < 0) {
    	reportErrSys("Cannot open data channel");
            control->ReturnVal(API_NAK, "Invalid socket to write");
            return -1;
          }
          FILE *fp = fdopen(control->getFd(), "wb");
          Timer::StopTimer();
          viewWin->GetWindowRep()->ExportGIF(fp);
          Timer::StartTimer();
          close(control->getFd());
          control->ReturnVal(API_ACK, "done");
          return 1;
        }
    }
    return ret_value;
}
int
DeviseCommand_getWindowImageAndSize::Run(int argc, char** argv)
{
    int ret_value;
    ret_value=DeviseCommand::Run(argc, argv);
    if (ret_value)
    {
        {
          // Arguments: <port number> <image type> <window name>
          // Returns: "done"
          return GetWindowImageAndSize(control, atoi(argv[1]), argv[2], argv[3]);
        }
    }
    return ret_value;
}
int
DeviseCommand_swapView::Run(int argc, char** argv)
{
    int ret_value;
    ret_value=DeviseCommand::Run(argc, argv);
    if (ret_value)
    {
        {
          ViewWin *viewWin = (ViewWin *)classDir->FindInstance(argv[1]);
          View *view1 = (View *)classDir->FindInstance(argv[2]);
          View *view2 = (View *)classDir->FindInstance(argv[3]);
          if (!viewWin || !view1 || !view2) {
    	control->ReturnVal(API_NAK, "Cannot find view or window");
    	return -1;
          }
          if (view1->GetParent() != viewWin || view2->GetParent() != viewWin) {
    	control->ReturnVal(API_NAK, "Views not in same window");
    	return -1;
          }
          viewWin->SwapChildren(view1, view2);
          control->ReturnVal(API_ACK, "done");
          return 1;
        }
    }
    return ret_value;
}
int
DeviseCommand_setAxisDisplay::Run(int argc, char** argv)
{
    int ret_value;
    ret_value=DeviseCommand::Run(argc, argv);
    if (ret_value)
    {
        {
          View *vg = (View *)classDir->FindInstance(argv[1]);
          if (!vg) {
    	control->ReturnVal(API_NAK, "Cannot find view");
    	return -1;
          }
          Boolean stat = atoi(argv[3]);
          if (!strcmp(argv[2], "X"))
    	vg->XAxisDisplayOnOff(stat);
          else
    	vg->YAxisDisplayOnOff(stat);
          control->ReturnVal(API_ACK, "done");
          return 1;
        }
    }
    return ret_value;
}
int
DeviseCommand_getCreateParam::Run(int argc, char** argv)
{
    int ret_value;
    ret_value=DeviseCommand::Run(argc, argv);
    if (ret_value)
    {
        {
          classDir->CreateParams(argv[1], argv[2], argv[3], numArgs, args);
          control->ReturnVal(numArgs, args);
          return 1;
        }
    }
    return ret_value;
}
int
DeviseCommand_getItems::Run(int argc, char** argv)
{
    int ret_value;
    ret_value=DeviseCommand::Run(argc, argv);
    if (ret_value)
    {
        {
          control->GetGroupDir()->get_items(result, argv[1], argv[2], argv[3]);
          control->ReturnVal(API_ACK, result);
          return 1;
        }
    }
    return ret_value;
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
    int ret_value;
    ret_value=DeviseCommand::Run(argc, argv);
    if (ret_value)
    {
        {
          ViewLayout *layout = (ViewLayout *)classDir->FindInstance(argv[1]);
          if (!layout) {
    	control->ReturnVal(API_NAK, "Cannot find window");
    	return -1;
          }
          layout->SetPreferredLayout(atoi(argv[2]), atoi(argv[3]));
          control->ReturnVal(API_ACK, "done");
          return 1;
        }
    }
    return ret_value;
}
int
DeviseCommand_setWindowLayout::Run_5(int argc, char** argv)
{
    int ret_value;
    ret_value=DeviseCommand::Run(argc, argv);
    if (ret_value)
    {
        {
          ViewLayout *layout = (ViewLayout *)classDir->FindInstance(argv[1]);
          if (!layout) {
    	control->ReturnVal(API_NAK, "Cannot find window");
    	return -1;
          }
          layout->SetPreferredLayout(atoi(argv[2]), atoi(argv[3]),
    				 (atoi(argv[4]) ? true : false));
          control->ReturnVal(API_ACK, "done");
          return 1;
        }
    }
    return ret_value;
}
int
DeviseCommand_saveWindowImage::Run(int argc, char** argv)
{
    int ret_value;
    ret_value=DeviseCommand::Run(argc, argv);
    if (ret_value)
    {
        {
          DisplayExportFormat format = POSTSCRIPT;
          if (!strcmp(argv[1], "eps"))
    	format = EPS;
          else if (!strcmp(argv[1], "gif"))
    	format = GIF;
          ViewWin *viewWin = (ViewWin *)classDir->FindInstance(argv[2]);
          if (!viewWin) {
    	control->ReturnVal(API_NAK, "Cannot find window");
    	return -1;
          }
          viewWin->ExportImage(format, argv[3]);
    
          control->ReturnVal(API_ACK, "done");
          return 1;
        }
    }
    return ret_value;
}
int
DeviseCommand_setViewOverrideColor::Run(int argc, char** argv)
{
    int ret_value;
    ret_value=DeviseCommand::Run(argc, argv);
    if (ret_value)
    {
        {
    		control->ReturnVal(API_ACK, "done");
    		return 1;
        }
    }
    return ret_value;
}
int
DeviseCommand_parseSchema::Run(int argc, char** argv)
{
    int ret_value;
    ret_value=DeviseCommand::Run(argc, argv);
    if (ret_value)
    {
        {
          char *name = ParseSchema(argv[1], argv[2], argv[3]);
          if (!name) {
    	control->ReturnVal(API_NAK, "Cannot parse schema(s)");
    	return -1;
          }
          control->ReturnVal(API_ACK, name);
          return 1;
        }
    }
    return ret_value;
}
int
DeviseCommand_winSetPrint::Run(int argc, char** argv)
{
    int ret_value;
    ret_value=DeviseCommand::Run(argc, argv);
    if (ret_value)
    {
        {
    #if defined(DEBUG)
          printf("winSetPrint %s %s %s\n", argv[1], argv[2], argv[3]);
    #endif
          ViewWin *win = (ViewWin *)classDir->FindInstance(argv[1]);
          if (!win) {
            control->ReturnVal(API_NAK, "Cannot find window");
            return -1;
          }
          Boolean exclude = atoi(argv[2]);
          Boolean pixmap = atoi(argv[3]);
          win->SetPrintExclude(exclude);
          win->SetPrintPixmap(pixmap);
          control->ReturnVal(API_ACK, "done");
          return 1;
        }
    }
    return ret_value;
}
int
DeviseCommand_setLabel::Run(int argc, char** argv)
{
    int ret_value;
    ret_value=DeviseCommand::Run(argc, argv);
    if (ret_value)
    {
        {
          View *view = (View *)classDir->FindInstance(argv[1]);
          if (!view) {
    	control->ReturnVal(API_NAK, "Cannot find view");
    	return -1;
          }
          view->SetLabelParam(atoi(argv[2]), atoi(argv[3]), argv[4]);
          control->ReturnVal(API_ACK, "done");
          return 1;
        }
    }
    return ret_value;
}
int
DeviseCommand_dataSegment::Run(int argc, char** argv)
{
    int ret_value;
    ret_value=DeviseCommand::Run(argc, argv);
    if (ret_value)
    {
        {
          DataSeg::Set(argv[1], argv[2], atoi(argv[3]), atoi(argv[4]));
          control->ReturnVal(API_ACK, "done");
          return 1;
        }
    }
    return ret_value;
}
int
DeviseCommand_viewSetHorPan::Run(int argc, char** argv)
{
    int ret_value;
    ret_value=DeviseCommand::Run(argc, argv);
    if (ret_value)
    {
        {
          // Arguments: <viewName> <mode> <relativePan> <absolutePan>
    #if defined(DEBUG)
          printf("viewSetHorPan <%s> <%s> <%s> <%s>\n", argv[1], argv[2], argv[3],
    	argv[4]);
    #endif
          ViewGraph *view = (ViewGraph *)classDir->FindInstance(argv[1]);
          if (!view) {
    	control->ReturnVal(API_NAK, "Cannot find view");
    	return -1;
          }
          ViewPanInfo info;
          info.mode = (ViewPanMode) atoi(argv[2]);
          info.relPan = atof(argv[3]);
          info.absPan = atof(argv[4]);
          view->SetHorPanInfo(info);
          control->ReturnVal(API_ACK, "done");
          return 1;
        }
    }
    return ret_value;
}
int
DeviseCommand_setCursorGrid::Run(int argc, char** argv)
{
    int ret_value;
    ret_value=DeviseCommand::Run(argc, argv);
    if (ret_value)
    {
        {
          // Arguments: <cursorName> <useGrid> <gridX> <gridY>
    #if defined(DEBUG)
          printf("setCursorGrid <%s>\n", argv[1]);
    #endif
          DeviseCursor *cursor = (DeviseCursor *) classDir->FindInstance(argv[1]);
          if (!cursor) {
    	control->ReturnVal(API_NAK, "Cannot find cursor");
    	return -1;
          }
          Boolean useGrid = atoi(argv[2]);
          Coord gridX = atof(argv[3]);
          Coord gridY = atof(argv[4]);
          cursor->SetGrid(useGrid, gridX, gridY);
          control->ReturnVal(API_ACK, "done");
          return 1;
        }
    }
    return ret_value;
}
int
DeviseCommand_saveSession::Run(int argc, char** argv)
{
    int ret_value;
    ret_value=DeviseCommand::Run(argc, argv);
    if (ret_value)
    {
        {
          // Arguments: <file name> <as template> <as export> <with data>
    #if defined(DEBUG)
          printf("saveSession <%s> <%s> <%s> <%s>\n", argv[1], argv[2], argv[3],
    	argv[4]);
    #endif
          if (!Session::Save(argv[1], atoi(argv[2]), atoi(argv[3]),
    	  atoi(argv[4])).IsComplete()) {
    	control->ReturnVal(API_NAK, "Unable to save session file");
    	return -1;
          }
          control->ReturnVal(API_ACK, "done");
          return 1;
        }
    }
    return ret_value;
}
int
DeviseCommand_setWinGeometry::Run(int argc, char** argv)
{
    int ret_value;
    ret_value=DeviseCommand::Run(argc, argv);
    if (ret_value)
    {
        {
          View *view = (View *)classDir->FindInstance(argv[1]);
          if (!view) {
    	control->ReturnVal(API_NAK, "Cannot find view");
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
          control->ReturnVal(API_ACK, "done");
          return 1;
        }  
    }
    return ret_value;
}
int
DeviseCommand_setFilter::Run(int argc, char** argv)
{
    int ret_value;
    ret_value=DeviseCommand::Run(argc, argv);
    if (ret_value)
    {
        {
          View *view = (View *)classDir->FindInstance(argv[1]);
          if (!view) {
    	control->ReturnVal(API_NAK, "Cannot find view");
    	return -1;
          }
          VisualFilter filter;
          view->GetVisualFilter(filter);
          if (!ParseFloatDate(argv[2], filter.xLow) ||
    	  !ParseFloatDate(argv[3], filter.yLow) ||
    	  !ParseFloatDate(argv[4], filter.xHigh) ||
    	  !ParseFloatDate(argv[5], filter.yHigh)) {
    	control->ReturnVal(API_NAK, "invalid date or float");
    	return -1;
          }
          view->SetVisualFilter(filter);
          control->ReturnVal(API_ACK, "done");
          return 1;
        }
    }
    return ret_value;
}
int
DeviseCommand_saveDisplayImageAndMap::Run(int argc, char** argv)
{
    int ret_value;
    ret_value=DeviseCommand::Run(argc, argv);
    if (ret_value)
    {
        {
          DisplayExportFormat format = POSTSCRIPT;
          if (!strcmp(argv[1], "eps"))
            format = EPS;
          else if (!strcmp(argv[1], "gif"))
            format = GIF;
          DeviseDisplay::DefaultDisplay()->ExportImageAndMap(format, argv[2], 
    					argv[3], argv[4], argv[5]);
          control->ReturnVal(API_ACK, "done");
          return 1;
        }
    }
    return ret_value;
}
int
DeviseCommand_insertViewHistory::Run(int argc, char** argv)
{
    int ret_value;
    ret_value=DeviseCommand::Run(argc, argv);
    if (ret_value)
    {
        {
          View *view = (View *)classDir->FindInstance(argv[1]);
          if (!view) {
    	control->ReturnVal(API_NAK, "Cannot find view");
    	return -1;
          }
          VisualFilter filter;
          (void)ParseFloatDate(argv[2],filter.xLow); 
          (void)ParseFloatDate(argv[3],filter.yLow);
          (void)ParseFloatDate(argv[4],filter.xHigh); 
          (void)ParseFloatDate(argv[5],filter.yHigh);
          filter.marked = atoi(argv[6]);
          view->InsertHistory(filter);
          control->ReturnVal(API_ACK, "done");
          return 1;
        }
    }
    return ret_value;
}
int
DeviseCommand_setFont::Run(int argc, char** argv)
{
    int ret_value;
    ret_value=DeviseCommand::Run(argc, argv);
    if (ret_value)
    {
        {
          View *view = View::FindViewByName(argv[1]);
          if (view == NULL) {
            control->ReturnVal(API_NAK, "Cannot find view");
            return -1;
          }
    
          view->SetFont(argv[2], atoi(argv[3]), atof(argv[4]), atoi(argv[5]),
    	atoi(argv[6]));
    
          control->ReturnVal(API_ACK, "done");
          return 1;
        }
    }
    return ret_value;
}
int
DeviseCommand_set3DLocation::Run(int argc, char** argv)
{
    int ret_value;
    ret_value=DeviseCommand::Run(argc, argv);
    if (ret_value)
    {
        {
          ViewGraph *view = (ViewGraph *)classDir->FindInstance(argv[1]);
          if (!view) {
    	control->ReturnVal(API_NAK, "Cannot find view");
    	return -1;
          }
          Camera c = view->GetCamera();
          Boolean isSphere = c.spherical_coord;
          c.spherical_coord = false;
          c.x_ = atof(argv[2]);
          c.y_ = atof(argv[3]);
          c.z_ = atof(argv[4]);
          c.fx = atof(argv[5]);
          c.fy = atof(argv[6]);
          c.fz = atof(argv[7]);
          view->SetCamera(c);
          if (isSphere) {
	    c = view->GetCamera();
	    c.spherical_coord = true;
	    view->SetCamera(c);
          }
          control->ReturnVal(API_ACK, "done");
          return 1;
        }
    }
    return ret_value;
}
int
DeviseCommand_setViewGDS::Run(int argc, char** argv)
{
    int ret_value;
    ret_value=DeviseCommand::Run(argc, argv);
    if (ret_value)
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
    	control->ReturnVal(API_NAK, "Cannot find view");
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
    
          control->ReturnVal(API_ACK, "done");
          return 1;
        }
    }
    return ret_value;
}
int
DeviseCommand_viewSetHome::Run(int argc, char** argv)
{
    int ret_value;
    ret_value=DeviseCommand::Run(argc, argv);
    if (ret_value)
    {
        {
          // Arguments: <viewName> <mode> <autoXMargin> <autoYMargin> <manXLo>
          // <manYLo> <manXHi> <manYHi>
    #if defined(DEBUG)
          printf("viewSetHome <%s> <%s> <%s> <%s> <%s> <%s> <%s> <%s>\n", argv[1],
    	argv[2], argv[3], argv[4], argv[5], argv[6], argv[7], argv[8]);
    #endif
          ViewGraph *view = (ViewGraph *)classDir->FindInstance(argv[1]);
          if (!view) {
    	control->ReturnVal(API_NAK, "Cannot find view");
    	return -1;
          }
          ViewHomeInfo info;
          info.mode = (ViewHomeMode) atoi(argv[2]);
          info.autoXMargin = atof(argv[3]);
          info.autoYMargin = atof(argv[4]);
          info.manXLo = atof(argv[5]);
          info.manYLo = atof(argv[6]);
          info.manXHi = atof(argv[7]);
          info.manYHi = atof(argv[8]);
          view->SetHomeInfo(info);
          control->ReturnVal(API_ACK, "done");
          return 1;
        }
    }
    return ret_value;
}

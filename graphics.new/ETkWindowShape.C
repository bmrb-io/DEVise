/*
  ========================================================================
  DEVise Data Visualization Software
  (c) Copyright 1992-1997
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
  Revision 1.18  1999/05/26 19:50:49  wenger
  Added bounding box info to GData, so that the selection of records by the
  visual filter is more accurate.  (Note that at this time the bounding box
  info does not take into account symbol orientation; symbol alignment is
  taken into account somewhat crudely.) This includes considerable
  reorganization of the mapping-related classes.  Fixed problem with
  pixelSize getting used twice in Rect shape (resulted in size being the
  square of pixel size).

  Revision 1.17  1999/05/21 14:52:24  wenger
  Cleaned up GData-related code in preparation for including bounding box
  info.

  Revision 1.16  1999/05/20 15:17:36  wenger
  Fixed bugs 490 (problem destroying piled parent views) and 491 (problem
  with duplicate elimination and count mappings) exposed by Tim Wilson's
  two-station session.

  Revision 1.15  1998/11/06 17:59:48  wenger
  Multiple string tables fully working -- allows separate tables for the
  axes in a given view.

  Revision 1.14  1998/11/04 20:33:54  wenger
  Multiple string tables partly working -- loading and saving works, one
  table per mapping works; need multiple tables per mapping, API and GUI,
  saving to session, sorting.

  Revision 1.13  1997/11/24 23:14:50  weaver
  Changes for the new ColorManager.

  Revision 1.12  1997/11/18 23:26:49  wenger
  First version of GData to socket capability; removed some extra include
  dependencies; committed test version of TkControl::OpenDataChannel().

  Revision 1.11  1997/08/27 14:47:38  wenger
  Removed some unnecessary include dependencies.

  Revision 1.10  1997/08/20 22:10:55  wenger
  Merged improve_stop_branch_1 through improve_stop_branch_5 into trunk
  (all mods for interrupted draw and user-friendly stop).

  Revision 1.9.2.1  1997/08/07 16:56:30  wenger
  Partially-complete code for improved stop capability (includes some
  debug code).

  Revision 1.9  1997/07/15 14:29:51  wenger
  Moved hashing of strings from TData*Interp classes to MappingInterp
  class; cleaned up a few extra includes of StringStorage.h.

  Revision 1.8  1997/05/28 20:01:27  andyt
  Shape attributes for 'Tcl/Tk Window' shape can now be variables, numeric
  constants, or string constants. Used to assume that all attributes except
  argc were strings.

  Revision 1.7  1997/05/21 22:12:26  andyt
  Added EmbeddedTk and Tasvir functionality to client-server library.
  Changed protocol between devise and ETk server: 1) devise can specify
  that a window be "anchored" at an x-y location, with the anchor being
  either the center of the window, or the upper-left corner. 2) devise can
  let Tk determine the appropriate size for the new window, by sending
  width and height values of 0 to ETk. 3) devise can send Tcl commands to
  the Tcl interpreters running inside the ETk process.

  Revision 1.6.6.1  1997/05/21 20:40:25  weaver
  Changes for new ColorManager

  Revision 1.6  1997/03/28 16:09:43  wenger
  Added headers to all source files that didn't have them; updated
  solaris, solsparc, and hp dependencies.

 */

#include <sys/param.h>
#include <strstream.h>

#include "ETkWindowShape.h"
#include "ETk.h"
#include "Init.h"
#include "Util.h"
#include "DevError.h"
#include "StringStorage.h"
#include "ViewGraph.h"

//#define DEBUG

static Boolean
GetShapeAttrValue(int i,
		  TDataMap *map,
		  char *gdataBuffer,
		  Coord &value,
		  AttrType &attrType);
		  
static char *
GetShapeAttrString(int i,
		   TDataMap *map,
		   char *gdataBuffer,
		   Boolean &caller_should_free,
		   StringStorage *stringTable);

int FullMapping_ETkWindowShape::NumShapeAttrs()
{
    return MAX_SHAPE_ATTRS;
}


void
FullMapping_ETkWindowShape::FindBoundingBoxes(void *gdataArray, int numRecs,
    TDataMap *tdMap, Coord &maxWidth, Coord &maxHeight)
{
#if defined(DEBUG)
  printf("FullMapping_ETkWindowShape::FindBoundingBoxes(%d)\n", numRecs);
#endif

  GDataAttrOffset *offsets = tdMap->GetGDataOffset();

  if (offsets->_bbULxOffset < 0 && offsets->_bbULyOffset < 0 &&
      offsets->_bbLRxOffset < 0 && offsets->_bbLRyOffset < 0) {
#if defined(DEBUG)
  printf("  Bounding box is constant\n");
#endif
    // Just do one record, since they're all the same.
    numRecs = 1;
    gdataArray = NULL; // because accessing GData is an error here
  } else {
#if defined(DEBUG)
    printf("  Bounding box is variable\n");
#endif
  }

  char *dataP = (char *)gdataArray; // char * for ptr arithmetic
  int recSize = tdMap->GDataRecordSize();
  Coord tmpMaxSize = 0.0;
  for (int recNum = 0; recNum < numRecs; recNum++) {
    Coord symSize = tdMap->GetSize(dataP);

    tmpMaxSize = MAX(tmpMaxSize, symSize);

    Coord halfSize = symSize / 2.0;

    tdMap->SetBoundingBox(dataP, -halfSize, halfSize, halfSize, -halfSize);

    dataP += recSize;
  }

  maxWidth = tmpMaxSize;
  maxHeight = tmpMaxSize;
}


void FullMapping_ETkWindowShape::DrawGDataArray(WindowRep *win,
						void **gdataArray,
						int numSyms, TDataMap *map,
						ViewGraph *view,
						int pixelSize,
						int &recordsProcessed,
						Boolean timeoutAllowed)
{
    BEGIN_ETK_TRACE(__FUNCTION__);
    
    int i, j;
    char *script;
    char *gdata;
    GDataAttrOffset *offset;
    Coord x, y, tx, ty, size;
    Coord x0, y0, x1, y1, width, height;
    int argc;
    char argv[MAX_SHAPE_ATTRS][ETK_MAX_STR_LENGTH + 1];
    Coord attrValue;
    AttrType attrType;
    char *temp;
    char *argv2[MAX_SHAPE_ATTRS];
    int handle;    
    Boolean freeScript, freeTemp;
    
    if (view->GetNumDimensions() == 3)
    {
	Draw3DGDataArray(win, gdataArray, numSyms, map, view, pixelSize,
	  recordsProcessed, timeoutAllowed);
	return;
    }
    
    //
    // Shape attributes should store:
    //   attr[0] = name of script
    //   attr[1] = number of arguments to be passed to the script
    //   attr[2] = arg
    //   attr[3] = arg
    //   ...
    //   attr[argc+1] = arg
    //

    offset = map->GetGDataOffset();
    StringStorage *stringTable = map->GetStringTable(TDataMap::TableGen);

    for (i = 0; i < numSyms; i++)
    {
	// first draw a cross mark at each Tk window location;
	// if there is a problem in displaying the Tk window,
	// then at least the user sees some symbol in the window

	gdata = (char *) gdataArray[i];
	x = map->GetX(gdata);
	y = map->GetY(gdata);

	win->SetForeground(map->GetColor(gdata));

	win->SetPattern(map->GetPattern(gdata));
	win->Transform(x, y, tx, ty);
	win->PushTop();
	win->MakeIdentity();
	win->Line(tx - 3, ty, tx + 3, ty, 1);
	win->Line(tx, ty - 3, tx, ty + 3, 1);
	win->PopTransform();

//	if (color == XorColor)
//	{
//	    win->SetCopyMode();
//	}
	
	// Size is expressed in data units, so convert to width and
	// height in pixels.
	size = map->GetSize(gdata);
	win->Transform(0.0, 0.0, x0, y0);
	win->Transform(size, size, x1, y1);
	width = fabs(x1 - x0);
	width *= pixelSize;
	width = MAX(width, pixelSize);
	height = fabs(y1 - y0);
	height *= pixelSize;
	height = MAX(height, pixelSize);

	//
	// Initialize the argv array with NULL strings
	//
	argc = 0;
	for (j = 0; j < MAX_SHAPE_ATTRS; j++)
	{
	    argv[j][0] = '\0';
	}
	
	//
	// Get name of script (shape attr 0)
	//
	if ((script = GetShapeAttrString(0, map,
		 gdata, freeScript, stringTable)) == NULL)
	{
		reportError("No Tcl script specified", devNoSyserr);
	    continue;
	}
	
	//
	// Get argc (shape attr 1)
	//
	if (GetShapeAttrValue(1, map, gdata,
			      attrValue, attrType) == false)
	{
	    reportError("No argument count specified", devNoSyserr);
	    continue;
	}
	if (attrType == StringAttr)
	{
	    if ((temp = GetShapeAttrString(1, map,
		   gdata, freeTemp, stringTable)) == NULL)
	    {
		reportError("No argument count specified", devNoSyserr);
		continue;
	    }
	    argc = atoi(temp);
	    if (freeTemp)
	    {
		delete [] temp;
	    }
	}
	else
	{
	    argc = int(attrValue);
	}
	if (argc < 0)
	{
	    reportError("Invalid argument count for Tcl/Tk window",
			devNoSyserr);
	    continue;
	}
	if (argc > (MAX_SHAPE_ATTRS - 2))
	{
	    argc = (MAX_SHAPE_ATTRS - 2);
	}
	
	//
	// Get arguments from shape attrs 2 to (2 + argc)
	//
	for (j = 0; j < argc; j++)
	{
	    if ((temp = GetShapeAttrString(j + 2, map, gdata,
		   freeTemp, stringTable)) == NULL)
	    {
		continue;
	    }
	    sprintf(argv[j], "%.*s", ETK_MAX_STR_LENGTH, temp);
	    if (freeTemp)
	    {
		delete [] temp;
	    }
	}
	
#ifdef DEBUG
	printf("Displaying Tcl/Tk window at %.2f,%.2f. script = %s\n",
	       x, y, script);
	for (j = 0; j < argc; j++)
	{
	    printf("    arg: %s\n", argv[j]? argv[j] : "(NULL)");
	}
#endif

	//
	// Display the window
	//
	
	//
	// argv is a 2-D char array, and we need a (char **) parameter
	// to pass to the CreateWindow function.
	//
	for (j = 0; j < argc; j++)
	{
	    argv2[j] = argv[j];
	}
	
	//
	// Create a new window
	//
	win->ETk_CreateWindow(tx, ty, width, height, ETkIfc::Center,
			      script, argc, (char **) argv2, handle);
	
    }
    
    recordsProcessed = numSyms;

    END_ETK_TRACE(__FUNCTION__);

}

static Boolean
GetShapeAttrValue(int i,
		  TDataMap *map,
		  char *gdataBuffer,
		  Coord &attrValue,
		  AttrType &attrType)
{
    GDataAttrOffset *offset;
    const ShapeAttr *defaultAttrs;
    int gdataOffset;
    AttrInfo *attrInfo;
    
    if (i < 0 || i >= MAX_SHAPE_ATTRS)
    {
	return false;
    }
    
    if ((attrInfo = map->GDataAttrList()->FindShapeAttr(i)) == NULL)
    {
	return false;
    }
    
    attrType = attrInfo->type;
    offset = map->GetGDataOffset();
    defaultAttrs = map->GetDefaultShapeAttrs();
    
    if ((gdataOffset = offset->_shapeAttrOffset[i]) < 0)
    {
	attrValue = Coord(defaultAttrs[i]);
    }
    else
    {
	attrValue = *(Coord *) (gdataBuffer + gdataOffset);
    }
    
    return true;

}

static char *
GetShapeAttrString(int i,
		   TDataMap *map,
		   char *gdataBuffer,
		   Boolean &caller_should_free,
		   StringStorage *stringTable)
{
    Coord attrValue;
    AttrType attrType;
    char *returnValue;
    ostrstream os;
    
    caller_should_free = false;

    if (GetShapeAttrValue(i, map, gdataBuffer, attrValue, attrType) == false)
    {
	return NULL;
    }
    
    switch (attrType)
    {
      case StringAttr:
	if (stringTable->Lookup(int(attrValue), returnValue) < 0)
	{
	    return NULL;
	}
	caller_should_free = false;
	break;
      
      case DoubleAttr:
	os << double(attrValue) << ends;
	returnValue = os.str();
	caller_should_free = true;
	break;
      
      case FloatAttr:
	os << float(attrValue) << ends;
	returnValue = os.str();
	caller_should_free = true;
	break;
      
      case IntAttr:
      case DateAttr:
	os << int(attrValue) << ends;
	returnValue = os.str();
	caller_should_free = true;
	break;
      
    }
    
    return returnValue;
    
}

//******************************************************************************

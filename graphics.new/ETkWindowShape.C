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
 */

#include <sys/param.h>

#include "ETkWindowShape.h"
#include "ETk.h"
#include "Map3D.h"
#include "Init.h"
#include "Util.h"
#include "DevError.h"

//#define DEBUG

int FullMapping_ETkWindowShape::NumShapeAttrs()
{
    return MAX_GDATA_ATTRS;
}


void FullMapping_ETkWindowShape::MaxSymSize(TDataMap *map, void *gdata,
					    int numSyms,
					    Coord &width, Coord &height)
{
    width = 0.0;
    height = 0.0;
}


void FullMapping_ETkWindowShape::DrawGDataArray(WindowRep *win,
						void **gdataArray,
						int numSyms, TDataMap *map,
						ViewGraph *view,
						int pixelSize)
{
    BEGIN_ETK_TRACE(__FUNCTION__);
    
    int i, j, k;
    char *script;
    int key;
    int code = -1;
    int done;
    char *gdata;
    GDataAttrOffset *offset;
    Coord x, y, tx, ty, size;
    Coord x0, y0, x1, y1, width, height;
    GlobalColor color;
    int argc;
    char argv[MAX_GDATA_ATTRS][ETK_MAX_STR_LENGTH];
    ShapeAttr *defaultAttrs;
    unsigned long attrflags;
    int gdataOffset;
    Coord attrValue;
    AttrInfo *attrinfo;
    char *temp;
    char *argv2[MAX_GDATA_ATTRS];
    int handle;    
    
    if (view->GetNumDimensions() == 3)
    {
	Draw3DGDataArray(win, gdataArray, numSyms, map, view, pixelSize);
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
    defaultAttrs = map->GetDefaultShapeAttrs();
    attrflags = map->GetDynamicShapeAttrs();
    
    offset = map->GetGDataOffset();

    for (i = 0; i < numSyms; i++)
    {
	// first draw a cross mark at each Tk window location;
	// if there is a problem in displaying the Tk window,
	// then at least the user sees some symbol in the window

	gdata = (char *) gdataArray[i];
	x = ShapeGetX(gdata, map, offset);
	y = ShapeGetY(gdata, map, offset);

	color = GetColor(view, gdata, map, offset);
	if (color == XorColor)
	{
	    win->SetXorMode();
	}
	else
	{
	    win->SetFgColor(color);
	}
	
	win->SetPattern(GetPattern(gdata, map, offset));
	win->Transform(x, y, tx, ty);
	win->PushTop();
	win->MakeIdentity();
	win->Line(tx - 3, ty, tx + 3, ty, 1);
	win->Line(tx, ty - 3, tx, ty + 3, 1);
	win->PopTransform();
	if (color == XorColor)
	    win->SetCopyMode();
	
	// Size is expressed in data units, so convert to width and
	// height in pixels.
        size = GetSize(gdata, map, offset);
	win->Transform(0.0, 0.0, x0, y0);
	win->Transform(size, size, x1, y1);
	width = fabs(x1 - x0);
	width *= pixelSize;
	width = MAX(width, pixelSize);
	height = fabs(y1 - y0);
	height *= pixelSize;
	height = MAX(height, pixelSize);
	
	//
	// Now we need to get our hands on all the shape attributes.
	//
	// To test whether shape attribute i is defined at all
	//   map->GetDynamicShapeAttrs() & (1 << i)
	//
	// To test for a constant-valued attribute:
	//   offset->shapeAttrOffset[i] < 0
	//
	// To get a pointer to the value of a string variable
	//   attrValue = *(Coord *) (gdata + gdataOffset);
	//   code = StringStorage::Lookup((int) attrValue, POINTER_TO_VALUE);
	//

	// Initialize the argv array with NULL strings
	argc = 0;
	for (j = 0; j < MAX_GDATA_ATTRS; j++)
	{
	    argv[j][0] = '\0';
	}
	
	//
	// Make sure the name of the script and argc are defined
	//
	if (!(attrflags & (1 << 0)))
	{
	    reportError("No Tcl script specified", devNoSyserr);
	    continue;
	}
	if (!(attrflags & (1 << 1)))
	{
	    reportError("No argument count specified for Tcl/Tk window",
			devNoSyserr);
	    continue;
	}

	//
	// Get the name of the script. Assume that it is a string
	// variable reference attr.
	//
	script = NULL;
	code = -1;
	if ((gdataOffset = offset->shapeAttrOffset[0]) >= 0)
	{
	    attrValue = *(Coord *) (gdata + gdataOffset);
	    code = StringStorage::Lookup((int) attrValue, script);
	}
	//
	// Maybe the attrValue is an index into the StringLookup
	// table
	//
	else
	{
	    attrValue = (Coord) defaultAttrs[0];
	    code = StringStorage::Lookup((int) attrValue, script);
	}
	if (script == NULL || code < 0)
	{
	    reportError("No Tcl script specified", devNoSyserr);
	    continue;
	}

	//
	// Get argument count from shape attr 1
	//
	gdataOffset = offset->shapeAttrOffset[1];
	if (gdataOffset < 0)
	{
	    attrValue = (Coord) defaultAttrs[1];
	}
	else
	{
	    attrValue = *(Coord *) (gdata + gdataOffset);
	}
	argc = (int) attrValue;
	if (argc < 0)
	{
	    reportError("Invalid argument count for Tcl/Tk window",
			devNoSyserr);
	    continue;
	}
	if (argc > (MAX_GDATA_ATTRS - 2))
	    argc = (MAX_GDATA_ATTRS - 2);
	
	//
	// Get arguments from shape attrs 2 to (2 + argc)
	//
	for (j = 0; j < argc; j++)
	{
	
	    if (!(attrflags & (1 << (j + 2))))
	    {
		continue;
	    }
	    
	    gdataOffset = offset->shapeAttrOffset[j + 2];
	    if (gdataOffset < 0)
	    {
		attrValue = (Coord) defaultAttrs[j + 2];
		sprintf(argv[j], "%f", attrValue);
		continue;
	    }

	    attrValue = *(Coord *) (gdata + gdataOffset);
	    attrinfo = map->MapShapeAttr2TAttr(j + 2);

	    if (attrinfo != NULL && attrinfo->type == StringAttr)
	    {
		code = StringStorage::Lookup((int) attrValue, temp);
		if (code == 0 && temp != NULL)
		{
		    strncpy(argv[j], temp, ETK_MAX_STR_LENGTH);
		}
	    }
	    else
	    {
		sprintf(argv[j], "%f", attrValue);
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
	// Is there already a window at these x,y coords?
	//
	
	//
	// This feature not supported yet. Andy will replace the "if (0)"
	// with the following two lines once the window searches work
	// correctly.
	//
	// handle = win->ETk_FindWindow(x, y, script);
	// if (handle >= 0)
	//
	if (0)
	{
	    //
	    // Update the window
	    //
	    reportError("Window updates not implemented yet", devNoSyserr);
	    //
	    // Mark the window as "in_use"
	    //
	    win->ETk_Mark(handle, true);
	}
	
	else
	{
	    //
	    // Create a new window
	    //
	    win->ETk_CreateWindow(tx, ty, width, height,
				  script, argc, (char **) argv2, handle);
	}

    }
    
    END_ETK_TRACE(__FUNCTION__);

}


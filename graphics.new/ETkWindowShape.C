
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
#if defined(DEBUG)
    printf("%s\n", __PRETTY_FUNCTION__);
#endif
    
    int i, j, k;
    char *script;
    int key;
    int code;
    int done;
    char *gdata;
    GDataAttrOffset *offset;
    Coord x, y, tx, ty, size;
    GlobalColor color;
    
    if (view->GetNumDimensions() == 3) {
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
    //   attr[argc-1] = arg
    //
    int argc;
    char argv[MAX_GDATA_ATTRS][ETK_MAX_STR_LENGTH];
    ShapeAttr *defaultAttrs = map->GetDefaultShapeAttrs();
    unsigned long attrflags = map->GetDynamicShapeAttrs();
    int gdataOffset;
    Coord attrValue;
    AttrInfo *attrinfo;
    char *temp;
    
    offset = map->GetGDataOffset();

    for (i = 0; i < numSyms; i++) {
	
	// first draw a cross mark at each Tk window location;
	// if there is a problem in displaying the Tk window,
	// then at least the user sees some symbol in the window
	gdata = (char *)gdataArray[i];
	x = GetX(gdata, map, offset);
	y = GetY(gdata, map, offset);
	color = GetColor(view, gdata, map, offset);
	if (color == XorColor)
	    win->SetXorMode();
	else
	    win->SetFgColor(color);
	win->SetPattern(GetPattern(gdata, map, offset));
	win->Transform(x, y, tx, ty);
	win->PushTop();
	win->MakeIdentity();
	win->Line(tx - 3, ty, tx + 3, ty, 1);
	win->Line(tx, ty - 3, tx, ty + 3, 1);
	win->PopTransform();
	if (color == XorColor)
	    win->SetCopyMode();
	
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
	script = NULL;
	argc = 0;
	for (j = 0; j < MAX_GDATA_ATTRS; j++)
	    argv[j][0] = '\0';
	
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
	if ((gdataOffset = offset->shapeAttrOffset[0]) >= 0)
	{
	    attrValue = *(Coord *) (gdata + gdataOffset);
	    attrinfo = map->MapShapeAttr2TAttr(j);
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
	       tx, ty, script);
	for (j = 0; j < argc; j++)
	{
	    printf("    arg: %s\n", argv[j]? argv[j] : "(NULL)");
	}
#endif
	
	// Display the window
	char *argv2[MAX_GDATA_ATTRS];
	for (j = 0; j < argc; j++)
	{
	    argv2[j] = argv[j];
	}
	size = GetSize(gdata, map, offset);
	size *= pixelSize;
	int dummy;
	win->ETk_CreateWindow(tx, ty, size, size, script,
			      argc, argv2, dummy);

    }

}


/*
  ========================================================================
  DEVise Data Visualization Software
  (c) Copyright 1992-1996
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
  Revision 1.14  1996/06/16 01:54:19  jussi
  Added IsComplexShape() method.

  Revision 1.13  1996/06/15 14:05:34  jussi
  Cleaned up a little bit.

  Revision 1.12  1996/05/07 17:03:27  jussi
  Method MapGAttr2TAttr() now returns NULL by default. It used
  to be a pure virtual method but that would have required changes
  in some files in ams and multi.

  Revision 1.11  1996/05/07 16:39:10  jussi
  Added MapGAttr2TAttr() method for translating GData attributes
  to TData attributes.

  Revision 1.10  1996/04/16 20:42:32  jussi
  Removed #include assert.h.

  Revision 1.9  1996/04/09 22:54:08  jussi
  Added View parameter to DrawGDataArray().

  Revision 1.8  1996/03/25 23:02:23  jussi
  Removed references to _view. A more comprehensive mechanism is needed
  for storing views that the map is associated with (right now is stores
  just one).

  Revision 1.7  1996/03/07 16:52:56  jussi
  Added association of TDataMap and ViewGraph.

  Revision 1.6  1996/03/05 23:20:36  jussi
  Added destructor which deletes dynamically allocated objects.

  Revision 1.5  1996/01/13 23:10:07  jussi
  Added support for Z attribute and shape attribute 2.

  Revision 1.4  1995/11/25  01:20:16  jussi
  This code now uses Transform matrix operations to convert user/world
  coordinates to screen pixel coordinates. This is to avoid any future
  inconsistencies in how the different code locations compute the
  conversion. xPerPixel and yPerPixel are now obsolete coefficients.

  Revision 1.3  1995/11/21 23:20:03  jussi
  Added copyright notice and cleaned up code. Added SetDefaultX
  and SetDefaultY methods, removed SetDefaultLocation.

  Revision 1.2  1995/09/05 22:15:55  jussi
  Added CVS header.
*/

#ifndef TDataMap_h
#define TDataMap_h

#include <stdio.h>

#include "DeviseTypes.h"
#include "Exit.h"
#include "DList.h"
#include "Color.h"
#include "Pattern.h"
#include "ShapeID.h"
#include "VisualArg.h"
#include "RecId.h"
#include "GDataRec.h"
#include "ViewGraph.h"

class AttrList;

class TData;
class GData;
class Symbol;
class WindowRep;

/* Offsets for GData attributes. Not all offsets are valid (negative). */

struct GDataAttrOffset {
  int xOffset;
  int yOffset;
  int zOffset;
  int colorOffset;
  int sizeOffset;
  int shapeOffset;
  int patternOffset;
  int orientationOffset;
  int shapeAttrOffset[MAX_GDATA_ATTRS];
};

class TDataMap {
public:
  /* constructor. dynamicArgs == attributes that are
     dynamically alloated. dynamicAttrs: set bit i to 1 if
     ith shape attribute is dynamically allocated.
     name: name of this type of mapping.
     gdataName:  name of GData (name of this mapping instance */

  TDataMap(char *name, TData *tdata, char *gdataName, int gdataRecSize,
	   VisualFlag dynamicArgs, int dynamicShapeAttrs, int maxGDataPages,
	   VisualFlag *dimensionInfo, int numDimensions,
	   Boolean createGData = true);
  
  /* destructor */
  virtual ~TDataMap();

  /* Set/Get GData attribute list */
  void SetGDataAttrList(AttrList *gAttrList) { _gdataAttrList = gAttrList; }
  AttrList *GDataAttrList() { return _gdataAttrList; }

  /* Get/Set info about the dimensions of the GData */
  void SetDimensionInfo(VisualFlag *dimensionInfo, int numDimensions);
  int DimensionInfo(VisualFlag *&dimensionInfo) { 
    dimensionInfo = _dimensionInfo;
    return _numDimensions;
  }
		
  virtual Boolean IsInterpreted() { return false; }
  virtual Boolean IsComplexShape(ShapeID shape) { return false; }

  char *GetName() { return _mappingName; }    /* type of mapping */
  char *GetGDataName() { return _gdataName; } /* name of GData */

  /* clear GData and start anew. This
     happens when subclass changes its mapping, and
     needs to get rid of cached mappings */
  void ResetGData(int gRecSize);
  
  VisualFlag GetDynamicArgs() { return _dynamicArgs; }
  int GetDynamicShapeAttrs() { return _dynamicAttrs; }
  void GetDefaultLocation(Coord &x, Coord &y){ x = _x; y = _y; }
  Coord GetDefaultX() { return _x; }
  Coord GetDefaultY() { return _y; }
  Coord GetDefaultZ() { return _z; }
  Color GetDefaultColor() { return _color; }
  Coord GetDefaultSize() { return _size; }
  Pattern GetDefaultPattern() { return _pattern; }
  Coord GetDefaultOrientation() { return _orientation; }
  ShapeID GetDefaultShape() { return _shapeId; }
  int GetDefaultNumShapeAttrs() { return _numShapeAttr; }
  ShapeAttr *GetDefaultShapeAttrs() { return _shapeAttrs; }

  int GetPixelWidth() { return _pixelWidth; }
  void SetPixelWidth(int width) { _pixelWidth = width; }
  
  /* Map record to symbol by calling user defined Map() function.
     This is called by the query processor.
     TData == textual data being mapped.
     recId = record ID of the record
     rec == the actual record
     symbol =  the symbol to be mapped. */
#ifdef TDATAMAP_MAP_TO_SYMBOL
  void MapToSymbol(TData *tdata, RecId recID, void *rec, Symbol *sym);
#endif

  /*************************************************************
    User Defined Map function.
  **************************************************************/

#ifdef TDATAMAP_MAP_TO_SYMBOL
  virtual void Map(TData *, RecId, void * /*rec*/, Symbol *)=0;
#endif

  /* convert from Tdata to Gdata. buf contains
     buffer for data. tRecs are pointers to variable size
     records only. */
  virtual void ConvertToGData(RecId startRecId, void *buf,
			      void **tRecs, int numRecs,
			      void *gdataPtr) = 0;

  /* Get the GData file for the mapping */
  GData *GetGData() { return _gdata; }
  
  /* Get the TData file for the mapping */
  TData *GetTData() { return _tdata; }
  
  /* Get record size for TData */
  int TDataRecordSize();
  
  /* Get record size for GData */
  int GDataRecordSize() { return _gRecSize; }
  
  /**************************************************************
    User provided function to return a hint of the RecId
    of the TData closest to the given X value. Note:
    the recId returned is just a hint, does not have to be
    exact. Return true if hint is given.
    Default Implementation: Assumes that tdata is sorted in the
    x direction and evenly distributed. Return the RecId 
    in proportion to its x value.
    Parameters:
      tdata: tdata to find hint.
      x: x coord of page 
      isPrefetch: set to TRUE if we're doing prefetch. 
    Output:
      RecId: page number where x is likely to be located.
   ****************************************************************/
  virtual Boolean PageHint(TData *tdata, Coord x, Boolean isPrefetch,
			   RecId &hintId);

  /* Update/get maximum symbol size */
  virtual void UpdateMaxSymSize(void *gdata, int numSyms) = 0;
  virtual void GetMaxSymSize(Coord &width, Coord &height, Coord &depth) {
    width = _maxSymWidth; height = _maxSymHeight; depth = _maxSymDepth;
  }

  virtual void DrawGDataArray(View *view, WindowRep *win,
			      void **gdataArray, int num) = 0;

  virtual AttrInfo *MapGAttr2TAttr(char *attrName) { return 0; }

  /* Hint for current focus in GData */
  void SetFocusId(RecId id);
  RecId GetFocusId();
  
  /* Setting and getting offsets of GData attributes */
  void SetGDataOffset(GDataAttrOffset *offset) { _gOffset = offset; }
  GDataAttrOffset *GetGDataOffset() { return _gOffset; }
  
  void InsertUserData(void *data) { _userData = data; }
  void *GetUserData() { return _userData; }
  
protected:
  
  /* Set new defaults. Should only be done in the constructor of
     derived classes.*/
  void SetDefaultX(Coord x) { _x = x; }
  void SetDefaultY(Coord y) { _y = y; }
  void SetDefaultZ(Coord z) { _z = z; }
  void SetDefaultColor(Color color) { _color = color; }
  void SetDefaultSize(Coord size) { _size = size; }
  void SetDefaultPattern(Pattern pattern) { _pattern = pattern; }
  void SetDefaultOrientation(Coord orientation) { _orientation = orientation; }
  void SetDefaultShape(ShapeID shapeID, int numAttr = 0, 
		       ShapeAttr *shapeAttr = NULL);
  void SetDefaultShapeAttr(int attrNum, Coord shapeAttr);

  Coord _maxSymWidth; /* bounding box for symbol */
  Coord _maxSymHeight;
  Coord _maxSymDepth;

private:
  GDataAttrOffset *_gOffset; /* offset of GData attributes */

  int _gRecSize; /* gdata record size */
  
  /* incarnation: how many TDataMaps have been created */
  static int _incarnation;
  
  /* Create name of GData */
  char *CreateGDataName(char *tdataName, char *mappingName);
  char *CreateGDataPath(char *gdataName);
  
  /* default values */
  Coord _x;
  Coord _y;
  Coord _z;
  Color _color;
  Coord _size;
  Pattern _pattern;
  Coord _orientation;
  ShapeID _shapeId;
  int _numShapeAttr;
  ShapeAttr *_shapeAttrs;
  
  /* For hint calculation */
  Boolean _hintInitialized;  /* TRUE if _minX, _maxX, _numPages initialized */
  Coord _minX, _maxX, _numPages; /* minimum and max X values, and # of pages*/
  TData *_hintTdata; /* tdata for the above cached info */
  
  char *_mappingName;
  TData *_tdata;
  GData *_gdata;
  VisualFlag _dynamicArgs; /* attributes that are set dynamically */
  int _dynamicAttrs;
  void *_tmpPtr[1]; /* used as temp storage for storing data used
		       in hint calculation */
  
  int _pixelWidth; /* width of pixel when size of symbol <= 1 pixel.
		      Set to 1 for drawing 1 pixel. Set to > 1 to 
		      enhance the points. */
  
  VisualFlag *_dimensionInfo;
  int _numDimensions;
  
  char *_gdataName; /* name of GData */
  char *_gdataPath; /* actual path used to store gdata, generated
		       from GData Name */
  
  RecId _focusId;
  Boolean _createGData;
  
  AttrList *_gdataAttrList;
  int _maxGDataPages; /* max # of GData pages, or -1 if no limit */
  
  /* user data. used by QueryProcessor to insert its own info. */
  void *_userData;
};

#endif

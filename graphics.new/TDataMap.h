/*
  ========================================================================
  DEVise Data Visualization Software
  (c) Copyright 1992-1995
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
  Revision 1.2  1995/09/05 22:15:55  jussi
  Added CVS header.
*/

/* Mapping from TData to GData */

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
#include "Bitmap.h"
#include "RecId.h"
#include "GDataRec.h"

class AttrList;

class TData;
class GData;
class Symbol;
class WindowRep;
class Bitmap;

const unsigned MAX_MAPPING_SHAPE_ATTRS = 10;

/* Offsets for GData attributes.Not all is valid. */
struct GDataAttrOffset {
  int xOffset, yOffset, colorOffset, sizeOffset, shapeOffset,
      patternOffset, orientationOffset;
  int shapeAttrOffset[MAX_GDATA_ATTRS];
};

class TDataMap {
public:
  /* initializer. dynamicArgs == attributes that are
     dynamically alloated. dynamicAttrs: set bit i to 1 if
     ith shape attribute is dynamically allocated.
     name: name of this type of mapping.
     gdataName:  name of GData (name of this mapping instance */

  TDataMap(char *name, TData *tdata, char *gdataName, 
	   int gdataRecSize,
	   VisualFlag dynamicArgs, int dynamicShapeAttrs, int maxGDataPages,
	   VisualFlag *dimensionInfo, int numDimensions,
	   Boolean createGData = true);
  
  /* Set/Get GData attribute list */
  void SetGDataAttrList(AttrList *gAttrList) { _gdataAttrList = gAttrList; }
  AttrList *GDataAttrList() { return _gdataAttrList; }

  /* Get current defaults */
  /* Get/Set info about the dimensions of the GData */
  void SetDimensionInfo(VisualFlag *dimensionInfo, int numDimensions);
  int DimensionInfo(VisualFlag *&dimensionInfo) { 
    dimensionInfo = _dimensionInfo;
    return _numDimensions;
  }
		
  virtual Boolean IsInterpreted() { return false; }

  char *GetName() { return _mappingName; }	/* type of mapping */
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
  Color GetDefaultColor() { return _color;};
  Coord GetDefaultSize() {return _size; };
  Pattern GetDefaultPattern() { return _pattern ;};
  Coord GetDefaultOrientation() { return _orientation ;};
  ShapeID GetDefaultShape(){ return _shapeId;}
  int GetDefaultNumShapeAttrs(){ return _numShapeAttr;}
  ShapeAttr *GetDefaultShapeAttrs(){ return _shapeAttrs;};
  int GetPixelWidth() { return _pixelWidth; }
  void SetPixelWidth(int width) {_pixelWidth = width; }
  
  /* Map record to symbol by calling user defined Map() function.
     This is called by the query processor.
     TData == textual data being mapped.
     recId = record ID of the record
     rec == the actual record
     symbol =  the symbol to be mapped. */
#ifdef TDATAMAP_MAP_TO_SYMBOL
  void MapToSymbol(TData *tdata, RecId recID, void *rec, Symbol *sym);
#endif

  /* return the max bounding box found so far */
  void MaxBoundingBox(Coord &width, Coord &height);
  
  /*************************************************************
    User Defined Map function.
  **************************************************************/

#ifdef TDATAMAP_MAP_TO_SYMBOL
  virtual void Map(TData *, RecId , void * /*rec*/, Symbol *)=0;
#endif

  /* convert from Tdata to Gdata. buf contains
     buffer for data. tRecs are pointers to variable size
     records only. */
  virtual void ConvertToGData(RecId startRecId,void *buf,
			      void **tRecs,int numRecs,void *gdataPtr)=0;

  /* Get the GData file for the mapping */
  GData *GetGData() { return _gdata;}
  
  /* Get the TData file for the mapping */
  TData *GetTData() { return _tdata;}
  
  /* Get record size for TData */
  int TDataRecordSize();
  
  /* Get record size for GData */
  int GDataRecordSize(){ return _gRecSize;}
  
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

  /* find the max box bounding for all records */
  /*
     virtual void UpdateBoundingBox(int pageNum, 
     void **gdataArray, int numRecs)= 0;
  */

  virtual void DrawGDataArray(WindowRep *win, void **gdataArray, int num,
			      Coord xPerPixel, Coord yPerPixel)=0;
  
  /* Hint for current focus in GData */
  void SetFocusId(RecId id);
  RecId GetFocusId();
  
  /* Setting and getting offsets of GData attributes */
  void SetGDataOffset(GDataAttrOffset *offset){
    _gOffset = offset;
  }
  GDataAttrOffset *GetGDataOffset() {
    return _gOffset;
  }
  
  void InsertUserData(void *data) { _userData = data; }
  void *GetUserData() { return _userData; }
  
protected:
  
  /* Set new defaults. Should only be done in the constructor of
     derived classes.*/
  void SetDefaultX(Coord x) { _x = x; }
  void SetDefaultY(Coord y) { _y = y; }
  void SetDefaultColor(Color color) { _color = color; }
  void SetDefaultSize(Coord size) { _size = size; }
  void SetDefaultPattern(Pattern pattern) { _pattern = pattern; }
  void SetDefaultOrientation(Coord orientation) { _orientation = orientation; }
  void SetDefaultShapeAttr(int attrNum, Coord shapeAttr);
  
  void SetDefaultShape(ShapeID shapeID, int numAttr = 0, 
		       ShapeAttr *shapeAttr = NULL);
  
  void UpdateBoundingBox(Coord width, Coord height);
  
  Boolean TestAndSetPage(int pageNum){
    Boolean result;
    if (!(result=_boundingBoxBitmap->TestBit(pageNum)))
      _boundingBoxBitmap->SetBit(pageNum);
    return result;
  }

private:
  GDataAttrOffset *_gOffset; /* offset of GData attributes */

  int _gRecSize; /* gdata record size */
  
  /* incarnation: how many TDataMaps have been created */
  static int _incarnation;
  
  /* Create name of GData */
  char *CreateGDataName(char *tdataName, char *mappingName);
  char *CreateGDataPath(char *gdataName);
  
  /* default values */
  Coord _x, _y;
  Color _color; Coord _size;
  Pattern _pattern; Coord _orientation;
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
  
  Bitmap *_boundingBoxBitmap; /* keeps track which pages have
				 been tested for bounding box */
  
  VisualFlag *_dimensionInfo;
  int _numDimensions;
  
  char *_gdataName; /* name of GData */
  char *_gdataPath; /* actual path used to store gdata, generated
		       from GData Name */
  
  RecId _focusId;
  Boolean _createGData;
  
  AttrList *_gdataAttrList;
  int _maxGDataPages; /* max # of GData pages, or -1 if no limit */
  
  /* user data. (used by QueryProcessor to insert its own info) */
  void *_userData;
  
  Coord _boundWidth, _boundHeight; /* bounding box width/height */
};

#endif

/*
  ========================================================================
  DEVise Data Visualization Software
  (c) Copyright 1992-2000
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
  Revision 1.39  1999/07/21 18:51:14  wenger
  Moved alignment and data font information from view into mapping.

  Revision 1.38  1999/07/13 17:32:44  wenger
  Parent view can now control attribute(s) in child view's mapping;
  cleaned up some of the mapping-related code; better command logging.

  Revision 1.37  1999/05/28 16:32:45  wenger
  Finished cleaning up bounding-box-related code except for PolyLineFile
  symbol type; fixed bug 494 (Vector symbols drawn incorrectly); improved
  drawing of Polyline symbols.

  Revision 1.36  1999/05/26 19:50:54  wenger
  Added bounding box info to GData, so that the selection of records by the
  visual filter is more accurate.  (Note that at this time the bounding box
  info does not take into account symbol orientation; symbol alignment is
  taken into account somewhat crudely.) This includes considerable
  reorganization of the mapping-related classes.  Fixed problem with
  pixelSize getting used twice in Rect shape (resulted in size being the
  square of pixel size).

  Revision 1.35  1999/05/21 14:52:50  wenger
  Cleaned up GData-related code in preparation for including bounding box
  info.

  Revision 1.34  1999/05/20 15:17:55  wenger
  Fixed bugs 490 (problem destroying piled parent views) and 491 (problem
  with duplicate elimination and count mappings) exposed by Tim Wilson's
  two-station session.

  Revision 1.33  1999/03/03 18:22:01  wenger
  Fixed bugs 426 and 432 (problems with '5' (home) key); fixed bugs 466
  and 467 (query errors with sorted attributes); minor improvements to
  view symbols.

  Revision 1.32  1998/11/06 17:59:54  wenger
  Multiple string tables fully working -- allows separate tables for the
  axes in a given view.

  Revision 1.31  1998/11/04 20:34:01  wenger
  Multiple string tables partly working -- loading and saving works, one
  table per mapping works; need multiple tables per mapping, API and GUI,
  saving to session, sorting.

  Revision 1.30  1998/05/06 22:05:00  wenger
  Single-attribute set links are now working except where the slave of
  one is the master of another.

  Revision 1.29  1998/04/29 17:53:57  wenger
  Created new DerivedTable class in preparation for moving the tables
  from the TAttrLinks to the ViewDatas; found bug 337 (potential big
  problems) while working on this.

  Revision 1.28  1998/04/28 18:03:17  wenger
  Added provision for "logical" and "physical" TDatas to mappings,
  instead of creating new mappings for slave views; other TAttrLink-
  related improvements.

  Revision 1.27  1998/02/26 22:59:54  wenger
  Added "count mappings" to views, except for API and GUI (waiting for
  Dongbin to finish his mods to ParseAPI); conditionaled out unused parts
  of VisualFilter struct; did some cleanup of MappingInterp class.

  Revision 1.26  1998/02/19 23:25:18  wenger
  Improved color library and got client/server test code to work
  (except for setting colors by RGB): reduced compile interdependencies,
  especially in color library; color and utils libraries install headers
  as per code reorg plans; added standard DEVise headers to all color
  manager files; moved color initialization into Display constructors;
  fixed some compile warnings throughout the code.

  Revision 1.25  1997/11/24 23:15:20  weaver
  Changes for the new ColorManager.

  Revision 1.24  1997/11/18 23:27:03  wenger
  First version of GData to socket capability; removed some extra include
  dependencies; committed test version of TkControl::OpenDataChannel().

  Revision 1.23  1997/08/20 22:11:11  wenger
  Merged improve_stop_branch_1 through improve_stop_branch_5 into trunk
  (all mods for interrupted draw and user-friendly stop).

  Revision 1.22.2.1  1997/08/07 16:56:43  wenger
  Partially-complete code for improved stop capability (includes some
  debug code).

  Revision 1.22  1997/05/28 15:39:31  wenger
  Merged Shilpa's layout manager code through the layout_mgr_branch_2 tag.

  Revision 1.21.4.1  1997/05/20 16:11:17  ssl
  Added layout manager to DEVise

  Revision 1.21.6.1  1997/05/21 20:40:49  weaver
  Changes for new ColorManager

  Revision 1.21  1997/04/21 22:59:24  guangshu
  Added function MapTAttr2GAttr.

  Revision 1.20  1997/03/19 19:41:52  andyt
  Embedded Tcl/Tk windows are now sized in data units, not screen pixel
  units. The old list of ETk window handles (WindowRep class) has been
  replaced by a list of ETkInfo structs, each with fields for the window
  handle, x-y coordinates, name of the Tcl script, and an "in_use"
  flag. Added an ETk_Cleanup() procedure that gets called inside
  View::ReportQueryDone and destroys all ETk windows that are not marked
  as in_use.

  Revision 1.19  1996/12/30 23:57:37  andyt
  First version with support for Embedded Tcl/Tk windows. Added new
  ETkWindow symbol shape. Improved the MappingInterp::MapGAttr2TAttr
  function to handle all GData attributes (used to only handle a subset).

  Revision 1.18  1996/11/23 21:17:17  jussi
  Removed reference to DEVISE_PAGESIZE. Removed StripPath() which
  is already in Util.c.

  Revision 1.17  1996/11/13 16:57:10  wenger
  Color working in direct PostScript output (which is now enabled);
  improved ColorMgr so that it doesn't allocate duplicates of colors
  it already has, also keeps RGB values of the colors it has allocated;
  changed Color to GlobalColor, LocalColor to make the distinction
  explicit between local and global colors (_not_ interchangeable);
  fixed global vs. local color conflict in View class; changed 'dali'
  references in command-line arguments to 'tasvir' (internally, the
  code still mostly refers to Dali).

  Revision 1.16  1996/07/19 02:54:10  jussi
  Changed view parameter to DrawGDataArray().

  Revision 1.15  1996/07/02 22:47:07  jussi
  Rewrote interface for setting and querying maximum symbol
  size (bounding box).

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

#include "DeviseTypes.h"
#include "Pattern.h"
#include "ShapeID.h"
#include "VisualArg.h"
#include "RecId.h"
#include "GDataRec.h"
#include "AttrList.h"

#include "Color.h"
#include "XColor.h"
#include "Coloring.h"
#include "ObjectValid.h"

class AttrList;

class TData;
class GData;
class Symbol;
class WindowRep;
class ViewGraph;
class StringStorage;

//******************************************************************************
// class TDataMap
//******************************************************************************

class TDataMap
{
	private:

		Coloring	coloring;		// Default data coloring; not used if
									// color is non-constant

	public:

		// Constructors and Destructors

		// constructor. dynamicArgs == attributes that are dynamically
		// allocated. dynamicAttrs: set bit i to 1 if ith shape attribute
		// is dynamically allocated. name: name of this type of mapping.
		// gdataName:  name of GData (name of this mapping instance */
		TDataMap(char* name, TData* tdata, char* gdataName, int gdataRecSize,
				 VisualFlag dynamicArgs, unsigned long dynamicShapeAttrs,
				 int maxGDataPages, VisualFlag* dimensionInfo,
				 int numDimensions, Boolean createGData = true);

		virtual ~TDataMap(void);

		// Getters and Setters
		Coloring&			GetColoring(void)			{ return coloring;	}
		const Coloring&		GetColoring(void) const 	{ return coloring;	}

		// Utility Functions
		PColorID	GetPColorID(const char* recPtr) const;
		ColorID		GetColorID(const char* recPtr) const;
		XColorID	GetXColorID(const char* recPtr) const;

/* Set/Get GData attribute list */
  void SetGDataAttrList(AttrList *gAttrList) { _gdataAttrList = gAttrList; }
  AttrList *GDataAttrList() { return _gdataAttrList; }

  /* Get/Set info about the dimensions of the GData */
  // I think this info tells you the number of dimensions and which
  // dimensions are mapped to a sorted TData attribute.  RKW 1999-03-02.
  void SetDimensionInfo(VisualFlag *dimensionInfo, int numDimensions);
  int DimensionInfo(VisualFlag *&dimensionInfo) { 
    dimensionInfo = _dimensionInfo;
    return _numDimensions;
  }

  // String tables for this mapping.
  enum TableType { TableInvalid, TableX, TableY, TableZ, TableGen };
  char **TableType2NameP(TableType type);

  void SetStringTable(TableType type, char *name);
  StringStorage *GetStringTable(TableType type);
		

  virtual Boolean IsInterpreted() { return false; }
  virtual Boolean IsComplexShape(ShapeID shape) { return false; }

  const char *GetName() { return _mappingName; }    /* type of mapping */
  char *GetGDataName() { return _gdataName; } /* name of GData */

  // Create a GData, whether or not we already have one.  Added to reduce
  // complications of determining GData record size in MappingInterp class.
  // RKW 1999-05-21.
  void CreateGData(int gRecSize);

  /* clear GData and start anew. This
     happens when subclass changes its mapping, and
     needs to get rid of cached mappings */
  void ResetGData(int gRecSize);
  
  RecId GetDefaultRecId() const { return 0; }

  VisualFlag GetDynamicArgs() { return _dynamicArgs; }

  unsigned long GetDynamicShapeAttrs() { return _dynamicAttrs; }
  void SetDynamicShapeAttrs(unsigned long attrs) { _dynamicAttrs = attrs; }
  
  // Default GData values.
  void GetDefaultLocation(Coord &x, Coord &y) const {
    x = _defaults._x; y = _defaults._y; }
  Coord GetDefaultX() const { return _defaults._x; }
  Coord GetDefaultY() const { return _defaults._y; }
  Coord GetDefaultZ() const { return _defaults._z; }
  Coord GetDefaultSize() const { return _defaults._size; }
  Pattern GetDefaultPattern() const { return _defaults._pattern; }
  Coord GetDefaultOrientation() const { return _defaults._orientation; }
  ShapeID GetDefaultShape() const { return _defaults._shape; }
  Coord GetDefaultBbULx() const { return _defaults._bbULx; }
  Coord GetDefaultBbULy() const { return _defaults._bbULy; }
  Coord GetDefaultBbLRx() const { return _defaults._bbLRx; }
  Coord GetDefaultBbLRy() const { return _defaults._bbLRy; }
  int GetDefaultNumShapeAttrs() const { return _numShapeAttr; }
  const ShapeAttr *GetDefaultShapeAttrs() const { return _defaults._shapeAttrs; }
  
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
			      int numRecs, void *gdataPtr) = 0;

  /* Get the GData file for the mapping */
  GData *GetGData() { return _gdata; }
  
  /* Get the "logical" (user) TData file for the mapping */
  TData *GetLogTData() { return _logicalTdata; }

  /* Get the "physical" TData -- the same as the logical unless this
   * mapping is the slave of a TAttrLink. */
  TData *GetPhysTData() { return _tdata; }

  /* Set the "physical" TData.  The new TData should have the same
   * attributes as the old TData. */
  void SetPhysTData(TData *tdata);

  int GetPhysTdTimestamp() { return _physTdTimestamp; }
  
  /* Get record size for TData */
  int TDataRecordSize();
  
  /* Get record size for GData */
  int GDataRecordSize() { return _gRecSize; }

  virtual void SetParentValue(const char *value) = 0;
  
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
  virtual void ResetMaxSymSize() { _maxSymWidth = _maxSymHeight =
      _maxSymDepth = 0.0; }
  virtual void GetMaxSymSize(Coord &width, Coord &height, Coord &depth) {
    width = _maxSymWidth; height = _maxSymHeight; depth = _maxSymDepth;
  }

  virtual void DrawGDataArray(ViewGraph *view, WindowRep *win,
			      void **gdataArray, int num,
			      int &recordsProcessed,
			      Boolean timeoutAllowed) = 0;

  /* Get the AttrInfo for a GData attribute. The argument should be
     one of the MappingCmd_??? constants defined in MappingInterp.h */
  virtual AttrInfo *MapGAttr2TAttr(int which_attr) { return 0; }
  virtual char *MapTAttr2GAttr(char *tname) { return 0; }
  
  /* Get the AttrInfo for shape attribute i */
  virtual AttrInfo *MapShapeAttr2TAttr(int i) { return 0; }

  /* Hint for current focus in GData */
  void SetFocusId(RecId id);
  RecId GetFocusId();
  
  /* Setting and getting offsets of GData attributes */
  GDataAttrOffset*			GetGDataOffset(void)		{ return _gOffset; }
  const GDataAttrOffset*	GetGDataOffset(void) const	{ return _gOffset; }
		void	SetGDataOffset(GDataAttrOffset *offset) { _gOffset = offset; }
  
  void InsertUserData(void *data) { _userData = data; }
  void *GetUserData() { return _userData; }

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Moved here from Shape.h...

#define GetAttr(gdataRecP, attrName, attrType, offset) \
	*((attrType *)(gdataRecP+offset->attrName))

  inline RecId GetRecId(const char *gdataRecP) const
  {
    if (_gOffset->_recIdOffset < 0)
      return GetDefaultRecId();
    return GetAttr(gdataRecP, _recIdOffset, RecId, _gOffset);
  }

  inline Coord GetX(const char *gdataRecP) const
  {
    if (_gOffset->_xOffset < 0)
      return GetDefaultX();
    return GetAttr(gdataRecP, _xOffset, Coord, _gOffset);
  }

  inline Coord GetY(const char *gdataRecP) const
  {
    if (_gOffset->_yOffset < 0)
      return GetDefaultY();
    return GetAttr(gdataRecP, _yOffset, Coord, _gOffset);
  }

  inline Coord GetZ(const char *gdataRecP) const
  {
    if (_gOffset->_zOffset < 0)
      return GetDefaultZ();
    return GetAttr(gdataRecP, _zOffset, Coord, _gOffset);
  }

  inline PColorID GetColor(const char* gdataRecP) const
  {
    if (_gOffset->_colorOffset < 0) {
      return GetColoring().GetForeground();
    } else {
      return GetPColorID(gdataRecP);
    }
  }

  inline Coord GetSize(const char *gdataRecP) const
  {
    if (_gOffset->_sizeOffset < 0)
      return GetDefaultSize();
    return GetAttr(gdataRecP, _sizeOffset, Coord, _gOffset);
  }

  inline Pattern GetPattern(const char *gdataRecP) const
  {
    if (_gOffset->_patternOffset < 0)
      return GetDefaultPattern();
    return GetAttr(gdataRecP, _patternOffset, Pattern, _gOffset);
  }

  inline Coord GetOrientation(const char *gdataRecP) const
  {
    if (_gOffset->_orientationOffset < 0)
      return GetDefaultOrientation();
    return GetAttr(gdataRecP, _orientationOffset, Coord, _gOffset);
  }

  inline ShapeID GetShape(const char *gdataRecP) const
  {
    if (_gOffset->_shapeOffset < 0)
      return GetDefaultShape();
    return GetAttr(gdataRecP, _shapeOffset, ShapeID, _gOffset);
  }

  inline Coord GetShapeAttr(const char *gdataRecP, int attrNum) const
  {
    if (_gOffset->_shapeAttrOffset[attrNum] < 0) {
      const ShapeAttr *attrs = GetDefaultShapeAttrs();
      return attrs[attrNum];
    }
    return GetAttr(gdataRecP, _shapeAttrOffset[attrNum], Coord, _gOffset);
  }

  inline Coord GetShapeAttr0(const char *gdataRecP) const
  {
    if (_gOffset->_shapeAttrOffset[0] < 0) {
      const ShapeAttr *attrs = GetDefaultShapeAttrs();
      return attrs[0];
    }
    return GetAttr(gdataRecP, _shapeAttrOffset[0], Coord, _gOffset);
  }

  inline Coord GetShapeAttr1(const char *gdataRecP) const
  {
    if (_gOffset->_shapeAttrOffset[1] < 0){
      const ShapeAttr *attrs = GetDefaultShapeAttrs();
      return attrs[1];
    }
    return GetAttr(gdataRecP, _shapeAttrOffset[1], Coord, _gOffset);
  }

  inline Coord GetShapeAttr2(const char *gdataRecP) const
  {
    if (_gOffset->_shapeAttrOffset[2] < 0){
      const ShapeAttr *attrs = GetDefaultShapeAttrs();
      return attrs[2];
    }
    return GetAttr(gdataRecP, _shapeAttrOffset[2], Coord, _gOffset);
  }

  inline Coord GetShapeAttr3(const char *gdataRecP) const
  {
    if (_gOffset->_shapeAttrOffset[3] < 0){
      const ShapeAttr *attrs = GetDefaultShapeAttrs();
      return attrs[3];
    }
    return GetAttr(gdataRecP, _shapeAttrOffset[3], Coord, _gOffset);
  }

  inline Coord GetShapeAttr4(const char *gdataRecP) const
  {
    if (_gOffset->_shapeAttrOffset[4] < 0){
      const ShapeAttr *attrs = GetDefaultShapeAttrs();
      return attrs[4];
    }
    return GetAttr(gdataRecP, _shapeAttrOffset[4], Coord, _gOffset);
  }

  // hack alert: GetLineWidth really returns ShapeAttr4 (temporarily)
  inline int GetLineWidth(const char *gdataRecP) const
  {
    return int(GetShapeAttr4(gdataRecP)+0.5);
  }

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

  // Bounding box location is given relative to symbol's x, y location
  inline void
  GetBoundingBox(const char *gdataRecP, Coord &ULx, Coord &ULy, Coord &LRx,
      Coord &LRy) const
  {
    if (_gOffset->_bbULxOffset < 0) {
      ULx = GetDefaultBbULx();
    } else {
      ULx = GetAttr(gdataRecP, _bbULxOffset, Coord, _gOffset);
    }
    if (_gOffset->_bbULyOffset < 0) {
      ULy = GetDefaultBbULy();
    } else {
      ULy = GetAttr(gdataRecP, _bbULyOffset, Coord, _gOffset);
    }

    if (_gOffset->_bbLRxOffset < 0) {
      LRx = GetDefaultBbLRx();
    } else {
      LRx = GetAttr(gdataRecP, _bbLRxOffset, Coord, _gOffset);
    }
    if (_gOffset->_bbLRyOffset < 0) {
      LRy = GetDefaultBbLRy();
    } else {
      LRy = GetAttr(gdataRecP, _bbLRyOffset, Coord, _gOffset);
    }
  }

  // Bounding box relative to symbol x, y.
  inline void SetBoundingBox(char *gdataP, Coord ULx, Coord ULy, Coord LRx,
      Coord LRy)
  {
    if (ULx > LRx) {
      Coord tmp = ULx;
      ULx = LRx;
      LRx = tmp;
    }
    if (ULy < LRy) {
      Coord tmp = ULy;
      ULy = LRy;
      LRy = tmp;
    }

    if (_gOffset->_bbULxOffset < 0) {
      SetDefaultBbULx(ULx);
    } else {
      *((Coord *)(gdataP + _gOffset->_bbULxOffset)) = ULx;
    }

    if (_gOffset->_bbULyOffset < 0) {
      SetDefaultBbULy(ULy);
    } else {
      *((Coord *)(gdataP + _gOffset->_bbULyOffset)) = ULy;
    }

    if (_gOffset->_bbLRxOffset < 0) {
      SetDefaultBbLRx(LRx);
    } else {
      *((Coord *)(gdataP + _gOffset->_bbLRxOffset)) = LRx;
    }

    if (_gOffset->_bbLRyOffset < 0) {
      SetDefaultBbLRy(LRy);
    } else {
      *((Coord *)(gdataP + _gOffset->_bbLRyOffset)) = LRy;
    }
  }
  
protected:
  void InitializeDefaults();
  
  /* Set new defaults. Should only be done in derived classes.*/
  void SetDefaultX(Coord x) { _defaults._x = x; }
  void SetDefaultY(Coord y) { _defaults._y = y; }
  void SetDefaultZ(Coord z) { _defaults._z = z; }
  void SetDefaultSize(Coord size) { _defaults._size = size; }
  void SetDefaultPattern(Pattern pattern) { _defaults._pattern = pattern; }
  void SetDefaultOrientation(Coord orientation) {
    _defaults._orientation = orientation; }
  void SetDefaultBbULx(Coord x) { _defaults._bbULx = x; }
  void SetDefaultBbULy(Coord y) { _defaults._bbULy = y; }
  void SetDefaultBbLRx(Coord x) { _defaults._bbLRx = x; }
  void SetDefaultBbLRy(Coord y) { _defaults._bbLRy = y; }
  void SetDefaultShape(ShapeID shapeID, int numAttr = 0, 
		       ShapeAttr *shapeAttr = NULL);
  void SetDefaultShapeAttr(int attrNum, Coord shapeAttr);

  Coord _maxSymWidth; /* bounding box for symbol */
  Coord _maxSymHeight;
  Coord _maxSymDepth;

private:
  ObjectValid _objectValid;

  GDataAttrOffset *_gOffset; /* offset of GData attributes */

  int _gRecSize; /* gdata record size */
  
  /* incarnation: how many TDataMaps have been created */
  static int _incarnation;
  
  /* Create name of GData */
  char *CreateGDataName(const char *tdataName, char *mappingName);
  char *CreateGDataPath(char *gdataName);

  /* default values */
  GDataRec _defaults;
  int _numShapeAttr; // Number of shape attrs that are valid
  
  /* For hint calculation */
  Boolean _hintInitialized;  /* TRUE if _minX, _maxX, _numPages initialized */
  Coord _minX, _maxX, _numPages; /* minimum and max X values, and # of pages*/
  TData *_hintTdata; /* tdata for the above cached info */
  
  char *_mappingName;

  // Note: the "logical" TData is the TData of the view as far as the user
  // is concerned; the "physical" TData is the same as the logical unless
  // the mapping is a TAttrLink slave -- in this case, the "physical"
  // TData is the result of the join(s) with the master table(s).
  TData *_logicalTdata;
  TData *_tdata; // "physical" tdata
  int _physTdTimestamp; // when physical tdata last got changed

  GData *_gdata;
  VisualFlag _dynamicArgs; /* attributes that are set dynamically */
  
  unsigned long _dynamicAttrs;
  
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

  char *_stringXTableName;
  char *_stringYTableName;
  char *_stringZTableName;
  char *_stringGenTableName;
};

//******************************************************************************
#endif

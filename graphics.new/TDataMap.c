/*
  ========================================================================
  DEVise Data Visualization Software
  (c) Copyright 1992-1999
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
  Revision 1.34  1999/05/21 14:52:45  wenger
  Cleaned up GData-related code in preparation for including bounding box
  info.

  Revision 1.33  1999/04/16 20:59:23  wenger
  Fixed various bugs related to view symbols, including memory problem
  with MappingInterp dimensionInfo; updated create_condor_session script
  to take advantage of view symbol TData switching capability.

  Revision 1.32  1999/03/03 18:22:01  wenger
  Fixed bugs 426 and 432 (problems with '5' (home) key); fixed bugs 466
  and 467 (query errors with sorted attributes); minor improvements to
  view symbols.

  Revision 1.31  1999/03/01 23:09:11  wenger
  Fixed a number of memory leaks and removed unused code.

  Revision 1.30  1998/11/09 20:33:26  wenger
  Fixed bug 433 (drill-down problem); improved debug output in various
  related modules.

  Revision 1.29  1998/11/06 17:59:53  wenger
  Multiple string tables fully working -- allows separate tables for the
  axes in a given view.

  Revision 1.28  1998/11/04 20:34:01  wenger
  Multiple string tables partly working -- loading and saving works, one
  table per mapping works; need multiple tables per mapping, API and GUI,
  saving to session, sorting.

  Revision 1.27  1998/08/17 18:51:55  wenger
  Updated solaris dependencies for egcs; fixed most compile warnings;
  bumped version to 1.5.4.

  Revision 1.26  1998/05/06 22:04:59  wenger
  Single-attribute set links are now working except where the slave of
  one is the master of another.

  Revision 1.25  1998/04/30 14:24:21  wenger
  DerivedTables are now owned by master views rather than links;
  views now unlink from master and slave links in destructor.

  Revision 1.24  1998/04/29 17:53:56  wenger
  Created new DerivedTable class in preparation for moving the tables
  from the TAttrLinks to the ViewDatas; found bug 337 (potential big
  problems) while working on this.

  Revision 1.23  1998/04/28 18:03:15  wenger
  Added provision for "logical" and "physical" TDatas to mappings,
  instead of creating new mappings for slave views; other TAttrLink-
  related improvements.

  Revision 1.22  1998/03/04 19:11:05  wenger
  Fixed some more dynamic memory errors.

  Revision 1.21  1998/02/19 23:25:17  wenger
  Improved color library and got client/server test code to work
  (except for setting colors by RGB): reduced compile interdependencies,
  especially in color library; color and utils libraries install headers
  as per code reorg plans; added standard DEVise headers to all color
  manager files; moved color initialization into Display constructors;
  fixed some compile warnings throughout the code.

  Revision 1.20  1997/11/24 23:15:19  weaver
  Changes for the new ColorManager.

  Revision 1.19.10.1  1997/05/21 20:40:48  weaver
  Changes for new ColorManager

  Revision 1.19  1996/12/30 23:57:37  andyt
  First version with support for Embedded Tcl/Tk windows. Added new
  ETkWindow symbol shape. Improved the MappingInterp::MapGAttr2TAttr
  function to handle all GData attributes (used to only handle a subset).

  Revision 1.18  1996/11/23 21:17:16  jussi
  Removed reference to DEVISE_PAGESIZE. Removed StripPath() which
  is already in Util.c.

  Revision 1.17  1996/09/27 21:09:37  wenger
  GDataBin class doesn't allocate space for connectors (no longer used
  anyhow); fixed some more memory leaks and made notes in the code about
  some others that I haven't fixed yet; fixed a few other minor problems
  in the code.

  Revision 1.16  1996/07/10 00:02:41  jussi
  Added memory allocation checks. Got rid of TDataMapDispatch class.

  Revision 1.15  1996/07/02 22:47:07  jussi
  Rewrote interface for setting and querying maximum symbol
  size (bounding box).

  Revision 1.14  1996/06/21 19:33:21  jussi
  Replaced MinMax calls with MIN() and MAX().

  Revision 1.13  1996/03/25 23:01:37  jussi
  Removed reference to _view.

  Revision 1.12  1996/03/07 16:56:06  jussi
  Added association of TDataMap and ViewGraph.

  Revision 1.11  1996/03/05 23:26:49  jussi
  Added destructor which deletes dynamically allocated objects.

  Revision 1.10  1996/01/30 21:16:06  jussi
  Removed references to specific colors.

  Revision 1.9  1996/01/20 00:47:13  jussi
  Disabled debugging, which I forgot to do in last check-in.

  Revision 1.8  1996/01/20 00:44:23  jussi
  Fixed problem when record size of GData is less than that of
  TData and adding a dynamic field to GData causes the record
  size to be more than the record size of TData.

  Revision 1.7  1996/01/13 23:10:02  jussi
  Added support for Z attribute and shape attribute 2.

  Revision 1.6  1995/12/28 19:52:21  jussi
  Small fixes to remove compiler warnings.

  Revision 1.5  1995/12/14 15:41:24  jussi
  Added some debugging statements.

  Revision 1.4  1995/11/28 00:08:01  jussi
  Replaced constant RectShapeID with 0.

  Revision 1.3  1995/11/21 23:19:24  jussi
  Added copyright notice and cleaned up code.

  Revision 1.2  1995/09/05 22:15:54  jussi
  Added CVS header.
*/

#include <stdio.h>
#include <string.h>

#include "Init.h"
#include "TDataMap.h"
#include "GData.h"
#include "GDataBin.h"
#include "TData.h"
#include "QueryProc.h"
#include "Util.h"
#include "XColor.h"
#include "DevError.h"
#include "TimeStamp.h"
#include "StringStorage.h"

//#define DEBUG

/* counts how many TDataMaps have been created.
   Also used in creating the name of GData */
int TDataMap::_incarnation= 0; 

//******************************************************************************
// Constructors and Destructors
//******************************************************************************

TDataMap::TDataMap(char *name, TData *tdata, char *gdataName, 
		   int gdataRecSize, VisualFlag dynamicArgs,
                   unsigned long dynamicAttrs, int maxGDataPages,
		   VisualFlag *dimensionInfo, int numDimensions,
		   Boolean createGData)
{
#if defined(DEBUG)
  printf("TDataMap::TDataMap(%s)\n", name);
  printf("  dimensionInfo = %d\n", *dimensionInfo);
  printf("  numDimensions = %d\n", numDimensions);
#endif

  _gOffset = NULL;

  _incarnation++;

  _userData = NULL;

  _gdataAttrList = NULL;
  _createGData = createGData;
  
  _focusId = 0;
  _maxGDataPages = maxGDataPages;
  
  /* set defaults */
  _dimensionInfo = dimensionInfo;
  _numDimensions = numDimensions;
  _mappingName = name;
  _pixelWidth = 1;
  _dynamicAttrs = dynamicAttrs;
  _dynamicArgs = dynamicArgs;
  _tdata = tdata;
  _logicalTdata = tdata;
  _gRecSize = gdataRecSize;
  
  _gdataName = gdataName;
  if (!_gdataName)
    _gdataName = CreateGDataName(_tdata->GetName(), name);
  
  _gdataPath = CreateGDataPath(_gdataName);
  _gdata = NULL;
  
  if (_createGData) {
#ifdef DEBUG
    printf("Creating new instance of GData with recSize %d\n", _gRecSize);
#endif
    _gdata = new GData(_tdata, _gdataPath, _gRecSize, 
		       maxGDataPages * Init::PageSize());
    DOASSERT(_gdata, "Out of memory");
  }
  
  _defaults._x = 0.0;
  _defaults._y = 0.0;
  _defaults._z = 0.0;
  _defaults._size = 1.0;
  _defaults._pattern = Pattern0;
  _defaults._orientation = 0.0;
  _defaults._shape = 0;
  _defaults._bbULx = 0.0;
  _defaults._bbULy = 0.0;
  _defaults._bbLRx = 0.0;
  _defaults._bbLRy = 0.0;

  _numShapeAttr = MAX_SHAPE_ATTRS;
  for(unsigned int i = 0; i < (unsigned int)MAX_SHAPE_ATTRS; i++) {
    _defaults._shapeAttrs[i] = 0.1;
  }

  _maxSymWidth = 0.0;
  _maxSymHeight = 0.0;
  _maxSymDepth = 0.0;

  _hintInitialized = false;

  // Timestamp is 0 here in case mapping gets created after link somehow.
  _physTdTimestamp = 0;

  _stringXTableName = NULL;
  _stringYTableName = NULL;
  _stringZTableName = NULL;
  _stringGenTableName = NULL;

  _objectValid.Set();
}

TDataMap::~TDataMap()
{
  delete _gdata;
  _gdata = NULL;

  delete [] _stringXTableName;
  _stringXTableName = NULL;

  delete [] _stringYTableName;
  _stringYTableName = NULL;

  delete [] _stringZTableName;
  _stringZTableName = NULL;

  delete [] _stringGenTableName;
  _stringGenTableName = NULL;

  delete [] _gdataPath;
  _gdataPath = NULL;
}

//******************************************************************************
// Utility Functions
//******************************************************************************
// Central point for mapping->color conversions. Nowhere else!!!
// With all three of these, recPtr=NULL will return the default color.
// ONLY GetPColorID CAN "FAIL", BECAUSE PALETTES DON'T HAVE DEFAULTS.

PColorID	TDataMap::GetPColorID(const char* recPtr) const
{
	if ((GetGDataOffset()->_colorOffset >= 0) && (recPtr != NULL)) {
		return *(PColorID*)(recPtr + GetGDataOffset()->_colorOffset);
	} else {
		return nullPColorID;
    }
}

//TEMP -- does the color stuff really have to be treated specially??
ColorID		TDataMap::GetColorID(const char* recPtr) const
{
	PColorID	pcid = GetPColorID(recPtr);
	ColorID		cid = nullColorID;

	if (pcid != nullPColorID)			// Try the mapped palette index
		cid = PM_GetColorID(pcid);

	if (cid == nullColorID)				// Try the map default foreground
		cid = GetColoring().GetForeground();

	if (cid == nullColorID)				// Try the global default foreground
		cid = defForeColor;

	return cid;
}

XColorID	TDataMap::GetXColorID(const char* recPtr) const
{
	PColorID	pcid = GetPColorID(recPtr);
	XColorID	xcid = nullXColorID;
	
	if (pcid != nullPColorID)			// Try the mapped palette index
		xcid = AP_GetXColorID(pcid);

	if (xcid == nullXColorID)			// Try the map default foreground
		xcid = CM_GetXColorID(GetColoring().GetForeground());

	if (xcid == nullXColorID)			// Try the global default foreground
		xcid = CM_GetXColorID(::GetColorID(defForeColor));

	return xcid;
}

//******************************************************************************

void TDataMap::SetDimensionInfo(VisualFlag *dimensionInfo, int numDimensions)
{
#if defined(DEBUG)
  printf("TDataMap(%s)::SetDimensionInfo()\n", GetName());
  printf("  old: 0x%p\n", _dimensionInfo);
  printf("  new: 0x%p\n", dimensionInfo);
#endif

  delete _dimensionInfo;
  _dimensionInfo = dimensionInfo;
  _numDimensions = numDimensions;
}

/****************************************************************************
Map record to symbol by calling user defined Map() function.
Fill in the values for the symbol. This is called by the query processor
****************************************************************************/

#ifdef TDATAMAP_MAP_TO_SYMBOL
void TDataMap::MapToSymbol(TData *tdata, RecId recId, void *rec, Symbol *sym)
{
  /* init default values */
  sym->recId = recId;
  sym->x = _defaults._x;
  sym->y = _defaults._y;
  sym->z = _defaults._z;
  sym->color = GetColoring().GetForeground();
  sym->size = _defaults._size;
  sym->pattern = _defaults._pattern; 
  sym->orientation = _defaults._orientation;
  sym->shapeID = _defaults._shapeId;
  sym->numShapeAttr = _defaults._numShapeAttr;
  for(int i = 0; i < MAX_SHAPE_ATTRS; i++) {
    sym->shapeAttrs[i] = _defaults._shapeAttrs[i];
  }

  /* call the user-defined mapping function */
  Map(tdata, recId, rec, sym);
  
  /* find the bounding box for the symbol */
  ShapeRegistrar::SymbolBoundingBox(sym);
  
  /* update the maximum bounding box found so far */
  _deltaX = MAX(sym->width, _deltaX);
  _deltaY = MAX(sym->height, _deltaY);
}
#endif

/************************************************************************
Hint calculation
************************************************************************/
Boolean TDataMap::PageHint(TData *tdata, Coord x, Boolean isPrefetch,
			   RecId &hintId)
{
  return false;

#ifdef OLD_CODE
  if (!_hintInitialized || tdata != _hintTdata ||
      _numPages != tdata->NumPages() ) {
    /* initialize/reinit hint */
    
    _hintInitialized = false;	/* set to true later */
    _hintTdata = tdata;
    
    _numPages = tdata->NumPages();
    if (_numPages == 0)
      return false;
    
    /* get 1st page and last page and test */
    int numRecs; 
    RecId startRid;
    void **recs;
    tdata->GetPage(tdata->FirstPage(), numRecs, startRid,recs,isPrefetch);
    if (numRecs == 0) {
      tdata->FreePage(tdata->FirstPage(), Stay);
      return false;
    }
    ConvertToGData(recs, 1, _tmpPtr);
    tdata->FreePage(tdata->FirstPage(), Stay);
    
    if (GetDynamicArgs() & VISUAL_X)
      _minX = ((GDataBinRec *)_tmpPtr[0])->x;
    else
      _minX = GetDefaultX();
    
    tdata->GetPage(tdata->LastPage(), numRecs, startRid, recs,isPrefetch);
    if (numRecs == 0) {
      tdata->FreePage(tdata->LastPage(), Stay);
      return false;
    }
    
    ConvertToGData(&recs[numRecs - 1], 1, _tmpPtr);
    tdata->FreePage(tdata->LastPage(), Stay);
    
    if (GetDynamicArgs() & VISUAL_X)
      _maxX = ((GDataBinRec *)_tmpPtr[0])->x;
    else
      _minX = GetDefaultX();
    
    _hintInitialized = true;
  }
  
  if ( x < _minX)
    pageNum = tdata->FirstPage();
  else if (x > _maxX)
    pageNum = tdata->LastPage();
  else {
    pageNum= ((int)((x-_minX)/(_maxX-_minX)*_numPages))+tdata->FirstPage();
    if (pageNum > tdata->LastPage())
      pageNum = tdata->LastPage();
  }
  
  return true;
#endif
}

/***********************************************************
Make a name for GData
************************************************************/
char *TDataMap::CreateGDataName(char *tdataName, char *mappingName)
{
  char *name;
  char *tname= StripPath(tdataName);
  name = new char [strlen(tname) + strlen(mappingName) + 20];
  sprintf(name,"%s.%d.%s.G", tname, _incarnation, mappingName);
  return name;
}

/*********************************************************
Make a path for storing gdata
*********************************************************/
char *TDataMap::CreateGDataPath(char *gdataName)
{
  char *tmpDir = Init::TmpDir();
  char *name = new char [strlen(gdataName) + 1 + strlen(tmpDir) + 1];//TEMP --  leaked
  sprintf(name, "%s/%s", tmpDir, gdataName);
  return name;
}

void TDataMap::SetPhysTData(TData *tdata)
{
  DOASSERT(_objectValid.IsValid(), "operation on invalid object");
  if (tdata->RecSize() != _logicalTdata->RecSize()) {
    char errBuf[1024];
    sprintf(errBuf, "New physical TData %s is incompatible with logical "
	"TData %s", tdata->GetName(), _logicalTdata->GetName());
    reportErrNosys(errBuf);
  } else {
    //TEMP -- possibly also compare attribute lists
    _tdata = tdata;
    _physTdTimestamp = TimeStamp::NextTimeStamp();
    ResetGData(_gRecSize);
  }
}

int TDataMap::TDataRecordSize()
{
  return _tdata->RecSize();
}

/* Hint for current focus in GData */
void TDataMap::SetFocusId(RecId id) {
  _focusId = id;
}

RecId TDataMap::GetFocusId() {
  return _focusId;
}

void TDataMap::SetDefaultShapeAttr(int attrNum, Coord shapeAttr)
{
  if (attrNum < 0 || attrNum >= (int)MAX_SHAPE_ATTRS) {
    fprintf(stderr,"Mapping::SetDefaultShapeAttr: attrNum %d\n", attrNum);
    Exit::DoExit(1);
  }
  _defaults._shapeAttrs[attrNum] = shapeAttr;
}

void TDataMap::SetDefaultShape(ShapeID shapeID, int numAttr,
			       ShapeAttr *shapeAttr)
{
  if (numAttr > (int)MAX_SHAPE_ATTRS) {
    fprintf(stderr,"Mapping::SetDefaultShape: too many attrs %d\n", numAttr);
    Exit::DoExit(1);
  }
  
  _defaults._shape = shapeID;
  _numShapeAttr = numAttr;
  if (numAttr > 0) {
    for(int i = 0; i < numAttr; i++) {
      _defaults._shapeAttrs[i] = shapeAttr[i];
    }
    for(int i = numAttr; i < MAX_SHAPE_ATTRS; i++) {
      _defaults._shapeAttrs[i] = 0.1;
    }
  }
}

void
TDataMap::CreateGData(int gRecSize)
{
  _createGData = true;
  ResetGData(gRecSize);
}

void TDataMap::ResetGData(int gRecSize)
{
#if defined(DEBUG)
  printf("TDataMap::ResetGData with recSize %d\n", gRecSize);
#endif

  if (_gdata) {
    QueryProc::Instance()->ClearGData(_gdata);
    delete _gdata;
    _gdata = NULL;
  }

  _gRecSize = gRecSize;

  if (_createGData) {
#ifdef DEBUG
    printf("Creating new instance of GData with recSize %d\n", _gRecSize);
#endif
    _gdata = new GData(_tdata, _gdataPath, _gRecSize,
		       _maxGDataPages * Init::PageSize());
    QueryProc::Instance()->ResetGData(_tdata, _gdata);
  }
}

void
TDataMap::SetStringTable(TableType type, char *name)
{
#if defined(DEBUG)
  printf("TDataMap(%s)::SetStringTable(%d, %s)\n", GetName(), type,
    name ? name : "NULL");
#endif

  char **tableNameP = TableType2NameP(type);

  // Note: we're storing the table name, rather than a pointer to the table,
  // here in case the string table is destroyed while this mapping still
  // exists -- if the user loads a different strings file, for example.
  // RKW 1998-11-03.
  delete [] *tableNameP;
  *tableNameP = CopyString(name);
  if (!StringStorage::FindByName(*tableNameP)) {
    fprintf(stderr, "Warning: string table %s does not exist\n",
      *tableNameP);
  }
}

StringStorage *
TDataMap::GetStringTable(TableType type)
{
#if defined(DEBUG)
  printf("TDataMap(%s)::GetStringTable(%d)\n", GetName(), type);
#endif

  char **tableNameP = TableType2NameP(type);

  StringStorage *stringTable = StringStorage::FindByName(*tableNameP);
  if (!stringTable) {
	fprintf(stderr,
	  "Warning: string table %s not found; creating it\n", *tableNameP);
    stringTable = new StringStorage(*tableNameP);
  }
  return stringTable;
}

char **
TDataMap::TableType2NameP(TableType type)
{
  char **tableNameP;

  switch (type) {
  case TableX:
    tableNameP = &_stringXTableName;
    break;

  case TableY:
    tableNameP = &_stringYTableName;
    break;

  case TableZ:
    tableNameP = &_stringZTableName;
    break;

  case TableGen:
    tableNameP = &_stringGenTableName;
    break;
  
  default:
    DOASSERT(0, "Invalid TableType");
    break;
  }

  return tableNameP;
}

//******************************************************************************

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
  Revision 1.88  1999/06/04 20:52:52  wenger
  Improved error message.

  Revision 1.87  1999/05/28 16:32:44  wenger
  Finished cleaning up bounding-box-related code except for PolyLineFile
  symbol type; fixed bug 494 (Vector symbols drawn incorrectly); improved
  drawing of Polyline symbols.

  Revision 1.86  1999/05/27 17:45:13  wenger
  Lots of cleanup of MappingInterp code.

  Revision 1.85  1999/05/26 19:50:51  wenger
  Added bounding box info to GData, so that the selection of records by the
  visual filter is more accurate.  (Note that at this time the bounding box
  info does not take into account symbol orientation; symbol alignment is
  taken into account somewhat crudely.) This includes considerable
  reorganization of the mapping-related classes.  Fixed problem with
  pixelSize getting used twice in Rect shape (resulted in size being the
  square of pixel size).

  Revision 1.84  1999/05/21 14:52:32  wenger
  Cleaned up GData-related code in preparation for including bounding box
  info.

  Revision 1.83  1999/05/20 15:17:54  wenger
  Fixed bugs 490 (problem destroying piled parent views) and 491 (problem
  with duplicate elimination and count mappings) exposed by Tim Wilson's
  two-station session.

  Revision 1.82  1999/03/03 18:21:56  wenger
  Fixed bugs 426 and 432 (problems with '5' (home) key); fixed bugs 466
  and 467 (query errors with sorted attributes); minor improvements to
  view symbols.

  Revision 1.81  1999/01/04 15:33:32  wenger
  Improved View symbol code; removed NEW_LAYOUT and VIEW_SHAPE conditional
  compiles; added code (GUI is currently disabled) to manually set view
  geometry (not yet saved to sessions).

  Revision 1.80  1998/11/25 22:31:17  wenger
  Reduced the amount of memory allocated in the MappingInterp constructor,
  mainly by improving the AttrList class.

  Revision 1.79  1998/11/20 18:38:52  wenger
  Fixed bug 440 (crash caused by empty mapping command for X in combination
  with other errors).

  Revision 1.78  1998/11/11 14:30:59  wenger
  Implemented "highlight views" for record links and set links; improved
  ClassDir::DestroyAllInstances() by having it destroy all links before
  it destroys anything else -- this cuts down on propagation problems as
  views are destroyed; added test code for timing a view's query and draw.

  Revision 1.77  1998/11/09 20:33:22  wenger
  Fixed bug 433 (drill-down problem); improved debug output in various
  related modules.

  Revision 1.76  1998/11/06 17:59:51  wenger
  Multiple string tables fully working -- allows separate tables for the
  axes in a given view.

  Revision 1.75  1998/11/04 20:33:58  wenger
  Multiple string tables partly working -- loading and saving works, one
  table per mapping works; need multiple tables per mapping, API and GUI,
  saving to session, sorting.

  Revision 1.74  1998/10/21 17:16:41  wenger
  Fixed bug 101 (problems with the '5' (home) key); added "Set X, Y to
  Show All" (go home) button to Query dialog; fixed bug 421 (crash when
  closing set link sessions); fixed bug 423 (saving session file over
  directory).

  Revision 1.73  1998/08/17 18:51:50  wenger
  Updated solaris dependencies for egcs; fixed most compile warnings;
  bumped version to 1.5.4.

  Revision 1.72  1998/07/10 21:20:06  wenger
  Minor cleanups and improvements.

  Revision 1.71  1998/04/16 21:51:40  wenger
  Committed Sanjay's text code.

  Revision 1.70  1998/02/26 22:59:50  wenger
  Added "count mappings" to views, except for API and GUI (waiting for
  Dongbin to finish his mods to ParseAPI); conditionaled out unused parts
  of VisualFilter struct; did some cleanup of MappingInterp class.

  Revision 1.69  1998/01/30 21:53:16  wenger
  Did some cleaning up of the MappingInterp and NativeExpr code
  (NativeExpr still needs a lot more); NativeExpr code can now
  parse expressions containing constant strings (they are treated
  as numerical zero for now) (this fixes bug 275).

  Revision 1.68  1998/01/07 19:29:50  wenger
  Merged cleanup_1_4_7_br_4 thru cleanup_1_4_7_br_5 (integration of client/
  server library into Devise); updated solaris, sun, linux, and hp
  dependencies.

  Revision 1.67  1997/12/04 18:31:38  wenger
  Merged new expression evaluation code thru the expression_br_2 tag.

  Revision 1.66  1997/11/24 23:15:09  weaver
  Changes for the new ColorManager.

  Revision 1.65  1997/11/18 23:26:55  wenger
  First version of GData to socket capability; removed some extra include
  dependencies; committed test version of TkControl::OpenDataChannel().

  Revision 1.64  1997/11/12 15:46:36  wenger
  Merged the cleanup_1_4_7_br branch through the cleanup_1_4_7_br_2 tag
  into the trunk.

  Revision 1.63.4.2  1997/12/15 23:44:32  ssl
  Fixed the bug where a recId in a complex expression was not handled properly.

  Revision 1.63.4.1  1997/11/04 18:48:30  wenger
  Fixed bug 238 (core dump in search engine demo).

  Revision 1.63.6.1  1997/12/03 22:52:26  whh
  Devise now uses new C++ expression evaluation code instead of Tcl
  interpreter.

  Revision 1.63  1997/08/27 14:47:40  wenger
  Removed some unnecessary include dependencies.

  Revision 1.62  1997/08/20 22:11:01  wenger
  Merged improve_stop_branch_1 through improve_stop_branch_5 into trunk
  (all mods for interrupted draw and user-friendly stop).

  Revision 1.61.2.3  1997/08/15 23:06:30  wenger
  Interruptible drawing now pretty much working for TDataViewX class,
  too (connector drawing may need work, needs a little more testing).
  (Some debug output still turned on.)

  Revision 1.61.2.2  1997/08/14 16:16:02  wenger
  Statistics, etc., now work correctly for timed-out draw in ViewScatter-
  type views; bumped up version because of improved stop capability.

  Revision 1.61.2.1  1997/08/07 16:56:36  wenger
  Partially-complete code for improved stop capability (includes some
  debug code).

  Revision 1.61  1997/07/16 15:49:19  wenger
  Moved string allocation/deallocation within StringStorage class, fixed
  memory leak of strings.

  Revision 1.60  1997/07/15 14:29:57  wenger
  Moved hashing of strings from TData*Interp classes to MappingInterp
  class; cleaned up a few extra includes of StringStorage.h.

  Revision 1.59.2.1  1997/05/21 20:40:39  weaver
  Changes for new ColorManager

  Revision 1.59  1997/05/08 00:18:56  wenger
  'Can't find attribute in mapping' warning not printed for recId's.

  Revision 1.58  1997/05/06 17:20:39  wenger
  Now gives warning if your mapping has a TData attribute that doesn't
  exist!

  Revision 1.57  1997/04/30 21:45:38  wenger
  Fixed non-constant strings in complex mappings bug; TDataAsciiInterp
  no longer gives warning message on blank data lines; added makefile
  targets to make a Purify'd version of multi; fixed uninitialized memory
  read in the DList code; fixed bug that caused 1.4 version of multi to
  always crash; better error messages in DTE command parser; version is
  now 1.4.4.

  Revision 1.56  1997/04/29 17:35:12  wenger
  Minor fixes to new text labels; added fixed text label shape;
  CheckDirSpace() no longer prints an error message if it can't get disk
  status.

  Revision 1.55  1997/04/25 23:15:53  ssl
  turned off debug code.

  Revision 1.53  1997/04/21 22:53:44  guangshu
  Added function MapTAttr2GAttr.

  Revision 1.52  1997/04/16 18:53:37  wenger
  Text labels can now show non-string attributes; fixed a bug in constant
  strings in mapping (non-terminated string); added constant attributes to
  GData attribute list; commented out some debug code in mapping dialog.

  Revision 1.51  1997/04/14 17:25:25  beyer
  Added debugging.

  Revision 1.50  1997/04/09 23:31:23  beyer
  String constants in the mapping now work.

  Revision 1.49  1997/03/23 23:46:14  donjerko
  *** empty log message ***

  Revision 1.48  1997/03/19 19:41:48  andyt
  Embedded Tcl/Tk windows are now sized in data units, not screen pixel
  units. The old list of ETk window handles (WindowRep class) has been
  replaced by a list of ETkInfo structs, each with fields for the window
  handle, x-y coordinates, name of the Tcl script, and an "in_use"
  flag. Added an ETk_Cleanup() procedure that gets called inside
  View::ReportQueryDone and destroys all ETk windows that are not marked
  as in_use.

  Revision 1.47  1997/02/26 16:31:37  wenger
  Merged rel_1_3_1 through rel_1_3_3c changes; compiled on Intel/Solaris.

  Revision 1.46.4.1  1997/02/13 18:11:44  ssl
  Added a check to the user interface asking when user links two different
  data sets with a record link

  Revision 1.46  1996/12/30 23:57:35  andyt
  First version with support for Embedded Tcl/Tk windows. Added new
  ETkWindow symbol shape. Improved the MappingInterp::MapGAttr2TAttr
  function to handle all GData attributes (used to only handle a subset).

  Revision 1.45  1996/12/30 16:12:10  wenger
  Removed unecessary assertion in MappingInterp::ConvertToGDataSimple().

  Revision 1.44  1996/11/23 21:19:31  jussi
  Removed failing support for variable-sized records.

  Revision 1.43  1996/11/13 16:57:05  wenger
  Color working in direct PostScript output (which is now enabled);
  improved ColorMgr so that it doesn't allocate duplicates of colors
  it already has, also keeps RGB values of the colors it has allocated;
  changed Color to GlobalColor, LocalColor to make the distinction
  explicit between local and global colors (_not_ interchangeable);
  fixed global vs. local color conflict in View class; changed 'dali'
  references in command-line arguments to 'tasvir' (internally, the
  code still mostly refers to Dali).

  Revision 1.42  1996/11/07 22:40:29  wenger
  More functions now working for PostScript output (FillPoly, for example);
  PostScript output also working for piled views; PSWindowRep member
  functions no longer do so much unnecessary rounding to integers (left
  over from XWindowRep); kept in place (but disabled) a bunch of debug
  code I added while figuring out piled views; added PostScript.doc file
  for some high-level documentation on the PostScript output code.

  Revision 1.41  1996/09/27 15:53:20  wenger
  Fixed a number of memory leaks.

  Revision 1.40  1996/09/10 20:07:23  wenger
  High-level parts of new PostScript output code are in place (conditionaled
  out for now so that the old code is used until the new code is fully
  working); changed (c) (tm) in windows so images are not copyrighted
  by DEVise; minor bug fixes; added more debug code in the course of working
  on the PostScript stuff.

  Revision 1.39  1996/09/04 21:25:02  wenger
  'Size' in mapping now controls the size of Dali images; improved Dali
  interface (prevents Dali from getting 'bad window' errors, allows Devise
  to kill off the Dali server); added devise.dali script to automatically
  start Dali server along with Devise; fixed bug 037 (core dump if X is
  mapped to a constant); improved diagnostics for bad command-line arguments.

  Revision 1.38  1996/08/04 21:59:49  beyer
  Added UpdateLinks that allow one view to be told to update by another view.
  Changed TData so that all TData's have a DataSource (for UpdateLinks).
  Changed all of the subclasses of TData to conform.
  A RecFile is now a DataSource.
  Changed the stats buffers in ViewGraph to be DataSources.

  Revision 1.37  1996/07/19 02:53:40  jussi
  Increase number of shapes to 15 (LineShape and LineShadeShape
  were added). RecId is now the first GData attribute in all
  GData records.

  Revision 1.36  1996/07/15 21:14:06  jussi
  Minor cleanup.

  Revision 1.35  1996/07/15 17:02:01  jussi
  Added support for string attributes in GData.

  Revision 1.34  1996/07/10 00:03:37  jussi
  Replaced TDataMapDispatch with TDataMap.

  Revision 1.33  1996/07/03 22:15:25  jussi
  Fixed an index error.

  Revision 1.32  1996/07/02 22:45:59  jussi
  The bounding box of symbols is now correctly computed. Scatter
  plots sometimes did not have all necessary data displayed in
  them, as bounding box used to be incorrectly computed.

  Revision 1.31  1996/06/27 19:06:54  jussi
  Merged 3D block shape into 2D rect shape, the appropriate shape
  is chosen based on current view setting. Removed Block and 3DVector
  shapes. Added empty default 3D drawing routine to all other
  shapes.

  Revision 1.30  1996/06/16 01:53:35  jussi
  Added PolylineShape, PolylineFileShape, and TextLabelShape.
  Made cmdFlag and cmdAttrFlag unsigned long int's.

  Revision 1.29  1996/06/04 14:20:25  wenger
  Ascii data can now be read from session files (or other files
  where the data is only part of the file); added some assertions
  to check for pointer alignment in functions that rely on this;
  Makefile changes to make compiling with debugging easier.

  Revision 1.28  1996/05/22 21:05:09  jussi
  Added HighLowShape. Added tentative version of GifImageShape.

  Revision 1.27  1996/05/07 16:39:08  jussi
  Added MapGAttr2TAttr() method for translating GData attributes
  to TData attributes.

  Revision 1.26  1996/04/23 20:35:40  jussi
  Added Segment shape which just connects two end points.

  Revision 1.25  1996/04/16 20:53:47  jussi
  Added HorLineShape. Replaced DoExit() calls with DOASSERT macro.

  Revision 1.24  1996/04/15 16:08:09  jussi
  Moved call to SetGDataOffset() to the constructor.

  Revision 1.23  1996/04/09 23:05:40  jussi
  Added View parameter to DrawGDataArray().

  Revision 1.22  1996/03/26 15:34:55  wenger
  Fixed various compile warnings and errors; added 'clean' and
  'mostlyclean' targets to makefiles; changed makefiles so that
  object lists are derived from source lists.

  Revision 1.21  1996/02/05 19:59:21  yuc
  updating _shapes, get 3Dvector.

  Revision 1.20  1996/01/29 23:57:07  jussi
  Removed extra printf debugging statement.

  Revision 1.19  1996/01/27 00:18:19  jussi
  Made recId be evaluated as a simple command rather than as
  a complex command.

  Revision 1.18  1996/01/20 00:46:27  jussi
  Small fixes to produce better debugging output.

  Revision 1.17  1996/01/13 23:08:51  jussi
  Added support for Z attribute and shape attribute 2.

  Revision 1.16  1996/01/09 22:27:44  jussi
  Added 3D block shape.

  Revision 1.15  1996/01/09 16:35:48  jussi
  Added missing SetDefaultX() and SetDefaultY() calls.

  Revision 1.14  1995/12/28 19:36:59  jussi
  Small fixes to remove compiler warnings.

  Revision 1.13  1995/12/22 18:07:20  jussi
  Added Vector shape.

  Revision 1.12  1995/12/14 21:17:57  jussi
  Replaced 0x%x with 0x%p.

  Revision 1.11  1995/12/14 17:35:32  jussi
  Made small fixes to get rid of g++ -Wall warnings.

  Revision 1.10  1995/12/14 00:36:57  jussi
  Minor fix to conversion of shape attributes to GData.

  Revision 1.9  1995/12/14 00:28:24  jussi
  Fixed interpretation of shape attributes; shape attributes were not
  converted to GData at all in some cases. This resulted in width or
  height values (shapeAttr[0] and [1]) having zero values.

  Revision 1.8  1995/11/30 00:36:15  jussi
  Commented out a couple of debugging output statements.

  Revision 1.7  1995/11/28 00:04:53  jussi
  Added polygon and oval shapes.

  Revision 1.6  1995/11/25  01:20:12  jussi
  This code now uses Transform matrix operations to convert user/world
  coordinates to screen pixel coordinates. This is to avoid any future
  inconsistencies in how the different code locations compute the
  conversion. xPerPixel and yPerPixel are now obsolete coefficients.

  Revision 1.5  1995/11/21 23:30:58  jussi
  Added copyright notice and cleaned up the code. Fixed bug in
  ChangeCmd where attrList gets passed to FindGDataSize which
  in turn resets _attrList to be the list of GData attributes
  instead of TData attributes.

  Revision 1.4  1995/09/27 16:57:26  jussi
  Minor fixes.

  Revision 1.3  1995/09/26 23:08:17  jussi
  Fixed interface to FindGDataSize so that attribute-valued
  color/size/shape/pattern/orientation mappings would work.

  Revision 1.2  1995/09/05 22:15:02  jussi
  Added CVS header.
*/

//#define DEBUG

#include <stdio.h>
#include <tcl.h>
#include <assert.h>

#include "DeviseTypes.h"
#include "Bitmap.h"
#include "AttrList.h"
#include "TData.h"
#include "Pattern.h"
#include "WindowRep.h"
#include "MappingInterp.h"
#include "ViewGraph.h"
#include "MapInterpShape.h"
#include "ViewShape.h"
#include "ETkWindowShape.h"
#include "Exit.h"
#include "Util.h"
#include "Init.h"
#include "StringStorage.h"
#include "GDataSock.h"
#include "NativeExpr.h"

#include "Color.h"

// Define the following true to force X or Y to be put into GData records,
// even if they're constants (needed for "count mappings").
#define FORCE_X_INTO_GDATA 1
#define FORCE_Y_INTO_GDATA 1

Shape     **MappingInterp::_shapes       = NULL;

//--------------------------------------------------------------------------
/* Return true if command is a constant, and return the constant value */
Boolean MappingInterp::IsConstCmd(char *cmd, AttrList *attrList, Coord &val,
								  AttrType &attrType)
{
#if defined(DEBUG)
  printf("  MappingInterp::IsConstCmd(%s)\n", cmd);
#endif

  // We don't care about the string values here, since we only want to see
  // if the command is a constant; therefore, we can use the default string
  // table and make this a static member function.  RKW 1998-11-03.
  StringStorage *stringTable = StringStorage::GetDefaultTable();

  MappingSimpleCmdEntry entry;
  Boolean isSorted;
  if (ConvertSimpleCmd(cmd, attrList, entry, attrType, isSorted, stringTable)
      && entry.cmdType == MappingSimpleCmdEntry::ConstCmd) {
    val = entry.cmd.num;
    return true;
  }
  return false;
}

//--------------------------------------------------------------------------
MappingInterp::MappingInterp(char *name, TData *tdata,
			     MappingInterpCmd *cmd,
			     unsigned long int cmdFlag,
			     unsigned long int attrFlag,
			     VisualFlag *dimensionInfo, int numDimensions):
	TDataMap(name, tdata, name,
				 0,
                 MappingInterpAllFlags, attrFlag,
		 Init::MaxGDataPages(),
                 dimensionInfo, numDimensions, false)
{
#if defined(DEBUG)
  printf("MappingInterp::MappingInterp(0x%p, %d dimensions, cmdFlag 0x%lx, "
	 "attrFlag 0x%lx\n", this, numDimensions, cmdFlag, attrFlag);
#endif

  _tclCmd = new MappingInterpCmd();
  _tdata = tdata; // saved so we can get it's attribute list
  _simpleCmd = new MappingSimpleCmd();
  _pNativeExpr = NULL;

  if (!_shapes) {
    /* Init shapes */
    _shapes = new Shape *[MaxInterpShapes];
    /* Note: this shape-value mapping must correspond to that in the
     * DEViseShapes variable in lib/control.tk.  RKW 4/29/97. */
    /* Note: if these values are changed, MappingInterp::IsComplexShape()
     * may also need to be changed. */
    _shapes[0]  = new FullMapping_RectShape;
    _shapes[1]  = new FullMapping_RectXShape;
    _shapes[2]  = new FullMapping_BarShape;
    _shapes[3]  = new FullMapping_RegularPolygonShape;
    _shapes[4]  = new FullMapping_OvalShape;
    _shapes[5]  = new FullMapping_VectorShape;
    _shapes[6]  = new FullMapping_HorLineShape;
    _shapes[7]  = new FullMapping_SegmentShape;
    _shapes[8] = new FullMapping_HighLowShape;
    _shapes[9] = new FullMapping_PolylineShape;
    _shapes[10] = new FullMapping_GifImageShape;
    _shapes[11] = new FullMapping_PolylineFileShape;
    _shapes[12] = new FullMapping_TextLabelShape;
    _shapes[13] = new FullMapping_LineShape;
    _shapes[14] = new FullMapping_LineShadeShape;
    _shapes[15] = new FullMapping_ETkWindowShape;
    _shapes[16] = new FullMapping_FixedTextLabelShape;
    _shapes[17] = new FullMapping_ViewShape;
    _shapes[18] = new FullMapping_TextDataLabelShape;
  }

  _tdataFlag = new Bitmap(tdata->GetAttrList()->NumAttrs());
  
  _offsets = new GDataAttrOffset;
  SetGDataOffset(_offsets);

  ChangeCmd(cmd, cmdFlag, attrFlag, dimensionInfo, numDimensions);
}

//--------------------------------------------------------------------------
MappingInterp::~MappingInterp()
{
  delete _tclCmd;
  delete _simpleCmd;

  delete _offsets;
  SetGDataOffset(NULL);

  delete GDataAttrList();
  SetGDataAttrList(NULL);

  delete [] _tdataFlag;

  // added by whh, support for native expression analysis
  delete _pNativeExpr;
}

//--------------------------------------------------------------------------
void MappingInterp::ChangeCmd(MappingInterpCmd *cmd,
			      unsigned long int flag,
			      unsigned long int attrFlag,
			      VisualFlag *dimensionInfo,
			      int numDimensions)
{
#if defined(DEBUG)
  printf("MappingInterp::ChangeCmd(0x%p, %d dimensions, cmdFlag 0x%lx, "
	 "attrFlag 0x%lx\n", this, numDimensions, flag, attrFlag);
#endif

  if (cmd->xCmd == NULL) {
    fprintf(stderr, "Warning: null X command in mapping %s\n", GetName());
  }
  if (cmd->yCmd == NULL) {
    fprintf(stderr, "Warning: null Y command in mapping %s\n", GetName());
  }

  _cmd = cmd; // Note: cmd is allocated in MapInterpClassInfo.
  _cmdFlag = flag;
  _cmdAttrFlag = attrFlag;
  
  /* Need to keep the flags in the TDataMap object consistent */
  SetDynamicShapeAttrs(_cmdAttrFlag);
  
  AttrList *attrList = GDataAttrList();
  delete attrList;

  int gRecSize;
  attrList = InitCmd(GetName(), gRecSize);
  SetGDataAttrList(attrList);

  /* sorted in the X direction? */
  AttrInfo *info = attrList->Find("x");
  if ((info != NULL) && (info->isSorted)) {
    SetDimensionInfo(new VisualFlag(VISUAL_X), 1);
  } else {
    SetDimensionInfo(new VisualFlag(0), 0);
  }

  if (GetGData()) {
    ResetGData(gRecSize);
  } else {
    CreateGData(gRecSize);
  }

  // added by whh, support for native expression analysis
  delete _pNativeExpr;
  _pNativeExpr = NULL;
  if (!_isSimpleCmd) _pNativeExpr = new CGraphicExpr( cmd );
}

//--------------------------------------------------------------------------
/* Get current commands */
MappingInterpCmd *MappingInterp::GetCmd(unsigned long int &cmdFlag,
					unsigned long int &attrFlag)
{
  cmdFlag = _cmdFlag;
  attrFlag = _cmdAttrFlag;
  return _cmd;
}

//--------------------------------------------------------------------------
/* Get the AttrInfo for a GData attribute. */
AttrInfo *MappingInterp::MapGAttr2TAttr(int which_attr)
{
#if defined(DEBUG)
    printf("MappingInterp::MapGAttr2TAttr()\n");
#endif

    MappingSimpleCmdEntry entry;
    AttrType attrType;
    Boolean isSorted;
    Boolean simpleCmd = false;

    if (!(_cmdFlag & which_attr)) {
	return 0;
    }

    switch (which_attr)
    {
      case MappingCmd_X:
	simpleCmd = ConvertSimpleCmd(_cmd->xCmd, _tdata->GetAttrList(), entry,
		attrType, isSorted, GetStringTable(TableX));
	break;
      case MappingCmd_Y:
	simpleCmd = ConvertSimpleCmd(_cmd->yCmd, _tdata->GetAttrList(), entry,
		attrType, isSorted, GetStringTable(TableY));
	break;
      case MappingCmd_Z:
	simpleCmd = ConvertSimpleCmd(_cmd->zCmd, _tdata->GetAttrList(), entry,
		attrType, isSorted, GetStringTable(TableZ));
	break;
      case MappingCmd_Color:
	simpleCmd = ConvertSimpleCmd(_cmd->colorCmd, _tdata->GetAttrList(),
		entry, attrType, isSorted, GetStringTable(TableGen));
	break;
      case MappingCmd_Size:
	simpleCmd = ConvertSimpleCmd(_cmd->sizeCmd, _tdata->GetAttrList(),
		entry, attrType, isSorted, GetStringTable(TableGen));
	break;
      case MappingCmd_Pattern:
	simpleCmd = ConvertSimpleCmd(_cmd->patternCmd, _tdata->GetAttrList(),
		entry, attrType, isSorted, GetStringTable(TableGen));
	break;
      case MappingCmd_Orientation:
	simpleCmd = ConvertSimpleCmd(_cmd->orientationCmd, _tdata->GetAttrList(),
		entry, attrType, isSorted, GetStringTable(TableGen));
	break;
      case MappingCmd_Shape:
	simpleCmd = ConvertSimpleCmd(_cmd->shapeCmd, _tdata->GetAttrList(),
		entry, attrType, isSorted, GetStringTable(TableGen));
	break;
      default:
	return 0;

    }
    if (simpleCmd && entry.cmdType == MappingSimpleCmdEntry::AttrCmd) {
      // If attribute is recId, return a pointer to this "dummy" AttrInfo
      // structure, so calling function can tell the difference between
      // recId and a constant mapping.
      if (entry.cmd.attrNum == -1) {
	static AttrInfo recId;
        recId.name = REC_ID_NAME;
        recId.attrNum = -1;
        recId.offset = -1;
        recId.length = 0;
        recId.isComposite = false;
        recId.isSorted = true;
        recId.hasMatchVal = false;
        recId.hasHiVal = false;
        recId.hasLoVal = false;

	return &recId;
      } else {
        return _tdata->GetAttrList()->Get(entry.cmd.attrNum);
      }
    }
    return 0;
}


//--------------------------------------------------------------------------
char *MappingInterp::MapTAttr2GAttr(char *tname)
{
    int i;
    AttrInfo *info;
    char *gname = new char[64];
    for (i = 1; i <= 8; ) {
    	info = MapGAttr2TAttr(i);
	if ( info && !strcmp(info->name, tname)) break;
	i = i*2;
    }
    if (i > 8) return NULL;
    switch (i) {
        case 1: 
	   strcpy(gname, "X");
	   break;
        case 2: 
	   strcpy(gname, "Y");
	   break;
        case 4: 
	   strcpy(gname, "Z");
	   break;
        case 8: 
	   strcpy(gname, "Color");
	   break;
	default: 
	   fprintf(stderr, "Invalid attribute type\n");
	   return NULL;

    }
    return gname;
}

//--------------------------------------------------------------------------
/* Get the AttrInfo for shape attribute i */
AttrInfo *MappingInterp::MapShapeAttr2TAttr(int i)
{
#if defined(DEBUG)
    printf("MappingInterp::MapShapeAttr2TAttr()\n");
#endif

    MappingSimpleCmdEntry entry;
    AttrType attrType;
    Boolean isSorted;
    Boolean simpleCmd = false;
    
    if (!(_cmdAttrFlag & (1 << i)))
    {
	return 0;
    }

    StringStorage *stringTable = GetStringTable(TableGen);
    
    simpleCmd = ConvertSimpleCmd(_cmd->shapeAttrCmd[i], _tdata->GetAttrList(),
		entry, attrType, isSorted, stringTable);
    
    if (simpleCmd && entry.cmdType == MappingSimpleCmdEntry::AttrCmd) {
	return _tdata->GetAttrList()->Get(entry.cmd.attrNum);
    }
    
    return 0;
}

//--------------------------------------------------------------------------
void MappingInterp::DrawGDataArray(ViewGraph *view, WindowRep *win,
				   void **gdataArray, int num,
				   int &recordsProcessed,
				   Boolean timeoutAllowed)
{
#if defined(DEBUG)
  printf("MappingInterp::DrawGDataArray(%s, 0x%p, %d)\n", view->GetName(),
    win, num);
#endif

  if (view->GetDrawToScreen()) {
    if (_offsets->_shapeOffset < 0) {
      /* constant shape */
      ShapeID shape = GetDefaultShape();
#if defined(DEBUG)
      printf("Drawing shape %d\n", shape);
#endif
      _shapes[shape]->DrawGDataArray(win, gdataArray, num, this,
				     view, GetPixelWidth(), recordsProcessed,
				     timeoutAllowed);
    } else {
      /* dynamic shape */
      recordsProcessed = 0;
      int i = 0;
      Boolean timedOut = false;
      while (i < num && !timedOut) {
        ShapeID shape = *((ShapeID *)((char *)gdataArray[i]+_offsets->_shapeOffset));
        int j;
        for(j = i + 1; j < num; j++) {
	  ShapeID nextShape =
	                *((ShapeID *)((char *)gdataArray[j]+_offsets->_shapeOffset));
	  if (shape != nextShape)
	    break;
        }
        /* gdataArray[i..j-1] have the same shape */
#if defined(DEBUG)
        printf("Drawing shape %d (records %d thru %d)\n", shape, i, j - 1);
#endif
        int tmpRecs;
        _shapes[shape]->DrawGDataArray(win, &gdataArray[i], j - i, this,
				       view, GetPixelWidth(), tmpRecs,
				       timeoutAllowed);
        recordsProcessed = i + tmpRecs;
        if (tmpRecs != j - i) {
	  timedOut = true;
        }
        i = j;
      }
    }
  } else {
    recordsProcessed = num;
  }

  if (view->GetSendToSocket()) {
#if defined(DEBUG)
    printf("  sending %d GData records to socket\n", recordsProcessed);
#endif
    (void) view->Send(gdataArray, this, recordsProcessed);
  }

#if defined(DEBUG)
  printf("  %d records processed\n", recordsProcessed);
#endif
}

//--------------------------------------------------------------------------
// convert from Tdata to Gdata. buf contains
// buffer for data. tRecs are pointers to variable size records only.
//
// modified by whh for native expression support

void MappingInterp::ConvertToGData(RecId startRecId, void *buf,
				   int numRecs, void *gdataPtr)
{
#if defined(DEBUG)
    printf("MappingInterp::ConvertToGData id %d numRecs %d, buf 0x%p,"
	   " gbuf 0x%p\n", (int) startRecId, numRecs, buf, gdataPtr);
#endif

  if (_isSimpleCmd) {
    /* Do simple command conversion */
    ConvertToGDataSimple(startRecId, buf, numRecs, gdataPtr);
  } else {
    ConvertToGDataComplex(startRecId, buf, numRecs, gdataPtr);
  }

  FindBoundingBoxes(gdataPtr, numRecs);
}

static int
InsertAttr(AttrList *attrList, int &attrNum, char *name, int &offset,
    int size, AttrType attrType, Boolean isSorted)
{
#if defined(DEBUG)
  printf("InsertAttr(%d, <%s>, %d)\n", attrNum, name, offset);
#endif

  // Make sure things are aligned on double boundaries.
  int result = offset = WordBoundary(offset, sizeof(double));
  attrList->InsertAttr(attrNum++, name, offset, size,
      attrType, false, NULL, false, isSorted);
  offset += size;
  return result;
}

//--------------------------------------------------------------------------
AttrList *MappingInterp::InitCmd(char *name, int &gRecSize)
{
#if defined(DEBUG)
  printf("MappingInterp::InitCmd(%s)\n", name);
#endif

  StringStorage *xStringTable = GetStringTable(TableX);
  StringStorage *yStringTable = GetStringTable(TableY);
  StringStorage *zStringTable = GetStringTable(TableZ);
  StringStorage *genStringTable = GetStringTable(TableGen);

  AttrList *attrList = new AttrList(name);

  if (InitCmdSimple(xStringTable, yStringTable, zStringTable, genStringTable,
      attrList, gRecSize)) {
    _isSimpleCmd = true;
  } else {
    _isSimpleCmd = false;
    delete attrList;
    attrList = new AttrList(name);
	InitCmdComplex(xStringTable, yStringTable, zStringTable, genStringTable, 
	    attrList, gRecSize);
  }

#if defined(DEBUG)
  printf("Command is %s\n", name);
  PrintCmd();
  attrList->Print();
  printf("offsets: x %d, y %d, z %d, color %d, size %d,\n", 
	 _offsets->_xOffset, _offsets->_yOffset, _offsets->_zOffset,
	 _offsets->_colorOffset, _offsets->_sizeOffset);
  printf("         shape %d, pattern %d, orientation %d\n", 
	 _offsets->_shapeOffset, _offsets->_patternOffset,
	 _offsets->_orientationOffset);
  printf("         ULx %d, ULy %d, LRx %d, LRy %d\n", _offsets->_bbULxOffset,
         _offsets->_bbULyOffset, _offsets->_bbLRxOffset,
	 _offsets->_bbLRyOffset);
  printf("attr offsets: ");
  for(int shapeAttr = 0; shapeAttr < MAX_SHAPE_ATTRS; shapeAttr++) {
    printf(" %d", _offsets->_shapeAttrOffset[shapeAttr]);
  }
  printf("\n");
  printf("gRecSize = %d\n", gRecSize);
#endif
  
  return attrList;
}

//--------------------------------------------------------------------------
void
MappingInterp::InitOffsets()
{
  /* Record ID is always first GData attribute */
  _offsets->_recIdOffset = 0;

  /* Init offsets to other GData attributes */
  _offsets->_xOffset = _offsets->_yOffset = _offsets->_zOffset = -1;
  _offsets->_colorOffset = _offsets->_sizeOffset = _offsets->_shapeOffset = -1;
  _offsets->_patternOffset = _offsets->_orientationOffset = -1;
  _offsets->_bbULxOffset = _offsets->_bbULyOffset = -1;
  _offsets->_bbLRxOffset = _offsets->_bbLRyOffset = -1;

  for(int shapeAttr = 0; shapeAttr < MAX_SHAPE_ATTRS; shapeAttr++) {
    _offsets->_shapeAttrOffset[shapeAttr] = -1;
  }
}

//--------------------------------------------------------------------------
Boolean
MappingInterp::InitCmdSimple(StringStorage *xStringTable,
    StringStorage *yStringTable, StringStorage *zStringTable,
	StringStorage *genStringTable, AttrList *attrList, int &gRecSize)
{
#if defined(DEBUG)
  printf("MappingInterp(%s)::InitCmdSimple()\n", GetName());
#endif

  InitOffsets();

  _tdataFlag->ClearBitmap();
  _maxTDataAttrNum = 0;
  
  int offset = sizeof(RecId);
  Boolean isSorted;
  AttrType attrType;
  int attrNum = 0;

  ShapeID shape = (ShapeID)9999; // invalid unless shape is constant

  if (_cmdFlag & MappingCmd_X) {
    if (!ConvertSimpleCmd(_cmd->xCmd, _tdata->GetAttrList(), _simpleCmd->xCmd,
	    attrType, isSorted, xStringTable)) {
      return false;
    } else if (_simpleCmd->xCmd.cmdType == MappingSimpleCmdEntry::ConstCmd &&
		!FORCE_X_INTO_GDATA) {
      /* constant */
      SetDefaultX((Coord)_simpleCmd->xCmd.cmd.num);
      attrList->InsertAttr(attrNum++, "x", -1, sizeof(Coord),
			   attrType, false, NULL, false, isSorted);
    } else {
      _offsets->_xOffset = InsertAttr(attrList, attrNum, "x", offset,
	      sizeof(Coord), attrType, isSorted);
    }
  }

  if (_cmdFlag & MappingCmd_Y) {
    if (!ConvertSimpleCmd(_cmd->yCmd, _tdata->GetAttrList(),
		_simpleCmd->yCmd, attrType, isSorted, yStringTable)) {
      return false;
    } else if (_simpleCmd->yCmd.cmdType == MappingSimpleCmdEntry::ConstCmd &&
		!FORCE_Y_INTO_GDATA) {
      /* constant */
      SetDefaultY((Coord)_simpleCmd->yCmd.cmd.num);
      attrList->InsertAttr(attrNum++, "y", -1, sizeof(Coord),
			   attrType, false, NULL, false, isSorted);
    } else {
      _offsets->_yOffset = InsertAttr(attrList, attrNum, "y", offset,
	      sizeof(Coord), attrType, isSorted);
    }
  }

  if (_cmdFlag & MappingCmd_Z) {
    if (!ConvertSimpleCmd(_cmd->zCmd, _tdata->GetAttrList(),
		_simpleCmd->zCmd, attrType, isSorted, zStringTable)) {
      return false;
    } else if (_simpleCmd->zCmd.cmdType == MappingSimpleCmdEntry::ConstCmd) {
      /* constant */
      SetDefaultZ((Coord)_simpleCmd->zCmd.cmd.num);
      attrList->InsertAttr(attrNum++, "z", -1, sizeof(Coord),
			   attrType, false, NULL, false, isSorted);
    } else {
      _offsets->_zOffset = InsertAttr(attrList, attrNum, "z", offset,
	      sizeof(Coord), attrType, isSorted);
    }
  }

  // Color command
  if (_cmdFlag & MappingCmd_Color)
  {
    if (!ConvertSimpleCmd(_cmd->colorCmd, _tdata->GetAttrList(),
		_simpleCmd->colorCmd, attrType, isSorted, genStringTable)) {
      return false;
    } else if (_simpleCmd->colorCmd.cmdType ==
	    MappingSimpleCmdEntry::ConstCmd) {
      PColorID	pcid = (PColorID)_simpleCmd->colorCmd.cmd.num;

	  // Coloring in TDataMap is apparently used to store color if constant.
      GetColoring().SetForeground(pcid);
      attrList->InsertAttr(attrNum++, "color", -1, sizeof(PColorID),
      attrType, false, NULL, false, isSorted);
    } else {
      _offsets->_colorOffset = InsertAttr(attrList, attrNum, "color", offset,
	      sizeof(PColorID), attrType, isSorted);
    }
  }

  if (_cmdFlag & MappingCmd_Size) {
    if (!ConvertSimpleCmd(_cmd->sizeCmd, _tdata->GetAttrList(),
		_simpleCmd->sizeCmd, attrType, isSorted, genStringTable)) {
      return false;
    } else if (_simpleCmd->sizeCmd.cmdType ==
	    MappingSimpleCmdEntry::ConstCmd) {
      /* constant */
      SetDefaultSize((Coord)_simpleCmd->sizeCmd.cmd.num);
      attrList->InsertAttr(attrNum++, "size", -1, sizeof(Coord),
			   attrType, false, NULL, false, isSorted);
    } else {
      _offsets->_sizeOffset = InsertAttr(attrList, attrNum, "size", offset,
	      sizeof(Coord), attrType, isSorted);
    }
  }

  if (_cmdFlag & MappingCmd_Pattern) {
    if (!ConvertSimpleCmd(_cmd->patternCmd, _tdata->GetAttrList(),
		_simpleCmd->patternCmd, attrType, isSorted, genStringTable)) {
      return false;
    } else if (_simpleCmd->patternCmd.cmdType ==
	    MappingSimpleCmdEntry::ConstCmd) {
      /* constant */
      SetDefaultPattern((Pattern)_simpleCmd->patternCmd.cmd.num);
      attrList->InsertAttr(attrNum++, "pattern", -1, sizeof(Pattern),
			   attrType, false, NULL, false, isSorted);
    } else {
      _offsets->_patternOffset = InsertAttr(attrList, attrNum, "pattern",
	      offset, sizeof(Pattern), attrType, isSorted);
    }
  }

  if (_cmdFlag & MappingCmd_Shape) {
    if (!ConvertSimpleCmd(_cmd->shapeCmd, _tdata->GetAttrList(),
		_simpleCmd->shapeCmd, attrType, isSorted, genStringTable)) {
      return false;
    } else if (_simpleCmd->shapeCmd.cmdType ==
	    MappingSimpleCmdEntry::ConstCmd) {
      /* constant */
      shape = (ShapeID)_simpleCmd->shapeCmd.cmd.num;
      if (shape >= MaxInterpShapes) shape = 0;
      SetDefaultShape(shape);
      attrList->InsertAttr(attrNum++, "shape", -1, sizeof(ShapeID),
			   attrType, false, NULL, false, isSorted);
    } else {
      _offsets->_shapeOffset = InsertAttr(attrList, attrNum, "shape", offset,
	      sizeof(ShapeID), attrType, isSorted);
    }
  }

  if (_cmdFlag & MappingCmd_Orientation) {
    if (!ConvertSimpleCmd(_cmd->orientationCmd,
			  _tdata->GetAttrList(), _simpleCmd->orientationCmd, attrType,
			  isSorted, genStringTable)) {
      return false;
    } else if (_simpleCmd->orientationCmd.cmdType == 
	    MappingSimpleCmdEntry::ConstCmd) {
      /* constant */
      SetDefaultOrientation((Coord)_simpleCmd->orientationCmd.cmd.num);
      attrList->InsertAttr(attrNum++, "orientation", -1, sizeof(Coord),
			   attrType, false, NULL, false, isSorted);
    } else {
      _offsets->_orientationOffset = InsertAttr(attrList, attrNum,
	      "orientation", offset, sizeof(Coord), attrType, isSorted);
    }
  }

  _maxGDataShapeAttrNum = -1;
  for(int shapeAttr = 0; shapeAttr < MAX_SHAPE_ATTRS; shapeAttr++) {
    if (_cmdAttrFlag & (1 << shapeAttr)) {
      _maxGDataShapeAttrNum = shapeAttr;
      if (!ConvertSimpleCmd(_cmd->shapeAttrCmd[shapeAttr],
	      _tdata->GetAttrList(), _simpleCmd->shapeAttrCmd[shapeAttr], attrType,
		  isSorted, genStringTable)) {
        return false;
	  } else if (_simpleCmd->shapeAttrCmd[shapeAttr].cmdType ==
	      MappingSimpleCmdEntry::ConstCmd) {
	    /* constant */
	    SetDefaultShapeAttr(shapeAttr,
		    (Coord)_simpleCmd->shapeAttrCmd[shapeAttr].cmd.num);
	    char attrName [80];
	    sprintf(attrName, "shapeAttr_%d", shapeAttr);
	    attrList->InsertAttr(attrNum++, attrName, -1, sizeof(ShapeAttr),
			     attrType, false, NULL, false, isSorted);
      } else {
	    char attrName [80];
	    sprintf(attrName, "shapeAttr_%d", shapeAttr);
          _offsets->_shapeAttrOffset[shapeAttr] = InsertAttr(attrList,
		      attrNum, attrName, offset, sizeof(ShapeAttr), attrType, isSorted);
      }
    }
  }

  SetBBOffsets(attrList, attrNum, offset);

  gRecSize = WordBoundary(offset, sizeof(double));

  return true;
}

//--------------------------------------------------------------------------
void
MappingInterp::InitCmdComplex(StringStorage *xStringTable,
    StringStorage *yStringTable, StringStorage *zStringTable,
	StringStorage *genStringTable, AttrList *attrList, int &gRecSize)
{
#if defined(DEBUG)
  printf("MappingInterp(%s)::InitCmdComplex()\n", GetName());
#endif

/* Note:  I don't understand why having a single "complex" GData attribute
 * forces all other GData attributes to be converted in the "complex" mode.
 * RKW 4/24/97. */

  InitOffsets();

  _tdataFlag->ClearBitmap();
  _maxTDataAttrNum = 0;

  int offset = sizeof(RecId);
  Boolean isSorted;
  AttrType attrType;
  int attrNum = 0;

  ShapeID shape = (ShapeID)9999; // invalid unless shape is constant

  double constVal;

  if (_cmdFlag & MappingCmd_X) {
    if (IsConstCmd(_cmd->xCmd, attrList, constVal, attrType) &&
		!FORCE_X_INTO_GDATA) {
      SetDefaultX((Coord)constVal);
      _tclCmd->xCmd = "";
      attrList->InsertAttr(attrNum++, "x", -1, sizeof(double), attrType,
			   false, NULL, false, false);
    } else {
      _tclCmd->xCmd = ConvertCmd(_cmd->xCmd, attrType, isSorted);
      _offsets->_xOffset = InsertAttr(attrList, attrNum, "x", offset,
	      sizeof(Coord), attrType, isSorted);
    }
  }

  if (_cmdFlag & MappingCmd_Y) {
    if (IsConstCmd(_cmd->yCmd, attrList, constVal, attrType) &&
		!FORCE_Y_INTO_GDATA) {
      SetDefaultY((Coord)constVal);
      _tclCmd->yCmd = "";
      attrList->InsertAttr(attrNum++, "y", -1, sizeof(double), attrType,
			   false, NULL, false, false);
    } else {
      _tclCmd->yCmd = ConvertCmd(_cmd->yCmd, attrType, isSorted);
      _offsets->_yOffset = InsertAttr(attrList, attrNum, "y", offset,
	      sizeof(Coord), attrType, isSorted);
    }
  }

  if (_cmdFlag & MappingCmd_Z) {
    if (IsConstCmd(_cmd->zCmd, attrList, constVal, attrType)) {
      SetDefaultZ((Coord)constVal);
      _tclCmd->zCmd = "";
      attrList->InsertAttr(attrNum++, "z", -1, sizeof(double), attrType,
			   false, NULL, false, false);
    } else {
      _tclCmd->zCmd = ConvertCmd(_cmd->zCmd, attrType, isSorted);
      _offsets->_zOffset = InsertAttr(attrList, attrNum, "z", offset,
	      sizeof(Coord), attrType, isSorted);
    }
  }

// Color command
  if (_cmdFlag & MappingCmd_Color) {
    if (IsConstCmd(_cmd->colorCmd, attrList, constVal, attrType)) {
      PColorID	pcid = (PColorID)constVal;

      // Coloring in TDataMap is apparently used to store color if
      // constant.
      GetColoring().SetForeground(pcid);
      _tclCmd->colorCmd = "";
      attrList->InsertAttr(attrNum++, "color", -1, sizeof(double), attrType,
         false, NULL, false, false);
    } else {
      _tclCmd->colorCmd = ConvertCmd(_cmd->colorCmd, attrType, isSorted);
      _offsets->_colorOffset = InsertAttr(attrList, attrNum, "color", offset,
          sizeof(PColorID), attrType, isSorted);
	}
  }

  if (_cmdFlag & MappingCmd_Size) {
    if (IsConstCmd(_cmd->sizeCmd, attrList, constVal, attrType)) {
      SetDefaultSize(constVal);
      _tclCmd->sizeCmd = "";
      attrList->InsertAttr(attrNum++, "size", -1, sizeof(double), attrType,
			   false, NULL, false, false);
    } else {
      _tclCmd->sizeCmd = ConvertCmd(_cmd->sizeCmd, attrType, isSorted);
      _offsets->_sizeOffset = InsertAttr(attrList, attrNum, "size", offset,
	      sizeof(Coord), attrType, isSorted);
    }
  }

  if (_cmdFlag & MappingCmd_Pattern) {
    if (IsConstCmd(_cmd->patternCmd, attrList, constVal, attrType)) {
      SetDefaultPattern((Pattern)constVal);
      _tclCmd->patternCmd = "";
      attrList->InsertAttr(attrNum++, "pattern", -1, sizeof(double), attrType,
			   false, NULL, false, false);
    } else {
      _tclCmd->patternCmd = ConvertCmd(_cmd->patternCmd, attrType, isSorted);
      _offsets->_patternOffset = InsertAttr(attrList, attrNum, "pattern",
	      offset, sizeof(Pattern), attrType, isSorted);
    }
  }

  if (_cmdFlag & MappingCmd_Orientation) {
    if (IsConstCmd(_cmd->orientationCmd, attrList, constVal, attrType)) {
      SetDefaultOrientation(constVal);
      _tclCmd->orientationCmd = "";
      attrList->InsertAttr(attrNum++, "orientation", -1, sizeof(double),
	      attrType, false, NULL, false, false);
    } else {
      _tclCmd->orientationCmd = ConvertCmd(_cmd->orientationCmd, attrType, 
					   isSorted);
      _offsets->_orientationOffset = InsertAttr(attrList, attrNum,
	      "orientation", offset, sizeof(Coord), attrType, isSorted);
    }
  }

  if (_cmdFlag & MappingCmd_Shape) {
    if (IsConstCmd(_cmd->shapeCmd, attrList, constVal, attrType)) {
      shape = (ShapeID)constVal;
      if (shape >= MaxInterpShapes) shape = 0;
      SetDefaultShape(shape);
      _tclCmd->shapeCmd = "";
      attrList->InsertAttr(attrNum++, "shape", -1, sizeof(double), attrType,
			   false, NULL, false, false);
    } else {
      _tclCmd->shapeCmd = ConvertCmd(_cmd->shapeCmd, attrType, isSorted);
      _offsets->_shapeOffset = InsertAttr(attrList, attrNum, "shape", offset,
	      sizeof(ShapeID), attrType, isSorted);
    }
  }

  _maxGDataShapeAttrNum = -1;
  for(int shapeAttr = 0; shapeAttr < MAX_SHAPE_ATTRS; shapeAttr++) {
    char attrName [80];
    sprintf(attrName, "shapeAttr_%d", shapeAttr);
    if (_cmdAttrFlag & (1 << shapeAttr)) {
      _maxGDataShapeAttrNum = shapeAttr;
      if (IsConstCmd(_cmd->shapeAttrCmd[shapeAttr], attrList, constVal,
	      attrType)) {
	    SetDefaultShapeAttr(shapeAttr,constVal);
	    _tclCmd->shapeAttrCmd[shapeAttr] = "";
	    attrList->InsertAttr(attrNum++, attrName, -1, sizeof(double),
			     attrType, false, NULL, false, false);
      } else {
	    _tclCmd->shapeAttrCmd[shapeAttr] = 
	        ConvertCmd(_cmd->shapeAttrCmd[shapeAttr], attrType, isSorted);
        _offsets->_shapeAttrOffset[shapeAttr] = InsertAttr(attrList, attrNum,
		    attrName, offset, sizeof(ShapeAttr), attrType, isSorted);
      }
    }
  }

  SetBBOffsets(attrList, attrNum, offset);

  gRecSize = WordBoundary(offset, sizeof(double));
}

//--------------------------------------------------------------------------

void
MappingInterp::SetBBOffsets(AttrList *attrList, int &attrNum, int &offset)
{
  Boolean bbIsVariable = false;
  if (_offsets->_shapeOffset >= 0) {
    bbIsVariable = true;
  } else {
	ShapeID shape = GetDefaultShape();
    bbIsVariable = _shapes[shape]->BBIsVariable(_offsets);
  }

  if (bbIsVariable) {
    _offsets->_bbULxOffset = InsertAttr(attrList, attrNum, "bbULx", offset,
	    sizeof(Coord), DoubleAttr, false);

    _offsets->_bbULyOffset = InsertAttr(attrList, attrNum, "bbULy", offset,
	    sizeof(Coord), DoubleAttr, false);

    _offsets->_bbLRxOffset = InsertAttr(attrList, attrNum, "bbLRx", offset,
	    sizeof(Coord), DoubleAttr, false);

    _offsets->_bbLRyOffset = InsertAttr(attrList, attrNum, "bbLRy", offset,
	    sizeof(Coord), DoubleAttr, false);
  } else {
	attrList->InsertAttr(attrNum++, "bbULx", -1, sizeof(Coord), DoubleAttr,
	  false);

	attrList->InsertAttr(attrNum++, "bbULy", -1, sizeof(Coord), DoubleAttr,
	  false);

	attrList->InsertAttr(attrNum++, "bbLRx", -1, sizeof(Coord), DoubleAttr,
	  false);

	attrList->InsertAttr(attrNum++, "bbLRy", -1, sizeof(Coord), DoubleAttr,
	  false);
  }

}

//--------------------------------------------------------------------------

/* string manipulation commands */
const int MAX_STRING = 1024;
static char _stringBuf[MAX_STRING];
static int _numChar = 0;

//--------------------------------------------------------------------------
static void InitString()
{
  _numChar = 0;
  _stringBuf[0] = '\0';
}

//--------------------------------------------------------------------------
static void InsertChar(char c)
{
  DOASSERT(_numChar + 1 < MAX_STRING, "No more string space");
  _stringBuf[_numChar++] = c;
  _stringBuf[_numChar] = '\0';
}

//--------------------------------------------------------------------------
static void InsertString(char *string)
{
  int length = strlen(string);
  DOASSERT(_numChar + length < MAX_STRING, "No more string space");
  char *ptr = &_stringBuf[_numChar];
  while (*string != '\0') {
    *ptr++ = *string++;
  }
  _numChar += length;
  _stringBuf[_numChar] = '\0';
}

//--------------------------------------------------------------------------
static char *GetString()
{
  return _stringBuf;
}

//--------------------------------------------------------------------------
/* Return TRUE if cmd is a simple command, and set entry to the
   converted cmmand entry. A simple command
   is either a constant, or a tdata attribute */

Boolean MappingInterp::ConvertSimpleCmd(char *cmd, AttrList *attrList,
					MappingSimpleCmdEntry &entry,
					AttrType &type, Boolean &isSorted,
					StringStorage *stringTable)
{
#if defined(DEBUG)
  printf("MappingInterp::ConvertSimpleCmd: '%s'\n",cmd);
#endif

  /* do not use simple interpreter? */
  if (!Init::UseSimpleInterpreter())
    return false;

  /* skip leading blanks and tabs */
  while (*cmd == ' ' || *cmd == '\t') cmd++;

  /* NULL command? */
  if (*cmd == '\0') {
    entry.cmdType = MappingSimpleCmdEntry::NULLCmd;
    type = FloatAttr;
    isSorted = false;
    return true;
  }
  
  AttrInfo *info;
  double num;

  if (*cmd == '$') {
    /*
       printf("got '$'\n");
    */
    if ( *(cmd+1) == '\0' )
      return false;

    if (!strcmp(cmd + 1, REC_ID_NAME)) {
      entry.cmdType = MappingSimpleCmdEntry::AttrCmd;
      entry.cmd.attrNum = -1;
      type = IntAttr;
      isSorted = true;
      return true;
    }
    
    if ((info = attrList->Find(cmd+1)) != NULL) {
      entry.cmdType = MappingSimpleCmdEntry::AttrCmd;
      entry.cmd.attrNum = info->attrNum;
      type = info->type;
      isSorted = info->isSorted;
      return true;
    }
#if defined(DEBUG)
    printf("Undefined variable name: %s\n", cmd + 1);
    //printf("Attribute list:\n");
    //attrList->Print();
#endif
    return false;
  }

  if (*cmd == '"') {
    int len = strlen(cmd);
    char* end = cmd + len - 1;
    if( len == 1 || *end != '"' ) return false;
    len -= 2; 
    char* str = new char[len+1];
    strncpy(str, cmd+1, len);
    str[len] = '\0'; // Terminate the string!
    int strid;
    int code = stringTable->Insert(str, strid);
    DOASSERT(code >= 0, "Cannot insert string");
    delete [] str;
#if defined(DEBUG)
    printf("string constant at %d: %s\n", strid, str != NULL ? str : "NULL");
#endif
    entry.cmdType = MappingSimpleCmdEntry::ConstCmd;
    entry.cmd.num = strid;
    type = StringAttr;
    isSorted = false;
    return true;
  }

  if (ConvertNum(cmd, num)) {
    entry.cmdType = MappingSimpleCmdEntry::ConstCmd;
    entry.cmd.num = num;
    type = DoubleAttr;
    isSorted = false;
    return true;
  }

  return false;
}

//--------------------------------------------------------------------------
// Note: sorted will now be set to false if the command is anything but
// a simple invocation of a sorted attribute.  This fixes bug 466.
// RKW 1999-03-02.
char *MappingInterp::ConvertCmd(char *cmd, AttrType &attrType,
				Boolean &isSorted)
{
#if defined(DEBUG)
  printf("  MappingInterp::ConvertCmd: '%s'\n",cmd);
#endif

  InitString();

  attrType = DoubleAttr;
  isSorted = true;
  
  while (*cmd != '\0') {
    if (*cmd == '$') {
      /* convert variable name */
      char *ptr = cmd+1;
      while ( (*ptr >= 'a' && *ptr <='z') ||
	     (*ptr >= 'A' && *ptr <= 'Z') ||
	     (*ptr >= '0' && *ptr <= '9') ||
	     (*ptr == '_')) {
	      ptr++;
      }
      if (ptr == cmd+1) {
	    /* did not get a variable name */
	      isSorted = false;
	      InsertChar(*cmd);
      } else {
		    /* from cmd+1 to ptr-1 is a variable name */
			  char buf[80];
			  int len = ptr - 1 - (cmd + 1) + 1;
			  DOASSERT(len < (int)sizeof buf, "Variable name too long");
			  memcpy(buf, cmd + 1, len);
			  buf[len] = 0;
			
	      if (strcmp(buf, REC_ID_NAME) == 0) {
          InsertString("$recId");
	      } else {
			    AttrInfo *info = _tdata->GetAttrList()->Find(buf);
			    if (!info) {
			      /* can't find variable name */
			      char errBuf[256];
			      sprintf(errBuf, "Can't find attribute '%s' requested by mapping %s",
			              buf, GetName());
		        reportErrNosys(errBuf);
			    } else {
			      /* found the attribute */
			      if (!info->isSorted) {
			        isSorted = false;
				  }
			      if (info->type == DateAttr) {
			        attrType = DateAttr;
			      } else if (info->type == StringAttr) {
			        attrType = StringAttr;
			      }
			  
			      if (info->attrNum > _maxTDataAttrNum)
			        _maxTDataAttrNum = info->attrNum;
			      _tdataFlag->SetBit(info->attrNum);
			  
		         sprintf(buf, "$interpAttr_%d", info->attrNum);
			       InsertString(buf);
			    }
		    }
			  cmd = ptr - 1;
      }
    } else {
	  isSorted = false;
      InsertChar(*cmd);
    }
    cmd++;
  }
  return CopyString(GetString());
}

//--------------------------------------------------------------------------
void MappingInterp::PrintSimpleCmdEntry(MappingSimpleCmdEntry *entry)
{
  switch(entry->cmdType) {
  case MappingSimpleCmdEntry::AttrCmd:
    if (entry->cmd.attrNum != -1) {
      AttrInfo *info = _tdata->GetAttrList()->Get(entry->cmd.attrNum);
      printf("\"%s\"", info->name);
    }
    else {
      printf("\"recId\"");
    }
    break;

  case MappingSimpleCmdEntry::ConstCmd:
    printf("%f", entry->cmd.num);
    break;

  case MappingSimpleCmdEntry::NULLCmd:
    printf("NULL");
    break;

  default:
    printf("Unknown");

  }
}

//--------------------------------------------------------------------------
void MappingInterp::PrintCmd()
{
  if (_isSimpleCmd) {
    /* simple command */
    printf("simple command\n");
    if (_cmdFlag & MappingCmd_X) {
      printf("x: %s --> ", _cmd->xCmd);
      PrintSimpleCmdEntry(&_simpleCmd->xCmd);
      printf("\n");
    }
    
    if (_cmdFlag & MappingCmd_Y) {
      printf("y: %s --> ", _cmd->yCmd);
      PrintSimpleCmdEntry(&_simpleCmd->yCmd);
      printf("\n");
    }
    
    if (_cmdFlag & MappingCmd_Z) {
      printf("z: %s --> ", _cmd->zCmd);
      PrintSimpleCmdEntry(&_simpleCmd->zCmd);
      printf("\n");
    }

    if (_cmdFlag & MappingCmd_Color) {
      printf("color: %s --> ", _cmd->colorCmd);
      PrintSimpleCmdEntry(&_simpleCmd->colorCmd);
      printf("\n");
    }
    
    if (_cmdFlag & MappingCmd_Size) {
      printf("size: %s --> ", _cmd->sizeCmd);
      PrintSimpleCmdEntry(&_simpleCmd->sizeCmd);
      printf("\n");
		}
    
    if (_cmdFlag & MappingCmd_Pattern) {
      printf("pattern: %s --> ", _cmd->patternCmd);
      PrintSimpleCmdEntry(&_simpleCmd->patternCmd);
      printf("\n");
    }
    
    if (_cmdFlag & MappingCmd_Shape) {
      printf("shape: %s --> ", _cmd->shapeCmd);
      PrintSimpleCmdEntry(&_simpleCmd->shapeCmd);
      printf("\n");
    }
    
    if (_cmdFlag & MappingCmd_Orientation) {
      printf("orientation: %s --> ", _cmd->orientationCmd);
      PrintSimpleCmdEntry(&_simpleCmd->orientationCmd);
      printf("\n");
    }

    for(int j = 0; j < MAX_SHAPE_ATTRS; j++) {
      if (_cmdAttrFlag & (1 << j)) {
	printf("shapeAttr_%d: %s --> ", j, _cmd->shapeAttrCmd[j]);
	PrintSimpleCmdEntry(&_simpleCmd->shapeAttrCmd[j]);
	printf("\n");
      }
    }

  } else {

    printf("complex command\n");
    if (_cmdFlag & MappingCmd_X)
      printf("x: %s --> %s\n", _cmd->xCmd, _tclCmd->xCmd);
    
    if (_cmdFlag & MappingCmd_Y)
      printf("y: %s --> %s\n", _cmd->yCmd, _tclCmd->yCmd);
    
    if (_cmdFlag & MappingCmd_Z)
      printf("z: %s --> %s\n", _cmd->zCmd, _tclCmd->zCmd);
    
    if (_cmdFlag & MappingCmd_Color)
      printf("color: %s --> %s\n", _cmd->colorCmd, _tclCmd->colorCmd);
    
    if (_cmdFlag & MappingCmd_Size)
      printf("size: %s --> %s\n", _cmd->sizeCmd, _tclCmd->sizeCmd);

    if (_cmdFlag & MappingCmd_Pattern)
      printf("pattern: %s --> %s\n", _cmd->patternCmd, _tclCmd->patternCmd);

    if (_cmdFlag & MappingCmd_Shape)
      printf("shape: %s --> %s\n", _cmd->shapeCmd, _tclCmd->shapeCmd);

    if (_cmdFlag & MappingCmd_Orientation)
      printf("orientation: %s --> %s\n", _cmd->orientationCmd,
	     _tclCmd->orientationCmd);

    for(int j = 0; j < MAX_SHAPE_ATTRS; j++) {
      if (_cmdAttrFlag & (1 << j))
	printf("shapeAttr_%d: %s --> %s\n", j, _cmd->shapeAttrCmd[j],
	       _tclCmd->shapeAttrCmd[j]);
    }
  }
}

//--------------------------------------------------------------------------
// Return the value to be mapped using from as the pointer to record,
// entry as entry describing mapping, and defaultVal as the default value.

double MappingInterp::ConvertOneAttr(char *from, MappingSimpleCmdEntry *entry, 
				 double defaultVal, StringStorage *stringTable)
{
#if defined(DEBUG)
  printf("MappingInterp::ConvertOneAttr(0x%p)\n", from);
#endif

  AttrInfo *info;
  int offset;
  char *ptr;
  double retVal;

  int code = 0;
  int key = 0;

//  printf("entry->cmdType : %d\n", entry->cmdType);
  switch(entry->cmdType) {
  
  case MappingSimpleCmdEntry::AttrCmd: {
    info = _tdata->GetAttrList()->Get(entry->cmd.attrNum);
    if (!info)
      return MappingInterp::_recId;

    offset = info->offset;
    ptr = from + offset;

    switch(info->type) {

      case IntAttr:{
      	int tmp;
      	memcpy(&tmp, ptr, sizeof(int));
	return tmp;
        break;
      }
      case FloatAttr:{
      	float tmp;
      	memcpy(&tmp, ptr, sizeof(float));
	return tmp;
        break;
     }

      case DoubleAttr:{
      	double tmp;
      	memcpy(&tmp, ptr, sizeof(double));
	return tmp;
        break;
      }

      case StringAttr:
        code = stringTable->Insert(ptr, key);
#ifdef DEBUG
        printf("Converted string \"%s\" to key %d, code %d\n", ptr, key, code);
#endif
        if (code < 0)
          return defaultVal;
        return (double)key;
        break;

      case DateAttr:{
      	time_t tmp;
      	memcpy(&tmp, ptr, sizeof(time_t));
	return (double) tmp;
        break;
      }
    }
    DOASSERT(0, "Unknown attr cmd type");
    break;
  }
  case MappingSimpleCmdEntry::ConstCmd:
    /*
       printf("returning num: %f\n", entry->cmd.num);
    */
    return entry->cmd.num;
    break;

  case MappingSimpleCmdEntry::NULLCmd:
    /*
       printf("returning NULL\n");
    */
    return defaultVal;
    break;
  }
  fflush(stdout);
  DOASSERT(0, "Unknown simple command type");

  // keep compiler happy
  return 0;
}

//--------------------------------------------------------------------------
void MappingInterp::ConvertToGDataSimple(RecId startRecId, void *buf,
					 int numRecs, void *gdataPtr)
{
#if defined(DEBUG)
  printf("ConvertToGDataSimple\n");
#endif

  int tRecSize = TDataRecordSize();
  int gRecSize = GDataRecordSize();
  char *tPtr = (char *)buf;
  char *gPtr = (char *)gdataPtr;
  _recId = startRecId;

  StringStorage *xStringTable = GetStringTable(TableX);
  StringStorage *yStringTable = GetStringTable(TableY);
  StringStorage *zStringTable = GetStringTable(TableZ);
  StringStorage *genStringTable = GetStringTable(TableGen);

  for(int i = 0; i < numRecs; i++) {
    DOASSERT((int)gPtr % sizeof(double) == 0, "Unaligned GData record");

    /* Store ID of current record */
    *((RecId *)(gPtr + _offsets->_recIdOffset)) = startRecId + i;
    
    double *dPtr;
    if (_offsets->_xOffset >= 0) {
      dPtr = (double *)(gPtr + _offsets->_xOffset);
      *dPtr = ConvertOneAttr(tPtr, &_simpleCmd->xCmd, 1.0, xStringTable);
    }
    if ( _offsets->_yOffset >= 0) {
      dPtr = (double *)(gPtr + _offsets->_yOffset);
      *dPtr = ConvertOneAttr(tPtr, &_simpleCmd->yCmd, 1.0, yStringTable);
    }
    if (_offsets->_zOffset >= 0) {
      dPtr = (double *)(gPtr + _offsets->_zOffset);
      *dPtr = ConvertOneAttr(tPtr, &_simpleCmd->zCmd, 1.0, zStringTable);
    }

	// Color command
	if (_offsets->_colorOffset >= 0)
	{
		PColorID*	pcid = (PColorID*)(gPtr + _offsets->_colorOffset);

		*pcid = (PColorID)ConvertOneAttr(tPtr, &_simpleCmd->colorCmd, 1.0,
		  genStringTable);
	}

    if (_offsets->_sizeOffset >= 0) {
      dPtr = (double *)(gPtr + _offsets->_sizeOffset);
      *dPtr = ConvertOneAttr(tPtr, &_simpleCmd->sizeCmd, 1.0, genStringTable);
    }
    if (_offsets->_patternOffset >= 0) {
      Pattern *pPtr = (Pattern *)(gPtr + _offsets->_patternOffset);
      *pPtr = (Pattern)ConvertOneAttr(tPtr, &_simpleCmd->patternCmd, 0.0,
	    genStringTable);
    }
    if (_offsets->_orientationOffset >= 0) {
      dPtr = (double *)(gPtr + _offsets->_orientationOffset);
      *dPtr = ConvertOneAttr(tPtr, &_simpleCmd->orientationCmd, 0.0,
	    genStringTable);
    }
    if (_offsets->_shapeOffset >= 0) {
      ShapeID *sPtr = (ShapeID *)(gPtr + _offsets->_shapeOffset);
      *sPtr = (ShapeID) ConvertOneAttr(tPtr, &_simpleCmd->shapeCmd, 1.0,
	    genStringTable);
    }

    for(int j = 0; j <= _maxGDataShapeAttrNum; j++) {
      if (_offsets->_shapeAttrOffset[j] >= 0) {
	double *dPtr = (double *)(gPtr + _offsets->_shapeAttrOffset[j]);
	*dPtr =  ConvertOneAttr(tPtr, &_simpleCmd->shapeAttrCmd[j], 0.1,
	  genStringTable);
#ifdef DEBUG
	printf("ConvertGData: shape attribute %d: %.2f\n", j, *dPtr);
#endif
      }
    }

    tPtr += tRecSize;
    gPtr += gRecSize;

    _recId++;
  }
}

//--------------------------------------------------------------------------
void MappingInterp::ConvertToGDataComplex(RecId startRecId, void *buf,
					  int numRecs, void *gdataPtr)
{
#if defined(DEBUG)
  printf("ConvertToGDataComplex\n");
#endif

  int tRecSize = TDataRecordSize();
  int gRecSize = GDataRecordSize();

#ifdef DEBUG
  printf("ConvertToGDataComplex: gRecSize = %d, tRecSize %d\n",
	 gRecSize, tRecSize);
#endif

  char *tPtr = (char *)buf;
  char *gPtr = (char *)gdataPtr;
  _recId = startRecId;

  StringStorage *xStringTable = GetStringTable(TableX);
  StringStorage *yStringTable = GetStringTable(TableY);
  StringStorage *zStringTable = GetStringTable(TableZ);
  StringStorage *genStringTable = GetStringTable(TableGen);

  for(int i = 0; i < numRecs; i++) {
    DOASSERT((int)gPtr % sizeof(double) == 0, "Unaligned GData record");

    /* Store ID of current record */
    *((RecId *)(gPtr + _offsets->_recIdOffset)) = startRecId + i;

    /* Initialize tdata variables into tcl variables.*/
#if 0
    printf("setting attr values\n");
#endif

#ifdef DEBUG
  printf("!!!!!!!!!!!!!!!!!!!!!!!!!COMPLEX CMD!!!!!!!!!!!!!!!!!!!\n");
#endif
    /* evaluate commands */
    int code;
    static char cmdbuf[255];
    const int maxcmd = sizeof cmdbuf - 7 - 1;

	InsertExprAttrs(startRecId + i, tPtr, xStringTable);
    if (_offsets->_xOffset >= 0) {
      if (_tclCmd->xCmd == NULL) {
	      _exprResult = GetDefaultX();
      } else {
	// added by whh, support for native expression analysis
	_exprResult = EvalExpr( _pNativeExpr->pExprX );
      }
      *((double *)(gPtr + _offsets->_xOffset)) = _exprResult;
    }
    
	if (yStringTable != xStringTable) {
	  InsertExprAttrs(startRecId + i, tPtr, yStringTable);
	}
    if (_offsets->_yOffset >= 0) {
      if (_tclCmd->yCmd == NULL) {
	_exprResult = GetDefaultY();
      } else {
	// added by whh, support for native expression analysis
	_exprResult = EvalExpr( _pNativeExpr->pExprY );
      }
      *((double *)(gPtr + _offsets->_yOffset)) = _exprResult;
    }
    
	if (zStringTable != yStringTable) {
	  InsertExprAttrs(startRecId + i, tPtr, zStringTable);
	}
    if (_offsets->_zOffset >= 0) {
      if (_tclCmd->zCmd == NULL) {
	_exprResult = GetDefaultZ();
      } else {
	// added by whh, support for native expression analysis
	_exprResult = EvalExpr( _pNativeExpr->pExprZ );
      }
      *((double *)(gPtr + _offsets->_zOffset)) = _exprResult;
    }

    // Color command
	if (genStringTable != zStringTable) {
	  InsertExprAttrs(startRecId + i, tPtr, genStringTable);
	}
    if (_offsets->_colorOffset >= 0 ) {
      if (_tclCmd->colorCmd == NULL) {
        _exprResult = (Coord)nullPColorID;
      } else {
        // added by whh, support for native expression analysis
        _exprResult = EvalExpr( _pNativeExpr->pExprColor );
      }

      *((PColorID*)(gPtr + _offsets->_colorOffset)) = (PColorID)_exprResult;
    }

    if (_offsets->_sizeOffset >= 0) {
      if (_tclCmd->sizeCmd == NULL) {
	_exprResult = GetDefaultSize();
      } else {
	//added by whh, support for native expression analysis
	_exprResult = EvalExpr( _pNativeExpr->pExprSize );
      }
      /*
	 printf("eval size\n");
      */
      *((double *)(gPtr + _offsets->_sizeOffset)) = _exprResult;
    }
    
    if (_offsets->_patternOffset >= 0) {
      if (_tclCmd->patternCmd == NULL) {
	_exprResult = (double) GetDefaultPattern();
      } else {
	//added by whh, support for native expression analysis
	_exprResult = EvalExpr( _pNativeExpr->pExprPattern );
      }
      /*
	 printf("eval pattern\n");
      */
      *((Pattern *)(gPtr + _offsets->_patternOffset)) = (Pattern)_exprResult;
    }
    
    if (_offsets->_orientationOffset >= 0 ) {
      if (_tclCmd->orientationCmd == NULL) {
	_exprResult = GetDefaultOrientation();
      } else {
	// added by whh, support for native expression analysis
	_exprResult = EvalExpr( _pNativeExpr->pExprOrientation );
      }
      /*
	 printf("eval orientation\n");
      */
      *((double *)(gPtr + _offsets->_orientationOffset))= _exprResult;
    }

    if (_offsets->_shapeOffset >= 0) {
      if (_tclCmd->shapeCmd == NULL) {
	_exprResult = GetDefaultShape();
      } else {
	// added by whh, support for native expression analysis
	_exprResult = EvalExpr( _pNativeExpr->pExprShape );

	if (_exprResult <0 || _exprResult >= MaxInterpShapes)
	  _exprResult = 0;
      }
      /*
	 printf("eval shape\n");
      */
      *((ShapeID *)(gPtr + _offsets->_shapeOffset))= (ShapeID )_exprResult;
    }

    const ShapeAttr *shapeAttr = GetDefaultShapeAttrs();
    for(int j = 0; j <= _maxGDataShapeAttrNum; j++) {
      if (_offsets->_shapeAttrOffset[j] >= 0) {
	if (_tclCmd->shapeAttrCmd[j] == NULL) {
	  _exprResult = shapeAttr[j];
	} else {
	  // added by whh, support for native expression analysis
	  _exprResult = EvalExpr( _pNativeExpr->ppExprGDataAttr[j] );
	}
#if defined(DEBUG)
        printf("ShapeAttr%d = %f\n", j, _exprResult);
#endif
	*((double *)(gPtr + _offsets->_shapeAttrOffset[j]))= _exprResult;
      }
    }

    InitAttrList();

    tPtr += tRecSize;
    gPtr += gRecSize;

    _recId++;
  }


}

//--------------------------------------------------------------------------
void
MappingInterp::InsertExprAttrs(RecId recId, char *tDataRec,
  StringStorage *stringTable)
{
  InitAttrList();
  InsertAttr(REC_ID_NAME , recId);

  for(int attrNum = 0; attrNum <= _maxTDataAttrNum; attrNum++) {
    if (_tdataFlag->TestBit(attrNum)) {
	  /* attrNum'th attr of TData is to be used */
#if 0
      printf("bit %d set\n", attrNum);
#endif
	  AttrInfo *attrInfo = _tdata->GetAttrList()->Get(attrNum);
	  void *attrP;

	  switch(attrInfo->type) {

	  case IntAttr: {
	    attrP = tDataRec + attrInfo->offset;
	    int tmpInt;
	    memcpy((void *) &tmpInt, attrP, sizeof(tmpInt));

	    // added by whh, support for native expression analysis
	    InsertAttr( attrInfo->name, (double)tmpInt );

#if defined(DEBUG)
	    printf("Setting int attr %d to %f\n", attrNum, (double) tmpInt);
#endif
	    break;
      }

	  case FloatAttr: {
	    attrP = tDataRec + attrInfo->offset;
	    float tmpFloat;
	    memcpy((void *) &tmpFloat, attrP, sizeof(tmpFloat));

	    // added by whh, support for native expression analysis
	    InsertAttr( attrInfo->name, (double)tmpFloat );

#if defined(DEBUG)
	    printf("Setting float attr %d to %f\n", attrNum, (double) tmpFloat);
#endif
	    break;
      }

	  case DoubleAttr: {
	    attrP = tDataRec + attrInfo->offset;
	    double tmpDbl;
	    memcpy((void *) &tmpDbl, attrP, sizeof(tmpDbl));
	  
	    // added by whh, support for native expression analysis
	    InsertAttr( attrInfo->name, tmpDbl );
	  
#if defined(DEBUG)
	    printf("Setting double attr %d to %f\n", attrNum, tmpDbl);
#endif
	    break;
      }

	  case StringAttr: {
        int code = 0;
        int key = 0;
        char *string = NULL;

	    string = tDataRec + attrInfo->offset;
	    code = stringTable->Insert(string, key);
        DOASSERT(code >= 0, "Cannot insert string");

	    // added by whh, support for native expression analysis
	    InsertAttr( attrInfo->name, (double)key );
	  
#if defined(DEBUG)
        printf("  Setting string attr %d to %f\n", attrNum, (double) key);
#endif
	    break;
      }

	  case DateAttr: {
	    attrP = tDataRec + attrInfo->offset;
	    time_t tmpTimeT;
	    memcpy((void *) &tmpTimeT, attrP, sizeof(tmpTimeT));

	    // added by whh, support for native expression analysis
	    InsertAttr( attrInfo->name, (double)tmpTimeT );
	  
#if defined(DEBUG)
	    printf("Setting date attr %d to %f\n", attrNum, (double) tmpTimeT);
#endif
	    break;
      }

	  default:
	    DOASSERT(0, "Unknown attribute type");
	  }
    }
  }
}

void
MappingInterp::FindBoundingBoxes(void *gdataArray, int numRecs)
{
#if defined(DEBUG)
  printf("MappingInterp(%s)::FindBoundingBoxes(%d)\n", GetName(), numRecs);
#endif

  if (_offsets->_shapeOffset < 0) {
    /* constant shape */
    ShapeID shape = GetDefaultShape();
	Coord tmpMaxW, tmpMaxH;
    _shapes[shape]->FindBoundingBoxes(gdataArray, numRecs, this, tmpMaxW,
	    tmpMaxH);
    _maxSymWidth = MAX(_maxSymWidth, tmpMaxW);
    _maxSymHeight = MAX(_maxSymHeight, tmpMaxH);
  } else {
	// Note: this could be made more efficient by making one call to
	// FindBoundingBoxes() for each group of the same shape; but I'm not
	// going to worry about it for now.  RKW 1999-05-25.
	char *dataP = (char *)gdataArray; // char * for ptr arithmetic
    for (int recNum = 0; recNum < numRecs; recNum++) {
      ShapeID shape = *((ShapeID *)(dataP + _offsets->_shapeOffset));
	  Coord tmpMaxW, tmpMaxH;
      _shapes[shape]->FindBoundingBoxes(dataP, 1, this, tmpMaxW, tmpMaxH);
      _maxSymWidth = MAX(_maxSymWidth, tmpMaxW);
      _maxSymHeight = MAX(_maxSymHeight, tmpMaxH);
	  dataP += GDataRecordSize();
    }
  }
}

//******************************************************************************

/*
  ========================================================================
  DEVise Data Visualization Software
  (c) Copyright 1992-1998
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
  Revision 1.37  1998/04/16 21:51:42  wenger
  Committed Sanjay's text code.

  Revision 1.36  1998/02/26 22:59:52  wenger
  Added "count mappings" to views, except for API and GUI (waiting for
  Dongbin to finish his mods to ParseAPI); conditionaled out unused parts
  of VisualFilter struct; did some cleanup of MappingInterp class.

  Revision 1.35  1998/01/30 21:53:18  wenger
  Did some cleaning up of the MappingInterp and NativeExpr code
  (NativeExpr still needs a lot more); NativeExpr code can now
  parse expressions containing constant strings (they are treated
  as numerical zero for now) (this fixes bug 275).

  Revision 1.34  1997/12/04 18:31:39  wenger
  Merged new expression evaluation code thru the expression_br_2 tag.

  Revision 1.33.6.1  1997/12/03 22:52:28  whh
  Devise now uses new C++ expression evaluation code instead of Tcl
  interpreter.

  Revision 1.33  1997/09/09 21:41:05  wenger
  Workaround for bug 218 (images drawn even when outside visual filter):
  images are no longer complex shapes.

  Revision 1.32  1997/08/20 22:11:02  wenger
  Merged improve_stop_branch_1 through improve_stop_branch_5 into trunk
  (all mods for interrupted draw and user-friendly stop).

  Revision 1.31.8.2  1997/08/14 16:16:03  wenger
  Statistics, etc., now work correctly for timed-out draw in ViewScatter-
  type views; bumped up version because of improved stop capability.

  Revision 1.31.8.1  1997/08/07 16:56:37  wenger
  Partially-complete code for improved stop capability (includes some
  debug code).

  Revision 1.31  1997/04/29 17:35:13  wenger
  Minor fixes to new text labels; added fixed text label shape;
  CheckDirSpace() no longer prints an error message if it can't get disk
  status.

  Revision 1.30  1997/04/25 22:48:58  ssl
  Fixed bug in TData flush. MappingInterp doesn't store attr info anymore
  as DTE can change it.
  (Doesnt seem like a very severe performance penalty and is needed for
  correctness).

  Revision 1.29  1997/04/21 22:54:53  guangshu
  Added function MapTAttr2GAttr.

  Revision 1.28  1997/03/19 19:41:49  andyt
  Embedded Tcl/Tk windows are now sized in data units, not screen pixel
  units. The old list of ETk window handles (WindowRep class) has been
  replaced by a list of ETkInfo structs, each with fields for the window
  handle, x-y coordinates, name of the Tcl script, and an "in_use"
  flag. Added an ETk_Cleanup() procedure that gets called inside
  View::ReportQueryDone and destroys all ETk windows that are not marked
  as in_use.

  Revision 1.27  1996/12/30 23:57:36  andyt
  First version with support for Embedded Tcl/Tk windows. Added new
  ETkWindow symbol shape. Improved the MappingInterp::MapGAttr2TAttr
  function to handle all GData attributes (used to only handle a subset).

  Revision 1.26  1996/11/23 21:19:32  jussi
  Removed failing support for variable-sized records.

  Revision 1.25  1996/09/27 15:53:20  wenger
  Fixed a number of memory leaks.

  Revision 1.24  1996/07/19 02:53:41  jussi
  Increase number of shapes to 15 (LineShape and LineShadeShape
  were added). RecId is now the first GData attribute in all
  GData records.

  Revision 1.23  1996/07/15 17:02:02  jussi
  Added support for string attributes in GData.

  Revision 1.22  1996/07/10 00:03:38  jussi
  Replaced TDataMapDispatch with TDataMap.

  Revision 1.21  1996/07/02 22:45:59  jussi
  The bounding box of symbols is now correctly computed. Scatter
  plots sometimes did not have all necessary data displayed in
  them, as bounding box used to be incorrectly computed.

  Revision 1.20  1996/06/27 19:06:56  jussi
  Merged 3D block shape into 2D rect shape, the appropriate shape
  is chosen based on current view setting. Removed Block and 3DVector
  shapes. Added empty default 3D drawing routine to all other
  shapes.

  Revision 1.19  1996/06/16 01:53:56  jussi
  Added PolylineShape, PolylineFileShape, and TextLabelShape.
  Made cmdFlag and cmdAttrFlag unsigned long int's.
  Added IsComplexShape() method.

  Revision 1.18  1996/05/31 21:35:34  wenger
  Fixed core dump in SPARC/Solaris version caused by GData buffer
  misalignment; cleaned up generic/Makefile.base, etc., to get HP
  version to link correctly and eliminate special Makefile.base.aix.

  Revision 1.17  1996/05/22 21:05:10  jussi
  Added HighLowShape. Added tentative version of GifImageShape.

  Revision 1.16  1996/05/07 16:39:09  jussi
  Added MapGAttr2TAttr() method for translating GData attributes
  to TData attributes.

  Revision 1.15  1996/04/23 20:35:41  jussi
  Added Segment shape which just connects two end points.

  Revision 1.14  1996/04/16 20:54:14  jussi
  Added HorLineShape, a 2D horizontal line shape that is used
  by statistical views.

  Revision 1.13  1996/04/09 22:54:46  jussi
  Added View parameter to DrawGDataArray().

  Revision 1.12  1996/02/13 16:30:01  jussi
  Made declaration of friend ConvertOne use the inline keyword.

  Revision 1.11  1996/02/05 02:53:49  yuc
  Update MaxInterpShapes to 8, for adding
  3d vector

  Revision 1.10  1996/01/27 00:18:39  jussi
  Made recId be evaluated as a simple command rather than as
  a complex command.

  Revision 1.9  1996/01/13 23:10:18  jussi
  Added support for Z attribute and shape attribute 2.

  Revision 1.8  1996/01/09 22:27:59  jussi
  Added 3D block shape.

  Revision 1.7  1995/12/22 18:07:47  jussi
  Increased MaxInterpShapes to accommodate the new Vector shape.

  Revision 1.6  1995/11/28 00:06:02  jussi
  Increased MaxInterpShapes to reflect the addition of polygon and
  oval shapes.

  Revision 1.5  1995/11/25  01:20:14  jussi
  This code now uses Transform matrix operations to convert user/world
  coordinates to screen pixel coordinates. This is to avoid any future
  inconsistencies in how the different code locations compute the
  conversion. xPerPixel and yPerPixel are now obsolete coefficients.

  Revision 1.4  1995/11/22 00:34:38  jussi
  Added copyright notice and cleaned up the code a bit.

  Revision 1.3  1995/09/26 23:07:51  jussi
  Added attrList parameter to FindGDataSize so that attribute-valued
  color/size/shape/pattern/orientation would work.

  Revision 1.2  1995/09/05 22:15:03  jussi
  Added CVS header.
*/

#ifndef MappingInterp_h
#define MappingInterp_h

#include <tcl.h>

#include "TDataMap.h"
#include "Bitmap.h"
#include "GDataRec.h"
#include "AttrList.h"

class TData;

const int MappingCmd_X           = (1 << 0);
const int MappingCmd_Y           = (1 << 1);
const int MappingCmd_Z           = (1 << 2);
const int MappingCmd_Color       = (1 << 3);
const int MappingCmd_Size        = (1 << 4);
const int MappingCmd_Pattern     = (1 << 5);
const int MappingCmd_Orientation = (1 << 6);
const int MappingCmd_Shape       = (1 << 7);
const int MappingInterpAllFlags  = 0xffff;

/* structure used to store the commands for mapping */
struct MappingInterpCmd {
  char *xCmd;
  char *yCmd;
  char *zCmd;
  char *colorCmd;
  char *sizeCmd;
  char *patternCmd;
  char *shapeCmd;
  char *orientationCmd;
  char *shapeAttrCmd[MAX_GDATA_ATTRS];
};

struct MappingSimpleCmdEntry {
  enum SimpleCmdType { AttrCmd = 0, ConstCmd, NULLCmd };
  SimpleCmdType cmdType; /* type of command: either attribute, or constant*/
  union{
    int attrNum;		// rather than keeping a pointer which is 
				// unsafe as DTE can change it, we keep the 
				// attrnum which can be used to retrieve the 
				// attrInfo at any point.
//    AttrInfo *attr;      /* ptr to attribute info */
    double num;          /* value of constant field */
  } cmd;
};

struct MappingSimpleCmd {
  MappingSimpleCmdEntry xCmd;
  MappingSimpleCmdEntry yCmd;
  MappingSimpleCmdEntry zCmd;
  MappingSimpleCmdEntry colorCmd;
  MappingSimpleCmdEntry sizeCmd;
  MappingSimpleCmdEntry patternCmd;
  MappingSimpleCmdEntry shapeCmd;
  MappingSimpleCmdEntry orientationCmd;
  MappingSimpleCmdEntry shapeAttrCmd[MAX_GDATA_ATTRS];
};

// added by whh, support for native expression analysis
class CGraphicExpr;

class Shape;
class AttrList;
#ifdef VIEW_SHAPE 
const unsigned int MaxInterpShapes = 19;
#else 
const unsigned int MaxInterpShapes = 18;
#endif


class MappingInterp: public TDataMap {
//  friend inline double ConvertOne(char *from,
//				  MappingSimpleCmdEntry *entry,
//				  double defaultVal);
  
protected:
double ConvertOne(char *from,
		  MappingSimpleCmdEntry *entry,
		  double defaultVal);
public:
  // cmdFlag tells which attributes have some kind of value given for them
  // (see MappingCmd_X, etc.).  attrFlag tells which shape attributes have
  // some kind of value.
  MappingInterp(char *name,
		TData *tdata, MappingInterpCmd *cmd,
		unsigned long int cmdFlag,
		unsigned long int attrFlag,
		VisualFlag *dimensionInfo, int numDimensions);
  ~MappingInterp();

  MappingInterpCmd *GetMappingCmd() { return _cmd; }

  virtual Boolean IsInterpreted() { return true; }
				  
  // See assignments to _shapes array in constructor for meanings of ShapeID
  // values.
  virtual Boolean IsComplexShape(ShapeID shape) {
    switch (shape) {
    case 9:
    case 11:
    case 13:
    case 14:
      return true;
    }
    return false;
  }

  /* change the commands */
  // cmdFlag tells which attributes have some kind of value given for them
  // (see MappingCmd_X, etc.).  attrFlag tells which shape attributes have
  // some kind of value.
  void ChangeCmd(MappingInterpCmd *cmd, unsigned long int cmdFlag,
		 unsigned long int attrFlag, VisualFlag *dimensionInfo,
		 int NumDimensions);
  
#if 0 // Not currently used.  RKW Feb. 26, 1998.
  /* Get current commands */
  MappingInterpCmd *GetCmd(unsigned long int &cmdFlag,
			   unsigned long int &attrFlag);
#endif
  
  /* Update maximum symbol size */
  void UpdateMaxSymSize(void *gdata, int numSyms);
  
  virtual void DrawGDataArray(ViewGraph *view, WindowRep *win,
			      void **gdataArray, int num,
			      int &recordsProcessed);

  /* Get the AttrInfo for a GData attribute. which_attr should be
     one of the MappingCmd_??? constants defined at the top of 
     this file. */
  virtual AttrInfo *MapGAttr2TAttr(int which_attr);
  virtual char *MapTAttr2GAttr(char *tname);

  /* Get the AttrInfo for shape attribute i */
  virtual AttrInfo *MapShapeAttr2TAttr(int i);
				  

protected:	
  /* convert from Tdata to Gdata. buf contains buffer for data. */
  /* WARNING: gdataPtr must be aligned on a double (8 byte) boundary. */
  virtual void ConvertToGData(RecId startRecId, void *buf,
			      int numRecs, void *gdataPtr);

private:
  /* Initialize command by converting _cmd into _tclCmd,
     and initializing _tdataFlag */
  AttrList *InitCmd(char *name);
  
  /* Convert from interpreter command of the into TCL commnd 
     Example:
     TDAta has attributes time and radiaton
     interpreter command is: [expr $time*2]
     after conversion it's: [expr $interpAttr_0*2]
     global updated: _tdataFlag, and _maxTDataAttrNum;
  */
  char *ConvertCmd(char *cmd, AttrType &type, Boolean &isSorted);

  /* Return TRUE if cmd is a simple command,
     and set entry to the converted command entry.
     If entry is an attribute, also return the attribute type,
     and whether it's sorted.*/
  static Boolean ConvertSimpleCmd(char *cmd, AttrList *attrList,
  				  MappingSimpleCmdEntry &entry,
			          AttrType &type, Boolean &isSorted);
  
  /* Print one entry of simple command */
  void PrintSimpleCmdEntry(MappingSimpleCmdEntry *entry);
  
  /* Print command */
  void PrintCmd();
  
  /* Do convert to GData for simple command */
  /* WARNING: gdataPtr must be aligned on a double (8 byte) boundary. */
  void ConvertToGDataSimple(RecId startRecId, void *buf,
			    int numRecs, void *gdataPtr);
  
  /* Find size of GData given attribute flag information */
  static FindGDataSize(MappingInterpCmd *cmd, AttrList *attrList,
		       unsigned long int cmdFlag, unsigned long int attrFlag);
  
  static Boolean IsConstCmd(char *cmd, AttrList *attrList, Coord &val,
			    AttrType &attrType);
  
  /* command for the mapping and the associated flags */
  MappingInterpCmd *_cmd;
  MappingInterpCmd *_tclCmd;     /* actual tcl command used */
  MappingSimpleCmd *_simpleCmd;  /* simple command */
  Boolean _isSimpleCmd;          /* true if _simpleCmd, otherwise _tclCmd */
  
  unsigned long int _cmdFlag;
  unsigned long int _cmdAttrFlag;
  
  GDataAttrOffset *_offsets;   /* Offsets of GData attributes */
  
  Bitmap *_tdataFlag;          /* bit i set if ith attribute of TData is used */
  int _maxTDataAttrNum;        /* max # of attributes in TData used */
  int _maxGDataShapeAttrNum;   /* max shape attribute number encountered */
  
  double _exprResult;          /* result of evaluating an expression */
  
  static Shape **_shapes;
  int _recId;
  
  // native expression analysis engine
  // (only used for "complex" commands
  CGraphicExpr *_pNativeExpr;  

  TData *_tdata;
};

#endif

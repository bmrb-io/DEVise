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

#include "TDataMapDispatch.h"
#include "GDataRec.h"
#include "TData.h"
#include "AttrList.h"

/* structure used to store the commands for mapping */
const int MappingCmd_X           = (1 << 0);
const int MappingCmd_Y           = (1 << 1);
const int MappingCmd_Z           = (1 << 2);
const int MappingCmd_Color       = (1 << 3);
const int MappingCmd_Size        = (1 << 4);
const int MappingCmd_Pattern     = (1 << 5);
const int MappingCmd_Orientation = (1 << 6);
const int MappingCmd_Shape       = (1 << 7);
const int MappingInterpAllFlags  = 0xffff;

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
  enum SimpleCmdType { AttrCmd, ConstCmd, NULLCmd };
  SimpleCmdType cmdType; /* type of command: either attribute, or constant*/
  union{
    AttrInfo *attr;      /* ptr to attribute info */
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

class Shape;
class AttrList;
const int MaxInterpShapes = 15;

class MappingInterp: public TDataMapDispatch {
  friend inline double ConvertOne(char *from,
				  MappingSimpleCmdEntry *entry,
				  double defaultVal);

public:
  MappingInterp(char *name,
		TData *tdata, MappingInterpCmd *cmd,
		unsigned long int flag,
		unsigned long int attrFlag,
		VisualFlag *dimensionInfo, int numDimensions);

  MappingInterpCmd *GetMappingCmd() { return _cmd; }

  virtual Boolean IsInterpreted() { return true; }
  virtual Boolean IsComplexShape(ShapeID shape) {
    if (shape <= 10)
      return false;
    return true;
  }

  /* change the commands */
  void ChangeCmd(MappingInterpCmd *cmd, unsigned long int flag,
		 unsigned long int attrFlag, VisualFlag *dimensionInfo,
		 int NumDimensions);
  
  /* Get current commands */
  MappingInterpCmd *GetCmd(unsigned long int &cmdFlag,
			   unsigned long int &attrFlag);
  
  /* find the max box bounding for all records */
#if 0
  virtual void UpdateBoundingBox(int pageNum, void **gdataArray, int numRecs);
#endif

  virtual void DrawGDataArray(View *view, WindowRep *win,
			      void **gdataArray, int num);

  virtual AttrInfo *MapGAttr2TAttr(char *attrName);

protected:	
  /* convert from Tdata to Gdata. buf contains
     buffer for data. tRecs are pointers to variable size records only. */
  /* WARNING: gdataPtr must be aligned on a double (8 byte) boundary. */
  virtual void ConvertToGData(RecId startRecId, void *buf,
			      void **tRecs, int numRecs,
			      void *gdataPtr);
  
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
  Boolean ConvertSimpleCmd(char *cmd, MappingSimpleCmdEntry &entry,
			   AttrType &type, Boolean &isSorted);
  
  /* Print one entry of simple command */
  void PrintSimpleCmdEntry(MappingSimpleCmdEntry *entry);
  
  /* Print command */
  void PrintCmd();
  
  /* Do convert to GData for simple command */
  /* WARNING: gdataPtr must be aligned on a double (8 byte) boundary. */
  void ConvertToGDataSimple(RecId startRecId,void *buf,
			    void **tRecs,int numRecs,void *gdataPtr);
  
  /* Find size of GData given attribute flag information */
  FindGDataSize(MappingInterpCmd *cmd, AttrList *attrList,
		unsigned long int flag, unsigned long int attrFlag);
  
  Boolean IsConstCmd(char *cmd, Coord &val);
  
  /* command for the mapping and the associated flags */
  MappingInterpCmd *_cmd;
  MappingInterpCmd *_tclCmd;     /* actual tcl command used */
  MappingSimpleCmd *_simpleCmd;  /* simple command */
  Boolean _isSimpleCmd;          /* true if _simpleCmd, otherwise _tclCmd */
  
  unsigned long int _cmdFlag;
  unsigned long int _cmdAttrFlag;
  
  /* Offsets of GData attributes */
  GDataAttrOffset *_offsets;
  
  AttrList *_attrList;        /* list of tdata attributes */
  Bitmap *_tdataFlag;         /* bit i set if ith attribute of TData is used */
  int _maxTDataAttrNum;       /* max # of attributes in TData used */
  int _maxGDataShapeAttrNum;  /* max shape attribute number encountered */
  
  /* attributes used to evaluate tcl variables */
  static double *_tclAttrs;
  static char **_tclStrAttrs;
  static double _interpResult; /* result to be set by tcl interpreter */
  
  static Shape **_shapes;
  static int _tclRecId;
  
  /* tcl interpreter */
  static Tcl_Interp *_interp;
};

#endif

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
const int MappingCmd_Color       = (1 << 2);
const int MappingCmd_Size        = (1 << 3);
const int MappingCmd_Pattern     = (1 << 4);
const int MappingCmd_Orientation = (1 << 5);
const int MappingCmd_Shape       = (1 << 6);
const int MappingInterpAllFlags  = 0xffff;

struct MappingInterpCmd {
  char *xCmd, *yCmd, *colorCmd, *sizeCmd, *patternCmd,
  *shapeCmd, *orientationCmd;
  char *shapeAttrCmd[MAX_GDATA_ATTRS];
};

struct MappingSimpleCmdEntry {
  enum SimpleCmdType { AttrCmd, ConstCmd, NULLCmd};
  SimpleCmdType cmdType; /* type of command: either attribute, or constant*/
  union{
    AttrInfo *attr;      /* ptr to attribute info */
    double num;          /* value of constant field */
  } cmd;
};

struct MappingSimpleCmd {
  MappingSimpleCmdEntry xCmd, yCmd, colorCmd, sizeCmd, patternCmd,
  shapeCmd, orientationCmd;
  MappingSimpleCmdEntry shapeAttrCmd[MAX_GDATA_ATTRS];
};

class Shape;
class AttrList;
const int MaxInterpShapes = 7;

class MappingInterp: public TDataMapDispatch {
public:
  MappingInterp(char *name,
		TData *tdata, MappingInterpCmd *cmd, int flag,int attrFlag,
		VisualFlag *dimensionInfo, int numDimensions);

  MappingInterpCmd *GetMappingCmd() { return _cmd; }

  virtual Boolean IsInterpreted() { return true; }

  /* change the commands */
  void ChangeCmd(MappingInterpCmd *cmd, int flag, int attrFlag,
		 VisualFlag *dimensionInfo, int NumDimensions);
  
  /* Get current commands */
  MappingInterpCmd *GetCmd(int &cmdFlag, int &attrFlag);
  
  /* find the max box bounding for all records */
  /*
     virtual void UpdateBoundingBox(int pageNum,
     void **gdataArray, int numRecs);
  */

  virtual void DrawGDataArray(WindowRep *win, void **gdataArray, int num);

protected:	
  /* convert from Tdata to Gdata. buf contains
     buffer for data. tRecs are pointers to variable size records only. */
  virtual void ConvertToGData(RecId startRecId,void *buf,
			      void **tRecs,int numRecs,void *gdataPtr);
  
private:
  /* Initialize command by converting _cmd into _tclCmd,
     and initializing _tdataFlag*/
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
  
  /* print command */
  void PrintCmd();
  
  /* Do convert to GData for simple command */
  void ConvertToGDataSimple(RecId startRecId,void *buf,
			    void **tRecs,int numRecs,void *gdataPtr);
  
  /* Find size of GData given attribute flag information */
  FindGDataSize(MappingInterpCmd *cmd, AttrList *attrList,
		int flag, int attrFlag);
  
  Boolean IsConstCmd(char *cmd, Coord &val);
  
  /* command for the mapping and the associated flags */
  MappingInterpCmd *_cmd;
  MappingInterpCmd *_tclCmd; /* actual tcl command used */
  MappingSimpleCmd *_simpleCmd; /* simple command */
  Boolean _isSimpleCmd; /* true to use _simpleCmd. Otherwise, use _tclCmd */
  
  int _cmdFlag;
  int _cmdAttrFlag;
  
  /* Offsets of GData attributes */
  GDataAttrOffset *_offsets;
  
  AttrList *_attrList; /* list of tdata attributes */
  Bitmap *_tdataFlag; /* bit i set if ith attribute of TData is used */
  int _maxTDataAttrNum; /* max # of attributes in TData used */
  int _maxGDataShapeAttrNum; /* max shape attribute number encountered */
  
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

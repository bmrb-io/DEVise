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
#include "DeviseTypes.h"
#include "Bitmap.h"
#include "AttrList.h"
#include "TData.h"
#include "Pattern.h"
#include "Color.h"
#include "WindowRep.h"
#include "Temp.h"
#include "MappingInterp.h"
#include "MapInterpShape.h"
#include "Exit.h"
#include "Util.h"
#include "Exit.h"
#include "Init.h"

double *MappingInterp::_tclAttrs= NULL;
char **MappingInterp::_tclStrAttrs= NULL;
double MappingInterp::_interpResult;
int MappingInterp::_tclRecId;
Shape **MappingInterp::_shapes=NULL;
Tcl_Interp *MappingInterp::_interp= NULL;

/* Return true if command is a constant, and return the constant value */
Boolean MappingInterp::IsConstCmd(char *cmd, Coord &val)
{
  MappingSimpleCmdEntry entry;
  AttrType attrType;
  Boolean isSorted;
  if (ConvertSimpleCmd(cmd, entry,attrType, isSorted)
      && entry.cmdType == MappingSimpleCmdEntry::ConstCmd) {
    val = entry.cmd.num;
    return true;
  }
  return false;
}

/* Return the size of GData records, given command and attribute flags */
int MappingInterp::FindGDataSize(MappingInterpCmd *cmd, AttrList *attrList,
				 int flag, int attrFlag)
{
  _attrList = attrList;
  int size = 0;
  double val;

  if (flag & MappingCmd_X && !IsConstCmd(cmd->xCmd,val)) {
    size = WordBoundary(size, sizeof(double));
    size += sizeof(double);
  }
  if (flag & MappingCmd_Y && !IsConstCmd(cmd->yCmd,val)) {
    size = WordBoundary(size, sizeof(double));
    size += sizeof(double);
  }
  if ((flag & MappingCmd_Color) && !IsConstCmd(cmd->colorCmd,val)) {
    size = WordBoundary(size, sizeof(Color));
    size += sizeof(Color);
  }
  if ((flag & MappingCmd_Size) && !IsConstCmd(cmd->sizeCmd,val)) {
    size = WordBoundary(size, sizeof(double));
    size += sizeof(double);
  }
  if ((flag & MappingCmd_Pattern) && !IsConstCmd(cmd->patternCmd,val)) {
    size = WordBoundary(size, sizeof(Pattern));
    size += sizeof(Pattern);
  }
  if ((flag & MappingCmd_Shape) && !IsConstCmd(cmd->shapeCmd,val)) {
    size = WordBoundary(size, sizeof(ShapeID));
    size += sizeof(ShapeID);
  }
  if ((flag & MappingCmd_Orientation) && 
      !IsConstCmd(cmd->orientationCmd,val)) {
    size = WordBoundary(size, sizeof(double));
    size += sizeof(double);
  }
  
  for(int j = 0; j < MAX_GDATA_ATTRS; j++) {
    if ((attrFlag & (1 << j)) && !IsConstCmd(cmd->shapeAttrCmd[j], val) ) {
      size = WordBoundary(size, sizeof(double));
      size += sizeof(double);
    }
  }
  
  size = WordBoundary(size, sizeof(double));
  
#ifdef DEBUG
  printf("Gdata record size %d\n", size);
#endif

  return size;
}

MappingInterp::MappingInterp(char *name, TData *tdata,
			     MappingInterpCmd *cmd,
			     int flag, int attrFlag,
			     VisualFlag *dimensionInfo, int numDimensions):
	TDataMapDispatch(name, tdata, name,
			 FindGDataSize(cmd, tdata->GetAttrList(),
				       flag, attrFlag),
			 MappingInterpAllFlags, MappingInterpAllFlags,
			 Init::MaxGDataPages(),
			 dimensionInfo, numDimensions, true)
{
  /*
     printf("MappingInterp::constructor 0x%p, %d dimensions cmdFlag 0x%p, attrFlag 0x%p\n",
     this, numDimensions, flag, attrFlag);
  */

  _tclCmd = new MappingInterpCmd();

  /* 
     printf("new command color = '%s'\n", cmd->colorCmd);
  */

  _attrList = tdata->GetAttrList();
  _simpleCmd = new MappingSimpleCmd();

  if (_interp == NULL) {
    /* Init shapes */
    _shapes = new Shape *[MaxInterpShapes];
    _shapes[0] = new FullMapping_RectShape;
    _shapes[1] = new FullMapping_RectXShape;
    _shapes[2] = new FullMapping_BarShape;
    _shapes[3] = new FullMapping_PolygonShape;
    _shapes[4] = new FullMapping_OvalShape;
    _shapes[5] = new FullMapping_VectorShape;

    /* Init tcl */
    _interp = Tcl_CreateInterp();
    if (Tcl_Init(_interp) == TCL_ERROR) {
      fprintf(stderr, "MappingInterp: can't init tcl\n");
      Exit::DoExit(2);
    }
    /* set max precision */
    Tcl_Eval(_interp, "set tcl_precision 17");

    /* link interpreter variables with tcl variables */
    /* first, the result variable */
    Tcl_LinkVar(_interp, "recId",(char *)&_tclRecId, TCL_LINK_INT);
    Tcl_LinkVar(_interp, "interpResult", (char *)&_interpResult,
		TCL_LINK_DOUBLE);
    
    /* tcl variables used to store tdata variables */
    _tclAttrs = new double [MAX_TDATA_ATTRS];
    _tclStrAttrs = new char * [MAX_TDATA_ATTRS];
    char buf[80];
    for(int i = 0; i < MAX_TDATA_ATTRS; i++) {
      sprintf(buf, "interpAttr_%d",i);
      Tcl_LinkVar(_interp,buf,(char *)&_tclAttrs[i],TCL_LINK_DOUBLE);
      sprintf(buf, "interpStrAttr_%d",i);
      _tclStrAttrs[i] = NULL;
      Tcl_LinkVar(_interp,buf,(char *)&_tclStrAttrs[i],TCL_LINK_STRING);
    }
  }

  _tdataFlag = new Bitmap(MAX_TDATA_ATTRS);
  
  _offsets = new GDataAttrOffset;
  _cmd = cmd;
  _cmdFlag = flag;
  _cmdAttrFlag = attrFlag;
  AttrList *attrList = InitCmd(name);
  SetGDataAttrList(attrList);

  /* sorted in the X direction? */
  AttrInfo *info = attrList->Find("x");
  if (info->isSorted)
    SetDimensionInfo(new VisualFlag(VISUAL_X), 1);
}

void MappingInterp::ChangeCmd(MappingInterpCmd *cmd, int flag, int attrFlag,
			      VisualFlag *dimensionInfo, int numDimensions)
{
  _cmd = cmd;
  _cmdFlag = flag;
  _cmdAttrFlag = attrFlag;
  AttrList *attrList = GDataAttrList();
  delete attrList;
  attrList = InitCmd(GetName());
  SetGDataAttrList(attrList);

  /* sorted in the X direction? */
  AttrInfo *info = attrList->Find("x");
  if (info->isSorted)
    SetDimensionInfo(new VisualFlag(VISUAL_X), 1);

  TDataMap::ResetGData(FindGDataSize(cmd, _attrList, flag, attrFlag));
}

/* Get current commands */
MappingInterpCmd *MappingInterp::GetCmd(int &cmdFlag, int &attrFlag)
{
  cmdFlag = _cmdFlag;
  attrFlag = _cmdAttrFlag;
  return _cmd;
}

/* find the max box bounding for all records */
#if 0
void MappingInterp::UpdateBoundingBox(int pageNum,
				      void **gdataArray, 
				      int numRecs)
{
}
#endif

void MappingInterp::DrawGDataArray(WindowRep *win, void **gdataArray, int num)
{
  SetGDataOffset(_offsets);

  if (_offsets->shapeOffset < 0) {
    /* constant shape */
    ShapeID shape = GetDefaultShape();
#ifdef DEBUG
    printf("Drawing shape %d\n", shape);
#endif
    _shapes[shape]->DrawGDataArray(win, gdataArray, num, this,
				   GetPixelWidth());
  } else {
    /* dynamic shape */
    int i = 0;
    while (i < num) {
      ShapeID shape = *((ShapeID *)(gdataArray[i]+_offsets->shapeOffset));
      int j;
      for(j = i + 1; j < num; j++) {
	ShapeID nextShape =
	              *((ShapeID *)(gdataArray[j]+_offsets->shapeOffset));
	if (shape != nextShape)
	  break;
      }
      /* gdataArray[i..j-1] have the same shape */
#ifdef DEBUG
      printf("Drawing shape %d\n", shape);
#endif
      _shapes[shape]->DrawGDataArray(win, &gdataArray[i], j - i, this,
				     GetPixelWidth());
      i = j;
    }
  }
}

/*
   convert from Tdata to Gdata. buf contains
   buffer for data. tRecs are pointers to variable size records only.
*/

void MappingInterp::ConvertToGData(RecId startRecId,void *buf,
				   void **tRecs,int numRecs,void *gdataPtr)
{
  /*
     printf("ConvertToGdata id %d numRecs %d, buf 0x%p, gbuf 0x%p\n", 
     startRecId, numRecs, buf, gdataPtr);
  */

  if (_isSimpleCmd) {
    /* Do simple command conversion */
    ConvertToGDataSimple(startRecId, buf, tRecs, numRecs, gdataPtr);
    return;
  }
  
  int tRecSize = TDataRecordSize();
  int gRecSize = GDataRecordSize();
  /*
     printf("gRecSize = %d\n", gRecSize);
  */
  char *tPtr = (char *)buf;
  char *gPtr = (char *)gdataPtr;
  _tclRecId = startRecId;

  for(int i = 0; i < numRecs; i++) {
    /* Initialize tdata variables into tcl variables.*/
    /*
       printf("setting attr values\n");
    */

    int j;
    for(j = 0; j <= _maxTDataAttrNum; j++) {
      if (_tdataFlag->TestBit(j)) {
	/* jth attr of TData is to be used */
	/*
	   printf("bit %d set\n", j);
	*/
	AttrInfo *attrInfo = _attrList->Get(j);
	int *intPtr;
	float *fPtr;
	double *dPtr;
	time_t *tptr;

	switch(attrInfo->type) {

	case IntAttr:
	  intPtr = (int *)(tPtr+attrInfo->offset);
	  _tclAttrs[j] = (double)(*intPtr);
	  /*
	     printf("Setting int attr %d to %f\n",j, _tclAttrs[j]);
	  */
	  break;

	case FloatAttr:
	  fPtr = (float *)(tPtr+attrInfo->offset);
	  _tclAttrs[j] = (double)(*fPtr);
	  /*
	     printf("Setting float attr %d to %f\n",j, _tclAttrs[j]);
	  */
	  break;

	case DoubleAttr:
	  dPtr = (double *)(tPtr+attrInfo->offset);
	  _tclAttrs[j] = (*dPtr);
	  /*
	     printf("Setting float attr %d to %f\n",j, _tclAttrs[j]);
	  */
	  break;

	case StringAttr:
	  _tclStrAttrs[j] = tPtr+attrInfo->offset;
	  break;

	case DateAttr:
	  tptr = (time_t *)(tPtr+attrInfo->offset);
	  _tclAttrs[j] = (double)(*tptr);
	  break;

	default:
	  fprintf(stderr, "MappingInterp: unknown attr type\n");
	  Exit::DoExit(2);
	}
      }
    }
    
    /* evaluate commands */
    int code;
    
    /*
       printf("eval x\n");
    */

    if (_offsets->xOffset >= 0) {
      if (_tclCmd->xCmd == NULL) {
	_interpResult = GetDefaultX();
      } else {
	_interpResult = 0.0;
	code = Tcl_VarEval(_interp, "set interpResult ", _tclCmd->xCmd, NULL);
      }
      /*
	 printf("eval x\n");
      */
      *((double *)(gPtr+_offsets->xOffset)) = _interpResult;
    }
    
    /*
       printf("eval y\n");
    */
    if (_offsets->yOffset >= 0) {
      if (_tclCmd->yCmd == NULL) {
	_interpResult = GetDefaultY();
      } else {
	_interpResult = 0.0;
	code = Tcl_VarEval(_interp, "set interpResult ", _tclCmd->yCmd, NULL);
      }
      /*
	 printf("eval y\n");
      */
      *((double *)(gPtr+_offsets->yOffset)) = _interpResult;
      /*
	 printf("y is %f\n", _interpResult);
      */
    }
    
    if (_offsets->colorOffset >= 0 ) {
      if (_tclCmd->colorCmd == NULL) {
	_interpResult = (Coord) GetDefaultColor();
      } else {
	_interpResult = 0.0;
	code = Tcl_VarEval(_interp, "set interpResult ",
			   _tclCmd->colorCmd, NULL);
	printf("Color cmd code is %d\n", code);
      }
      /*
	 printf("eval color\n");
      */
      *((Color *)(gPtr+_offsets->colorOffset)) = (Color)_interpResult;
      /*
	 printf("color is %f\n", _interpResult);
      */
    }
    
    if (_offsets->sizeOffset >= 0) {
      if (_tclCmd->sizeCmd == NULL) {
	_interpResult = GetDefaultSize();
      } else {
	_interpResult = 0.0;
	code = Tcl_VarEval(_interp, "set interpResult ",
			   _tclCmd->sizeCmd, NULL);
      }
      /*
	 printf("eval size\n");
      */
      *((double *)(gPtr+_offsets->sizeOffset)) = _interpResult;
    }
    
    if (_offsets->patternOffset >= 0) {
      if (_tclCmd->patternCmd == NULL) {
	_interpResult = (double) GetDefaultPattern();
      } else {
	_interpResult = 0.0;
	code = Tcl_VarEval(_interp, "set interpResult ",
			   _tclCmd->patternCmd, NULL);
      }
      /*
	 printf("eval pattern\n");
      */
      *((Pattern *)(gPtr+_offsets->patternOffset)) = 
	(Pattern)_interpResult;
    }
    
    if (_offsets->shapeOffset >= 0) {
      if (_tclCmd->shapeCmd == NULL) {
	_interpResult = GetDefaultShape();
      } else {
	_interpResult = 0.0;
	code = Tcl_VarEval(_interp, "set interpResult ",
			   _tclCmd->shapeCmd, NULL);
	if (_interpResult <0 || _interpResult >= MaxInterpShapes)
	  _interpResult = 0;
      }
      /*
	 printf("eval shape\n");
      */
      *((ShapeID *)(gPtr+_offsets->shapeOffset))= (ShapeID )_interpResult;
    }

    if (_offsets->orientationOffset >= 0 ) {
      if (_tclCmd->yCmd == NULL) {
	_interpResult = GetDefaultOrientation();
      } else {
	_interpResult = 0.0;
	code = Tcl_VarEval(_interp, "set interpResult ",
			   _tclCmd->orientationCmd, NULL);
      }
      /*
	 printf("eval orientation\n");
      */
      *((double *)(gPtr+_offsets->orientationOffset))= _interpResult;
    }

    ShapeAttr *shapeAttr = GetDefaultShapeAttrs();
    for(j = 0; j <= _maxGDataShapeAttrNum; j++) {
      if (_offsets->shapeAttrOffset[j] >= 0) {
	if (_tclCmd->yCmd == NULL) {
	  _interpResult = shapeAttr[j];
	} else {
	  _interpResult = 0.0;
	  code = Tcl_VarEval(_interp, "set interpResult ",
			     _tclCmd->shapeAttrCmd[j], NULL);
	}
	/*
	   printf("eval shapeAttr %d\n", j);
	*/
	*((double *)(gPtr + _offsets->shapeAttrOffset[j]))= _interpResult;
      }
    }

    /*
       printf("ConvertGData gptr 0x%p, x: %f y: %f color %d\n",
       gPtr, *((double *)gPtr), *((double *)(gPtr+sizeof(double))),
       *((Color *)(gPtr+2*sizeof(double))));
    */

    tPtr += tRecSize;
    gPtr += gRecSize;
    _tclRecId++;
  }
}

AttrList *MappingInterp::InitCmd(char *name)
{
  Coord pixelWidth = 0.1;               // width of pixel in X units
  Coord pixelHeight = 0.1;              // height of pixel in Y units

  AttrList *attrList = new AttrList(name);

  /* Init offsets to GData attributes */
  _offsets->xOffset= _offsets->yOffset = _offsets->colorOffset = -1;
  _offsets->sizeOffset = _offsets->shapeOffset = -1;
  _offsets->patternOffset = _offsets->orientationOffset = -1;
  int i;
  for(i = 0; i < MAX_GDATA_ATTRS; i++)
    _offsets->shapeAttrOffset[i] = -1;
  
  _tdataFlag->ClearBitmap();
  _maxTDataAttrNum = 0;
  
  AttrType attrType;
  Boolean isSorted;

  int offset = 0;
  _isSimpleCmd = true;
  int j;

  if (_cmdFlag & MappingCmd_X) {
    if (!ConvertSimpleCmd(_cmd->xCmd, _simpleCmd->xCmd, attrType, isSorted))
      goto complexCmd;
    _offsets->xOffset = offset = WordBoundary(offset, sizeof(double));
    attrList->InsertAttr(0, "x",offset,sizeof(double), attrType,
			 false, NULL, false, isSorted);
    offset += sizeof(double);
  }

  if (_cmdFlag & MappingCmd_Y) {
    if (!ConvertSimpleCmd(_cmd->yCmd, _simpleCmd->yCmd, attrType, isSorted))
      goto complexCmd;
    _offsets->yOffset = offset = WordBoundary(offset,sizeof(double));
    attrList->InsertAttr(1, "y", offset, sizeof(double), attrType,
			 false,  NULL, false, isSorted);
    offset += sizeof(double);
  }

  if (_cmdFlag & MappingCmd_Color) {
    if (!ConvertSimpleCmd(_cmd->colorCmd, _simpleCmd->colorCmd, attrType,
			  isSorted))
      goto complexCmd;
    if (_simpleCmd->colorCmd.cmdType == MappingSimpleCmdEntry::ConstCmd) {
      /* constant */
      SetDefaultColor((Color)_simpleCmd->colorCmd.cmd.num);
    } else {
      _offsets->colorOffset = offset = WordBoundary(offset, sizeof(Color));
      attrList->InsertAttr(2, "color",offset,sizeof(double),attrType,
			   false,  NULL, false, isSorted);
      offset += sizeof(Color);
    }
  }

  if (_cmdFlag & MappingCmd_Size) {
    if (!ConvertSimpleCmd(_cmd->sizeCmd, _simpleCmd->sizeCmd, attrType,
			  isSorted))
      goto complexCmd;
    if (_simpleCmd->sizeCmd.cmdType == MappingSimpleCmdEntry::ConstCmd) {
      /* constant */
      SetDefaultSize((Coord)_simpleCmd->sizeCmd.cmd.num);
    } else {
      _offsets->sizeOffset = offset = WordBoundary(offset,sizeof(double));
      attrList->InsertAttr(3, "size",offset,sizeof(double),attrType,
			   false,  NULL, false, isSorted);
      offset += sizeof(double);
    }
  }

  if (_cmdFlag & MappingCmd_Pattern) {
    if (!ConvertSimpleCmd(_cmd->patternCmd, _simpleCmd->patternCmd, attrType,
			  isSorted))
      goto complexCmd;
    if (_simpleCmd->patternCmd.cmdType == MappingSimpleCmdEntry::ConstCmd) {
      /* constant */
      SetDefaultPattern((Pattern)_simpleCmd->patternCmd.cmd.num);
    } else {
      _offsets->patternOffset = offset = WordBoundary(offset,sizeof(Pattern));
      attrList->InsertAttr(4, "pattern",offset,sizeof(double),attrType,
			   false,  NULL, false, isSorted);
      offset += sizeof(Pattern);
    }
  }

  if (_cmdFlag & MappingCmd_Shape) {
    if (!ConvertSimpleCmd(_cmd->shapeCmd, _simpleCmd->shapeCmd, attrType,
			  isSorted))
      goto complexCmd;
    if (_simpleCmd->shapeCmd.cmdType == MappingSimpleCmdEntry::ConstCmd) {
      /* constant */
      ShapeID shape = (ShapeID)_simpleCmd->shapeCmd.cmd.num;
      if (shape < 0 || shape >= MaxInterpShapes)
	shape = 0;
      SetDefaultShape(shape);
    } else {
      _offsets->shapeOffset = offset = WordBoundary(offset,sizeof(ShapeID));
      attrList->InsertAttr(5, "shape",offset,sizeof(double),attrType,
			   false,  NULL, false, isSorted);
      offset += sizeof(ShapeID);
    }
  }

  if (_cmdFlag & MappingCmd_Orientation) {
    if (!ConvertSimpleCmd(_cmd->orientationCmd,
			  _simpleCmd->orientationCmd, attrType, isSorted))
      goto complexCmd;
    if (_simpleCmd->orientationCmd.cmdType == 
	MappingSimpleCmdEntry::ConstCmd) {
      /* constant */
      SetDefaultOrientation((Coord)_simpleCmd->orientationCmd.cmd.num);
    } else {
      _offsets->orientationOffset = offset = WordBoundary(offset,
							  sizeof(double));
      attrList->InsertAttr(6, "orientation",offset,sizeof(double),attrType,
			   false,  NULL, false, isSorted);
      offset += sizeof(double);
    }
  }

  _maxGDataShapeAttrNum = -1;
  for(j = 0; j < MAX_GDATA_ATTRS; j++) {
    if (_cmdAttrFlag & (1 << j)) {
      _maxGDataShapeAttrNum = j;
      if (!ConvertSimpleCmd(_cmd->shapeAttrCmd[j],
			    _simpleCmd->shapeAttrCmd[j], attrType, isSorted))
	goto complexCmd;
      if (_simpleCmd->shapeAttrCmd[j].cmdType ==
	  MappingSimpleCmdEntry::ConstCmd) {
	/* constant */
	SetDefaultShapeAttr(j, (Coord)_simpleCmd->shapeAttrCmd[j].cmd.num);
	if (j == 0)
	  pixelWidth = (Coord)_simpleCmd->shapeAttrCmd[j].cmd.num;
	else if (j == 1)
	  pixelHeight = (Coord)_simpleCmd->shapeAttrCmd[j].cmd.num;
      } else {
	char attrName [80];
	sprintf(attrName, "shapeAttr_%d", j);
	_offsets->shapeAttrOffset[j] = offset = WordBoundary(offset,
							     sizeof(double));
	attrList->InsertAttr(7 + j, attrName, offset, sizeof(double),
			     attrType, false,  NULL, false, isSorted);
	offset += sizeof(double);
      }
    }
  }

  UpdateBoundingBox(pixelWidth, pixelHeight);

#ifdef DEBUG
  printf("Command is %s\n", name);
  PrintCmd();
  attrList->Print();
  printf("offsets x: %d, y %d, color %d, size %d, shape %d, pattern %d, orientation %d\n", 
	 _offsets->xOffset, _offsets->yOffset, _offsets->colorOffset, 
	 _offsets->sizeOffset, _offsets->shapeOffset,
	 _offsets->patternOffset, _offsets->orientationOffset);
  printf("attr offsets:");
  for(i = 0; i < MAX_GDATA_ATTRS; i++)
    printf(" %d", _offsets->shapeAttrOffset[i]);
  printf("\n");
#endif

  return attrList;
  
 complexCmd:
  /* Init offsets to GData attributes */
  _offsets->xOffset= _offsets->yOffset = _offsets->colorOffset = -1;
  _offsets->sizeOffset = _offsets->shapeOffset = -1;
  _offsets->patternOffset = _offsets->orientationOffset = -1;
  for(i = 0; i < MAX_GDATA_ATTRS; i++)
    _offsets->shapeAttrOffset[i] = -1;

  _isSimpleCmd = false;

  if (attrList != NULL)
    delete attrList;
  attrList = new AttrList(name);

  _tdataFlag->ClearBitmap();
  _maxTDataAttrNum = 0;
  offset = 0;
  double constVal;

  if (_cmdFlag & MappingCmd_X) {
    if (IsConstCmd(_cmd->xCmd, constVal)) {
      SetDefaultX((Coord)constVal);
      _tclCmd->xCmd = NULL;
    } else {
      _tclCmd->xCmd = ConvertCmd(_cmd->xCmd, attrType, isSorted);
      _offsets->xOffset = offset = WordBoundary(offset,sizeof(double));
      attrList->InsertAttr(0, "x", offset, sizeof(double), attrType,
			   false, NULL, false, isSorted);
      offset += sizeof(double);
    }
  }

  if (_cmdFlag & MappingCmd_Y) {
    if (IsConstCmd(_cmd->yCmd, constVal)) {
      SetDefaultY((Coord)constVal);
      _tclCmd->yCmd = NULL;
    } else {
      _tclCmd->yCmd = ConvertCmd(_cmd->yCmd, attrType, isSorted);
      _offsets->yOffset = offset = WordBoundary(offset,sizeof(double));
      attrList->InsertAttr(1, "y", offset, sizeof(double), attrType,
			   false,  NULL, false, isSorted);
      offset += sizeof(double);
    }
  }

  if (_cmdFlag & MappingCmd_Color) {
    if (IsConstCmd(_cmd->colorCmd, constVal)) {
      SetDefaultColor((Color)constVal);
      _tclCmd->colorCmd = NULL;
    } else {
      _tclCmd->colorCmd = ConvertCmd(_cmd->colorCmd, attrType, isSorted);
      _offsets->colorOffset = offset = WordBoundary(offset,sizeof(Color));
      attrList->InsertAttr(2, "color",offset,sizeof(double),attrType,
			   false,  NULL, false, isSorted);
      offset += sizeof(Color);
    }
  }

  if (_cmdFlag & MappingCmd_Size) {
    if (IsConstCmd(_cmd->sizeCmd, constVal)) {
      SetDefaultSize(constVal);
      _tclCmd->sizeCmd = NULL;
    } else {
      _tclCmd->sizeCmd = ConvertCmd(_cmd->sizeCmd, attrType, isSorted);
      _offsets->sizeOffset = offset = WordBoundary(offset,sizeof(double));
      attrList->InsertAttr(3, "size",offset,sizeof(double),attrType,
			   false,  NULL, false, isSorted);
      offset += sizeof(double);
    }
  }

  if (_cmdFlag & MappingCmd_Pattern) {
    if (IsConstCmd(_cmd->patternCmd, constVal)) {
      SetDefaultPattern((Pattern)constVal);
      _tclCmd->patternCmd = NULL;
    } else {
      _tclCmd->patternCmd = 
	ConvertCmd(_cmd->patternCmd, attrType, isSorted);
      _offsets->patternOffset = offset = WordBoundary(offset,sizeof(Pattern));
      attrList->InsertAttr(4, "pattern",offset,sizeof(double),attrType,
			   false,  NULL, false, isSorted);
      offset += sizeof(Pattern);
    }
  }

  if (_cmdFlag & MappingCmd_Shape) {
    if (IsConstCmd(_cmd->shapeCmd, constVal)) {
      ShapeID shape = (ShapeID)constVal;
      if (shape < 0 || shape >= MaxInterpShapes)
	shape = 0;
      SetDefaultShape(shape);
      _tclCmd->shapeCmd = NULL;
    } else {
      _tclCmd->shapeCmd = ConvertCmd(_cmd->shapeCmd, attrType, isSorted);
      _offsets->shapeOffset = offset = WordBoundary(offset,sizeof(ShapeID));
      attrList->InsertAttr(5, "shape",offset,sizeof(double),attrType,
			   false,  NULL, false, isSorted);
      offset += sizeof(ShapeID);
    }
  }

  if (_cmdFlag & MappingCmd_Orientation) {
    if (IsConstCmd(_cmd->orientationCmd, constVal)) {
      SetDefaultOrientation(constVal);
      _tclCmd->orientationCmd = NULL;
    } else {
      _tclCmd->orientationCmd = ConvertCmd(_cmd->orientationCmd, attrType, 
					   isSorted);
      _offsets->shapeOffset = offset = WordBoundary(offset,sizeof(double));
      attrList->InsertAttr(6, "orientation",offset,sizeof(double),attrType,
			   false,  NULL, false, isSorted);
      offset += sizeof(double);
    }
  }

  _maxGDataShapeAttrNum = -1;
  for(j = 0; j < MAX_GDATA_ATTRS; j++) {
    char attrName [80];
    sprintf(attrName, "shapeAttr_%d", j);
    if (_cmdAttrFlag & (1 << j)) {
      _maxGDataShapeAttrNum = j;
      if (IsConstCmd(_cmd->shapeAttrCmd[j],constVal)) {
	SetDefaultShapeAttr(j,constVal);
	_tclCmd->shapeAttrCmd[j] = NULL;
      } else {
	_tclCmd->shapeAttrCmd[j] = 
	  ConvertCmd(_cmd->shapeAttrCmd[j],attrType, isSorted);
	_offsets->shapeAttrOffset[j] = offset = WordBoundary(offset,
							     sizeof(double));
	attrList->InsertAttr(j + 7, attrName, offset, sizeof(double),
			     attrType, false,  NULL, false, isSorted);
	offset += sizeof(double);
      }
    }
  }

#ifdef DEBUG
  printf("Command is %s\n", name);
  PrintCmd();
  attrList->Print();
  printf("offsets x: %d, y %d, color %d, size %d, shape %d, pattern %d, orientation %d\n", 
	 _offsets->xOffset, _offsets->yOffset, _offsets->colorOffset, 
	 _offsets->sizeOffset, _offsets->shapeOffset,
	 _offsets->patternOffset, _offsets->orientationOffset);
  printf("attr offsets: ");
  for(i = 0; i < MAX_GDATA_ATTRS; i++)
    printf(" %d", _offsets->shapeAttrOffset[i]);
  printf("\n");
#endif
  
  return attrList;
}

/* string manipulation commands */
const int MAX_STRING = 1024;
static char _stringBuf[MAX_STRING];
static int _numChar = 0;

static void InitString()
{
  _numChar = 0;
  _stringBuf[0] = '\0';
}

static void InsertChar(char c)
{
  if (_numChar +1 >= MAX_STRING) {
    fprintf(stderr, "Insertchar: no more space\n");
    Exit::DoExit(2);
  }
  _stringBuf[_numChar++] = c;
  _stringBuf[_numChar] = '\0';
}

static void InsertString(char *string)
{
  int length = strlen(string);
  if (_numChar+length >= MAX_STRING) {
    fprintf(stderr, "InsertString: no more space\n");
    Exit::DoExit(2);
  }
  char *ptr = &_stringBuf[_numChar];
  while (*string != '\0') {
    *ptr++ = *string++;
  }
  _numChar += length;
  _stringBuf[_numChar] = '\0';
}

static char *GetString()
{
  return _stringBuf;
}

/* Return TRUE if cmd is a simple command, and set entry to the
   converted cmmand entry. A simple command
   is either a constant, or a tdata attribute */

Boolean MappingInterp::ConvertSimpleCmd(char *cmd, 
					MappingSimpleCmdEntry &entry,
					AttrType &type, Boolean &isSorted)
{
  /* do not use simple interpreter? */
  if (!Init::UseSimpleInterpreter())
    return false;

#ifdef DEBUG
  printf("ConvertSimpleCmd: '%s'\n",cmd);
#endif

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

    if ((info = _attrList->Find(cmd+1)) != NULL) {
      entry.cmdType = MappingSimpleCmdEntry::AttrCmd;
      entry.cmd.attr = info;
      type = info->type;
      isSorted = info->isSorted;
      return true;
    }

#ifdef DEBUG
    printf("Undefined variable name: %s\n", cmd + 1);
    printf("Attribute list:\n");
    _attrList->Print();
#endif
    return false;
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

char *MappingInterp::ConvertCmd(char *cmd, AttrType &attrType,
				Boolean &isSorted)
{
  InitString();

  attrType = DoubleAttr;
  isSorted = false;
  
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
	InsertChar(*cmd);
      }
      else {
	/* from cmd+1 to ptr-1 is a variable name */
	char buf[80];
	int len = ptr - 1 - (cmd + 1) + 1;
	if (len >= (int)(sizeof buf)) {
	  fprintf(stderr, "can not handle variable name > %d\n",
		  (int)(sizeof buf));
	  Exit::DoExit(2);
	}
	memcpy(buf, cmd + 1, len);
	buf[len] = 0;
	
	AttrInfo *info = _attrList->Find(buf);
	if (info == NULL) {
	  /* can't find variable name */
	  InsertChar('$');
	  InsertString(buf);
	  /*
	     InsertString("1.0");
	  */

	} else {

	  /* found the attribute */
	  if (info->isSorted)
	    isSorted = true;
	  if (info->type == DateAttr)
	    attrType = DateAttr;
	  
	  if (info->attrNum > _maxTDataAttrNum)
	    _maxTDataAttrNum = info->attrNum;
	  _tdataFlag->SetBit(info->attrNum);
	  
	  if (info->type == StringAttr)
	    sprintf(buf, "$interpStrAttr_%d",info->attrNum);
	  else
	    sprintf(buf, "$interpAttr_%d",info->attrNum);
	  InsertString(buf);
	}
	cmd = ptr-1;
      }
    } else
      InsertChar(*cmd);
    
    cmd++;
  }

  return CopyString(GetString());
}

void MappingInterp::PrintSimpleCmdEntry(MappingSimpleCmdEntry *entry)
{
  switch(entry->cmdType) {
  case MappingSimpleCmdEntry::AttrCmd:
    printf("\"%s\"", entry->cmd.attr->name);
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

    for(int j = 0; j < MAX_GDATA_ATTRS; j++) {
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

    for(int j = 0; j < MAX_GDATA_ATTRS; j++) {
      if (_cmdAttrFlag & (1 << j)) {
	printf("shapeAttr_%d: %s --> %s", j, _cmd->shapeAttrCmd[j],
	       _tclCmd->shapeAttrCmd[j]);
	printf("\n");
      }
    }
  }
}

/*
   Return the value to be mapped using from as the pointer to record,
   entry as entry describing mapping, and defaultVal as the default value.
*/

inline double ConvertOne(char *from, MappingSimpleCmdEntry *entry, 
			 double defaultVal)
{
  AttrInfo *info;
  int offset;
  char *ptr;

  switch(entry->cmdType) {
  case MappingSimpleCmdEntry::AttrCmd:
    /*
       printf("returning attr\n");
    */
    info = entry->cmd.attr;
    offset = info->offset;
    ptr = from + offset;

    switch(info->type) {
    case IntAttr:
      return (double)(*((int *)ptr));
      break;

    case FloatAttr:
      return (double)(*((float *)ptr));
      break;

    case DoubleAttr:
      return *((double *)ptr);
      break;

    case StringAttr:
      return defaultVal;
      break;

    case DateAttr:
      return (double)(*((time_t *)ptr));
      break;
    }
    break;

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

  fprintf(stderr, "Unknown MappingSimpleCmdEntry\n");
  Exit::DoExit(1);

  // keep compiler happy
  return 0;
}

void MappingInterp::ConvertToGDataSimple(RecId startRecId, void *buf,
					 void **tRecs, int numRecs,
					 void *gdataPtr)
{
#ifdef DEBUG
  printf("ConvertToGdataSimple\n");
#endif

  int tRecSize = TDataRecordSize();
  int gRecSize = GDataRecordSize();
  char *tPtr = (char *)buf;
  char *gPtr = (char *)gdataPtr;
  _tclRecId = startRecId;

  for(int i = 0; i < numRecs; i++) {
    double *dPtr;

    if (_offsets->xOffset >= 0) {
      dPtr = (double *)(gPtr + _offsets->xOffset);
      *dPtr = ConvertOne(tPtr, &_simpleCmd->xCmd, 1.0);
    }
    if ( _offsets->yOffset >= 0) {
      dPtr = (double *)(gPtr + _offsets->yOffset);
      *dPtr = ConvertOne(tPtr, &_simpleCmd->yCmd, 1.0);
    }
    if (_offsets->colorOffset >= 0) {
      Color *cPtr = (Color *)(gPtr + _offsets->colorOffset);
      *cPtr= (Color)ConvertOne(tPtr, &_simpleCmd->colorCmd, 1.0);
    }
    if (_offsets->sizeOffset >= 0) {
      dPtr = (double *)(gPtr + _offsets->sizeOffset);
      *dPtr = ConvertOne(tPtr, &_simpleCmd->sizeCmd, 1.0);
    }
    if (_offsets->shapeOffset >= 0) {
      ShapeID *sPtr = (ShapeID *)(gPtr + _offsets->shapeOffset);
      *sPtr = (ShapeID) ConvertOne(tPtr, &_simpleCmd->shapeCmd, 1.0);
    }
    if (_offsets->patternOffset >= 0) {
      Pattern *pPtr = (Pattern *)(gPtr + _offsets->patternOffset);
      *pPtr = (Pattern)ConvertOne(tPtr, &_simpleCmd->patternCmd, 0.0);
    }
    if (_offsets->orientationOffset >= 0) {
      dPtr = (double *)(gPtr + _offsets->orientationOffset);
      *dPtr = ConvertOne(tPtr, &_simpleCmd->orientationCmd, 0.0);
    }

    for(int j = 0; j <= _maxGDataShapeAttrNum; j++) {
      if (_offsets->shapeAttrOffset[j] >= 0) {
	double *dPtr = (double *)(gPtr + _offsets->shapeAttrOffset[j]);
	*dPtr =  ConvertOne(tPtr, &_simpleCmd->shapeAttrCmd[j], 0.1);
#ifdef DEBUG
	printf("ConvertGData: shape attribute %d: %.2f\n", j, *dPtr);
#endif
      }
    }

    tPtr += tRecSize;
    gPtr += gRecSize;
    _tclRecId++;
  }
}

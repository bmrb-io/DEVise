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
  Revision 1.7  1996/04/14 00:19:11  jussi
  Removed the extraneous data structures and methods for recording
  new mapping class names. ClassDir records this information for
  us already! Added debugging statements. Changed interface to
  CreateWithParams() when a new mapping class is created, to reflect
  the new syntax of createMappingClass which will eventually replace
  createInterp.

  Revision 1.6  1996/01/13 23:09:33  jussi
  Added support for Z attribute and shape attribute 2.

  Revision 1.5  1995/12/28 19:37:35  jussi
  Small fix to remove compiler warning.

  Revision 1.4  1995/12/14 21:17:43  jussi
  Replaced 0x%x with 0x%p.

  Revision 1.3  1995/11/21 23:27:10  jussi
  Added copyright notice and cleaned up the code. Fixed bug
  in ParamNames where shapeAttr[1] got a bogus value "(null)"
  when in fact shapeAttr[0] had been defined.

  Revision 1.2  1995/09/05 22:14:59  jussi
  Added CVS header.
*/

#include <ctype.h>

#include "DeviseTypes.h"
#include "Control.h"
#include "MappingInterp.h"
#include "MapInterpClassInfo.h"
#include "Util.h"

//#define DEBUG

static char *rootClassName = "interpreted";
static char buf[14][256];
static char *args[14];

MapInterpClassInfo::MapInterpClassInfo()
{
#ifdef DEBUG
  printf("Creating root/prototype mapping class\n");
#endif

  _isInterp = true;
  _cmd = NULL;
  _fileAlias = NULL;
  _className = rootClassName;
  _name = NULL;
  _map = NULL;
  _dimensionInfo = NULL;
}

MapInterpClassInfo::MapInterpClassInfo(char *className)
{
#ifdef DEBUG
  printf("Creating new mapping class %s\n", className);
#endif

  _isInterp = true;
  _cmd = NULL;
  _fileAlias = NULL;
  _className = CopyString(className);
  _name = NULL;
  _map = NULL;
  _dimensionInfo = NULL;
}

MapInterpClassInfo::MapInterpClassInfo(char *className,
				       char *fileAlias, char *name, 
				       VisualFlag *dimensionInfo,
				       int numDimensions, MappingInterp *map, 
				       TData *tdata, MappingInterpCmd *cmd,
				       int cmdFlag, int attrFlag)
{
#ifdef DEBUG
  printf("Creating new mapping instance, alias %s, name %s\n",
	 fileAlias, name);
#endif

  _isInterp = true;
  _className = className;
  _fileAlias = fileAlias;
  _name = name;
  _dimensionInfo = dimensionInfo;
  _numDimensions = numDimensions;
  _map = map;
  _tdata = tdata;
  _cmd = cmd;
  _cmdFlag = cmdFlag;
  _attrFlag = attrFlag;
}

MapInterpClassInfo::~MapInterpClassInfo()
{
  delete _map;
  delete _dimensionInfo;
}

/* Return true if string is not empty. A string is not empty
   if it contains characters other than '\n', '\t', and '\r' */

static Boolean NotEmpty(char *str)
{
  while (*str != '\0') {
    if (!isspace(*str))
      return true;
    str++;
  }
  return false;
}

void MapInterpClassInfo::ExtractCommand(int argc, char **argv, 
					MappingInterpCmd *cmd,
					int &cmdFlag, int &attrFlag, 
					VisualFlag *dimensionInfo,
					int &numDimensions,
					char *&tdataAlias, TData *&tdata,
					char *&name)
{
  // we need to support both the old and the new style until
  // sufficient time has passed to make the old style really
  // obsolete

  // the old style has: x, y, color, size, pattern, orientation, shape,
  // and shape attributes 0 and 1

  // the new style has: x, y, z, color, size, pattern, orientation, shape,
  // and shape attributes 0, 1, and 2

  if (argc != 12 && argc != 14) {
    fprintf(stderr, "MapInterpClassInfo::ExtractCommand: argc != 14\n");
    Exit::DoExit(2);
  }

  cmd->xCmd = cmd->yCmd = cmd->zCmd = cmd->colorCmd = cmd->sizeCmd =
    cmd->patternCmd = cmd->shapeCmd = cmd->orientationCmd = NULL;
  for(int i = 0; i < MAX_GDATA_ATTRS; i++)
    cmd->shapeAttrCmd[i] = NULL;
  
  tdataAlias = CopyString(argv[0]);
  if ((tdata = (TData *)ControlPanel::FindInstance(tdataAlias)) == NULL) {
    fprintf(stderr, 
	    "MapInterpClassInfo::CreateWithParams: can't find tdata\n");
    Exit::DoExit(2);
  }
  
  name = CopyString(argv[1]);
  
  numDimensions = 0;
  if (argv[2][0] == 'X' || argv[2][0] == 'x') {
    numDimensions = 1;
    dimensionInfo[0] = VISUAL_X;
  }
  
  cmdFlag = 0;
  attrFlag = 0;

  if (NotEmpty(argv[3])) {
    cmd->xCmd = CopyString(argv[3]);
    cmdFlag |= MappingCmd_X;
  }
  if (NotEmpty(argv[4])) {
    cmd->yCmd = CopyString(argv[4]);
    cmdFlag |= MappingCmd_Y;
  }

  int shift = (argc == 14 ? 1 : 0); 

  if (NotEmpty(argv[5 + shift])) {
    cmd->colorCmd = CopyString(argv[5 + shift]);
    cmdFlag |= MappingCmd_Color;
  }
  if (NotEmpty(argv[6 + shift])) {
    cmd->sizeCmd = CopyString(argv[6 + shift]);
    cmdFlag |= MappingCmd_Size;
  }
  if (NotEmpty(argv[7 + shift])) {
    cmd->patternCmd = CopyString(argv[7 + shift]);
    cmdFlag |= MappingCmd_Pattern;
  }
  if (NotEmpty(argv[8 + shift])) {
    cmd->orientationCmd = CopyString(argv[8 + shift]);
    cmdFlag |= MappingCmd_Orientation;
  }
  if (NotEmpty(argv[9 + shift])) {
    cmd->shapeCmd = CopyString(argv[9 + shift]);
    cmdFlag |= MappingCmd_Shape;
  }
  if (NotEmpty(argv[10 + shift])) {
    cmd->shapeAttrCmd[0] = CopyString(argv[10 + shift]);
    attrFlag |= 1;
  }
  if (NotEmpty(argv[11 + shift])) {
    cmd->shapeAttrCmd[1] = CopyString(argv[11 + shift]);
    attrFlag |= 2;
  }

  if (argc == 14) {
    if (NotEmpty(argv[5])) {
      cmd->zCmd = CopyString(argv[5]);
      cmdFlag |= MappingCmd_Z;
    }
    if (NotEmpty(argv[13])) {
      cmd->shapeAttrCmd[2] = CopyString(argv[13]);
      attrFlag |= 4;
    }
  }
}

void MapInterpClassInfo::ParamNames(int &argc, char **&argv)
{
  if (!_cmd) {
    /* no params needed for creating a new mapping class*/
    argc = 0;
    argv = args;
    return;
  }

  /* params for an existing interpreted mapping */

  argc = 14;
  argv = args;

  if (_fileAlias == NULL) {
    sprintf(buf[0], "File_Alias {%s}", ControlPanel::Instance()->FileAlias());
  } else {
    sprintf(buf[0], "File_Alias {%s}", _fileAlias);
  }
    
  if (_name == NULL) {
    sprintf(buf[1], "Map_Name {%s}", ControlPanel::Instance()->GDataName());
  } else {
    sprintf(buf[1], "GData_Name {%s}", _name);
  }

  args[0] = buf[0];
  args[1] = buf[1];

  if (_numDimensions == 1) {
    args[2] = "Sorted X";
  } else
    args[2] = "Sorted";
  
  if (_cmd->xCmd != NULL) {
    sprintf(buf[3], "X {%s}", _cmd->xCmd);
    args[3] = buf[3];
  } else
    args[3] = "X";
    
  if (_cmd->yCmd != NULL) {
    sprintf(buf[4], "Y {%s}", _cmd->yCmd);
    args[4] = buf[4];
  } else
    args[4] = "Y";
    
  if (_cmd->zCmd != NULL) {
    sprintf(buf[5], "Z {%s}", _cmd->zCmd);
    args[5] = buf[5];
  } else
    args[5] = "Z";
    
  if (_cmd->colorCmd != NULL) {
    sprintf(buf[6], "Color {%s}", _cmd->colorCmd);
    args[6] = buf[6];
  } else
      args[6] = "Color";
    
  if ( _cmd->sizeCmd != NULL) {
    sprintf(buf[7], "Size {%s}", _cmd->sizeCmd);
    args[7] = buf[7];
  } else
    args[7] = "Size";
    
  if ( _cmd->patternCmd != NULL) {
    sprintf(buf[8], "Pattern {%s}", _cmd->patternCmd);
    args[8] = buf[8];
  } else
    args[8] = "Pattern";
  
  if ( _cmd->orientationCmd != NULL) {
    sprintf(buf[9], "Orientation {%s}", _cmd->orientationCmd);
    args[9] = buf[9];
  } else
    args[9] = "Orientation";
    
  if ( _cmd->shapeCmd!= NULL) {
    sprintf(buf[10], "Shape {%s}", _cmd->shapeCmd);
    args[10] = buf[10];
  } else
    args[10] = "Shape";
    
  if ( _cmd->shapeAttrCmd[0] != NULL) {
    sprintf(buf[11], "ShapeAttr0 {%s}", _cmd->shapeAttrCmd[0]);
    args[11] = buf[11];
  } else
    args[11] = "ShapeAttr0";
    
  if (_cmd->shapeAttrCmd[1] != NULL) {
    sprintf(buf[12], "ShapeAttr1 {%s}", _cmd->shapeAttrCmd[1]);
    args[12] = buf[12];
  } else
    args[12] = "ShapeAttr1";
    
  if (_cmd->shapeAttrCmd[2] != NULL) {
    sprintf(buf[13], "ShapeAttr2 {%s}", _cmd->shapeAttrCmd[2]);
    args[13] = buf[13];
  } else
    args[13] = "ShapeAttr2";
}

void MapInterpClassInfo::ChangeParams(int argc, char**argv)
{
  if (!_cmd)
    return;

  char *tdataAlias;
  char *name;
  TData *tdata;
  ExtractCommand(argc, argv, _cmd, _cmdFlag, _attrFlag,
		 _dimensionInfo, _numDimensions, tdataAlias, tdata, name);
  _map->ChangeCmd(_cmd, _cmdFlag, _attrFlag, _dimensionInfo, _numDimensions);
}


ClassInfo *MapInterpClassInfo::CreateWithParams(int argc, char **argv)
{
  // the new form uses just the class name parameter when a new class
  // is to be created, or a full list of parameters when an instance
  // is to be created

  if (argc == 1) {
#ifdef DEBUG
    printf("Creating new interpreted mapping class\n");
#endif
    return new MapInterpClassInfo(argv[0]);
  }

  // the old form has all the parameters even when creating a new class,
  // so we need to do the string comparison to figure out if a new
  // class or instance is requested; support for the old form may be
  // removed at some later date

  if (argc >= 1 && !strcmp(_className, rootClassName)) {
#ifdef DEBUG
    printf("Creating new (old-style) interpreted mapping class\n");
#endif
    return new MapInterpClassInfo(argv[1]);
  }

#ifdef DEBUG
  printf("Creating new instance of interpreted mapping class\n");
#endif
    
  MappingInterpCmd *cmd = new MappingInterpCmd;
  TData *tdata;
  int cmdFlag, attrFlag;
  VisualFlag *dimensionInfo = new VisualFlag;
  int numDimensions;
  char *tdataAlias, *name;

  ExtractCommand(argc, argv, cmd, cmdFlag, attrFlag,
		 dimensionInfo, numDimensions, tdataAlias, tdata, name);
  
  MappingInterp *map = new MappingInterp(name, tdata, cmd,
					 cmdFlag, attrFlag, dimensionInfo,
					 numDimensions);
  
  return new MapInterpClassInfo(_className, tdataAlias, name, dimensionInfo,
				numDimensions, map, tdata, cmd, cmdFlag,
				attrFlag);
}

/* Set default parameters */
void MapInterpClassInfo::SetDefaultParams(int argc, char **argv)
{
}

/* Get default parameters */
void MapInterpClassInfo::GetDefaultParams(int &argc, char **&argv)
{
  argc = 0;
}

char *MapInterpClassInfo::InstanceName()
{
  return _name;
}

void *MapInterpClassInfo::GetInstance()
{
  return _map;
}

void MapInterpClassInfo::CreateParams(int &argc, char **&argv)
{
  /* parameters are: fileAlias mapName  x, y, color, size, 
     pattern, orientation, shape, and shapeAttr */
  argc = 14;
  argv = args;
  
  args[0] = _fileAlias;
  if (_name == NULL) {
    /* info about creating an interpreted mapping class */
    args[1] = _className;
  } else
    /* creating a mapping instance */
    args[1] = _name;

  if (_numDimensions == 1) {
    args[2] = "X";
  } else
    args[2] = "";
  
  if (_cmd->xCmd != NULL) {
    args[3] = _cmd->xCmd;
  } else
    args[3] = "";
  
  if (_cmd->yCmd != NULL) {
    args[4] = _cmd->yCmd;
  } else
    args[4] = "";
  
  if (_cmd->zCmd != NULL) {
    args[5] = _cmd->zCmd;
  } else
    args[5] = "";
  
  if ( _cmd->colorCmd != NULL) {
    args[6] = _cmd->colorCmd;
  } else
    args[6] = "";
  
  if ( _cmd->sizeCmd != NULL) {
    args[7] = _cmd->sizeCmd;
  } else
    args[7] = "";
  
  if ( _cmd->patternCmd != NULL) {
    args[8] = _cmd->patternCmd;
  } else
    args[8] = NULL;
  
  if ( _cmd->orientationCmd != NULL) {
    args[9] = _cmd->orientationCmd;
  } else
    args[9] = "";
  
  if ( _cmd->shapeCmd!= NULL) {
    args[10] = _cmd->shapeCmd;
  } else
    args[10] = NULL;
  
  if ( _cmd->shapeAttrCmd[0] != NULL) {
    args[11] = _cmd->shapeAttrCmd[0];
  } else
    args[11] = "";
  
  if ( _cmd->shapeAttrCmd[1] != NULL) {
    args[12] = _cmd->shapeAttrCmd[1];
  } else
    args[12] = NULL;
  
  if ( _cmd->shapeAttrCmd[2] != NULL) {
    args[13] = _cmd->shapeAttrCmd[2];
  } else
    args[13] = NULL;
}

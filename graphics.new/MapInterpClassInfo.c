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
  Revision 1.17  1997/11/24 23:15:06  weaver
  Changes for the new ColorManager.

  Revision 1.16  1997/10/28 15:46:28  wenger
  Fixed bug 236.

  Revision 1.15  1997/10/03 14:37:11  wenger
  Various fixes to get session opening/saving to work with client/server
  version; reading old-style (Tcl) session files now works in back end;
  got back-end session file stuff working for multi.

  Revision 1.14  1997/09/23 19:57:40  wenger
  Opening and saving of sessions now working with new scheme of mapping
  automatically creating the appropriate TData.

  Revision 1.13  1997/04/16 18:53:34  wenger
  Text labels can now show non-string attributes; fixed a bug in constant
  strings in mapping (non-terminated string); added constant attributes to
  GData attribute list; commented out some debug code in mapping dialog.

  Revision 1.12  1996/09/27 21:09:37  wenger
  GDataBin class doesn't allocate space for connectors (no longer used
  anyhow); fixed some more memory leaks and made notes in the code about
  some others that I haven't fixed yet; fixed a few other minor problems
  in the code.

  Revision 1.11  1996/06/15 14:25:01  jussi
  Rewrote so that a variable number of shape attributes is
  allowed when mapping is created.

  Revision 1.10  1996/05/11 21:29:20  jussi
  Fixed problem with null valus being returned as CreateParams().

  Revision 1.9  1996/05/11 03:12:46  jussi
  Made this code independent of the ControlPanel variables
  like FileAlias and GDataName.

  Revision 1.8  1996/04/16 00:16:43  jussi
  Added initialization of _dimensionInfo to two constructors.

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
#include "Session.h"

//#define DEBUG

static char *rootClassName = "interpreted";
static char buf[11 + MAX_GDATA_ATTRS][64];
static char *args[11 + MAX_GDATA_ATTRS];

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
  delete _cmd;
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

DevStatus
MapInterpClassInfo::ExtractCommand(int argc, char **argv, 
					MappingInterpCmd *cmd,
					unsigned long int &cmdFlag,
					unsigned long int &attrFlag, 
					VisualFlag *dimensionInfo,
					int &numDimensions,
					char *&tdataAlias, TData *&tdata,
					char *&name)
{
  // we need to support both the old and the new style until
  // sufficient time has passed to make the old style really
  // obsolete; new style created January 13th, 1996

  // the old style has: x, y, color, size, pattern, orientation, shape,
  // and shape attributes 0 and 1

  // the new style has: x, y, z, color, size, pattern, orientation, shape,
  // and shape attributes 0 through n (n >= 2)

  if (argc < 12) {
    fprintf(stderr, "Invalid arguments in interpreted mapping\n");
    return StatusFailed;
  }

  if (argc > 11 + MAX_GDATA_ATTRS) {
    fprintf(stderr, "Ignoring extraneous shape attributes\n");
    argc = 11 + MAX_GDATA_ATTRS;
  }

  cmd->xCmd = cmd->yCmd = cmd->zCmd = cmd->colorCmd = cmd->sizeCmd = 0;
  cmd->patternCmd = cmd->shapeCmd = cmd->orientationCmd = 0;
  for(int i = 0; i < MAX_GDATA_ATTRS; i++)
    cmd->shapeAttrCmd[i] = 0;
  
  tdataAlias = CopyString(argv[0]);
  tdata = (TData *)ControlPanel::FindInstance(tdataAlias);
  // If we don't already have this TData, try to create it.
  if (!tdata) {
    Session::CreateTData(tdataAlias);
    tdata = (TData *)ControlPanel::FindInstance(tdataAlias);
  }
  if (!tdata) {
    fprintf(stderr, 
	    "MapInterpClassInfo::CreateWithParams: can't find tdata %s\n",
	    tdataAlias);
    return StatusFailed;
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

  // need to shift arguments if not old style (old style has no Z attribute)
  int shift = (argc > 12 ? 1 : 0); 

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

  // if new style, extract Z and remaining shape attributes
  if (argc > 12) {
    if (NotEmpty(argv[5])) {
      cmd->zCmd = CopyString(argv[5]);
      cmdFlag |= MappingCmd_Z;
    }
    // remainder of command line is shape attributes
    for(int i = 13; i < argc; i++) {
      if (NotEmpty(argv[i])) {
	cmd->shapeAttrCmd[i - 11] = CopyString(argv[i]);
	attrFlag |= 1 << (i - 11);
      }
    }
  }

  return StatusOk;
}

void MapInterpClassInfo::ParamNames(int &argc, char **&argv)
{
#ifdef DEBUG
  printf("MapInterpClassInfo::ParamNames\n");
#endif

  if (!_cmd) {
    /* no params needed for creating a new mapping class*/
    argc = 0;
    argv = args;
    return;
  }

  /* params for an existing interpreted mapping */

  argc = 11 + MAX_GDATA_ATTRS;
  argv = args;

  if (_fileAlias)
    sprintf(buf[0], "File_Alias {%s}", _fileAlias);
  else
    strcpy(buf[0], "File_Alias {foobar}");
  if (_name)
    sprintf(buf[1], "Map_Name {%s}", _name);
  else
    strcpy(buf[1], "Map_Name {foobar}");

  args[0] = buf[0];
  args[1] = buf[1];

  if (_numDimensions == 1) {
    args[2] = "Sorted X";
  } else
    args[2] = "Sorted";
  
  if (_cmd->xCmd) {
    sprintf(buf[3], "X {%s}", _cmd->xCmd);
    args[3] = buf[3];
  } else
    args[3] = "X";
    
  if (_cmd->yCmd) {
    sprintf(buf[4], "Y {%s}", _cmd->yCmd);
    args[4] = buf[4];
  } else
    args[4] = "Y";
    
  if (_cmd->zCmd) {
    sprintf(buf[5], "Z {%s}", _cmd->zCmd);
    args[5] = buf[5];
  } else
    args[5] = "Z";
    
  if (_cmd->colorCmd) {
    sprintf(buf[6], "Color {%s}", _cmd->colorCmd);
    args[6] = buf[6];
  } else
      args[6] = "Color";
    
  if ( _cmd->sizeCmd) {
    sprintf(buf[7], "Size {%s}", _cmd->sizeCmd);
    args[7] = buf[7];
  } else
    args[7] = "Size";
    
  if ( _cmd->patternCmd) {
    sprintf(buf[8], "Pattern {%s}", _cmd->patternCmd);
    args[8] = buf[8];
  } else
    args[8] = "Pattern";
  
  if ( _cmd->orientationCmd) {
    sprintf(buf[9], "Orientation {%s}", _cmd->orientationCmd);
    args[9] = buf[9];
  } else
    args[9] = "Orientation";
    
  if ( _cmd->shapeCmd) {
    sprintf(buf[10], "Shape {%s}", _cmd->shapeCmd);
    args[10] = buf[10];
  } else
    args[10] = "Shape";
    
  for(int i = 0; i < MAX_GDATA_ATTRS; i++) {
    if ( _cmd->shapeAttrCmd[i])
      sprintf(buf[11 + i], "ShapeAttr%d {%s}", i, _cmd->shapeAttrCmd[i]);
    else
      sprintf(buf[11 + i], "ShapeAttr%d", i);
    args[11 + i] = buf[11 + i];
  }
}

void MapInterpClassInfo::ChangeParams(int argc, char**argv)
{
  if (!_cmd)
    return;

  char *tdataAlias;
  char *name;
  TData *tdata;
  if (ExtractCommand(argc, argv, _cmd, _cmdFlag, _attrFlag,
		 _dimensionInfo, _numDimensions, tdataAlias, tdata,
		 name).IsComplete()) {
    _map->ChangeCmd(_cmd, _cmdFlag, _attrFlag, _dimensionInfo, _numDimensions);
  }
}


ClassInfo *MapInterpClassInfo::CreateWithParams(int argc, char **argv)
{
  // the new form uses just the class name parameter when a new class
  // is to be created, or a full list of parameters when an instance
  // is to be created

  if (argc == 1) {
#ifdef DEBUG
    printf("Creating new interpreted mapping class {%s}\n", argv[0]);
#endif
    return new MapInterpClassInfo(argv[0]);
  }

  // the old form has all the parameters even when creating a new class,
  // so we need to do the string comparison to figure out if a new
  // class or instance is requested; support for the old form may be
  // removed at some later date

  if (argc >= 1 && !strcmp(_className, rootClassName)) {
#ifdef DEBUG
    printf("Creating new (old-style) interpreted mapping class {%s}\n",
      argv[1]);
#endif
    return new MapInterpClassInfo(argv[1]);
  }

#ifdef DEBUG
  printf("Creating new instance of interpreted mapping class {%s}\n", argv[0]);
#endif
    
  MappingInterpCmd *cmd = new MappingInterpCmd;
  TData *tdata;
  unsigned long int cmdFlag;
  unsigned long int attrFlag;
  VisualFlag *dimensionInfo = new VisualFlag;
  int numDimensions;
  char *tdataAlias, *name;

  if (ExtractCommand(argc, argv, cmd, cmdFlag, attrFlag,
		 dimensionInfo, numDimensions, tdataAlias, tdata,
		 name).IsComplete()) {
    MappingInterp *map = new MappingInterp(name, tdata, cmd,
					   cmdFlag, attrFlag, dimensionInfo,
					   numDimensions);
  
    return new MapInterpClassInfo(_className, tdataAlias, name, dimensionInfo,
				  numDimensions, map, tdata, cmd, cmdFlag,
				  attrFlag);
  } else {
    delete cmd;
    return NULL;
  }
}

/* Set default parameters */
void MapInterpClassInfo::SetDefaultParams(int argc, char **argv)
{
}

/* Get default parameters */
void MapInterpClassInfo::GetDefaultParams(int &argc, char **&argv)
{
  argc = 0;
  argv = args;
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
#ifdef DEBUG
  printf("MapInterpClassInfo::CreateParams\n");
#endif

  DOASSERT(_name, "Invalid name of instance");

  /* parameters are: fileAlias, mapName, x, y, color, size, 
     pattern, orientation, shape, and shapeAttrs */
  argc = 11 + MAX_GDATA_ATTRS;
  argv = args;
  
  args[0] = _fileAlias;
  args[1] = _name;

  if (_numDimensions == 1)
    args[2] = "X";
  else
    args[2] = "";
  
  if (_cmd->xCmd)
    args[3] = _cmd->xCmd;
  else
    args[3] = "";
  
  if (_cmd->yCmd)
    args[4] = _cmd->yCmd;
  else
    args[4] = "";
  
  if (_cmd->zCmd)
    args[5] = _cmd->zCmd;
  else
    args[5] = "";
  
  if ( _cmd->colorCmd)
    args[6] = _cmd->colorCmd;
  else
    args[6] = "";
  
  if ( _cmd->sizeCmd)
    args[7] = _cmd->sizeCmd;
  else
    args[7] = "";
  
  if ( _cmd->patternCmd)
    args[8] = _cmd->patternCmd;
  else
    args[8] = "";
  
  if ( _cmd->orientationCmd)
    args[9] = _cmd->orientationCmd;
  else
    args[9] = "";
  
  if ( _cmd->shapeCmd)
    args[10] = _cmd->shapeCmd;
  else
    args[10] = "";
  
  for(int i = 0; i < MAX_GDATA_ATTRS; i++) {
    if ( _cmd->shapeAttrCmd[i])
      args[11 + i] = _cmd->shapeAttrCmd[i];
    else
      args[11 + i] = "";
  }
}

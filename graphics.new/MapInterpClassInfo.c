/*
  ========================================================================
  DEVise Data Visualization Software
  (c) Copyright 1992-2002
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
  Revision 1.30.10.1  2002/09/20 17:16:23  wenger
  Fixed memory leaks in MapInterpClassInfo.

  Revision 1.30  2001/02/20 20:02:53  wenger
  Merged changes from no_collab_br_0 thru no_collab_br_2 from the branch
  to the trunk.

  Revision 1.29.4.1  2001/02/16 21:37:59  wenger
  Updated DEVise version to 1.7.2; implemented 'forward' and 'back' (like
  a web browser) on 'sets' of visual filters.

  Revision 1.29  2000/03/14 17:05:30  wenger
  Fixed bug 569 (group/ungroup causes crash); added more memory checking,
  including new FreeString() function.

  Revision 1.28  2000/02/16 18:51:39  wenger
  Massive "const-ifying" of strings in ClassDir and its subclasses.

  Revision 1.27  1999/08/10 20:15:05  wenger
  Parent views can now control the titles of view symbols.

  Revision 1.26  1999/07/13 17:32:42  wenger
  Parent view can now control attribute(s) in child view's mapping;
  cleaned up some of the mapping-related code; better command logging.

  Revision 1.25  1999/06/22 18:30:55  wenger
  Visual filter values for view symbols can now be specified in the parent
  view's mapping.

  Revision 1.24  1999/05/21 14:52:28  wenger
  Cleaned up GData-related code in preparation for including bounding box
  info.

  Revision 1.23  1999/04/26 20:25:10  wenger
  Fixed bug 486.

  Revision 1.22  1999/04/16 21:03:55  wenger
  Fixed various bugs related to view symbols, including memory problem
  with MappingInterp dimensionInfo; updated create_condor_session script
  to take advantage of view symbol TData switching capability.

  Revision 1.21  1999/04/14 15:30:19  wenger
  Improved 'switch TData': moved the code from Tcl to C++, functionality
  is more flexible -- schemas don't have to match exactly as long as the
  appropriate TData attributes are present; TData can now be specified for
  view symbols in parent view mapping; updated shape help.

  Revision 1.20  1999/03/01 23:09:10  wenger
  Fixed a number of memory leaks and removed unused code.

  Revision 1.19  1998/11/20 18:38:46  wenger
  Fixed bug 440 (crash caused by empty mapping command for X in combination
  with other errors).

  Revision 1.18  1998/04/01 18:54:27  wenger
  Fixed bug 332.

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
#include "DeviseHistory.h"
#include "DevError.h"

//#define DEBUG

static char *rootClassName = "interpreted";
static const unsigned int bufLen = 256;
static char buf[11 + MAX_SHAPE_ATTRS][bufLen];
static const char *args[11 + MAX_SHAPE_ATTRS];

MapInterpClassInfo::MapInterpClassInfo()
{
#ifdef DEBUG
  printf("Creating root/prototype mapping class\n");
#endif

  _isInterp = true;
  _cmd = NULL;
  _fileAlias = NULL;
  _className = CopyString(rootClassName);
  _name = NULL;
  _map = NULL;
}

MapInterpClassInfo::MapInterpClassInfo(const char *className)
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
}

MapInterpClassInfo::MapInterpClassInfo(const char *className,
				       const char *fileAlias, const char *name, 
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
  _className = CopyString(className);
  _fileAlias = fileAlias;
  _name = name;
  _numDimensions = numDimensions;
  _map = map;
  _tdata = tdata;
  _cmd = cmd;
  _cmdFlag = cmdFlag;
  _attrFlag = attrFlag;
}

MapInterpClassInfo::~MapInterpClassInfo()
{
#if defined(DEBUG)
  printf("MapInterpClassInfo::~MapInterpClassInfo()\n");
#endif

  if (_map) {
    VisualFlag *dimensionInfo;
    (void)_map->DimensionInfo(dimensionInfo);
    delete dimensionInfo;
  }

  delete _map;

  if (_cmd != NULL) {
    // Note: we can't just do all this as part of a MappingInterpCmd
    // destructor, because we end up copying a bunch of the pointers
    // in MappingInterp.  RKW 2002-09-20.
    FreeString(_cmd->xCmd);
    FreeString(_cmd->yCmd);
    FreeString(_cmd->zCmd);
    FreeString(_cmd->colorCmd);
    FreeString(_cmd->sizeCmd);
    FreeString(_cmd->patternCmd);
    FreeString(_cmd->orientationCmd);
    FreeString(_cmd->shapeCmd);
    for (int index = 0; index < MAX_SHAPE_ATTRS; index++) {
      FreeString(_cmd->shapeAttrCmd[index]);
    }
  }

  delete _cmd;
  FreeString((char *)_fileAlias);
  FreeString((char *)_name);
  FreeString((char *)_className);
}

/* Return true if string is not empty. A string is not empty
   if it contains characters other than '\n', '\t', and '\r' */

static Boolean NotEmpty(const char *str)
{
  while (*str != '\0') {
    if (!isspace(*str))
      return true;
    str++;
  }
  return false;
}

DevStatus
MapInterpClassInfo::ExtractCommand(int argc, const char * const *argv, 
					MappingInterpCmd *cmd,
					unsigned long int &cmdFlag,
					unsigned long int &attrFlag, 
					VisualFlag *dimensionInfo,
					int &numDimensions,
					char *&tdataAlias, TData *&tdata,
					char *&name)
{
#if defined(DEBUG)
  printf("MapInterpClassInfo::ExtractCommand(");
  PrintArgs(stdout, argc, argv, false);
  printf(")\n");
#endif

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

  if (argc > 11 + MAX_SHAPE_ATTRS) {
    fprintf(stderr, "Ignoring extraneous shape attributes\n");
    argc = 11 + MAX_SHAPE_ATTRS;
  }

  cmd->xCmd = cmd->yCmd = cmd->zCmd = cmd->colorCmd = cmd->sizeCmd = 0;
  cmd->patternCmd = cmd->shapeCmd = cmd->orientationCmd = 0;
  for(int i = 0; i < MAX_SHAPE_ATTRS; i++) {
    cmd->shapeAttrCmd[i] = 0;
  }

  if (argv[0][0] == '.') {
    tdataAlias = CopyString(argv[0]);
  } else {
    // DTE goes nuts if first char of TData name isn't '.'.
    char namebuf[1024];
    sprintf(namebuf, ".%s", argv[0]);
    tdataAlias = CopyString(namebuf);
  }

  ClassInfo *info = ControlPanel::GetClassDir()->FindClassInfo(tdataAlias);
  if (info) {
    if (strcmp("tdata", info->CategoryName())) {
    char errBuf[1024];
      sprintf(errBuf,
          "Name conflict: a non-TData object named <%s> already exists",
	  tdataAlias);
      reportErrNosys(errBuf);
      return StatusFailed;
    } else {
      tdata = (TData *)info->GetInstance();
    }
  } else {
    tdata = NULL;
  }

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
  } else {
    // It's not safe to have a NULL command for X.
    cmd->xCmd = CopyString("0");
  }
  if (NotEmpty(argv[4])) {
    cmd->yCmd = CopyString(argv[4]);
    cmdFlag |= MappingCmd_Y;
  } else {
    // It's not safe to have a NULL command for Y.
    cmd->yCmd = CopyString("0");
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

void MapInterpClassInfo::ParamNames(int &argc, const char **&argv)
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

  argc = 11 + MAX_SHAPE_ATTRS;
  argv = args;

  if (_fileAlias) {
    sprintf(buf[0], "File_Alias {%s}", _fileAlias);
  } else {
    strcpy(buf[0], "File_Alias {foobar}");
  }
  DOASSERT(strlen(buf[0]) < bufLen, "param string too long");

  if (_name) {
    sprintf(buf[1], "Map_Name {%s}", _name);
  } else {
    strcpy(buf[1], "Map_Name {foobar}");
  }
  DOASSERT(strlen(buf[1]) < bufLen, "param string too long");

  args[0] = buf[0];
  args[1] = buf[1];

  if (_numDimensions == 1) {
    args[2] = "Sorted X";
  } else {
    args[2] = "Sorted";
  }
  
  if (_cmd->xCmd) {
    sprintf(buf[3], "X {%s}", _cmd->xCmd);
    DOASSERT(strlen(buf[3]) < bufLen, "param string too long");
    args[3] = buf[3];
  } else {
    args[3] = "X";
  }
    
  if (_cmd->yCmd) {
    sprintf(buf[4], "Y {%s}", _cmd->yCmd);
    DOASSERT(strlen(buf[4]) < bufLen, "param string too long");
    args[4] = buf[4];
  } else {
    args[4] = "Y";
  }
    
  if (_cmd->zCmd) {
    sprintf(buf[5], "Z {%s}", _cmd->zCmd);
    DOASSERT(strlen(buf[5]) < bufLen, "param string too long");
    args[5] = buf[5];
  } else {
    args[5] = "Z";
  }
    
  if (_cmd->colorCmd) {
    sprintf(buf[6], "Color {%s}", _cmd->colorCmd);
    DOASSERT(strlen(buf[6]) < bufLen, "param string too long");
    args[6] = buf[6];
  } else {
      args[6] = "Color";
  }
    
  if ( _cmd->sizeCmd) {
    sprintf(buf[7], "Size {%s}", _cmd->sizeCmd);
    DOASSERT(strlen(buf[7]) < bufLen, "param string too long");
    args[7] = buf[7];
  } else {
    args[7] = "Size";
  }
    
  if ( _cmd->patternCmd) {
    sprintf(buf[8], "Pattern {%s}", _cmd->patternCmd);
    DOASSERT(strlen(buf[8]) < bufLen, "param string too long");
    args[8] = buf[8];
  } else {
    args[8] = "Pattern";
  }
  
  if ( _cmd->orientationCmd) {
    sprintf(buf[9], "Orientation {%s}", _cmd->orientationCmd);
    DOASSERT(strlen(buf[9]) < bufLen, "param string too long");
    args[9] = buf[9];
  } else {
    args[9] = "Orientation";
  }
    
  if ( _cmd->shapeCmd) {
    sprintf(buf[10], "Shape {%s}", _cmd->shapeCmd);
    DOASSERT(strlen(buf[10]) < bufLen, "param string too long");
    args[10] = buf[10];
  } else {
    args[10] = "Shape";
  }
    
  for(int i = 0; i < MAX_SHAPE_ATTRS; i++) {
    if ( _cmd->shapeAttrCmd[i]) {
      sprintf(buf[11 + i], "ShapeAttr%d {%s}", i, _cmd->shapeAttrCmd[i]);
    } else {
      sprintf(buf[11 + i], "ShapeAttr%d", i);
    }
    DOASSERT(strlen(buf[11 + i]) < bufLen, "param string too long");
    args[11 + i] = buf[11 + i];
  }
}

void MapInterpClassInfo::ChangeParams(int argc, const char* const *argv)
{
#if defined(DEBUG)
  printf("MapInterpClassInfo(%s)::ChangeParams(", InstanceName());
  PrintArgs(stdout, argc, argv, false);
  printf(")\n");
#endif

  if (!_cmd)
    return;

  char *tdataAlias;
  char *name;
  TData *tdata;
  VisualFlag *dimensionInfo = new VisualFlag;
  if (ExtractCommand(argc, argv, _cmd, _cmdFlag, _attrFlag,
		 dimensionInfo, _numDimensions, tdataAlias, tdata,
		 name).IsComplete()) {
    _map->ChangeCmd(_cmd, _cmdFlag, _attrFlag, dimensionInfo, _numDimensions);
    DeviseHistory::GetDefaultHistory()->ClearAll();
    FreeString(name);
    FreeString(tdataAlias);
  }
}


ClassInfo *MapInterpClassInfo::CreateWithParams(int argc,
    const char * const *argv)
{
  // the new form uses just the class name parameter when a new class
  // is to be created, or a full list of parameters when an instance
  // is to be created

#if defined(DEBUG)
  printf("MapInterpClassInfo::CreateWithParams(");
  PrintArgs(stdout, argc, argv, false);
  printf(")\n");
#endif

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
void MapInterpClassInfo::SetDefaultParams(int argc, const char * const *argv)
{
}

/* Get default parameters */
void MapInterpClassInfo::GetDefaultParams(int &argc, const char **&argv)
{
  argc = 0;
  argv = args;
}

const char *MapInterpClassInfo::InstanceName()
{
  return _name;
}

void *MapInterpClassInfo::GetInstance()
{
  return _map;
}

void MapInterpClassInfo::CreateParams(int &argc, const char **&argv)
{
#ifdef DEBUG
  printf("MapInterpClassInfo::CreateParams\n");
#endif

  DOASSERT(_name, "Invalid name of instance");

  /* parameters are: fileAlias, mapName, x, y, color, size, 
     pattern, orientation, shape, and shapeAttrs */
  argc = 11 + MAX_SHAPE_ATTRS;
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
  
  for(int i = 0; i < MAX_SHAPE_ATTRS; i++) {
    if ( _cmd->shapeAttrCmd[i])
      args[11 + i] = _cmd->shapeAttrCmd[i];
    else
      args[11 + i] = "";
  }
}

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
  Module for reading physical and logical schemas.
 */

/*
  $Id$

  $Log$
  Revision 1.32  1996/11/01 19:28:20  kmurli
  Added DQL sources to include access to TDataDQL. This is equivalent to
  TDataAscii/TDataBinary. The DQL type in the Tcl/Tk corresponds to this
  class.

  Revision 1.31  1996/10/07 22:53:58  wenger
  Added more error checking and better error messages in response to
  some of the problems uncovered by CS 737 students.

  Revision 1.30  1996/08/07 20:11:46  wenger
  Fixed various key event-related bugs (see Bugs file); changed
  direction of camera movement for 3D to be more in agreement
  with how panning works for 2D views.

  Revision 1.29  1996/08/04 21:31:40  beyer
  Changed to use new DataSourceBuf constructor.

  Revision 1.28  1996/07/23 20:13:03  wenger
  Preliminary version of code to save TData (schema(s) and data) to a file.

  Revision 1.27  1996/07/15 17:21:23  jussi
  String attributes are no longer eliminated from the
  logical schema tree (group directory).

  Revision 1.26  1996/07/12 18:24:47  wenger
  Fixed bugs with handling file headers in schemas; added DataSourceBuf
  to TDataAscii.

  Revision 1.25  1996/05/22 17:52:10  wenger
  Extended DataSource subclasses to handle tape data; changed TDataAscii
  and TDataBinary classes to use new DataSource subclasses to hide the
  differences between tape and disk files.

  Revision 1.24  1996/05/07 22:28:37  jussi
  Moved the call to InsertCatFile() from ParseCatPhysical to
  ParseCat.

  Revision 1.23  1996/05/07 16:04:20  wenger
  Added final version of code for reading schemas from session files;
  added an error-reporting class to improve error info.

  Revision 1.22  1996/04/26 17:51:15  wenger
  Fixed up schema parsing code to also handle logical schemas in
  session files; restored parseSchema command to
  parseSchema <name> <physical> <logical>.

  Revision 1.21  1996/04/25 21:42:15  wenger
  Temporary version of code to parse schemas in session files --
  writes schema to a temporary file (currently not implemented for
  logical schemas).

  Revision 1.20  1996/04/22 18:35:02  wenger
  Unfixed a memory leak that I fixed earlier.  The fix caused errors
  in sessions with multiple physical schemas.

  Revision 1.19  1996/04/19 17:20:46  wenger
  Put the GenClassInfo code back in -- this is needed for tape data;
  started adding the tape-related code back in (it was previously
  deleted for some reason; I'm not yet done adding it back); added
  the 'DEVise parseSchema' command and the first parts of the code
  related to it.

  Revision 1.18  1996/04/17 16:34:38  wenger
  Conditionaled out the GenClassInfo class and all related code,
  since the program exits if that code is ever executed.

  Revision 1.17  1996/04/15 19:33:48  wenger
  Consolidated the two (nearly identical) functions for
  reading/parsing physical schema, and cleaned up the
  ParseCat.c module quite a bit.  As part of this, I added
  a new enum DevStatus (StatusOk, StatusFailed, etc.).

  Revision 1.16  1996/03/26 21:14:28  jussi
  String attributes are not inserted into the group directory.

  Revision 1.15  1996/03/05 23:25:03  jussi
  Open file now properly closed in ParseCatLogical().

  Revision 1.14  1996/01/23 20:50:17  jussi
  Added support for binary data files specified in the schema.

  Revision 1.13  1996/01/10 21:00:41  jussi
  Disabled debugging output.

  Revision 1.12  1996/01/10 19:02:36  jussi
  Reorganized code so that fewer function prototypes are needed.
  Removed bug which occurred when date hi/lo fields were defined
  in string form; Parse() uses static argument vectors which
  would overlay the attribute name when attrList->InsertAttr
  is called.

  Revision 1.11  1996/01/10 00:39:40  jussi
  Added support for storing date values as hi/lo values.

  Revision 1.10  1995/12/22 00:07:24  jussi
  Name of logical schema, not physical schema, is recorded in
  catFiles. The absolute path name gets stored in a session file,
  so storing the physical schema name in catFiles will cause the
  physical schema to appear in the Auto dialog when the session file
  is restored.

  Revision 1.9  1995/12/20 07:03:27  ravim
  High and low values of attrs can be specified.

  Revision 1.8  1995/12/14 17:44:02  jussi
  Small fixes to get rid of g++ -Wall warnings.

  Revision 1.7  1995/12/13 02:07:09  ravim
  Groups are identified not by the physical schema name but by the logical
  schema file name - which is guaranteed to be unique since they are all files
  in the same directory.

  Revision 1.6  1995/12/12 03:42:45  ravim
  Fixed a bug.

  Revision 1.5  1995/12/11 18:03:13  ravim
  Physical and logical schema handled.

  Revision 1.4  1995/11/18 01:55:29  ravim
  Default group created automatically if no groups specified. Also,
  parsing of schema bypassed if already loaded.

  Revision 1.3  1995/09/28 00:00:08  ravim
  Fixed some bugs. Added some new functions for handling groups.

  Revision 1.2  1995/09/05 22:15:09  jussi
  Added CVS header.
*/

#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>

#include "ParseCat.h"
#include "TDataAsciiInterp.h"
#include "TDataDQLInterp.h"
#include "TDataDQL.h"
#include "TDataBinaryInterp.h"
#include "Parse.h"
#include "Control.h"
#include "Util.h"
#include "Init.h"
#include "Group.h"
#include "GroupDir.h"
#include "DeviseTypes.h"
#include "DataSourceBuf.h"
#include "DataSourceFileStream.h"
#include "DevError.h"

//#define DEBUG

GroupDir *gdir = new GroupDir();

#define LINESIZE 512

const int INIT_CAT_FILES     = 64;
static char **_catFiles      = 0;
static int _numCatFiles      = 0;
static int _catFileArraySize = 0;
static int numAttrs          = 0;
static AttrList *attrs       = 0;

static int _line = 0;
static char *   srcFile = __FILE__;

/*------------------------------------------------------------------------------
 * function: SetVal
 * Set the value field in aval to the value equivalent of valstr based
 * on the valtype.
 */
static void
SetVal(AttrVal *aval, char *valstr, AttrType valtype)
{

  double tempval;

  switch(valtype) {
    case IntAttr: 
      aval->intVal = atoi(valstr);
      break;
    case FloatAttr:
      aval->floatVal = atof(valstr);
      break;
    case DoubleAttr:
      aval->doubleVal = atof(valstr);
      break;
    case StringAttr:
      aval->strVal = CopyString(valstr);
      break;
    case DateAttr:
      (void)ParseFloatDate(valstr, tempval);
      aval->dateVal = (time_t)tempval;
      break;
    default:
      fprintf(stderr,"unknown attr value\n");
      Exit::DoExit(2);
      break;
    }
}

/*------------------------------------------------------------------------------
 * function: InsertCatFile
 * Insert a new catalog file name into the list of catalog files.
 */
static void
InsertCatFile(char *name)
{
  if (!_catFiles){
    _catFiles = new char *[INIT_CAT_FILES];
    _catFileArraySize = INIT_CAT_FILES;
  } else if (_numCatFiles >= _catFileArraySize) {
    char **temp = new char *[_catFileArraySize + INIT_CAT_FILES];
    for(int i=0; i < _numCatFiles; i++) {
      temp[i] = _catFiles[i];
    }
    delete _catFiles;
    _catFiles = temp;
    _catFileArraySize += INIT_CAT_FILES;
  }
  _catFiles[_numCatFiles++] = name;
}

/*------------------------------------------------------------------------------
 * function: CatFiles
 * Get the catalog files.
 */
void
CatFiles(int &numFiles, char **&fileNames)
{
  numFiles = _numCatFiles;
  fileNames = _catFiles;
}

/*------------------------------------------------------------------------------
 * function: ClearCats
 * Clear the catalog files.
 */
void ClearCats()
{
  _numCatFiles = 0;
}

#ifndef NO_GEN_CLASS_INFO

const int MAX_GENCLASSINFO = 20;
static int _numGenClass = 0;

static struct { 
  char *source;
  GenClassInfo *genInfo;
} _genClasses[MAX_GENCLASSINFO];

/*------------------------------------------------------------------------------
 * function: RegisterGenClassInfo
 * Register the TData class generator for a given source.
 */
void
RegisterGenClassInfo(char *source, GenClassInfo *gen)
{
  if (_numGenClass == MAX_GENCLASSINFO) {
    fprintf(stderr, "too many interpreted TData class generator\n");
    Exit::DoExit(1);
  }
  _genClasses[_numGenClass].source = source;
  _genClasses[_numGenClass++].genInfo = gen;
}

/*------------------------------------------------------------------------------
 * function: FindGenClass
 * Find the TData generator for a given source.
 */
static GenClassInfo *
FindGenClass(char *source)
{
  for(int i = 0; i < _numGenClass; i++) {
    if (strcmp(_genClasses[i].source,source) == 0)
      return _genClasses[i].genInfo;
  }

  fprintf(stderr,"Can't find TData generator for input source %s\n",source);
  Exit::DoExit(1);
  
  // keep compiler happy
  return 0;
}
#endif

/*------------------------------------------------------------------------------
 * function: ParseChar
 * Parse a character, Return false if can't parse.
 */
static Boolean
ParseChar(char *instr, char &c)
{
  char *str = instr;
  if (*str == '\\') {
    str++;
    switch(*str) {
    case 'n':
      c = '\n';
      break;
    case 'r':
      c = '\r';
      break;
    case 't':
      c = '\t';
      break;
    case '\'':
      c = '\'';
      break;
    default:
      goto error;
      break;
    }
  } else
    c = *str;
  return true;

 error:
  fprintf(stderr, "ParseCat: invalid separator %s\n", instr);
  return false;
}

/* Parse separators */
const int MAX_SEPARATORS = 50;
static char separators[MAX_SEPARATORS];
static int numSeparators;

/*------------------------------------------------------------------------------
 * function: ParseSeparator
 * Parse a separator; return false if can't parse.
 */
static Boolean
ParseSeparator(int numArgs, char **args)
{
  if (numArgs >= MAX_SEPARATORS) {
    fprintf(stderr, "ParseCat: too many separators, max = %d\n",
	    MAX_SEPARATORS);
    return false;
  }

  for(int i = 1; i < numArgs; i++) {
    if (!ParseChar(args[i], separators[i - 1]))
      return false;
  }

  numSeparators = numArgs - 1;
  return true;
}


static char whitespaces[MAX_SEPARATORS];
static int numWhitespace;

/*------------------------------------------------------------------------------
 * function: ParseWhiteSpace
 * Parse whitespace; return false if can't parse.
 */
static Boolean
ParseWhiteSpace(int numArgs, char **args)
{
  if (numArgs >= MAX_SEPARATORS) {
    fprintf(stderr, "ParseCat: too many separators, max = %d\n",
	    MAX_SEPARATORS);
    return false;
  }

  for(int i = 1; i < numArgs; i++) {
    if (!ParseChar(args[i], whitespaces[i - 1]))
      return false;
  }

  numWhitespace = numArgs - 1;
  return true;
}

/*------------------------------------------------------------------------------
 * function: ParseAttr
 * Parse an attribute.
 */
static DevStatus
ParseAttr(
	int &	numArgs,
	char **	args,
	int &	recSize,
	Boolean	hasFileType,
	char *	fileType)
{
	int			attrLength;
	AttrType	attrType;
	DevStatus	result = StatusOk;

	/* an attribute */
	Boolean isSorted = false;
	if (strcmp(args[0],"sorted") == 0)
	{
		/* sorted attribute */
		isSorted = true;
		if (strcmp(args[1],"attr") && strcmp(args[1],"compattr"))
		{
			fprintf(stderr,"'sorted' must be followed by 'attr' or 'compattr'\n");
			result = StatusFailed;
			return result;
		}
		args = &args[1];
		numArgs--;
		isSorted = true;
	}

	Boolean isComposite;
	if (strcmp(args[0],"attr") == 0)
		isComposite = false;
	else isComposite = true;

	/* get attr type */
	int attrNum = 0;
	if (numArgs < 3)
	{
		fprintf(stderr,"attr needs at least 3 args\n");
		result = StatusFailed;
		return result;
	}

	if (strcmp(args[2],"int") == 0)
	{
		attrType = IntAttr;
		attrLength = sizeof(int);
		attrNum = 3;
	}
	else if (strcmp(args[2],"double") == 0)
	{
		attrType = DoubleAttr;
		attrLength = sizeof(double);
		attrNum = 3;
	}
	else if (strcmp(args[2],"float") == 0)
	{
		attrType = FloatAttr;
		attrLength = sizeof(float);
		attrNum = 3;
	}
	else if (strcmp(args[2],"date") == 0)
	{
		attrType = DateAttr;
		attrLength = sizeof(long);
		attrNum = 3;
	}
	else if (strcmp(args[2],"string") == 0)
	{
		attrType = StringAttr;
		if (numArgs < 4)
		{
			fprintf(stderr,"string attr needs length\n");
			result = StatusFailed;
			return result;
		}
		attrLength = atoi(args[3]);
		attrNum = 4;
	}
	else
	{
		fprintf(stderr,"unknown type %s\n",args[2]);
		result = StatusFailed;
		return result;
	}

	char *attrName = CopyString(args[1]);
	DOASSERT(strchr(attrName, '/') == NULL,
	  "Cannot have '/' in attribute name");

	Boolean hasMatchVal = false;
	AttrVal matchVal;
	Boolean hasHi = false;
	Boolean hasLo = false;
	AttrVal hiVal, loVal;

	if ((attrNum < numArgs) && (!strcmp(args[attrNum], "=")))
	{
		attrNum++;
		if (attrNum > numArgs-1)
		{
	    	fprintf(stderr,"expecting default value after '='\n");
			result = StatusFailed;
			return result;
		}
		hasMatchVal = true;
		SetVal(&matchVal, args[attrNum], attrType);
		attrNum++;
	}
			
	if ((attrNum < numArgs) && 
	    (strcmp(args[attrNum], "hi")) && 
	    (strcmp(args[attrNum], "lo"))) 
	{
		fprintf(stderr, "Unrecognized chars in an attribute definition line\n");
		result = StatusFailed;
		return result;
	} 
	else if (attrNum < numArgs)
	{
		if (!strcmp(args[attrNum], "hi"))
		{
	    	hasHi = true;
	    	attrNum++;
	    	if (attrNum >= numArgs)
	    	{
	    		fprintf(stderr, "Expecting value after keyword hi\n");
				result = StatusFailed;
				return result;
			}
			SetVal(&hiVal, args[attrNum], attrType);
			attrNum++;
		}
			  
		if ((attrNum < numArgs) && 
			(!strcmp(args[attrNum], "lo")))
		{
			hasLo = true;
			attrNum++;
			if (attrNum >= numArgs)
			{
				fprintf(stderr, "Expecting value after keyword lo\n");
				return result;
			}
			SetVal(&loVal, args[attrNum], attrType);
			attrNum++;
		}

		if (attrNum < numArgs)
		{
			fprintf(stderr, "Unrecognized chars in an attribute definition line\n");
			result = StatusFailed;
			return result;
		} 
	}

	if (attrs == NULL)
	{
	    if (!hasFileType )
		{
			reportError("no file type yet", devNoSyserr);
			result = StatusFailed;
			return result;
		}
		attrs = new AttrList(fileType);
	}

	int roundAmount = 0;
	switch(attrType)
	{
	  case FloatAttr:
	    roundAmount = sizeof(float);
	    break;
	  case DoubleAttr:
	    roundAmount = sizeof(double);
	    break;
	  case StringAttr:
	    roundAmount = sizeof(char);
	    break;
	  case DateAttr:
	    roundAmount = sizeof(time_t);
	    break;
	  case IntAttr:
	    roundAmount = sizeof(int);
	    break;
	  default:
	    fprintf(stderr,"ParseCat: don't know type\n");
	    Exit::DoExit(2);
	}

	if (recSize/roundAmount*roundAmount != recSize)
	{
		/* round to rounding boundaries */
		recSize = (recSize/roundAmount+1)*roundAmount;
	}

	attrs->InsertAttr(numAttrs, attrName, recSize,
			  attrLength, attrType, hasMatchVal,
			  &matchVal, isComposite, isSorted,
			  hasHi, &hiVal, hasLo, &loVal);
	numAttrs++;
	recSize += attrLength;

	delete attrName;

	return result;
}

/*------------------------------------------------------------------------------
 * function: ParseCatPhysical
 * Read and parse a physical schema from a catalog file.
 * physicalOnly should be true if only a physical schema (not a physical
 * schema and a logical schema) is being read.
 */
static char *
ParseCatPhysical(DataSource *schemaSource, Boolean physicalOnly,Boolean typeOnly= false)
{
	Boolean hasSource = false;
	char *source = 0; /* source of data. Which interpreter we use depends
			     on this */

	char buf[LINESIZE];
	Boolean hasFileType = false;
	Boolean hasSeparator = false;
	Boolean hasWhitespace = false;
	Boolean hasComment = false;

	Boolean isAscii = false;
	Boolean GLoad = true;
	char *fileType = 0;
	int numArgs;
	char **args;
	int recSize = 0;
	char *sep = 0;
	int numSep = 0;
	char *commentString = 0;
	Group *currgrp = NULL;

	attrs = NULL;
	numAttrs = 0;

	if (schemaSource->Open("r") != StatusOk)
	{
		reportError("schemaSource->Open() failed", devNoSyserr);
		goto error;
	}
	_line = 0;
	while (schemaSource->Fgets(buf, LINESIZE) != NULL)
	{
		StripTrailingNewline(buf);

		_line++;
#if 0
		printf("getting line '%s'\n", buf);
#endif
		if (buf[0] == '#' || buf[0] == '\n' || buf[0] == '\r')
			continue;
		Parse(buf,numArgs, args);
		if (numArgs == 0)
			continue;

#ifdef DEBUG
		printf("parse: ");
		for(int ind = 0; ind < numArgs; ind++)
		  printf("'%s' ", args[ind]);
		printf("\n");
#endif

		if (strcmp(args[0],"end")== 0)
		{
			break;
		}
		else if (strcmp(args[0],"source") == 0)
		{
			source = CopyString(args[1]);
			hasSource = true;
		}
		else if (strcmp(args[0],"separator")== 0)
		{
			/* parse separator */
			hasSeparator = ParseSeparator(numArgs, args);
			if (!hasSeparator){
				reportError("can't parse separator", devNoSyserr);
				goto error;
			}
		}
		else if (strcmp(args[0],"whitespace")== 0)
		{
			/* parse separator */
			hasWhitespace = ParseWhiteSpace(numArgs, args);
		}
		else if (strcmp(args[0],"comment") == 0)
		{
			if (numArgs != 2){
				reportError("can't parse comment string", devNoSyserr);
				goto error;
			}
			hasComment = true;
			commentString = CopyString(args[1]);
		}
		else if (strcmp(args[0],"type") == 0)
		{
			if (numArgs != 3)
			{
				reportError("can't parse file type -- need 3 args",
					devNoSyserr);
				goto error;
			}
			if (strcmp(args[2],"ascii") == 0)
			{
				isAscii = true;
			}
			else if (strcmp(args[2],"binary") == 0)
			{
				isAscii = false;
			}
			else
			{
				fprintf(stderr,"don't know file type %s, must be ascii or binary", args[2]);
				goto error;
			}
			fileType = CopyString(args[1]);
			
			// In case only the type information is needed return that...
			if (typeOnly){
				schemaSource->Close();
				return fileType;
			}

			hasFileType = true;
			if (physicalOnly)
			{
				/* Let's add the schema name to the directory now */
				/* First check if the schema is already loaded, in
				   which case we do nothing more */
				if (gdir->find_entry(StripPath(schemaSource->getLabel())))
				{
				  GLoad = false;
				}
				else
				{
				  printf("Adding (physical) schema %s to directory \n",
					StripPath(schemaSource->getLabel()));
				  gdir->add_entry(StripPath(schemaSource->getLabel()));
				  GLoad = true;
				}
			}
		}
		else if (strcmp(args[0],"attr") == 0 ||
			   strcmp(args[0],"compattr") == 0 ||
			   strcmp(args[0],"sorted") == 0)
		{
			if (ParseAttr(numArgs, args, recSize, hasFileType, fileType) !=
				StatusOk) goto error;
		}
		else if (physicalOnly && !strcmp(args[0], "group"))
		{
		  if (GLoad) {
		      if (!currgrp)		/* Top level */
		      {
			currgrp = new Group(args[1], NULL, TOPGRP);
			gdir->add_topgrp(StripPath(schemaSource->getLabel()), currgrp);
		      }
		      else
			currgrp = currgrp->insert_group(args[1]);
		    }
		}
		else if (physicalOnly && !strcmp(args[0], "item"))
		{
		  if (GLoad)
		  {
		      currgrp->insert_item(args[1]);
		  }
		}
		else if (physicalOnly && !strcmp(args[0], "endgroup"))
		{
		  if (GLoad)
		  {
		      if (!currgrp)
		      {
			fprintf(stderr, "Group begins and ends not matched\n");
			goto error;
		      }
		      currgrp = currgrp->parent_group();
		    }
		}
		else
		{
		fprintf(stderr, "Error at %s: %d\n", __FILE__, __LINE__);
	    	fprintf(stderr,"ParseCat: unknown command %s\n", args[0]);
	    	goto error;
		}
	}

	/* round record size */
	if (recSize/8*8 != recSize){
		/* round to rounding boundaries */
		recSize = (recSize/8+1)*8;
	}

	if (!hasFileType ){
		reportError("ParseCat: no file type specified", devNoSyserr);
		goto error;
	}

	if (numAttrs == 0){
		reportError("ParseCat: no attribute specified", devNoSyserr);
		goto error;
	}

	int i,j;

	if (physicalOnly)
	{
	/* If no group has been defined, create a default group */
	if (GLoad && (gdir->num_topgrp(StripPath(schemaSource->getLabel())) == 0))
	{
	  Group *newgrp = new Group("__default", NULL, TOPGRP);
	  gdir->add_topgrp(StripPath(schemaSource->getLabel()), newgrp);
	  for (i=0; i < numAttrs; i++) {
	    AttrInfo *iInfo = attrs->Get(i);
            newgrp->insert_item(iInfo->name);
	  }
	}
	}

	/* test attribute names */
	for (i=0 ; i < numAttrs-1;i++) {
		AttrInfo *iInfo = attrs->Get(i);
		if (strcmp(iInfo->name,"recId") == 0){
			fprintf(stderr,"attribute name 'recId' is reserved\n");
			goto error;
		}
		for (j=i+1; j < numAttrs; j++){
			AttrInfo *jInfo = attrs->Get(j);
			if (strcmp(iInfo->name,jInfo->name)== 0){
				fprintf(stderr,"ParseCat:duplicate attribute name %s\n",
					iInfo->name);
				goto error;
			}
		}
	}

	// Note: the only difference between separators and white space
	// is that if you specify separators, each separator is parsed
	// as delineating a new attribute, even if there are multiple
	// separators in a row.  If you specify whitespace, multiple
	// consecutive white space characters are treated as a single
	// separator; also they are ignored if they preceed the first
	// attribute.  A separator preceding the first attribute in
	// a record will cause an error.  See Parse.c.  RKW 10/7/96.
	if (isAscii) {
	  if (hasSeparator && hasWhitespace){
	    fprintf(stderr,"can't specify both whitespace and separator\n");
	    goto error;
	  }
	  if (!(hasSeparator || hasWhitespace)){
	    fprintf(stderr,"must specify either whitespace or separator\n");
	    goto error;
	  }
	}

	if (hasSeparator) {
	  sep = new char[numSeparators];
	  for (i=0; i < numSeparators; i++){
	    sep[i] = separators[i];
	  }
	  numSep = numSeparators;
	}
	if (hasWhitespace) {
	  sep = new char[numWhitespace];
	  for (i=0; i < numWhitespace; i++){
	    sep[i] = whitespaces[i];
	    }
	  numSep = numWhitespace;
	}
	
	if (!hasComment)
	  commentString = "#";
	  
	if (hasSource)
	{
#ifndef NO_GEN_CLASS_INFO
		if (physicalOnly)
		{
			printf("source: %s\n",source);
		}
		else
		{
			printf("schema: %s\n",source);
		}
		GenClassInfo *genInfo = FindGenClass(source);
		ControlPanel::RegisterClass(
			genInfo->Gen(source, isAscii, fileType,
			attrs, recSize,sep, numSep, hasSeparator, commentString),
			true);
#else
		fprintf(stderr, "Illegal token 'source' in schema\n");
		Exit::DoExit(1);
#endif
	}
	else
	{
		if (isAscii) {
		  printf("default source, recSize %d\n",recSize);
		  ControlPanel::RegisterClass(
		     new TDataAsciiInterpClassInfo(fileType,
			attrs, recSize,sep, numSep, hasSeparator,
			commentString), true);
		} else {
		  printf("default binary source, recSize %d\n",recSize);
		  ControlPanel::RegisterClass(
		     new TDataBinaryInterpClassInfo(fileType, attrs, recSize),
					      true);
		}
	}

	schemaSource->Close();

	if (Init::PrintTDataAttr())
		attrs->Print();
	return fileType;

error:
	schemaSource->Close();

	if (attrs != NULL) delete attrs;
	fprintf(stderr,"error at line %d\n", _line);
	return NULL;
}
/*------------------------------------------------------------------------------
 * function: ParseCatPhysical
 * Read and parse a physical schema from a catalog file.
 * physicalOnly should be true if only a physical schema (not a physical
 * schema and a logical schema) is being read.
 */
char *
ParseDQL(char * name,char * schema,char *schemaFile,char * query)
{

    gdir->add_entry(name);
	TDataDQLInterpClassInfo * DQLclass = 
		new TDataDQLInterpClassInfo(schema,schemaFile,query);
	
	ControlPanel::RegisterClass(DQLclass,true);
	
	if (Init::PrintTDataAttr())
		attrs->Print();

	if (gdir->num_topgrp(name) == 0)
	{
	  Group *newgrp = new Group("__default", NULL, TOPGRP);
	  gdir->add_topgrp(name,newgrp);

	  AttrList attrs(*DQLclass->GetAttrList());
	  int numAttrs = attrs.NumAttrs(); 

	  for (int i=0; i < numAttrs; i++) {
	    AttrInfo *iInfo = attrs.Get(i);
            newgrp->insert_item(iInfo->name);
	  }

	}
	return schema;
}

/*------------------------------------------------------------------------------
 * function: ParseCatLogical
 * Read and parse a logical schema from a catalog file.
 */
static char *
ParseCatLogical(DataSource *schemaSource, char *sname)
{
  Group *currgrp = NULL;
  Boolean GLoad = true;
  char buf[LINESIZE];
  int numArgs;
  char **args;

  if (schemaSource->Open("r") != StatusOk)
  {
    goto error;
  }
  _line = 0;
  
  /* Skip past any leading comment or blank lines, and past the line
   * specifying the physical schema.  Note that the line specifying the
   * physical schema MUST be the first nonblank, non-comment line. */
  buf[0] = '\0';
  while (IsBlank(buf) || (buf[0] == '#'))
  {
    schemaSource->Fgets(buf, LINESIZE);
  }
  
  /* Let's add the group name to the directory now */
  /* The groups for a particular logical schema are identified by the 
     schema file name. This is bcos the type name of the physical schema
     is not a unique identifier - several logical schemas may use the same
     physical schema */
  /* First check if the schema is already loaded, in
     which case we do nothing more */

  if (gdir->find_entry(schemaSource->getLabel()))
    GLoad = false;
  else
  {
    printf("Adding (logical) schema %s to directory \n",
		schemaSource->getLabel());
    gdir->add_entry(schemaSource->getLabel());
    GLoad = true;
  }
 
  while (schemaSource->Fgets(buf,LINESIZE) != NULL)
  {
	  StripTrailingNewline(buf);
      
      _line++;
      /*
	 printf("getting line %s\n", buf);
	 */
      if (buf[0] == '#' || buf[0] == '\n' || buf[0] == '\r')
	continue;
      Parse(buf,numArgs, args);
      if (numArgs == 0)
	continue;
     
#ifdef DEBUG
      printf("parse: ");
      for(int ind = 0; ind < numArgs; ind++)
	printf("'%s' ", args[ind]);
      printf("\n");
#endif

      if (strcmp(args[0], "group") == 0)
      {
	if (GLoad) {
	    if (!currgrp)		/* Top level */
	    {
	      currgrp = new Group(args[1], NULL, TOPGRP);
	      gdir->add_topgrp(schemaSource->getLabel(), currgrp);
	    }
	    else
	      currgrp = currgrp->insert_group(args[1]);
	  }
      }
      else if (strcmp(args[0], "item") == 0)
      {
	if (GLoad) {
	    currgrp->insert_item(args[1]);
	}
      }
      else if (strcmp(args[0], "endgroup") == 0)
      {
	if (GLoad) {
	    if (!currgrp)
	    {
	      fprintf(stderr, "Group begins and ends not matched\n");
	      goto error;
	    }
	    currgrp = currgrp->parent_group();
	  }
      }
      else {
	fprintf(stderr, "Error at %s: %d\n", __FILE__, __LINE__);
	  fprintf(stderr,"ParseCat: unknown command %s\n", args[0]);
	  goto error;
      }
  }

  /* If no group has been defined, create a default group */
  if (GLoad && (gdir->num_topgrp(schemaSource->getLabel()) == 0))
  {
    Group *newgrp = new Group("__default", NULL, TOPGRP);
    gdir->add_topgrp(schemaSource->getLabel(), newgrp);
    for(int i = 0; i < numAttrs; i++) {
      AttrInfo *iInfo = attrs->Get(i);
      newgrp->insert_item(iInfo->name);
    }
  }

  schemaSource->Close();

  return sname;

 error:
  schemaSource->Close();
  
  fprintf(stderr,"error at line %d\n", _line);
  return NULL;
}

/*------------------------------------------------------------------------------
 * function: ParseCat
 * Read and parse a schema file.
 */
char *
ParseCat(char *catFile) 
{
  // Check the first line of catFile - if it is "physical abc",
  // call ParseCatPhysical(DataSourceFile(abc), false) and then
  // ParseCatLogical(DataSourceFile(catFile)).
  // Otherwise, simply call ParseCatPhysical(DataSourceFile(catFile), true).

  char *	result = NULL;
  
  // Need to check if the type is DQL - then we dont have to open a file
  // simply get the data from a DataStreamBuf instead of from the file..
  
  FILE *fp = fopen(catFile, "r");
  if (!fp)
  {
    fprintf(stderr,"ParseCat: can't open file %s\n", catFile);
  }
  else
  {
    char buf[LINESIZE];
	// Find the first nonblank, non-comment line.
    buf[0] = '\0';
	while (IsBlank(buf) || (buf[0] == '#'))
	{
		fgets(buf, LINESIZE, fp);
		StripTrailingNewline(buf);
	}

	// Look for the keyword 'physical' to determine whether this is a
	// logical schema file.
        // Note: we could parse the header here to look for logical schema file
        // type, too.
	char token1[LINESIZE];
	char token2[LINESIZE];
	sscanf(buf, "%s %s", token1, token2);
    
	if (!strcmp(token1,"logical")) {
		
		// The query needs to be passed down to TDataDQL instead of being
		// evaluated..
		// Thus we need to call ParseDQL instead of anything else....
		// Pass the schema and the query itself..
    	
		buf[0] = '\0';
		while (IsBlank(buf) || (buf[0] == '#'))
		{
			fgets(buf, LINESIZE, fp);
			StripTrailingNewline(buf);
		}
		fclose(fp);
		char * query = strdup(buf);
		char * schema = strdup(token2);
		// Extract the physical schema name 	
		fp = fopen(	token2,"r");

	  	if (!fp)
	  	{
			fprintf(stderr,"ParseCat: can't open file %s\n", catFile);
	  	}
	  	else
	  	{
			char buf[LINESIZE];
			// Find the first nonblank, non-comment line.
			buf[0] = '\0';
			while (IsBlank(buf) || (buf[0] == '#'))
			{
				fgets(buf, LINESIZE, fp);
				StripTrailingNewline(buf);
			}
	
			// Look for the keyword 'physical' to determine whether this is a
		
			char token1[LINESIZE];
			char token2[LINESIZE];
			sscanf(buf, "%s %s", token1, token2);
    
			printf(" DQL:: Schema = %s Query = %s \n",token2,query);
     	 	DataSourceFileStream schemaSource(token2, StripPath(token2));
			//char * temp = ParseDQL((&schemaSource)->getLabel(),
		    // ParseCatPhysical(&schemaSource,false,true),schema,query);
			
			char * temp = ParseDQL(StripPath(catFile),
				ParseCatPhysical(&schemaSource,false,true),schema,query);
			
			free(query);
			free(schema);
			return temp;

		}
		return "";

	}
	
	fclose(fp);
	if (strcmp(token1, "physical"))
	{
      DataSourceFileStream	schemaSource(catFile, StripPath(catFile));
      result = ParseCatPhysical(&schemaSource, true);
      InsertCatFile(CopyString(catFile));
    }
    else
    {
      char *sname;
      DataSourceFileStream	schemaSource(token2, StripPath(token2));
      if (!(sname = ParseCatPhysical(&schemaSource, false))) result = NULL;

      InsertCatFile(CopyString(catFile));

      DataSourceFileStream	logSchemaSource(catFile, StripPath(catFile));
      result = ParseCatLogical(&logSchemaSource, sname);
    }
  }

  return result;
}

/*------------------------------------------------------------------------------
 * function: ParseSchema
 * Parse a schema from buffer(s).
 */
char *
ParseSchema(char *schemaName, char *physSchema, char *logSchema)
{
    char *  result = NULL;
    Boolean physicalOnly = (logSchema == NULL) || !strcmp(logSchema, "");
    
    if ((physSchema != NULL) && strcmp(physSchema, "")) {
	int len = strlen(physSchema);
	DataSourceBuf schemaSource(physSchema, len, len, schemaName);
	result = ParseCatPhysical(&schemaSource, physicalOnly);
    } else {
	fprintf(stderr, "No physical schema specified\n");
    }

    if (!physicalOnly) {
	int len = strlen(logSchema);
	DataSourceBuf schemaSource(logSchema, len, len, schemaName);
    	result = ParseCatLogical(&schemaSource, result);
    }

    return result;
}

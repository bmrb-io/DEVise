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

#include "ParseCat.h"
#include "TDataAsciiInterp.h"
#include "TDataBinaryInterp.h"
#include "Parse.h"
#include "Control.h"
#include "Util.h"
#include "Init.h"
#include "Group.h"
#include "GroupDir.h"

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

static char *getTail(char *fname)
{
  /* Either the character string after the last slash or the entire string
     if there are no slashes in it */

  char *ret = fname;
  int len = strlen(fname);
  
  for(int i = 0; i < len; i++)
    if (fname[i] == '/')
      ret = &(fname[i + 1]);

  return ret;
}

static void SetVal(AttrVal *aval, char *valstr, AttrType valtype)
{
  /* Set the value field in aval to the value equivalent of valstr based
     on the valtype */

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

/* Insert a new file name into file */
static void InsertCatFile(char *name)
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

void CatFiles(int &numFiles, char **&fileNames)
{
  numFiles = _numCatFiles;
  fileNames = _catFiles;
}

void ClearCats()
{
  _numCatFiles = 0;
}

const int MAX_GENCLASSINFO = 20;
static int _numGenClass = 0;

static struct { 
  char *source;
  GenClassInfo *genInfo;
} _genClasses[MAX_GENCLASSINFO];

void RegisterGenClassInfo(char *source, GenClassInfo *gen)
{
  if (_numGenClass == MAX_GENCLASSINFO) {
    fprintf(stderr, "too many interpreted TData class generator\n");
    Exit::DoExit(1);
  }
  _genClasses[_numGenClass].source = source;
  _genClasses[_numGenClass++].genInfo = gen;
}

GenClassInfo *FindGenClass(char *source)
{
  for(int i = 0; i < _numGenClass; i++) {
    if (strcmp(_genClasses[i].source,source) == 0)
      return _genClasses[i].genInfo;
  }

  fprintf(stderr,"Can't find TData generator for input sourrce %s\n",source);
  Exit::DoExit(1);
  
  // keep compiler happy
  return 0;
}

/* Parse a character, Return -1 if can't parse */
static Boolean ParseChar(char *instr, char &c)
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

Boolean ParseSeparator(int numArgs, char **args)
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

/* Parse white space */
static char whitespaces[MAX_SEPARATORS];
static int numWhitespace;

Boolean ParseWhiteSpace(int numArgs, char **args)
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

char *ParseCatOriginal(char *catFile){
	FILE *file= NULL;
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
	int attrLength;
	AttrType attrType;
	char *commentString = 0;
	Group *currgrp = NULL;
	attrs = NULL;
	numAttrs = 0;

	/*
	printf("opening file %s\n", catFile);
	*/
	file = fopen(catFile, "r");
	if (file == NULL){
		fprintf(stderr,"ParseCat: can't open file %s\n", catFile);
		goto error;
	}
	_line = 0;
	while (fgets(buf,LINESIZE, file) != NULL) {
		int len = strlen(buf);
		if (len > 0 && buf[len-1] == '\n')
			buf[len-1] = '\0';

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

		if (strcmp(args[0],"end")== 0)
			break;
		else if (strcmp(args[0],"source") == 0){
			source = CopyString(args[1]);
			hasSource = true;
		}
		else if (strcmp(args[0],"separator")== 0){
			/* parse separator */
			hasSeparator = ParseSeparator(numArgs, args);
			if (!hasSeparator){
				fprintf(stderr,"can't parse separator\n");
				goto error;
			}
		}
		else if (strcmp(args[0],"whitespace")== 0){
			/* parse separator */
			hasWhitespace = ParseWhiteSpace(numArgs, args);
		}
		else if (strcmp(args[0],"comment") == 0){
			if (numArgs != 2){
				fprintf(stderr,"can't parse comment string\n");
				goto error;
			}
			hasComment = true;
			commentString = CopyString(args[1]);
		}
		else if (strcmp(args[0],"type") == 0){
			if (numArgs != 3){
				fprintf(stderr,"can't parse file type need 3 args\n");
				goto error;
			}
			if (strcmp(args[2],"ascii") == 0){
				isAscii = true;
			}
			else if (strcmp(args[2],"binary") == 0){
				isAscii = false;
			}
			else {
				fprintf(stderr,"don't know file type %s, must be ascii or binary", args[2]);
				goto error;
			}
			fileType = CopyString(args[1]);
			hasFileType = true;
			/* Let's add the schema name to the directory now */
			/* First check if the schema is already loaded, in
			   which case we do nothing more */
			if (gdir->find_entry(getTail(catFile)))
			  GLoad = false;
			else
			{
			  printf("Adding schema %s to directory \n", getTail(catFile));
			  gdir->add_entry(getTail(catFile));
			  GLoad = true;
			}
		} else if (strcmp(args[0],"attr") == 0 ||
			   strcmp(args[0],"compattr") == 0 ||
			   strcmp(args[0],"sorted") == 0) {
			/* an attribute */
			Boolean isSorted = false;
			if (strcmp(args[0],"sorted") == 0) {
				/* sorted attribute */
				isSorted = true;
				if (strcmp(args[1],"attr") && strcmp(args[1],"compattr")){
					fprintf(stderr,"'sorted' must be followed by 'attr' or 'compattr'\n");
					goto error;
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
			if (numArgs < 3){
				fprintf(stderr,"attr needs at least 3 args\n");
				goto error;
			}
			if (strcmp(args[2],"int") == 0){
				attrType = IntAttr;
				attrLength = sizeof(int);
				attrNum = 3;
			}
			else if (strcmp(args[2],"double") == 0){
				attrType = DoubleAttr;
				attrLength = sizeof(double);
				attrNum = 3;
			}
			else if (strcmp(args[2],"float") == 0){
				attrType = FloatAttr;
				attrLength = sizeof(float);
				attrNum = 3;
			}
			else if (strcmp(args[2],"date") == 0){
				attrType = DateAttr;
				attrLength = sizeof(long);
				attrNum = 3;
			}
			else if (strcmp(args[2],"string") == 0){
				attrType = StringAttr;
				if (numArgs < 4){
					fprintf(stderr,"string attr needs length\n");
					goto error;
				}
				attrLength = atoi(args[3]);
				attrNum = 4;
			}
			else {
				fprintf(stderr,"unknown type %s\n",args[2]);
				goto error;
			}

			char *attrName = CopyString(args[1]);

			Boolean hasMatchVal = false;
			AttrVal matchVal;
			Boolean hasHi = false;
			Boolean hasLo = false;
			AttrVal hiVal, loVal;

			if ((attrNum < numArgs) && (!strcmp(args[attrNum], "=")))
			{
			  attrNum++;
			  if (attrNum > numArgs-1){
			      fprintf(stderr,"expecting default value after '='\n");
			      goto error;
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
			  goto error;
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
			      goto error;
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
			      goto error;
			    }
			    SetVal(&loVal, args[attrNum], attrType);
			    attrNum++;
			  }

			  if (attrNum < numArgs)
			  {
			    fprintf(stderr, "Unrecognized chars in an attribute definition line\n");
			    goto error;
			  } 
			}

		if (attrs == NULL){
		    if (!hasFileType ){
		        fprintf(stderr,"no file type yet\n");
			goto error;
		      }
		    attrs = new AttrList(fileType);
		}

		int roundAmount = 0;
		switch(attrType){
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
		if (recSize/roundAmount*roundAmount != recSize){
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
		      }
	else if (strcmp(args[0], "group") == 0)
		{
		  if (GLoad) {
		      if (!currgrp)		/* Top level */
		      {
			currgrp = new Group(args[1], NULL, TOPGRP);
			gdir->add_topgrp(getTail(catFile), currgrp);
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
		fprintf(stderr,"ParseCat: no file type specified\n");
		goto error;
	}

	if (numAttrs == 0){
		fprintf(stderr,"ParseCat: no attribute specified\n");
		goto error;
	}

	int i,j;

	/* If no group has been defined, create a default group */
	if (GLoad && (gdir->num_topgrp(getTail(catFile)) == 0))
	{
	  Group *newgrp = new Group("__default", NULL, TOPGRP);
	  gdir->add_topgrp(getTail(catFile), newgrp);
	  for (i=0; i < numAttrs; i++)
	  {
	    AttrInfo *iInfo = attrs->Get(i);
	    newgrp->insert_item(iInfo->name);
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
	  
	if (hasSource){
		printf("source: %s\n",source);
		GenClassInfo *genInfo = FindGenClass(source);
		ControlPanel::RegisterClass(
			genInfo->Gen(source, isAscii, fileType,
			attrs,recSize,sep, numSep, hasSeparator, commentString),
			true);
	} else {
		if (isAscii) {
		  printf("default source, recSize %d\n",recSize);
		  ControlPanel::RegisterClass(
		     new TDataAsciiInterpClassInfo(fileType,
			attrs,recSize,sep, numSep, hasSeparator,
			commentString), true);
		} else {
		  printf("default binary source, recSize %d\n",recSize);
		  ControlPanel::RegisterClass(
		     new TDataBinaryInterpClassInfo(fileType, attrs, recSize),
					      true);
		}
	}

	fclose(file);

	InsertCatFile(CopyString(catFile));

	if (Init::PrintTDataAttr())
		attrs->Print();
	return fileType;

error:
	if (file != NULL)
		fclose(file);

	if (attrs != NULL)
		delete attrs;
	fprintf(stderr,"error at line %d\n", _line);
	return NULL;
}

char *ParseCatPhysical(char *catFile){
	FILE *file= NULL;
	Boolean hasSource = false;
	char *source = 0; /* source of data. Which interpreter we use depends
			     on this */

	char buf[LINESIZE];
	Boolean hasFileType = false;
	Boolean hasSeparator = false;
	Boolean hasWhitespace = false;
	Boolean hasComment = false;

	Boolean isAscii = false;
	char *fileType = 0;
	int numArgs;
	char **args;
	int recSize = 0;
	char *sep = 0;
	int numSep = 0;
	int attrLength;
	AttrType attrType;
	char *commentString = 0;
	attrs = NULL;
	numAttrs = 0;

	/*
	printf("opening file %s\n", catFile);
	*/
	file = fopen(catFile, "r");
	if (file == NULL){
		fprintf(stderr,"ParseCat: can't open file %s\n", catFile);
		goto error;
	}
	_line = 0;
	while (fgets(buf,LINESIZE, file) != NULL) {
		int len = strlen(buf);
		if (len > 0 && buf[len-1] == '\n')
			buf[len-1] = '\0';

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

		if (strcmp(args[0],"end")== 0)
			break;
		else if (strcmp(args[0],"source") == 0){
			source = CopyString(args[1]);
			hasSource = true;
		}
		else if (strcmp(args[0],"separator")== 0){
			/* parse separator */
			hasSeparator = ParseSeparator(numArgs, args);
			if (!hasSeparator){
				fprintf(stderr,"can't parse separator\n");
				goto error;
			}
		}
		else if (strcmp(args[0],"whitespace")== 0){
			/* parse separator */
			hasWhitespace = ParseWhiteSpace(numArgs, args);
		}
		else if (strcmp(args[0],"comment") == 0){
			if (numArgs != 2){
				fprintf(stderr,"can't parse comment string\n");
				goto error;
			}
			hasComment = true;
			commentString = CopyString(args[1]);
		}
		else if (strcmp(args[0],"type") == 0){
			if (numArgs != 3){
				fprintf(stderr,"can't parse file type need 3 args\n");
				goto error;
			}
			if (strcmp(args[2],"ascii") == 0){
				isAscii = true;
			}
			else if (strcmp(args[2],"binary") == 0){
				isAscii = false;
			}
			else {
				fprintf(stderr,"don't know file type %s, must be ascii or binary", args[2]);
				goto error;
			}
			fileType = CopyString(args[1]);
			hasFileType = true;
		} else if (strcmp(args[0],"attr") == 0 ||
			   strcmp(args[0],"compattr") == 0 ||
			   strcmp(args[0],"sorted") == 0) {
			/* an attribute */
			Boolean isSorted = false;
			if (strcmp(args[0],"sorted") == 0) {
				/* sorted attribute */
				isSorted = true;
				if (strcmp(args[1],"attr") && strcmp(args[1],"compattr")){
					fprintf(stderr,"'sorted' must be followed by 'attr' or 'compattr'\n");
					goto error;
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
			if (numArgs < 3){
				fprintf(stderr,"attr needs at least 3 args\n");
				goto error;
			}
			if (strcmp(args[2],"int") == 0){
				attrType = IntAttr;
				attrLength = sizeof(int);
				attrNum = 3;
			}
			else if (strcmp(args[2],"double") == 0){
				attrType = DoubleAttr;
				attrLength = sizeof(double);
				attrNum = 3;
			}
			else if (strcmp(args[2],"float") == 0){
				attrType = FloatAttr;
				attrLength = sizeof(float);
				attrNum = 3;
			}
			else if (strcmp(args[2],"date") == 0){
				attrType = DateAttr;
				attrLength = sizeof(long);
				attrNum = 3;
			}
			else if (strcmp(args[2],"string") == 0){
				attrType = StringAttr;
				if (numArgs < 4){
					fprintf(stderr,"string attr needs length\n");
					goto error;
				}
				attrLength = atoi(args[3]);
				attrNum = 4;
			}
			else {
				fprintf(stderr,"unknown type %s\n",args[2]);
				goto error;
			}

			char *attrName = CopyString(args[1]);

			Boolean hasMatchVal = false;
			AttrVal matchVal;
			Boolean hasHi = false;
			Boolean hasLo = false;
			AttrVal hiVal, loVal;

			if ((attrNum < numArgs) && (!strcmp(args[attrNum], "=")))
			{
			  attrNum++;
			  if (attrNum > numArgs-1){
			      fprintf(stderr,"expecting default value after '='\n");
			      goto error;
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
			  goto error;
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
			      goto error;
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
			      goto error;
			    }
			    SetVal(&loVal, args[attrNum], attrType);
			    attrNum++;
			  }

			  if (attrNum < numArgs)
			  {
			    fprintf(stderr, "Unrecognized chars in an attribute definition line\n");
			    goto error;
			  } 
			}
		      

		if (attrs == NULL){
		    if (!hasFileType ){
			fprintf(stderr,"no file type yet\n");
			goto error;
		      }
		    attrs = new AttrList(fileType);
		  }

		int roundAmount = 0;
		switch(attrType){
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
		if (recSize/roundAmount*roundAmount != recSize){
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
		      }
	else {
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
		fprintf(stderr,"ParseCat: no file type specified\n");
		goto error;
	}

	if (numAttrs == 0){
		fprintf(stderr,"ParseCat: no attribute specified\n");
		goto error;
	}

	int i,j;

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
	
	if (hasSource){
		printf("schema: %s\n",source);
		GenClassInfo *genInfo = FindGenClass(source);
		ControlPanel::RegisterClass(
			genInfo->Gen(source, isAscii, fileType,
			attrs,recSize,sep, numSep, hasSeparator, commentString),
			true);
	}
	else {
		if (isAscii) {
		  printf("default source, recSize %d\n",recSize);
		  ControlPanel::RegisterClass(
		     new TDataAsciiInterpClassInfo(fileType,
			attrs,recSize,sep, numSep, hasSeparator,
			commentString), true);
		} else {
		  printf("default binary source, recSize %d\n",recSize);
		  ControlPanel::RegisterClass(
		     new TDataBinaryInterpClassInfo(fileType, attrs, recSize),
					      true);
		}
	}

	fclose(file);

	if (Init::PrintTDataAttr())
		attrs->Print();

	return fileType;

error:
	if (file != NULL)
		fclose(file);

	if (attrs != NULL)
		delete attrs;
	fprintf(stderr,"error at line %d\n", _line);
	return NULL;
}

char *ParseCatLogical(char *catFile, char *sname)
{
  Group *currgrp = NULL;
  FILE *file= NULL;
  Boolean GLoad = true;
  char buf[LINESIZE];
  int numArgs;
  char **args;

  file = fopen(catFile, "r");
  if (file == NULL) {
    fprintf(stderr,"ParseCat: can't open file %s\n", catFile);
    goto error;
  }
  _line = 0;
  
  /* read the first line first */
  fgets(buf, LINESIZE, file);
  
  /* Let's add the group name to the directory now */
  /* The groups for a particular logical schema are identified by the 
     schema file name. This is bcos the type name of the physical schema
     is not a unique identifier - several logical schemas may use the same
     physical schema */
  /* First check if the schema is already loaded, in
     which case we do nothing more */

  if (gdir->find_entry(getTail(catFile)))
    GLoad = false;
  else
  {
    printf("Adding schema %s to directory \n", getTail(catFile));
    gdir->add_entry(getTail(catFile));
    GLoad = true;
  }
 
  while (fgets(buf,LINESIZE, file) != NULL) {
      int len = strlen(buf);
      if (len > 0 && buf[len-1] == '\n')
	buf[len-1] = '\0';
      
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
	      gdir->add_topgrp(getTail(catFile), currgrp);
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
	  fprintf(stderr,"ParseCat: unknown command %s\n", args[0]);
	  goto error;
      }
  }

  /* If no group has been defined, create a default group */
  if (GLoad && (gdir->num_topgrp(getTail(catFile)) == 0))
  {
    Group *newgrp = new Group("__default", NULL, TOPGRP);
    gdir->add_topgrp(getTail(catFile), newgrp);
    for(int i = 0; i < numAttrs; i++)
    {
      AttrInfo *iInfo = attrs->Get(i);
      newgrp->insert_item(iInfo->name);
    }
  }

  fclose(file);

  return sname;

 error:
  if (file != NULL)
    fclose(file);
  
  fprintf(stderr,"error at line %d\n", _line);
  return NULL;
}

char *ParseCat(char *catFile) 
{
  // Check the first line of catFile - if it is "physical abc",
  // call ParseCatPhysical (abc) and then ParseCatLogical(catFile)
  // Otherwise, simply call ParseCatPhysical(catFile).

  FILE *fp = fopen(catFile, "r");
  if (!fp) {
    fprintf(stderr,"ParseCat: can't open file %s\n", catFile);
    return NULL;
  }

  char buf[100];
  if (fscanf(fp, "%s", buf) != 1 || strcmp(buf, "physical")) {
    fclose(fp);
    return ParseCatOriginal(catFile);
  }

  // Read in the file name
  fscanf(fp, "%s", buf);
  fclose(fp);

  char *sname;
  if (!(sname = ParseCatPhysical(buf)))
    return NULL;

  InsertCatFile(CopyString(catFile));

  return ParseCatLogical(catFile, sname);
}

/*
  $Id$

  $Log$*/

#include <stdio.h>
#include "ParseCat.h"
#include "TDataAsciiInterp.h"
#include "AttrList.h"
#include "Parse.h"
#include "Control.h"
#include "Util.h"
#include "Init.h"

const int INIT_CAT_FILES = 64;
static char **_catFiles = (char **)NULL;
static int _numCatFiles = 0;
static int _catFileArraySize= 0;

static int _line= 0; 	/* line number */
/* Insert a new file name into file */
static void InsertCatFile(char *name){
	if (_catFiles == (char **)NULL){
		_catFiles = new char *[INIT_CAT_FILES];
		_catFileArraySize = INIT_CAT_FILES;
	}
	else if (_numCatFiles >= _catFileArraySize) {
		char **temp = new char *[_catFileArraySize+INIT_CAT_FILES];
		int i;
		for (i=0; i < _numCatFiles; i++){
			temp[i] = _catFiles[i];
		}
		delete _catFiles;
		_catFiles = temp;
		_catFileArraySize += INIT_CAT_FILES;
	}
	_catFiles[_numCatFiles++] = name;
}

void CatFiles(int &numFiles, char **&fileNames){
	numFiles = _numCatFiles;
	fileNames = _catFiles;
}

void ClearCats(){
	_numCatFiles = 0;
}

const int MAX_GENCLASSINFO = 20;
static int _numGenClass = 0;
struct GenRec { 
	char *source;
	GenClassInfo *genInfo;
};
GenRec _genClasses[MAX_GENCLASSINFO];

void RegisterGenClassInfo(char *source, GenClassInfo *gen){
	if (_numGenClass == MAX_GENCLASSINFO ){
		fprintf(stderr,"too many interpreted TData class generator\n");
		Exit::DoExit(1);
	}
	_genClasses[_numGenClass].source = source;
	_genClasses[_numGenClass++].genInfo = gen;
}

GenClassInfo *FindGenClass(char *source){
	int i;
	for (i=0; i < _numGenClass; i++){
		if (strcmp(_genClasses[i].source,source) == 0)
			return _genClasses[i].genInfo;
	}
	fprintf(stderr,"Can't find TData generator for input sourrce %s\n",source);
	Exit::DoExit(1);
}


/* Parse a character, Return -1 if can't parse */
Boolean ParseChar(char *instr, char &c){
	char *str = instr;
	if (*str == '\\'){
		str++;
		switch(*str){
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
	}
	else c = *str;

	return true;
error:
	fprintf(stderr,"ParseCat: invalid separator %s\n", instr);
	return false;
}

/* Parse separators */
const int MAX_SEPARATORS = 50;
static char separators[MAX_SEPARATORS];
static int numSeparators;

Boolean ParseSeparator(int numArgs, char **args){
	Boolean hasSeparator = false;
	if (numArgs >= MAX_SEPARATORS){
		fprintf(stderr,"ParseCat: too many separators, max = %d\n",
			MAX_SEPARATORS);
		return false;
	}
	int i;
	for (i=1; i < numArgs; i++){
		if (!ParseChar(args[i], separators[i-1]))
			return false;
	}
	numSeparators = numArgs-1;
	return true;
}

/* Parse white space */
static char whitespaces[MAX_SEPARATORS];
static int numWhitespace;
Boolean ParseWhiteSpace(int numArgs, char **args){
	Boolean hasWhiteSpace = false;
	if (numArgs >= MAX_SEPARATORS){
		fprintf(stderr,"ParseCat: too many separators, max = %d\n",
			MAX_SEPARATORS);
		return false;
	}
	int i;
	for (i=1; i < numArgs; i++){
		if (!ParseChar(args[i], whitespaces[i-1]))
			return false;
	}
	numWhitespace = numArgs-1;
	return true;
}


const int NumDefaultSeparator = 2;
static char defaultSeparator[] = { ' ', '\t' };

char *ParseCat(char *catFile){
	FILE *file= NULL;
	AttrList *attrs=NULL ;
	Boolean hasSource = false;
	char *source; /* source of data. Which interpreter we use depends
					on this */

#define LINESIZE 512
	char buf[LINESIZE];
	Boolean hasFileType = false;
	Boolean hasSeparator = false;
	Boolean hasWhitespace = false;
	Boolean hasComment = false;
	Boolean mergeSeparator = true; /* true if separators should be merged */
	Boolean isAscii = false;
	char *fileType;
	int numArgs;
	char **args;
	int recSize = 0;
	int numAttrs = 0;
	int attrLength;
	AttrType attrType;
	char *commentString;

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
		int ind;

		/*
		printf("parse: ");
		for (ind=0; ind < numArgs; ind++){
			printf("'%s' ", args[ind]);
		}
		printf("\n");
		*/

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
			strcmp(args[0],"compattr") == 0 || strcmp(args[0],"sorted") == 0){
			/* an attribute */
			Boolean isSorted = false;
			if (strcmp(args[0],"sorted") == 0){
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

			Boolean hasMatchVal = false;
			AttrVal matchVal;
			if (attrNum < numArgs){
				if (strcmp(args[attrNum],"=") != 0){
					fprintf(stderr,"expecting '='\n");
					goto error;
				}
				attrNum++;
				if (attrNum != numArgs-1){
					fprintf(stderr,"expecting default value after '='\n");
					goto error;
				}
				hasMatchVal = true;
				switch(attrType){
					case IntAttr:
						matchVal.intVal = atoi(args[attrNum]);
						break;
					case FloatAttr:
						matchVal.floatVal = atof(args[attrNum]);
						break;
					case DoubleAttr:
						matchVal.doubleVal = atof(args[attrNum]);
						break;
					case StringAttr:
						matchVal.strVal= CopyString(args[attrNum]);
						break;
					case DateAttr:
						fprintf(stderr,"match of date not implemented\n");
						Exit::DoExit(2);
						break;
					default:
						fprintf(stderr,"unknown attr value\n");
						Exit::DoExit(2);
						break;
				}

			}
			if (attrs == NULL){
				if (!hasFileType ){
					fprintf(stderr,"no file type yet\n");
					goto error;
				}
				attrs = new AttrList(fileType);
			}

			int roundAmount;
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
			attrs->InsertAttr(numAttrs,args[1],recSize,
				attrLength,attrType, hasMatchVal, &matchVal,
				isComposite, isSorted);
			numAttrs++;
			recSize += attrLength;
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

	/* test attribute names */
	int i,j;
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

	if (hasSeparator && hasWhitespace){
		fprintf(stderr,"can't specify both whitespace and separator\n");
		goto error;
	}
	if (!(hasSeparator || hasWhitespace)){
		fprintf(stderr,"must specify either whitespace or separator\n");
		goto error;
	}

	char *sep;
	int numSep;
	if (hasSeparator) {
		sep = new char[numSeparators];
		for (i=0; i < numSeparators; i++){
			sep[i] = separators[i];
		}
		numSep = numSeparators;
	}
	else {
		sep = new char[numWhitespace];
		for (i=0; i < numWhitespace; i++){
			sep[i] = whitespaces[i];
		}
		numSep = numWhitespace;
	}

	if (!hasComment)
		commentString = "#";
	
	if (hasSource){
		printf("schema from %s\n",source);
		GenClassInfo *genInfo = FindGenClass(source);
		ControlPanel::RegisterClass(
			genInfo->Gen(source, fileType,
			attrs,recSize,sep, numSep, hasSeparator, commentString),
			true);
	}
	else {
		/* use default source */
		printf("default schema source, recSize %d\n",recSize);
		ControlPanel::RegisterClass(
			new TDataAsciiInterpClassInfo(fileType,
			attrs,recSize,sep, numSep, hasSeparator, commentString),
			true);
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

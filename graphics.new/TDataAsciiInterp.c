/* TDataAsciiInterp.c */
#include <string.h>
#include "TDataAsciiInterp.h"
#include "AttrList.h"
#include "RecInterp.h"
#include "CompositeParser.h"
#include "Parse.h"
#include "Control.h"
#include "Util.h"

TDataAsciiInterpClassInfo::TDataAsciiInterpClassInfo(
	char *className, AttrList *attrList, int recSize,
	char *separators, int numSeparators,
	Boolean isSeparator, char *commentString){
	_className = className;
	_attrList = attrList;
	_separators = separators;
	_numSeparators = numSeparators;
	_commentString = commentString;
	_recSize = recSize;
	_isSeparator = isSeparator;
	_tdata = NULL;

}

TDataAsciiInterpClassInfo::TDataAsciiInterpClassInfo(
	char *className, char *name, char *alias, TData *tdata){
	_className = className;
	_name = name;
	_alias = alias;
	_tdata = tdata;
}

TDataAsciiInterpClassInfo::~TDataAsciiInterpClassInfo(){
/*
printf("TDataAsciiInterpClassInfo: destructor\n");
*/
	if (_tdata != NULL)
		delete _tdata;
}

char *TDataAsciiInterpClassInfo::ClassName() { return _className; }

static char buf1[256], buf2[256];
static char *args[2];
void TDataAsciiInterpClassInfo::ParamNames(int &argc, char **&argv){
	argc = 2;
	argv = args;
	args[0] = buf1;
	args[1] = buf2;

	sprintf(buf1, "File %s", ControlPanel::Instance()->FileName());
	sprintf(buf2, "Alias %s", ControlPanel::Instance()->FileAlias());
}

ClassInfo *TDataAsciiInterpClassInfo::CreateWithParams(int argc, char **argv){
    if (argc != 2){
		fprintf(stderr,"TDataAsciiInterp::CreateWithParams: wrong args\n");
		for (int i=0; i < argc; i++)
			printf("%s\n", argv[i]);
			return (ClassInfo *)NULL;
	}
	char *name = CopyString(argv[0]);
	char *alias = CopyString(argv[1]);
	TDataAsciiInterp *tdata = new TDataAsciiInterp(name,_recSize, _attrList,
		_separators, _numSeparators, _isSeparator, _commentString);
	return new TDataAsciiInterpClassInfo(_className, name, alias, tdata);
}

char *TDataAsciiInterpClassInfo::InstanceName(){
	return _alias;
}

void *TDataAsciiInterpClassInfo::GetInstance(){
	return _tdata;
}

/* Get parameters that can be used to re-create this instance */
void TDataAsciiInterpClassInfo::CreateParams(int &argc, char **&argv) {
	argc = 2;
	argv = args;
	args[0] = _name;
	args[1] = _alias;
}



TDataAsciiInterp::TDataAsciiInterp(char *name, int recSize, AttrList *attrs,
	char *separators,int numSeparators, Boolean isSeparator,
	char *commentString):
	TDataAscii(name, recSize){
/*
printf("TDataAsciiInterp %s recSize %d\n", name, recSize);
*/
	_recInterp = new RecInterp();
	_recInterp->SetAttrs(attrs);

	_name = name;
	_recSize = recSize;
	_attrList = attrs;
	_separators = separators;
	_numSeparators = numSeparators;
	_commentString = commentString;
	_isSeparator = isSeparator;
	if (_commentString != NULL){
		_commentStringLength = strlen(_commentString);
	}
	_numAttrs = _attrList->NumAttrs();

	_numMatchingAttrs = 0;
	int i;
	for (i=0; i < _numAttrs; i++){
		AttrInfo *info = _attrList->Get(i);
		if (info->hasMatchVal){
			if (_numMatchingAttrs >= MAX_MATCHING_ATTRS){
				fprintf(stderr,"TDataASciiInterp: too many matching attrs\n");
				Exit::DoExit(2);
			}
			_matchingAttrs[_numMatchingAttrs++] = info;
		}
	}

	Initialize();
}

TDataAsciiInterp::~TDataAsciiInterp(){
/*
printf("TDataAsciiInterp: destructor\n");
*/
}

Boolean TDataAsciiInterp::Decode(RecId id, void *recordBuf, char *line){

	/* set buffer for interpreted record */
	_recInterp->SetBuf(recordBuf);

	int numArgs;
	char **args;
/*
	printf("1 parsing line %s\n", line);
*/
	Parse(line,numArgs, args, _separators, _numSeparators, _isSeparator);

/*
printf("got %d args\n", numArgs);
*/
	if (numArgs == 0 || 
		(_commentString != NULL &&
			strncmp(args[0],_commentString,_commentStringLength)== 0)) {
		/*
		printf("comment\n");
		*/
		return false;
	}

	
	int i;

	/* See if this record matches if the record specifies a match*/
	if (_numMatchingAttrs > 0){
		/*
		printf("test for match\n");
		*/

		for (i=0; i < _numMatchingAttrs; i++){
			AttrInfo *info = _matchingAttrs[i];
			if (info->attrNum >= numArgs)
				/* does not match */
				return false;
			switch(info->type){
				case IntAttr:
					int intVal = atoi(args[i]);
					if (intVal != info->matchVal.intVal)
						/* does not match */
						return false;
					break;
				case FloatAttr:
					float floatVal = UtilAtof(args[i]);
					if (floatVal != info->matchVal.floatVal)
						return false;
					break;
				case DoubleAttr:
					double doubleVal = UtilAtof(args[i]);
					if (doubleVal != info->matchVal.doubleVal)
						return false;
					break;
				case StringAttr:
					if (strcmp(args[i],info->matchVal.strVal) != 0)
						return false;
					break;
				case DateAttr:
					fprintf(stderr,"TDataAsciiInterp: date attr not implemented\n");
					Exit::DoExit(2);
					break;
				default:
					fprintf(stderr,"TDataAsciiInterp: unknown attrType\n");
					Exit::DoExit(2);
					break;
			}
		}
	}

/*
printf("match\n");
*/
	int maxArgs = MinMax::min(numArgs, _numAttrs);
	Boolean hasComposite = false;
	for (i=0; i < _numAttrs ; i++){
		AttrInfo *attrInfo = _attrList->Get(i);
		char *ptr = (char *)recordBuf + attrInfo->offset;
		if (attrInfo->isComposite){
			/* ignore composites for now */
			hasComposite = true;
			continue;
		}
		else {
			if (i >= maxArgs)
				/* nothing in buffer to convert */
				continue;
			switch(attrInfo->type){
				case IntAttr:
					int intVal = atoi(args[i]);
					*(int *)ptr = intVal;
					/*
					printf("%d\n",intVal);
					*/
					break;
				case FloatAttr:
					float floatVal = UtilAtof(args[i]);
					*(float *)ptr = floatVal;
					/*
					if (i == 0 )
						printf("attr 0 %f\n",floatVal);
					*/
					break;
				case DoubleAttr:
					double doubleVal = UtilAtof(args[i]);
					*(double *)ptr = doubleVal;
					/*
					if (i == 0 )
						printf("attr 0 %f\n",doubleVal);
					*/
					break;
				case StringAttr:
					strncpy(ptr, args[i], attrInfo->length);
					/*
					printf("'%s'\n",args[i]);
					*/
					break;
				case DateAttr:
					time_t tm = atoi(args[i]);
					*(time_t *)ptr = tm;
					break;
				default:
					fprintf(stderr,"TDataAsciiInterp: unknown attribute type\n");
					Exit::DoExit(2);
			}
		}
	}

	/* decode composite attributes */
	if (hasComposite)
		CompositeParser::Decode(_attrList->GetName(), _recInterp);
	return true;
}

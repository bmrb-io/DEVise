/* TDataMulti.c */
#include <stdio.h>
#include <stdlib.h>
#include "TDataMulti.h"
#include "MultiRec.h"
#include "Parse.h"
#include "Control.h"
#include "Exit.h"
#include "Util.h"

MultiClassInfo::MultiClassInfo(){
	_name = _alias = (char *)NULL;
	_tdata = (TDataMulti *)NULL;
}

MultiClassInfo::MultiClassInfo(char *name, char *alias, TDataMulti *tdata){
	_name = name;
	_alias = alias;
	_tdata = tdata;
}

static char buf1[512], buf2[256];
static char *args[5];
void MultiClassInfo::ParamNames(int &argc, char **&argv){
    argc = 2;
	argv = args;
	args[0] = buf1;
	args[1] = buf2;

	sprintf(buf1, "File %s", ControlPanel::Instance()->FileName());
	sprintf(buf2, "Alias %s", ControlPanel::Instance()->FileAlias());
}

ClassInfo *MultiClassInfo::CreateWithParams(int argc, char **argv){
    if (argc != 2){
		fprintf(stderr,"AfsioClassInfo::CreateWithParams: wrong args\n");
		for (int i=0; i < argc; i++)
		printf("%s\n", argv[i]);
		return (ClassInfo *)NULL;
	}
	char *name = CopyString(argv[0]);
	char *alias = CopyString(argv[1]);
	TDataMulti *tdata = new TDataMulti(name);
	return new MultiClassInfo(name, alias, tdata);
}

char *MultiClassInfo::InstanceName() { return _alias; }

void *MultiClassInfo::GetInstance() { return _tdata; }

/* Get parameters that can be used to re-create this instance */
void MultiClassInfo::CreateParams(int &argc, char **&argv) {
	argc = 2;
	argv = args;
	args[0] = _name;
	args[1] = _alias;
}


TDataMulti::TDataMulti(char *name): TDataAscii(name, sizeof(MultiRec)){
	_hasFirst = false;
	Initialize();
}

/* Decode a record and put data into buffer */
Boolean TDataMulti::Decode(RecId, void *recordBuf, char *line){
	MultiRec *rec = (MultiRec *)recordBuf;
	int numArgs; char **args;
	Parse(line, numArgs, args);
	if (numArgs != 5){
		fprintf(stderr,"TDatamulti::Decode: invalid line\n");
		return false;
	}
	if (!_hasFirst){
		_hasFirst = true;
		_first = atoi(args[1]);
	}

	rec->node = atoi(args[0]);
	/*
	rec->begin = atoi(args[1])- _first;
	rec->end = atoi(args[2]) - _first;
	*/
	rec->begin = atoi(args[1]);
	rec->end = atoi(args[2]);
	rec->job = atoi(args[3]);
	rec->cycles = atoi(args[4]);

	if (rec->node < 0 || rec->job < -32 || rec->job > 32){
		fprintf(stderr,"ignored line %d %d %d %d\n",
			rec->node, rec->begin, rec->end, rec->job);
		return false;
	}

	return true;
}


/* TDataHost.c */
#include <stdio.h>
#include <stdlib.h>
#include "TDataHost.h"
#include "HostRec.h"
#include "Parse.h"
#include "Control.h"
#include "Exit.h"
#include "Util.h"

HostClassInfo::HostClassInfo(){
	_name = _alias = (char *)NULL;
	_tdata = (TDataHost *)NULL;
}

HostClassInfo::HostClassInfo(char *name, char *alias, TDataHost *tdata){
	_name = name;
	_alias = alias;
	_tdata = tdata;
}

static char buf1[512], buf2[256];
static char *args[5];
void HostClassInfo::ParamNames(int &argc, char **&argv){
    argc = 2;
	argv = args;
	args[0] = buf1;
	args[1] = buf2;

	sprintf(buf1, "File %s", ControlPanel::Instance()->FileName());
	sprintf(buf2, "Alias %s", ControlPanel::Instance()->FileAlias());
}

ClassInfo *HostClassInfo::CreateWithParams(int argc, char **argv){
    if (argc != 2){
		fprintf(stderr,"AfsioClassInfo::CreateWithParams: wrong args\n");
		for (int i=0; i < argc; i++)
		printf("%s\n", argv[i]);
		return (ClassInfo *)NULL;
	}
	char *name = CopyString(argv[0]);
	char *alias = CopyString(argv[1]);
	TDataHost *tdata = new TDataHost(name);
	return new HostClassInfo(name, alias, tdata);
}

char *HostClassInfo::InstanceName() { return _alias; }

void *HostClassInfo::GetInstance() { return _tdata; }

/* Get parameters that can be used to re-create this instance */
void HostClassInfo::CreateParams(int &argc, char **&argv) {
	argc = 2;
	argv = args;
	args[0] = _name;
	args[1] = _alias;
}


TDataHost::TDataHost(char *name): TDataAscii(name, sizeof(HostRec)){
	_hasFirst = false;
	Initialize();
}

/* Decode a record and put data into buffer */
Boolean TDataHost::Decode(RecId, void *recordBuf, char *line){
	HostRec *rec = (HostRec *)recordBuf;
	int numArgs; char **args;
	Parse(line, numArgs, args);
	if (numArgs != 4){
		fprintf(stderr,"TDataHost::Decode: invalid line\n");
		return false;
	}
	if (!_hasFirst){
		_hasFirst = true;
		_first = atoi(args[1]);
	}

	rec->time = atoi(args[0]);
	rec->hosts = atoi(args[1]);
	rec->working = atoi(args[2]);
	rec->suspended = atoi(args[3]);

#if 0
	if (rec->working + rec->suspended > rec->hosts) {
		fprintf(stderr,"TDataHosts: ignored line %d %d %d %d\n",
				rec->time, rec->hosts, rec->working, rec->suspended);
		return false;
	}
#endif

	return true;
}


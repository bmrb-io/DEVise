/* VisualLLinkClassInfo.c */
#include <stdio.h>
#include "CursorClassInfo.h"
#include "Cursor.h"
#include "Util.h"
#include "Exit.h"

CursorClassInfo::CursorClassInfo() {}

CursorClassInfo::CursorClassInfo(char *name, VisualFlag flag, 
	DeviseCursor *cursor){
	_name = name;
	_flag = flag;
	_cursor = cursor;
}

CursorClassInfo::~CursorClassInfo(){
	if (_cursor != NULL)
		delete _cursor;
}

char *CursorClassInfo::ClassName() {
	/*
	printf("className %d\n", _flag);
	*/
	return "Cursor";
}

static char *args[2];
static char buf1[80];
void CursorClassInfo::ParamNames(int &argc, char **&argv) {
	argc = 2;
	argv = args;

	args[0] = "Name cursor";
	args[1] = "flags 2";
	return;
}

ClassInfo *CursorClassInfo::CreateWithParams(int argc, char **argv) {
	if (argc != 2){
		fprintf(stderr,"CursorClassInfo::CreateWithParams: need 2 arg\n");
		Exit::DoExit(2);
	}

	char *name = CopyString(argv[0]);
	int flag = atoi(argv[1]);
	DeviseCursor *cursor = new DeviseCursor(name,flag);
	return new CursorClassInfo(name, flag, cursor);
}


/* Set default parameters */
void CursorClassInfo::SetDefaultParams(int argc, char **argv){ }

/* Get default parameters */
void CursorClassInfo::GetDefaultParams(int &argc, char **&argv){
	argc = 0;
}

char *CursorClassInfo::InstanceName(){
	return _name;
}

void *CursorClassInfo::GetInstance(){
	return _cursor;
}

/* Get parameters that can be used to re-create this instance */
void CursorClassInfo::CreateParams(int &argc, char **&argv){
	argc = 2;
	argv= args;
	args[0] = _name;
	sprintf(buf1,"%d",_flag);
	args[1] = buf1;
}

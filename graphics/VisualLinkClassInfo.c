/* VisualLLinkClassInfo.c */
#include <stdio.h>
#include "VisualLink.h"
#include "VisualLinkClassInfo.h"
#include "Util.h"
#include "Exit.h"

VisualLinkClassInfo::VisualLinkClassInfo() {}

VisualLinkClassInfo::VisualLinkClassInfo(char *name, VisualFlag flag, 
	VisualLink *link){
	_name = name;
	_flag = flag;
	_link = link;
}

VisualLinkClassInfo::~VisualLinkClassInfo(){
	if (_link != NULL)
		delete _link;
}

char *VisualLinkClassInfo::ClassName() {
	/*
	printf("className %d\n", _flag);
	*/
	return "Visual_Link";
}

static char *args[2];
static char buf1[80];
void VisualLinkClassInfo::ParamNames(int &argc, char **&argv) {
	argc = 2;
	argv = args;

	args[0] = "Name link";
	args[1] = "flags 2";
	return;
}

ClassInfo *VisualLinkClassInfo::CreateWithParams(int argc, char **argv) {
	if (argc != 2){
		fprintf(stderr,"VisualLinkClassInfo::CreateWithParams: need 2 arg\n");
		Exit::DoExit(2);
	}

	char *name = CopyString(argv[0]);
	int flag = atoi(argv[1]);
	VisualLink *link = new VisualLink(name,flag);
	return new VisualLinkClassInfo(name, flag, link);
}


/* Set default parameters */
void VisualLinkClassInfo::SetDefaultParams(int argc, char **argv){ }

/* Get default parameters */
void VisualLinkClassInfo::GetDefaultParams(int &argc, char **&argv){
	argc = 0;
}

char *VisualLinkClassInfo::InstanceName(){
	return _name;
}

void *VisualLinkClassInfo::GetInstance(){
	return _link;
}

/* Get parameters that can be used to re-create this instance */
void VisualLinkClassInfo::CreateParams(int &argc, char **&argv){
	argc = 2;
	argv= args;
	args[0] = _name;
	sprintf(buf1,"%d",_flag);
	args[1] = buf1;
}

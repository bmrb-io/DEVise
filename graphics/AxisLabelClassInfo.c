/*
  $Id$

  $Log$*/

#include <stdio.h>
#include "AxisLabelClassInfo.h"
#include "View.h"
#include "Util.h"

AxisLabelClassInfo::AxisLabelClassInfo(char *className, GenAxisLabel *gen){
	_gen = gen;
	_className = className;
	_instName = NULL;
	_label = NULL;
}

AxisLabelClassInfo::AxisLabelClassInfo(char *className, char *instName, 
	AxisLabel *label) {
	_className = className;
	_instName = instName;
	_label = label;
}

AxisLabelClassInfo::~AxisLabelClassInfo(){
	if (_label != NULL)
		delete _label;
}

char *AxisLabelClassInfo::ClassName(){
	return _className;
}

static char *args[2];
static char buf1[80], buf2[80];
void AxisLabelClassInfo::ParamNames(int &argc, char **&argv){
	argc = 2;
	argv = args;
	if (_instName != NULL){
		args[0] = buf1;
		sprintf(buf1,"name %s",_instName);
	}
	else args[0] = "name";

	if (_label != NULL){
		args[1] = buf2;
		sprintf(buf2,"format %s",_label->GetFormat());
	} else {
		args[1] = "format";
	}
}
ClassInfo *AxisLabelClassInfo::CreateWithParams(int argc, char **argv) {
	if (argc != 2){
		fprintf(stderr,"AxisLabelClassInfo::CreateWithParams: need 2 params\n");
		Exit::DoExit(2);
	}
	char *name = CopyString(argv[0]);
	AxisLabel *label = _gen->MakeAxisLabel(name);
	label->SetFormat(argv[1]);

	return new AxisLabelClassInfo(_className, name, label);
}

void AxisLabelClassInfo::ChangeParams(int argc, char **argv){
	if (argc != 2){
		fprintf(stderr,"AxisLabelClassInfo::ChangeParams: need 2 params\n");
		Exit::DoExit(2);
	}

	_label->SetFormat(argv[1]);
}

/* Set default parameters */
void AxisLabelClassInfo::SetDefaultParams(int argc, char **argv){}

/* Get default parameters */
void AxisLabelClassInfo::GetDefaultParams(int &argc, char **&argv){
	argc = 0;
	argv = NULL;
}

char *AxisLabelClassInfo::InstanceName(){
	return _instName;
}

void *AxisLabelClassInfo::GetInstance(){
	return _label;
}

/* Get parameters that can be used to re-create this instance */
void AxisLabelClassInfo::CreateParams(int &argc, char **&argv) {
	argc = 2;
	argv = args;
	args[0] = buf1;
	args[1] = buf2;
	sprintf(buf1,"%s",_instName);
	sprintf(buf2,"%s",_label->GetFormat());
}

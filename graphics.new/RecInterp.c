/*
  $Id$

  $Log$*/

#include <stdio.h>
#include <time.h>
#include "AttrList.h"
#include "RecInterp.h"
#include "Util.h"

RecInterp::RecInterp(){
	_attrs = NULL;
	_buf = NULL;
}

void RecInterp::SetBuf(void *buf){
	_buf = buf;
}

void RecInterp::SetAttrs(AttrList *attrs){
	_attrs = attrs;
}

char *RecInterp::GetString(char *attrName){
	AttrInfo *info;
	if (_attrs == NULL || _buf == NULL)
		return NULL;
	if ((info=_attrs->Find(attrName)) == NULL)
		return NULL;
	return ((char *)_buf)+info->offset;
}

double *RecInterp::GetFloat(char *attrName){
	AttrInfo *info;
	if (_attrs == NULL || _buf == NULL)
		return NULL;
	if ((info=_attrs->Find(attrName)) == NULL)
		return NULL;
	return (double *)(((char *)_buf)+info->offset);
}

int *RecInterp::GetInt(char *attrName){
	AttrInfo *info;
	if (_attrs == NULL || _buf == NULL)
		return NULL;
	if ((info=_attrs->Find(attrName)) == NULL)
		return NULL;
	return (int *)(((char *)_buf)+info->offset);
}

time_t *RecInterp::GetDate(char *attrName){
	AttrInfo *info;
	if (_attrs == NULL || _buf == NULL)
		return NULL;
	if ((info=_attrs->Find(attrName)) == NULL)
		return NULL;
	return (time_t *)(((char *)_buf)+info->offset);
}

AttrInfo *RecInterp::GetAttrInfo(char *attrName){
	AttrInfo *info;
	if (_attrs == NULL )
		return NULL;
	return _attrs->Find(attrName);
}

void RecInterp::PrintAttrHeading(){
	if (_attrs == NULL )
		return;
	
	int num = _attrs->NumAttrs();
	int i;
	for (i=0; i < num; i++){
		AttrInfo *info = _attrs->Get(i);
		printf("%s: ",info->name);
	}
}

void RecInterp::Print(Boolean printAttrName) {
	if (_attrs == NULL || _buf == NULL)
		return;
	
	int num = _attrs->NumAttrs();
	int i;
	for (i=0; i < num; i++){
		AttrInfo *info = _attrs->Get(i);
		if (printAttrName)
			printf("%s: ",info->name);
		switch(info->type){
			case IntAttr:
				int *intVal = (int *)(((char *)_buf)+info->offset);
				printf("%d ",*intVal);
				break;
			case FloatAttr:
				float *floatVal = (float *)(((char *)_buf)+info->offset);
				printf("%f ",*floatVal);
				break;
			case DoubleAttr:
				double *doubleVal = (double *)(((char *)_buf)+info->offset);
				printf("%f ",*doubleVal);
				break;
			case StringAttr:
				char *strVal = ((char *)_buf)+info->offset;
				printf("%s ",strVal);
				break;
			case DateAttr:
				time_t *tm = (time_t *)(((char *)_buf)+info->offset);
				printf("'%s' ", DateString(*tm));
				break;
		}
	}
	printf("\n");
}

void RecInterp::PrintAttr(char *buf, int attrNum, Boolean printAttrName) {
	if (_attrs == NULL || _buf == NULL){
		buf[0] = '\0';
		return;
	}
	
	AttrInfo *info = _attrs->Get(attrNum);
	switch(info->type){
		case IntAttr:
			int *intVal = (int *)(((char *)_buf)+info->offset);
			if (printAttrName)
				sprintf(buf,"%s: %d",info->name,*intVal);
			else sprintf(buf,"%d ",*intVal);
			break;
		case FloatAttr:
			float *floatVal = (float *)(((char *)_buf)+info->offset);
			if (printAttrName)
				sprintf(buf,"%s: %.2f",info->name,*floatVal);
			else sprintf(buf,"%f",*floatVal);
			break;
		case DoubleAttr:
			double *doubleVal = (double *)(((char *)_buf)+info->offset);
			if (printAttrName)
				sprintf(buf,"%s: %.2f",info->name,*doubleVal);
			else sprintf(buf,"%f",*doubleVal);
			break;
		case StringAttr:
			char *strVal = ((char *)_buf)+info->offset;
			if (printAttrName)
				sprintf(buf,"%s: %s",info->name,strVal);
			else sprintf(buf,"%s",strVal);
			break;
		case DateAttr:
			time_t *tm = (time_t *)(((char *)_buf)+info->offset);
			if (printAttrName)
				sprintf(buf,"%s: '%s' ", info->name,DateString(*tm));
			else sprintf(buf,"'%s' ", DateString(*tm));
			break;
	}
}

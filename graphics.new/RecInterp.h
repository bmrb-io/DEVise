/*
  $Id$

  $Log$*/

/* Interpreted record */

#ifndef RecInterp_h
#define RecInterp_h
#include <sys/types.h>
#include <sys/time.h>
#include "DeviseTypes.h"

class AttrList;
class AttrInfo;
class RecInterp {
public:
	RecInterp();

	void SetBuf(void *buf);
	void *GetBuf() {return _buf;}

	void SetAttrs(AttrList *attrs);

	/* Get the address of attributes, or NULL if not found */
	char *GetString(char *attrName);
	double *GetFloat(char *attrName);
	int *GetInt(char *attrName);
	time_t *GetDate(char *attrName);

	/* Get attribute info, or NULL */
	AttrInfo *GetAttrInfo(char *attrName);

	/* print Attr names as heading */
	void PrintAttrHeading(); 
	/* Print record */
	void Print(Boolean printAttrName = false);

	/* Print info about ith attribute into buffer. */
	void PrintAttr(char *buf, int attrNum, Boolean printAttrName = false);
private:
	AttrList *_attrs;
	void *_buf;
};
#endif

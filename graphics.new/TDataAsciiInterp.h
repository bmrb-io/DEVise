/* TDataAsciiInterp.h: interpreted TData using parsed information */
#ifndef TDataASciiInterp_h
#define TDataASciiInterp_h
#include "DeviseTypes.h"
#include "ClassDir.h"
#include "TDataAscii.h"

class AttrList;
class AttrInfo;
class TDataAsciiInterpClassInfo: public ClassInfo{
public:
	TDataAsciiInterpClassInfo(char *className, AttrList *attrList, 
		int recSize, char *separators, int numSeparators,
		Boolean isSeparator, char *commentString);
	
	TDataAsciiInterpClassInfo(char *className, char *name, char *alias,
		TData *tdata);
	virtual ~TDataAsciiInterpClassInfo();

    /* Info for category */
	virtual char *CategoryName(){ return "tdata"; } 

	/* Info for class */
	virtual char *ClassName();    /* name of class */

	/* Get name of parameters and default/current values */
	virtual void ParamNames(int &argc, char **&argv);

	/* Create instance using the supplied parameters. Return
	the instance info if successful, otherwise return NULL. */
	virtual ClassInfo *CreateWithParams(int argc, char **argv);

    /**************************************************
	Instance Info.
	***************************************************/
	virtual char *InstanceName();
	virtual void *GetInstance();

	/* Get parameters that can be used to re-create this instance */
	virtual void CreateParams(int &argc, char **&argv);
private:
	char *_className;
	char *_name;
	char *_alias;
	TData *_tdata;
	int _recSize;
	AttrList *_attrList;
	char *_separators;
	int _numSeparators;
	char *_commentString;
	Boolean _isSeparator;
};

/* Max # of fields for which the interprter searches for a
matching value */
const int MAX_MATCHING_ATTRS = 10;
class RecInterp;
class TDataAsciiInterp: public TDataAscii {
public:
	TDataAsciiInterp(char *name, int recSize, AttrList *attrs,
		char *separators,int numSeparators, 
		Boolean isSeparator, char *commentString);
	virtual ~TDataAsciiInterp();

	AttrList *GetAttrList(){ return _attrList; };
protected:
    /* Decode a record and put data into buffer. Return false if
	this line is not valid. */
	virtual Boolean Decode(RecId id, void *recordBuf, char *line);

	virtual Boolean WriteCache(int fd){ return true; }
	virtual Boolean ReadCache(int fd){ return true;}

private:
AttrList *_attrList; /* list of attributes */
char *_name;
int _recSize;
char *_separators;
Boolean _isSeparator;
int _numSeparators;
char *_commentString; /* string for comment, or NULL */
int _commentStringLength; /* length of comment string */
int _numAttrs;
AttrInfo *_matchingAttrs[MAX_MATCHING_ATTRS];
int _numMatchingAttrs;
RecInterp *_recInterp;
};

#endif

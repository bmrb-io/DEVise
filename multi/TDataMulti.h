/*
  ========================================================================
  DEVise Data Visualization Software
  (c) Copyright 1992-1995
  By the DEVise Development Group
  Madison, Wisconsin
  All Rights Reserved.
  ========================================================================

  Under no circumstances is this software to be copied, distributed,
  or altered in any way without prior permission from the DEVise
  Development Group.
*/

/*
  $Id$

  $Log$*/

#ifndef TDataMulti_h
#define TDataMulti_h

#include "TDataAscii.h"
#include "ClassDir.h"

class TDataMulti;
class MultiClassInfo : public ClassInfo {
public:
	MultiClassInfo();	/* for creating class */

	/* for creating instance */
	MultiClassInfo(char *name, char *alias, TDataMulti *tdata); 

	/* Info for category */
	virtual char *CategoryName() { return "tdata"; }

	/* Info for class */
	virtual char *ClassName(){ return "multi"; }   /* name of class */

	/* Get name of parameters */
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
	char *_name;
	char *_alias;
	TDataMulti *_tdata;
};

class TDataMulti: public TDataAscii {
public:
	TDataMulti(char *name);

protected:
	/* Decode a record; return false if this line is not valid. */
	virtual Boolean IsValid(char *line);

	/* Decode a record and put data into buffer */
	virtual Boolean Decode(RecId id,void *recordBuf, char *line);

private:
	int _first;
	Boolean _hasFirst;
};

#endif

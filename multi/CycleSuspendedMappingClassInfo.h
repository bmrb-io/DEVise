/*
  ========================================================================
  DEVise Data Visualization Software
  (c) Copyright 1992-1997
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

  $Log$
 */

#ifndef CycleSuspendedMappingClassInfo_h
#define CycleSuspendedMappingClassInfo_h
#include "ClassDir.h"

/* Info needed by mapping */
struct CycleSuspendedMapInfo {
	int *job_ordering;
	GlobalColor **colorArrays;
};

extern CycleSuspendedMapInfo *CycleSuspendedmapInfo;


class CycleSuspendedMappingInfo : public ClassInfo {
public:
	CycleSuspendedMappingInfo();	/* for creating class */

	/* for creating instance */
	CycleSuspendedMappingInfo(char *tdataAlias, char *name, TDataMap *mapping);

	/* Info for category */
	virtual char *CategoryName() { return "mapping"; }

	/* Info for class */
	virtual char *ClassName(){ return "CycleSuspendedMap"; }   /* name of class */

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
	char *_tdataAlias;
	TDataMap *_map;
};

#endif

#ifndef CycleOccMappingClassInfo_h
#define CycleOccMappingClassInfo_h
#include "ClassDir.h"

/* Info needed by mapping */
struct CycleOccMapInfo {
	int *job_ordering;
		Color **colorArrays;
};

extern CycleOccMapInfo *CycleOccmapInfo;


class CycleOccMappingInfo : public ClassInfo {
public:
	CycleOccMappingInfo();	/* for creating class */

	/* for creating instance */
	CycleOccMappingInfo(char *tdataAlias, char *name, TDataMap *mapping);

	/* Info for category */
	virtual char *CategoryName() { return "mapping"; }

	/* Info for class */
	virtual char *ClassName(){ return "CycleOccMap"; }   /* name of class */

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

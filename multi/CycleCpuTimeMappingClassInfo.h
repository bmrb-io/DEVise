#ifndef CycleCpuTimeMappingClassInfo_h
#define CycleCpuTimeMappingClassInfo_h
#include "ClassDir.h"

/* Info needed by mapping */
struct CycleCpuTimeMapInfo {
	int *job_ordering;
	GlobalColor **colorArrays;
};

extern CycleCpuTimeMapInfo *CycleCpuTimemapInfo;


class CycleCpuTimeMappingInfo : public ClassInfo {
public:
	CycleCpuTimeMappingInfo();	/* for creating class */

	/* for creating instance */
	CycleCpuTimeMappingInfo(char *tdataAlias, char *name, TDataMap *mapping);

	/* Info for category */
	virtual char *CategoryName() { return "mapping"; }

	/* Info for class */
	virtual char *ClassName(){ return "CycleCpuTimeMap"; }   /* name of class */

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

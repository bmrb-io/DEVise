/* Mapping class info for compiled mapings */
#ifndef MapClassInfo_h
#define MapClassInfo_h
#include "ClassDir.h"

class MapInfo : public ClassInfo {
public:
	enum MapType {SolDnType, RainType, TempType};
	MapInfo(MapType mapType);	/* for creating class */

	/* for creating instance */
	MapInfo(MapType mapType, char *tdataAlias, char *name, TDataMap *mapping);

	/* Info for category */
	virtual char *CategoryName() { return "mapping"; }

	/* Info for class */
	virtual char *ClassName(){ 
		if (_mapType == SolDnType)
			return "SolDnCompiled"; 
		else if (_mapType == RainType)
			return "RainCompiled";
		else return "TempCompiled";
	} 

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
	MapType _mapType; 
};

#endif

/* Class info for MappingInterp */
#ifndef MapInterpClassInfo_h
#define MapInterpClassInfo_h
#include "ClassDir.h"
#include "VisualArg.h"

const int MAX_INTERPRETED_MAPPING_CLASS = 512;
class MappingInterp;
class TData;
class MappingInterpCmd;
class MapInterpClassInfo: public ClassInfo {
public:
	/* constructor for prototype MapInterp */
	MapInterpClassInfo();
	virtual ~MapInterpClassInfo();

	/* return user info, which is whether this is an interpreted 
	mapping */
	void *UserInfo() { return &_isInterp; }

	/* constructor for new interpreted mapping class 
	created by prototype */
	MapInterpClassInfo( char *className, char *fileAlias,
		VisualFlag *dimensionInfo,
		int numDimensions, MappingInterpCmd *cmd,int cmdFlag, int attrFlag);

	/* constructor for interpreted mapping instance */
	MapInterpClassInfo(char *className,
		char *fileAlias, char *name, VisualFlag *dimensionInfo,
		int numDimensions, MappingInterp *map, TData *tdata, 
		MappingInterpCmd *cmd, int cmdFlag, int attrFlag);

	/* Info for category */
	virtual char *CategoryName() { return "mapping"; }

	/* Info for class */
	virtual char *ClassName(){ return _className; }

	/* Get name of parameters */
	virtual void ParamNames(int &argc, char **&argv);

	/* Create instance using the supplied parameters. Return 
	the instance info if successful, otherwise return NULL. */
	virtual ClassInfo *CreateWithParams(int argc, char **argv) ;

	/* Return true is parameters can be changed dynamically at run time */
	virtual Boolean Changeable() { return true; }

	/* Change parameters dynamically at run time */
	virtual void ChangeParams(int argc, char **argv);


	/* Set default parameters */
	void SetDefaultParams(int argc, char **argv);

	/* Get default parameters */
	void GetDefaultParams(int &argc, char **&argv);

	/**************************************************
	Instance Info. 
	***************************************************/
	virtual char *InstanceName();
	virtual void *GetInstance();

	/* Get parameters that can be used to re-create this instance */
	virtual void CreateParams(int &argc, char **&argv);

	/* Get list of interpreted mapping classes */
	void MapClasses(int &num, MapInterpClassInfo **&classes) {
		num = _numMapClasses;
		classes = _mapClasses;
	}

	/* Clear list of mapping classes, for closing a session */ 
	void ClearMapClasses(){
		_numMapClasses = 0;
	}

private:
void ExtractCommand(int argc, char **argv, MappingInterpCmd *cmd,
	int &cmdFlag,int &attrFlag, VisualFlag *dimensionInfo, int &numDimensions,
		char *&tdataAlias, TData *&tdata, char *&name);

	char *_fileAlias;
	char *_className; /* name of this interpreted mapping class */
	MappingInterpCmd *_cmd;
	int _attrFlag;
	int _cmdFlag;
	char *_name;
	MappingInterp *_map;
	TData *_tdata;
	VisualFlag *_dimensionInfo;
	int _numDimensions;

	int _isInterp; /* always set to true */

	/* list of interpreted mapping classes. Used only by the prototype
	mapping classinfo generator */
	static int _numMapClasses;
	static MapInterpClassInfo *_mapClasses[MAX_INTERPRETED_MAPPING_CLASS];
};
#endif

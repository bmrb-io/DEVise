/* MapInterpClassInfo.c */
#include "DeviseTypes.h"
#include "Control.h"
#include "MappingInterp.h"
#include "MapInterpClassInfo.h"
#include "Util.h"

int MapInterpClassInfo::_numMapClasses = 0;
MapInterpClassInfo *
	MapInterpClassInfo::_mapClasses[MAX_INTERPRETED_MAPPING_CLASS];

MapInterpClassInfo::MapInterpClassInfo(){
	_isInterp = true;
	_cmd = NULL;
	_fileAlias = NULL;
	_className = "Interpreted";
	_name = NULL;
	_map = NULL;
}

MapInterpClassInfo::MapInterpClassInfo(char *className, char *fileAlias,
	VisualFlag *dimensionInfo, int numDimensions, 
	MappingInterpCmd *cmd, int cmdFlag, int attrFlag){
/*
printf("Creating new mapping class %s file %s\n",className, fileAlias);
*/
	_isInterp = true;
	_className = className;
	_dimensionInfo = dimensionInfo;
	_numDimensions = numDimensions;
	_cmd = cmd;
	_cmdFlag = cmdFlag;
	_attrFlag = attrFlag;
	_fileAlias = fileAlias;
	_name = NULL;
	_map = NULL;
}


MapInterpClassInfo::MapInterpClassInfo(char *className,
	char *fileAlias, char *name, 
	VisualFlag *dimensionInfo, int numDimensions, MappingInterp *map, 
	TData *tdata, MappingInterpCmd *cmd, int cmdFlag, int attrFlag){
/*
printf("MapInterpClassINfo alias %s, name %s\n", fileAlias, name);
*/
	_isInterp = true;

	_className = className;
	_fileAlias = fileAlias;
	_name = name;
	_dimensionInfo = dimensionInfo;
	_numDimensions = numDimensions;
	_map = map;
	_tdata = tdata;
	_cmd = cmd;
	_cmdFlag = cmdFlag;
	_attrFlag = attrFlag;
}

MapInterpClassInfo::~MapInterpClassInfo(){
	/* we'll waste memory for now by not deleting stuff */
	/*
	printf("MapInterpClassInfo destructor\n");
	*/
	if (_map != NULL)
		delete _map;
}

/* Return true if string is not emtpy. A string is not empty
if it contains characters other than '\n', '\t', and '\r' */
Boolean NotEmpty(char *str){
	while (*str != '\0'){
		if (!( *str == '\n' || *str == ' ' || *str == '\r' || *str == '\t'))
			return true;
		str++;
	}
	return false;
}

/* Extrac command from parameters */
void MapInterpClassInfo::ExtractCommand(int argc, char **argv, 
	MappingInterpCmd *cmd, int &cmdFlag,int &attrFlag, 
	VisualFlag *dimensionInfo, int &numDimensions,
	char *&tdataAlias, TData *&tdata, char *&name){
	if (argc != 12){
		fprintf(stderr,"MapInterpClassInfo::ExtractCommand: argc != 12\n");
		Exit::DoExit(2);
	}
	cmdFlag = 0;
	attrFlag=0;
	int i;
	for (i=3; i <= 9; i++){
		if (NotEmpty(argv[i])){
			cmdFlag |= (1 << (i-3));
		}
	}
	if (NotEmpty(argv[10]))
		attrFlag |= 1;
	if (NotEmpty(argv[11]))
		attrFlag |= 2;

	/*
	printf("cmdFlag 0x%x attrFlag 0x%x\n",cmdFlag, attrFlag);
	*/

	cmd->xCmd = cmd->yCmd = cmd->colorCmd = cmd->sizeCmd =
	cmd->patternCmd = cmd->shapeCmd = cmd->orientationCmd = NULL;
	for (i=0; i < MAX_GDATA_ATTRS; i++)
		cmd->shapeAttrCmd[i] = NULL;

	tdataAlias = CopyString(argv[0]);
	if ((tdata=(TData *)ControlPanel::FindInstance(tdataAlias)) == NULL){
		fprintf(stderr,"MapInterpClassInfo::CreateWithParams: can't find tdata\n");
		Exit::DoExit(2);
	}

	name = CopyString(argv[1]);

	numDimensions = 0;
	if (argv[2][0] == 'X' || argv[2][0] == 'x') {
		numDimensions = 1;
		dimensionInfo[0] = VISUAL_X;
	}

	if (cmdFlag & MappingCmd_X){
		cmd->xCmd = CopyString(argv[3]);
	}

	if (cmdFlag & MappingCmd_Y){
		cmd->yCmd = CopyString(argv[4]);
	}

	if (cmdFlag & MappingCmd_Color){
		cmd->colorCmd = CopyString(argv[5]);
	}

	if (cmdFlag & MappingCmd_Size){
		cmd->sizeCmd = CopyString(argv[6]);
	}

	if (cmdFlag & MappingCmd_Pattern){
		cmd->patternCmd = CopyString(argv[7]);
	}

	if (cmdFlag & MappingCmd_Orientation){
		cmd->orientationCmd = CopyString(argv[8]);
	}

	if (cmdFlag & MappingCmd_Shape){
		cmd->shapeCmd = CopyString(argv[9]);
	}

	if (attrFlag & 1){
		cmd->shapeAttrCmd[0] = CopyString(argv[10]);
	}

	if (attrFlag & 2){
		cmd->shapeAttrCmd[1] = CopyString(argv[11]);
	}
}

static char buf[12][256];
static char *args[12];
void MapInterpClassInfo::ParamNames(int &argc, char **&argv){

	argc = 12;
	argv = args;

	/* parameters are: tdata, x, y, color, size, pattern, orientation,
		shape, and shapeAttr */
	if (_cmd == NULL){
		/* params for creating a new mapping */
		sprintf(buf[0],"File_Alias {%s}", ControlPanel::Instance()->FileAlias());
		sprintf(buf[1],"Map_Name mapname");
		args[0] = buf[0];
		args[1] = buf[1];
		args[2] = "Sorted X";
		args[3] = "X";
		args[4] = "Y";
		args[5] = "Color";
		args[6] = "Size";
		args[7] = "Pattern";
		args[8] = "Orientation";
		args[9] = "Shape";
		args[10] = "ShapeAttr0";
		args[11] = "ShapeAttr1";
	}
	else {
		/* params for an existing interpreted mapping */
		if (_fileAlias == NULL){
			sprintf(buf[0],"File_Alias {%s}", 
				ControlPanel::Instance()->FileAlias());
		}
		else {
			sprintf(buf[0],"File_Alias {%s}",_fileAlias);
		}
	
		if (_name == NULL){
			sprintf(buf[1],"Map_Name {%s}",
				ControlPanel::Instance()->GDataName());
		} else {
			sprintf(buf[1],"GData_Name {%s}", _name);
		}
		args[0] = buf[0];
		args[1] = buf[1];
		if (_numDimensions == 1){
			args[2] = "Sorted X";
		}
		else args[2] = "Sorted";

		if (_cmd->xCmd != NULL){
			sprintf(buf[3],"X {%s}",_cmd->xCmd);
			args[3] = buf[3];
		}
		else args[3] = "X";

		if (_cmd->yCmd != NULL){
			sprintf(buf[4],"Y {%s}",_cmd->yCmd);
			args[4] = buf[4];
		}
		else args[4] = "Y";

		if ( _cmd->colorCmd != NULL){
			sprintf(buf[5],"Color {%s}",_cmd->colorCmd);
			args[5] = buf[5];
		}
		else args[5] = "Color";

		if ( _cmd->sizeCmd != NULL){
			sprintf(buf[6],"Size {%s}",_cmd->sizeCmd);
			args[6] = buf[6];
		}
		else args[6] = "Size";

		if ( _cmd->patternCmd != NULL){
			sprintf(buf[7],"Pattern {%s}",_cmd->patternCmd);
			args[7] = buf[7];
		}
		else args[7] = "Pattern";

		if ( _cmd->orientationCmd != NULL){
			sprintf(buf[8],"Orientation {%s}",_cmd->orientationCmd);
			args[8] = buf[8];
		}
		else args[8] = "Orientation";

		if ( _cmd->shapeCmd!= NULL){
			sprintf(buf[9],"Shape {%s}",_cmd->shapeCmd);
			args[9] = buf[9];
		}
		else args[9] = "Shape";

		if ( _cmd->shapeAttrCmd[0] != NULL){
			sprintf(buf[10],"ShapeAttr0 {%s}",_cmd->shapeAttrCmd[0]);
			args[10] = buf[10];
		}
		else args[10] = "ShapeAttr0";

		if (_cmd != NULL && _cmd->shapeAttrCmd[0] != NULL){
			sprintf(buf[11],"ShapeAttr1 {%s}",_cmd->shapeAttrCmd[1]);
			args[11] = buf[11];
		}
		else args[11] = "ShapeAttr1";
	}
}

void MapInterpClassInfo::ChangeParams(int argc, char**argv){
	char *tdataAlias;
	char *name;
	TData *tdata;
	ExtractCommand(argc, argv, _cmd, _cmdFlag, _attrFlag,
		_dimensionInfo, _numDimensions, tdataAlias, tdata, name);
	_map->ChangeCmd(_cmd, _cmdFlag, _attrFlag, _dimensionInfo,_numDimensions);
}


ClassInfo *MapInterpClassInfo::CreateWithParams(int argc, char **argv){
/*
printf("MapInterpclassInfo: CreateWithParams\n");
*/
	MappingInterpCmd *cmd = new MappingInterpCmd;
	TData *tdata;
	int cmdFlag, attrFlag;
	VisualFlag *dimensionInfo = new VisualFlag;
	int numDimensions;
	char *tdataAlias, *name;
/*
printf("MapInterpclassInfo: CreateWithParams: extractCmd\n");
*/
	ExtractCommand(argc, argv, cmd, cmdFlag, attrFlag,
		dimensionInfo, numDimensions, tdataAlias, tdata, name);
/*
printf("MapInterpclassInfo: CreateWithParams: after extractCmd\n");
*/
	
	if (_cmd == NULL) {
/*
printf("MapInterpclassInfo: CreateWithParams: create new class\n");
*/
		/* create a new interpreted mapping class */
		if (_numMapClasses >= MAX_INTERPRETED_MAPPING_CLASS){
			fprintf(stderr,"MapInterpClassInfo: too many mapping classes\n");
			Exit::DoExit(2);
		}
		MapInterpClassInfo *cInfo =
			new MapInterpClassInfo(name, tdataAlias, dimensionInfo,
			numDimensions, cmd, cmdFlag, attrFlag);
		_mapClasses[_numMapClasses++] = cInfo;
		return cInfo;
	}
	else {
		/* We are an interpreted mapping class. Create a
		new interpreted mapping */
/*
printf("MapInterpclassInfo: CreateWithParams: create new instance\n");
*/

		/*
		if (strcmp(tdataAlias, _fileAlias) !=0  ){
			fprintf(stderr,"interpreted mapping: mapping is defined for tdata %s not $s\n", _fileAlias, tdataAlias);
			return NULL;
		}
		*/

		MappingInterp *map = new MappingInterp(name,tdata,cmd,
			cmdFlag, attrFlag, dimensionInfo, numDimensions);
		
		return  new MapInterpClassInfo(_className,
			tdataAlias, name, dimensionInfo,
			numDimensions, map, tdata, cmd, cmdFlag, attrFlag);
	}
}


/* Set default parameters */
void MapInterpClassInfo::SetDefaultParams(int argc, char **argv){
}

/* Get default parameters */
void MapInterpClassInfo::GetDefaultParams(int &argc, char **&argv){
	argc = 0;
}

/**************************************************
Instance Info.
***************************************************/
char *MapInterpClassInfo::InstanceName(){
	return _name;
}

void *MapInterpClassInfo::GetInstance(){
	return _map;
}

/* Get parameters that can be used to re-create this instance */
void MapInterpClassInfo::CreateParams(int &argc, char **&argv){

	/* parameters are: fileAlias mapName  x, y, color, size, 
	pattern, orientation, shape, and shapeAttr */
	argc = 12;
	argv = args;

	args[0] = _fileAlias;
	if (_name == NULL){
		/* info about creating an interpreted mapping class */
		args[1] = _className;
	}
	else {
		/* creating a mapping instance */
		args[1] = _name;
	}
	if (_numDimensions == 1){
		args[2] = "X";
	}
	else args[2] = "";

	if (_cmd->xCmd != NULL){
		args[3] = _cmd->xCmd;
	}
	else args[3] = "";

	if (_cmd->yCmd != NULL){
		args[4] = _cmd->yCmd;
	}
	else args[4] = "";

	if ( _cmd->colorCmd != NULL){
		args[5] = _cmd->colorCmd;
	}
	else args[5] = "";

	if ( _cmd->sizeCmd != NULL){
		args[6] = _cmd->sizeCmd;
	}
	else args[6] = "";

	if ( _cmd->patternCmd != NULL){
		args[7] = _cmd->patternCmd;
	}
	else args[7] = NULL;

	if ( _cmd->orientationCmd != NULL){
		args[8] = _cmd->orientationCmd;
	}
	else args[8] = "";

	if ( _cmd->shapeCmd!= NULL){
		args[9] = _cmd->shapeCmd;
	}
	else args[9] = NULL;

	if ( _cmd->shapeAttrCmd[0] != NULL){
		args[10] = _cmd->shapeAttrCmd[0];
	}
	else args[10] = "";

	if (_cmd != NULL && _cmd->shapeAttrCmd[0] != NULL){
		args[11] = _cmd->shapeAttrCmd[1];
	}
	else args[11] = NULL;
}

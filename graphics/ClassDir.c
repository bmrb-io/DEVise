/* ClassDir.c */
#include <stdio.h>
#include "ClassDir.h"
#include "Exit.h"
#include "Util.h"

ClassDir::ClassDir() {
	_numCategories = 0;
}

void ClassDir::InsertCategory(char *name) {
	if (_numCategories >= MaxCategories){
		fprintf(stderr,"ClassDir::InsertCategory\n");
		Exit::DoExit(1);
	}

	CategoryRec *rec = new CategoryRec;
	rec->name = name;
	rec->_numClasses = 0;
	_categories[_numCategories++] = rec;
}

void ClassDir::InsertClass(ClassInfo *cInfo){
	int i;
	for (i=0; i < _numCategories; i++){
		CategoryRec *catRec = _categories[i];
		if (strcmp(catRec->name,cInfo->CategoryName())== 0){
			/* found */
			if (catRec->_numClasses >= MaxClasses){
				fprintf(stderr,"ClassDir::InsertClass MaxClass\n");
				Exit::DoExit(1);
			}
			ClassRec *classRec = new ClassRec;
			classRec->classInfo = cInfo;
			classRec->_numInstances = 0;
			catRec->_classRecs[catRec->_numClasses++] = classRec;
			return;
		}
	}
	fprintf(stderr,"ClassDir::InsertClass\n");
	Exit::DoExit(1);
}

char *argArray[512];

void ClassDir::ClassNames(char *category, int &num, char **&names){
	int i,j;
	num =0 ;
	names = argArray;
	for (i=0; i < _numCategories; i++){
		CategoryRec *catRec = _categories[i];
		if (strcmp(catRec->name,category)== 0){
			for (j=0; j < catRec->_numClasses; j++){
				argArray[num++] = catRec->_classRecs[j]->classInfo->ClassName();
			}
			return;
		}
	}
}

/* Get name of all instances */
void ClassDir::InstancePointers(char *category, char *className,
	int &num, char **&instanceNames){
	int i,j, k;
	num =0 ;
	instanceNames = argArray;
	for (i=0; i < _numCategories; i++){
		CategoryRec *catRec = _categories[i];
		if (strcmp(catRec->name,category)== 0){
			for (j=0; j < catRec->_numClasses; j++){
				ClassRec *classRec = catRec->_classRecs[j];
				if (strcmp(classRec->classInfo->ClassName(), className) == 0) {
					for (k=0; k < classRec->_numInstances; k++){
						argArray[num++] = (char *)
							classRec->_instances[k]->GetInstance();
					}
					return;
				}
			}
		}
	}
}
/* Get name of all instances */
void ClassDir::InstanceNames(char *category, char *className,
	int &num, char **&instanceNames){
	int i,j, k;
	num =0 ;
	instanceNames = argArray;
	for (i=0; i < _numCategories; i++){
		CategoryRec *catRec = _categories[i];
		if (strcmp(catRec->name,category)== 0){
			for (j=0; j < catRec->_numClasses; j++){
				ClassRec *classRec = catRec->_classRecs[j];
				if (strcmp(classRec->classInfo->ClassName(), className) == 0) {
					for (k=0; k < classRec->_numInstances; k++){
						argArray[num++] = 
							classRec->_instances[k]->InstanceName();
					}
					return;
				}
			}
		}
	}
}

/* Get creation parameters for a class */
void ClassDir::GetParams(char *category, char *className, 
	int &numParams, char **&params){
	int i,j;
	for (i=0; i < _numCategories; i++){
		CategoryRec *catRec = _categories[i];
		if (strcmp(catRec->name,category)== 0){
			for (j=0; j < catRec->_numClasses; j++){
				ClassRec *classRec = catRec->_classRecs[j];
				if (strcmp(classRec->classInfo->ClassName(),className)== 0){
					classRec->classInfo->ParamNames(numParams, params);
					return;
				}
			}
		}
	}
	/* not found */
	numParams = 0;
	params = NULL;
}

/* Set default values for class parameters*/
void ClassDir::SetDefault(char *category, char *className,
			int numParams, char **params){
	int i,j;
	for (i=0; i < _numCategories; i++){
		CategoryRec *catRec = _categories[i];
		if (strcmp(catRec->name,category)== 0){
			for (j=0; j < catRec->_numClasses; j++){
				ClassRec *classRec = catRec->_classRecs[j];
				if (strcmp(classRec->classInfo->ClassName(),className)== 0){
					classRec->classInfo->SetDefaultParams(numParams, params);
					return ;
				}
			}
		}
	}
	/* not found */
	return ;
}

/* Create a new instances given the parameters */
char *ClassDir::CreateWithParams(char *category, char *className,
			int numParams, char **paramNames){
	int i,j;
	for (i=0; i < _numCategories; i++){
		CategoryRec *catRec = _categories[i];
		if (strcmp(catRec->name,category)== 0){
			for (j=0; j < catRec->_numClasses; j++){
				ClassRec *classRec = catRec->_classRecs[j];
				if (strcmp(classRec->classInfo->ClassName(),className)== 0){
					if (classRec->_numInstances >= MaxInstances){
						fprintf(stderr,"ClassDir::CreateWithParams:MaxInstn\n");
						Exit::DoExit(1);
					}
					ClassInfo *iInfo = 
						classRec->classInfo->CreateWithParams(numParams,
						paramNames);
					if (iInfo != NULL){
						classRec->_instances[classRec->_numInstances++] = iInfo;
						return iInfo->InstanceName();
					}
					else return NULL;
				}
			}
		}
	}
	/* not found */
	return NULL;
}

/* Find instance with given name */
void *ClassDir::FindInstance(char *name){
/*
printf("FindInstance '%s'\n");
*/
	int i,j,k;
	for (i=0; i < _numCategories; i++){
		CategoryRec *catRec = _categories[i];
		for (j=0; j < catRec->_numClasses; j++){
			ClassRec *classRec = catRec->_classRecs[j];
			for (k=0; k < classRec->_numInstances; k++){
/*
printf("compare with '%s'\n",classRec->_instances[k]->InstanceName());
*/
				if (strcmp(classRec->_instances[k]->InstanceName(), name) == 0)
					return classRec->_instances[k]->GetInstance();
			}
		}
	}
	return NULL;
}

/* Destroy instance */
void ClassDir::DestroyInstance(char *name){
/*
printf("DestroyInstance '%s'\n");
*/
	int i,j,k,l;
	for (i=0; i < _numCategories; i++){
		CategoryRec *catRec = _categories[i];
		for (j=0; j < catRec->_numClasses; j++){
			ClassRec *classRec = catRec->_classRecs[j];
			for (k=0; k < classRec->_numInstances; k++){
					ClassInfo *instRec = classRec->_instances[k];
/*
printf("compare with '%s'\n",instRec->InstanceName());
*/
				if (strcmp(instRec->InstanceName(), name) == 0){
					/*
					printf("now destroying %s\n", name);
					*/
					delete instRec;
					for (l = k; l < classRec->_numInstances; l++){
						classRec->_instances[l] = classRec->_instances[l+1];
					}
					classRec->_numInstances--;
					return ;
				}
			}
		}
	}
}

/* Destroy all transient classes */
void ClassDir::DestroyTransientClasses(){
	int i,j,k,l;
	for (i=0; i < _numCategories; i++){
		CategoryRec *catRec = _categories[i];
		j=0;
		while (j < catRec->_numClasses){
			ClassRec *classRec = catRec->_classRecs[j];
			if (classRec->classInfo->IsTransient()){
				for (k=0; k < classRec->_numInstances; k++){
					ClassInfo *instRec = classRec->_instances[k];
					delete instRec;
				}
				delete classRec->classInfo;
				for (l=j; l < catRec->_numClasses; l++){
					catRec->_classRecs[l] = catRec->_classRecs[l+1];
				}
				catRec->_numClasses--;
			}
			else j++;
		}
	}
}

/* Get creation parameter for instance */
void ClassDir::CreateParams(char *category, char *className, char *instanceName,
	int &numParams, char **&params){
	int i,j, k;
	for (i=0; i < _numCategories; i++){
		CategoryRec *catRec = _categories[i];
		if (strcmp(catRec->name,category)== 0){
			for (j=0; j < catRec->_numClasses; j++){
				ClassRec *classRec = catRec->_classRecs[j];
				if (strcmp(classRec->classInfo->ClassName(), className) == 0) {
					for (k=0; k < classRec->_numInstances; k++){
						ClassInfo *iInfo = classRec->_instances[k];
						if (strcmp(iInfo->InstanceName(), instanceName) == 0){
							/* found */
							iInfo->CreateParams(numParams, params);
							return;
						}
					}
				}
			}
		}
	}
	fprintf(stderr,"ClassDir::CreateParams: instance %s not found \n",
		instanceName);
	numParams = 0;
	params = NULL;
}

ClassInfo::ClassInfo(){
	_defaultParams = NULL;
	_numDefaultParams = 0;
}

ClassInfo::~ClassInfo(){
	/*
	printf("ClassInfo destructor\n");
	*/
}

char *ClassInfo::InstanceName(){
	fprintf(stderr,"ClassInfo::InstanceName(): should not be called\n");
	Exit::DoExit(1);
	return NULL; /* to keep compiler happy*/
}
void *ClassInfo::GetInstance(){
	fprintf(stderr,"ClassInfo::GetInstance(): should not be called\n");
	Exit::DoExit(1);
	return NULL; /* to keep compiler happy*/
}

/* Get parameters that can be used to re-create this instance */
void ClassInfo::CreateParams(int &argc, char **&argv){
	fprintf(stderr,"ClassInfo::CreateParams(): should not be called\n");
	Exit::DoExit(1);
}

/* Set default parameters */
void ClassInfo::SetDefaultParams(int argc, char **argv){
	if (argc == 0)
		return;

	int i;
	if (_defaultParams != NULL){
		/* free parameters */
		for (i=0; i < _numDefaultParams; i++){
			delete _defaultParams[i];
			delete _defaultParams;
		}
	}
	_defaultParams = new char *[argc];
	_numDefaultParams = argc;
	for (i=0; i < argc; i++){
		_defaultParams[i] = CopyString(argv[i]);
	}
}

/* Get default parameters */
void ClassInfo::GetDefaultParams(int &argc, char **&argv){
	argc = _numDefaultParams;
	argv = _defaultParams;
}

/* Find instance with given name */
Boolean ClassDir::Changeable(char *name){
	int i,j,k;
	for (i=0; i < _numCategories; i++){
		CategoryRec *catRec = _categories[i];
		for (j=0; j < catRec->_numClasses; j++){
			ClassRec *classRec = catRec->_classRecs[j];
			for (k=0; k < classRec->_numInstances; k++){
				if (strcmp(classRec->_instances[k]->InstanceName(), name) == 0)
					return classRec->_instances[k]->Changeable();
			}
		}
	}
	return false;
}
 /* Change with params */
void ClassDir::ChangeParams(char *name, int num, char **paramNames){
	int i,j,k;
	for (i=0; i < _numCategories; i++){
		CategoryRec *catRec = _categories[i];
		for (j=0; j < catRec->_numClasses; j++){
			ClassRec *classRec = catRec->_classRecs[j];
			for (k=0; k < classRec->_numInstances; k++){
				if (strcmp(classRec->_instances[k]->InstanceName(), name) == 0)
					classRec->_instances[k]->ChangeParams(num, paramNames);
			}
		}
	}
}

 /* Change with params */
void ClassDir::GetParams(char *name, int &num, char **&paramNames){
	int i,j,k;
	for (i=0; i < _numCategories; i++){
		CategoryRec *catRec = _categories[i];
		for (j=0; j < catRec->_numClasses; j++){
			ClassRec *classRec = catRec->_classRecs[j];
			for (k=0; k < classRec->_numInstances; k++){
				if (strcmp(classRec->_instances[k]->InstanceName(), name) == 0){
					classRec->_instances[k]->ParamNames(num, paramNames);
					return;
				}
			}
		}
	}
}

/* Get user info for a class */
void *ClassDir::UserInfo(char *category, char *className){
	int i,j;
	for (i=0; i < _numCategories; i++){
		CategoryRec *catRec = _categories[i];
		if (strcmp(catRec->name,category)== 0){
			for (j=0; j < catRec->_numClasses; j++){
				ClassRec *classRec = catRec->_classRecs[j];
				if (strcmp(classRec->classInfo->ClassName(),className)== 0){
					return classRec->classInfo->UserInfo();
				}
			}
		}
	}
	return NULL;
}

/* Print name of all instances */
void ClassDir::Print(){
	int i,j, k;
	printf("Classes are:\n");
	for (i=0; i < _numCategories; i++){
		CategoryRec *catRec = _categories[i];
		for (j=0; j < catRec->_numClasses; j++){
			ClassRec *classRec = catRec->_classRecs[j];
			for (k=0; k < classRec->_numInstances; k++){
				printf("category:%s class:%s instance:%s\n",
					catRec->name,
					classRec->classInfo->InstanceName(),
					classRec->_instances[k]->InstanceName());
			}
		}
	}
}

/* Find name of instance */
char *ClassDir::FindInstanceName(void *inst){
	int i,j,k;
	for (i=0; i < _numCategories; i++){
		CategoryRec *catRec = _categories[i];
		for (j=0; j < catRec->_numClasses; j++){
			ClassRec *classRec = catRec->_classRecs[j];
			for (k=0; k < classRec->_numInstances; k++){
/*
printf("compare with '%s'\n",classRec->_instances[k]->InstanceName());
*/
				if (classRec->_instances[k]->GetInstance()==inst)
					return classRec->_instances[k]->InstanceName();
			}
		}
	}
	return NULL;
}

/*
  ========================================================================
  DEVise Data Visualization Software
  (c) Copyright 1992-1996
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
  Revision 1.3  1996/04/14 00:15:35  jussi
  Added copyright notice and cleaned up the code quite a bit.
  Fixed bug in Print() method.

  Revision 1.2  1995/09/05 21:12:26  jussi
  Added/updated CVS header.
*/

#include <stdio.h>

#include "ClassDir.h"
#include "Exit.h"
#include "Util.h"

//#define DEBUG

ClassDir::ClassDir()
{
  _numCategories = 0;
}

void ClassDir::InsertCategory(char *name)
{
  if (_numCategories >= MaxCategories) {
    fprintf(stderr,"ClassDir::InsertCategory\n");
    Exit::DoExit(1);
  }

  CategoryRec *rec = new CategoryRec;
  rec->name = name;
  rec->_numClasses = 0;
  _categories[_numCategories++] = rec;
}

void ClassDir::InsertClass(ClassInfo *cInfo)
{
  for(int i = 0; i < _numCategories; i++) {
    CategoryRec *catRec = _categories[i];
    if (!strcmp(catRec->name,cInfo->CategoryName())) {
      /* found */
      if (catRec->_numClasses >= MaxClasses) {
	fprintf(stderr, "ClassDir::InsertClass MaxClass\n");
	Exit::DoExit(1);
      }
      ClassRec *classRec = new ClassRec;
      classRec->classInfo = cInfo;
      classRec->_numInstances = 0;
      catRec->_classRecs[catRec->_numClasses++] = classRec;
      return;
    }
  }

  fprintf(stderr, "ClassDir::InsertClass\n");
  Exit::DoExit(1);
}

char *argArray[512];

void ClassDir::ClassNames(char *category, int &num, char **&names)
{
  num = 0;
  names = argArray;

  for(int i = 0; i < _numCategories; i++) {
    CategoryRec *catRec = _categories[i];
    if (!strcmp(catRec->name, category)) {
      for(int j = 0; j < catRec->_numClasses; j++)
	argArray[num++] = catRec->_classRecs[j]->classInfo->ClassName();
      return;
    }
  }
}

/* Get pointers to all instances */

void ClassDir::InstancePointers(char *category, char *className,
				int &num, char **&instanceNames)
{
  num = 0;
  instanceNames = argArray;

  for(int i = 0; i < _numCategories; i++) {
    CategoryRec *catRec = _categories[i];
    if (!strcmp(catRec->name, category)) {
      for(int j = 0; j < catRec->_numClasses; j++) {
	ClassRec *classRec = catRec->_classRecs[j];
	if (!strcmp(classRec->classInfo->ClassName(), className)) {
	  for(int k = 0; k < classRec->_numInstances; k++)
	    argArray[num++] = (char *)classRec->_instances[k]->GetInstance();
	  return;
	}
      }
    }
  }
}

/* Get names of all instances */

void ClassDir::InstanceNames(char *category, char *className,
			     int &num, char **&instanceNames)
{
  num = 0;
  instanceNames = argArray;

  for(int i = 0; i < _numCategories; i++) {
    CategoryRec *catRec = _categories[i];
    if (!strcmp(catRec->name,category)) {
      for(int j = 0; j < catRec->_numClasses; j++) {
	ClassRec *classRec = catRec->_classRecs[j];
	if (!strcmp(classRec->classInfo->ClassName(), className)) {
	  for(int k = 0; k < classRec->_numInstances; k++)
	    argArray[num++] = classRec->_instances[k]->InstanceName();
	  return;
	}
      }
    }
  }
}

/* Get creation parameters for a class */

void ClassDir::GetParams(char *category, char *className, 
			 int &numParams, char **&params)
{
  for(int i = 0; i < _numCategories; i++) {
    CategoryRec *catRec = _categories[i];
    if (!strcmp(catRec->name,category)) {
      for(int j = 0; j < catRec->_numClasses; j++) {
	ClassRec *classRec = catRec->_classRecs[j];
	if (!strcmp(classRec->classInfo->ClassName(), className)) {
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
			  int numParams, char **params)
{
  for(int i = 0; i < _numCategories; i++) {
    CategoryRec *catRec = _categories[i];
    if (!strcmp(catRec->name,category)) {
      for(int j = 0; j < catRec->_numClasses; j++) {
	ClassRec *classRec = catRec->_classRecs[j];
	if (!strcmp(classRec->classInfo->ClassName(),className)) {
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
				 int numParams, char **paramNames)
{
  for(int i = 0; i < _numCategories; i++) {
    CategoryRec *catRec = _categories[i];
    if (!strcmp(catRec->name,category)) {
      for(int j = 0; j < catRec->_numClasses; j++) {
	ClassRec *classRec = catRec->_classRecs[j];
	if (!strcmp(classRec->classInfo->ClassName(),className)) {
	  if (classRec->_numInstances >= MaxInstances) {
	    fprintf(stderr,"ClassDir::CreateWithParams:MaxInstn\n");
	    Exit::DoExit(1);
	  }
	  ClassInfo *iInfo = classRec->classInfo->CreateWithParams(numParams,
								   paramNames);
	  if (iInfo) {
	    classRec->_instances[classRec->_numInstances++] = iInfo;
	    return iInfo->InstanceName();
	  }
	  fprintf(stderr,
		  "Cannot create instance in class %s of category %s\n",
		  className, category);
	  return NULL;
	}
      }
      fprintf(stderr, "Cannot find class %s in category %s\n",
	      className, category);
      return NULL;
    }
  }

  fprintf(stderr, "Cannot find category %s\n", category);
  return NULL;
}

/* Find instance with given name */

void *ClassDir::FindInstance(char *name)
{
  for(int i = 0; i < _numCategories; i++) {
    CategoryRec *catRec = _categories[i];
    for(int j = 0; j < catRec->_numClasses; j++) {
      ClassRec *classRec = catRec->_classRecs[j];
      for(int k = 0; k < classRec->_numInstances; k++) {
	if (!strcmp(classRec->_instances[k]->InstanceName(), name))
	  return classRec->_instances[k]->GetInstance();
      }
    }
  }

  return NULL;
}

/* Destroy instance */

void ClassDir::DestroyInstance(char *name)
{
  for(int i = 0; i < _numCategories; i++) {
    CategoryRec *catRec = _categories[i];
    for(int j = 0; j < catRec->_numClasses; j++) {
      ClassRec *classRec = catRec->_classRecs[j];
      for(int k = 0; k < classRec->_numInstances; k++) {
	ClassInfo *instRec = classRec->_instances[k];
	if (!strcmp(instRec->InstanceName(), name)) {
	  /*
	     printf("now destroying %s\n", name);
	  */
	  delete instRec;
	  for(int l = k; l < classRec->_numInstances - 1; l++) {
	    classRec->_instances[l] = classRec->_instances[l + 1];
	  }
	  classRec->_numInstances--;
	  return;
	}
      }
    }
  }
}

/* Destroy all transient classes */

void ClassDir::DestroyTransientClasses()
{
#ifdef DEBUG
  printf("Before DestroyTransientClasses, ClassDir shows:\n");
  Print();
#endif

  for(int i = 0; i < _numCategories; i++) {
    CategoryRec *catRec = _categories[i];
    int j = 0;
    while (j < catRec->_numClasses) {
      ClassRec *classRec = catRec->_classRecs[j];
      if (!classRec->classInfo->IsTransient()) {
	j++;
	continue;
      }
#ifdef DEBUG
      printf("Now destroying category %s, class %s\n", catRec->name,
	     classRec->classInfo->ClassName());
#endif
      for(int k = 0; k < classRec->_numInstances; k++) {
	ClassInfo *instRec = classRec->_instances[k];
#ifdef DEBUG
	printf("  Destroying instance %s\n", instRec->InstanceName());
#endif
	delete instRec;
      }
#ifdef DEBUG
      printf("  Destroying class %s\n", classRec->classInfo->ClassName());
#endif
      delete classRec->classInfo;
      for(int l = j; l < catRec->_numClasses - 1; l++) {
	catRec->_classRecs[l] = catRec->_classRecs[l + 1];
      }
      catRec->_numClasses--;
    }
  }
}

/* Get creation parameter for instance */

void ClassDir::CreateParams(char *category, char *className,
			    char *instanceName, 
			    int &numParams, char **&params)
{
  for(int i = 0; i < _numCategories; i++) {
    CategoryRec *catRec = _categories[i];
    if (!strcmp(catRec->name,category)) {
      for(int j = 0; j < catRec->_numClasses; j++) {
	ClassRec *classRec = catRec->_classRecs[j];
	if (!strcmp(classRec->classInfo->ClassName(), className)) {
	  for(int k = 0; k < classRec->_numInstances; k++) {
	    ClassInfo *iInfo = classRec->_instances[k];
	    if (!strcmp(iInfo->InstanceName(), instanceName)) {
	      /* found */
	      iInfo->CreateParams(numParams, params);
	      return;
	    }
	  }
	}
      }
    }
  }

  fprintf(stderr,"Instance %s not found\n", instanceName);

  numParams = 0;
  params = NULL;
}

ClassInfo::ClassInfo()
{
  _defaultParams = NULL;
  _numDefaultParams = 0;
}

ClassInfo::~ClassInfo()
{
  /*
     printf("ClassInfo destructor\n");
  */
}

char *ClassInfo::InstanceName()
{
  fprintf(stderr,"ClassInfo::InstanceName(): should not be called\n");
  Exit::DoExit(1);
  return NULL; /* to keep compiler happy*/
}

void *ClassInfo::GetInstance()
{
  fprintf(stderr,"ClassInfo::GetInstance(): should not be called\n");
  Exit::DoExit(1);
  return NULL; /* to keep compiler happy*/
}

/* Get parameters that can be used to re-create this instance */

void ClassInfo::CreateParams(int &argc, char **&argv)
{
  fprintf(stderr,"ClassInfo::CreateParams(): should not be called\n");
  Exit::DoExit(1);
}

/* Set default parameters */

void ClassInfo::SetDefaultParams(int argc, char **argv)
{
  if (argc == 0)
    return;

  if (_defaultParams) {
    /* free parameters */
    for(int i = 0; i < _numDefaultParams; i++) {
      delete _defaultParams[i];
      delete _defaultParams;
    }
  }

  _defaultParams = new char *[argc];
  _numDefaultParams = argc;
  for(int i = 0; i < argc; i++) {
    _defaultParams[i] = CopyString(argv[i]);
  }
}

/* Get default parameters */

void ClassInfo::GetDefaultParams(int &argc, char **&argv)
{
  argc = _numDefaultParams;
  argv = _defaultParams;
}

/* Find instance with given name */

Boolean ClassDir::Changeable(char *name)
{
  for(int i = 0; i < _numCategories; i++) {
    CategoryRec *catRec = _categories[i];
    for(int j = 0; j < catRec->_numClasses; j++) {
      ClassRec *classRec = catRec->_classRecs[j];
      for(int k = 0; k < classRec->_numInstances; k++) {
	if (!strcmp(classRec->_instances[k]->InstanceName(), name))
	  return classRec->_instances[k]->Changeable();
      }
    }
  }

  return false;
}

/* Change params */

void ClassDir::ChangeParams(char *name, int num, char **paramNames)
{
  for(int i = 0; i < _numCategories; i++) {
    CategoryRec *catRec = _categories[i];
    for(int j = 0; j < catRec->_numClasses; j++) {
      ClassRec *classRec = catRec->_classRecs[j];
      for(int k = 0; k < classRec->_numInstances; k++) {
	if (!strcmp(classRec->_instances[k]->InstanceName(), name))
	  classRec->_instances[k]->ChangeParams(num, paramNames);
      }
    }
  }
}

/* Get params */

void ClassDir::GetParams(char *name, int &num, char **&paramNames)
{
  for(int i = 0; i < _numCategories; i++) {
    CategoryRec *catRec = _categories[i];
    for(int j = 0; j < catRec->_numClasses; j++) {
      ClassRec *classRec = catRec->_classRecs[j];
      for(int k = 0; k < classRec->_numInstances; k++) {
	if (!strcmp(classRec->_instances[k]->InstanceName(), name)) {
	  classRec->_instances[k]->ParamNames(num, paramNames);
	  return;
	}
      }
    }
  }
}

/* Get user info for a class */

void *ClassDir::UserInfo(char *category, char *className)
{
  for(int i = 0; i < _numCategories; i++) {
    CategoryRec *catRec = _categories[i];
    if (!strcmp(catRec->name,category)) {
      for(int j = 0; j < catRec->_numClasses; j++) {
	ClassRec *classRec = catRec->_classRecs[j];
	if (!strcmp(classRec->classInfo->ClassName(), className)) {
	  return classRec->classInfo->UserInfo();
	}
      }
    }
  }

  return NULL;
}

/* Print name of all instances */

void ClassDir::Print()
{
  printf("%d categories\n", _numCategories);
  for(int i = 0; i < _numCategories; i++) {
    CategoryRec *catRec = _categories[i];
    printf("Category %s: %d classes\n", catRec->name, catRec->_numClasses);
    for(int j = 0; j < catRec->_numClasses; j++) {
      ClassRec *classRec = catRec->_classRecs[j];
      printf("  Class %s: %d instances\n", classRec->classInfo->ClassName(),
	     classRec->_numInstances);
      for(int k = 0; k < classRec->_numInstances; k++) {
	printf("    %s\n", classRec->_instances[k]->InstanceName());
      }
    }
  }
}

/* Find name of instance */

char *ClassDir::FindInstanceName(void *inst)
{
  for(int i = 0; i < _numCategories; i++) {
    CategoryRec *catRec = _categories[i];
    for(int j = 0; j < catRec->_numClasses; j++) {
      ClassRec *classRec = catRec->_classRecs[j];
      for(int k = 0; k < classRec->_numInstances; k++) {
	if (classRec->_instances[k]->GetInstance() == inst)
	  return classRec->_instances[k]->InstanceName();
      }
    }
  }

  return NULL;
}

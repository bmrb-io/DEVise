#ifndef ViewClassInfo_h
#define ViewClassInfo_h
#include "ClassDir.h"

class TDataViewX;
class View;
class ViewXInfo: public ClassInfo {
public:
	ViewXInfo(); /* class constructor */
	ViewXInfo(char *name, char *bgName,
		TDataViewX *view); /* instance constructor */
	virtual ~ViewXInfo();

	/************** Class Info ******************************/
    virtual char *CategoryName() { return "view"; }
	virtual char *ClassName() { return "SortedX"; }

	/* Get name of parameters */
	 virtual void ParamNames(int &argc, char **&argv);

	virtual ClassInfo *CreateWithParams(int argc, char **argv);

	/****************** Instance Info *****************************/
    virtual char *InstanceName();
	virtual void *GetInstance();

	/* Get parameters that can be used to re-create this instance */
	virtual void CreateParams(int &argc, char **&argv);

private:
	char *arg[6];
	TDataViewX *_view;
	char *_name;
	char *_bgColorName;
};

class ViewScatter;
class ViewScatterInfo : public ClassInfo {
public:
	ViewScatterInfo(); /* class constructor */
	ViewScatterInfo(char *name, 
		char *bgName, ViewScatter *view); /* instance constructor*/
	virtual ~ViewScatterInfo();

	/************* Class Info ***********************/
    virtual char *CategoryName() { return "view"; }
	virtual char *ClassName() { return "Scatter"; }

	/* Get name of parameters */
	 virtual void ParamNames(int &argc, char **&argv);

	 virtual ClassInfo *CreateWithParams(int argc, char **argv);

	 /************** Instance Info *****************************/
	 virtual char *InstanceName();
	 virtual void *GetInstance();

	 /* Get parameters that can be used to re-create this instance */
	 virtual void CreateParams(int &argc, char **&argv);

private:
char *arg[6];
ViewScatter *_view;
char *_name;
char *_bgName;
};

#endif

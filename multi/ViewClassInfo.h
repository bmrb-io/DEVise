#ifndef ViewClassInfo_h
#define ViewClassInfo_h
#include "ClassDir.h"

class TDataViewX;

class View;

class ViewScatter;
class ViewScatterInfo : public ClassInfo {
public:
	ViewScatterInfo(); /* class constructor */
	ViewScatterInfo(char *name, ViewScatter *view); /* instance constructor*/

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
};

#endif

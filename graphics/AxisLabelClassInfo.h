/*
  $Id$

  $Log$*/

#ifndef AxisLabelClassInfo_h
#define AxisLabelClassInfo_h
#include "DeviseTypes.h"
#include "ClassDir.h"

class AxisLabel;
class GenAxisLabel;
class AxisLabelClassInfo: public ClassInfo {
public:
	AxisLabelClassInfo(char *className, GenAxisLabel *gen);
	AxisLabelClassInfo(char *className, char *instName, AxisLabel *label);
	virtual ~AxisLabelClassInfo();

	/* Info for category */
	virtual char *CategoryName() { return "axisLabel";}

	/* Info for class */
	virtual char *ClassName(); 	/* name of class */

	/* Get name of parameters and default/current values */
	virtual void ParamNames(int &argc, char **&argv);

	/* Create instance using the supplied parameters. Return 
	the instance info if successful, otherwise return NULL. */
	virtual ClassInfo *CreateWithParams(int argc, char **argv);

	/* Return true is parameters can be changed dynamically at run time */
	virtual Boolean Changeable() { return true;}

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
private:
	GenAxisLabel *_gen;
	char *_className, *_instName;
	AxisLabel *_label;
};

#endif

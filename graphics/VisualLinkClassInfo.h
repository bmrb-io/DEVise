/* VisualLinkClassInfo.h */
#ifndef VisualLinkClassInfo_h
#define VisualLinkClassInfo_h
#include "ClassDir.h"
#include "VisualArg.h"


class VisualLink;
class VisualLinkClassInfo: public ClassInfo {
public:
	/* specify VISUAL_X, VISUAL_Y, VISUAL_LOC */
	VisualLinkClassInfo();
	VisualLinkClassInfo(char *name, VisualFlag flag, VisualLink *link);
	virtual ~VisualLinkClassInfo();

	/* Info for category */
	virtual char *CategoryName() { return "link"; } 

	/* Info for class */
	virtual char *ClassName(); 	/* name of class */

	/* Get name of parameters and default/current values */
	virtual void ParamNames(int &argc, char **&argv);

	/* Create instance using the supplied parameters. Return 
	the instance info if successful, otherwise return NULL. */
	virtual ClassInfo *CreateWithParams(int argc, char **argv) ;

	/* Return true is parameters can be changed dynamically at run time */
	virtual Boolean Changeable() { return false; }

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
	VisualFlag _flag;
	char *_name;
	VisualLink *_link;
};

#endif

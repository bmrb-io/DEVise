/* CursorClassInfo.h */
#ifndef CursorClassInfo_h
#define CursorClassInfo_h
#include "ClassDir.h"
#include "VisualArg.h"


class DeviseCursor;
class CursorClassInfo: public ClassInfo {
public:
	/* specify VISUAL_X, VISUAL_Y, VISUAL_LOC */
	CursorClassInfo();
	CursorClassInfo(char *name, VisualFlag flag, DeviseCursor *cursor);
	virtual ~CursorClassInfo();

	/* Info for category */
	virtual char *CategoryName() { return "cursor"; } 

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
	DeviseCursor *_cursor;
};

#endif

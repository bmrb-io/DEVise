/*
  $Id$

  $Log$*/

/* Window class info */

#ifndef WinClassInfo_h
#define  WinClassInfo_h
#include "ClassDir.h"

class ViewWinVer;
class WinVerInfo: public ClassInfo {
public:
	WinVerInfo();	/* class constructor */
	WinVerInfo(char *name, ViewWinVer *win); /* instance constructor */
	virtual ~WinVerInfo();	/* class constructor */

	/*********** Class methods **************************/
    virtual char *CategoryName() { return "window"; }
	virtual char *ClassName() { return "WinVertical"; }

	/* Get name of parameters */
	virtual void ParamNames(int &argc, char **&argv) ;

	/* Create instance using the supplied parameters. Return
	the instance info if successful, otherwise return NULL. */
	virtual ClassInfo *CreateWithParams(int argc, char **argv);

	/************* Instance Methods ***************************/
	virtual char *InstanceName();
	virtual void *GetInstance();

	/* Get parameters that can be used to re-create this instance */
	virtual void CreateParams(int &argc, char **&argv);

private:
	char *arg[5];
	char *_name;
	ViewWinVer *_win;
};

#ifdef WIN_HOR
class WinHorInfo: public ClassInfo {
    virtual char *CategoryName() { return "window"; }
	virtual char *ClassName() { return "WinHorizontal"; }

	/* Get name of parameters */
	virtual void ParamNames(int &argc, char **&argv) {
		argc = 5;
		argv = arg;
		arg[0] = "name";
		arg[1] = "x";
		arg[2] = "y";
		arg[3] = "width";
		arg[4] = "height";
	}

	/* Create instance using the supplied parameters. Return
	the instance info if successful, otherwise return NULL. */
	virtual InstanceInfo *CreateWithParams(int argc, char **argv);
private:
	char *arg[5];

};
#endif


#endif

/* ViewClassInfo.c */
#include <sys/types.h>
#include <sys/time.h>
#include <time.h>
#include <stdio.h>
#include "VisualArg.h"
#include "TDataViewX.h"
#include "ViewScatter.h"
#include "ViewClassInfo.h"
#include "Selection.h"
#include "Control.h"
#include "Util.h"
#include "ActionDefault.h"
#include "VisualLink.h"
#include "DispQueryProc.h"
#include "Message.h"
#include "TDataMap.h"

extern "C" double trunc(double);

QueryProc *GetQueryProc(){
static QueryProc *qp = NULL;
    int bufSize;
	Boolean prefetch, existing;
	BufPolicy::policy policy;
	Init::BufPolicies(bufSize, prefetch, policy, existing);

	if (qp == NULL){
/*		qp = new DispQueryProcFull(prefetch,policy,existing); */
		qp = new DispQueryProcFull;
	}
	return qp;
}

class MyXAxisLabel: public AxisCallback {
public:
    virtual char *GenerateLabel(Coord val){
        /* Convert into # of hours */
        /*
        int clock = (int)val;
        int day = clock/(3600*24);
        clock -= day*(3600*24);
        int hour = clock/3600;
        clock -= hour*3600;
        int min = clock/60;
        int sec = clock % 60;
        sprintf(buf,"%d:%02d:%02d:%02d", day, hour, min, sec);
        */
        sprintf(buf,"%.2f",val);
        return buf;
    }
private:
    char buf[80];
};


class MyYAxisLabel: public AxisCallback {
public:
	virtual char *GenerateLabel(Coord val){
		/* Convert into # of days */
		sprintf(buf,"%.1f", val);
		return buf;
	}
private:
	char buf[80];
};

class MultiAction: public ActionDefault1 {
public:
    MultiAction(Coord leftEdge = 0.0, 
	Boolean useLeftFlag = false,
        Coord rightEdge = 0.0, Boolean useRightFlag = false):
    ActionDefault1(leftEdge, useLeftFlag, rightEdge,
            useRightFlag){}

    virtual Boolean PopUp(View *view, Coord x, Coord y,
		int /* button*/, char **& msgs, int & numMsgs){
		/* just do 1st tdata */
		ViewGraph *vg = (ViewGraph *)view;
		for (vg->InitMappingIterator(); vg->MoreMapping(); ){
			TDataMap *map = vg->NextMapping();
	        int time = (int)trunc(x);
	        int node = (int)trunc(y);
	        GenMessage(map->GetTData(),time, node, numMsgs, msgs);
			break;
		}
		vg->DoneMappingIterator();

        return true;
    }
private:
TData *tdata;
};

ViewScatterInfo::ViewScatterInfo(){}

/* instance constructor */
ViewScatterInfo::ViewScatterInfo(char *name, ViewScatter *view){
	_name = name;
	_view = view;
}

static char buf1[80], buf2[80], buf3[80], buf4[80], buf5[80];
void ViewScatterInfo::ParamNames(int &argc, char **&argv){
	 argc = 5;
	 argv = arg;
	 arg[0] = buf1;
	 arg[1] = buf2;
	 arg[2] = buf3;
	 arg[3] = buf4;
	 arg[4] = buf5;

	 sprintf(buf1,"name %s", ControlPanel::Instance()->ViewName());

	int numDefaults;
	char **defaults;
	GetDefaultParams(numDefaults, defaults);
	if (numDefaults == 4){
		sprintf(arg[1],"xlow %s",defaults[0]);
		sprintf(arg[2],"xhigh %s",defaults[1]);
		sprintf(arg[3],"ylow %s",defaults[2]);
		sprintf(arg[4],"yhigh %s",defaults[3]);
	}
	else {
		 arg[1] = "xlow -10.0";
		 arg[2] = "xhigh 1000.0";
		 arg[3] = "ylow -10.0";
		 arg[4] = "yhigh 150.0";
	}
}


ClassInfo *ViewScatterInfo::CreateWithParams(int argc, char **argv){
	if ( argc != 5){
		fprintf(stderr,"ViewScatterInfo::CreateWithParams wrong args\n");
		return NULL;
	}
	VisualFilter filter;
	filter.xLow = atof(argv[1]);
	filter.xHigh = atof(argv[2]);
	filter.yLow = atof(argv[3]);
	filter.yHigh  = atof(argv[4]);
	filter.flag = VISUAL_LOC;

char buf[80];
static sel_id = 0;
static VisualLink *scatterLink = NULL;
static MultiAction *multiAct = NULL;
static ActionDefault1 *hostAct = NULL;
static AxisCallback *yLabel= NULL;

	char *name = CopyString(argv[0]);
	Action *act;
	sprintf(buf,"sel_%d",sel_id++);
	int numDefaults;
	char **defaults;
	GetDefaultParams(numDefaults, defaults);
	Coord xlow;
	if (numDefaults == 4){
		xlow = atof(defaults[0]);
	}
	else
		xlow = 0.0;

	/* Use different actions for host/muti mappings */
	Boolean isMulti = (strstr(name, "Multi")!=NULL) ;
	if (isMulti){
		if (multiAct == NULL)
			multiAct = new MultiAction(xlow,true);
		act = multiAct;
	}
	else {
		if (hostAct == NULL)
			hostAct = new ActionDefault1(xlow, true);
		act = hostAct;
	}

	if (scatterLink == NULL){
		scatterLink = new VisualLink(CopyString(buf),VISUAL_X);
		yLabel = new MyYAxisLabel();
	}
	ViewScatter *view = new ViewScatter(name,filter, GetQueryProc(), 
		yLabel, yLabel, act);
	scatterLink->InsertView(view);
	return new ViewScatterInfo(name, view);
}

char *ViewScatterInfo::InstanceName(){
	return _name;
}

void *ViewScatterInfo::GetInstance(){
	return _view;
}

/* Get parameters that can be used to re-create this instance */
void ViewScatterInfo::CreateParams(int &argc, char **&argv){
	 argc = 5;
	 argv = arg;
	 arg[0] = _name;
	 VisualFilter *filter = _view->GetVisualFilter();

	 arg[1] = buf2;
	 sprintf(buf2, "%f", filter->xLow);;

	 arg[2] = buf3;
	 sprintf(buf3, "%f", filter->xHigh);

	 arg[3] = buf4;
	 sprintf(buf4,"%f", filter->yLow);

	 arg[4] = buf5;
	 sprintf(buf5,"%f", filter->yHigh);
}

/*
  $Id$

  $Log$
  Revision 1.2  1995/09/05 22:16:50  jussi
  Added CVS header.
*/

/* displays string data */

#include <sys/time.h>
#include "DeviseTypes.h"
#include "Dispatcher.h"
#include "RectShape.h"
#include "ColorMgr.h"
#include "Control.h"
#include "TDataCMap.h"
#include "Connector.h"
#include "ActionDefault.h"
#include "Selection.h"
#include "VisualArg.h"
#include "Init.h"
//#include "TDataMapDispatch.h"
//#include "DispQueryProc.h"
#include "BufMgr.h"
//#include "TDataRec.h"
//#include "ViewScatter.h"
#include "Command.h"
//#include "StrData.h"
#include "Temp.h"
#include "ViewWinVer.h"
//#include "StringHash.h"
//#include "Snapshot.h"

int debug = 0;
Color red;

#include "StrMapFile.h"

StringHash *strHash;

const int MAX_FILES = 10;
void usage(char *name){
	fprintf(stderr,"usage %s [-b size] [-vn] [-aa] file ...",name);
	exit(1);
}


class StrAction: public ActionDefault {
public:
	StrAction(Selection *selection, Coord left=0.0, Boolean useLeft=false,
	Coord right=0.0, Boolean useRight=false):ActionDefault(selection,
	left, useLeft, right, useRight){}

	virtual Boolean PopUp(View *, Coord x, Coord y,
			int /*button*/, char **& msgs, int & numMsgs) {
			msgBuf[0] = strHash->GiveClosestString((int)x);
			if (msgBuf[0] == NULL)
				msgBuf[0]="";
			sprintf(buf1,"X: %.2f",x);
			sprintf(buf2,"Y: %.2f",y);
			msgBuf[1] = buf1;
			msgBuf[2] = buf2;
			msgs = msgBuf;
			numMsgs = 3;
			return true;
	}
private:
	Selection *sel;
	char buf1[20];
	char buf2[20];
	char *msgBuf[3];
};


class MyXLabel: public AxisCallback {
	virtual char *GenerateLabel(Coord val){
		sprintf(buf,"%.0f",val);
		return buf;
		/*
		return strHash->GiveClosestString((int)val);
		*/
	}
private:
char buf[80];
};

class MyYLabel: public AxisCallback {
	virtual char *GenerateLabel(Coord val){
		sprintf(buf,"%.0f",val);
	}
private:
	char buf[80];
};

const int MAX_COLORS = 16;
main(int argc, char **argv){
	Init::DoInit(argc,argv);

	int arg= 1;
	Boolean snapshot = false;
	while (arg < argc){
		if (argv[arg][0] == '-'){
			switch(argv[arg][1]){
				case 's':
					snapshot = true;
					break;
			}
		}
		arg++;
	}

	/* Create hash table for strig */
	strHash = new StringHash();

	/* init colors */
	Color colorArray[MAX_COLORS];
	colorArray[0] = ColorMgr::AllocColor("blue");
	colorArray[1] = ColorMgr::AllocColor("green");
	colorArray[2] = ColorMgr::AllocColor("orange");
	colorArray[3] = ColorMgr::AllocColor("yellow");
	colorArray[4] = ColorMgr::AllocColor("red");
	colorArray[5] = ColorMgr::AllocColor("cyan");
	colorArray[6] = ColorMgr::AllocColor("violet");
	colorArray[7] = ColorMgr::AllocColor("sea green");
	colorArray[8] = ColorMgr::AllocColor("goldenrod");
	colorArray[9] = ColorMgr::AllocColor("indian red");
	colorArray[10] = ColorMgr::AllocColor("light pink");
	colorArray[11] = ColorMgr::AllocColor("maroon");
	colorArray[12] = ColorMgr::AllocColor("ivory1");
	colorArray[13] = ColorMgr::AllocColor("azure1");
	colorArray[14] = ColorMgr::AllocColor("gray80");
	colorArray[15] = ColorMgr::AllocColor("gray40");
	red = colorArray[4];

	/*
	int i;
	for (i=0; i < argc; i++)
		printf("%s\n",argv[i]);
	*/
	/* init defaults */
	VisualFilter filter;
	filter.flag = VISUAL_LOC;
	filter.xLow = -10.0;
	filter.yLow = -15.0;
	filter.xHigh = 700.0;
	filter.yHigh = 100.0;

	int bufSize; 
	Boolean prefetch, existing;
	BufPolicy::policy policy;
	Init::BufPolicies(bufSize, prefetch, policy, existing);

	/* create stuff needed by all views  */
	Selection *sel= new SelectionX("s1");

	ViewWinVer *win;
	ViewWinVer *snapWin;
	if (snapshot){
		win= new ViewWinVer("CPU",.1, .25, .8, .7);
		snapWin = new ViewWinVer("Snapshot", .1, 0.0, .8, .20);
	}

	/* Create stuff for each view */
	BufMgr *mgr;

	AxisCallback *xLabel = new MyXLabel();
	AxisCallback *yLabel = new MyYLabel();

	QueryProc *qp = new DispQueryProc(prefetch,policy,existing);


	TDataRec *tdata = new TDataRec("strData", sizeof(StrData));
		VisualFlag dimensionInfo[1]= { VISUAL_X};
	TDataMap *userMap= new UserMapping(tdata, 
			dimensionInfo, 0,Init::MaxGDataPages(), strHash);
	TDataMap *cpuMap= new CpuMapping(tdata, 
			dimensionInfo, 0,Init::MaxGDataPages(), strHash);
	TDataMap *ioMap= new IoMapping(tdata, 
			dimensionInfo, 0,Init::MaxGDataPages(), strHash);
	TDataMap *totalMap= new TotalMapping(tdata, 
			dimensionInfo, 0,Init::MaxGDataPages(), strHash);

	Action *action = new StrAction(sel,0.0, true);

	/* Create 3 views */
	ViewScatter *userView = 
		new ViewScatter("User", win, filter, qp, userMap, action);
	userView->SetXAxisCallback(xLabel);
	userView->SetYAxisCallback(yLabel);

	ViewScatter *cpuView = 
		new ViewScatter("CPU", win, filter, qp, cpuMap, action);
	cpuView->SetXAxisCallback(xLabel);
	cpuView->SetYAxisCallback(yLabel);

	ViewScatter *ioView = 
		new ViewScatter("IO", win, filter, qp, ioMap, action);
	ioView->SetXAxisCallback(xLabel);
	ioView->SetYAxisCallback(yLabel);

	VisualFilter totalFilter = filter;
	totalFilter.yHigh = 200;
	ViewScatter *totalView = 
		new ViewScatter("Total", win, totalFilter, qp, totalMap, action);
	totalView->SetXAxisCallback(xLabel);
	totalView->SetYAxisCallback(yLabel);

	sel->InsertView(userView);
	sel->InsertView(cpuView);
	sel->InsertView(ioView);
	sel->InsertView(totalView);

	if (snapshot){
		MakeSnapshotView(userMap,qp->GetMgr(), snapWin);
		MakeSnapshotView(cpuMap,qp->GetMgr(), snapWin,
			DisplayGDataInMem | DisplayGDataConverted);
		MakeSnapshotView(ioMap,qp->GetMgr(), snapWin,
			DisplayGDataInMem | DisplayGDataConverted);
		MakeSnapshotView(totalMap,qp->GetMgr(), snapWin,
			DisplayGDataInMem | DisplayGDataConverted);
	}

	Command *cmd = new Command(/*mgr*/NULL, qp);

	Dispatcher::RunNoReturn();
}

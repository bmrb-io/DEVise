/*
  ========================================================================
  DEVise Data Visualization Software
  (c) Copyright 1992-1997
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
  Revision 1.2  1997/03/28 16:10:45  wenger
  Added headers to all source files that didn't have them; updated
  solaris, solsparc, and hp dependencies.

  Revision 1.1  1995/11/27 16:20:42  jussi
  Moved these files from parent directory.

  Revision 1.2  1995/09/05 22:16:35  jussi
  Added CVS header.
*/

/* displays grids of ams data: soldn vs time 

   flags:
	-s  show snapshot of gdata conversion
	-o  overlap the graphs together.
	-m  use only 1 buffer manager.
	-r  display solar radiation,too.
*/

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
#include "TDataMapDispatch.h"
#include "DispQueryProc.h"
#include "BufMgr.h"
#include "TDataRec.h"
#include "TDataViewX.h"
#include "Command.h"
#include "ams.h"
#include "Temp.h"
#include "ViewWinVer.h"
#include "ViewWinHor.h"
#include "Snapshot.h"


int debug = 0;
Color red;

#include "mapfile.h"


const int MAX_FILES = 10;
void usage(char *name){
	fprintf(stderr,"usage %s [-b size] [-vn] [-aa] file ...",name);
	fprintf(stderr,"-b size\tbuffer size for focus\n");
	fprintf(stderr,"-c display connector\n");
	fprintf(stderr,"-vn symbol visibility, default = 1\n");
	fprintf(stderr,"\t0 = invisible\n");
	fprintf(stderr,"\t1 = visible\n");
	fprintf(stderr,"-aa = display axis\n");
	fprintf(stderr,"\tx = x axis\n");
	fprintf(stderr,"\ty = y axis\n");
	exit(1);
}


class SolDnAction: public ActionDefault {
public:
	SolDnAction(Selection *selection,
		Coord leftEdge = 0.0, Boolean useLeftFlag = false,
		Coord rightEdge = 0.0, Boolean useRightFlag = false): 
		ActionDefault(selection, leftEdge, useLeftFlag, rightEdge,
		useRightFlag){};

	virtual Boolean PopUp(View *, Coord x, Coord y,
			int /* button*/, char **& msgs, int & numMsgs){
		numMsgs = 2;
		msgs = msgBuf;
		msgBuf[0] = buf1;
		msgBuf[1] = buf2;
		sprintf(buf1,"x:%.2f",x/(60*24.0));
		sprintf(buf2,"y:%.2f",y);
		return true;
	}
private:
	char buf1[30];
	char buf2[30];
	char *msgBuf[2];
};

class MyCMap: public TDataCMap{
public:
	MyCMap(Color c){
		SetDefaultColor(BlackColor);
		color = c;
	}

	virtual Boolean Map(void *s1, void *s2, Connector *c){
		SolDnMapping_GData *rec1 = (SolDnMapping_GData *)s1;
		SolDnMapping_GData *rec2 = (SolDnMapping_GData *)s2;

		if (rec1->y <= 0.0 || rec2->y <= 0.0)
			return false;

		/* set Y coordinate */
		c->x1 = rec1->x;
		c->y1 = rec1->y;
		c->x2 = rec2->x;
		c->y2 = rec2->y;
		c->color = color;

		return true;
	}
private:
	Color color;
};

class MyXLabel: public AxisCallback {
	virtual char *GenerateLabel(Coord val){
		/* Convert into # of days */
		sprintf(buf,"%.0f", val/(60.0*24.0));
		return buf;
	}
private:
	char buf[80];
};

class MyYLabel: public AxisCallback {
	virtual char *GenerateLabel(Coord val){
		sprintf(buf,"%.0f",val);
		return buf;
	}
private:
	char buf[80];
};

const int MAX_COLORS = 16;

main(int argc, char **argv){

	Init::DoInit(argc,argv);

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
	if (argc < 2){
		fprintf(stderr,"no file specified\n");
		exit(2);
	}

	/* init defaults */
	VisualFilter filter;
	filter.flag = VISUAL_LOC;
	filter.xLow = 0.0;
	filter.yLow = -120.0;
	filter.xHigh = 352814.0;
	filter.yHigh = 1200.0;

	int bufSize; 
	Boolean prefetch, existing;
	BufPolicy::policy policy;
	Init::BufPolicies(bufSize, prefetch, policy, existing);


	/* Create mapping */
	Boolean overlap = false;
	Boolean showSnapShot = false;
	Boolean oneBufMgr = false;
	Boolean dispRain = false;
	char *fileNames[MAX_FILES];
	TDataMap *mappings[MAX_FILES];
	int numFiles = 0;
	int arg;
	for (arg = 1; arg < argc; arg++){
		switch(argv[arg][0]){
			case '-':
				switch(argv[arg][1]){
					case 's':
						/* show snapshot of gdata conversion */
						showSnapShot = true;
						break;
					case 'o':
						/* overlap the views */
						overlap = true;
						break;
					case 'm':
						/* use only 1 buffer manager */
						oneBufMgr = true;
						break;
					case 'r':
						/* display rain */
						dispRain = true;
						break;

					default:
						fprintf(stderr,"unknown flag %c\n",argv[arg][1]);
						exit(1);
						break;
				}
				break;
			default:
				/* new file */
				if (numFiles >= MAX_FILES){
					fprintf(stderr,"only %d files allowed", MAX_FILES);
                    reportErrNosys("Fatal error");//TEMP -- replace with better message
					Exit::DoExit(2);
				}
				fileNames[numFiles++] = argv[arg];
				break;
		}
	}

	if (numFiles == 0){
		fprintf(stderr,"no file specified\n");
        reportErrNosys("Fatal error");//TEMP -- replace with better message
		Exit::DoExit(2);
	}
	else if (numFiles >= MAX_COLORS/2){
		fprintf(stderr,"%d files: too many\n", numFiles);
        reportErrNosys("Fatal error");//TEMP -- replace with better message
		Exit::DoExit(2);
	}

	int i;
	/*
	printf("%d files: ",numFiles);
	for (i=0; i < numFiles; i++)
		printf("%s ", fileNames[i]);
	printf("\n");
	*/

	/* create stuff needed by all views  */
	Selection *sel= new SelectionX("s1");

	ViewWin	*win;
	ViewWin	*snapWin;
	if (showSnapShot){
		/*
	 	win= new ViewWinVer("Soil", .12, .15, .8, .8);
		*/
		win= new ViewWinVer("Soil");
		snapWin = new ViewWinHor("SnapShot", 0.0, 0.15, .09, .8);
	}
	else  win= new ViewWinVer("Soil", .12);

	/* Set max # of buffer pages we'll allocated */
	int numBuffers;
	if (dispRain)
		numBuffers = numFiles*bufSize*2;
	else numBuffers = numFiles * bufSize;

	/*
	BufPage::SetMaxPages(numBuffers + 5*numFiles*2);
	*/

	/* Create stuff for each view */
	TDataViewX *view;
	TDataViewX *rainView;
	BufMgr *mgr, *rainMgr;
	if (oneBufMgr){
		/*
		mgr = new BufMgr(numBuffers);
		rainMgr = mgr;
		*/
	}

	AxisCallback *xLabel = new MyXLabel();
	AxisCallback *yLabel = new MyYLabel();

	for (i=0; i < numFiles ; i++){
		QueryProc *qp = new DispQueryProc(prefetch,policy,existing);
		QueryProc *rainQp;
		if (dispRain){
			rainQp = qp;
			/*
			rainQp = new DispQueryProc(prefetch,policy,existing);
			*/
		}


		/* create buffer manager */
		if (!oneBufMgr){
			/* create buffer manager */
			/*
			mgr = new BufMgr(bufSize);

			if (dispRain)
				rainMgr = new BufMgr(bufSize);
			*/
		}

		TDataRec *tdata = new TDataRec(fileNames[i], sizeof(AmsData));
		VisualFlag dimensionInfo[1]= { VISUAL_X};
		TDataMap *map = new SolDnMapping(tdata, 
			dimensionInfo, 1,Init::MaxGDataPages(),
				&colorArray[i]);
		Action *action = new SolDnAction(sel, 0.0, true);

		/* create connector mapping */
		TDataCMap *cMap = new MyCMap(colorArray[MAX_COLORS/2+i]);
		TDataMap *rainMap;
		TDataCMap *rainCMap;
		if (dispRain){
			rainMap = new RainMapping(tdata, dimensionInfo, 1,
					Init::MaxGDataPages(),NULL);
		}

		/* Create a view */
		if (i== 0 || !overlap){
			view = new TDataViewX( (overlap? argv[0]: fileNames[i]), 
				win, filter, qp, map, NULL, action);
				view->SetXAxisCallback(xLabel);
				view->SetYAxisCallback(yLabel);

			sel->InsertView(view);

			if (dispRain){
				VisualFilter rainFilter= filter;
				rainFilter.yLow = -2.0;
				rainFilter.yHigh = 25.0;
				rainView = new TDataViewX("rain", win, rainFilter, rainQp, 
					rainMap, NULL, action);
				rainView->SetXAxisCallback(xLabel);
				rainView->SetYAxisCallback(yLabel);
				sel->InsertView(rainView);
			}
		}

		if (showSnapShot){
			MakeSnapshotView(map, qp->GetMgr(), snapWin, 
				 DisplayTDataInMem | DisplayGDataInMem | DisplayGDataConverted,
				 false);
			
			if (dispRain)
				MakeSnapshotView(rainMap, qp->GetMgr(), snapWin,
					DisplayGDataInMem|DisplayGDataConverted, false);
		}

		if (i == 0){
			Command *cmd = new Command(/*mgr*/NULL, qp);
		}
	}

	Dispatcher::RunNoReturn();
}

/* testXY: displays XY datafile 
flags:
	-s 	show snapshot of gdata conversion
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
#include "Temp.h"
#include "ViewWinVer.h"
#include "ViewWinHor.h"
#include "Snapshot.h"


int debug = 0;
Color red;

struct XYData {
	double x, y;
};

#include "XYmapfile.h"


const int MAX_FILES = 10;
void usage(char *name){
	fprintf(stderr,"usage %s [-b size] [-vn] [-aa] file ...",name);
	fprintf(stderr,"-aa = display axis\n");
	fprintf(stderr,"\tx = x axis\n");
	fprintf(stderr,"\ty = y axis\n");
	exit(1);
}





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
					Exit::DoExit(2);
				}
				fileNames[numFiles++] = argv[arg];
				break;
		}
	}

	if (numFiles == 0){
		fprintf(stderr,"no file specified\n");
		Exit::DoExit(2);
	}
	else if (numFiles >= MAX_COLORS/2){
		fprintf(stderr,"%d files: too many\n", numFiles);
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
		win= new ViewWinVer("Soil");
		snapWin = new ViewWinHor("SnapShot", 0.0, 0.15, .09, .8);
	}
	else  win= new ViewWinVer("Soil");

	/* Create stuff for each view */
	TDataViewX *view;
	TDataViewX *rainView;
	BufMgr *mgr, *rainMgr;

	for (i=0; i < numFiles ; i++){
		QueryProc *qp = new DispQueryProc(prefetch,policy,existing);
		QueryProc *rainQp;

		TDataRec *tdata = new TDataRec(fileNames[i], sizeof(XYData));
		VisualFlag dimensionInfo[1]= { VISUAL_X};
		TDataMap *map = new XYMapping(tdata, 
			dimensionInfo, 1,Init::MaxGDataPages(),
				&colorArray[i]);
		Action *action = new ActionDefault(sel, 0.0, true);

		/* create connector mapping */
		/*
		TDataCMap *cMap = new MyCMap(colorArray[MAX_COLORS/2+i]);
		*/
		TDataCMap *cMap = NULL;

		/* Create a view */
		if (i== 0 || !overlap){
			view = new TDataViewX( (overlap? argv[0]: fileNames[i]), 
				win, filter, qp, map, action);

			sel->InsertView(view);

		}

		if (showSnapShot){
			MakeSnapshotView(map, qp->GetMgr(), snapWin, 
				 DisplayTDataInMem | DisplayGDataInMem | DisplayGDataConverted,
				 false);
		}

		if (i == 0){
			Command *cmd = new Command(/*mgr*/NULL, qp);
		}
	}

	Dispatcher::RunNoReturn();
}

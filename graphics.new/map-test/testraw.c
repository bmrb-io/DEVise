/*
  $Id$

  $Log$
  Revision 1.2  1995/09/05 22:17:03  jussi
  Added CVS header.
*/

/*
   displays Khoros raw image
   usage: testraw [-w width] [-h height] [-f fname] [-t threshold]
*/

#include <sys/time.h>
#include "DeviseTypes.h"
#include "XDisplay.h"
#include "ViewWindow.h"
#include "Dispatcher.h"
#include "RectShape.h"
#include "ColorMgr.h"
#include "Control.h"
#include "TDataMapDispatch.h"
#include "Connector.h"
#include "Action.h"
#include "Selection.h"
#include "VisualArg.h"
#include "Init.h"
#include "DispQueryProc.h"
#include "TDataRec.h"
#include "TDataView.h"
#include "Command.h"
#include "TData2DRec.h"
#include "Temp.h"

int debug = 0;
Color red;
int threshold = 10;

#include "maprawfile.h"


const int MAX_FILES = 10;
void usage(char *name){
	exit(1);
}


class Action2D: public Action {
public:
	Action2D(Selection *selection){
		sel = selection;
	}

	virtual void AreaSelected(View *view, Coord xlow, Coord ylow,
				Coord xhigh, Coord yhigh, int button){
		VisualFilter filter;
		view->GetVisualFilter(filter);
		filter.xLow = xlow;
		filter.xHigh = xhigh;
		filter.yLow = ylow;
		filter.yHigh = yhigh;
		if (button == 1)
			sel->ChangeSelection(view, filter, filter,VISUAL_X);
		else if (button == 2)
			sel->ChangeSelection(view, filter, filter,VISUAL_X|VISUAL_Y);
	};

    void KeySelected(View *view, char key){
		VisualFilter filter;
		int index;
		TDataView *v = (TDataView *)view;
		if (key == 'c'){
			v->DisplayConnectors(!v->DisplayConnectors());
		}
		else if (key == 's'){
			view->DisplaySymbols(!view->DisplaySymbols());
		}
		else if (key == '+'){
			/* increase pixel size */
			Boolean changed = false;
			for (index = v->InitViewDataIterator(); v->MoreViewData(index);){
				TDataViewData *viewData = v->NextViewData(index);
				TDataMap *map = viewData->tMap;
				if (map->GetPixelWidth() < 30){
					changed = true;
					map->SetPixelWidth(map->GetPixelWidth()+1);
				}
			}
			v->DoneViewDataIterator(index);
			if (changed) view->Refresh();
		}
		else if (key == '-'){
			/* decrease pixel size */
			Boolean changed = false;
			for (index = v->InitViewDataIterator(); v->MoreViewData(index);){
				TDataViewData *viewData = v->NextViewData(index);
				TDataMap *map = viewData->tMap;
				if (map->GetPixelWidth() > 1){
					changed = true;
					map->SetPixelWidth(map->GetPixelWidth()-1);
				}
			}
			v->DoneViewDataIterator(index);
			if (changed) view->Refresh();
		}
		else if (key == '<' || key == ','){
			/* scroll left */
			view->GetVisualFilter(filter);
			Coord halfWidth = (filter.xHigh-filter.xLow)/2.0;
			filter.xLow -= halfWidth;
			filter.xHigh -= halfWidth;
			sel->ChangeSelection(view, filter, filter,VISUAL_X);
		}
		else if (key == '>' || key == '.'){
			/* scroll right */
			view->GetVisualFilter(filter);
			Coord halfWidth = (filter.xHigh-filter.xLow)/2.0;
			filter.xLow += halfWidth;
			filter.xHigh += halfWidth;
			sel->ChangeSelection(view, filter, filter,VISUAL_X);
		}
		else if (key == 'a' || key == 'A'){
			/* zoom in */
			view->GetVisualFilter(filter);
			Coord quarterWidth = (filter.xHigh-filter.xLow)/4.0;
			filter.xLow += quarterWidth;
			filter.xHigh -= quarterWidth;
			sel->ChangeSelection(view, filter, filter,VISUAL_X);
		}
		else if (key == 'z' || key == 'z'){
			/* zoom out */
			view->GetVisualFilter(filter);
			Coord halfWidth = (filter.xHigh-filter.xLow)/2.0;
			filter.xLow -= halfWidth;
			filter.xHigh += halfWidth;
			sel->ChangeSelection(view, filter, filter,VISUAL_X);
		}
		else if (key == '1' || key == '!'){
			/* zoom in y */
			view->GetVisualFilter(filter);
			Coord quarterHeight = (filter.yHigh-filter.yLow)/4.0;
			filter.yLow += quarterHeight;
			filter.yHigh -= quarterHeight;
			sel->ChangeSelection(view, filter, filter,VISUAL_Y);
		}
		else if (key == 'q' || key == 'Q'){
			/* zoom out */
			view->GetVisualFilter(filter);
			Coord halfHeight = (filter.yHigh-filter.yLow)/2.0;
			filter.yLow -= halfHeight;
			filter.yHigh += halfHeight;
			sel->ChangeSelection(view, filter, filter,VISUAL_Y);
		}
		else if (key == 'd')
			debug++;
		else if (key == 'e')
			debug--;
	}
private:
	Selection *sel;
};


int WIDTH = 512;
int HEIGHT = 512;
const int MAX_COLORS = 16;
main(int argc, char **argv){

	Init::DoInit(argc,argv);

	char *fname = "ball.raw";
	int arg = 1;
	while (arg < argc-1){
		if (argv[arg][0] == '-'){
			switch(argv[arg][1]){
				case 'w':
					WIDTH = atoi(argv[arg+1]);
					arg++;
					break;
				case 'h':
					HEIGHT = atoi(argv[arg+1]);
					arg++;
					break;
				case 'f':
					fname = argv[arg+1];
					arg++;
					break;
				case 't':
					threshold = atoi(argv[arg+1]);
					arg++;
					break;
				default:
					fprintf(stderr,"unknown flag -%c\n", argv[arg][1]);
					exit(1);
			}
		}
		else {
			fprintf(stderr,"unknown pararm %s\n", argv[arg]);
			exit(1);
		}

		arg++;
	}

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


	/* init defaults */
	Coord x = 0.0;
	Coord y = 0.0;
	Coord width = WIDTH;
	Coord height = HEIGHT;

	int bufSize; 
	Boolean prefetch, existing;
	BufPolicy::policy policy;
	Init::BufPolicies(bufSize, prefetch, policy, existing);



	/* create stuff needed by all views  */
	Selection *sel= new SelectionX("s1");

	ViewWindow *win = new ViewWindowVer("2D", BlackColor, WhiteColor);


	/* Create stuff for each view */
	TDataView *view;
	QueryProc *qp = new DispQueryProc(prefetch,policy,existing);

	TData2DRec *tdata = new TData2DRec(fname, sizeof(char),WIDTH, HEIGHT);
	static VisualFlag dimensionInfo[2];
	dimensionInfo[0] = VISUAL_Y;
	dimensionInfo[1] = VISUAL_X;
	TDataMap *map = new Mapping2D(tdata, 
			dimensionInfo, 2,Init::MaxGDataPages());
	Action *action = new Action2D(sel);

	/* Create a view */
	view = new TDataView( "Raw Image", qp,
			tdata, map, x,y,width, height, true, NULL, false, 1, action);

	sel->InsertView(view);

	win->Bind(view);

	Command *cmd = new Command(/*mgr*/NULL, qp);

	Dispatcher::RunNoReturn();
}

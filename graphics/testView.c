/* testView.c: suplply > 1 param to get boundary for window.
Not param for no window boundary for view. */
#include <stdio.h>
#include "Init.h"
#include "ViewWinVer.h"
#include "View.h"
#include "Action.h"
#include "Color.h"

class MyAxisCallback: public AxisCallback {
public:
	virtual char *GenerateLabel(Coord val){
		sprintf(buf,"%.0f",val);
		return buf;
	}
private:
char buf[50];
};

class MyView: public View{
public:
    MyView(char *name, Action *action,VisualFilter &initFilter,
			Coord x, Coord y, Coord w, Coord h):
		View(name, action, initFilter, x,y,w,h){};
	
    MyView(char *name, ViewWin *parent, Action *action,
		VisualFilter &initFilter, Boolean boundary): 
			View(name, parent, action, initFilter, 1, boundary){};
private:

	virtual void DerivedStartQuery(VisualFilter &queryFilter){
		WindowRep *win = GetWindowRep();
		printf("Start Query: %f,%f,%f,%f\n",
			queryFilter.xLow, queryFilter.yLow, 
			queryFilter.xHigh, queryFilter.yHigh);
		
		
		VisualFilter filter;
		GetVisualFilter(filter);
		win->SetFgColor(BlackColor);
		win->Line(filter.xLow, filter.yLow, filter.xHigh,
			filter.yHigh,2.0);
		win->Line(filter.xLow, filter.yHigh, filter.xHigh,
			filter.yLow,2.0);

		ReportQueryDone();

	}
	virtual void DerivedAbortQuery(){};


};

class MyAction: public Action{
public:
    virtual void AreaSelected(View *, Coord xlow, Coord ylow,
			Coord xhigh, Coord yhigh, int button){
			printf("Area %f,%f,%f,%f,%d\n",
				xlow, ylow, xhigh, yhigh, button);
	}

	/* key pressed */
	virtual void KeySelected(View *view, char key, Coord x, Coord y){
		printf("key %c,%f,%f\n", key, x,y);
		VisualFilter filter;
		if (key == '<' || key == ','){
			/* scroll left */
			view->GetVisualFilter(filter);
			Coord width = filter.xHigh-filter.xLow+1;
			filter.xLow -= width/2.0;
			filter.xHigh -= width/2.0;
			view->SetVisualFilter(filter);
		}
		else if (key == '>' || key == '.'){
			/* scroll right */
			view->GetVisualFilter(filter);
			Coord width = filter.xHigh-filter.xLow+1;
			filter.xLow += width/2.0;
			filter.xHigh += width/2.0;
			view->SetVisualFilter(filter);
		}
	}

	virtual Boolean PopUp(View *, Coord x, Coord y,
			char **& /*msgs*/, int & /*numMsgs*/) {
		printf("PopUp %f,%f\n", x, y);
		return 0;
	}

};

main(int argc, char **argv){

	Init::DoInit(argc,argv);

	Boolean boundary;
	if (argc > 1)
		boundary = true;
	else boundary = false;

	ViewWin *viewWin = new ViewWinVer("TestView");

	Action *myAction = new MyAction();
	VisualFilter filter;
	filter.flag = VISUAL_LOC;
	filter.xLow = 100.0;
	filter.yLow = 100.0;
	filter.xHigh = 200.0;
	filter.yHigh = 200.0;
	View *view1 = new MyView("view1",viewWin, myAction, filter, boundary);
	/* Set axes callback */
	AxisCallback *axisCallback = new MyAxisCallback();
	view1->SetXAxisCallback(axisCallback);
	view1->SetYAxisCallback(axisCallback);

	View *view2 = new MyView("view2",viewWin, myAction, filter, boundary);
	view2->SetXAxisCallback(axisCallback);
	view2->SetYAxisCallback(axisCallback);

	View *view3 = new MyView("view3",viewWin, myAction, filter, boundary);
	view3->SetXAxisCallback(axisCallback);
	view3->SetYAxisCallback(axisCallback);

	Dispatcher::RunNoReturn();
}

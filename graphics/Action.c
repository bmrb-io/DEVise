/*
  $Id$

  $Log$*/

#include "Action.h"
#include "Control.h"
#include "View.h"

void Action::AreaSelected(View *view, Coord xlow, Coord ylow,
			Coord xhigh, Coord yhigh, int button){
	if (xlow == xhigh && ylow == yhigh){
		ControlPanel::Instance()->SelectView(view);
		return;
	}
	VisualFilter filter;
	view->GetVisualFilter(filter);
	if (button == 1){
		filter.xLow = xlow;
		filter.xHigh = xhigh;
		view->SetVisualFilter(filter);
	}
	else if (button == 3){
		filter.xLow = xlow;
		filter.xHigh = xhigh;
		filter.yLow = ylow;
		filter.yHigh = yhigh;
		view->SetVisualFilter(filter);
	}
}


void Action::KeySelected(View *view, char key, Coord x, Coord y){
	VisualFilter filter;
	if (key == 'c'){
		/*
		v->DisplayConnectors(!v->DisplayConnectors());
		*/
	}
	else if (key == 's'){
		/*
		view->DisplaySymbols(!view->DisplaySymbols());
		*/
	}
	else if (key == '>' || key == '.'){
		/* scroll data right */
		view->GetVisualFilter(filter);
		Coord width = filter.xHigh - filter.xLow;
		Coord halfWidth = (filter.xHigh-filter.xLow)/2.0;
		filter.xLow -= halfWidth;
		filter.xHigh = filter.xLow + width;
		view->SetVisualFilter(filter);
	}
	else if (key == '<' || key == ','){
		/* scroll data left */
		view->GetVisualFilter(filter);
		Coord width = filter.xHigh - filter.xLow;
		Coord halfWidth = (filter.xHigh-filter.xLow)/2.0;
		filter.xLow += halfWidth;
		filter.xHigh = filter.xLow + width;
		view->SetVisualFilter(filter);
	}
	else if (key == 'a' || key == 'A'){
		/* zoom in */
		view->GetVisualFilter(filter);
		Coord quarterWidth = (filter.xHigh-filter.xLow)/4.0;
		filter.xLow += quarterWidth;
		filter.xHigh -= quarterWidth;
		view->SetVisualFilter(filter);
	}
	else if (key == 'z' || key == 'z'){
		/* zoom out */
		view->GetVisualFilter(filter);
		Coord halfWidth = (filter.xHigh-filter.xLow)/2.0;
		filter.xLow -= halfWidth;
		filter.xHigh += halfWidth;
		view->SetVisualFilter(filter);
	}
	else if (key == '1' || key == '!'){
		/* zoom in y */
		view->GetVisualFilter(filter);
		Coord quarterHeight = (filter.yHigh-filter.yLow)/4.0;
		filter.yLow += quarterHeight;
		filter.yHigh -= quarterHeight;
		view->SetVisualFilter(filter);
	}
	else if (key == 'q' || key == 'Q'){
		/* zoom out */
		view->GetVisualFilter(filter);
		Coord halfHeight = (filter.yHigh-filter.yLow)/2.0;
		filter.yLow -= halfHeight;
		filter.yHigh += halfHeight;
		view->SetVisualFilter(filter);
	}
}

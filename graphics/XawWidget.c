/*
  $Id$

  $Log$*/

#include "XawWidget.h"
/* DList<XawWidgetMapEntry*> XawWidget::_widgetMap;*/
XawWidgetMapEntryList XawWidget::_widgetMap;
int XawWidget::_widgetNum = 0;


/* Callback: find the appropriate widget to call */
void XawWidget::DoXtCallback(Widget w, XtPointer client_data, 
	XtPointer call_data){
	int index;
	/* DList<XawWidgetMapEntry*> &_widgetMap = XawWidget::_widgetMap;*/
	XawWidgetMapEntryList &_widgetMap = XawWidget::_widgetMap;
	for(index=_widgetMap.InitIterator(); _widgetMap.More(index); ){
		XawWidgetMapEntry *mapEntry= _widgetMap.Next(index);
		if (mapEntry->widget == w){
			mapEntry->xawWidget->DoCallback(w,client_data,call_data);
			break;
		}
	}
	_widgetMap.DoneIterator(index);
};

/************************************************************************/
void XawWidget::InsertWidget(Widget w){
	XawWidgetMapEntry *mapEntry = new XawWidgetMapEntry;
	mapEntry->widget = w;
	mapEntry->xawWidget = this;
	_widgetMap.Append(mapEntry);
	XtAddCallback(w,XtNcallback,XawWidget::DoXtCallback,0);
};


/**************************************************************/
void XawMinMaxWidget::XawWidgetNotify(XawWidget *xawWidget){
	if (xawWidget == _minWidget){
		if (_useInteger){
			int minVal = _minWidget->GetIntVal();
			int maxVal = _maxWidget->GetIntVal();
			if (minVal > maxVal)
				_maxWidget->SetVal(minVal);
		}
		else {
			double minVal = _minWidget->GetFloatVal();
			double maxVal = _maxWidget->GetFloatVal();
			if (minVal > maxVal)
				_maxWidget->SetVal(minVal);
		}
	}
	else if (xawWidget == _maxWidget){
		if (_useInteger){
			int minVal = _minWidget->GetIntVal();
			int maxVal = _maxWidget->GetIntVal();
			if (maxVal < minVal)
				_minWidget->SetVal(maxVal);
		}
		else {
			double minVal = _minWidget->GetFloatVal();
			double maxVal = _maxWidget->GetFloatVal();
			if (maxVal < minVal )
				_minWidget->SetVal(maxVal);
		}
	}
	ReportCallback();
}

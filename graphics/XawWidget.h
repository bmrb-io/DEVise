/* XawWidget.h */
#ifndef XawWidget_h
#define XawWidget_h
#include <stdio.h>
#include <X11/Intrinsic.h>
#include <X11/StringDefs.h>
#include <X11/Xaw/Label.h>
#include <X11/Xaw/Command.h>
#include <X11/Xaw/Form.h>
#include <X11/Xaw/Toggle.h>
#include <X11/Xaw/List.h>
#include <X11/Xaw/Viewport.h>
#include <X11/Xaw/AsciiText.h>
#include <X11/Xaw/Repeater.h>
#include "DeviseTypes.h"
#include "DList.h"

class XawWidget;
struct XawWidgetMapEntry {
	XawWidget *xawWidget;
	Widget widget;
};

class XawWidgetCallback{ 
public:
	virtual void XawWidgetNotify(XawWidget *xawWidget) = 0;
};

DefinePtrDList(XawWidgetCallbackList, XawWidgetCallback *);
DefinePtrDList(XawWidgetMapEntryList, XawWidgetMapEntry *);

class XawWidget {
public:
	/* callback to be informed when state of widget changes*/
	void InsertCallback(XawWidgetCallback *callback){
		_callbacks.Append(callback);
	}

	virtual Widget GetWidget()=0; /* get corresponding Xt widget */

protected:

	/* used by derived classes */


	/* report callback to registered callbacsk. */
	void ReportCallback(){
		int index;
		for (index=_callbacks.InitIterator(); _callbacks.More(index); ){
			XawWidgetCallback *callback =_callbacks.Next(index);
			callback->XawWidgetNotify(this);
		}
		_callbacks.DoneIterator(index);
	}

	/* insert Xt widget into callback list. DoCallback will be
	called when the given widget is notified by Xt. */
	void InsertWidget(Widget w);
	virtual void DoCallback(Widget /*w*/, XtPointer /*client_data*/, 
		XtPointer /*call_data*/){};

	char *WidgetName(char *name){
		sprintf(_name,"%s%d",name,_widgetNum++);
		return _name;
	}
private:

	/* DList<XawWidgetCallback *>_callbacks;*/
	XawWidgetCallbackList _callbacks;
	/* list of widget maps */
	/* static DList<XawWidgetMapEntry *> _widgetMap;*/
	static XawWidgetMapEntryList _widgetMap;

	/* base callback function that intercepts callbacks and route them
	to the approsiate XawWidget instance*/
	static void DoXtCallback(Widget w, XtPointer client_data, 
		XtPointer call_data);
	char _name[20];
	static int _widgetNum;
};

class XawComposite : public XawWidget {
public:
	/* insert widget as child. Should only be called by
	XawWidgets not composed of more than 1 X athena widgets */
	virtual void InsertChild(XawWidget * ){}; 
};

/* top level widget */
class XawTop: public XawComposite{
public:
	XawTop(Widget w) { _widget = w; };
	virtual Widget GetWidget(){ return _widget; };

private:
	Widget _widget;
};


class XawVertHorBox: public XawComposite{
public:
	XawVertHorBox(XawComposite *parent,Boolean vert=true){
/*
printf("XawVertBox\n");
*/
		_form = XtVaCreateManagedWidget(WidgetName("XawVertBox"),
					formWidgetClass, parent->GetWidget(), NULL);
/*
printf("XawVertBox after create widget\n");
*/
		_num = 0;
		_lastWidget = NULL;
		_vert = vert;
		parent->InsertChild(this);
	}

	virtual Widget GetWidget() { return _form; };

protected:
	virtual void InsertChild(XawWidget *widget){
		if (_num > 0)
			XtVaSetValues(widget->GetWidget(), (_vert?XtNfromVert:XtNfromHoriz),
				_lastWidget->GetWidget(),0);
		_num++;
		_lastWidget = widget;
	}
private:
	Widget _form;	/* form widget for us */
	int _num;	/* # of widgets */
	XawWidget *_lastWidget;
	Boolean _vert;
};

class XawVertBox: public XawVertHorBox{
public:
	XawVertBox(XawComposite *parent):XawVertHorBox(parent,true){};
};

class XawHorizBox: public XawVertHorBox{
public:
	XawHorizBox(XawComposite *parent):XawVertHorBox(parent,false){};
};

/* command widget */
class XawCommand: public XawWidget {
public:
	XawCommand(XawComposite *parent, char *label){
		_widget = XtVaCreateManagedWidget(WidgetName("Xawcommand"),
					commandWidgetClass, parent->GetWidget(), NULL);
		XtVaSetValues(_widget,XtNlabel,label,NULL);
		InsertWidget(_widget);
		parent->InsertChild(this);
	}
	virtual Widget GetWidget() { return _widget; };
protected:
	virtual void DoCallback(Widget , XtPointer , XtPointer ){ 
		ReportCallback(); };
private:
	Widget _widget;
};

/* repeater widget */
class XawRepeater: public XawWidget {
public:
	XawRepeater(XawComposite *parent, char *label){
		_widget = XtVaCreateManagedWidget(WidgetName("Xawrepeater"),
					repeaterWidgetClass, parent->GetWidget(), NULL);
		XtVaSetValues(_widget,XtNlabel,label,NULL);
		InsertWidget(_widget);
		parent->InsertChild(this);
	}
	virtual Widget GetWidget() { return _widget; };
protected:
	virtual void DoCallback(Widget , XtPointer , XtPointer ){ 
		ReportCallback(); };
private:
	Widget _widget;
};

/* label widget */
class XawLabel: public XawWidget {
public:
	XawLabel(XawComposite *parent, char *label){
		_widget = XtVaCreateManagedWidget(WidgetName("Xawlabel"),
					labelWidgetClass, parent->GetWidget(), NULL);
		XtVaSetValues(_widget,XtNlabel,label,NULL);
		XtVaSetValues(_widget,XtNborderWidth,0,NULL);
		parent->InsertChild(this);
	}
	virtual Widget GetWidget() { return _widget; };
protected:
	virtual void DoCallback(Widget , XtPointer , XtPointer ){ 
		ReportCallback(); };
private:
	Widget _widget;
};

/* toggle widget */
class XawToggle: public XawWidget {
public:
	XawToggle(XawComposite *parent, char *label){
		_widget = XtVaCreateManagedWidget(WidgetName("Xawtoggle"),
					toggleWidgetClass, parent->GetWidget(), NULL);
		XtVaSetValues(_widget,XtNlabel,label,NULL);
		InsertWidget(_widget);
		parent->InsertChild(this);
	}

	/* return current state of toggle widget */
	Boolean GetState(){
		Boolean state;
		XtVaGetValues(_widget,XtNstate,&state,0);
		return state;
	}

	void SetState(Boolean state){
		XtVaSetValues(_widget,XtNstate,state,0);
	}
	virtual Widget GetWidget() { return _widget; };

protected:
	virtual void DoCallback(Widget , XtPointer , XtPointer ){ 
		ReportCallback(); };
private:
	Widget _widget;
};

class XawViewport: public XawComposite{
public:
	XawViewport(XawComposite *parent){
		_widget = XtVaCreateManagedWidget(WidgetName("XawViewport"),
			viewportWidgetClass, parent->GetWidget(), NULL);
		XtVaSetValues(_widget, XtNallowVert,1,NULL);
		XtVaSetValues(_widget, XtNallowHoriz,1,NULL);
		parent->InsertChild(this);
	}
	virtual Widget GetWidget() { return _widget; };
private:
	Widget _widget;
};

class XawList: public XawWidget{
public:
	XawList(XawComposite *parent){
		_widget = XtVaCreateManagedWidget(WidgetName("Xawlist"),
			listWidgetClass, parent->GetWidget(), NULL);

		/* temp name to occupy initialy value */
		char *name = "1234567890123";
		for (int i=0; i < 5; i++)
			_tempNames[i] = name;

		XtVaSetValues(_widget, XtNdefaultColumns,1,NULL);
		XtVaSetValues(_widget, XtNforceColumns,1,NULL);
		XtVaSetValues(_widget, XtNlist, _tempNames,NULL);
		XtVaSetValues(_widget, XtNnumberStrings,5,NULL);

		InsertWidget(_widget);
		parent->InsertChild(this);
	}

	/* set new list of strings */
	void ChangeList (String *strings, int n){
		XtVaSetValues(_widget, XtNlist, strings,NULL);
		XtVaSetValues(_widget, XtNnumberStrings,n,NULL);
	}

	/* return currently selected string , or NULL if nothing selected */
	char *CurrentSelection(){
		XawListReturnStruct *returnList = XawListShowCurrent(_widget);
		if (returnList->list_index == XAW_LIST_NONE)
			return NULL;
		else return returnList->string;
	}

	virtual Widget GetWidget() { return _widget; };
protected:
	virtual void DoCallback(Widget , XtPointer , 
			XtPointer ){ ReportCallback(); };

private:
	Widget _widget;
	String _tempNames[5];
};

/* list wrapped in a viewport, making it scrollable */
class XawViewportList : public XawWidget, public XawWidgetCallback {
public:
	XawViewportList(XawComposite *parent){
		_viewPort = new XawViewport(parent);
		_list = new XawList(_viewPort);
		_list->InsertCallback(this);
	}

	/* set new list of strings */
	void ChangeList (String *strings, int n){
		_list->ChangeList(strings,n);
	}

	/* return currently selected string index */
	char *CurrentSelection(){
		return _list->CurrentSelection();
	}


	virtual Widget GetWidget() { return _viewPort->GetWidget(); }

private:
	/* _list widget informed as of callback, propagate it  */
	void XawWidgetNotify(XawWidget *){
		ReportCallback();
	}

	XawViewport *_viewPort;
	XawList *_list;
};

/* text widget */
class XawText : public XawWidget {
public:
	/* width = width in pixels */
	XawText(XawComposite *parent, Boolean editable=false,
		int width = 50){
		_widget = XtVaCreateManagedWidget(WidgetName("XawText"),
				asciiTextWidgetClass, parent->GetWidget(), NULL);
		XtVaSetValues(_widget,XtNwidth,width,NULL);
		_editable = editable;
		/*
		XtVaSetValues(_widget,XtNeditType,XawtextEdit,NULL);
		*/
		parent->InsertChild(this);
	};

	void SetText(String text){
		XtVaSetValues(_widget,XtNstring,text,NULL);
	}

	String GetText(){
		String string;
		XtVaGetValues(_widget, XtNstring,&string,0);
		return string;
	};

	Widget GetWidget() { return _widget; };

private:
	Widget _widget;
	Boolean _editable;
};

const unsigned USE_MIN_VAL_MASK = 1;
const unsigned USE_MAX_VAL_MASK = 2;
const unsigned USE_MIN_MAX_MASK = (USE_MIN_VAL_MASK | USE_MAX_VAL_MASK);
typedef unsigned MinMaxFlag;
class XawValWidget: public XawWidget, public XawWidgetCallback {
public:
	XawValWidget(XawComposite *parent, int curVal, int minVal=0, int maxVal=10,
		int stepSize=1, MinMaxFlag flag = USE_MIN_MAX_MASK){
		_curIntVal = curVal;
		_minIntVal = minVal;
		_maxIntVal = maxVal;
		_stepInt = stepSize;
		_flag = flag;
		_useInteger = true;

		CreateWidgets(parent);
	}
	XawValWidget(double curVal, XawComposite *parent, double minVal=0.0, 
		double maxVal=10.0, double stepSize=1.0,MinMaxFlag flag=USE_MIN_MAX_MASK){
		_curFloatVal = curVal;
		_minFloatVal = minVal;
		_maxFloatVal = maxVal;
		_stepFloat = stepSize;
		_flag = flag;
		_useInteger = false;
		CreateWidgets(parent);
	}
	void SetVal(int val){
		if (_useInteger)
			DoSetVal(val);
		else DoSetVal((double)val);

	}

	void SetVal(double val){
		if (_useInteger)
			DoSetVal((int)val);
		else DoSetVal(val);

	}

	int GetIntVal(){
		if (_useInteger)
			return _curIntVal;
		else return ((int)_curFloatVal);
	}

	double GetFloatVal(){
		if (_useInteger)
			return ((double)_curIntVal);
		else return _curFloatVal;
	}

	Widget GetWidget(){ return _boxWidget->GetWidget(); };
private:
	void DoSetVal(int val){
		if (val < _minIntVal)
			val = _minIntVal;
		if (val > _maxIntVal)
			val = _maxIntVal;
		_curIntVal = val;
		sprintf(buf,"%d",val);
		_textWidget->SetText(buf);
	}

	void DoSetVal(double val){
		if (val < _minFloatVal)
			val = _minFloatVal;
		if (val > _maxFloatVal)
			val = _maxFloatVal;
		_curFloatVal = val;
		sprintf(buf,"%.2f",val);
		_textWidget->SetText(buf);
	}

	void CreateWidgets(XawComposite *parent){
		_boxWidget = new XawHorizBox(parent);
		_textWidget = new XawText(_boxWidget);

		_leftWidget = new XawRepeater(_boxWidget,"<");
		_leftWidget->InsertCallback(this);

		_rightWidget = new XawRepeater(_boxWidget,">");
		_rightWidget->InsertCallback(this);

		if (_useInteger)
			SetVal(_curIntVal);
		else SetVal(_curFloatVal);
	}

	/* callback from children, propagate*/
	void XawWidgetNotify(XawWidget *xawWidget){
		if (xawWidget == _leftWidget){
			if (_useInteger)
				SetVal(_curIntVal-_stepInt);
			else 
				SetVal(_curFloatVal-_stepFloat);
		}
		else if (xawWidget == _rightWidget){
			if (_useInteger)
				SetVal(_curIntVal+_stepInt);
			else
				SetVal(_curFloatVal+_stepFloat);
		}
		ReportCallback();
	};

	int _curIntVal, _minIntVal, _maxIntVal, _stepInt;
	double _curFloatVal, _minFloatVal, _maxFloatVal, _stepFloat;
	XawHorizBox *_boxWidget;
	XawText *_textWidget;	/* label */
	XawRepeater *_leftWidget,*_rightWidget; /* left and right buttons */
	MinMaxFlag _flag;
	Boolean _useInteger;	/* TRUE if val is integer, otherwise it's double */
	char buf[20];
};

class XawMinMaxWidget: public XawWidget, public XawWidgetCallback {
public:
	XawMinMaxWidget(XawComposite *parent, int curMin, int curMax,
		int minVal=0, int maxVal=10, int stepSize=1, 
		MinMaxFlag flag = USE_MIN_MAX_MASK){
		_boxWidget = new XawHorizBox(parent);
		_minWidget = new XawValWidget(_boxWidget,curMin,minVal,maxVal,
			stepSize,flag);
		_minWidget->InsertCallback(this);

		_maxWidget = new XawValWidget(_boxWidget,curMax,minVal,maxVal,
			stepSize, flag);
		_maxWidget->InsertCallback(this);
		_useInteger = true;
	}

	XawMinMaxWidget(double curMin, double curMax, XawComposite *parent, 
		double minVal=0.0, double maxVal=10.0, double stepSize=1.0, 
		MinMaxFlag flag = USE_MIN_MAX_MASK){

		_boxWidget = new XawHorizBox(parent);
		_minWidget = new XawValWidget(curMin,_boxWidget,minVal,maxVal,
				stepSize,flag);
		_minWidget->InsertCallback(this);

		_maxWidget = new XawValWidget(curMax,_boxWidget,minVal,maxVal,
				stepSize, flag);
		_maxWidget->InsertCallback(this);
		_useInteger = false;
	}

	void GetVal(int &minVal, int &maxVal){
		minVal = _minWidget->GetIntVal();
		maxVal = _maxWidget->GetIntVal();
	}
	void GetVal(double &minVal, double &maxVal){
		minVal = _minWidget->GetFloatVal();
		maxVal = _maxWidget->GetFloatVal();
	}

	void SetVal(int minVal, int maxVal){
		_minWidget->SetVal(minVal);
		_maxWidget->SetVal(maxVal);
	}
	void SetVal(double minVal, double maxVal){
		_minWidget->SetVal(minVal);
		_maxWidget->SetVal(maxVal);
	}

	Widget GetWidget(){ return _boxWidget->GetWidget();};
private:
	/* notification from children about changes in the widget */
	void XawWidgetNotify(XawWidget *xawWidget);

	XawHorizBox *_boxWidget;
	XawValWidget *_minWidget, *_maxWidget;
	Boolean _useInteger;
};

/* MinMax widget with toggle label.
If label is on, all changes in value is reported.
Whenlabel is off, no change in value is reported.
Changes in label state are reported.*/
class XawToggleMinMaxWidget: public XawWidget,public XawWidgetCallback {
public:
	XawToggleMinMaxWidget(XawComposite *parent, char *label,
		int curMin, int curMax, int minVal=0, int maxVal=10, int stepSize=1, 
		MinMaxFlag flag = USE_MIN_MAX_MASK){
		_boxWidget = new XawHorizBox(parent);
		_toggleWidget = new XawToggle(_boxWidget,label);
		_toggleWidget->InsertCallback(this);

		_minMaxWidget = new XawMinMaxWidget(_boxWidget,
			curMin,curMax,minVal,maxVal,stepSize,flag);
		_minMaxWidget->InsertCallback(this);
	}
	XawToggleMinMaxWidget(char *label,XawComposite *parent, 
		double curMin, double curMax, double minVal=0.0, double maxVal=10.0, 
		double stepSize=1, MinMaxFlag flag = USE_MIN_MAX_MASK){
		_boxWidget = new XawHorizBox(parent);
		_toggleWidget = new XawToggle(_boxWidget,label);
		_toggleWidget->InsertCallback(this);

		_minMaxWidget = new XawMinMaxWidget(curMin,curMax,_boxWidget,
			minVal,maxVal,stepSize,flag);
		_minMaxWidget->InsertCallback(this);
	}

	void GetState(Boolean &labelState, int &minVal, int &maxVal){
		labelState = _toggleWidget->GetState();
		_minMaxWidget->GetVal(minVal,maxVal);
	};
	void GetState(Boolean &labelState, double &minVal, double &maxVal){
		labelState = _toggleWidget->GetState();
		_minMaxWidget->GetVal(minVal,maxVal);
	};

	void SetToggleState(Boolean labelState){
		_toggleWidget->SetState(labelState);
	}

	void SetState(Boolean labelState, int minVal, int maxVal){
		_toggleWidget->SetState(labelState);
		_minMaxWidget->SetVal(minVal, maxVal);
	}
	void SetState(Boolean labelState, double minVal, double maxVal){
		_toggleWidget->SetState(labelState);
		_minMaxWidget->SetVal(minVal, maxVal);
	}

	Widget GetWidget() { return _boxWidget->GetWidget();};
private:
	void XawWidgetNotify(XawWidget *xawWidget){
		if (xawWidget == _toggleWidget){
			ReportCallback();
		}
		else {
			/* _minMaxWidget: report change in state only when label is active*/
			if (_toggleWidget->GetState())
				ReportCallback();
		}
	}

	XawHorizBox *_boxWidget ;
	XawToggle *_toggleWidget;
	XawMinMaxWidget *_minMaxWidget;
};

/* label with left/right widgets for use in scrolling and zooming*/
class XawLeftRight: public XawWidget, public XawWidgetCallback {
public:
	XawLeftRight(XawComposite *parent, char *label){
		_boxWidget = new XawHorizBox(parent);
		_labelWidget = new XawLabel(_boxWidget,label);

		_leftWidget = new XawCommand(_boxWidget,"<");
		_leftWidget->InsertCallback(this);

		_rightWidget = new XawCommand(_boxWidget,">");
		_rightWidget->InsertCallback(this);
	}

	/* Return TRUE if last button pressed was left, otherwise, it's the
	right button */
	Boolean LastLeft(){ return _lastLeft; }

	Widget GetWidget() { return _boxWidget->GetWidget();};

private:

	/* callback from left/right buttons */
	virtual void XawWidgetNotify(XawWidget *xawWidget){
		if (xawWidget == _leftWidget)
			_lastLeft = true;
		else _lastLeft = false;

		/* report button hit */
		ReportCallback();
	}

	XawHorizBox *_boxWidget;
	XawLabel *_labelWidget;
	XawCommand *_leftWidget, *_rightWidget;
	Boolean _lastLeft;/*TRUE if last button was left, otherwise it's right button*/
};

#endif

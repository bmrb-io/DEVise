/*
  $Id$

  $Log$*/

#include <stdio.h>
#include <time.h>
#include "ActionDefault.h"
#include "TDataMap.h"
#include "Selection.h"
#include "ViewGraph.h"
#include "RecInterp.h"
#include "QueryProc.h"
#include "TData.h"



ActionDefault::ActionDefault( char *name, Coord leftEdge, 
	Boolean useLeftFlag, Coord rightEdge, Boolean useRightFlag):
	Action(name){
	left = leftEdge;
	right = rightEdge;
	useLeft = useLeftFlag;
	useRight = useRightFlag;
}

void ActionDefault::AreaSelected(View *view, Coord xlow, Coord ylow,
				Coord xhigh, Coord yhigh, int button){
/* printf("Area Selected %f,%f,%f,%f, %d\n",xlow,ylow,xhigh,yhigh,button); */

	ControlPanel::Instance()->SelectView(view);

	if (xlow == xhigh || ylow == yhigh){
		if (xlow == xhigh && ylow == yhigh){
			ControlPanel::Instance()->SelectView(view);
		}
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

void ActionDefault::KeySelected(View *view, char key, Coord x, Coord y){
	ControlPanel::Instance()->SelectView(view);
	VisualFilter filter;
	ViewGraph *vg = (ViewGraph *)view;
	if (key == 'c'){
		/*
		v->DisplayConnectors(!v->DisplayConnectors());
		*/
	}
	/*
	else if (key == 's'){
		view->DisplaySymbols(!view->DisplaySymbols());
	}
	*/
	else if (key == '+'){
		/* increase pixel size */
		Boolean changed = false;
		for (vg->InitMappingIterator(); vg->MoreMapping();){
			TDataMap *map = vg->NextMapping();
			if (map->GetPixelWidth() < 30){
				changed = true;
				map->SetPixelWidth(map->GetPixelWidth()+1);
			}
		}
		vg->DoneMappingIterator();
		if (changed) view->Refresh();

	}
	else if (key == '-'){
		/* decrease pixel size */
		Boolean changed = false;
		for (vg->InitMappingIterator(); vg->MoreMapping();){
			TDataMap *map = vg->NextMapping();
			if (map->GetPixelWidth() >1){ 
				changed = true;
				map->SetPixelWidth(map->GetPixelWidth()-1);
			}
		}
		vg->DoneMappingIterator();
		if (changed) view->Refresh();
	}
	else if (key == '>' || key == '.' || key == '6'){
		/* scroll data right */
		view->GetVisualFilter(filter);
		Coord width = filter.xHigh - filter.xLow;
		Coord halfWidth = (filter.xHigh-filter.xLow)/2.0;
		filter.xLow -= halfWidth;
		Coord xMin;
		if (view->GetXMin(xMin) && filter.xLow < xMin)
			filter.xLow = xMin;
		/*
		if (useLeft && filter.xLow < left)
			filter.xLow = left;
		*/
		filter.xHigh = filter.xLow + width;
		view->SetVisualFilter(filter);
	}
	else if (key == '<' || key == ',' || key == '4'){
		/* scroll data left */
		view->GetVisualFilter(filter);
		Coord width = filter.xHigh - filter.xLow;
		Coord halfWidth = (filter.xHigh-filter.xLow)/2.0;
		filter.xLow += halfWidth;
		filter.xHigh = filter.xLow + width;
		if (useRight && filter.xHigh > right){
			filter.xHigh = right;
			filter.xLow = filter.xHigh - width;
		}
		view->SetVisualFilter(filter);
	}
	else if (key == 'a' || key == 'A' || key == '7'){
		/* zoom in */
		view->GetVisualFilter(filter);
		Coord quarterWidth = (filter.xHigh-filter.xLow)/4.0;
		filter.xLow += quarterWidth;
		filter.xHigh -= quarterWidth;
		view->SetVisualFilter(filter);
	}
	else if (key == 'z' || key == 'z' || key == '9'){
		/* zoom out */
		view->GetVisualFilter(filter);
		Coord halfWidth = (filter.xHigh-filter.xLow)/2.0;
		filter.xLow -= halfWidth;
		filter.xHigh += halfWidth;
		Coord xMin;
		if (view->GetXMin(xMin) && filter.xLow < xMin)
			filter.xLow = xMin;
		/*
		if (useLeft && filter.xLow < left)
			filter.xLow = left;
		*/
		if (useRight && filter.xHigh > right)
			filter.xHigh = right;
		view->SetVisualFilter(filter);
	}
	else if (key == 's' || key == 'S' || key == '1'){
		/* zoom in y */
		view->GetVisualFilter(filter);
		Coord quarterHeight = (filter.yHigh-filter.yLow)/4.0;
		filter.yLow += quarterHeight;
		filter.yHigh -= quarterHeight;
		view->SetVisualFilter(filter);
	}
	else if (key == 'x' || key == 'X' || key == '3'){
		/* zoom out */
		view->GetVisualFilter(filter);
		Coord halfHeight = (filter.yHigh-filter.yLow)/2.0;
		filter.yLow -= halfHeight;
		filter.yHigh += halfHeight;
		view->SetVisualFilter(filter);
	}
}


const int MAX_MSGS = 50;
const int MSG_BUF_SIZE = 4096;
static char msgBuf[MSG_BUF_SIZE];
static int msgIndex;
static char *msgPtr[MAX_MSGS];
static int numMsgs;

/* Return true if we're OK. Otherwise, we have exeeced resource */
static Boolean CheckParams(){
	if (numMsgs >= MAX_MSGS){
		return false;
		/*
		fprintf(stderr,"MAX_MSGS exceeded\n");
		Exit::DoExit(1);
		*/
	}
	if (msgIndex >= MSG_BUF_SIZE){
		return false;
		/*
		fprintf(stderr,"MSG_BUF_SIZE exceeded\n");
		Exit::DoExit(1);
		*/
	}
	return true;
}

static Boolean PutMessage(char *msg){
	if (!CheckParams())
		return false;
	
	int len = strlen(msg)+1;
	if (msgIndex+len > MSG_BUF_SIZE)
		return false;
	sprintf(&msgBuf[msgIndex],"%s", msg);
	msgPtr[numMsgs++] = &msgBuf[msgIndex];
	msgIndex += len;
	return true;
}

static Boolean InitPutMessage(double x, AttrType xAttr, double y,
	AttrType yAttr){
	msgIndex = 0;
	numMsgs = 0;
	char buf[128];
	char *dateStr;

	if (xAttr == DateAttr){
		time_t clock = (time_t)x;
		sprintf(buf,"x: %s", DateString(clock));
	} else {
		sprintf(buf,"x: %.2f",x);
	}
	if (!PutMessage(buf))
		return false;

	if (yAttr == DateAttr){
		time_t clock = (time_t)y;
		sprintf(buf,"y: %s", DateString(clock));
	}
	else {
		sprintf(buf,"y: %.2f",y);
	}
	return PutMessage(buf);
}

static void EndPutMessage(int &numMessages, char **&msgs) {
	numMessages = numMsgs;
	msgs = msgPtr;
}

/* print contents of message buffer */
static void PrintMsgBuf(){
	int i;
	for (i=0; i < numMsgs; i++){
		if (strcmp(msgPtr[i],"")== 0)
			printf("\n");
		else printf("%s ",msgPtr[i]);
	}
	printf("\n");
}

Boolean ActionDefault::PopUp(View *view, Coord x, Coord y, Coord xHigh,
	Coord yHigh,
	int /* button*/, char **& msgs, int & numMsgs){
	ControlPanel::Instance()->SelectView(view);
	numMsgs = 2;
	msgs = msgBuf;

	AttrType xAttr = view->GetXAxisAttrType();
	AttrType yAttr = view->GetYAxisAttrType();

	InitPutMessage((x+xHigh)/2.0, xAttr, (y+yHigh)/2.0, yAttr);

	char *errorMsg;
	if (!PrintRecords(view, x, y, xHigh, yHigh,errorMsg)){
		InitPutMessage((x+xHigh)/2.0, xAttr, (y+yHigh)/2.0, yAttr);
		PutMessage("");
		PutMessage(errorMsg);
	}
	EndPutMessage(numMsgs, msgs);
	return true;
}


/* Print infor about records in view */
Boolean ActionDefault::PrintRecords(View *view, Coord x, Coord y, 
	Coord xHigh, Coord yHigh, char *&errorMsg) {
/*
printf("ActionDefault::PrintRecords %s %f %f %f %f\n", view->GetName(), x, y,
	xHigh, yHigh);
*/
static RecInterp *recInterp = NULL;

	if (recInterp == NULL)
		recInterp = new RecInterp;

	VisualFilter filter;
	filter.flag = VISUAL_X;
	filter.xLow = x;
	filter.xHigh = xHigh;

	/*
	filter.yLow = y ;
	filter.yHigh = yHigh;
	*/

	/* get mapping */
	ViewGraph *vg = (ViewGraph *)view;
	TDataMap *map = NULL;
	for (vg->InitMappingIterator(); vg->MoreMapping(); ){
		map = vg->NextMapping();
		break;
	}
	vg->DoneMappingIterator();
	if (map == NULL){
		errorMsg= "PrintRecord: no mapping";
		return false;
	}

	RecId startRid;
	int numRecs;
	char *buf;
	QueryProc *qp = QueryProc::Instance();
	/*
	printf("getting TData\n");
	*/
	int numDimensions;
	VisualFlag *dimensionInfo;
	numDimensions = map->DimensionInfo(dimensionInfo);
	Boolean approxFlag = (numDimensions == 1 && dimensionInfo[0] == VISUAL_X);

	if (!approxFlag){
		errorMsg= "Query of scatter plot to be implemented";
		return false;
	}

	qp->InitTDataQuery(map, filter, approxFlag);
	TData *tdata = map->GetTData();
	int tRecSize = tdata->RecSize();
	AttrList *attrs = tdata->GetAttrList();
	if (attrs == NULL){
		errorMsg= "PrintRecords: no attribute info";
		return false;
	}
	recInterp->SetAttrs(attrs);
	/*
	recInterp->PrintAttrHeading();
	*/
	Boolean tooMuch = false;
	int numAttrs = attrs->NumAttrs();
	int j;
	while (qp->GetTData(startRid, numRecs, buf)){
		/*
		printf("got records rid %d numRecs %d buf 0x%x\n",
			startRid, numRecs, buf);
		*/
		char *ptr = buf;
		int i;
		for (i=0; i< numRecs; i++){
			recInterp->SetBuf(ptr);
			ptr += tRecSize;

			if (!tooMuch){
				/* set up for display inside the window */
				PutMessage("");
				for (j=0; j < numAttrs; j++){
					char buf[128];
					recInterp->PrintAttr(buf,j,true);
					if (!tooMuch){
						if (!PutMessage(buf)){
							PrintMsgBuf();
							errorMsg = "see text window";
							tooMuch = true;
						}
					}
					else {
						printf(buf);
					}
				}
				if (tooMuch)
					printf("\n");
			}
			else  /* print in text window */
				recInterp->Print(true);
		}
	}
	qp->DoneTDataQuery();
	return !tooMuch;
error:
	fprintf(stderr,"PrintRecords: can't find attribute\n");
	Exit::DoExit(2);
}

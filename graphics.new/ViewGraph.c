/* ViewGraph.c */
#include "ViewGraph.h"
#include "TDataMap.h"
#include "ActionDefault.h"


ViewGraph::ViewGraph(char *name, VisualFilter &initFilter, 
		AxisLabel *xAxis, AxisLabel *yAxis,
		Color fg, Color bg,
		Action *action):
		View(name,action,initFilter, fg, bg, xAxis, yAxis){
	if (action == NULL){
		SetAction(new ActionDefault("default"));
	}
}

void ViewGraph::InsertMapping(TDataMap *map){
	_mappings.Append(map);

	AttrList *attrList = map->GDataAttrList();
	if (attrList != NULL){
		AttrInfo *info = attrList->Find("x");
		if (info != NULL && info->type == DateAttr)
			SetXAxisAttrType(DateAttr);

		info = attrList->Find("y");
		if (info != NULL && info->type == DateAttr)
			SetYAxisAttrType(DateAttr);
	}


	Refresh();
}

void ViewGraph::InitMappingIterator(){
	_index = _mappings.InitIterator();
}

Boolean ViewGraph::MoreMapping(){
	return _mappings.More(_index);
}

TDataMap *ViewGraph::NextMapping(){
	return _mappings.Next(_index);
}
void ViewGraph::DoneMappingIterator(){
	_mappings.DoneIterator(_index);
}

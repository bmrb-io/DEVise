/* ViewGraph.h */
#ifndef ViewGraph_h
#define ViewGraph_h
#include "View.h"
#include "DList.h"
#include "Color.h"

class TDataMap;
DefinePtrDList(TDataMapList, TDataMap *)

class ViewGraph: public View{
public:
	ViewGraph(char *name, VisualFilter &initFilter, 
		AxisLabel *xAxis, AxisLabel *yAxis,
		Color fg, Color bg,
		Action *action = NULL);
	
	virtual void InsertMapping(TDataMap *map);

	void InitMappingIterator();
	Boolean MoreMapping();
	TDataMap *NextMapping();
	void DoneMappingIterator();

private:
TDataMapList _mappings;
int _index;
};
#endif

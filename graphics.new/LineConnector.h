/* LineConnector.h*/
#ifndef LineConnector_h
#define LineConnector_h
#include "ConnectorShape.h"
#include "Connector.h"
#include "WindowRep.h"

/*************************************************************
A Discrete line connector that plots the following:
                 | (x2,y2)
                 |
(x1,y1)----------+
***************************************************************/
class DiscLineConnector: public ConnectorShape{
public:
	/* draw connection between two symbols */
	virtual void DrawConnection(WindowRep *win, Connector *con){

		win->SetFgColor(con->color);
		win->SetPattern(con->pattern);
		Coord width;
		if (con->numShapeAttrs > 0)
			width = con->shapeAttrs[0];
		else width = 2.0;
		win->Line(con->x1,con->y1,con->x2,con->y1,width);
		win->Line(con->x2,con->y1,con->x2,con->y2,width);
	};
};

/******************************************************************
A discrete line connector that plots the following:
		   +------- (x2,y2)
           |
    (x1,y1)|
*******************************************************************/
class DiscLineConnector2: public ConnectorShape{
public:
	/* draw connection between two symbols */
	virtual void DrawConnection(WindowRep *win, Connector *con){

		win->SetFgColor(con->color);
		win->SetPattern(con->pattern);
		Coord width;
		if (con->numShapeAttrs > 0)
			width = con->shapeAttrs[0];
		else width = 2.0;
		win->Line(con->x1,con->y1,con->x1,con->y2,width);
		win->Line(con->x1,con->y2,con->x2,con->y2,width);
	};
};


/*****************************************************************
Continuous connector
******************************************************************/
class ContLineConnector : public ConnectorShape{
public:
	virtual void DrawConnection(WindowRep *win,Connector *con){
		win->SetFgColor(con->color);
		win->SetPattern(con->pattern);
		Coord width;
		if (con->numShapeAttrs > 0)
			width = con->shapeAttrs[0];
		else width = 1.0;
		/*
		printf("connection %f, %f, %f, %f, color %d\n",
			con->x1,con->y1, con->x2,con->y2,con->color);
		*/
		win->Line(con->x1,con->y1,con->x2,con->y2,width);
	};
};

/*****************************************************************
bar connector
******************************************************************/
class BarConnector : public ConnectorShape{
public:
	virtual void DrawConnection(WindowRep *win,Connector *con){
		win->SetFgColor(con->color);
		win->SetPattern(con->pattern);

		/*
		printf("connection %f, %f, %f, %f, color %d\n",
			con->x1,con->y1, con->x2,con->y2,con->color);
		*/
		win->FillRect(con->x1,0.0, con->x2-con->x1,con->y1);
	};
};
#endif

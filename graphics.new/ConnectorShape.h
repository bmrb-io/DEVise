/*
  $Id$

  $Log$*/

#ifndef ConnectorShape_h
#define ConnectorShape_h
#include "DeviseTypes.h"
#include <stdio.h>

/* ID for shapes. NumShapes is used as placeholder for # of shapes. */
typedef int ConnectorShapeID;

/* predefine connector id's */
const ConnectorShapeID ContLineConnectorID= 0;
const ConnectorShapeID  DiscLineConnectorID=1;
const ConnectorShapeID BarConnectorID = 2;
const ConnectorShapeID DiscLineConnector2ID = 3;

class WindowRep;

class Connector;
class ConnectorShape {
public:
	
	/* Draw the shape for the symbol */
	virtual void DrawConnection(WindowRep *win, Connector *connector)=0;
};

/* max connector shapes that can be registered with the connector
registrar */
const int MaxConnectorShapes = 20;

class ConnectorShapeRegistrar {
public:

	/* Register a connector shape */
	void RegisterConnectorShape(ConnectorShapeID id, ConnectorShape *cShape){
		if (_theConnectorShapeRegistrar == NULL){
			_theConnectorShapeRegistrar = new ConnectorShapeRegistrar;
		}
		_theConnectorShapeRegistrar->DoRegisterConnectorShape(id,cShape);
	};

	/* Draw the shape of connector */
	static void DrawConnection(WindowRep *win, Connector *connector){
		if (_theConnectorShapeRegistrar == NULL){
			_theConnectorShapeRegistrar = new ConnectorShapeRegistrar;
		}
		_theConnectorShapeRegistrar->DoDrawConnection(win,connector);
	};

private:
	/* constructor */
	ConnectorShapeRegistrar();

	void DoRegisterConnectorShape(ConnectorShapeID id, ConnectorShape *cShape);

	void DoDrawConnection(WindowRep *win, Connector *connector);


ConnectorShape *_connectorShapes[MaxConnectorShapes];
/*the one and only shape registrar*/
static ConnectorShapeRegistrar *_theConnectorShapeRegistrar; 
};

#endif

/* ConnectorShape.c */
#include "LineConnector.h"

ConnectorShapeRegistrar 
	*ConnectorShapeRegistrar::_theConnectorShapeRegistrar = NULL;


/******************************************************************
constructor: This is where new shapes are added int the array
of shapes in the shape registrar.
*******************************************************************/
ConnectorShapeRegistrar::ConnectorShapeRegistrar(){
	
	int i;
	for (i=0; i < MaxConnectorShapes; i++){
		_connectorShapes[i] = NULL;
	}

	_connectorShapes[DiscLineConnectorID] = new DiscLineConnector;
	_connectorShapes[ContLineConnectorID] = new ContLineConnector;
	_connectorShapes[BarConnectorID] = new BarConnector;
	_connectorShapes[DiscLineConnector2ID] = new DiscLineConnector2;
}

/******************************************************************
Register a connector shape 
*****************************************************************/
void ConnectorShapeRegistrar::DoRegisterConnectorShape(
	ConnectorShapeID id, ConnectorShape *cShape){
	if (id < 0 || id >= MaxConnectorShapes){
		fprintf(stderr,"ConnectorShapeRegistrar: id %d invalid\n",id);
		Exit::DoExit(1);
	}

	if (_connectorShapes[id] != NULL){
		fprintf(stderr,"ConnectorShapeRegistrar: id %d already in use\n",id);
		Exit::DoExit(1);
	}
	_connectorShapes[id] = cShape;
}


/***************************************************************
draw connector
*****************************************************************/
void ConnectorShapeRegistrar::DoDrawConnection
	(WindowRep *win, Connector *connector){
	_connectorShapes[connector->cShapeID]->DrawConnection(win,connector);
}


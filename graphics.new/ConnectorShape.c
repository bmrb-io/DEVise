/*
  ========================================================================
  DEVise Data Visualization Software
  (c) Copyright 1992-1995
  By the DEVise Development Group
  Madison, Wisconsin
  All Rights Reserved.
  ========================================================================

  Under no circumstances is this software to be copied, distributed,
  or altered in any way without prior permission from the DEVise
  Development Group.
*/

/*
  $Id$

  $Log$
  Revision 1.3  1995/12/29 22:43:02  jussi
  Added the copyright message and cleaned up the code a bit.

  Revision 1.2  1995/09/05 22:14:38  jussi
  Added CVS header.
*/

#include "LineConnector.h"

ConnectorShapeRegistrar 
	*ConnectorShapeRegistrar::_theConnectorShapeRegistrar = NULL;

/******************************************************************
  constructor: This is where new shapes are added int the array
  of shapes in the shape registrar.
*******************************************************************/

ConnectorShapeRegistrar::ConnectorShapeRegistrar()
{
  for(int i = 0; i < MaxConnectorShapes; i++)
    _connectorShapes[i] = NULL;

  _connectorShapes[DiscLineConnectorID] = new DiscLineConnector;
  _connectorShapes[ContLineConnectorID] = new ContLineConnector;
  _connectorShapes[BarConnectorID] = new BarConnector;
  _connectorShapes[DiscLineConnector2ID] = new DiscLineConnector2;
}

/******************************************************************
  Register a connector shape 
*****************************************************************/

void ConnectorShapeRegistrar::DoRegisterConnectorShape(ConnectorShapeID id,
						       ConnectorShape *cShape)
{
  if (id < 0 || id >= MaxConnectorShapes) {
    fprintf(stderr,"ConnectorShapeRegistrar: id %d invalid\n", id);
    reportErrNosys("Fatal error");//TEMP -- replace with better message
    Exit::DoExit(1);
  }

  if (_connectorShapes[id] != NULL) {
    fprintf(stderr,"ConnectorShapeRegistrar: id %d already in use\n", id);
    reportErrNosys("Fatal error");//TEMP -- replace with better message
    Exit::DoExit(1);
  }

  _connectorShapes[id] = cShape;
}

/***************************************************************
  draw connector
*****************************************************************/

void ConnectorShapeRegistrar::DoDrawConnection(WindowRep *win,
					       Connector *connector)
{
  _connectorShapes[connector->cShapeID]->DrawConnection(win, connector);
}

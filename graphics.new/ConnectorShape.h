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
  Revision 1.2  1995/09/05 22:14:39  jussi
  Added CVS header.
*/

#ifndef ConnectorShape_h
#define ConnectorShape_h

#include <stdio.h>
#include "DeviseTypes.h"

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
  virtual void DrawConnection(WindowRep *win, Connector *connector) = 0;
};

/* max connector shapes that can be registered with the connector registrar */
const int MaxConnectorShapes = 10;

class ConnectorShapeRegistrar {
public:
  /* Register a connector shape */
  static void RegisterConnectorShape(ConnectorShapeID id,
				     ConnectorShape *cShape) {
    if (_theConnectorShapeRegistrar == NULL)
      _theConnectorShapeRegistrar = new ConnectorShapeRegistrar;
    _theConnectorShapeRegistrar->DoRegisterConnectorShape(id, cShape);
  }

  /* Draw the shape of connector */
  static void DrawConnection(WindowRep *win, Connector *connector) {
    if (_theConnectorShapeRegistrar == NULL)
      _theConnectorShapeRegistrar = new ConnectorShapeRegistrar;
    _theConnectorShapeRegistrar->DoDrawConnection(win, connector);
  }

private:
  ConnectorShapeRegistrar();

  void DoRegisterConnectorShape(ConnectorShapeID id, ConnectorShape *cShape);
  void DoDrawConnection(WindowRep *win, Connector *connector);

  static ConnectorShapeRegistrar *_theConnectorShapeRegistrar; 

  ConnectorShape *_connectorShapes[MaxConnectorShapes];
};

#endif

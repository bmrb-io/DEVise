// UpperPanel.java

// DTE Window Project

// Yunrui Li
// CS Dept. UW-Madison
// Copyright (c) 1999

import java.io.*;
import java.awt.*;
import java.awt.event.*;
import java.lang.*;
import java.util.*;

// The upper command panel.
public class UpperPanel extends Panel implements ActionListener, DialogShower
{ 
  // The main layout.
  private CardLayout layout;
  
  // The component panes.
  private final UpperComponent [] components =
  {
    new CreateTablePane (),
    new CreateViewPane (),
    new CreateGestaltPane (),
    new CreateIndexPane (),
    new ShowGestaltPane (),
    new ODBCPane (),
    new ExecuteSQLPane (),
    new ShowEnvironmentsPane (),
    new MiscPane ()
  };
  
  // The component names.
  public static final String [] comNames =
  {
    "Create Table",
    "Create View",
    "Create Gestalt",
    "Create Index",
    "Show Gestalt",
    "ODBC Commands",
    "Execute SQL",
    "Show Environments",
    "Miscellaneous Commands"
  };

  // Constructor.
  public UpperPanel (Environment env)
    {
      setLayout (layout = new CardLayout ());
      enableEvents (AWTEvent.CONTAINER_EVENT_MASK);

      // Add the components.
      for (int i = 0; i < components.length; i ++)
	{
	  add (components [i], comNames [i]);
	  components [i].setEnvironment (env);
	}
    }
  
  // Set the redirect result outputer for each components.
  public final void setOutputerRedirect (ResultOutputer r)
    {
      for (int i = 0; i < components.length; components [i ++].setOutputerRedirect (r));
    }

  // Set the redirect catalog refresher for each components.
  public final void setCatalogRefresher (CatalogRefresher r)
    {
      for (int i = 0; i < components.length; components [i ++].setCatalogRefresher (r));
    }
  
  // Action (menu) event handler, bring up the corresponding pane.
  public final void actionPerformed (ActionEvent e)
    {
      layout.show (this, e.getActionCommand ());
    }

  // Handle the show dialog command request.
  public final void showDialog (int dialog, String param)
    {
      // Bring up the correct pane.
      if (dialog == DialogShower.CREATE_INDEX)
	layout.show (this, "Create Index");
      else if (dialog == DialogShower.SHOW_GESTALT)
	layout.show (this, "Show Gestalt");
      
      // Distribute the request to all of its component panes.
      for (int i = 0; i < components.length; components [i ++].showDialog (dialog, param));
    }
}

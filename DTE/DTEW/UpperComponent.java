// UpperComponent.java

// DTE Window Project

// Yunrui Li
// CS Dept UW-Madison
// Copyright (c) 1999

import java.io.*;
import java.awt.*;
import java.awt.event.*;
import java.lang.*;
import java.util.*;

// An UpperComponent instance is a pane in the upper command panel.
public abstract class UpperComponent extends OutputerPane 
  implements ActionListener, SizedComponent, CatalogRefresher, DialogShower
{
  // Preferred size.
  private static final int nSizeX = 1200;
  private static final int nSizeY = 300;

  // The main environment.
  protected Environment env = null;

  // The redirect catalog refresher.
  protected CatalogRefresher ref = null;
  
  // Return the preferred size.
  public final Dimension getPreferredSize ()
    {
      return ScreenScale.getDimension (nSizeX, nSizeY);
    }

  // Constructor.
  public UpperComponent ()
    {
      super (ScrollPane.SCROLLBARS_ALWAYS);
      enableEvents (AWTEvent.CONTAINER_EVENT_MASK);
    }

  // Set the environment.
  public void setEnvironment (Environment env)
    {
      this.env = env;
    }

  // Return the environment.
  public final Environment getEnvironment ()
    {
      return env;
    }

  // Set the redirect catalog refresher.
  public final void setCatalogRefresher (CatalogRefresher ref)
    {
      this.ref = ref;
    }

  // Handle the home catalog change event.
  public final void updateHome ()
    {
      ref.updateHome ();
    }

  // Handle the general catalog change event.
  public final void updateTable (String table)
    {
      ref.updateTable (table);
    }

  // Handle the show command request.
  public void showDialog (int dialog, String param)
    {
    }
}

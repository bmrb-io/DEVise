// DTEWindow.java

// DTE Window Project

// Yunrui Li
// CS Dept. UW-Madison.
// Copyright (c) 1999

import java.io.*;
import java.awt.*;
import java.awt.event.*;
import java.lang.*;
import java.util.*;

// DTE Window main frame.
public class DTEWindow extends Frame implements ActionListener
{
  // Title.
  private static final String szTitle = "DTE Window";

  // Left-up corner position in the screen.
  private static final int nLocX = 50;
  private static final int nLocY = 50;
  
  // The panels.
  private Panel bottomPanel = null;
  private UpperPanel upperPanel = null;
  private CatalogPanel catalogPanel = null;
  private ResultPanel resultPanel = null;
  
  // The main environment.
  private Environment env = null;
  
  // Constructor.
  public DTEWindow (Environment e)
    {
      this.env = e;
    }
  
  // Draw the frame.
  public void draw ()
    {
      // Set the location.
      ScreenScale.setLocation (nLocX, nLocY, this);
      setLayout (new BorderLayout ());
  
      // Load panels.
      bottomPanel = new Panel ();
      add (bottomPanel, "South");
      bottomPanel.setLayout (new BorderLayout ());

      loadResultPanel ();
      loadCatalogPanel (env);

      bottomPanel.add (catalogPanel, "West");
      bottomPanel.add (resultPanel, "East");
      
      catalogPanel.setOutputerRedirect (resultPanel);
      
      loadUpperPanel ();
      upperPanel.setOutputerRedirect (resultPanel);
      add (upperPanel, "North");

      upperPanel.setCatalogRefresher (catalogPanel);
      catalogPanel.setDialogRedirect (upperPanel);
      
      enableEvents (AWTEvent.WINDOW_EVENT_MASK);
      enableEvents (AWTEvent.CONTAINER_EVENT_MASK);
      
      // Set title and menu.
      setTitle (szTitle);
      createMenu ();

      // Show.
      pack ();
      setVisible (true);
    }
  
  // Process window event.
  protected void processWindowEvent (WindowEvent e)
    {
      if (e.getID () == WindowEvent.WINDOW_CLOSING)
	System.exit (0);
      
      super.processWindowEvent (e);
    }

  // Action event handler.
  public void actionPerformed (ActionEvent e)
    {
      String com = e.getActionCommand ();
      
      if (com.equals ("Exit"))
	processWindowEvent (new WindowEvent (this, WindowEvent.WINDOW_CLOSING));
    }
  
  // Load the catalog panel.
  private final void loadCatalogPanel (Environment env)
    {
      catalogPanel = new CatalogPanel (env);
      catalogPanel.loadCatalog ();
    }
  
  // Load the result panel.
  private final void loadResultPanel ()
    {
      resultPanel = new ResultPanel ();
    }

  // Load the upper command panel.
  private final void loadUpperPanel ()
    {
      upperPanel = new UpperPanel (env);
    }
  
  // Create menus.
  private void createMenu ()
    {
      MenuBar mb = new MenuBar ();
      
      Menu mTable = new Menu ("Create");
      mTable.add (new MenuItem ("Create Table"));
      mTable.add (new MenuItem ("Create View"));
      mTable.add (new MenuItem ("Create Gestalt"));
      mTable.add (new MenuItem ("Create Index"));
      
      mTable.addActionListener (upperPanel);
      mb.add (mTable);

      mTable = new Menu ("Others");
      mTable.add (new MenuItem ("Show Gestalt"));
      mTable.add (new MenuItem ("ODBC Commands"));
      mTable.add (new MenuItem ("Execute SQL"));
      mTable.add (new MenuItem ("Show Environments"));
      mTable.add (new MenuItem ("Miscellaneous Commands"));

      mTable.addActionListener (upperPanel);
      mb.add (mTable);

      mTable = new Menu ("Exit");
      mTable.add (new MenuItem ("Exit"));
      
      mTable.addActionListener (this);
      mb.add (mTable);
      
      setMenuBar (mb);
    }

  // Main function. Show one DTEWindow.
  public static void main (String [] args) 
    {
      try
	{
	  new DTEWindow (new Environment ()).draw ();
	}
      catch (Throwable e)
	{
	  e.printStackTrace (System.err);
	  System.exit (1);
	}
    }
  
}


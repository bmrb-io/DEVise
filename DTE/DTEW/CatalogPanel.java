// CatalogPanel.java

// DTE Window Project

// Yunrui Li
// CS Dept. UW-Madison
// Copyright (c) 1999

import java.io.*;
import java.awt.*;
import java.awt.event.*;
import java.lang.*;
import java.util.*;

// The panel to show catalogs.
public class CatalogPanel extends OutputerPane 
  implements ActionListener, SizedComponent, CatalogRefresher, DialogShower
{
  // The in-line buttons associated with each catalog entries.
  static class EntryButton extends Button
  {
    // The vector where the catalog entry information is stored.
    private PairVector pv;
    
    // Constructor
    public EntryButton (String com, PairVector pv, Color c)
      {
	super (com);
	this.pv = pv;
	setBackground (c);
      }

    // Return the catalog entry vector
    public final PairVector getPairVector ()
      {
	return pv;
      }
    
    // Return the catalog entry name
    public final String getName ()
      {
	return (String) (pv.find (Catalog.sName));
      }

    // Return the catalog entry type
    public final String getType ()
      {
	return (String) (pv.find (Catalog.sType));
      }
  }
  
  // Catalog panel size
  private static final int nSizeX = 600;
  private static final int nSizeY = 600;

  // Main catalog
  private Catalog catalog = null;

  private Panel mainPanel = new Panel ();
  private Panel topPanel = new Panel ();
  private Panel bottomPanel = new Panel ();

  // The vector that contains all the catalog entry pair-vectors.
  private Vector panelVector = new Vector ();
  
  // Constants and components
  private static final String lCat = "Catalog :";
  private TextField catField = new TextField ("", 30);
  private Button catButton = new Button ("Load Catalog");
  private Button fileButton = new Button ("Load File");
  private Button clearButton = new Button ("Clear");
  
  // Entry color
  private static final Color rightColor = new Color (200, 200, 200);
   
  // Entry fonts
  private static final Font leftFont = new Font ("Courier", Font.BOLD, 12);
  private static final Font rightFont = new Font ("Courier", Font.PLAIN, 12);
  
  // Main environment
  private Environment env;

  // Status of the currently shown catalog.
  // If the current item is home table of DTE
  private boolean isHome = true;

  // If current item is shown as a table (as opposed as shown as a file)
  private boolean isTable = true;

  // The current table name. (e.g. .Server1)
  private String tName = "";

  // The current catalog file name (e.g. /p/...)
  private String pName = null;

  // In-line button commands
  private static final String comCreateIndex = "Create Index ...";
  private static final String comShow = "Show ...";
  private static final String comLoad = "Load";
  private static final String comSchema = "Show Schema";
  private static final String comMaterialize = "Materialize";

  // The redirect dialog shower (the upper panel)
  private DialogShower shower = null;
  
  // Constructor
  public CatalogPanel (Environment env)
    {
      super (ScrollPane.SCROLLBARS_ALWAYS);
      this.env = env;
      
      topPanel.setLayout (new FlowLayout (FlowLayout.LEFT));
      
      // Add components
      topPanel.add (new Label (lCat));
      topPanel.add (catField);
      topPanel.add (clearButton);
      topPanel.add (fileButton);
      topPanel.add (catButton);
      catButton.addActionListener (this);
      fileButton.addActionListener (this);
      clearButton.addActionListener (this);
      
      enableEvents (AWTEvent.CONTAINER_EVENT_MASK);
      
      mainPanel.setLayout (new BorderLayout ());
      mainPanel.add (topPanel, "North");
      mainPanel.add (bottomPanel, "South");

      add (mainPanel);
      setScrollPosition (0, 0);
    }
  
  // Action event handler
  public void actionPerformed (ActionEvent e)
    {
      String com = e.getActionCommand ();
      Object o = e.getSource ();
      
      // If it is an in-line button
      if (o instanceof EntryButton)
	processEntryCommand ((EntryButton) o, com);
      else if (com.equals ("Load Catalog"))
	loadCatalog (catField.getText ().trim ());
      else if (com.equals ("Load File"))
	loadFile (catField.getText ().trim ());
      else if (com.equals ("Clear"))
	catField.setText ("");
    }

  // Process in-line buttons
  private void processEntryCommand (EntryButton o, String com)
    {
      String cur = tName;
      if (! (cur.endsWith (".")))
	cur += ".";
      
      // Get catalog entry type and name
      String type = o.getType ();
      String name = o.getName ();
      
      // According to commands, redirect commands or execute appropriate query.
      if (com.equals (comCreateIndex))
	showDialog (DialogShower.CREATE_INDEX, cur + name);
      else if (com.equals (comShow))
	showDialog (DialogShower.SHOW_GESTALT, cur + name);
      else if (com.equals (comLoad))
	{
	  if (type.equals (Catalog.sDBServer))
	    loadCatalog (cur + name);
	  else
	    loadFile ((String) (o.getPairVector ().find (Catalog.sCatalogpath)));
	}
      else if (com.equals (comSchema))
	{
	  String q = "Schema " + cur + name + ";";
	  env.runQuery (q, this);
	}
      else if (com.equals (comMaterialize))
	{
	  String q = "Materialize " + cur + name + ";";
	  env.runQuery (q, this);
	  updateHome ();
	}
    }
  
  public Dimension getPreferredSize ()
    {
      return ScreenScale.getDimension (nSizeX, nSizeY);
    }

  // Redraw the catalog entry components
  private void redrawPane (String path)
    {
      // Remove old catalog entry components.
      bottomPanel.removeAll ();
      panelVector.removeAllElements ();
      
      catField.setText (path);

      // Get the current catalog entries.
      Vector v = catalog.getVector ();
      Panel curp = bottomPanel, nextp;

      for (int i = 0; i < v.size (); i ++)
	{
	  PairVector pv = (PairVector) (v.elementAt (i));

	  panelVector.addElement (curp);

	  Panel p1 = new Panel ();
	  Panel p2 = new Panel ();
	  
	  // Set the correct layout, color and font.
	  p1.setLayout (new GridLayout (0, 1));
	  p2.setLayout (new GridLayout (0, 1));
	  
	  Color color = colorMap ((String) (pv.find (Catalog.sType)));
	  p1.setBackground (color);
	  p2.setBackground (rightColor);
	  p1.setFont (leftFont);
	  p2.setFont (rightFont);
	  
	  // Add the catalog entry attributes as labels.
	  for (int j = 0; j < pv.size (); j ++)
	    {
	      p1.add (new Label ((String) (pv.elementAtFirst (j)), Label.RIGHT));
	      p2.add (new Label ((String) (pv.elementAtSecond (j)), Label.LEFT));
	    }

	  // Get in-line buttons.
	  Panel pButton = ((isTable) ? addButtons (pv, color) : null);

	  Panel p = new Panel ();
	  if (pButton == null)
	    {
	      // If there is no in-line buttons.
	      p.setLayout (new FlowLayout (FlowLayout.LEFT));
	      p.add (p1);
	      p.add (p2);
	    }
	  else
	    {
	      // If there are in-line buttons.
	      p.setLayout (new BorderLayout (0, 0));
	      
	      Panel r = new Panel ();
	      r.setLayout (new FlowLayout (FlowLayout.LEFT));
	      r.add (p1);
	      r.add (p2);

	      Panel q = new Panel ();
	      q.setLayout (new FlowLayout (FlowLayout.LEFT, 0, 0));
	      q.add (pButton);

	      p.add (r, "North");
	      p.add (q, "West");
	    }
	       
	  curp.setLayout (new BorderLayout ());
	  curp.add (p, "North");
	  nextp = new Panel ();
	  curp.add (nextp, "South");
	  curp = nextp;
	}

      // Refresh the whole pane.
      validate ();
    }

  // get the color associated with catalog entry type
  Color colorMap (String type)
    {
      if (type.equals (Catalog.sStandardTable))
	return new Color (0, 192, 192);
      else if (type.equals (Catalog.sSQLView))
	return new Color (192, 0, 192);
      else if (type.equals (Catalog.sGestalt))
	return new Color (192, 192, 0);
      else if (type.equals (Catalog.sDBServer))
	return new Color (0, 224, 224);
      else if (type.equals (Catalog.sODBC))
	return new Color (224, 0, 224);
      else if (type.equals (Catalog.sTable))
	return new Color (224, 224, 0);
      else if (type.equals (Catalog.sDirectory))
	return new Color (0, 128, 128);
      else if (type.equals (Catalog.sUNIXFILE))
	return new Color (128, 0, 128);
      else if (type.equals (Catalog.sMaterView))
	return new Color (128, 128, 0);
      else
	return new Color (128, 0, 0);
    }
  
  // Return a panel that contains current in-line buttons, return null if no in-line buttons.
  private Panel addButtons (PairVector pv, Color c)
    {
      String name = (String) (pv.find (Catalog.sName));
      String type = (String) (pv.find (Catalog.sType));
      
      Panel pButton = new Panel ();
      Button b;

      pButton.setLayout (new FlowLayout (FlowLayout.LEFT));
      if (type.equals (Catalog.sStandardTable))
	{
	  b = new EntryButton (comCreateIndex, pv, c);
	  b.addActionListener (this);
	  pButton.add (b);
	}
      else if (type.equals (Catalog.sSQLView))
	{
	  b = new EntryButton (comSchema, pv, c);
	  b.addActionListener (this);
	  pButton.add (b);

	  b = new EntryButton (comMaterialize, pv, c);
	  b.addActionListener (this);
	  pButton.add (b);
	}
      else if (type.equals (Catalog.sGestalt))
	{
	  b = new EntryButton (comShow, pv, c);
	  b.addActionListener (this);
	  pButton.add (b);
	}
      else if (type.equals (Catalog.sDBServer) || type.equals (Catalog.sDirectory))
	{
	  b = new EntryButton (comLoad, pv, c);
	  b.addActionListener (this);
	  pButton.add (b);
	}
      else if (type.equals (Catalog.sMaterView))
	{
	  b = new EntryButton (comSchema, pv, c);
	  b.addActionListener (this);
	  pButton.add (b);
	}
      else
	return null;
      
      return pButton;
    }

  // Load a catalog file.
  public void loadFile (String path)
    {
      // Resolve environment variable name.
      if (path.equals ("$"))
	path = env.getHomeTable ().trim ();
      else if (path.startsWith ("$"))
	path = env.getenv (path.substring (1)).trim ();

      // Distinguish if it is the home table.
      String home = env.getHomeTable ().trim ();
      pName = path;
      isTable = false;
      isHome = (home.equals (path));
      
      // Load file.
      try
	{
	  catalog = Catalog.load (path);
	  redrawPane (path);
	}
      catch (Catalog.ParseException e)
	{
	  writeError (e);
	}
    }
  
  // Load a catalog.
  public final void loadCatalog (String path)
    {
      loadCatalog (path, true);
    }
  
  // Load a catalog.
  public void loadCatalog (String path, boolean output)
    {
      // Resolve environment variable name.
      if (path.equals ("$"))
	path = env.getHomeTable ().trim ();
      else if (path.startsWith ("$"))
	path = env.getenv (path.substring (1)).trim ();
  
      // Catalog name should start with "."
      String p = ((path.startsWith (".")) ? path : ("." + path));
      
      isHome = (p.equals ("."));
      isTable = true;
      tName = p;
      
      try
	{
	  // Get the input stream from the catalog, and read it.
	  InputStream file = getCatalog (p, output);
	  catalog = Catalog.load (file);
	  redrawPane (path);
	}
      catch (Catalog.ParseException e)
	{
	  writeError (e);
	}
    }

  // Get the input-stream from catalog name.
  public InputStream getCatalog (String name, boolean output) throws Catalog.ParseException
    {
      try 
	{
	  // Select everything from the catalog and return the result stream as the input-stream.
	  String q = "Select * from " + name  + ";";
	  
	  InputStream [] is = env.runQuery (q);

	  if (output)
	    writeError (is [1]);

	  return (is [0]);
	}
      catch (IOException e)
	{
	  throw new Catalog.ParseException (e);
	}     
    }

  // Load the home table in the first time.
  public final void loadCatalog ()
    {
      loadCatalog (".", false);
    }

  // Reload currently shown catalog.
  private final void reload ()
    {
      if (isTable)
	loadCatalog (tName);
      else
	loadFile (pName);
    }
  
  // Handle the event that the home table has been possibly changed.
  public final void updateHome ()
    {
      if (isHome)
	reload ();
    }
  
  // Handle the event that some table has been possibly changed.
  public final void updateTable (String table)
    {
      if (table.equals ("."))
	updateHome ();
      else if (isTable && table.equals (tName))
	loadCatalog (table);
    }

  public final void setDialogRedirect (DialogShower d)
    {
      this.shower = d;
    }

  // Redirect the show dialog commands to another dialog shower.
  public final void showDialog (int dialog, String param)
    {
      shower.showDialog (dialog, param);
    }
}


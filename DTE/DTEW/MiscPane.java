// MiscPane.java

// DTE Window Project

// Yunrui Li
// CS Dept. UW-Madison
// Copyright (c) 1999

import java.io.*;
import java.awt.*;
import java.awt.event.*;
import java.lang.*;
import java.util.*;

// Miscellaneous command pane.
public class MiscPane extends UpperComponent implements TextListener
{
  // Main panel.
  protected Panel mainPanel = new Panel ();

  // Drop catalog entry command related components and constants.
  private static final String comDropEntry = "Drop";
  private static final String sDropEntry = "Drop Catalog Entry: "; 
  private TextField tDropEntry = new TextField ("", 30);
  private Button bDropEntry = new Button (comDropEntry);

  // Drop index command related components and constants.
  private static final String comDropIndex = "Drop Index";
  private static final String sTable = "Drop Index Of Table: ";
  private static final String sIndex = "Index Name: ";
  private TextField tTable = new TextField ("", 30);
  private TextField tIndex = new TextField ("", 30);
  private Button bDropIndex = new Button (comDropIndex);
  
  // Show schema command related components and constants.
  private static final String comShow = "Show Schema";
  private static final String sShow = "Show Schema Of: ";
  private TextField tShow = new TextField ("", 30);
  private Button bShow = new Button (comShow);
  
  // Materialize view command related components and constants.
  private static final String comMater = "Materialize";
  private static final String sMater = "Materialize View: ";
  private TextField tMater = new TextField ("", 30);
  private Button bMater = new Button (comMater);
  
  // The panels.
  private Panel pDropEntry = new Panel ();
  private Panel pDropIndex = new Panel ();
  private Panel pShow = new Panel ();
  private Panel pMater = new Panel ();

  // Constructor.
  public MiscPane ()
    {
      mainPanel.setLayout (new GridLayout (0, 1));

      // Add drop catalog entry panel.
      pDropEntry.setLayout (new FlowLayout (FlowLayout.LEFT));
      pDropEntry.add (new Label (sDropEntry));
      pDropEntry.add (tDropEntry);
      pDropEntry.add (bDropEntry);
      
      bDropEntry.setEnabled (false);      
      bDropEntry.addActionListener (this);
      tDropEntry.addTextListener (this);
      
      mainPanel.add (pDropEntry);
     
      // Add drop index panel.
      pDropIndex.setLayout (new FlowLayout (FlowLayout.LEFT));
      pDropIndex.add (new Label (sTable));
      pDropIndex.add (tTable);
      pDropIndex.add (new Label (sIndex));
      pDropIndex.add (tIndex);
      pDropIndex.add (bDropIndex);
      
      bDropIndex.setEnabled (false);      
      bDropIndex.addActionListener (this);
      tTable.addTextListener (this);
      tIndex.addTextListener (this);
      
      mainPanel.add (pDropIndex);
      
      // Add show schema panel.
      pShow.setLayout (new FlowLayout (FlowLayout.LEFT));
      pShow.add (new Label (sShow));
      pShow.add (tShow);
      pShow.add (bShow);
            
      bShow.setEnabled (false);      
      bShow.addActionListener (this);
      tShow.addTextListener (this);
      
      mainPanel.add (pShow);

      // Add materialize view panel.
      pMater.setLayout (new FlowLayout (FlowLayout.LEFT));
      pMater.add (new Label (sMater));
      pMater.add (tMater);
      pMater.add (bMater);
            
      bMater.setEnabled (false);      
      bMater.addActionListener (this);
      tMater.addTextListener (this);
      
      mainPanel.add (pMater);
      
      add (mainPanel);
      setScrollPosition (0, 0);
    }

  // Action event handler.
  public final void actionPerformed (ActionEvent e)
    {
      String com = e.getActionCommand ();
      
      if (com.equals (comDropEntry))
	dropEntry ();
      else if (com.equals (comDropIndex))
	dropIndex ();
      else if (com.equals (comShow))
	showSchema ();
      else if (com.equals (comMater))
	materialize ();
    }

  // Text value changed event handler.
  public final void textValueChanged (TextEvent e)
    {
      Object o = e.getSource ();
      
      if (e.getID () == TextEvent.TEXT_VALUE_CHANGED)
	if (o == tDropEntry)
	  bDropEntry.setEnabled (! (tDropEntry.getText ().trim ().equals ("")));
	else if (o == tTable || o == tIndex)
	  bDropIndex.setEnabled (! (tTable.getText ().trim ().equals ("") || 
				    tIndex.getText ().trim ().equals ("")));
	else if (o == tShow)
	  bShow.setEnabled (! (tShow.getText ().trim ().equals ("")));
	else if (o == tMater)
	  bMater.setEnabled (! (tMater.getText ().trim ().equals ("")));
    }
  
  // Drop a catalog entry.
  private void dropEntry ()
    {
      // Construct the query.
      String name = tDropEntry.getText ().trim ();
      String q = "Delete From . as t where t.name = '" + name + "';";

      // Execute the query.
      getEnvironment ().runQuery (q, this);

      // The catalog is changed.
      updateHome ();
    }

  // Drop an index
  private void dropIndex ()
    {
      // Construct the query.
      String index = tIndex.getText (). trim ();
      String name = tTable.getText ().trim ();
      if (! (name.startsWith (".")))
	name = "." + name;
      
      String q = "Drop Index " + name + " " + index + " ;";

      // Execute the query.
      getEnvironment ().runQuery (q, this);
    }

  // Show schema of a catalog entry.
  private void showSchema ()
    {
      // Construct the query.
      String name = tShow.getText ().trim ();
      if (! (name.startsWith (".")))
	name = "." + name;
	  
      String q = "Schema " + name + ";";

      // Execute the query.
      getEnvironment ().runQuery (q, this);
    }
  
  // Materialize a view.
  private void materialize ()
    {
      // Construct the query.
      String name = tMater.getText ().trim ();
      if (! (name.startsWith (".")))
	name = "." + name;
	  
      String q = "Materialize " + name + ";";

      // Execute the query.
      getEnvironment ().runQuery (q, this);

      // The catalog may have changed.
      updateHome ();
    }
}

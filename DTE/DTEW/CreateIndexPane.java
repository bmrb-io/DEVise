// CreateIndexPane.java

// DTE Window Project

// Yunrui Li
// CS Dept. UW-Madison
// Copyright (c) 1999

import java.io.*;
import java.awt.*;
import java.awt.event.*;
import java.lang.*;
import java.util.*;

// The pane where user can create index for a table.
public class CreateIndexPane extends UpperComponent implements TextListener
{
  protected Panel mainPanel = new Panel ();
  
  // The key attribute and additional attribute panel.
  private SingleListPanel leftPanel;
  private SingleListPanel middlePanel;
  
  // Component constants.
  private static final String sKey = "Key Attributes :";
  private static final String sNonkey = "Additional Attributes :";
  private static final String sField = "Field :";
  private static final String comSetKey = "Modify Key Field";
  private static final String comSetNonkey = "Modify Non-Key Field";
  private static final String comAddKey = "Add As Key Field";
  private static final String comAddNonkey = "Add As Non-Key Field";
  private static final String comClear = "Clear";
  private static final String sIndex = "Index Name :";
  private static final String sTable = "On Table :";
  private static final String comCreate = "Create Index";
  
  // Components.
  private Button bAddKey = new Button (comAddKey);
  private Button bAddNonkey = new Button (comAddNonkey);
  private Button bClear = new Button (comClear);
  private Button bCreate = new Button (comCreate);
  
  // The Attribute list size.
  private static final int nSizeXList = 350;
  private static final int nSizeYList = 100;

  // Components.
  private TextField tBottom = new TextField ("", 30);
  private TextField tIndex = new TextField ("", 30);
  private TextField tTable = new TextField ("", 30);
  
  // Flag to show if the key attribute is not empty.
  private boolean notempty = false;
  
  // Constructor.
  public CreateIndexPane ()
    {
      Panel leftup = new Panel ();
      Panel left = new Panel ();
      
      leftPanel = new SingleListPanel (sKey, sField, comSetKey, this, nSizeXList, nSizeYList);
      middlePanel = new SingleListPanel (sNonkey, sField, comSetNonkey, this, nSizeXList, nSizeYList);

      Panel bottomPanel = makeBottomPanel ();
      Panel rightPanel = makeRightPanel ();
      
      leftup.setLayout (new BorderLayout ());
      leftup.add (leftPanel, "West");
      leftup.add (middlePanel, "East");
      
      left.setLayout (new BorderLayout ());
      left.add (leftup, "North");
      left.add (bottomPanel, "Center");
      
      mainPanel.setLayout (new BorderLayout ());
      mainPanel.add (left, "West");
      mainPanel.add (rightPanel, "Center");
      
      add (mainPanel);
      setScrollPosition (0, 0);
    }
  
  // Construct the bottom panel.
  private Panel makeBottomPanel ()
    {
      Panel p = new Panel ();
      p.setLayout (new FlowLayout ());
      p.add (new Label (sField));
      p.add (tBottom);
      p.add (bAddKey);
      p.add (bAddNonkey);
      p.add (bClear);

      tBottom.addTextListener (this);
      bAddKey.addActionListener (this);
      bAddNonkey.addActionListener (this);
      bClear.addActionListener (this);
      
      bAddKey.setEnabled (false);
      bAddNonkey.setEnabled (false);
      
      return p;
    }
  
  // Construct the right panel.
  private Panel makeRightPanel ()
    {
      Panel p = new Panel (), r = new Panel (), s;
      p.setLayout (new BorderLayout ());
      
      Panel q = new Panel ();
      q.setLayout (new FlowLayout (FlowLayout.LEFT));
      q.add (new Label (sIndex));
      p.add (q, "North");
      p.add (r, "South");
      
      q = new Panel ();
      q.setLayout (new FlowLayout (FlowLayout.LEFT));
      q.add (tIndex);
      r.setLayout (new BorderLayout ());
      r.add (q, "North");
      s = new Panel ();
      r.add (s, "South");
      
      q = new Panel ();
      q.setLayout (new FlowLayout (FlowLayout.LEFT));
      q.add (new Label (sTable));
      s.setLayout (new BorderLayout ());
      s.add (q, "North");
      r = new Panel ();
      s.add (r, "South");
      
      q = new Panel ();
      q.setLayout (new FlowLayout (FlowLayout.LEFT));
      q.add (tTable);
      r.setLayout (new BorderLayout ());
      r.add (q, "North");
      s = new Panel ();
      r.add (s, "South");
      
      q = new Panel ();
      q.setLayout (new FlowLayout (FlowLayout.LEFT));
      q.add (bCreate);
      s.setLayout (new BorderLayout ());
      s.add (q, "North");
      
      s = new Panel ();
      s.setLayout (new BorderLayout ());
      s.add (p, "North");
      
      bCreate.addActionListener (this);
      tTable.addTextListener (this);
      tIndex.addTextListener (this);
      bCreate.setEnabled (false);
      
      return s;
    }
  
  // Action envent handler.
  public final void actionPerformed (ActionEvent e)
    {
      String com = e.getActionCommand ();
      Object o = e.getSource ();
      
      if (com.equals (comAddKey))
	{
	  leftPanel.addItem (tBottom.getText ().trim ());
	 
	  notempty = true;
	  enableAction ();
	}
      else if (com.equals (comAddNonkey))
	middlePanel.addItem (tBottom.getText ().trim ());
      else if (com.equals (comClear))
	tBottom.setText ("");
      else if (com.equals (comCreate))
	create ();
      else if (o == leftPanel)
	{
	  notempty = ((leftPanel.getItemCount () > 0));
	  enableAction ();
	}
    }

  // Set the enabled status of the create button.
  private final void enableAction ()
    {
      bCreate.setEnabled (! (tTable.getText ().trim ().equals ("") || 
			     tIndex.getText ().trim ().equals ("")) && notempty);
    }
  
  // Text value changed handler.
  public final void textValueChanged (TextEvent e)
    {
      Object o = e.getSource ();

      if (e.getID () == TextEvent.TEXT_VALUE_CHANGED)
	if (o == tTable || o == tIndex)
	  enableAction ();
	else if (o == tBottom)
	  {
	    bAddKey.setEnabled (! (tBottom.getText ().trim ().equals ("")));
	    bAddNonkey.setEnabled (! (tBottom.getText ().trim ().equals ("")));
	  }
    }

  // Create the index.
  private void create ()
    {
      // Get the table name, index name and key attributes.
      String table = tTable.getText ().trim ();
      String index = tIndex.getText ().trim ();
      String [] keys = leftPanel.getListItems ();

      // Construct the query.
      String q = "Create rtree Index " + index + " On ";
      q += ((table.startsWith (".")) ? table : ("." + table));
      q += " ( ";
    
      for (int i = 0; i < keys.length; i ++)
	{
	  q += keys [i];
	  q += ((i == keys.length - 1) ? " " : ", ");
	}

      q += ") ";
      
      // If there are additional attributes, add them into the query.
      if (middlePanel.getItemCount () > 0)
	{
	  String [] additional = middlePanel.getListItems ();
	  q += "Add ( ";
	  
	  for (int i = 0; i < additional.length; i ++)
	    {
	      q += additional [i];
	      q += ((i == additional.length - 1) ? " " : ", ");
	    }

	  q += ") ";
	}

      q += ";";

      // Execute the query.
      getEnvironment ().runQuery (q, this);
    }

  // If a show dialog command is received from external world.
  public final void showDialog (int dialog, String param)
    {
      if (dialog == DialogShower.CREATE_INDEX)
	tTable.setText (param);
    }
}


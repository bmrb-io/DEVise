// CreateViewPane.java

// DTE Window Project

// Yunrui Li
// CS Dept. UW-Madison
// Copyright (c) 1999

import java.io.*;
import java.awt.*;
import java.awt.event.*;
import java.lang.*;
import java.util.*;

// The pane where user can create a view.
public class CreateViewPane extends UpperComponent
{
  // Component contants.
  private static final String sTableName = "View Name : ";
  private static final String sFile = "SQL Query : ";
  private static final String sFieldName = "Field Name : ";
  private static final String comCreate = "Create";
  
  private static final String comSet = "Modify Field Name";
  
  // Attribute list size.
  private static final int nSizeXList = 400;
  private static final int nSizeYList = 150;

  // The main panel.
  protected ListActionPanel mainPanel = new ListActionPanel 
  (this, sTableName, sFile, comCreate, nSizeXList, nSizeYList, false, sFieldName, null, comSet);

  // Constructor.
  public CreateViewPane ()
    {
      add (mainPanel);
      setScrollPosition (0, 0);
    }

  // Action event handler.
  public final void actionPerformed (ActionEvent e)
    {
      if (e.getActionCommand ().equals (comCreate))
	create ();
    }

  // Create the view.
  private void create ()
    {
      // Get the view name, definition and attribute names.
      String name = mainPanel.getNameString ();
      String desc = mainPanel.getDesc ();
      String [] items = mainPanel. getListItems ();

      // Construct the query.
      String q = "Insert Into . values ( ";
      q += "'" + name + "', 'SQLView', '";
      q += items.length + " ";
      
      for (int i = 0; i < items.length; q += items [i ++].trim () + " ");
      
      if (desc.startsWith ("\""))
	q += desc + " ') ;";
      else
	q += "\"" + desc + "\" ') ;";
	  
      // Execute the query.
      getEnvironment ().runQuery (q, this);

      // The home catalog has changed.
      updateHome ();
    }
}

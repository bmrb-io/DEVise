// CreateTablePane.java

// DTE Window Project

// Yunrui Li
// CS Dept. UW-Madison
// Copyright (c) 1999

import java.io.*;
import java.awt.*;
import java.awt.event.*;
import java.lang.*;
import java.util.*;

// The pane where user can create a table.
public class CreateTablePane extends UpperComponent
{
  // Component constants.
  private static final String sTableName = "Table Name : ";
  private static final String sFile = "Data File : ";
  private static final String sFieldName = "Field Name : ";
  private static final String sType = "Type : ";
  private static final String comCreate = "Create";
  
  private static final String comSet = "Modify Field";
  
  // Field list size.
  private static final int nSizeXList = 400;
  private static final int nSizeYList = 150;

  // Main panel.
  protected ListActionPanel mainPanel = new ListActionPanel 
  (this, sTableName, sFile, comCreate, nSizeXList, nSizeYList, false, sFieldName, sType, comSet);

  // Constructor.
  public CreateTablePane ()
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

  // Create a table.
  private void create ()
    {
      // Get the table name, data file and schema.
      String name = mainPanel.getNameString ();
      String desc = mainPanel.getDesc ();
      String [] items = mainPanel. getListItems ();

      try 
	{
	  // Clear the data file.
	  new FileWriter (desc).close ();
	}
      catch (IOException ex)
	{
	  writeError (ex);
	}           
      
      // Construct query.
      String q = "Insert Into . values ( ";
      q += "'" + name + "', 'StandardTable', '";
      q += items.length + " ";
      
      for (int i = 0; i < items.length; i ++)
	{
	  String s = items [i];
	  int ind = s.indexOf (" ");
	  q += s.substring (ind).trim () + " ";
	  q += s.substring (0, ind).trim () + " ";
	}
      
      q += desc + " ');";
      
      // Execute the query.
      getEnvironment ().runQuery (q, this);

      // The home catalog has changed.
      updateHome ();
    }
}

// CreateGestaltPane.java

// DTE Window Project

// Yunrui Li
// CS Dept. UW-Madison
// Copyright (c) 1999

import java.io.*;
import java.awt.*;
import java.awt.event.*;
import java.lang.*;
import java.util.*;

// The pane where user can create a gestalt.
public class CreateGestaltPane extends UpperComponent
{
  // Component constants.
  private static final String sGestaltName = "Gestalt Name : ";
  private static final String sFile = "Member File (Optional) : ";
  private static final String sFieldName = "Field Name : ";
  private static final String sType = "Type : ";
  private static final String comCreate = "Create";
  
  private static final String comSet = "Modify Field";
  
  // Field list size.
  private static final int nSizeXList = 400;
  private static final int nSizeYList = 150;

  // The main panel.
  protected ListActionPanel mainPanel = new ListActionPanel 
  (this, sGestaltName, sFile, comCreate, nSizeXList, nSizeYList, true, sFieldName, sType, comSet);

  // Constructor.
  public CreateGestaltPane ()
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

  // Create a gestalt.
  private void create ()
    {
      // Get the gestalt name, member file and schema.
      String name = mainPanel.getNameString ();
      String desc = mainPanel.getDesc ();
      String [] items = mainPanel. getListItems ();
      
      // Construct query.
      String q;
      
      if (desc.trim ().equals (""))
	{
	  // If the member file name is not provided, use "create gestalt" command.
	  q = "Create Gestalt ";
      
	  name = ((name.startsWith (".")) ? name : ("." + name));
	  q += name + " ( ";
	  
	  for (int i = 0; i < items.length; i ++)
	    {
	      String s = items [i];
	      int ind = s.indexOf (" ");
	      q += s.substring (ind).trim () + " ";
	      q += s.substring (0, ind).trim ();
	      q += ((i == items.length - 1) ? " " : ", ");
	    }

	  q += ") ;";
	}
      else
	{
	  // If the member file name if provided, use "insert into ." command.
	  q = "Insert Into . values ( ";
	  q += "'" + name + "', 'Gestalt', '";
	  q += items.length + " ";
	  
	  for (int i = 0; i < items.length; i ++)
	    {
	      String s = items [i];
	      int ind = s.indexOf (" ");
	      q += s.substring (ind).trim () + " ";
	      q += s.substring (0, ind).trim () + " ";
	    }
	  
	  q += desc + " ');";
	}
      
      // Execute query.
      getEnvironment ().runQuery (q, this);

      // The catalog may have changed, update it.
      updateHome ();
    }
}

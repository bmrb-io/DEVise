// ShowGestaltPane.java

// DTE Window Project

// Yunrui Li
// CS Dept. UW-Madison
// Copyright (c) 1999

import java.io.*;
import java.awt.*;
import java.awt.event.*;
import java.lang.*;
import java.util.*;

// The pane where user can review the members of a gestalt, register/unregister to/from a gestalt.
public class ShowGestaltPane extends UpperComponent implements TextListener, ItemListener
{
  // The main panel.
  protected Panel mainPanel = new Panel ();

  // Component constants.
  private static final String comReg = "Register";
  private static final String comUnreg = "Unregister";
  private static final String comShow = "Show";
  private static final String comClear = "Clear";
  private static final String sName = "Name: ";

  // The member list size.
  private static final int nSizeXList = 600;
  private static final int nSizeYList = 200;
  
  // Components.
  private List l = new SizedList (nSizeXList, nSizeYList);
  private Button bReg = new Button (comReg);
  private Button bUnreg = new Button (comUnreg);
  private Button bShow = new Button (comShow);
  private Button bClear = new Button (comClear);
  private TextField tReg = new TextField ("", 30);
  private TextField tUnreg = new TextField ("", 30);
  private TextField tName = new TextField ("", 30);
  
  // Constructor.
  public ShowGestaltPane ()
    {
      // Add the appropriate panels.
      Panel left = new Panel (), right = new Panel ();
      Panel leftup = new Panel (), leftdown = new Panel ();
      Panel rightup = new Panel (), rightdown = new Panel ();
      
      tUnreg.setEditable (false);
      
      leftup.add (new Label (sName));
      leftup.add (tName);
      leftup.add (bShow);
      leftdown.add (l);
      rightup.add (tReg);
      rightup.add (bReg);
      rightup.add (bClear);
      rightdown.add (tUnreg);
      rightdown.add (bUnreg);

      left.setLayout (new BorderLayout ());
      left.add (leftup, "North");
      left.add (leftdown, "Center");

      right.setLayout (new BorderLayout ());
      right.add (rightup, "North");
      right.add (rightdown, "Center");
      
      mainPanel.setLayout (new BorderLayout ());
      mainPanel.add (left, "West");
      mainPanel.add (right, "Center");

      add (mainPanel);
      
      bReg.addActionListener (this);
      bUnreg.addActionListener (this);
      bShow.addActionListener (this);
      bClear.addActionListener (this);

      tReg.addTextListener (this);
      tName.addTextListener (this);
      
      l.addItemListener (this);

      bReg.setEnabled (false);
      bUnreg.setEnabled (false);
      bShow.setEnabled (false);
      setScrollPosition (0, 0);
    }

  // Prefix a name with "."
  private final String getName (TextField t)
    {
      String name = t.getText ().trim ();
      if (! (name.startsWith (".")))
	name = "." + name;

      return name;
    }
  
  // Load a new gestalt.
  private final void loadAll (String var)
    {
      String name = getName (tName).substring (1);
      BufferedReader bf = null;
      
      try
	{
	  // Load the catalog.
	  Catalog catalog = Catalog.load (getEnvironment ());
	  Vector v = catalog.getVector ();
	  PairVector pv = null;
	  int i;
	  
	  // Look for the catalog entry for the gestalt.
	  for (i = 0; i < v.size (); i ++)
	    {
	      pv = (PairVector) (v.elementAt (i));

	      // Throw exception if the entry is not a gestalt.
	      if (pv.find (Catalog.sName).equals (name))
		if (! (pv.find (Catalog.sType).equals (Catalog.sGestalt)))
		  throw new Catalog.ParseException (name + " is not a gestalt");
		else 
		  break;
	    }

	  // If the gestalt cannot be found, throw an exception.
	  if (i == v.size ())
	    throw new Catalog.ParseException ("Could not find gestalt " + name + " in catalog");
	  
	  // Get the member file of the gestalt.
	  bf = new BufferedReader (new FileReader ((String) (pv.find (Catalog.sMemberpath))));
	}
      catch (Catalog.ParseException e)
	{
	  writeError (e);
	  return;
	}
      catch (IOException e)
	{
	  writeError (e);
	  return;
	}

      l.removeAll ();
      tReg.setText ("");
      tUnreg.setText ("");
      bUnreg.setEnabled (false);
      
      try
	{
	  // Read the member file and insert members into the list.
	  String s;
	  for (; (s = bf.readLine ()) != null; l.add (s));
	  bf.close ();
	}
      catch (IOException e)
	{
	  l.removeAll ();
	  writeError (e);
	}

      int size = l.getItemCount ();
      if (var != null)
	{
	  for (int i = 0; i < size; i ++)
	    if (l.getItem (i).equals (var))
	      {
		l.select (i);
		tUnreg.setText (var);
		bUnreg.setEnabled (true);
		break;
	      }
	}
      else if (l.getItemCount () > 0)
	{
	  l.select (0);
	  tUnreg.setText ((String) (l.getItem (0)));
	  bUnreg.setEnabled (true);
	}
      
      validate ();
    }
  
  // Register a member into the current gestalt.
  private void register ()
    {
      String name = getName (tName);
      String mem = getName (tReg);
      String q = "Register " + mem + " into gestalt " + name + ";";
      
      getEnvironment ().runQuery (q, this);
      loadAll (mem);
    }
  
  // Unregister a member from the current gestalt.
  private void unregister ()
    {
      String name = getName (tName);
      String mem = getName (tUnreg);      
      String q = "Unregister " + mem + " from gestalt " + name + ";";

      getEnvironment ().runQuery (q, this);
      loadAll (null);
    }
    
  // Action event handler.
  public final void actionPerformed (ActionEvent e)
    {
      String com = e.getActionCommand ();
      
      if (com.equals (comReg))
	register ();
      else if (com.equals (comUnreg))
	unregister ();
      else if (com.equals (comShow))
	loadAll (null);
      else if (com.equals (comClear))
	tReg.setText ("");
    }

  // Text value change event handler.
  public final void textValueChanged (TextEvent e)
    {
      if (e.getID () == TextEvent.TEXT_VALUE_CHANGED)
	if (e.getSource () == tReg)
	  bReg.setEnabled (! (tReg.getText ().trim ().equals ("")));
	else if (e.getSource () == tName)
	  bShow.setEnabled (! (tName.getText ().trim ().equals ("")));
    }
  
  // List selection changed event handler.
  public final void itemStateChanged (ItemEvent e)
    {
      if (e.getID () == ItemEvent.ITEM_STATE_CHANGED)
	{
	  tUnreg.setText (l.getSelectedItem ());
	  bUnreg.setEnabled (true);
	}
    }

  // Process the show dialog command from external world.
  public final void showDialog (int dialog, String param)
    {
      // If the command is a show gestalt command, load the gestalt.
      if (dialog == DialogShower.SHOW_GESTALT)
	{
	  tName.setText (param);
	  loadAll (null);
	}
    }
}

// ShowEnvironmentsPane.java

// DTE Window Project

// Yunrui Li
// CS Dept. UW-Madison
// Copyright (c) 1999

import java.io.*;
import java.awt.*;
import java.awt.event.*;
import java.lang.*;
import java.util.*;

// The pane where user can review and modify the environment variable values.
public class ShowEnvironmentsPane extends UpperComponent implements TextListener, ItemListener
{
  // The main panel.
  protected Panel mainPanel = new Panel ();

  // The component constants.
  private static final String comSet = "Set";
  private static final String comClear = "Clear";
  private static final String sEqual = " = ";

  // The list size.
  private static final int nSizeXList = 600;
  private static final int nSizeYList = 200;
  
  // Components.
  private List l = new SizedList (nSizeXList, nSizeYList);
  private Button bSet = new Button (comSet);
  private Button bClear = new Button (comClear);
  private TextField tVar = new TextField ("", 30);
  private TextField tValue = new TextField ("", 30);

  // Constructor
  public ShowEnvironmentsPane ()
    {
      Panel bottom = new Panel (), up = new Panel ();

      bottom.add (tVar);
      bottom.add (new Label (sEqual));
      bottom.add (tValue);
      bottom.add (bSet);
      bottom.add (bClear);
      
      up.add (l);
      
      mainPanel.setLayout (new BorderLayout ());
      mainPanel.add (up, "North");
      mainPanel.add (bottom, "Center");

      add (mainPanel);
      
      bSet.addActionListener (this);
      bClear.addActionListener (this);
      l.addItemListener (this);
      tVar.addTextListener (this);

      bSet.setEnabled (false);
      setScrollPosition (0, 0);
    }

  // Set the Environment.
  public final void setEnvironment (Environment env)
    {
      super.setEnvironment (env);
      loadEnvironments (null);
    }
  
  // Load environment variables.
  private final void loadEnvironments (String var)
    {
      // Remove old list entries.
      l.removeAll ();

      // Insert new list entries.
      PairVector v = getEnvironment ().getenv ();
      for (int i = 0; i < v.size (); i ++)
	l.addItem ((String) (v.elementAtFirst (i)) + "  =  " + (String) (v.elementAtSecond (i)));
      
      int size = l.getItemCount ();
      if (var != null)
	{
	  for (int i = 0; i < size; i ++)
	    if (l.getItem (i).startsWith (var + " "))
	      {
		l.select (i);
		break;
	      }
	}
      else if (size > 0)
	{
	  l.select (0);
	  tVar.setText ((String) (v.elementAtFirst (0)));
	  tValue.setText ((String) (v.elementAtSecond (0)));
	}
      
      validate ();
    }
  
  // Action event handler.
  public final void actionPerformed (ActionEvent e)
    {
      String com = e.getActionCommand ();
      
      if (com.equals (comSet))
	{
	  String var = tVar.getText ().trim ();
	  getEnvironment ().setenv (var, tValue.getText ().trim ());
	  loadEnvironments (var);
	}
      else if (com.equals (comClear))
	{
	  tVar.setText ("");
	  tValue.setText ("");
	}
    }

  // Text value changed event  handler.
  public final void textValueChanged (TextEvent e)
    {
      if (e.getID () == TextEvent.TEXT_VALUE_CHANGED)
	bSet.setEnabled (! (tVar.getText ().trim ().equals ("")));
    }
  
  // List selection state change handler.
  public final void itemStateChanged (ItemEvent e)
    {
      if (e.getID () == ItemEvent.ITEM_STATE_CHANGED)
	{
	  String s = l.getSelectedItem ();
	  int ind = s.indexOf ("=");
	  tVar.setText (s.substring (0, ind).trim ());
	  tValue.setText (s.substring (ind + 1).trim ());
	}  	
    }
}

// ListActionPanel.java

// DTE Window Project

// Yunrui Li
// CS Dept. UW-Madison
// Copyright (c) 1999

import java.io.*;
import java.awt.*;
import java.awt.event.*;
import java.lang.*;
import java.util.*;

// ListActionPanel encapsulates the list manipulation (insertion/deletion/move up/move down/...).
// Besides a list, a ListActionPanel also contains a name field and a description field and an action button.
public class ListActionPanel extends Panel implements TextListener, ItemListener, ActionListener
{
  // A list and an action button.
  private List l;
  private Button bAction;

  // Component constants.
  private static final String comSet = "Set";
  private static final String comAdd = "Add";
  private static final String comClear = "Clear";
  private static final String comMoveUp = "Move Up";
  private static final String comMoveDown = "Move Down";
  private static final String comRemove = "Remove";
  
  // Components.
  private Button bSet;
  private String comSetButton;
  
  private Button bAdd = new Button (comAdd);
  private Button bClear = new Button (comClear);
  private Button bMoveUp = new Button (comMoveUp);
  private Button bMoveDown = new Button (comMoveDown);
  private Button bRemove = new Button (comRemove);
  
  private TextField tName = new TextField ("", 30);
  private TextField tDesc = new TextField ("", 30);
  private TextField tsPart1 = new TextField ("", 30);
  private TextField tsPart2 = null;
  private TextField taPart1 = new TextField ("", 30);
  private TextField taPart2 = null;

  // The parent.
  private ActionListener parent;

  // Parameters.
  private String name;
  private String desc;
  private String action;
  private int xSize;
  private int ySize;
  private String part1;
  private String part2;

  // Flag to show whether the list is a single text list or double text list.
  private boolean notsingle;

  // Flag to show that description field is optional
  private boolean optional;
  
  // List not empty flag.
  private boolean notempty = false;
  
  // Constructor.
  public ListActionPanel (ActionListener parent, String name, String desc, String action, int x, int y,
			  boolean descoptional, String part1, String part2, String set)
    {
      this.parent = parent;
      this.name = name;
      this.desc = desc;
      this.action = action;
      this.xSize = x;
      this.ySize = y;
      this.part1 = part1;
      this.part2 = part2;
      this.comSetButton = set;
      this.optional = descoptional;
      
      notsingle = (part2 != null);
      bSet = new Button (set);
      
      // Set up panels.
      Panel up = new Panel ();
      Panel middle = new Panel ();
      Panel bottom = new Panel ();
      Panel midbot = new Panel ();
      Panel left = new Panel ();
      Panel right = new Panel ();
      Panel rightup = new Panel ();
      Panel rightmid = new Panel ();
      Panel rightbot = new Panel ();    

      left.setLayout (new FlowLayout ());
      left.add (l = new SizedList (xSize, ySize));
      
      rightup.setLayout (new FlowLayout ());
      rightup.add (new Label (name));
      rightup.add (tName);
      rightup.add (bAction = new Button (action));

      rightmid.setLayout (new FlowLayout ());
      rightmid.add (new Label (desc));
      rightmid.add (tDesc);

      rightbot.setLayout (new FlowLayout ());
      rightbot.add (bMoveUp);
      rightbot.add (bMoveDown);
      rightbot.add (bRemove);
      
      right.setLayout (new BorderLayout ());
      right.add (rightup, "North");
      right.add (rightmid, "Center");
      right.add (rightbot, "South");

      up.setLayout (new BorderLayout ());
      up.add (left, "West");
      up.add (right, "Center");
      
      middle.setLayout (new FlowLayout ());
      middle.add (new Label (part1));
      middle.add (tsPart1);

      if (notsingle)
	{
	  middle.add (new Label (part2));
	  middle.add (tsPart2 = new TextField ("", 30));
	}
      
      middle.add (bSet);
      
      bottom.setLayout (new FlowLayout ());
      bottom.add (new Label (part1));
      bottom.add (taPart1);
      
      if (notsingle)
	{
	  bottom.add (new Label (part2));
	  bottom.add (taPart2 = new TextField ("", 30));
	}

      bottom.add (bAdd);
      bottom.add (bClear);
      
      midbot.setLayout (new BorderLayout ());
      midbot.add (middle, "North");
      midbot.add (bottom, "Center");
      
      setLayout (new BorderLayout ());
      add (up, "North");
      add (midbot, "Center");

      bAction.addActionListener (parent);
      bSet.addActionListener (this);
      bAdd.addActionListener (this);
      bClear.addActionListener (this);
      bMoveUp.addActionListener (this);
      bMoveDown.addActionListener (this);
      bRemove.addActionListener (this);
      
      l.addItemListener (this);
      tsPart1.addTextListener (this);
      taPart1.addTextListener (this);
      
      if (notsingle)
	{
	  tsPart2.addTextListener (this);
	  taPart2.addTextListener (this);
	}

      tName.addTextListener (this);
      tDesc.addTextListener (this);
      
      bMoveUp.setEnabled (false);
      bMoveDown.setEnabled (false);
      bRemove.setEnabled (false);
      bSet.setEnabled (false);
      bAdd.setEnabled (false);
      bAction.setEnabled (false);
      tsPart1.setEnabled (false);
      if (notsingle)
	tsPart2.setEnabled (false);
    }

  // Constructor.
  public ListActionPanel (ActionListener parent, String name, String desc, String action, int x, int y,
			  boolean descoptional, String part1, String part2)
    {
      this (parent, name, desc, action, x, y, descoptional, part1, part2, comSet);
    }

  // Return the string in the name text field.
  public final String getNameString ()
    {
      return tName.getText ();
    }

  // Return the string in the description text field.
  public final String getDesc ()
    {
      return tDesc.getText ();
    }

  // Return the list items.
  public final String [] getListItems ()
    {
      return l.getItems ();
    }

  // Action event handler.
  public final void actionPerformed (ActionEvent e)
    {
      String com = e.getActionCommand ();
      int index = l.getSelectedIndex ();
      int count = l.getItemCount ();
      
      if (com.equals (comSetButton))
	{
	  // List item value modify button is pressed.
	  String s = tsPart1.getText ();
	  if (notsingle)
	    s += "  " + tsPart2.getText ();

	  l.replaceItem (s, index);
	  redraw (index);
	}
      else if (com.equals (comAdd))
	{
	  // List item add button is pressed.
	  String s = taPart1.getText ();
	  if (notsingle)
	    s += "  " + taPart2.getText ();
	  
	  l.add (s);
	  
	  notempty = true;
	  enableAction ();
	  
	  redraw (count);
	}
      else if (com.equals (comClear))
	{
	  // Item text field clear button is pressed.
	  taPart1.setText ("");
	  if (notsingle)
	    taPart2.setText ("");
	}
      else if (com.equals (comMoveUp))
	{
	  // Move up current list item.
	  String s = l.getSelectedItem ();
	  String t = l.getItem (index - 1);
	  l.replaceItem (s, index - 1);
	  l.replaceItem (t, index);
	  redraw (index - 1);
	}
      else if (com.equals (comMoveDown))
	{
	  // Move down current list item.
	  String s = l.getSelectedItem ();
	  String t = l.getItem (index + 1);
	  l.replaceItem (s, index + 1);
	  l.replaceItem (t, index);
	  redraw (index + 1);
	}
      else if (com.equals (comRemove))
	{
	  // Remove current list item.
	  l.remove (index);
	  if (index == count - 1)
	    redraw (count - 2);
	  else
	    redraw (index);

	  notempty = ((count > 1) ? true : false);
	  enableAction ();
	}
    }

  // Redraw the whole panel. 
  private final void redraw (int index)
    {
      // If the list becomes empty.
      if (index == -1)
	{
	  bMoveUp.setEnabled (false);
	  bMoveDown.setEnabled (false);
	  bRemove.setEnabled (false);
	  tsPart1.setText ("");
	  tsPart1.setEnabled (false);
	  if (notsingle)
	    {
	      tsPart2.setText ("");
	      tsPart2.setEnabled (false);
	    }
	  bSet.setEnabled (false);
	}
      else
	{
	  // Redraw the list and set the enabled status of other components.
	  l.select (index);
	  bRemove.setEnabled (true);

	  String s = l.getSelectedItem ();

	  tsPart1.setEnabled (true);
	  bSet.setEnabled (true);
	  if (notsingle)
	    {
	      tsPart2.setEnabled (true);
	      int ind = s.indexOf (" ");
	      tsPart1.setText (s.substring (0, ind).trim ());
	      tsPart2.setText (s.substring (ind).trim ());
	    }
	  else
	    tsPart1.setText (s);

	  int curcount = l.getItemCount ();
      
	  bMoveUp.setEnabled (index != 0);
	  bMoveDown.setEnabled (index != curcount - 1);
	}

      validate ();
    }

  // Set the enabled status of the action button.
  private final void enableAction ()
    {
      bAction.setEnabled (! (tName.getText ().trim ().equals ("")) && 
			  (! (tDesc.getText ().trim ().equals ("")) || optional) &&
			  notempty);
    }
  
  // Text value changed event handler. Enable/disable certain components.
  public final void textValueChanged (TextEvent e)
    {
      Object o = e.getSource ();

      if (e.getID () == TextEvent.TEXT_VALUE_CHANGED)
	if (o == tName || o == tDesc)
	  enableAction ();
	else if (notsingle)
	  {
	    if (o == tsPart1 || o == tsPart2)
	      bSet.setEnabled (! (tsPart1.getText ().trim ().equals ("") || 
				  tsPart2.getText ().trim ().equals ("")));
	    else if (o == taPart1 || o == taPart2)
	      bAdd.setEnabled (! (taPart1.getText ().trim ().equals ("") || 
				  taPart2.getText ().trim ().equals ("")));
	  }
	else
	  {
	    if (o == tsPart1)
	      bSet.setEnabled (! (tsPart1.getText ().trim ().equals ("")));
	    else if (o == taPart1)
	      bAdd.setEnabled (! (taPart1.getText ().trim ().equals ("")));
	  }
    }

  // List selection change handler.
  public final void itemStateChanged (ItemEvent e)
    {
      String s;
      
      if (e.getID () == ItemEvent.ITEM_STATE_CHANGED && (s = l.getSelectedItem ()) != null)
	{
	  if (notsingle)
	    {
	      int ind = s.indexOf (" ");
	      tsPart1.setText (s.substring (0, ind).trim ());
	      tsPart2.setText (s.substring (ind).trim ());
	    }
	  else
	    tsPart1.setText (s);
	  
	  bSet.setEnabled (true);
	}
      
      int curindex = l.getSelectedIndex ();
      int curcount = l.getItemCount ();
      
      bMoveUp.setEnabled (curindex != 0);
      bMoveDown.setEnabled (curindex != curcount - 1);
    }
}


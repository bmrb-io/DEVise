// SingleListPanel.java

// DTE Window Project

// Yunrui Li
// CS Dept. UW-Madison
// Copyright (c) 1999

import java.io.*;
import java.awt.*;
import java.awt.event.*;
import java.lang.*;
import java.util.*;

// SingleListPanel encapsulates list operation, including Move Up/Move Down/Remove list items.
public class SingleListPanel extends Panel implements TextListener, ItemListener, ActionListener
{
  // The main panel.
  private Panel mainPanel = new Panel ();
  
  // The parent, who is notified when the number of items in the list changes.
  private ActionListener parent;

  // Parameters.
  private String title;
  private String prompt;
  private String action;
  private int sizex;
  private int sizey;
  
  // Component constants.
  private static final String comMoveUp = "Move Up";
  private static final String comMoveDown = "Move Down";
  private static final String comRemove = "Remove";
  
  // Components.
  private Button bMoveUp = new Button (comMoveUp);
  private Button bMoveDown = new Button (comMoveDown);
  private Button bRemove = new Button (comRemove);
  private Button bSet;
  
  private List l;
  private TextField t = new TextField ("", 25);

  // Constructor.
  public SingleListPanel (String title, String prompt, String action, ActionListener parent, int x, int y)
    {
      this.title = title;
      this.prompt = prompt;
      this.action = action;
      this.parent = parent;
      this.sizex = x;
      this.sizey = y;
      
      l = new SizedList (sizex, sizey);
      bSet = new Button (action);

      mainPanel.setLayout (new BorderLayout ());
      
      Panel p = new Panel ();
      Panel q = new Panel ();
      Panel r;
      
      p.add (new Label (title));
      mainPanel.add (p, "North");
      mainPanel.add (q, "South");
      q.setLayout (new BorderLayout ());
      
      p = new Panel ();
      p.add (l);
      q.add (p, "North");
      r = new Panel ();
      q.add (r, "South");
      r.setLayout (new BorderLayout ());
      
      p = new Panel ();
      p.add (bMoveUp);
      p.add (bMoveDown);
      p.add (bRemove);
      r.add (p, "North");
      q = new Panel ();
      r.add (q, "South");
      q.setLayout (new BorderLayout ());
      
      p = new Panel ();
      p.add (new Label (prompt));
      p.add (t);
      p.add (bSet);
      q.add (p, "North");

      add (mainPanel);
      
      bMoveUp.addActionListener (this);
      bMoveDown.addActionListener (this);
      bRemove.addActionListener (this);
      bSet.addActionListener (this);
      t.addTextListener (this);
      l.addItemListener (this);

      t.setEnabled (false);
      bMoveUp.setEnabled (false);
      bMoveDown.setEnabled (false);
      bRemove.setEnabled (false);
      bSet.setEnabled (false);
    }

  // Return the list items.
  public final String [] getListItems ()
    {
      return l.getItems ();
    }

  // Return the list item count.
  public final int getItemCount ()
    {
      return l.getItemCount ();
    }
  
  // Add an item into the list.
  public final void addItem (String s)
    {
      l.add (s);
      redraw (l.getItemCount () - 1);
    }
  
  // Action event handler.
  public final void actionPerformed (ActionEvent e)
    {
      String com = e.getActionCommand ();
      int index = l.getSelectedIndex ();
      int count = l.getItemCount ();
      
      if (com.equals (action))
	{
	  // Change the current list item.
	  String s = t.getText ();
	  l.replaceItem (s, index);
	  redraw (index);
	}
      else if (com.equals (comMoveUp))
	{
	  // Move up the current list item.
	  String s = l.getSelectedItem ();
	  String t = l.getItem (index - 1);
	  l.replaceItem (s, index - 1);
	  l.replaceItem (t, index);
	  redraw (index - 1);
	}
      else if (com.equals (comMoveDown))
	{
	  // Move down the current list item.
	  String s = l.getSelectedItem ();
	  String t = l.getItem (index + 1);
	  l.replaceItem (s, index + 1);
	  l.replaceItem (t, index);
	  redraw (index + 1);
	}
      else if (com.equals (comRemove))
	{
	  // Remove the current list item.
	  l.remove (index);
	  if (index == count - 1)
	    redraw (count - 2);
	  else
	    redraw (index);

	  parent.actionPerformed (new ActionEvent (this, 0, comRemove));
	}
    }

  // Redraw the whole panel.
  private final void redraw (int index)
    {
      if (index == -1)
	{
	  bMoveUp.setEnabled (false);
	  bMoveDown.setEnabled (false);
	  bRemove.setEnabled (false);
	  t.setText ("");
	  t.setEnabled (false);
	  bSet.setEnabled (false);
	}
      else
	{
	  l.select (index);
	  bRemove.setEnabled (true);

	  String s = l.getSelectedItem ();

	  t.setEnabled (true);
	  bSet.setEnabled (true);
	  t.setText (s);

	  int curcount = l.getItemCount ();
      
	  bMoveUp.setEnabled (index != 0);
	  bMoveDown.setEnabled (index != curcount - 1);
	}

      validate ();
    }

  // Text value changed event handler.
  public final void textValueChanged (TextEvent e)
    {
      if (e.getID () == TextEvent.TEXT_VALUE_CHANGED)
	bSet.setEnabled (! (t.getText ().trim ().equals ("")));
    }
  
  // List selection changed event handler.
  public final void itemStateChanged (ItemEvent e)
    {
      String s;
      
      if (e.getID () == ItemEvent.ITEM_STATE_CHANGED && (s = l.getSelectedItem ()) != null)
	{
	  t.setText (s);
	  bSet.setEnabled (true);
	}
      
      int curindex = l.getSelectedIndex ();
      int curcount = l.getItemCount ();
      
      bMoveUp.setEnabled (curindex != 0);
      bMoveDown.setEnabled (curindex != curcount - 1);
    }
}



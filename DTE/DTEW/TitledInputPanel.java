// TitledInputPanel.java

// DTE Window Project

// Yunrui Li
// CS Dept. UW-Madison
// Copyright (c) 1999

import java.io.*;
import java.awt.*;
import java.awt.event.*;
import java.lang.*;
import java.util.*;

// TitleInputPanel encapsulates the operations on several text fields, with a title for the whole panel.
public class TitledInputPanel extends Panel implements TextListener, ActionListener
{
  // The parent.
  private ActionListener parent = null;

  // The title.
  private String title = null;

  // The action button command.
  private String action = null;

  // The prompt strings for each tect fields.
  private String [] prompts = null;
  private int size;

  // Text fields and the button.
  private TextField [] ts = null;
  private Button bAction = null;
  
  // The color and font for the title.
  private static final Color leftColor = new Color (192, 0, 192);
  private static final Font leftFont = new Font ("Courier", Font.BOLD, 12);

  // Constructor.
  public TitledInputPanel (String title, String action, ActionListener parent)
    {
      init (title, action, parent, new String [0]);
    }
  
  // Constructor.
  public TitledInputPanel (String title, String action, ActionListener parent, String p)
    {
      String [] prompts = new String []
	{
	  p
	};
	
      init (title, action, parent, prompts);
    }
  
  // Constructor.
  public TitledInputPanel (String title, String action, ActionListener parent, String p1, String p2)
    {
      String [] prompts = new String []
	{
	  p1,
	  p2
	};
	
      init (title, action, parent, prompts);
    }
  
  // Constructor.
  public TitledInputPanel (String title, String action, ActionListener parent, 
			   String p1, String p2, String p3)
    {
      String [] prompts = new String []
	{
	  p1,
	  p2,
	  p3
	};
	
      init (title, action, parent, prompts);
    }
  
  // Constructor.
  public TitledInputPanel (String title, String action, ActionListener parent, String [] prompts)
    {
      init (title, action, parent, prompts);
    }
  
  // Initializor.
  private void init (String title, String action, ActionListener parent, String [] prompts)
    {
      this.title = title;
      this.action = action;
      this.parent = parent;
      this.prompts = prompts;

      // Create text fields.
      size = prompts.length;
      ts = new TextField [size];
      for (int i = 0; i < size; ts [i ++] = new TextField ("", 25));

      setLayout (new FlowLayout (FlowLayout.LEFT));

      // Add components.
      Label l = new Label (title);
      l.setBackground (leftColor);
      l.setFont (leftFont);
      add (l);
      
      for (int i = 0; i < size; ts [i ++].addTextListener (this))
	{
	  add (new Label (prompts [i]));
	  add (ts [i]);
	}

      bAction = new Button (action);
      add (bAction);
      
      bAction.addActionListener (this);
      bAction.setEnabled (size == 0);
    }
  
  // Return the text in the i'th text field.
  public final String getText (int i)
    {
      return ts [i].getText ();
    }
  
  // Action event handler.
  public final void actionPerformed (ActionEvent e)
    {
      if (e.getActionCommand ().equals (action))
	parent.actionPerformed (new ActionEvent (this, 0, action));
    }

  // Text value changed event handler.
  public final void textValueChanged (TextEvent e)
    {
      if (e.getID () == TextEvent.TEXT_VALUE_CHANGED)
	{
	  for (int i = 0; i < size; i ++)
	    if (ts [i].getText ().trim ().equals (""))
	      {
		bAction.setEnabled (false);
		return;
	      }
	  
	  bAction.setEnabled (true);
	}
    }
}

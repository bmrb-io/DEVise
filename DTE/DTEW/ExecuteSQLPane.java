// ExecuteSQLPane.java

// DTE Window Project

// Yunrui Li
// CS Dept. UW-Madison
// Copyright (c) 1999

import java.io.*;
import java.awt.*;
import java.awt.event.*;
import java.lang.*;
import java.util.*;

// The pane where user can execute any SQL commands.
public class ExecuteSQLPane extends UpperComponent
{
  // The main panel.
  protected Panel mainPanel = new Panel ();

  // Component constants.
  private static final String prompt = "Enter SQL Command: ";
  private static final String comExec = "Execute";
  private static final String comClear = "Clear";
  
  // Components.
  private TextArea ta = new TextArea ();
  private Button bExecute = new Button (comExec);
  private Button bClear = new Button (comClear);
  
  // Constructor.
  public ExecuteSQLPane ()
    {
      mainPanel.add (new Label (prompt));
      mainPanel.add (ta);
      mainPanel.add (bExecute);
      mainPanel.add (bClear);
      
      add (mainPanel);
      
      bExecute.addActionListener (this);
      bClear.addActionListener (this);
      setScrollPosition (0, 0);
    }
  
  // Action event handler.
  public final void actionPerformed (ActionEvent e)
    {
      String com = e.getActionCommand ();
      
      if (com.equals (comExec))
	getEnvironment ().runQuery (ta.getText (), this);
      else if (com.equals (comClear))
	ta.setText ("");
    }
}

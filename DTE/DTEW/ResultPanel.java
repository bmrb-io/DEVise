// ResultPanel.java

// DTE Window Project

// Yunrui Li
// CS Dept. UW-Madison
// Copyright (c) 1999

import java.io.*;
import java.awt.*;
import java.awt.event.*;
import java.lang.*;
import java.util.*;

// The result panel.
public class ResultPanel extends ScrollPane implements ResultOutputer, SizedComponent
{
  // The size of the output pane and the error pane.
  private static final int nSizeXHalf = 580;
  private static final int nSizeYHalf = 290;
  
  // The main panel.
  private Panel mainPanel = new Panel ();

  // The output text area and the error text area.
  private TextArea outArea = new SizedTextArea (nSizeXHalf, nSizeYHalf);
  private TextArea errArea = new SizedTextArea (nSizeXHalf, nSizeYHalf);

  // The output writer and the erro writer.
  private AreaPrintWriter outWriter = new AreaPrintWriter (outArea);
  private AreaPrintWriter errWriter = new AreaPrintWriter (errArea);
  
  // The panel size.
  private static final int nSizeX = 600;
  private static final int nSizeY = 600;

  // Constructor.
  public ResultPanel ()
    {
      super (ScrollPane.SCROLLBARS_ALWAYS);
      
      // Set the two text area as not editable.
      outArea.setEditable (false);
      errArea.setEditable (false);
      
      mainPanel.setLayout (new BorderLayout ());
      mainPanel.add (outArea, "North");
      mainPanel.add (errArea, "South");

      add (mainPanel);
      setScrollPosition (0, 0);
    }
  
  public Dimension getPreferredSize ()
    {
      return ScreenScale.getDimension (nSizeX, nSizeY);
    }

  // Write as output.
  public final void writeOutput (InputStream i)
    {
      outputText (i, outWriter);
    }
  
  // Write as error.
  public final void writeError (InputStream i)
    {
      outputText (i, errWriter);
    }

  // Write as output.
  public final void writeOutput (String s)
    {
      outputText (s, outWriter);
    }
  
  // Write as error.
  public final void writeError (String s)
    {
      outputText (s, errWriter);
    }

  // Write as error.
  public final void writeError (Throwable e)
    {
      errWriter.clear ();
      e.printStackTrace (errWriter);
      errWriter.validate ();
    }

  // Write into a text area.
  private final void outputText (String s, AreaPrintWriter a)
    {
      a.clear ();
      a.print (s);
      a.validate ();
    }
  
  // Write into a text area.
  private void outputText (InputStream i, AreaPrintWriter a)
    {
      try
	{
	  a.clear ();
	  BufferedReader bf = new BufferedReader (new InputStreamReader (i));
	  String s;
	  
	  while ((s = bf.readLine ()) != null)
	    a.println (s);

	  bf.close ();
	  a.validate ();
	}
      catch (IOException e)
	{
	  writeError (e);
	}
    }
}

// AreaPrintWriter.java

// DTE Window Project

// Yunrui Li
// CS Dept. UW-Madison
// Copyright (c) 1999

import java.io.*;
import java.awt.*;
import java.awt.event.*;
import java.lang.*;
import java.util.*;

// The print writer that outputs to an text area.
public class AreaPrintWriter extends PrintWriter
{
  // The output stream that outputs to an text area
  private static class AreaOutputStream extends OutputStream
  {
    // The text area.
    private TextArea text;
    
    // Constructor
    public AreaOutputStream (TextArea t)
      {
	this.text = t;
      }

    // Write a single character.
    public final void write (int b)
      {
	String s = "";
	text.append (s + (char) b);
      }
  }
  
  // The text area.
  private TextArea text;
  
  // Constructor
  public AreaPrintWriter (TextArea t)
    {
      super (new BufferedOutputStream (new AreaOutputStream (t)));
      text = t;
    }

  // Clear the text area.
  public final void clear ()
    {
      text.setText ("");
    }
  
  // Flush all data to the text area and show it.
  public final void validate ()
    {
      flush ();
      text.validate ();
    }
}


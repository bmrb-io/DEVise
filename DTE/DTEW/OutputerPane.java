// OutputerPane.java

// DTE Window Project

// Yunrui Li
// CS Dept. UW-Madison
// Copyright (c) 1999

import java.io.*;
import java.awt.*;
import java.awt.event.*;
import java.lang.*;
import java.util.*;

// The combination of scroll pane and result outputer which redirects results to other result outputers.
public class OutputerPane extends ScrollPane implements ResultOutputer
{ 
  // The redirect result outputer.
  private ResultOutputer redirect = null;

  // Constructor.
  public OutputerPane ()
    {
    }
  
  // Constructor.
  public OutputerPane (int scroll)
    {
      super (scroll);
    }
  
  // Set the redirect.
  public final void setOutputerRedirect (ResultOutputer r)
    {
      redirect = r;
    }
  
  // Write as output.
  public final void writeOutput (InputStream i)
    {
      redirect.writeOutput (i);
    }
  
  // Write as error.
  public final void writeError (InputStream i)
    {
      redirect.writeError (i);
    }

  // Write as output.
  public final void writeOutput (String s)
    {
      redirect.writeOutput (s);
    }
  
  // Write as error.
  public final void writeError (String s)
    {
      redirect.writeError (s);
    }

  // Write as error.
  public final void writeError (Throwable e)
    {
      redirect.writeError (e);
    }
}

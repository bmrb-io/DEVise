// ScreenScale.java

// DTE Window Project

// Yunrui Li
// CS Dept. UW-Madison
// Copyright (c) 1999

import java.io.*;
import java.awt.*;
import java.awt.event.*;
import java.lang.*;

// Class ScreenScale scales component sizes to fit in the current screen. 
public class ScreenScale
{
  // The virtual screen size.
  private static final int standardX = 1280;
  private static final int standardY = 1024;

  // The size ratio between the actural screen and the virtual screen.
  private static double ratioX;
  private static double ratioY;
  
  static
    {
      // Get the size ratio.
      Dimension d = Toolkit.getDefaultToolkit ().getScreenSize ();
      ratioX = d.width / (double) standardX;
      ratioY = d.height / (double) standardY;
    }
  
  // Get the scaled dimension.
  public static final Dimension getDimension (int x, int y)
    {
      return new Dimension ((int) (x * ratioX), (int) (y * ratioY));
    }

  // Set the scaled location.
  public static final void setLocation (int x, int y, Component c)
    {
      c.setLocation ((int) (x * ratioX), (int) (y * ratioY));
    }
}


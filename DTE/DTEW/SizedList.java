// SizedList.java

// DTE Window Project

// Yunrui Li
// CS Dept. UW-Madison
// Copyright (c) 1999

import java.awt.*;

// List with preferred size.
public class SizedList extends List implements SizedComponent
{
  // Preferred size.
  private int x;
  private int y;
  
  // Constructor.
  public SizedList (int x, int y)
    {
      this.x = x;
      this.y = y;
    }
  
  // Return the preferred size.
  public final Dimension getPreferredSize ()
    {
      return ScreenScale.getDimension (x, y);
    }
}


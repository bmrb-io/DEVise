// DialogShower.java

// DTE Window Project

// Yunrui Li
// CS Dept. UW-Madison
// Copyright (c) 1999

// A dialog shower processes show dialog commands.
public interface DialogShower
{
  // Command types.
  public static final int CREATE_INDEX = 0;
  public static final int SHOW_GESTALT = 1;
  
  // Process dialog command with parameter.
  public void showDialog (int dialog, String param);
}

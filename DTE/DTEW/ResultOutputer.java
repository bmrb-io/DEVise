// ResultOutputer.java

// DTE Window Project

// Yunrui Li
// CS Dept. UW-Madison
// Copyright (c) 1999

import java.io.*;

// An instance of ResultOutputer is able to write information as output or error.
public interface ResultOutputer
{
  public void writeOutput (InputStream i);
  public void writeError (InputStream i);
  public void writeOutput (String s);
  public void writeError (String s);
  public void writeError (Throwable e);
}

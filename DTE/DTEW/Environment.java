// Environment.java

// DTE Window Project

// Yunrui Li
// CS Dept. UW-Madison
// Copyright (c) 1999

import java.util.*;
import java.io.*;
import java.lang.*;

// Class Environment represents current environment variable settings.
public class Environment
{
  // Devise related environment variables.
  private static final String [] envVar =
  {
    "DEVISE_HOME_TABLE",
    "DEVISE_MINMAX_TABLE",
    "DEVISE_MATER_DIR",
    "DEVISE_TEMP_FILE_DIR",
    "DEVISE_ID_FILE",
    "DEVISE_DEF_FILE",
    "DEVISE_INDEX_TABLE",
    "DEVISE_CONVERT_BULK",
    "DEVISE_TEMP_FILE_DIR"
  };
  
  // DTE Window related environment variables.
  private static final String [] envVarDTEW =
  {
    "DTEW_WORK_DIR",
    "DTEW_DTE_EXEC",
    "DTEW_TEMP_STDIN",
    "DTEW_TEMP_STDOUT",
    "DTEW_TEMP_STDERR"
  };

  // The default values for DTE Window related environment variables.
  private static String [] envDTEWDefs =
  {
    "",
    "dte",
    "temp.input",
    "temp.output",
    "temp.error"
  };
  
  // The main vector that contains environment settings.
  private PairVector v = new PairVector ();
  private static PairVector otherEnv = new PairVector ();
  
  // System properties.
  // Current directory.
  private static String pwd;

  // The "execute" executables.
  private static String comExec;

  // File separator.
  private static final String sep = System.getProperty ("file.separator");

  // Temporary file.
  private static String tempFile;

  // Shell command to get system environment settings.
  private static final String comSetenv = "env";

  // Load system properties.
  static
    {
      try
	{
	  pwd = System.getProperty ("user.dir");
      
	  if (! (pwd.endsWith (sep)))
	    pwd += sep;
	  
	  System.load (pwd + "Environment.so");
	  
	  envDTEWDefs [0] = pwd;
	  comExec = pwd + "execute";
	  
	  tempFile = pwd + "temp~";
	  loadEnvironments ();
	}
      catch (Throwable e)
	{
	  e.printStackTrace (System.err);
	  System.exit (1);
	}
    }

  // Constructor.
  public Environment ()
    {
      // Add DTE Window related variables.
      for (int i = 0; i < envVarDTEW.length; i ++)
	{
	  String s = getSystemEnv (envVarDTEW [i]);
	  
	  if (s.equals (""))
	    s = envDTEWDefs [i];

	  v.addElement (envVarDTEW [i], s);
	}

      // Add Devise related variables.
      for (int i = 0; i < envVar.length; i ++)
	v.addElement (envVar [i], getSystemEnv (envVar [i]));

      // Load system environments.
      for (int i = 0; i < otherEnv.size (); i ++)
	setenv ((String) (otherEnv.elementAtFirst (i)), (String) (otherEnv.elementAtSecond (i)));
    }
  
  // Get the value of one system environment variable.
  private static native String getSystemEnv (String var);

  // Run an external executable.
  public static final void runCommand (String [] s) throws IOException
    {
      try
	{
	  Runtime.getRuntime ().exec (s).waitFor ();
	}
      catch (InterruptedException e)
	{
	}
    }
  
  // Run an external executable.
  public static final void runCommand (String [] com, String [] env) throws IOException
    {
      try
	{
	  Runtime.getRuntime ().exec (com, env).waitFor ();
	}
      catch (InterruptedException e)
	{
	}
    }
   
  // Load all environments.
  private static void loadEnvironments ()
    {
      try
	{
	  // Run the shell command to retrieve system environment settings.
	  String com [] = new String []
	    {
	      comExec,
	      comSetenv,
	      "",
	      tempFile,
	      ""
	    };
	  
	  runCommand (com);
	  
	  // Read in the result.
	  File f;
	  BufferedReader bf = new BufferedReader (new FileReader (f = new File (tempFile)));
	  String s;
	  
	  // Put the results into the variable vector.
	  while ((s = bf.readLine ()) != null)
	    {
	      int ind = s.indexOf ("=");
	      if (ind < 1)
		continue;
	      
	      otherEnv.addElement (s.substring (0, ind), s.substring (ind + 1));
	    }
	  
	  
	  bf.close ();
	  f.delete ();
	}
      catch (IOException e)
	{
	  e.printStackTrace (System.err);
	  throw new RuntimeException (e.getMessage ());
	}
    }
  
  // Set the value of one environment variable.
  public void setenv (String var, String value)
    {
      for (int i = 0; i < v.size (); i ++)
	if (v.elementAtFirst (i).equals (var))
	  {
	    v.setElementAt (var, value, i);
	    return;
	  }
      
      v.addElement (var, value);
    }
  
  // Get the value of one environment variable.
  public final String getenv (String var)
    {
      String s = (String) (v.find (var));
      return (s == null) ? "" : s;
    }

  // Get the home catalog file name.
  public final String getHomeTable ()
    {
      return getenv ("DEVISE_HOME_TABLE");
    }
  
  // Get the whole environment variable vector.
  public final PairVector getenv ()
    {
      return v;
    }

  // Execute a query, output results to an outputer.
  public void runQuery (String q, ResultOutputer outputer)
    {
      try
	{
	  InputStream [] is = runQuery (q);
	  outputer.writeOutput (is [0]);
	  outputer.writeError (is [1]);
	}
      catch (IOException e)
	{
	  outputer.writeError (e);
	}
    }
  
  // Execute a query.
  public InputStream [] runQuery (String query) throws IOException
    {
      String s = getenv (envVarDTEW [0]);
      int size;
      
      if (! (s.endsWith (sep)))
	s += sep;
	  
      String com [] = new String [5];

      com [0] = comExec;
      for (int i = 1; i < 5; i ++)
	com [i] = s + v.elementAtSecond (i);

      // Construct the correct environment block.
      String env [] = new String [size = v.size () - envVarDTEW.length];

      for (int i = 0; i < size; i ++)
	env [i] = (String) (v.elementAtFirst (i + envVarDTEW.length)) + "=" + 
	  (String) (v.elementAtSecond (i + envVarDTEW.length));

      // Write the query into file, which is inputed to the DTE executable.
      FileWriter fw = new FileWriter (new File (com [2]));
      String q = query.trim ();
      if (! (q.endsWith (";")))
	q += ";";
      
      fw.write (q + "\n");
      fw.close ();

      runCommand (com, env);
      
      FileInputStream fout = new FileInputStream (com [3]);
      FileInputStream ferr = new FileInputStream (com [4]);
      
      // Return the result streams.
      FileInputStream [] ret = new FileInputStream []
	{
	  fout,
	  ferr
	};
	
      return ret;
    }
}


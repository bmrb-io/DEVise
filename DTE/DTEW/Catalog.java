// Catalog.java

// DTE Window Project

// Yunrui Li
// CS Dept. UW-Madison
// Copyright (c) 1999

import java.io.*;
import java.awt.*;
import java.awt.event.*;
import java.lang.*;
import java.util.*;

// A DTE catalog.
public class Catalog
{
  // Catalog parsing exception
  static class ParseException extends Exception
  {
    // Constructor
    public ParseException (String s)
      {
	super (s);
      }

    // Constructor
    public ParseException (Throwable e)
      {
	super (e.getMessage ());
      }
  }

  // A catalog file token reader
  private static class Line
  {
    // Buffer and current position.
    private String s = "";
    private int pos = 0;

    // The input reader.
    private BufferedReader bf;
    
    // Constructor
    public Line (BufferedReader bf)
      {
	this.bf = bf;
      }
    
    // Return the next token in the buffer.
    private String advance ()
      {
	// Skip blanks.
	for (; pos < s.length () && Character.isWhitespace (s.charAt (pos)); pos ++);
	
	// Tell if the end of buffer has been encountered.
	if (pos == s.length ())
	  return null;
	
	String ret = "";
	char c;
	
	// Handle string quotation mark.
	if (s.charAt (pos) == '"')
	  for (++ pos; pos < s.length () && (c = s.charAt (pos ++)) != '"'; ret += c);
	else
	  for (; pos < s.length () && ! Character.isWhitespace (c = s.charAt (pos ++)); ret += c);
	
	// Skip blanks.
	for (; pos < s.length () && Character.isWhitespace (s.charAt (pos)); pos ++);
	
	return ret;
      }

    // Return the next token in the incoming reader.
    public String next () throws IOException
      {
	String ret;
	
	if ((ret = advance ()) != null)
	  return ret;
	
	for (pos = 0; (s = bf.readLine ()) != null && (ret = advance ()) == null;);
	return ret;
      }
  }

  // Catalog entry attribute constants.
  public static final String sName = "          Name : ";
  public static final String sType = "          Type : ";
  public static final String sDatapath = "     Data File : ";
  public static final String sMemberpath = "   Member File : ";
  public static final String sSchema = "        Schema : ";
  public static final String sAddress = "     Host:Port : ";
  public static final String sInfo = "   Information : ";
  public static final String sFields = "        Fields : ";
  public static final String sDefn = "    Definition : ";
  public static final String sViewpath = "     View File : ";
  public static final String sSchemapath = "   Schema File : ";
  public static final String sCatalogpath = "  Catalog File : ";
  public static final String sConnect = "Connect String : ";
  public static final String sTablename = "    Table Name : ";
  
  // Catalog entry type constants.
  public static final String sStandardTable = "Standard Table";
  public static final String sSQLView = "View";
  public static final String sTable = "Devise Table";
  public static final String sMaterView = "Materialized View";
  public static final String sCGIInterface = "CGI Interface";
  public static final String sDirectory = "Catalog File";
  public static final String sODBC = "ODBC Table";
  public static final String sDBServer = "Remote DTE Server";
  public static final String sGestalt = "Gestalt";
  public static final String sUNIXFILE = "UNIX File";

  // The vector that contains all the catalog entries.
  private Vector v = new Vector ();

  // Error messages.
  private static final String msgBadEntry = "Bad catalog entry : ";

  // Load the home catalog.
  public static final Catalog load (Environment env) throws ParseException
    {
      return load (env.getHomeTable ());
    }
  
  // Load a catalog from a stream.
  public static final Catalog load (InputStream stream) throws ParseException
    {
      return load (new BufferedReader (new InputStreamReader (stream)));
    }
  
  // Load a catalog from a file.
  public static final Catalog load (String path) throws ParseException
    {
      try
	{
	  BufferedReader bf = new BufferedReader (new FileReader (path));
	  return load (bf);
	}
      catch (IOException e)
	{
	  throw new ParseException (e);
	}
    }
  
  // Load a catalog from a reader.
  public static Catalog load (BufferedReader bf) throws ParseException
    {
      Catalog c = new Catalog ();
      String ret, n = null;
      
      try
	{
	  // Initialize a token reader.
	  Line l = new Line (bf);
	  
	  // Get the catalog entry name.
	  while ((n = l.next ()) != null)
	    {
	      PairVector pv = new PairVector ();
	      
	      // "Stats" indicates the end of the catalog file.
	      if (n.equalsIgnoreCase ("Stats"))
		break;
	      
	      pv.addElement (sName, n);
	
	      // Get the catalog entry type.
	      ret = l.next ();

	      // Read the rest part of the catalog entry according to the entry type.
	      if (ret.equalsIgnoreCase ("StandardTable"))
		readStandardTable (pv, l);
	      else if (ret.equalsIgnoreCase ("SQLView"))
		readSQLView (pv, l);
	      else if (ret.equalsIgnoreCase ("Gestalt"))
		readGestalt (pv, l);
	      else if (ret.equalsIgnoreCase ("DBServer"))
		readDBServer (pv, l);
	      else if (ret.equalsIgnoreCase ("Table"))
		readTable (pv, l);
	      else if (ret.equalsIgnoreCase ("MaterView"))
		readMaterView (pv, l);
	      else if (ret.equalsIgnoreCase ("CGIInterface"))
		readCGIInterface (pv, l);
	      else if (ret.equalsIgnoreCase ("Directory"))
		readDirectory (pv, l);
	      else if (ret.equalsIgnoreCase ("ODBC"))
		readODBC (pv, l);
	      else if (ret.equalsIgnoreCase ("UNIXFILE"))
		readUNIXFILE (pv, l);
	      else
		throw new ParseException ("");

	      c.v.addElement (pv);
	    }

	  bf.close ();
	}
      catch (IOException e)
	{
	  throw new ParseException (e);
	}
      catch (Throwable e)
	{
	  throw new ParseException (msgBadEntry + n);
	}
      
      return c;
    }

  // Read a standard table entry.
  protected static final void readStandardTable (PairVector pv, Line l) throws ParseException, IOException
    {
      readSQLTable (pv, l, sDatapath, sStandardTable);
    }
  
  // Read a gestalt entry.
  protected static final void readGestalt (PairVector pv, Line l) throws ParseException, IOException
    {
      readSQLTable (pv, l, sMemberpath, sGestalt);
    }

  // Read a standard table or gestalt entry.
  protected static void readSQLTable (PairVector pv, Line l, String filetype, String type) 
    throws ParseException, IOException
    {
      String schema = "";
      String file, s;
      
      // Get the number of fields and schema string.
      int f = Integer.parseInt (l.next ());
      for (int i = 0; i ++ < f; schema += l.next () + ":" + s + "  ")
	s = l.next ();

      // Get the data (or member) file name.
      file = l.next ();

      pv.addElement (sType, type);
      pv.addElement (sSchema, schema);
      pv.addElement (filetype, file);
    
      // Read the semicolon.
      readSemiColon (l);
    }

  // Read an SQL View entry.
  protected static void readSQLView (PairVector pv, Line l) throws ParseException, IOException
    {
      String fields = "";
      String defn;
     
      // Get the number of fields and the field names.
      int f = Integer.parseInt (l.next ());
      for (int i = 0; i ++ < f; fields += l.next () + "  ");

      // Get the SQL View definition.
      defn = l.next ();
      
      pv.addElement (sType, sSQLView);
      pv.addElement (sFields, fields);
      pv.addElement (sDefn, defn);
      
      // Read the semicolon.
      readSemiColon (l);
    }

  // Read a materialized view entry.
  protected static void readMaterView (PairVector pv, Line l) throws ParseException, IOException
    {
      String schema = "";
      String defn, file, s;
      int i;
      
      // Get the number of fields and the schema string.
      int f = Integer.parseInt (l.next ());
      for (i = 0; i ++ < f; schema += l.next () + ":" + s + "  ")
	s = l.next ();
      
      // Skip the field names.
      file = l.next ();
      for (i = 0; i ++ < f; l.next ());
      defn = l.next ();
      
      pv.addElement (sType, sMaterView);
      pv.addElement (sSchema, schema);
      pv.addElement (sDatapath, file);
      pv.addElement (sDefn, defn);
      
      // Read the semicolon.
      readSemiColon (l);
    }

  // Read a DBServer entry.
  protected static void readDBServer (PairVector pv, Line l) throws ParseException, IOException
    {
      pv.addElement (sType, sDBServer);

      // Read the host address and port number.
      pv.addElement (sAddress, l.next () + ":" + l.next ());
      
      // Read the semicolon.
      readSemiColon (l);
    }

  // Read a Devise table entry.
  protected static void readTable (PairVector pv, Line l) throws ParseException, IOException
    {
      pv.addElement (sType, sTable);

      // Read the schema path, data path and view path.
      pv.addElement (sSchemapath, l.next ());
      pv.addElement (sDatapath, l.next ());
      pv.addElement (sViewpath, l.next ());
      
      // Read the semicolon.
      readSemiColon (l);
    }

  // Read a CGI Interface entry.
  protected static final void readCGIInterface (PairVector pv, Line l) throws ParseException, IOException
    {
      pv.addElement (sType, sCGIInterface);
      readOthers (pv, l);
    }

  // Read a catalog entry.
  protected static void readDirectory (PairVector pv, Line l) throws ParseException, IOException
    {
      pv.addElement (sType, sDirectory);
      pv.addElement (sCatalogpath, l.next ());
      
      readSemiColon (l);
    }

  // Read an ODBC table entry.
  protected static void readODBC (PairVector pv, Line l) throws ParseException, IOException
    {
      pv.addElement (sType, sODBC);
      pv.addElement (sConnect, l.next ());
      pv.addElement (sTablename, l.next ());
      
      readSemiColon (l);
    }

  // Read a UNIX file entry.
  protected static final void readUNIXFILE (PairVector pv, Line l) throws ParseException, IOException
    {
      pv.addElement (sType, sUNIXFILE);
      readOthers (pv, l);
    }

  // Read other entries.
  protected static final void readOthers (PairVector pv, Line l) throws ParseException, IOException
    {
      String info = "", s;
      
      // Read till a semicolon is encountered.
      for (; ! ((s = l.next ()).equals (";")); info += s + "  ");
    }

  // Skip a semicolon.
  protected static final void readSemiColon (Line l) throws ParseException, IOException
    {
      if (! (l.next ().equals (";")))
	throw new ParseException ("");
    }
  
  // Return the vector that contains the catalog entries.
  public final Vector getVector ()
    {
      return v;
    }
}

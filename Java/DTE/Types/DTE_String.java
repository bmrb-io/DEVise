package Types;

import java.io.*;

public class DTE_String extends DTE_Type implements Cloneable 
{
  String val;

  public DTE_String()
  {
    val = null;
  }
  
  public DTE_String(String i)
  {
    val = new String(i);
  }
	
  public String get_type( )
  {
    return "string";
  }

  public Object get_val( )
  {
    return val;
  }

  public boolean read(StreamTokenizer st) throws IOException 
  { 
    int status = st.nextToken();
    if(status == StreamTokenizer.TT_EOF)
      return false;
    
    if(status != StreamTokenizer.TT_WORD)
      {
	int lineno = st.lineno();
	System.err.println("The data on line " + lineno + " is not a String!");
	return false;
      }
    
    val = st.sval;
    return true;
  }
  
  public void print(PrintWriter ps)
  {
    ps.print(val);
  }

  public void print(PrintStream ps)
  {
    ps.print(val);
    ps.print(" ");   
  }


  public Object clone()
  {
    return new DTE_String(val);
  }
}


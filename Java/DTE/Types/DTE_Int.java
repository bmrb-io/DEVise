package Types;

import java.io.*;
import java.lang.*;

public class DTE_Int extends DTE_Type// implements Cloneable 
{
  int val;

  public String toString()
  {
     return  String.valueOf( val );
  }
  
  public DTE_Int( )
  {
    val = 0;
  }
  
  public DTE_Int( int i )
  {
    val = i;
  }
  
  public TypeDesc getType( )
  {
    return new IntDesc( );
  }

  public int getValue( )
  {
    return val;
  }

  public void setValue( int i )
  {
    val = i;
  }

  public boolean read(StreamTokenizer st) throws IOException 
  {
    int status = st.nextToken( );
    if(status == StreamTokenizer.TT_EOF)
      return false;
    
    if(status != StreamTokenizer.TT_NUMBER)
      {
	int lineno = st.lineno( );
	System.err.println("The data on line " + lineno + " is not an integer!");
	return false;
      }
    
    // Need to check that this is really an int.
    // return 0 if this is not an int and print the msg to stderr.
    // use st.lineno()
    
    val = (int) st.nval;
    
    if((double)val != st.nval)
      {
	int lineno = st.lineno();
	System.err.println("The data on line " + lineno + " is not an integer!");
	return false;
      }
    
    return true;
  }
  
  public void print(PrintWriter ps)
  {
    ps.print(val);
    ps.print(" ");
  }

  public void print(PrintStream ps)
  {
    ps.print(val);   
    ps.print(" "); 
  }
    /*
  public Object clone()
  {
    return new DTE_Int(val);
  }
    */

    public void copyTo(DTE_Type d)
	{
	   ((DTE_Int)d).val = val;
	}
}


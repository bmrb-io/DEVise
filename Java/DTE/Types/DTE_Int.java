package Types;

import java.io.*;
import java.lang.*;

/** The DTE_Type for integers. */
public class DTE_Int extends DTE_Type 
{
  int val;

  /** Return its String representation. */
  public String toString()
  {
     return  String.valueOf( val );
  }
  
     /** Check if two DTE_Int are equal. */
     public boolean equals(Object obj){
          if(! getClass().equals(obj.getClass())){
               return false;
          }
          DTE_Int cObj = (DTE_Int) obj;
          return val == cObj.val;
     }

  /** Create an DTE_Int object with value to be 0.*/  
  public DTE_Int( )
  {
    val = 0;
  }
  
  /** Create an DTE_Int object with value to be i.*/  
  public DTE_Int( int i )
  {
    val = i;
  }
  
  /** Return an IntDesc object. */
  public TypeDesc getType( )
  {
    return new IntDesc( );
  }

  /** Return its value. */
  public int getValue( )
  {
    return val;
  }

  /** Set its value to be a given number. */
  public void setValue( int i )
  {
    val = i;
  }

  /** Read a DTE_Int from a StreamTokenizer object "st".*/
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
  
/** Print to a PrintWriter object "ps".*/
  public void print(PrintWriter ps)
  {
    ps.print(val);
    ps.print(" ");
  }

/** Print to a PrintStream object "ps".*/
  public void print(PrintStream ps)
  {
    ps.print(val);   
    ps.print(" "); 
  }

  /** Copy this DTE_Int object to another DTE_Type object 'd'.*/    
    public void copyTo(DTE_Type d)
	{
	   ((DTE_Int)d).val = val;
	}
}


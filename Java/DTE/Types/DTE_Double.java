package Types;

import java.io.*;
import java.lang.*;

public class DTE_Double extends DTE_Type  
{
  double val;

  public String toString()
  {
     return String.valueOf( val );
  }

     public boolean equals(Object obj){
          if(! getClass().equals(obj.getClass())){
               return false;
          }
          DTE_Double cObj = (DTE_Double) obj;
          return val == cObj.val;
     }

  /** Create an DTE_Double object with value to be 0.*/  
  public DTE_Double()
  {
    val = 0;
  }

  /** Create an DTE_Double object with value to be i.*/  
  public DTE_Double(double i)
  {
    val = i;
  }
  
  public TypeDesc getType( )
  {
    return new DoubleDesc( );
  }

  public double getValue( )
  {
    return val;
  }

  public void setValue( double d)
  {
    val = d;
  }

  /** Read a DTE_Double from a StreamTokenizer object "st".*/
  public boolean read(StreamTokenizer st) throws IOException 
  {
    int status = st.nextToken();
    if(status == StreamTokenizer.TT_EOF)
      return false;
    
    if(status != StreamTokenizer.TT_NUMBER)
      {
	int lineno = st.lineno();
	System.err.println("The data on line " + lineno + " is not a double!");
	return false;
      }
    
    val = st.nval;
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

  /** Copy this DTE_Double object to another DTE_Type object 'd'.*/ 
    public void copyTo(DTE_Type d)
	{
	    ((DTE_Double)d).val = val;
	}
}



package Types;

import java.io.*;

public class DTE_Boolean extends DTE_Type //implements Cloneable 
{
  boolean val;
  
  public DTE_Boolean( )
  {
    val = true;
  }
  
  public DTE_Boolean( boolean b )
  {
    val = b;
  }
  
  public String toString( ){
      return String.valueOf( val );
  }

  public TypeDesc getType( )
  {
    return new BooleanDesc( );
  }

  public boolean getValue( )
  {
    return val;
  }

  public void setValue( boolean b )
  {
    val = b;
  }

  public boolean read(StreamTokenizer st) throws IOException 
  {
    throw new IOException( "it can't be read ...\n" );
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
    /*
  public Object clone()
  {
    return new DTE_Boolean(val);
  }
    */

    public void copyTo(DTE_Type d)
	{
	    ((DTE_Boolean)d).val = val;
	}
}


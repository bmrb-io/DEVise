package Types;

import java.io.*;

public class DTE_String extends DTE_Type implements Cloneable 
{
    public static final int LENGTH = 40; 
    char[] val = new char[LENGTH];
    int length = 0;

    public DTE_String() { }
    
    /*    public DTE_String(String i)
	{
	    val = new String(i);
	}
    
    public DTE_String(int i) 
	{
	    if(i < LENGTH)
		val = new char[i];
	    else
		{
		    System.out.println("String lengh can't be more than "+LENGTH);
		    System.exit(-1);
		}
	}
    */

    public TypeDesc getType( )
	{
	    return new StringDesc( );
	}

    public int  getValue( char[] a)
	{
	    if(a.length < length)
		{
		    System.out.println("Array length is shorter than what wanted.");
		    System.exit(-1);
		}

	    for(int i =0; i< length; i++)
		a[i] = val[i];
	
	    return length;
	}
    
    public void setValue( char[] str )
	{
	    if(str.length > LENGTH)
		{
		    System.out.println("Array length is longer than " + LENGTH);
		    System.exit(-1);
		}

	    for(int i =0; i< str.length; i++)
		val[i] = str[i];

	    length = str.length;
	}
   

    /** This function truncate the input string if it is more than LENGTH. */
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
	    
	    length = st.sval.length();
	    if( length > LENGTH)
		length = LENGTH;
	    
	    st.sval.getChars(0, length, val, 0);
	    
	    return true;
	}
  

  public void print(PrintWriter ps)
  {
      for(int i =0; i < length; i++)
	  ps.print(val[i]);
      ps.print(" ");
  }

  public void print(PrintStream ps)
	{
	    for(int i =0; i < length; i++)
		ps.print(val[i]);
	    ps.print(" ");   
	}
    
    public void copyTo(DTE_Type d)
	{
	    for(int i =0; i < length; i++)
		((DTE_String)d).val[i] = val[i];
	    ((DTE_String)d).length = length;
	}

    public boolean equals( DTE_String str ) {
	if ( length != str.length )
	    return false;

        for ( int i=0; i < length; i++ ) 
	    if ( val[i] != str.val[i] )
		return false;

        return true;
    }

    public int compare( DTE_String str ) {
	int i = 0;
	int j = 0;

	while ( i < length && j < str.length ) {
	    if ( val[i] < str.val[i] )
		return -1;
	    if ( val[i] > str.val[i] )
		return 1;
        }

	if ( length < str.length )
	    return -1;
	if ( length > str.length )
	    return 1;
        return 0;
    }

}
		    
	    



	    /*
	      public Object clone()
	      {
	      return new DTE_String(val);
	      }
	    */



package Types;

import java.io.*;

/** We decided to use character array to store String object in this class. The reason is that we can copy character stream to the buffer of an existing character array without creating a new object. Eg: if we use String object to store character stream, then every time when we copy character stream to this DTE_String object, we have to new a new String object, which is quite expensive, and it also makes the Blocked Nested Loop very hard to implement, if not at all.(We can't new a new String everytime when copy an "string" attribute to a tuple in the memory bucket because the size of the memory bucket suppose to be fixed.)*/  

public class DTE_String extends DTE_Type
{
  public static final int LENGTH = 40; 
  char[] val = new char[LENGTH];
  int length = 0;

  /** This constructor does nothing except creating an empty DTE_String object.*/  
  public DTE_String() { }
    
  /** Create a DTE_String object and assign String "i" to it. If the length of "i" is longer that LENGTH, truncate it.*/
  public DTE_String(String i)
  {
    length = i.length();
    if( length > LENGTH)
      length = LENGTH;
    
    i.getChars(0, length, val, 0);
  }

  /** Return a StringDesc object. */
  public TypeDesc getType( )
  {
    return new StringDesc( );
  }
  
  /** Return the String stored, truncate the String if the length of 'a' is less than the String's length.*/
  public int  getValue( char[] a)
  {
    int l = length;
    if(a.length < length)
      {
	l = a.length;
	
      }
    
    for(int i = 0; i< l; i++)
      a[i] = val[i];
    
    return l;
  }
  

  /** Set the DTE_String object to the char[] 'a' and truncate 'a' if its length is longer than LENGTH.*/
  public void setValue( char[] str )
  {
    int l = str.length;
    if(str.length > LENGTH)
      {
	l = LENGTH;
      }
    
    for(int i =0; i< l; i++)
      val[i] = str[i];
    
    length = l;
  }
  
  
    /** Read a DTE_String from a StreamTokenizer object "st". This function truncate the input string if it is more than LENGTH. */
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

  /** Print to a PrintWriter object "ps".*/
  public void print(PrintWriter ps)
  {
      for(int i =0; i < length; i++)
	  ps.print(val[i]);
      ps.print(" ");
  }

  public String toString( ) {
      String str = new String( val );
      return str;
  }

 /** Print to a PrintStream object "ps".*/
  public void print(PrintStream ps)
	{
	    for(int i =0; i < length; i++)
		ps.print(val[i]);
	    ps.print(" ");   
	}

  /** Copy this DTE_String object to another DTE_Type object 'd'.*/    
    public void copyTo(DTE_Type d)
	{
	    for(int i =0; i < length; i++)
		((DTE_String)d).val[i] = val[i];
	    ((DTE_String)d).length = length;
	}

    /** Check if two DTE_String objects are equal.*/
    public boolean equals( DTE_String str ) {
	if ( length != str.length )
	    return false;

        for ( int i=0; i < length; i++ ) 
	    if ( val[i] != str.val[i] )
		return false;

        return true;
    }

    /** Compare two DTE_String objects; 0 for equal, 1 for >, 
	and -1 for <. */
    public int compares( DTE_String str ) {
	for ( int i=0; i < length && i < str.length; i++ ) {
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

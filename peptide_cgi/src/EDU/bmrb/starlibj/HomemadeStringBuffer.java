package EDU.bmrb.starlibj;

import java.lang.*;
import java.util.*;

/** I made this as a stopgap measure to behave much like the
  * standard type: <TT>java.lang.StringBuffer.</TT>  I needed
  * a drop-in replacement for StringBuffer.because the one
  * that came with the Blackdown JDK for Linux is broken.  It
  * eats ten times as much memory as needed when you append to
  * it slowly in a loop while doing other things (it is hard
  * to recreate).
  */
public class HomemadeStringBuffer
{
    private char  myBuf[];  // the buffer inside me that stores the string.
    private int   usefulLen; // what subset of myBuf holds actual data.

    public HomemadeStringBuffer( )
    {
	myBuf = new char[0];
	usefulLen = myBuf.length;
    }

    public HomemadeStringBuffer( String copyMe )
    {
	myBuf = new char[ copyMe.length() ];
	copyMe.getChars( 0, copyMe.length(), myBuf, 0 );
	usefulLen = myBuf.length;
    }

    /** Get the character array buffer that holds this string */
    public char [] getBuf( )
    {
	return myBuf;
    }

    /** Get the useful length of the array buffer */
    public int length()
    {
	return usefulLen;
    }
    
    /** Get the total size the array buffer is using (This is
      * how big the array can grow before the next time it will
      * need to automatically relocate itself into a bigger array).
      */
    public int capacity()
    {
	return myBuf.length;
    }

    /** Functions identically to the StringBuffer function of the
      * same name.
      */
    public char charAt( int idx )
    {
	return myBuf[ idx ];
    }

    /** Functions identically to the StringBuffer function of the
      * same name.
      */
    public void getChars( int  srcBegin, 
	                  int  srcEnd,
			  char dst[],
			  int  dstBegin )
    {
	System.arraycopy( myBuf, srcBegin, dst, dstBegin, srcEnd-srcBegin );
    }

    /** Append a string to this string buffer. */
    public void append( String appendMe )
    {
	int newLength = usefulLen + appendMe.length();

	// See if we need to grow the array larger first:
	if( newLength > myBuf.length )
	{
	    // The new array will be 1/3 larger (mulitply by 4/3), plus
	    // 1 for slop in case the array is so small that 1/3 of its
	    // size rounds down to zero.
	    char tmp[] = new char[ (newLength * 4 ) / 3  + 1 ];
	    System.arraycopy( myBuf, 0, tmp, 0, myBuf.length );
	    myBuf = tmp;
	    tmp = null;
	}
	appendMe.getChars( 0, appendMe.length(), myBuf, usefulLen );
	usefulLen = newLength;

    }

    /** Append a char array to this string buffer. */
    public void append( char appendMe[] )
    {
	int newLength = usefulLen + appendMe.length;

	// See if we need to grow the array larger first:
	if( newLength > myBuf.length )
	{
	    // The new array will be 1/3 larger (mulitply by 4/3), plus
	    // 1 for slop in case the array is so small that 1/3 of its
	    // size rounds down to zero.
	    char tmp[] = new char[ (newLength * 4 ) / 3  + 1 ];
	    System.arraycopy( myBuf, 0, tmp, 0, myBuf.length );
	    myBuf = tmp;
	    tmp = null;
	}
	System.arraycopy( appendMe, 0, myBuf, usefulLen, appendMe.length );
	usefulLen = newLength;
    }
    /** Append a char to this string buffer. */
    public void append( char appendMe )
    {
	int newLength = usefulLen + 1;

	// See if we need to grow the array larger first:
	if( newLength > myBuf.length )
	{
	    // The new array will be 1/3 larger (mulitply by 4/3), plus
	    // 1 for slop in case the array is so small that 1/3 of its
	    // size rounds down to zero.
	    char tmp[] = new char[ (newLength * 4 ) / 3  + 1 ];
	    System.arraycopy( myBuf, 0, tmp, 0, myBuf.length );
	    myBuf = tmp;
	    tmp = null;
	}
	myBuf[newLength-1] = appendMe;
	usefulLen = newLength;
    }


}

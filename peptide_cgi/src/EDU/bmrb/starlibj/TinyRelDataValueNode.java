package EDU.bmrb.starlibj;

import java.lang.*;
import java.util.*;

/** Used internally by LoopRowNodes to store values
  * in an efficient manner. 
  */
public class TinyRelDataValueNode
{
    public short offset; // line/column offset from the loopRow's
                         // starting point.  The most signifigant 6 bits is
                         // the line offset.  The least signifigant 10 bits
    			 // is the column offset.
    public char str[]; // the string value with delimiters included.

    public TinyRelDataValueNode( short off, String s )
    {
	offset = off;
	str = new char[s.length()];
	s.getChars( 0, s.length(), str, 0 );
    }

    // Make a new node given the TinyAbsDataValueNode version, and
    // and the parent loop row node line number that the
    // relative measures should be taken from.
    public TinyRelDataValueNode( int parLine, TinyAbsDataValueNode n )
    {
	// rightmost six bits = line offset, leftmost 10 bits = column offset:
	// (column offset is actually absolute - only the line number is
	// a relative offset)
	offset = (short) ( ( ( (n.lNum - parLine) & 0x002F ) << 10 ) +
	                 ( (n.cNum) & 0x02FF ) );
	str = new char[n.str.length];
	System.arraycopy( n.str, 0, str, 0, n.str.length );
    }

    // Make a new node given the full-fledged DataValueNode version,
    // and the parent loop row node that the relative measures should
    // be taken from.
    public TinyRelDataValueNode( int parLine, DataValueNode dvn )
    {
	// rightmost six bits = line offset, leftmost 10 bits = column offset:
	// (column offset is actually absolute - only the line number is
	// a relative offset)
	offset = (short) ( ( ( (dvn.getLineNum()-parLine) & 0x002F ) << 10 ) +
	                 (     (dvn.getColNum())          & 0x02FF ) );
	short delim = dvn.getDelimType();
	String dvnVal = dvn.getValue();
	if ( delim == DataValueNode.SINGLE )
	{
	    str = new char[ dvnVal.length() + 2 ];
	    str[0] = '\'';
	    dvnVal.getChars( 0, dvnVal.length(), str, 1 );
	    str[ dvnVal.length() + 1 ] = '\'';
	}
	else if ( delim == DataValueNode.DOUBLE )
	{
	    str = new char[ dvnVal.length() + 2 ];
	    str[0] = '"';
	    dvnVal.getChars( 0, dvnVal.length(), str, 1 );
	    str[ dvnVal.length() + 1 ] = '"';
	}
	else if ( delim == DataValueNode.SEMICOLON )
	{
	    str = new char[ dvnVal.length() + 4 ];
	    str[0] = ';' ;
	    str[1] = '\n' ;
	    dvnVal.getChars( 0, dvnVal.length(), str, 2 );
	    str[ dvnVal.length() + 2 ] = '\n' ;
	    str[ dvnVal.length() + 3 ] = ';' ;
	}
	else if ( delim == DataValueNode.FRAMECODE )
	{
	    str = new char[ dvnVal.length() + 1 ];
	    str[0] = '$' ;
	    dvnVal.getChars( 0, dvnVal.length(), str, 1 );
	}
	else
	{
	    str = new char[ dvnVal.length() ];
	    dvnVal.getChars( 0, dvnVal.length(), str, 0 );
	}
    }

    // Convert this value into a DataValueNode and return it:
    public DataValueNode makeIntoDVN( LoopRowNode par)
    {
	ParseValFromRetVal parseRet;
	DataValueNode retVal;
	parseRet = StarValidity.parseValFrom( new String( str ), true );

	retVal = new DataValueNode( parseRet.str, parseRet.delim );
	retVal.setLineNum( par.getLineNum() + ( offset>>>10 ) );
	retVal.setColNum( offset & 0x02FF );
	retVal.setParent( par );

	return retVal;
    }

    // for my debugging:
    public void printSize()
    {
	System.out.println( "TinyRel size = " + str.length + " + 2 for the short= " + str.length + 2 );
    }
}

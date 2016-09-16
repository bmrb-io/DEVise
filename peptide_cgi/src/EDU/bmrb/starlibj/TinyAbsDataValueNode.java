package EDU.bmrb.starlibj;

import java.lang.*;
import java.util.*;

/** Used internally by LoopRowNodes to store values
  * in an efficient manner. 
  */
public class TinyAbsDataValueNode
{
    public int   lNum; // line number - absolute
    public short cNum; // column numer - absolute
    public char  str[]; // the string value with delimiters included.

    // Make a new node given all the parameters directly:
    public TinyAbsDataValueNode( int l, short c, String s )
    {
	lNum = l ;
	cNum = c ;
	int len = s.length();
	str = new char[ len ] ;
	s.getChars( 0, len, str, 0) ;
    }

    // Make a new node given the full-fledged DataValueNode version,
    // and the line/col numbers
    public TinyAbsDataValueNode( int l, short c, DataValueNode dvn )
    {
	lNum = l;
	cNum = c;
	short delim = dvn.getDelimType();
	String dvnVal = dvn.getValue();
	if ( delim == DataValueNode.SINGLE )
	{
	    str = new char[ dvn.getValue().length() + 2 ];
	    str[0] = '\'';
	    dvnVal.getChars( 0, dvnVal.length(), str, 1 );
	    str[ dvnVal.length() + 1 ] = '\'';
	}
	else if ( delim == DataValueNode.DOUBLE )
	{
	    str = new char[ dvn.getValue().length() + 2 ];
	    str[0] = '"';
	    dvnVal.getChars( 0, dvnVal.length(), str, 1 );
	    str[ dvnVal.length() + 1 ] = '"';
	}
	else if ( delim == DataValueNode.SEMICOLON )
	{
	    str = new char[ dvn.getValue().length() + 4 ];
	    str[0] = ';' ;
	    str[1] = '\n' ;
	    dvnVal.getChars( 0, dvnVal.length(), str, 2 );
	    str[ dvnVal.length() + 2 ] = '\n' ;
	    str[ dvnVal.length() + 3 ] = ';' ;
	}
	else if ( delim == DataValueNode.FRAMECODE )
	{
	    str = new char[ dvn.getValue().length() + 1 ];
	    str[0] = '$' ;
	    dvnVal.getChars( 0, dvnVal.length(), str, 1 );
	}
	else
	{
	    str = new char[ dvn.getValue().length() ];
	    dvnVal.getChars( 0, dvnVal.length(), str, 0 );
	}
    }

    // Convert this value into a DataValueNode and return it:
    public DataValueNode makeIntoDVN( LoopRowNode par)
    {
	ParseValFromRetVal parseRet;
	DataValueNode retVal;
	parseRet = StarValidity.parseValFrom( new String( str ), true  );

	retVal = new DataValueNode( parseRet.str, parseRet.delim );
	retVal.setLineNum( lNum );
	retVal.setColNum( cNum );
	retVal.setParent( par );

	return retVal;
    }

}

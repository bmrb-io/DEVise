package EDU.bmrb.starlibj;

import java.lang.*;
import java.util.*;

/** DataValueNode is the class that stores a single value from the
  * STAR tree.  It is used for both loop values and the values associated
  * with tags outside of loops.  Making this a class instead of just a
  * string allows for the handling of the delimiter type.
  */
public class DataValueNode extends StarNode implements Cloneable
{
    /** delimiter type that indicates that you don't care about
      * the delimiter type - only used for searches such as
      * <TT>searchForType()</TT>
      * @see getDelimType
      @ @see StarNode.searchForType()
      */
    public static short  DONT_CARE = -1;

    /** delimiter type that indicates that this is a nonquoted value.
      * @see getDelimType
      */
    public static short  NON = 0;
    /** delimiter type that indicates that this is a doublequoted value (")
      * @see getDelimType
      */
    public static short  DOUBLE = 1;
    /** delimiter type that indicates that this is a singlequoted value (')
      * @see getDelimType
      */
    public static short  SINGLE = 2;
    /** delimiter type that indicates that this is semicolon demilited (;) 
      * @see getDelimType
      */
    public static short  SEMICOLON = 3;
    /** delimiter type that indicates that this is a framecode value 
      * @see getDelimType
      */
    public static short  FRAMECODE = 4;

    /** Returns the delimiter type of this value.
      * Note that the delimiter type of this value determines what kind
      * of delimiter is used by this value.  The delimiter itself is not
      * inlcuded as part of the value string.  So a value that appears
      * as "sample value" in the STAR file will contain <TT>sample value</TT>
      * in the string, and will be of delimiter type <TT>DOUBLE</TT>.  This
      * is true for all the value types, including framecodes (the dollarsign
      * is not part of the string value itself).
      * @return NON, DOUBLE, SINGLE, SEMICOLON, or FRAMECODE
      */
    public short getDelimType()
    {
	return delimType;
    }

    /** Sets the delimiter type of this value.
      * Lets the programmer choose a delimiter type for this value.
      *
      * @exception BadValueForDelimiter Throws this exception if
      * the delimiter type is incorrect for the given type of string
      * (for example, if a string with whitespace is given a delimiter
      * type of "NON", that's an error.)
      */
    public void setDelimType( short setTo )
	throws BadValueForDelimiter
    {
	if( StarValidity.isValidForDelim( myStrVal, setTo ) )
	    delimType = setTo;
	else
	    throw new BadValueForDelimiter(myStrVal, setTo);
    }

    /** Returns the string containing the value of this node.  All values
      * are stored as strings, even numbers.  If you desire to look at the
      * value as a number, there are many Java methods that will let you
      * convert strings to numbers on-the-fly.
      */
    public String getValue()
    {
	return myStrVal;
    }

    /** Sets the string value for this node.
      * @exception BadValueForDelimiter The string value is not
      * acceptable for the delimiter the value has.  If you are
      * intending to change the delimiter too in the next statement,
      * try changing the delimiter *first*, or use the method to
      * change both at once <TT>setValAndDelim</TT>
      * @see setValAndDelim()
      */
    public void setValue( String newVal )
	throws BadValueForDelimiter
    {
	if( StarValidity.isValidForDelim( newVal, delimType ) )
	    myStrVal = newVal;
	else
	    throw new BadValueForDelimiter(newVal,delimType);
    }

    /** Sets the string for this value, and the delimiter.
      * @exception BadValueForDelimiter The string value is not
      * acceptable for the delimiter given..
      */
    public void setValue( String newVal, short newDelim )
	throws BadValueForDelimiter
    {
	if( StarValidity.isValidForDelim( newVal, newDelim ) )
	{   myStrVal  = newVal;
	    delimType = newDelim;
	}
	else
	    throw new BadValueForDelimiter(newVal,newDelim);
    }

    /** Constructor - all DataValueNodes must have a string value,
      * so no provisions are made for a 'default' no-args constructor.
      * The delimiter type will be chosen as the first delimiter type
      * that is valid for the string value.  It will try all the
      * types of delimiter until a valid type is found, using the
      * following order to try them in:
      * <TT><OL>
      *     <LI>NON</LI>
      *     <LI>DOUBLE</LI>
      *     <LI>SINGLE</LI>
      *     <LI>SEMICOLON</LI>
      * </OL></TT>
      * @exception BadValueForDelimiter if the value for the
      * node won't be valid syntax given the kind of delimiter
      * the node has.  In theory this should be very rare
      * given that most strings should fit into at least one of
      * the delimiter types.  The only problem is when a multiline
      * string cannot be a semicolon string because it has 
      * embedded semicolons that appear at the start of a line.
      */
    public DataValueNode( String str )
	throws BadValueForDelimiter
    {
	super();
	short i;
	myStrVal = str;

	// Find the appropriate delimiter type:
	// (Never pick FRAMECODE unless explicity told to do so.)
	for( i = NON ; i <= SEMICOLON ; i++ )
	    if( StarValidity.isValidForDelim( str, i ) )
	    {   delimType = i;
		break;
	    }
	if( i > SEMICOLON )
	    throw new BadValueForDelimiter(str,i);
    }

    /** Constructor - all DataValueNodes must have a string value,
      * so no provisions are made for a 'default' no-args constructor.
      * @param str The string to set the value to.
      * @param delim The delimiter type to set the value to.
      */
    public DataValueNode( String str, short delim )
	throws BadValueForDelimiter
    {
	super();
	if( StarValidity.isValidForDelim( str, delim ) )
	{   myStrVal  = str;
	    delimType = delim;
	}
	else
	    throw new BadValueForDelimiter(str,delim);
    }

    /** Constructor - copy another DataValueNode. */
    public DataValueNode( DataValueNode copyMe )
    {
	super( copyMe );
	myStrVal  = (copyMe.getValue() == null ) ?
	                null : new String( copyMe.getValue() );
	delimType = copyMe.getDelimType();
    }

    /** Allocates a new copy of me and returns a reference to it.
      * This is a deep copy, meaning that all children are copied
      * instead of linked.
      */
    public Object clone()
    {
	return new DataValueNode( this );
    }

    /** Alias for getValue */
    public String getLabel()
    {
	return myStrVal;
    }

    /** Unparse prints the contents of the StarNode object out to the
      * given stream.  This is essentially the inverse of the CS term
      * to "parse", hence the name "Unparse".  The parameter given is
      * the indentation level to print things.
      */
    public void Unparse( int indent )
    {
	// Fill this messy thing in later.
    }

    /** Useful for printing.
      */
    public int myLongestStr()
    {
	int retVal = myStrVal.length();

	if( delimType == NON )
	    retVal += 0;  // Leave it as it is.
	else if( delimType == SINGLE )
	    retVal += 2;
	else if( delimType == DOUBLE )
	    retVal += 2;
	else if( delimType == FRAMECODE )
	    retVal += 1;
	else if( delimType == SEMICOLON )
	    retVal += 4; // fairly meaningless, really
	return retVal;
    }

    public VectorCheckType searchForType( Class type, short delim )
    {
	// Default behaviour if not overridden is to check my
	// own type and add myself to the list if I am the
	// type being looked for.
	VectorCheckType retVal;
	int  i;

	i = delim; // Useless statement to get the compiler to shut up.

	retVal = new VectorCheckType();
	try
	{ retVal.addType( Class.forName( StarValidity.clsNameStarNode) );
	    retVal.freezeTypes();
	    if(    type.isInstance(this) &&
		   ( delim == getDelimType() || delim == DONT_CARE )   ) 
		retVal.addElement( this );
	}
	catch( ClassNotFoundException exc )
	{   System.err.println( "Should never happen exception: " +
		                exc.getMessage() );
	    exc.printStackTrace();
	}
	return retVal; // Intended to be overridden
    }

    protected String myStrVal;
    protected short  delimType;
}


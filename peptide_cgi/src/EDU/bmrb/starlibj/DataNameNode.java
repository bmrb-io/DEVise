package EDU.bmrb.starlibj;

import java.lang.*;
import java.util.*;

/** This is a simple class that holds a single tag name (either
  * a free tag/value pair or a name in a loop).  This is a very
  * simple class that exists mainly for orthogonality.
  */
public class DataNameNode extends StarNode implements Cloneable
{

    /** Returns the string contained in this name.  This is identical to
      * <TT>getLabel()</TT>.
      */
    public String getValue()
    {
	return myStrVal;
    }
    /** Returns the string contained in this name.  This is identical to
      * <TT>getValue()</TT>.
      */
    public String getLabel()
    {
	return myStrVal;
    }

    /** Sets the string name for this node.  This is identical
      * to <TT>setLabel()</TT>.
      */
    public void setValue( String newVal )
	throws NameViolatesStarSyntax
    {
	if( StarValidity.isValidTagName( newVal ) )
	    myStrVal = newVal;
	else
	    throw new NameViolatesStarSyntax(newVal,"tag name");
    }

    /** Sets the string name for this node.  This is identical
      * to <TT>setValue()</TT>.
      */
    public void setLabel( String newVal )
	throws NameViolatesStarSyntax
    {
	if( StarValidity.isValidTagName( newVal ) )
	    myStrVal = newVal;
	else
	    throw new NameViolatesStarSyntax(newVal, "tag name");
    }

    /** Constructor - all DataNameNodes must have a string value,
      * so no provisions are made for a 'default' no-args constructor.
      * @exception NameViolatesStarSyntax thrown when the string given
      * is not a valid STAR tag name.
      */
    public DataNameNode( String str )
	throws NameViolatesStarSyntax
    {
	super();
	myStrVal = str;

	if( ! StarValidity.isValidTagName( str ) )
	    throw new NameViolatesStarSyntax(str, "tag name");
    }

    /** Constructor - copy another DataValueNode. */
    public DataNameNode( DataNameNode copyMe )
    {
	super( copyMe );
	myStrVal  = (copyMe.myStrVal == null ) ?
	                null : new String( copyMe.myStrVal );
    }

    /** Allocates a new copy of me and returns a reference to it.
      * This is a deep copy, meaning that all children are copied
      * instead of linked.
      */
    public Object clone()
    {
	return new DataNameNode( this );
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

    protected String myStrVal;
}


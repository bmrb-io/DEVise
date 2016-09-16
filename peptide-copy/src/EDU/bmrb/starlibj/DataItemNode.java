package EDU.bmrb.starlibj;

import java.lang.*;
import java.util.*;

/** Holds a single "free" tag and value pair.  Can be found inside
  * SaveFrameNodes and BlockNodes
  */
public class DataItemNode extends StarNode
{

    /** This constructor makes a new item with the given name
      * and value.  It makes a <b>copy</b> of the value passed,
      * not a reference to it.
      * @exception NameViolatesStarSyntax if the tag name is
      * not valid star syntax for a tag name.
      */
    public DataItemNode( DataNameNode name, DataValueNode value )
	throws NameViolatesStarSyntax
    {
	super();
	myDataName = new DataNameNode(name);
	myDataName.setParent(this);
	myDataValue = new DataValueNode(value);
	myDataValue.setParent(this);
    }

    /** (RECENT CHANGE) This constructor makes a new item with the given
      * name and string value, giving it the default delimiter type
      * that works for the value given.  Copies of the strings
      * are made, not references.
      * @exception NameViolatesStarSyntax if the tag name is
      * not valid star syntax for a tag name.
      * @exception BadValueForDelimiter if the value for the
      * node won't be valid syntax given the kind of delimiter
      * the node has.  In theory this should be very rare
      * given that most strings should fit into at least one of
      * the delimiter types.  The only problem is when a multiline
      * string cannot be a semicolon string because it has 
      * embedded semicolons that appear at the start of a line.
      */
    public DataItemNode( String name, String value )
	throws NameViolatesStarSyntax, BadValueForDelimiter
    {
	super();
	myDataName = new DataNameNode(name);
	myDataName.setParent(this);
	myDataValue = new DataValueNode( value );
	myDataValue.setParent(this);
    }

    /** Make a new item given the name and value and delimiter
      * type.  Note that no checking is done to ensure that the
      * delimiter type given is appropriate for the value given.
      * (It is possible to make a value with spaces and tell
      * it to be nonquoted, for example.)
      * <p>
      * Copies of the strings are made, not references.
      * @exception NameViolatesStarSyntax if the tag name is
      * not valid star syntax for a tag name.
      * @exception BadValueForDelimiter if the value for the
      * node won't be valid syntax given the kind of delimiter
      * the node has.
      */
    public DataItemNode( String name, String value, short delim )
	throws NameViolatesStarSyntax, BadValueForDelimiter
    {
	super();
	myDataName = new DataNameNode(name);
	myDataName.setParent(this);
	myDataValue = new DataValueNode( value, delim );
	myDataValue.setParent(this);
    }

    /** copy constructor: */
    public DataItemNode( DataItemNode copyMe )
    {
	super(copyMe);
	myDataName = (DataNameNode) ( copyMe.myDataName.clone() );
	myDataName.setParent(this);
	myDataValue = (DataValueNode) ( copyMe.myDataValue.clone() );
	myDataValue.setParent(this);
    }

    /** Allocates a new copy of me and returns a reference to it.
      * This is a deep copy, meaning that all children are copied
      * instead of linked.
      */
    public Object clone()
    {
	return new DataItemNode( this );
    }

    /** Returns the delimiter type of the value in this item.
      * @see DataValueNode::NON
      * @see DataValueNode::DOUBLE
      * @see DataValueNode::SINGLE
      * @see DataValueNode::SEMICOLON
      * @see DataValueNode::FRAMECODE
      */
    public short getDelimType()
    {
	return myDataValue.getDelimType();
    }

    /** Sets the delimiter type of the value in this item.
      * Note that no checks are done to ensure that the delimiter
      * type given is valid for the string in this class.
      * @see DataValueNode::NON
      * @see DataValueNode::DOUBLE
      * @see DataValueNode::SINGLE
      * @see DataValueNode::SEMICOLON
      * @see DataValueNode::FRAMECODE
      * @exception BadValueForDelimiter if the value for the
      * node won't be valid syntax given the kind of delimiter
      * the node has.
      */
    public void setDelimType( short delim )
	throws BadValueForDelimiter
    {
	myDataValue.setDelimType( delim );
    }

    /** Gets the String value of this item.  */
    public String getValue()
    {
	return myDataValue.getValue();
    }

    /** getValueNode() is like getValue(), but it returns the
      * whole DataValueNode, not just the string inside it.
      * @return the DataValueNode inside this item
      */
    public DataValueNode getValueNode()
    {
	return myDataValue;
    }

    /** Sets the string value of this item.
      * @exception BadValueForDelimiter if the value for the
      * node won't be valid syntax given the kind of delimiter
      * the node has.
      */
    public void setValue( String val )
	throws BadValueForDelimiter
    {
        myDataValue.setValue( val );
    }
 
    /** Sets the String value and delimiter of this item together.
      * @exception BadValueForDelimiter if the value for the
      * node won't be valid syntax given the kind of delimiter
      * the node has.
      */
    public void setValue( String val, short delim )
	throws BadValueForDelimiter
    {
        myDataValue.setValue( val, delim );
    }

    /** Gets the String tag name of this item. */
    public String getLabel()
    {
	return myDataName.getLabel();
    }

    /** Gets the entire DataNameNode inside this item
      * (as opposed to just the string name like
      * getLabel() does.)
      * @return the DataNameNode
      */
    public DataNameNode getNameNode()
    {
	return myDataName;
    }

    /** Sets the String tag name of this item - it's an
      * exception if the name does not begin with an underscore.
      * @exception BadValueForDelimiter if the value for the
      * node won't be valid syntax given the kind of delimiter
      * the node has.
      */
    public void setLabel( String name )
	throws NameViolatesStarSyntax
    {
	myDataName = new DataNameNode(name);
    }

    /** Included for orthogonality with StarNode.  Not really
      * very useful at this scope.
      * The search for names is case insensitive
      */
    public VectorCheckType searchByName( String searchFor )
    {
	VectorCheckType retVal = new VectorCheckType();
	try
	{
	    retVal.addType( Class.forName( StarValidity.clsNameStarNode ));
	    retVal.freezeTypes();
	    if( myDataName.getLabel().equalsIgnoreCase(searchFor) )
		retVal.addElement(this);
	}
	catch( ClassNotFoundException exc )
	{   System.err.println( "Should never happen exception: " +
		    exc.getMessage() );
	    exc.printStackTrace();
	}
	return retVal;
    }
  
    /** Included for orthogonality with StarNode.  Not really
      * very useful at this scope.
      * The search for names is case insensitive, but the search
      * for values is case-sensitive.
      */
    public VectorCheckType searchByTagValue( String tag, String value )
    {
	VectorCheckType retVal = new VectorCheckType();
	try
	{
	    retVal.addType( Class.forName( StarValidity.clsNameStarNode ));
	    retVal.freezeTypes();
	    if( myDataName.getLabel().equalsIgnoreCase(tag) )
		if( myDataValue.getValue().equals(value) )
		    retVal.addElement(this);
	}
	catch( ClassNotFoundException exc )
	{   System.err.println( "Should never happen exception: " +
		    exc.getMessage() );
	    exc.printStackTrace();
	}
	return retVal;
    }

    /** Included for orthogonality with StarNode.  Not really
      * very useful at this scope.
      */
    public VectorCheckType searchForType( Class type, short delim )
    {
	VectorCheckType retVal = new VectorCheckType();
	int             i;

	try
	{
	    retVal.addType( Class.forName( StarValidity.clsNameStarNode));
	    retVal.freezeTypes();
	    if( type.isInstance( this ) )
	    {   retVal.addElement( this );
	    }
	    else if( type.isInstance( myDataValue ) )
	    {   if(     delim == myDataValue.getDelimType()  ||
			delim == DataValueNode.DONT_CARE  )
		    retVal.addElement( myDataValue );
	    }
	    else if( type.isInstance( myDataName ) )
	    {   retVal.addElement( myDataName );
	    }

	}
	catch( ClassNotFoundException exc )
	{   System.err.println( "Should never happen exception: " +
		    exc.getMessage() );
	    exc.printStackTrace();
	}
	return retVal;
    }

    /** Unparse prints the contents of the StarNode object out to the
      * given stream.  This is essentially the inverse of the CS term
      * to "parse", hence the name "Unparse".  The parameter given is
      * the indentation level to print things.
      */
    public void Unparse( int indent )
    {
	// TODO - when I know more about Java printing.
    }

    protected DataNameNode myDataName;
    protected DataValueNode myDataValue;
}


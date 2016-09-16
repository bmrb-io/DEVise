package EDU.bmrb.starlibj;

import java.lang.*;
import java.util.*;

/** A DataLoopNode holds one loop from the Star file tree.
  * The loop can be nested.  A DataLoopNode is mostly just
  * a pairing of a LoopTableNode with its associated
  * DataLoopNameListNode.  Most of the interesting methods
  * are contained in one of those two types.
  */
public class DataLoopNode extends StarNode implements Cloneable
{
    /** constructor - makes empty DataLoopNode.  Defaults its
      * loop format to 'linear'.
      */
    public DataLoopNode()
    {
	super();
	myVals = new LoopTableNode();
	myNames = new DataLoopNameListNode();
	myVals.setParent(this);
	myNames.setParent(this);
	parseMismatchFlag = false;
	skipper = null;
    }
    /** constructor - makes a loop with the given tabulation setting.
      * @param tab true = loop will print tabularly, false = linearly
      */
    public DataLoopNode( boolean tab )
    {
	super();
	myVals = new LoopTableNode();
	myNames = new DataLoopNameListNode();
	myVals.setTabFlag(tab);
	myVals.setParent(this);
	myNames.setParent(this);
	parseMismatchFlag = false;
	skipper = null;
    }
    /** constructor - makes a loop with the given settings.
      * @param tab true = loop will print tabularly, false = linearly
      * @param indent true = loop body will be indented (normal), false = not indented.
      * @param rowsPerLine integer number of rows printed in a line of text. Normally
      *        you set it to 1.
      */
    public DataLoopNode( boolean tab, boolean indent, int rowsPerLine )
    {
	super();
	myVals = new LoopTableNode();
	myNames = new DataLoopNameListNode();
	myVals.setTabFlag(tab);
	myVals.setIndentFlag(indent);
	myVals.setRowsPerLine(rowsPerLine);
	myVals.setParent(this);
	myNames.setParent(this);
	parseMismatchFlag = false;
	skipper = null;
    }

    /** Constructor, given the name list and table.  Typically only
      * used by the parser.
      */
    public DataLoopNode( DataLoopNameListNode names, LoopTableNode table )
    {
	myNames = names;
	myVals = table;
	myVals.setParent(this);
	myNames.setParent(this);
	parseMismatchFlag = false;
	skipper = null;
    }

    /** copy constructor - deep copy. */
    public DataLoopNode( DataLoopNode copyMe )
    {
	super(copyMe);
	int i;

	myVals = (LoopTableNode) copyMe.getVals().clone();
	myVals.setParent(this);
	myNames = (DataLoopNameListNode) copyMe.getNames().clone();
	myNames.setParent(this);
	parseMismatchFlag = copyMe.parseMismatchFlag;
	skipper = new SkipTextHandler( copyMe.mySkips() );
    }

    /** clone - allocate a copy of me and return it */
    public Object clone()
    {
	return new DataLoopNode(this);
    }

    /** return a reference to the LoopTableNode of values in me. */
    public LoopTableNode getVals()
    {
	return myVals;
    }
    /** return a reference to the DataLoopNameList of names in me. */
    public DataLoopNameListNode getNames()
    {
	return myNames;
    }

    /** Get the tabulation flag for this loop:
      * @return the tab flag
      */
    public boolean getTabFlag()
    {   return myVals.getTabFlag();
    }

    /** Set the tabulation flag for this loop:
      * @param  fl the tab flag to set it to.
      */
    public void  setTabFlag( boolean fl )
    {   myVals.setTabFlag( fl );
    }

  /** searchByName() will generate a list of all the places a particular
    * name exists in this Star object.  This name will match tag names
    * in this DataLoopNode.  The full tag name
    * must be passed, so to look for a tag called <TT>foo</TT>, you need to
    * use the underscore in the name: <TT>"_foo"</TT>.  Also, to look for
    * a saveframe called <TT>foo</TT>, you need the save_ prefix, like 
    * this: <TT>"save_foo"</TT>.  This search is an exact string match,
    * and it is case-sensitive.
    * <P>
    * The returned results are DataNameNodes when looking in a DataLoopNode
    * like this.
    * <P>
    * It should be noted that this algorithm, and the other search
    * algorithms that follow, are simple linear searches with no indexing.
    * So they are computationally slow.  So far the need has not yet
    * surfaced for a faster indexed search technique, although one could
    * be added behind the scenes without changing the interface.
    * <P>
    * The search for names is case-insensitive.
    *
    * @param searchFor the string name to look for.
    * @return A VectorCheckType containing the StarNodes that matched.
    *         This vector will have a size of zero if there are no matches.
    */
    public VectorCheckType searchByName( String searchFor )
    {
	VectorCheckType retVal;
	retVal = myNames.searchByName(searchFor);
	return retVal;
    }

  /** Given a tag name and a value, find the DataValueNodes from
    * inside this loop that match the criteria.  More than one
    * DataValueNode could be returned from the same DataLoopNode if
    * the value being looked-for appears more than once in the column
    * for a tag name.
    * <P>
    * The search for tag names is case-insensitive.
    * <P>
    * The search for values, however is case-sensitive.
    * <P>
    * @param tag - Look for this tag...
    * @param value - Where it has this value.
    * @return A java.util.vector containing the matching StarNodes.
    *         This vector will have a size of zero if there are no matches.
    */
    public VectorCheckType searchByTagValue( String tag, String value )
    {
	VectorCheckType  retVal;
	RemoteInt        nestLvl = new RemoteInt();
	RemoteInt        column  = new RemoteInt();

	myNames.tagPositionDeep( tag, nestLvl, column );
	retVal = myVals.searchForValsInColumn( nestLvl.num , column.num, value );

	return retVal;
    }

  /** This method returns a vector of all the nodes of the given type.
    * It is much like searchByName() in that it heirarchically walks
    * the STAR tree and calls the searchForType() functions of the subtrees
    * within the tree.  In this way it is possible to call this function
    * at any level of the STAR file.
    * <p>
    * The second parameter is optional and is only useful when you are
    * searching for DataValueNodes.  It determines the kind of
    * DataValueNode you are searching for, by delimiter type.  For
    * example, you could search for only those DataValueNodes that
    * are semicolon-delimited by passing DataValueNode::SEMICOLON
    * as the second argument.  Or you could look for just framecodes
    * by passing DataValueNode::FRAMECODE as the second parameter.
    * Passing a negative number says you want all the DataValueNodes,
    * regardless of their delimiter type.
    * <p>
    * If the search is for some ASTtype other than DataValueNode, then
    * it is irrelevant what the second parameter of this function is, as
    * it will never be used - You can just leave it off and accept the
    * default.
    * <p>
    * @param type - type to search for 
    * @param delim - DataValueNode::ValType to look for.  Default = "dont-care".
    * @return A java.util.vector containing the matching StarNodes.
    *         This vector will have a size of zero if there are no matches.
    */
    public VectorCheckType searchForType( Class type, short delim )
    {
	// Default behaviour if not overridden is to check my
	// own type and add myself to the list if I am the
	// type being looked for.
	VectorCheckType retVal = new VectorCheckType();
	int  i;

	i = delim; // Useless statement to get the compiler to shut up.

	try
	{   retVal.addType( Class.forName( StarValidity.clsNameStarNode ) );
	    retVal.freezeTypes();

	    if( type.isInstance(this) )
	        retVal.addElement(this);

	    if( type == Class.forName( StarValidity.clsNameDataNameNode) )
	    {
		retVal = myNames.searchForType( type, delim );
	    }
	    else if( type == Class.forName( StarValidity.clsNameDataValueNode ) )
	    {
		retVal = myVals.searchForType( type, delim );
	    }
	    // Else nothing to return - return the default empty list.
	}
	catch( ClassNotFoundException exc )
	{   System.err.println( "Should never happen exception: " +
		                exc.getMessage() );
	    exc.printStackTrace();
	}
	return retVal; // Intended to be overridden
    }
    public SkipTextHandler mySkips()
    {
        if( skipper != null )
            return skipper;
        else
        {   skipper = new SkipTextHandler();
            return skipper;
        }
    }
    public    SkipTextHandler  skipper;

    protected DataLoopNameListNode myNames;
    protected LoopTableNode        myVals;
    protected boolean              parseMismatchFlag;
}


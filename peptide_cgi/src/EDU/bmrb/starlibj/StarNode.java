package EDU.bmrb.starlibj;

import java.lang.*;
import java.util.*;

/** StarNode is the generic class on which every other node in the
  * STAR file tree is based.  All nodes in the STAR tree are
  * derived from StarNode.  Some of the functionality is described
  * in StarNode so that it can be guaranteed to always be there even
  * when the type of StarNode is unknown.  (You can <TT>searchByTag</TT>
  * on any StarNode, whether you cast it to a specific type or not.)
  */
public class StarNode implements Cloneable
{

    /** Default Constructor. */
    public StarNode()
    {
	parent     = null;
	lineNum    = -1;
	colNum     = -1;
	preComment = "";
    }

    /** Copy Constructor - deep copy.  This simply copies
      * the node given, deeply down to the leaf nodes.
      */
    public StarNode( StarNode copyMe )
    {
	parent     = copyMe.parent;
	lineNum    = copyMe.lineNum;
	colNum     = copyMe.colNum;
	preComment = (copyMe.preComment == null ) ?
	                  null : new String( copyMe.preComment );
    }

    /** Unparse prints the contents of the StarNode object out to the
      * given stream.  This is essentially the inverse of the CS term
      * to "parse", hence the name "Unparse".  The parameter given is
      * the indentation level to print things.
      */
    public void Unparse( int indent )
    {
	// Nothing - this method is intended to be overridden.
    }

  /** Return the parent of this StarNode.  In other words, return the
    * StarNode object in which this StarNode is inserted.
    */
    public StarNode getParent()
    {
	return parent;
    }

  /** searchByName() will generate a list of all the places a particular
    * name exists in this AST object.  This name will match tag names or
    * saveframe names or data block names.  Note that the full string name
    * must be passed, so to look for a tag called <TT>foo</TT>, you need to
    * use the underscore in the name: <TT>"_foo"</TT>.  Also, to look for
    * a saveframe called <TT>foo</TT>, you need the save_ prefix, like 
    * this: <TT>"save_foo"</TT>.  This search is an exact string match,
    * and it is case-sensitive.
    * <P>
    * This search is fully recursive.  All the parts of the star tree
    * that exist below this point will also be searched.  Therefore if
    * you call a StarFileNode's searchByName(), you search the whole
    * star file, while if you call a SaveFrameNode's searchByName() you
    * search just that saveframe.
    * <P>
    * It should be noted that this algorithm, and the other search
    * algorithms that follow, are simple linear searches with no indexing.
    * So they are computationally slow.  So far the need has not yet
    * surfaced for a faster indexed search technique, although one could
    * be added behind the scenes without changing the interface.
    * <P>
    * The search for names is case-insensitive.
    * @param searchFor the string name to look for.
    * @return A VectorCheckType containing the StarNodes that matched.
    *         This vector will have a size of zero if there are no matches.
    */
    public VectorCheckType searchByName( String searchFor )
    {
	VectorCheckType retVal = new VectorCheckType();
	try
	{   retVal.addType( Class.forName( StarValidity.clsNameStarNode) );
	    retVal.freezeTypes();
	}
	catch( ClassNotFoundException exc )
	{   System.err.println( "Should never happen exception: " +
		                exc.getMessage() );
	    exc.printStackTrace();
	}
	return retVal; // Intended to be overridden
    }

  /** Given a tag name and a value, find the AST object that that
    * particular tag and value pair resides in.  This is like
    * performing an SQL search: WHERE tag = value.
    * <p>
    * Only searches starting at the node it was called from, and
    * its children.  Recurses downward, but does not recurse upward.
    * This function is only capable of returning one answer, so it
    * cannot be called at the same levels where searchByTag() can
    * be called (see above).
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
	VectorCheckType retVal = new VectorCheckType();
	try
	{   retVal.addType( Class.forName( StarValidity.clsNameStarNode) );
	    retVal.freezeTypes();
	}
	catch( ClassNotFoundException exc )
	{   System.err.println( "Should never happen exception: " +
		                exc.getMessage() );
	    exc.printStackTrace();
	}
	return retVal; // Intended to be overridden
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
	VectorCheckType retVal;
	int  i;

	i = delim; // Useless statement to get the compiler to shut up.

	retVal = new VectorCheckType();
	try
	{ retVal.addType( Class.forName( StarValidity.clsNameStarNode) );
	    retVal.freezeTypes();
	    if( type.isInstance(this) )
		retVal.addElement( this );
	}
	catch( ClassNotFoundException exc )
	{   System.err.println( "Should never happen exception: " +
		                exc.getMessage() );
	    exc.printStackTrace();
	}
	return retVal; // Intended to be overridden
    }

  /** Find all the occurrances where there is a node of the given type
    * containing something with the given name.
    * <p>
    * First, this method searches for the given string name, just like
    * <TT>searchForName()</TT> does.  Then looks to see if that node is
    * of the type requested.  If not, it looks at the parent node to see if
    * it is of the type given.  If not that, then it looks at the grandparent
    * node, and so on up until it hits the root of the tree.
    * <P>
    * In general, this method can be thought of as meaning, "Search for the
    * nodes of such-and-such a type that <b>contain</b> this name."
    * <p>
    * If a name is matched, but it is not contained in a node of the specified
    * type, then that is not considered a match, and it is not returned.
    * <p>
    * The search for names is case-insensitive.
    * @param type - the type to search for.
    * @param name - the name to search for.
    * @return A VectorCheckType containing the StarNode objects
    *         that were matched.  If nothing matched, an empty vector is
    *         returned.
    */
    public VectorCheckType searchForTypeByName( Class type, String name )
    {
	int             i;
	StarNode         par;
	VectorCheckType retVal;
	
	retVal = searchByName( name );
	for( i = 0 ; i < retVal.size() ; i++ )
	{
	    par = (StarNode) retVal.elementAt(i);
	    while( par != null && ! (type.isInstance(par)) )
		par = par.getParent();
	    if( par == null )
	    {
		retVal.removeElementAt(i);
		i--;
	    }
	    else
		retVal.setElementAt( par, i );
	}
	return retVal;
    }

  /** This is much like <TT>searchForTypeByTagValue()</TT> above, except that
    * it looks for places where the given tag/value matches, <b>and</b> it
    * contains the given value, <b>then</b> it looks to find a node of the
    * given type that the match is inside of.
    * <P>
    * The search for tag names is case-insensitive.
    * <P>
    * The search for values, however is case-sensitive.
    * <P>
    * @param type - the type to search for.
    * @param tag - the tag name to search for.
    * @param value - the value to search for.
    * @return A VectorCheckType containing the StarNode objects
    *         that were matched.  If nothing matched, an empty vector is
    *         returned.
    */
    public VectorCheckType searchForTypeByTagValue( Class type, String tag, String value )
    {
	int              i;
	StarNode         par;
	VectorCheckType  retVal;
	
	retVal = searchByTagValue( tag, value );
	for( i = 0 ; i < retVal.size() ; i++ )
	{
	    par = (StarNode) retVal.elementAt(i);
	    while( par != null && ! (type.isInstance(par) ) )
		par = par.getParent();
	    if( par == null )
	    {
		retVal.removeElementAt(i);
		i--;
	    }
	    else
		retVal.setElementAt( par, i );
	}
	return retVal;
    }

    /** get parallelCopy is not really implemented yet.  It is here as a
      * stub.
      */
    public StarNode getParallelCopy()
    {
	// TODO
	return null;
    }

    /** setPeer is not really implemented yet.  It is here as a stub. */
    public void setPeer( StarNode peer )
    {
	// TODO
    }

    /** Get the line number that this node was on in the original file.
      * Returns -1 if this node was added programmatically and therefore was
      * not actually on a line number in any original file.
      */
    public int getLineNum()
    {
	return lineNum;
    }
    /** setLineNum sets the line number from the text file for this node.
      * This really only has meaningful purpose for the parser.
      */
    public void setLineNum( int num )
    {
	lineNum = num;
    }
    /** Get the column number that this node was on in the original file.
      * Returns -1 if this node was added programmatically and therefore was
      * not actually on a column number in any original file.
      */
    public int getColNum()
    {
	return colNum;
    }
    /** setColNum sets the column number from the text file for this node.
      * This really only has meaningful purpose for the parser.
      */
    public void setColNum( int num )
    {
	colNum = num;
    }
    /** This functions are used to give each node in the
      * AST tree the ability to remember a comment to be
      * pasted into the file in front of that node.  This
      * is useful if you want to insert header comments of
      * some sort into the output produced by Unparse().
      * As of this writing, no provisions are being made
      * to handle the parsing of comments from the original
      * file and storing them via these functions.  The
      * grammar to do that would be rather convoluted.
      * These functions are only intended to be used by programs
      * insterting their own comments after the file has
      * been read.
      * <p>
      * The string must contain the comment characters embedded
      * inside, like so: "# this is a\n# multiline comment.",
      * not like this: "this is a\nmultiline comment."  This is
      * so that the caller is allowed to have the comment contain
      * blank lines like this:
      * <pre>
      *      # This is an example comment.
      *
      *      # The comment has some blank
      *
      *      # lines in it.
      * </pre>
      * If the Unparse() function were designed to insert the comment
      * characters (#) itself, then such a comment block would be impossible
      * to create.
      * <p>
      * Note that the comment lines are not syntax-checked in any way, so
      * using these functions it is entirely possible to create invalid
      * STAR files, since these "comments" can really be strings with
      * anything at all in them - so be careful.
      * <p>
      * To get rid of the preComment if you change your mind, set it to
      * a zero-length string with setPreComment().
      */
    public String getPreComment( )
    {
	return preComment;
    }
    public void setPreComment( String cmt)
    {
	preComment = cmt;
    }

    public SkipTextHandler mySkips()
    {
        return null;
    }

    /** Allocates a new copy (clone) of this StarNode and returns a reference
      * to it.  The copy is deep, meaning that the children are also copied
      * (rather than linked).  Thus copying a StarFileNode results in a new
      * copy of the whole treee.
      * @see StarFileNode
      */
    public Object clone()
    {
	return new StarNode( this );
    }


    protected StarNode   parent;
    protected int       lineNum;
    protected int       colNum;
    protected String    preComment;

    protected void setParent( StarNode p )
    {
	parent = p;
    }

}


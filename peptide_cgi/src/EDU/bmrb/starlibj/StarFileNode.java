package EDU.bmrb.starlibj;

import java.lang.*;
import java.util.*;

/** StarFileNode is the mother of all STAR nodes (literally).
  * This node is the root of the whole star tree for a star file.
  * If a star file is parsed in with the parser, it produces
  * an object of this type, which in turn contains the rest of
  * the tree.
  */
public class StarFileNode extends StarNode implements Cloneable
{
    /** Constructor - makes an empty star file:
      */
    public StarFileNode()
    {
	super();
	myStarList = new StarListVector();
	skipper = null;
    }

    /** Copy Constructor - makes a copy of an existing tree:
      */
    public StarFileNode( StarFileNode copyMe )
    {
	super(copyMe);
	int i;
	myStarList = new StarListVector();
	for( i = 0 ; i < copyMe.size() ; i++ )
	    addElement( copyMe.elementAt(i).clone() );
	skipper = new SkipTextHandler( copyMe.mySkips() );
    }

    // ---------------- StarVectorLike ----------------------
    // ---------------- interface ---------------------------

    /** Just like the Vector method of the same name.
      * Makes enough room so that there can be <tt>newSize</tt>
      * elements in the node, without having to insert them
      * manually one at a time.  The nodes start out will
      * a null value that can be replaced with <tt>setElementAt</tt>.
      * @see VectorCheckType.setSize
      */
    public  void setSize(int newSize)
    {
	myStarList.setSize(newSize);
    }
    /** Just like the Vector method of the same name.
      * Returns the number of block elements in this Star
      * File.
      * @see java.util.Vector.size
      */
    public int size()
    {   return myStarList.size();
    }
    /** Just like the Vector method of the same name.
      * True if the star file has nothing in it.
      * @see java.util.Vector.isEmpty
      */
    public boolean isEmpty()
    {   return myStarList.isEmpty();
    }
    /** Just like the Vector method of the same name.
      * Gives an enumeration over the blocks in the
      * star file.
      * @see java.util.Vector.Enumeration
      */
    public  Enumeration elements()
    {   return myStarList.elements();
    }
    /** Just like the Vector method of the same name.
      * True if the node given is a BlockNode inside
      * this StarFileNode.
      * @param obj The BlockNode to look for.
      * @see java.util.Vector.contains
      */
    public boolean contains(Object obj)
    {   return myStarList.contains(obj);
    }
    /** Just like the Vector method of the same name.
      * Returns the integer index of the Given BlockNode
      * inside this StarFileNode.
      * @param obj The BlockNode to look for.
      * @see java.util.Vector.indexOf
      */
    public int indexOf(Object obj)
    {   return myStarList.indexOf(obj);
    }
    /** Just like the Vector method of the same name.
      * Returns the integer index of the next BlockNode
      * inside this StarFileNode, starting at the index
      * given.
      * @param obj The BlockNode to look for.
      * @param index Start searching at this point in the vector.
      * @see java.util.Vector.indexOf
      */
    public  int indexOf(Object obj,
                                       int index)
    {   return myStarList.indexOf(obj,index);
    }
    /** Just like the Vector method of the same name.
      * Returns the lastmost integer index of the given
      * BlockNode.
      * @param obj The BlockNode to look for.
      * @see java.util.Vector.lastIndexOf
      */
    public int lastIndexOf(Object obj)
    {   return myStarList.lastIndexOf(obj);
    }
    /** Just like the Vector method of the same name.
      * Returns the lastmost integer index of the given
      * BlockNode, but going no higher than the given
      * index.
      * @param obj The BlockNode to look for.
      * @param index Start searching back from this point in
      * the vector.
      * @see java.util.Vector.lastIndexOf
      */
    public  int lastIndexOf(Object obj,
                                           int index)
    {   return myStarList.lastIndexOf(obj,index);
    }
    /** Just like the Vector method of the same name.
      * Returns the BlockNode object at the given index.
      * @param index The index to return the BlockNode at.
      * @return The Returned Object is a BlockNode and can be safely
      *         casted to BlockNode.
      * @see java.util.Vector.elementAt
      */
    public  BlockNode elementAt(int index)
    {   return (BlockNode) ( myStarList.elementAt(index) );
    }
    /** Just like the Vector method of the same name.
      * Returns the first BlockNode object in the file.
      * @return The Returned Object is a BlockNode and can be safely
      *         casted to BlockNode.
      * @see java.util.Vector.firstElement
      */
    public  BlockNode firstElement()
    {   return (BlockNode) (myStarList.firstElement());
    }
    /** Just like the Vector method of the same name.
      * Returns the last Blocknode object in the file.
      * @return The Returned Object is a BlockNode and can be safely
      *         casted to BlockNode.
      * @see java.util.Vector.lastElement
      */
    public  BlockNode lastElement()
    {   return (BlockNode) ( myStarList.lastElement() );
    }
    /** Just like the Vector method of the same name.
      * Clobbers the BlockNode at the given index
      * with the blocknode given.
      * @param obj The BlockNode to set it to.
      * @param index the position to replace.
      * @see java.util.Vector.setElementAt
      */
    public  void setElementAt(Object obj,
                                             int index)
	    throws WrongElementType
    {
	myStarList.setElementAt(obj,index);
	((StarNode)myStarList.elementAt(index)).setParent(this);
    }
    /** Similar to the Vector method of the same name.
      * Deletes the BlockNode at the given index from the
      * star file node.
      * @param index the position to remove.
      * @see java.util.Vector.removeElementAt
      */
    public  void removeElementAt(int index)
    {
	((StarNode)myStarList.elementAt(index)).setParent(null);
	myStarList.removeElementAt(index);
    }
    /** Just like the Vector method of the same name.
      * Inserts a Blocknode.
      * @param obj The BlockNode to insert.
      * @param index the position to insert before.
      * @see java.util.Vector.insertElementAt
      */
    public  void insertElementAt(Object obj,
                                                int index)
	    throws WrongElementType
    {   
	myStarList.insertElementAt(obj,index);
	((StarNode)myStarList.elementAt(index)).setParent(this);
    }
    /** Just like the Vector method of the same name.
      * Adds a BlockNode to the end of the list.
      * @param obj The BlockNode to add.
      * @see java.util.Vector.addElement
      */
    public  void addElement(Object obj)
	    throws WrongElementType
    {
	myStarList.addElement(obj);
	((StarNode)myStarList.lastElement()).setParent(this);
    }
    /** Just like the Vector method of the same name.
      * Removes the Blocknode matching the one given.
      * @param obj The BlockNode to remove.
      * @see java.util.Vector.removeElement
      */
    public  boolean removeElement( Object obj)
    {
	((StarNode)obj).setParent(null);
	return myStarList.removeElement( obj );
    }

    /** Allocates a new copy of me and returns a reference to it.
      * This is a deep copy, meaning that all children are copied
      * instead of linked.
      */
    public Object clone()
    {
	return new StarFileNode( this );
    }

    /** Given a tag name, find the AST object it resides in.  It returns
      * a reference to the lowest level AST object that the tag resides in.
      * The caller needs to use the type-aware features of Java to discover
      * what the object's type really is.
      * <P>
      * The search for names is case-insensitive.
      *
      * @param searchFor - Look for this string as the tag name.
      */
    public VectorCheckType searchByName( String searchFor )
    {
        VectorCheckType retVal = new VectorCheckType();
	VectorCheckType tmpVect;
        int             i,j;

	try
	{
	    retVal.addType( Class.forName( StarValidity.clsNameStarNode) );
	    retVal.freezeTypes();

	    for( i = 0 ; i < myStarList.size() ; i++ )
	    {
		tmpVect = ( (StarNode)myStarList.elementAt(i)
			  ).searchByName(searchFor);
		for( j = 0 ; j < tmpVect.size() ; j++ )
		    retVal.addElement( tmpVect.elementAt(j) );
	    }
	}
	catch( ClassNotFoundException exc )
	{   System.err.println( "Should never happen exception: " +
		                exc.getMessage() );
	    exc.printStackTrace();
	}
	return retVal;
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
	VectorCheckType tmpVect;
        int             i,j;

	try
	{   retVal.addType( Class.forName( StarValidity.clsNameStarNode ) );
	    retVal.freezeTypes();

	    for( i = 0 ; i < myStarList.size() ; i++ )
	    {
		tmpVect = ( (StarNode)myStarList.elementAt(i)
			  ).searchByTagValue(tag,value);
		for( j = 0 ; j < tmpVect.size() ; j++ )
		    retVal.addElement( tmpVect.elementAt(j) );
	    }
	}
	catch( ClassNotFoundException exc )
	{   System.err.println( "Should never happen exception: " +
		                exc.getMessage() );
	    exc.printStackTrace();
	}
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
        VectorCheckType retVal = new VectorCheckType();
	VectorCheckType tmpVect;
        int             i,j;

	try
	{   retVal.addType( Class.forName( StarValidity.clsNameStarNode) );
	    retVal.freezeTypes();

	    // See if I qualify as a 'hit':
	    if( type.isInstance(this) )
		retVal.addElement( this );
	    for( i = 0 ; i < myStarList.size() ; i++ )
	    {
		tmpVect = ( (StarNode)myStarList.elementAt(i)
			  ).searchForType(type, delim);
		for( j = 0 ; j < tmpVect.size() ; j++ )
		    retVal.addElement( tmpVect.elementAt(j) );
	    }
	}
	catch( ClassNotFoundException exc )
	{   System.err.println( "Should never happen exception: " +
		                exc.getMessage() );
	    exc.printStackTrace();
	}
	return retVal;
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

    protected StarListVector myStarList;

}


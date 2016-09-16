package EDU.bmrb.starlibj;

import java.lang.*;
import java.util.*;
import EDU.bmrb.starlibj.*;

/** A single block in the file - this can be either a
  * data block or a global block - they are both handled
  * the same way.
  */
public class BlockNode extends StarNode implements Cloneable
{

    /** Make a block given its name, which must be of the form
      * <TT>global_</TT> or <TT>data_*</TT>.
      */
    public BlockNode( String name )
	throws NameViolatesStarSyntax
    {
	super();
	if( ! StarValidity.isValidBlockName(name) )
	    throw new NameViolatesStarSyntax(name,"Block Name");
	myHeading = name;
	myDataList = new BlockListVector();
        skipper = null;
    }

    /** Copy constructor - deep copy. */
    public BlockNode( BlockNode copyMe )
    {
	super(copyMe);
	int i;

	myDataList = new BlockListVector();
	for( i = 0 ; i < copyMe.size() ; i++ )
	    addElement( ((StarNode)copyMe.elementAt(i)).clone() );

	myHeading = (copyMe.myHeading == null ) ?
	                null : new String( copyMe.myHeading );
	skipper = new SkipTextHandler( copyMe.mySkips() );
    }

    /** Allocates a new copy of me and returns a reference to it.
      * This is a deep copy, meaning that all children are copied
      * instead of linked.
      */
    public Object clone()
    {
	return new BlockNode( this );
    }

    /** Return my name (global_ or data_..) */
    public String getLabel()
    {
	return myHeading;
    }
    /** Change my name (global_ or data_..) - generate an
      * exception if the given name is not valid.
      * <P>
      */
    public void setLabel( String newName )
	throws NameViolatesStarSyntax
    {
	myHeading = newName;
	if( ! StarValidity.isValidBlockName( newName ) )
	    throw new NameViolatesStarSyntax(newName,"Block Name");
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
	myDataList.setSize(newSize);
    }
    /** Just like the Vector method of the same name.
      * Returns the number of elements in this block.
      * @see java.util.Vector.size
      */
    public int size()
    {   return myDataList.size();
    }
    /** Just like the Vector method of the same name.
      * True if this block has no elements in it.
      * @see java.util.Vector.isEmpty
      */
    public boolean isEmpty()
    {   return myDataList.isEmpty();
    }
    /** Just like the Vector method of the same name.
      * Gives an enumeration over the loops, saveframes,
      * and data items in this block.
      * @see java.util.Vector.Enumeration
      */
    public  Enumeration elements()
    {   return myDataList.elements();
    }
    /** Just like the Vector method of the same name.
      * True if the node given is a loop, item, or
      * saveframe immediately inside this block.  This
      * is <b>not</b> a deep recursive search - it only
      * looks at the level immediately inside the block.
      * @param obj The loop, item, or saveframe to look for
      * @see java.util.Vector.contains
      */
    public boolean contains(Object obj)
    {   return myDataList.contains(obj);
    }
    /** Just like the Vector method of the same name.
      * Returns the integer index of the given item, loop
      * or saveframe inside this StarFileNode.  This is
      * <b>not</b> a deep recursive search.
      * @param obj The loop, item, or saveframe to look for
      * @see java.util.Vector.indexOf
      */
    public int indexOf(Object obj)
    {   return myDataList.indexOf(obj);
    }
    /** Just like the Vector method of the same name.
      * Returns the integer index of the next item, loop
      * or saveframe inside this StarFileNode, starting at
      * the index given.  This is <b>not</b> a deep recursive
      * search.
      * @param obj The item, loop, or saveframe to look for.
      * @param index Start searching at this point in the vector.
      * @see java.util.Vector.indexOf
      */
    public  int indexOf(Object obj,
                                       int index)
    {   return myDataList.indexOf(obj,index);
    }
    /** Just like the Vector method of the same name.
      * Returns the lastmost integer index of the given
      * item, loop, or saveframe
      * @param obj The item, loop, or saveframe to look for.
      * @see java.util.Vector.lastIndexOf
      */
    public int lastIndexOf(Object obj)
    {   return myDataList.lastIndexOf(obj);
    }
    /** Just like the Vector method of the same name.
      * Returns the lastmost integer index of the given
      * item, loop, or saveframe, but going no higher
      * than the given index.
      * @param obj The item, loop, or saveframe to look for.
      * @param index Start searching back from this point in
      * the vector.
      * @see java.util.Vector.lastIndexOf
      */
    public  int lastIndexOf(Object obj,
                                           int index)
    {   return myDataList.lastIndexOf(obj,index);
    }
    /** Just like the Vector method of the same name.
      * Returns the item, loop, or saveframe object at the given index.
      * @param index The index to return the item, loop, or saveframe at.
      * @return The returned object is either an item, loop, or saveframe.
      * @see java.util.Vector.elementAt
      */
    public  StarNode elementAt(int index)
    {   return (StarNode) ( myDataList.elementAt(index) );
    }
    /** Just like the Vector method of the same name.
      * Returns the first item. loop. or saveframe object in the file.
      * @return The returned object is a item, loop, or saveframe.
      * @see java.util.Vector.firstElement
      */
    public  StarNode firstElement()
    {   return (StarNode) (myDataList.firstElement());
    }
    /** Just like the Vector method of the same name.
      * Returns the last item, loop, or saveframe object in the file.
      * @return The returned object is a item, loop, or saveframe
      * @see java.util.Vector.lastElement
      */
    public  StarNode lastElement()
    {   return (StarNode) (myDataList.lastElement());
    }
    /** Just like the Vector method of the same name.
      * Clobbers the item, loop, or saveframe at the given index
      * with the object given.
      * @param obj The item, loop, or saveframe to set it to.
      * @param index the position to replace.
      * @see java.util.Vector.setElementAt
      */
    public  void setElementAt(Object obj, int index)
	    throws WrongElementType
    {   
	myDataList.setElementAt(obj,index);
	((StarNode)myDataList.elementAt(index)).setParent(this);
    }
    /** Similar to the Vector method of the same name.
      * Deletes the item, loop, or saveframe at the given index from the
      * block node
      * @param index the position to remove.
      * @see java.util.Vector.removeElementAt
      */
    public  void removeElementAt(int index)
    {   
	((StarNode)myDataList.elementAt(index)).setParent(null);
	myDataList.removeElementAt(index);
    }
    /** Just like the Vector method of the same name.
      * Inserts an item, loop, or saveframe at the position given.
      * @param obj The item, loop, or saveframe to insert.
      * @param index the position to insert it in from of.
      * @see java.util.Vector.insertElementAt
      */
    public  void insertElementAt(Object obj,
                                                int index)
	    throws WrongElementType
    {  
	myDataList.insertElementAt(obj,index);
	((StarNode)myDataList.elementAt(index)).setParent(this);
    }
    /** Just like the Vector method of the same name.
      * Adds an item, loop or saveframe to the end of the list.
      * @param obj The item, loop, or saveframe to add.
      * @see java.util.Vector.addElement
      */
    public  void addElement(Object obj)
	    throws WrongElementType
    {
	myDataList.addElement(obj);
	((StarNode)myDataList.lastElement()).setParent(this);
    }
    /** Just like the Vector method of the same name.
      * Removes the item, loop, or saveframe matching the one given.
      * @param obj The item, loop, or saveframe to remove.
      * @see java.util.Vector.removeElement
      */
    public  boolean removeElement( Object obj)
    {   
	((StarNode)obj).setParent(null);
	return myDataList.removeElement( obj );
    }


    /** Given a tag name, find the AST object it resides in.  It returns
      * a reference to the lowest level AST object that the tag resides in.
      * The caller needs to use the type-aware features of Java to discover
      * what the object's type really is.
      *
      * The search for names is case-insensitive
      *
      * @param searchFor - Look for this String as the tag name.
      */
    public VectorCheckType searchByName( String searchFor )
    {
	int i,j;
	VectorCheckType retVal = new VectorCheckType();
	VectorCheckType tmpVect;

	try
	{
	    retVal.addType( Class.forName( StarValidity.clsNameStarNode ) );
	    retVal.freezeTypes();
	    // See if I am a 'hit'.
	    if( getLabel().equalsIgnoreCase(searchFor) )
		retVal.addElement( this );
	    for( i = 0 ; i < myDataList.size() ; i++ )
	    {   tmpVect = ( (StarNode)
		            (myDataList.elementAt(i))
			  ).searchByName( searchFor );
		for( j = 0 ; j < tmpVect.size() ; j++ )
		{
		    retVal.addElement(tmpVect.elementAt(j));
		}
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
    * <p>
    * The search for names is case-insensitive
    * <p>
    * The search for values, however is case-sensitive.
    * <p>
    * @param tag - Look for this tag...
    * @param value - Where it has this value.
    * @return A java.util.vector containing the matching StarNodes.
    *         This vector will have a size of zero if there are no matches.
    */
    public VectorCheckType searchByTagValue( String tag, String value )
    {
	int i,j;
	VectorCheckType retVal = new VectorCheckType();
	VectorCheckType tmpVect;

	try
	{
	    retVal.addType( Class.forName( StarValidity.clsNameStarNode ) );
	    retVal.freezeTypes();
	    for( i = 0 ; i < myDataList.size() ; i++ )
	    {   tmpVect = ( (StarNode) (myDataList.elementAt(i))
			  ).searchByTagValue( tag,value );
		for( j = 0 ; j < tmpVect.size() ; j++ )
		    retVal.addElement(tmpVect.elementAt(j));
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
    * @param delim - DataValueNode::ValType to look for.  Set to
    *        DataValueNode.DONT_CARE if it doesn't matter.
    * @return A java.util.vector containing the matching StarNodes.
    *         This vector will have a size of zero if there are no matches.
    */
    public VectorCheckType searchForType( Class type, short delim )
    {
	int i,j;
	VectorCheckType retVal = new VectorCheckType();
	VectorCheckType tmpVect;

	try
	{
	    retVal.addType( Class.forName( StarValidity.clsNameStarNode ) );
	    retVal.freezeTypes();
	    // Check to see if I myself should be added to the pile:
	    if( type.isInstance(this) )
		retVal.addElement( this );
	    // Check my children.
	    for( i = 0 ; i < myDataList.size() ; i++ )
	    {   tmpVect = ( (StarNode) (myDataList.elementAt(i))
		          ).searchForType( type, delim );
		for( j = 0 ; j < tmpVect.size() ; j++ )
		    retVal.addElement(tmpVect.elementAt(j));
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

    protected BlockListVector  myDataList;
    protected String           myHeading;
}



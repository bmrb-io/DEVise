package EDU.bmrb.starlibj;

import java.lang.*;
import java.util.*;


/** A SaveFrameNode contains the contents of a STAR file saveframe.
  * It has a name, available through "getLabel()" and "setLabel()".  This
  * name must have a prefix of "save_".  A SaveFrameNode behaves like
  * a vector (it implements StarVectorLike) that can contain
  * DataItemNodes and DataLoopNodes.
  */
public class SaveFrameNode extends StarNode implements Cloneable
{
    /** constructor - Generates exception if the given
      * saveframe name is not a legal saveframe name
      */
    public SaveFrameNode( String name)
    {
	super();
	myName = name;
	myDataList = new SaveListVector();
	skipper = null;
    }
    /** copy constructor - deep copy. */
    public SaveFrameNode( SaveFrameNode copyMe )
    {
	super();
	int i;

	myName  = ( copyMe.myName == null ) ?
	             null : new String( copyMe.myName );
	myDataList = new SaveListVector();

	for( i = 0 ; i < copyMe.size() ; i++ )
	    addElement( copyMe.elementAt(i).clone() );
	skipper = new SkipTextHandler( copyMe.mySkips() );
    }
    
    /** clone allocates a copy of me and returns a reference to it. */
    public Object clone()
    {
	return new SaveFrameNode(this);
    }

    /** Gets the string name of this saveframe (save_whatever). */
    public String getLabel()
    {
	return myName;
    }
    /** Sets the string name of this saveframe - generates an
      * exception if the name is invalid.
      */
    public void setLabel( String newName )
	throws NameViolatesStarSyntax
    {
	myName = newName;
	if( ! StarValidity.isValidSaveName(newName) )
	    throw new NameViolatesStarSyntax(newName, "saveframe name");
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
      * Returns the number of elements in this saveframe.
      * @see java.util.Vector.size
      */
    public int size()
    {   return myDataList.size();
    }
    /** Just like the Vector method of the same name.
      * True if this saveframe has no elements in it.
      * @see java.util.Vector.isEmpty
      */
    public boolean isEmpty()
    {   return myDataList.isEmpty();
    }
    /** Just like the Vector method of the same name.
      * Gives an enumeration over the loops and
      * data items in this block.
      * @see java.util.Vector.Enumeration
      */
    public  Enumeration elements()
    {   return myDataList.elements();
    }
    /** Just like the Vector method of the same name.
      * True if the node given is a loop or item
      * immediately inside this saveframe.  This
      * is <b>not</b> a deep recursive search - it only
      * looks at the level immediately inside the saveframe.
      * @param obj The loop or item to look for
      * @see java.util.Vector.contains
      */
    public boolean contains(Object obj)
    {   return myDataList.contains(obj);
    }
    /** Just like the Vector method of the same name.
      * Returns the integer index of the given item or loop
      * inside this saveframe.  This is
      * <b>not</b> a deep recursive search.
      * @param obj The loop or item to look for.
      * @see java.util.Vector.indexOf
      */
    public int indexOf(Object obj)
    {   return myDataList.indexOf(obj);
    }
    /** Just like the Vector method of the same name.
      * Returns the integer index of the next item or loop
      * inside this saveframe, starting at
      * the index given.  This is <b>not</b> a deep recursive
      * search.
      * @param obj The item or loop to look for.
      * @param index Start searching at this point in the vector.
      * @see java.util.Vector.indexOf
      */
    public  int indexOf(Object obj,
                                       int index)
    {   return myDataList.indexOf(obj,index);
    }
    /** Just like the Vector method of the same name.
      * Returns the lastmost integer index of the given
      * item or loop.
      * @param obj The item or loop to look for.
      * @see java.util.Vector.lastIndexOf
      */
    public int lastIndexOf(Object obj)
    {   return myDataList.lastIndexOf(obj);
    }
    /** Just like the Vector method of the same name.
      * Returns the lastmost integer index of the given
      * item or loop but going no higher
      * than the given index.
      * @param obj The item or loop to look for.
      * @param index Start searching back from this point in
      * the vector.
      * @see java.util.Vector.lastIndexOf
      */
    public  int lastIndexOf(Object obj,
                                           int index)
    {   return myDataList.lastIndexOf(obj,index);
    }
    /** Just like the Vector method of the same name.
      * Returns the item or loop object at the given index.
      * @param index The index to return the item or loop at.
      * @return The returned object is either an item or loop.
      * @see java.util.Vector.elementAt
      */
    public  StarNode elementAt(int index)
    {   return (StarNode) ( myDataList.elementAt(index) );
    }
    /** Just like the Vector method of the same name.
      * Returns the first item or loop object in the file.
      * @return The returned object is a item or loop.
      * @see java.util.Vector.firstElement
      */
    public  StarNode firstElement()
    {   return (StarNode) (myDataList.firstElement());
    }
    /** Just like the Vector method of the same name.
      * Returns the last item or loop in the saveframe.
      * @return The returned object is a item or loop.
      * @see java.util.Vector.lastElement
      */
    public  StarNode lastElement()
    {   return (StarNode) ( myDataList.lastElement() );
    }
    /** Just like the Vector method of the same name.
      * Clobbers the item or loop at the given index
      * with the object given.
      * @param obj The item or loop to set it to.
      * @param index the position to replace.
      * @see java.util.Vector.setElementAt
      */
    public  void setElementAt(Object obj,
                                             int index)
	    throws WrongElementType
    {   
	myDataList.setElementAt(obj,index);
	((StarNode)myDataList.elementAt(index)).setParent(this);
    }
    /** Similar to the Vector method of the same name.
      * Deletes the item or loop at the given index from the
      * saveframe.
      * @param index the position to remove.
      * @see java.util.Vector.removeElementAt
      */
    public  void removeElementAt(int index)
    {   
	((StarNode)myDataList.elementAt(index)).setParent(null);
	myDataList.removeElementAt(index);
    }
    /** Just like the Vector method of the same name.
      * Inserts an item or loop at the position given.
      * @param obj The item or loopto insert.
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
      * Adds an item or loop to the end of the saveframe.
      * @param obj The item or loop to add.
      * @see java.util.Vector.addElement
      */
    public  void addElement(Object obj)
	    throws WrongElementType
    {   
	myDataList.addElement(obj);
	((StarNode)myDataList.lastElement()).setParent(this);
    }
    /** Just like the Vector method of the same name.
      * Removes the item or loop matching the one given.
      * @param obj The item or loop to remove.
      * @see java.util.Vector.removeElement
      */
    public  boolean removeElement( Object obj)
    {   
	((StarNode)obj).setParent(this);
	return myDataList.removeElement( obj );
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
	int i,j;
	VectorCheckType retVal = new VectorCheckType();
	VectorCheckType tmpVect;

	try
	{
	    retVal.addType( Class.forName( StarValidity.clsNameStarNode) );
	    retVal.freezeTypes();
	    // See if I am a 'hit'.
	    if( getLabel().equalsIgnoreCase(searchFor) )
		retVal.addElement( this );
	    // Look through my contents:
	    for( i = 0 ; i < myDataList.size() ; i++ )
	    {   tmpVect = ( (StarNode)myDataList.elementAt(i)
		          ).searchByName( searchFor );
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
	int i,j;
	VectorCheckType retVal = new VectorCheckType();
	VectorCheckType tmpVect;

	try
	{
	    retVal.addType( Class.forName( StarValidity.clsNameStarNode) );
	    retVal.freezeTypes();
	    // Look through my contents:
	    for( i = 0 ; i < myDataList.size() ; i++ )
	    {   tmpVect = ( (StarNode)myDataList.elementAt(i)
		          ).searchByTagValue( tag, value );
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
    * If the search is for some StarNode other than DataValueNode, then
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
	    retVal.addType( Class.forName( StarValidity.clsNameStarNode) );
	    retVal.freezeTypes();
	    // Check to see if I myself should be added to the pile:
	    if( type.isInstance(this) )
		retVal.addElement( this );
	    // Check my contents:
	    for( i = 0 ; i < myDataList.size() ; i++ )
	    {   tmpVect = ( (StarNode)myDataList.elementAt(i)
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
	// TODO - look into Java output formatting more.
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

    protected String          myName;
    protected SaveListVector  myDataList;
}


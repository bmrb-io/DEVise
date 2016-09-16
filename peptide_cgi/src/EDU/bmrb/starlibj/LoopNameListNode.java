package EDU.bmrb.starlibj;

import java.lang.*;
import java.util.*;


/** Holds the list of tag names that represents one nesting level
  * of the loop.
  * <P>
  * To keep the API familiar to the user, I have tried to mimic the
  * methods in java.util.vector as closely as possible.
  */
public class LoopNameListNode extends StarNode implements Cloneable
{
    /** empty constructor */
    public LoopNameListNode()
    {
	super();
	nameList = new TagsVector();
    }
    /** copy constructor */
    public LoopNameListNode( LoopNameListNode copyMe )
    {
	super(copyMe);
	int i;
	nameList = new TagsVector();

	for( i = 0 ; i < copyMe.size() ; i++ )
	    addElement( new DataNameNode(copyMe.elementAt(i)) );
    }

    /** From interface <TT>Cloneable</TT>. */
    public Object clone()
    {
	return new LoopNameListNode(this);
    }


    // ---------------- StarVectorLike ----------------------
    // ---------------- interface ---------------------------


    // TODO - none of these have any sort of double-checking on them
    // yet.  Need to add the ability to add columns to them when
    // a name is added, but I haven't gotten that far yet because I haven't
    // got the LoopRowNode and LoopTableNode up and running yet.

    /** Just like the Vector method of the same name.
      * Makes enough room so that there can be <tt>newSize</tt>
      * elements in the node, without having to insert them
      * manually one at a time.  The nodes start out will
      * a null value that can be replaced with <tt>setElementAt</tt>.
      * @see VectorCheckType.setSize
      */
    public void setSize(int newSize)
    {
	nameList.setSize(newSize);
    }
    /** Just like the Vector method of the same name.
      * Returns the number of names in this list
      * @see java.util.Vector.size
      */
    public int size()
    {   return nameList.size();
    }
    /** Just like the Vector method of the same name.
      * True if this list has no names in it.
      * @see java.util.Vector.isEmpty
      */
    public boolean isEmpty()
    {   return nameList.isEmpty();
    }
    /** Just like the Vector method of the same name.
      * Gives an enumeration over the names in this block.
      * @see java.util.Vector.Enumeration
      */
    public Enumeration elements()
    {   return nameList.elements();
    }
    /** Just like the Vector method of the same name.
      * True if the node given is in this name list.
      * @param obj The string name.
      * @see java.util.Vector.contains
      */
    public boolean contains(Object obj)
    {   return nameList.contains(obj);
    }
    /** Just like the Vector method of the same name.
      * Returns the integer index of the given name
      * inside this list.
      * @param obj The name to look for.
      * @see java.util.Vector.indexOf
      */
    public int indexOf(Object obj)
    {   return nameList.indexOf(obj);
    }
    /** Just like the Vector method of the same name.
      * Returns the integer index of the next occurrance
      * of the given name afterthe given index.
      * @param obj The name to look for.
      * @param index Start searching at this point in the vector.
      * @see java.util.Vector.indexOf
      */
    public int indexOf(Object obj,
                                       int index)
    {   return nameList.indexOf(obj,index);
    }
    /** Just like the Vector method of the same name.
      * Returns the lastmost integer index of the given name
      * @param obj The name to look for.
      * @see java.util.Vector.lastIndexOf
      */
    public int lastIndexOf(Object obj)
    {   return nameList.lastIndexOf(obj);
    }
    /** Just like the Vector method of the same name.
      * Returns the lastmost integer index of the given
      * name, but going no higher than the given index.
      * @param obj The name to look for.
      * @param index Start searching back from this point in
      * the vector.
      * @see java.util.Vector.lastIndexOf
      */
    public int lastIndexOf(Object obj,
                                           int index)
    {   return nameList.lastIndexOf(obj,index);
    }
    /** Just like the Vector method of the same name.
      * Returns the name object at the given index.
      * @param index The index to return the name for.
      * @return The returned object is a string.
      * @see java.util.Vector.elementAt
      */
    public DataNameNode elementAt(int index)
    {   return (DataNameNode) (nameList.elementAt(index) );
    }
    /** Just like the Vector method of the same name.
      * Returns the first name in the list
      * @return The returned object is a string.
      * @see java.util.Vector.firstElement
      */
    public  DataNameNode firstElement()
    {   return (DataNameNode) ( nameList.firstElement() );
    }
    /** Just like the Vector method of the same name.
      * Returns the last name in the list.
      * @return The returned object is a string.
      * @see java.util.Vector.lastElement
      */
    public  DataNameNode lastElement()
    {   return (DataNameNode) ( nameList.lastElement() );
    }
    /** Just like the Vector method of the same name.
      * Clobbers the name at the index given with the new name.
      * @param obj The name to replace it with.
      * @param index the position to replace.
      * @see java.util.Vector.setElementAt
      */
    public  void setElementAt(Object obj,
                                             int index)
	    throws WrongElementType
    {   
	nameList.setElementAt(obj,index);
	((DataNameNode)nameList.elementAt(index)).setParent(this);
    }
    /** Similar to the Vector method of the same name.
      * Deletes the name.  If this is contained inside a DataLoopNode,
      * then it also removes all the values from the associated loop table
      * that are under this name.
      * @param index the position to remove.
      * @see java.util.Vector.removeElementAt
      */
    public  void removeElementAt(int index)
    {   
	((DataNameNode)nameList.elementAt(index)).setParent(null);
	nameList.removeElementAt(index);

	int depth = getDepth();
	if( depth >= 0 )
	{
	    StarNode par;

	    for(    par = this ;
		    par != null && !(par instanceof DataLoopNode ) ;
		    par = par.getParent()   )
	    { /*void-body*/ }
	    if( par != null )
	    {   ((DataLoopNode)par).getVals().removeColumnAtDepth(
			depth, index );
	    }
	}
    }

    /** Just like the Vector method of the same name.
      * Inserts a name just in front of the index given..
      * If this is in a DataLoopNode, it also inserts all the
      * appropriate columns into the data below so that it matches
      * the newly inserted name.  The new values will all be
      * star nulls (single dot '.' values).
      * @param obj The name to insert.
      * @param index the position to insert it in from of.
      * @see java.util.Vector.insertElementAt
      */
    public  void insertElementAt(Object obj,
                                                int index)
	    throws WrongElementType
    {   
	insertElementAt( obj, index, new DataValueNode( "." ) );
    }

    /** Identical to the version above, except that the
      * value to be padded into the loop values is chosen
      * by the caller instead of being a dot ('.')
      * @param obj The name to insert.
      * @param index the position to insert it in from of.
      * @param val The value to insert in the columns below.
      * @see java.util.Vector.insertElementAt
      */
    public  void insertElementAt( Object         obj,
	                          int            index,
				  DataValueNode  val)
	    throws WrongElementType
    {   
	nameList.insertElementAt(obj,index);
	((DataNameNode)nameList.elementAt(index)).setParent(this);

	int depth = getDepth();
	if( depth >= 0 )
	{
	    StarNode par;

	    for(    par = this ;
		    par != null && !(par instanceof DataLoopNode ) ;
		    par = par.getParent()   )
	    { /*void-body*/ }
	    if( par != null )
	    {   ((DataLoopNode)par).getVals().insertColumnAtDepth(
		    depth, index, val );
	    }
	}
    }

    /** Just like the Vector method of the same name.
      * Adds a name to the end of the list.
      * Also adds a default value into the loop in a column to match up
      * with the new name if the loop is there.  The default value is
      * a single nonquoted dot (.).
      * @param obj The name to add.
      * @see java.util.Vector.addElement
      */
    public  void addElement(Object obj)
	    throws WrongElementType
    {   
	insertElementAt( obj, size() );
    }

    /** Just like the Vector method of the same name.
      * Adds a name to the end of the list.
      * Also adds new DataValueNodes into the loop in a column to match up
      * with the new name where needed.  The default value is
      * the value passed in the parameter
      * @param obj The name to add.
      * @param val the new DataValueNode to copy from if need be.
      * @see java.util.Vector.addElement
      */
    public  void addElement( Object obj, DataValueNode val)
	    throws WrongElementType
    {   
	insertElementAt( obj, size(), val );
    }

    /** Just like the Vector method of the same name.
      * Removes the name matching the one given.
      * @param obj (string) The name to remove.
      * @see java.util.Vector.removeElement
      */
    public  void removeElement( Object obj)
    {
	int idx = indexOf( obj );
	removeElementAt( idx );
    }

    /** Get the depth of this name list in the loop it is in.
      * (The depth is the level of nesting.  If this is the outermost
      * list of names, it is at depth 'zero', if it is the next level in,
      * is is depth 1, and so on...)
      * <PRE>
      *     loop_
      *         _tag1    # --.
      *         _tag2    #   |-- depth 0.
      *         _tag3    # --'
      *         loop_
      *             _tagA    # --- depth 1.
      *             loop_
      *                 _tagX    # --.__ depth 2.
      *                 _tagY    # --'
      * </PRE>
      * @return depth - negative number if this is not inside a DataLoopNode.
      */
    public int getDepth()
    {
	StarNode par = getParent();
	if( par == null )
	    return -1;
	return ((DataLoopNameListNode)par).indexOf( this );
    }

    /** Returns the name of the first tag in the list, which is sometimes
      * used to refer to the whole loop list.
      */
    public String getLabel()
    {
	if( nameList.size() > 0 )
	    return ((DataNameNode)nameList.elementAt(0)).getLabel();
	else
	    return null;
    }

    /** Find the name given in this name list.
      * <P>
      * The search for names is case-insensitive.
      * @param searchFor look for this tag name.
      */
    public VectorCheckType  searchByName( String searchFor )
    {
	VectorCheckType retVal = new VectorCheckType();
        int             i;
	try
	{
	    retVal.addType( Class.forName( StarValidity.clsNameStarNode));
	    retVal.freezeTypes();
	    
	    for( i = 0 ; i < nameList.size() ; i++ )
		if(     ( (DataNameNode)(nameList.elementAt(i))
		        ).getLabel().equalsIgnoreCase(searchFor)   )
		    retVal.addElement( nameList.elementAt(i) );
	}
	catch( ClassNotFoundException exc )
	{   System.err.println( "Should never happen exception: " +
		    exc.getMessage() );
	    exc.printStackTrace();
	    return null;
	}
	return retVal;
    }

    /** Find the type given in this name list.
      * @param searchFor look for this tag name.
      */
    public VectorCheckType  searchForType( Class type, short delim )
    {
	VectorCheckType retVal = new VectorCheckType();
        int             i;

	try
	{
	    retVal.addType( Class.forName( StarValidity.clsNameStarNode));
	    retVal.freezeTypes();
	    
	    // Am I the right type?
	    if( type.isInstance(this) )
		retVal.addElement( this );
	    if( type == Class.forName( StarValidity.clsNameDataNameNode) )
		for( i = 0 ; i < nameList.size() ; i++ )
		    retVal.addElement( nameList.elementAt(i) );
	}
	catch( ClassNotFoundException exc )
	{   System.err.println( "Should never happen exception: " +
		    exc.getMessage() );
	    exc.printStackTrace();
	    return null;
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
	// TODO - do when I understand Java printing better.
    }

    TagsVector nameList;
}


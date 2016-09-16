package EDU.bmrb.starlibj;

import java.lang.*;
import java.util.*;


/** This class contains the list of lists of names that represents
  * all the tag names for a loop.  This class mimics the functionality
  * of java.util.vector, so that programmers can learn it easier.
  * Each element in this 'vector' is a LoopNameListNode (described
  * elsewhere), which is a list of tagnames.  Each element in this
  * 'vector' is therefore one of the the nesting levels of the loop's
  * names.  Here is an example.  Assume the original star file contained
  * the following piece of text:
  * <P>
  * <PRE>
  *            loop_
  *                _tag_I  _tag_II  _tag_III
  *                loop_
  *                    _tag_A  _tag_B
  *                    loop_
  *                        _tag1 _tag2 _tag3 _tag4
  *
  *           <I>... loop values ... </I>
  * </PRE>
  * Then the DataLoopNameListNode to store those tag names would
  * look like this:
  * <TABLE BORDER=YES>
  *     <TR>
  *         <TH>index</TH>  <TH>contains</TH>
  *     </TR>
  *     <TR>
  *         <TD>0</TD>      <TD>a LoopNameListNode which in turn contains
  *                             "_tag_I", "_tag_II", and "_tagIII"</TD>
  *     </TR>
  *     <TR>
  *         <TD>1</TD>      <TD>a LoopNameListNode which in turn contains
  *                             "_tag_A" and "_tag_B"</TD>
  *     </TR>
  *     <TR>
  *         <TD>2</TD>      <TD>a LoopNameListNode which in turn contains
  *                             "_tag_1", "_tag_2", "_tag3", and "_tag_4"</TD>
  *     </TR>
  * </TABLE>
  * @see LoopNameListNode
  */
public class DataLoopNameListNode extends StarNode implements Cloneable
{
    /** no-arg constructor */
    public DataLoopNameListNode()
    {
	super();
	myRows = new NameListVector();
    }
    /** copy constructor */
    public DataLoopNameListNode( DataLoopNameListNode copyMe )
    {
	super(copyMe);
	int i;
	myRows = new NameListVector();

	for( i = 0 ; i < copyMe.size() ; i++ )
	    addElement( (LoopNameListNode) ( copyMe.elementAt(i).clone() ) );
    }
    /** Copy a vector of LoopNameListNodes */
    public DataLoopNameListNode( NameListVector copyMe )
    {
	super();
	int i;
	myRows = new NameListVector();

	for( i = 0 ; i < copyMe.size() ; i++ )
	    addElement( (LoopNameListNode)
		            ( ( (LoopNameListNode)
			         copyMe.elementAt(i)).clone() )  );
    }

    public Object clone()
    {
	return new DataLoopNameListNode( this );
    }

    // -------------------------------------------
    //    Things designed to mimic Vector:
    // -------------------------------------------


    /** Just like the Vector method of the same name.
      * @see VectorCheckType.setSize
      */
    public  void setSize(int newSize)
    {
	myRows.setSize(newSize);
    }
    /** Just like the Vector method of the same name.
      * @see VectorCheckType.capacity
      */
    public int capacity()
    {
	return myRows.capacity();
    }
    /** Just like the Vector method of the same name.
      * @see VectorCheckType.size
      */
    public int size()
    {
	return myRows.size();
    }
    /** Just like the Vector method of the same name.
      * @see VectorCheckType.isEmpty
      */
    public boolean isEmpty()
    {
	return myRows.isEmpty();
    }
    /** Just like the Vector method of the same name.
      * @see VectorCheckType.Enumeration
      */
    public  Enumeration elements()
    {
	return myRows.elements();
    }
    /** Just like the Vector method of the same name.
      * @see VectorCheckType.contains
      */
    public boolean contains(LoopNameListNode row)
    {
	return myRows.contains(row);
    }
    /** Just like the Vector method of the same name.
      * @see VectorCheckType.indexOf
      */
    public int indexOf(LoopNameListNode row)
    {
	return myRows.indexOf(row);
    }
    /** Just like the Vector method of the same name.
      * @see VectorCheckType.indexOf
      */
    public  int indexOf(LoopNameListNode row,
                                       int index)
    {
	return myRows.indexOf(row,index);
    }
    /** Just like the Vector method of the same name.
      * @see VectorCheckType.lastIndexOf
      */
    public int lastIndexOf(LoopNameListNode row)
    {
	return myRows.lastIndexOf( row );
    }
    /** Just like the Vector method of the same name.
      * @see VectorCheckType.lastIndexOf
      */
    public  int lastIndexOf(LoopNameListNode row,
                                           int index)
    {
	return myRows.lastIndexOf( row, index );
    }
    /** Just like the Vector method of the same name.
      * @see VectorCheckType.elementAt
      */
    public  LoopNameListNode elementAt(int index)
    {
	return (LoopNameListNode) ( myRows.elementAt(index) );
    }
    /** Just like the Vector method of the same name.
      * @see VectorCheckType.firstElement
      */
    public  LoopNameListNode firstElement()
    {
	return (LoopNameListNode) ( myRows.firstElement() );
    }
    /** Just like the Vector method of the same name.
      * @see VectorCheckType.lastElement
      */
    public  LoopNameListNode lastElement()
    {
	return (LoopNameListNode) ( myRows.lastElement() );
    }
    /** Just like the Vector method of the same name.
      * @see VectorCheckType.setElementAt
      */
    public  void setElementAt(LoopNameListNode row,
                                             int index)
    {
	myRows.setElementAt(row,index);
	( (LoopNameListNode) (myRows.elementAt(index))
	).setParent(this);
    }
    /** Just like the Vector method of the same name.
      * @see VectorCheckType.removeElementAt
      */
    public  void removeElementAt(int index)
    {
	( (LoopNameListNode) (myRows.elementAt(index))
        ).setParent(null);
	myRows.removeElementAt(index);
    }
    /** Just like the Vector method of the same name.
      * @see VectorCheckType.insertElementAt
      */
    public  void insertElementAt(LoopNameListNode row,
                                                int index)
    {
	myRows.insertElementAt(row,index);
	( (LoopNameListNode) (myRows.elementAt(index))
	).setParent(this);
    }
    /** Just like the Vector method of the same name.
      * @see VectorCheckType.addElement
      */
    public  void addElement(LoopNameListNode row)
    {
	myRows.addElement(row);
	( (LoopNameListNode) (myRows.lastElement())
	).setParent(this);
    }
    /** Just like the Vector method of the same name.
      * @see VectorCheckType.removeElement
      */
    public  boolean removeElement( LoopNameListNode row)
    {
	row.setParent(null);
	return myRows.removeElement(row);
    }

    /** Returns the name of the first tag in the list, which is sometimes
      * used to refer to the whole loop list.
      * @return null if the list is empty (which should only happen
      * when the list is still in creation.)
      */
    public String getLabel()
    {
	if( myRows.size() > 0 )
	    return ( (LoopNameListNode) myRows.elementAt(0) ).getLabel();
	else
	    return null;
    }

    /** Find the name given in this name list.
      * The search for names is case-insensitive.
      *
      * @param searchFor look for this tag name, case insensitively.
      */
    public VectorCheckType  searchByName( String searchFor )
    {
	VectorCheckType retVal = new VectorCheckType();
	VectorCheckType tmpVect;

	try
	{
	    retVal.addType( Class.forName( StarValidity.clsNameStarNode ));
	    retVal.freezeTypes();
	    int             i,j;
	    
	    for( i = 0 ; i < myRows.size() ; i++ )
	    {
		tmpVect = ( (LoopNameListNode) myRows.elementAt(i)
			  ).searchByName(searchFor);
		for( j = 0 ; j < tmpVect.size() ; j++ )
		    retVal.addElement( tmpVect.elementAt(j));
	    }
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

	try
	{
	    retVal.addType( Class.forName( StarValidity.clsNameStarNode ));
	    retVal.freezeTypes();

	    if( type.isInstance(this) )
		retVal.addElement(this);
	    if( type == Class.forName( StarValidity.clsNameDataNameNode) )
	    {
		for( int i = 0 ; i < myRows.size() ; i++ )
		{   VectorCheckType tmpVect = 
		       ((LoopNameListNode)myRows.elementAt(i)).searchForType( type, delim );
		    for( int j = 0 ; j < tmpVect.size() ; j++ )
			retVal.addElement(tmpVect.elementAt(j));
		}
	    }
	}
	catch( ClassNotFoundException exc )
	{   System.err.println( "Should never happen exception: " +
		    exc.getMessage() );
	    exc.printStackTrace();
	    return null;
	}
	return retVal;
    }

    /** Get the index of the given name.  Returns the
      * nest depth and the column index within that nest depth.
      * (indexes start counting at zero, negative numbers returned
      * mean the tag was not found.)
      * <P>
      * Note that the search for tag names is always case-insensitive,
      * as per the STAR syntax.
      * <P>
      * @param tagName   The tag to look for.
      * @param nestLevel (out) - Returns the nesting level.  The use of the
      *        trivial "RemoteInt" class is required because Java can only
      *        pass an int by value, and the class "Integer" doesn't have any
      *        methods for setting the value after construction.
      * @param column (out) - Returns the nesting level.  The use of the
      *        trivial "RemoteInt" class is required because Java can only
      *        pass an int by value, and the class "Integer" doesn't have any
      *        methods for setting the value after construction.
      */
    public void tagPositionDeep( String tagName,
	                         RemoteInt nestLevel, RemoteInt column )
    {
	boolean foundIt = false;
	LoopNameListNode  curList;
	int     i;

	for( nestLevel.num = 0; nestLevel.num < myRows.size(); nestLevel.num++ )
	{
	    curList = (LoopNameListNode) myRows.elementAt(nestLevel.num);
	    column.num = -1;
	    for( i = 0 ; i < curList.size() ; i++ )
	    {
		if( curList.elementAt(i).getLabel().equalsIgnoreCase( tagName ) )
		{
		    column.num = i;
		    break;
		}
	    }
	    if( column.num >= 0 )
	    {
		foundIt = true;
		break;
	    }
	}
	if( ! foundIt )
	{   nestLevel.num = -1;
	    column.num = -1;
	}
	return;
    }

    NameListVector myRows;
}


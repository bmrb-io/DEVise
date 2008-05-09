package EDU.bmrb.starlibj;

import java.lang.*;
import java.util.*;


/** A LoopRowNode is a single tuple of values for a loop, like
  * a 'row' in a table.  It behaves like a VectorCheckType class,
  *implementing most of the same API as VectorCheckType.
  * @see VectorCheckType
  */
public class LoopRowNode extends StarNode implements Cloneable
{
    StringBuffer   myVal;
    int            myStarts[];
    LoopTableNode  myInnerLoop;
    int            mySize;

    /** Default constructor */
    public LoopRowNode()
    {
	super();
	myVal = new StringBuffer();
	myStarts = new int[0];
	mySize = 0;
    }
    /** copy constructor */
    public LoopRowNode( LoopRowNode copyMe )
    {
	super(copyMe);
	int i;

	myVal = new StringBuffer( copyMe.myVal.toString() ); 

	if( copyMe.myInnerLoop != null )
	    myInnerLoop = new LoopTableNode( copyMe.myInnerLoop );
	else
	    myInnerLoop = null;

	mySize = copyMe.mySize;
	myStarts = new int[ copyMe.myStarts.length ];
	System.arraycopy( copyMe.myStarts, 0, myStarts, 0, myStarts.length );
    }

    /** clone - make a deep copy of me and return a reference to it. */
    public Object clone()
    {
	return new LoopRowNode(this);
    }

    // These are here for performance tweaking:
    // ----------------------------------------

    /** Enlarge capacity of the stringbuffer so that it can
      * hold newCap characters.  This is not necessary to
      * call this method, but if you know roughly how many
      * total characters will be in a row of values ahead of time,
      * calling this method can make things run a bit faster.
      */
    public void ensureCapacity( int newCap )
    {
	if( myVal.capacity() < newCap )
	    myVal.ensureCapacity( newCap );
    }

    /** Return the max number of characters that this row can hold
      * before it needs to allocate a bigger chunk of heap memory.
      * This is not a hard limit - if it needs to it will expand this
      * number.  It is just here for help with performance tuning.
      * (Using this you can know when to manually expand the memory
      * when you wish instead of relying on the library to do it
      * when the string runs out of room.)
      * Example:
      * <PRE>
      *      if( numberOfCharactersIKnowINeed &gt; row.capacity() )
      *          row.ensureCapacity( numberOfCharactersIKnowINeed );
      * </PRE>
      */
    public int capacity()
    {
	return myVal.capacity();
    }

    // -------------------------------------------
    //    Things designed to mimic Vector:
    // -------------------------------------------


    // Not used with the new byte array techniques.
    // /** Just like the Vector method of the same name.
    //  * @see VectorCheckType.setSize
    //  */
    // public  void setSize(int newSize)
    // {
    // 	if( newSize > 
    // 	myVals.setSize(newSize);
    // }
    // Not used with the new byte array techniques.
    // /** Just like the Vector method of the same name.
    //   * @see VectorCheckType.capacity
    //   */
    // public int capacity()
    // {
    // 	return myVals.capacity();
    // }

    /** Just like the Vector method of the same name.
      * @see VectorCheckType.size
      */
    public int size()
    {
	return mySize;
    }
    /** Just like the Vector method of the same name.
      * @see VectorCheckType.isEmpty
      */
    public boolean isEmpty()
    {
	return  ( size() == 0 );
    }
    // Can't work with byte array technique:
    // /** Just like the Vector method of the same name.
    //   * @see VectorCheckType.Enumeration
    //   */
    // public  Enumeration elements()
    // {
    // 	return myVals.elements();
    // }

    // Can't be reconciled with the new non-DataValueNode
    // implementation.
    ///** Just like the Vector method of the same name.
    //  * @see VectorCheckType.contains
    //  */
    //public boolean contains(DataValueNode val)
    //{
    //	return myVals.contains(val);
    //}

    // Can't be reconciled with the new non-DataValueNode
    // implementation.
    ///** Just like the Vector method of the same name.
    //  * @see VectorCheckType.indexOf
    //  */
    //public int indexOf(DataValueNode val)
    //{
    //   return myVals.indexOf(val);
    //}

    // Can't be reconciled with the new non-DataValueNode
    // implementation.
    ///** Just like the Vector method of the same name.
    //  * @see VectorCheckType.indexOf
    //  */
    //public  int indexOf(DataValueNode val,
    //                                   int index)
    //{
    //	return myVals.indexOf(val,index);
    //}

    // Can't be reconciled with the new non-DataValueNode
    // implementation.
    // /** Just like the Vector method of the same name.
    //   * @see VectorCheckType.lastIndexOf
    //   */
    // public int lastIndexOf(DataValueNode row)
    // {
    //     return myVals.lastIndexOf( row );
    // }

    // Can't be reconciled with the new non-DataValueNode
    // implementation.
    ///** Just like the Vector method of the same name.
    //  * @see VectorCheckType.lastIndexOf
    //  */
    //public  int lastIndexOf(DataValueNode val,
    //                                       int index)
    //{
    //    return myVals.lastIndexOf(val,index);
    //}
    
    /** Just like the Vector method of the same name.
      * @see VectorCheckType.elementAt
      */
    public  DataValueNode elementAt(int index)
    {
	ParseValFromRetVal   parsed;
	DataValueNode        retVal;
	char                 delimChar;


	delimChar = myVal.charAt( myStarts[index] );
	retVal = new DataValueNode(
		    myVal.toString().substring( myStarts[index]+1,
				     ( index < mySize-1 ) ?
					 myStarts[index+1] : myVal.length() ),
		    ( delimChar == '\002' ) ?
			DataValueNode.SINGLE :
		    ( delimChar == '\003' ) ?
			DataValueNode.DOUBLE :
		    ( delimChar == '\004' ) ?
			DataValueNode.SEMICOLON :
		    ( delimChar == '\005' ) ?
			DataValueNode.FRAMECODE :
			DataValueNode.NON  // default or '\001'
		    );

	retVal.setParent( this );
	retVal.setLineNum( getLineNum() ); // not quite true.
	retVal.setColNum( getColNum() ); // not quite true.
	return retVal;
    }

    /** return the element at the position given, but return
     * the result as a simple string rather than as a DataValueNode.
     * Returns null if the element specified does not exist in the row.
     */
    public String stringAt( int i )
    {
	DataValueNode dvn;

	if( i >= 0 && i < size() )
	{
	    dvn = elementAt( i );
	    if( dvn != null )

		return dvn.getValue();
	    else
		return null;
	}
	else 
	    return null;

    }

    /** Just like the Vector method of the same name.
      * @see VectorCheckType.firstElement
      */
    public  DataValueNode firstElement()
    {
	return elementAt( 0 );
    }
    /** Just like the Vector method of the same name.
      * @see VectorCheckType.lastElement
      */
    public  DataValueNode lastElement()
    {
	return elementAt( mySize - 1 );
    }
    /** Just like the Vector method of the same name.
      * @see VectorCheckType.setElementAt
      */
    public  void setElementAt(DataValueNode val,
                                             int index)
    {
	ParseValFromRetVal   parsed;
	DataValueNode        retVal;
	StringBuffer         insertStr;
	int                  offset;
	int                  i;
	short                delim;

	insertStr = new StringBuffer();
	delim = val.getDelimType();
	if( delim == DataValueNode.SINGLE )
	{
	    insertStr.append( '\002' );
	    insertStr.append( val.getValue() );
	}
	else if( delim == DataValueNode.DOUBLE )
	{
	    insertStr.append( '\003' );
	    insertStr.append( val.getValue() );
	}
	else if( delim == DataValueNode.SEMICOLON )
	{
	    insertStr.append( '\004' );
	    insertStr.append( val.getValue() );
	}
	else if( delim == DataValueNode.FRAMECODE )
	{
	    insertStr.append( '\005' ); 
	    insertStr.append( val.getValue() );
	}
	else // NON or Dont-care
	{
	    insertStr.append( '\001' );
	    insertStr.append( val.getValue() );
	}

	// offset = difference between the new length and the old length:
	offset = insertStr.length() - (
		     (  ( index < mySize - 1 ) ?
			    myStarts[index+1] : myVal.length() )
		     - myStarts[index] ) ;

	StringBuffer  newMyVal = new StringBuffer();
	newMyVal.setLength(0);
	newMyVal.append( myVal.toString().substring( 0, myStarts[ index ] ) );
	newMyVal.append( insertStr );
	newMyVal.append(  ( index < mySize - 1 ) ?
		       myVal.toString().substring( myStarts[ index + 1 ] ) :
		       ""  );
	myVal = newMyVal;
	newMyVal = null;

	// Shift the remaining starting indeces forward to match the
	// change in length of this value: (shifts backward if offset
	// is calculated as a negative number, meaning the new string
	// is shorter than the old.
	for( i = index + 1 ; i < mySize ; i++ )
	{
	    myStarts[i] = myStarts[i] + offset;
	}
    }

    /** Similar to the Vector method of the same name, however
      * It will refuse to work if this row is already inside a
      * DataLoopNode, and it will generate an exception.
      * @see VectorCheckType.removeElementAt
      */
    public  void removeElementAt(int index)
	throws OperationCausesMismatchedLoopData
    {
	if( isInLoop() > 0)
	    throw new OperationCausesMismatchedLoopData();

	privateRemoveElementAt( index );
    }
    // Just like above, but it inserts elements without checking
    // for mismatched loop data.
    private  void privateRemoveElementAt(int index)
	throws OperationCausesMismatchedLoopData
    {
	int charsLost;
	int i;

	// charsLost = difference between the new length and the old length:
	charsLost = (
		    ( index < mySize - 1 ) ?
		        myStarts[index + 1] : myVal.length()
		 ) - myStarts[index];
	StringBuffer newMyVal = new StringBuffer();
	newMyVal.setLength(0);
	newMyVal.append( myVal.toString().substring( 0, myStarts[index] ) );
	newMyVal.append( ( index < mySize - 1 ) ?
			   myVal.toString().substring( myStarts[index+1] ) :
			   ""   );
	myVal = newMyVal;
	newMyVal = null;

	// Shift the remaining starting indeces backward to match the change:
	for( i = index ; i < mySize - 1 ; i++ )
	{
	    myStarts[i] = myStarts[i+1] - charsLost;
	}
	mySize--;
    }
    /** Just like the Vector method of the same name,
      * but It will refuse to work if this row is already inside
      * a DataLoopNode, and it will generate an exception.
      * @see VectorCheckType.insertElementAt
      */
    public  void insertElementAt(DataValueNode val,
                                                int index)
	throws OperationCausesMismatchedLoopData
    {
	if( isInLoop() > 0)
	    throw new OperationCausesMismatchedLoopData();

	privateInsertElementAt( val, index );
	// ((DataValueNode)myVals.elementAt(index)).setParent(this);
    }
    // Just like above, but it inserts elements without checking
    // for mismatched loop data.
    private  void privateInsertElementAt(DataValueNode val,
                                                int index)
    {
	ParseValFromRetVal   parsed;
	DataValueNode        retVal;
	StringBuffer         insertStr;
	int                  offset;
	int                  i;
	short                delim;

	insertStr = new StringBuffer();
	delim = val.getDelimType();
	if( delim == DataValueNode.SINGLE )
	{
	    insertStr.append( '\002' );
	    insertStr.append( val.getValue() );
	}
	else if( delim == DataValueNode.DOUBLE )
	{
	    insertStr.append( '\003' );
	    insertStr.append( val.getValue() );
	}
	else if( delim == DataValueNode.SEMICOLON )
	{
	    insertStr.append( '\004' );
	    insertStr.append( val.getValue() );
	}
	else if( delim == DataValueNode.FRAMECODE )
	{
	    insertStr.append( '\005' );
	    insertStr.append( val.getValue() );
	}
	else // NON or Dont-care
	{
	    insertStr.append ('\001');
	    insertStr.append( val.getValue() );
	}

	// offset = difference between the new length and the old length:
	offset = insertStr.length();
	
	int oldSize = myVal.length();

	// Starting at the right size to begin with will greatly speed
	// up the appending.
	myVal.insert( (index < mySize) ? myStarts[index] : myVal.length(),
		       insertStr );

	// Shift the remaining starting indeces forward to match the
	// change in length of this value: (shifts backward if offset
	// is calculated as a negative number, meaning the new string
	// is shorter than the old.)
	int tmp[] = new int[ mySize+1 ];
	// special case: starting empty:
	if( mySize == 0 )
	    tmp[0] = 0;
	else
	{
	    for( i = 0 ; i <= index ; i++ )
	    {
		tmp[i] = ( i < mySize ) ? myStarts[i] : oldSize;
	    }
	    for( i = index + 1 ; i < tmp.length ; i++ )
	    {
		tmp[i] = myStarts[i-1] + offset;
	    }
	}
	myStarts = tmp;
	tmp = null;
	mySize++;

    }
    /** Just like the Vector method of the same name,
      * but It will refuse to work if this row is already inside
      * a DataLoopNode, and it will generate an exception.
      * @see VectorCheckType.addElement
      */
    public  void addElement(DataValueNode val)
	throws OperationCausesMismatchedLoopData
    {
	privateInsertElementAt( val, mySize );
    }

    // Disabled for the new DVN-less setup:
    ///** Just like the Vector method of the same name,
    //  * but It will refuse to work if this row is already inside
    //  * a DataLoopNode, and it will generate an exception.
    //  * @see VectorCheckType.removeElement
    //  */
    //public  boolean removeElement( DataValueNode val)
    //	throws OperationCausesMismatchedLoopData
    //{
    //	if( isInLoop() > 0 )
    //	    throw new OperationCausesMismatchedLoopData();

	// val.setParent(null);
    //	return myVals.removeElement(val);
    //}

    // insertColumnAtDepth: Insert a column at the specified depth.  A depth
    // of zero means this row itself, a larger depth means to go down a level
    // and do it in the inner loop if there is one.
    // The given DataValueNode is the value to insert.
    // This method is only usable by the other methods in this package,
    // because it causes mismatched loop data with the names above if
    // it is called otherwise.
    void insertColumnAtDepth( int depth, int columnIdx, DataValueNode val )
    {
	if( depth > 0 ) // recursive case:
        {
	    if( getInnerLoop() != null )
		getInnerLoop().insertColumnAtDepth( depth - 1, columnIdx, val );
	}
	else // base case:
	{
	    // myVals.insertElementAt( new DataValueNode(val), columnIdx );
	    privateInsertElementAt( new DataValueNode(val), columnIdx );
	}
    }

    // This method deletes the specified column at the specified depth.
    // Only to be called from within the library itself, else its
    // dangerous.
    // If depth == 0 , then it refers to this row, else it needs to
    // be recursively passed to a deeper level (inner loop).
    void removeColumnAtDepth( int depth, int columnIdx )
    {
	if( depth > 0 ) // recursive case:
	{
	    if( getInnerLoop() != null )
		getInnerLoop().removeColumnAtDepth( depth - 1, columnIdx );
	}
	else // base case:
	{
	    // myVals.removeElementAt( columnIdx );
	    privateRemoveElementAt( columnIdx );
	}
    }

    /** isInLoop: Returns &gt;zero if this LoopRowNode is inside a DataLoopNode,
      * or zero if it is not.  (In a complete STAR tree it is always &gt;0,
      * but when building up a loop piece by piece an LoopRowNode might not
      * have been attached to a loop yet.)
      * <p>
      * The value returned is an integer.  It is a count of how many nesting
      * levels deep this is in the loop that it was found it.  Thus if
      * it is zero (false) it was not found in a loop, and if it is 1 then
      * it was found in nesting level 1 of a loop, 2 = nesting level 2, etc.
      * <p>
      * Note that this is off-by-one with the index used in the [] operator,
      * which starts counting at zero.  This was done so that this method
      * could be used like a boolean. (zero means not found, nonzero means
      * found).
      */
    public int isInLoop()
    {
	StarNode  par;
	int retVal;

	par = this;
	retVal = 0;
	for(  par = this , retVal = 0 ;
	      ( par != null && !( par instanceof DataLoopNode ) ) ;
	      par = par.getParent()  )
	{
	    if( par instanceof LoopTableNode )
		retVal++;
	}
	if( par == null )
	    return 0;
	else
	    return retVal;
    }

    /**  Returns a reference to the inner loop that is under this
      *  row.  This will return nil if there is no inner loop under
      *  this row.
      *  @return null if no inner loop.
      */
    public LoopTableNode    getInnerLoop()
    {
	return myInnerLoop;
    }

    /**  Allows the user to set the inner loop under this row.
      *  (TODO: need to throw exceptions here.  Not implemented
      *  yet, just a placeholder.)
      *  @exception OperationCausesMismatchedLoopData when the new row
      *  does not match the tagnames for this loop.  If this row is not
      *  in a loop yet, then this check is not performed, obviously.
      */
    public void setInnerLoop( LoopTableNode l )
	throws OperationCausesMismatchedLoopData
    {
	myInnerLoop = l; 
	l.setParent(this);
	if( false ) // TODO - need real checks here.
	    throw new OperationCausesMismatchedLoopData();
    }

    /**  De-links the inner loop from this row.
      */
    public void removeInnerLoop()
    {
	myInnerLoop = null;
    }

    /** Find all values of the type given in this table - this is fairly
      * nonsensical because all values in the table are of type
      * DataValueNode, but this function is here for orthogonality
      * with the base class StarNode.
      * @param type look for this type.
      */
    public VectorCheckType  searchForType( Class type )
    {
	VectorCheckType retVal = new VectorCheckType();
	VectorCheckType tmpVect;
	int             i,j;

	try
	{
	    retVal.addType( Class.forName( StarValidity.clsNameStarNode) );
	    retVal.freezeTypes();

	    if( type.isInstance(this) )
		retVal.addElement(this);
	    for( i = 0 ; i < size() ; i++ )
	    {
		tmpVect = elementAt(i).searchForType(
			type, DataValueNode.DONT_CARE);
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

    /** Find the type given in this row - Return all the matching
      * values that meet the criteria given in the parameters.
      * @param type look for this type
      * @param delim Look for this delimiter type.
      * @see DataValueNode::NON
      */
    public VectorCheckType  searchForType( Class type, short delim )
    {
	VectorCheckType retVal = new VectorCheckType();
	VectorCheckType tmpVect;
	int             i,j;

	try
	{
	    retVal.addType( Class.forName( StarValidity.clsNameStarNode) );
	    retVal.freezeTypes();

	    if( type.isInstance(this) )
		retVal.addElement(this);
	    for( i = 0 ; i < size() ; i++ )
	    {
		tmpVect = elementAt(i).searchForType( type, delim );
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

    /** Unparse prints the contents of the StarNode object out to the
      * given stream.  This is essentially the inverse of the CS term
      * to "parse", hence the name "Unparse".  The parameter given is
      * the indentation level to print things.
      */
    public void Unparse( int indent )
    {
	// TODO - when I know more about Java printing.
    }

}

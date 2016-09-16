package EDU.bmrb.starlibj;

import java.lang.*;
import java.util.*;

/** A LoopTableNode is a 'table' of values in a loop.  It is a vector
  * of LoopRowNodes.  Every DataLoopNode has at least one
  * LoopTableNode for the outermost loop.  Each nested subtable is
  * represented by another LoopTableNode.
  * <P>
  * It is possible for a LoopRowNode to contain a LoopTableNode under it.
  * This is how nested loops are linked together.  At the outermost level,
  * there is a LoopTableNode containing LoopRowNodes.  Each of these
  * LoopRowNodes could have a LoopTableNode under it, which in turn is
  * a collection of LoopRowNodes which could have LoopTableNodes under
  * them, and so on.
  */
public class LoopTableNode extends StarNode implements Cloneable
{
    /** default constructor - makes a loop with indent, linear. */
    public LoopTableNode()
    {
	super();
	myRows = new LoopRowsVector();
	myTabFlag = false;
	myIndentFlag = true;
	myRowsPerLine = 1;
    }
    /** @param tabFlag set to true for a tabularly printed loop, or false
      * for a linearly printed loop.  This only affects output with
      * Unparse() and nothing else.
      */
    public LoopTableNode( boolean tabFlag )
    {
	super();
	myRows = new LoopRowsVector();
	myTabFlag = tabFlag;
	myIndentFlag = true;
	myRowsPerLine = 1;
    }
    /** @param tabFlag set to true for a tabularly printed loop, or false
      * for a linearly printed loop.  This only affects output with
      * Unparse() and nothing else.
      * @param indentFl set to true if this loop should be output indented.
      * @param rowsPerLn set to the number of rows to include on one
      * line of text.  Only has meaning if tabFlag is true.
      */
    public LoopTableNode( boolean tabFlag, boolean indentFl, int rowsPerLn )
    {
	super();
	myRows = new LoopRowsVector();
	myTabFlag = tabFlag;
	myIndentFlag = indentFl;
	myRowsPerLine = rowsPerLn;
    }

    /** copy constructor  - deep copy. */
    public LoopTableNode( LoopTableNode copyMe )
    {
	super();
	int i;
	myRows = new LoopRowsVector();

	myTabFlag = copyMe.myTabFlag;
	myIndentFlag = copyMe.myIndentFlag;
	myRowsPerLine = copyMe.myRowsPerLine;

	for( i = 0 ; i < copyMe.size() ; i++ )
	    addElement( (LoopRowNode) copyMe.elementAt(i).clone() );
    }

    /** clone - make a deep copy of me and return a reference to it. */
    public Object clone()
    {
	return new LoopTableNode(this);
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
    public boolean contains(LoopRowNode row)
    {
	return myRows.contains(row);
    }
    /** Just like the Vector method of the same name.
      * @see VectorCheckType.indexOf
      */
    public int indexOf(LoopRowNode val)
    {
	return myRows.indexOf(val);
    }
    /** Just like the Vector method of the same name.
      * @see VectorCheckType.indexOf
      */
    public  int indexOf(LoopRowNode row,
                                       int index)
    {
	return myRows.indexOf(row,index);
    }
    /** Just like the Vector method of the same name.
      * @see VectorCheckType.lastIndexOf
      */
    public int lastIndexOf(LoopRowNode row)
    {
	return myRows.lastIndexOf(row);
    }
    /** Just like the Vector method of the same name.
      * @see VectorCheckType.lastIndexOf
      */
    public  int lastIndexOf(LoopRowNode row,
                                           int index)
    {
	return myRows.lastIndexOf(row,index);
    }
    /** Just like the Vector method of the same name.
      * @see VectorCheckType.elementAt
      */
    public  LoopRowNode elementAt( int index )
    {
	return (LoopRowNode) myRows.elementAt(index);
    }
    /** Gets the value at a specific row and column in the
     * table.  This routine counts starting at zero, in
     * row-major order.  If the specific row or column don't
     * exist, then a null is returned.
     */
    public  DataValueNode elementAt( int row, int col )
    {
	DataValueNode dvn = null;
	LoopRowNode lrn = null;

	if( row >= 0 && row < myRows.size() )
	    lrn = elementAt( row );
	if( lrn != null )
	{
	}
	return dvn;
    }

    /** This routine obtains the value at the specified row
     * and column as a string rather than as a DataValueNode
     * A null is returned if no such row or column exists.
     */
    public  String  stringAt( int row, int col )
    {
        DataValueNode dvn;

	dvn = elementAt( row, col );
	if( dvn != null )
	    return dvn.getValue();
	else
	    return null;
    }

    /** Just like the Vector method of the same name.
      * @see VectorCheckType.firstElement
      */
    public  LoopRowNode firstElement()
    {
	return (LoopRowNode) myRows.firstElement();
    }
    /** Just like the Vector method of the same name.
      * @see VectorCheckType.lastElement
      */
    public  LoopRowNode lastElement()
    {
	return (LoopRowNode) myRows.lastElement();
    }
    /** Just like the Vector method of the same name, except
      * that it throws an exception if the number of values in
      * the row is incorrect for the number of names in the list
      * of names in this DataLoopNode.  (If this table is not
      * inside a DataLoopNode, then this exception is never thrown.
      * @see VectorCheckType.setElementAt
      */
    public  void setElementAt(LoopRowNode row,
                                             int index)
	throws OperationCausesMismatchedLoopData
    {
        int cols = privateNumColsAtMyDepth() ;
	if( cols >= 0 && cols != row.size() )
	{
	   throw new OperationCausesMismatchedLoopData();
	}
	myRows.setElementAt(row,index);
	((LoopRowNode)myRows.elementAt(index)).setParent(this);
    }
    /** Similar to the Vector method of the same name.
      * If this is the last row in te table, and if this
      * table is nested inside a LoopRowNode, then it will
      * unlink itself from the parent LoopRowNode (which should
      * make this table go away at garbage collection time.)
      * @see VectorCheckType.removeElementAt
      */
    public  void removeElementAt(int index)
    {
	((LoopRowNode)myRows.elementAt(index)).setParent(null);
	myRows.removeElementAt(index);
	if( size() == 0 )
	{
	    StarNode par = getParent();
	    if( par != null && par instanceof LoopRowNode )
		( (LoopRowNode)par ).removeInnerLoop();
	}
    }
    /** Just like the Vector method of the same name,
      * but it will throw an exception when invalid insertions
      * are attempted.
      * <P>
      * An invalid insertion is one with the following conditions:
      *  1 - This table is already inside a DataLoopNode (and not
      * 'free floating'.), <B>and</B> <BR>
      *  2 - The row being inserted has the wrong number of values.
      *  (The number of values in the row must match the number of names
      *  in the loop header.)
      * @see VectorCheckType.insertElementAt
      */
    public  void insertElementAt(LoopRowNode row,
                                                int index)
    {
	myRows.insertElementAt(row,index);
	((LoopRowNode)myRows.elementAt(index)).setParent(this);
	// TODO - exception check mentioned in the comment above.
    }


    protected int privateNumColsAtMyDepth()
    {
        int depth = 0;
	StarNode par = null;

	try 
	{
	    for( par = this ;
		 par != null && ! Class.forName(StarValidity.clsNameDataLoopNode).isInstance(par) ;
		 par = par.getParent() )
	    {
		if( Class.forName(StarValidity.clsNameLoopRowNode).isInstance(par) )
		   depth++;
	    }
	}
	catch( ClassNotFoundException exc )
	{   System.err.println( "Should never happen exception: " +
		                exc.getMessage() );
	    exc.printStackTrace();
	}
	

	if( par == null )
	  return -1;
	else
	  return ((DataLoopNode)par).getNames().elementAt( depth ).size();
    }


    /** Just like the Vector method of the same name,
      * but it will throw an exception when invalid insertions
      * are attempted.
      * <P>
      * An invalid insertion is one with the following conditions:
      *  1 - This table is already inside a DataLoopNode (and not
      * 'free floating'.), <B>and</B> <BR>
      *  2 - The row being inserted has the wrong number of values.
      *  (The number of values in the row must match the number of names
      *  in the loop header.)
      * @see VectorCheckType.addElement
      */
    public  void addElement(LoopRowNode row)
        throws OperationCausesMismatchedLoopData
    {
        int cols = privateNumColsAtMyDepth() ;
	if( cols >= 0 && cols != row.size() )
	{
	   throw new OperationCausesMismatchedLoopData();
	}
	myRows.addElement(row);
	((LoopRowNode)myRows.lastElement()).setParent(this);
    }
    /** Just like the Vector method of the same name, but
      * it makes this table go away if the row removed was the
      * last one.
      * <P>
      * If this is the last row in the table, and if this
      * table is nested inside a LoopRowNode, then it will
      * unlink itself from the parent LoopRowNode (which should
      * make this table go away at garbage collection time.)
      * @see VectorCheckType.removeElement
      */
    public  boolean removeElement( LoopRowNode row)
    {
	boolean retVal;

	row.setParent(null);
	retVal = myRows.removeElement(row);
	if( size() == 0 )
	{
	    StarNode par = getParent();
	    if( par != null && par instanceof LoopRowNode )
		((LoopRowNode)par).removeInnerLoop();
	}
	return retVal;
    }

    // removeColumnAtDepth: remove a column at the specified depth.  A depth
    // of zero means this row itself, a larger depth means to go down a level
    // and do it in the inner loop if there is one.
    // This method is only usable by the other methods in this package,
    // because it causes mismatched loop data with the names above if
    // it is called otherwise.
    void removeColumnAtDepth( int depth, int columnIdx )
    {
	int i;
	for( i = 0 ; i < myRows.size() ; i++ )
	{
	    ( (LoopRowNode)(myRows.elementAt(i)) ).removeColumnAtDepth(
	        depth, columnIdx );
	}
    }

    // insertColumnAtDepth: Insert a column at the specified depth.  A depth
    // of zero means this row itself, a larger depth means to go down a level
    // and do it in the inner loop if there is one.
    // The given DataValueNode is the value to insert.
    // This method is only usable by the other methods in this package,
    // because it causes mismatched loop data with the names above if
    // it is called otherwise.
    void insertColumnAtDepth( int depth, int columnIdx, DataValueNode val )
    {
	int i;
	for( i = 0 ; i < myRows.size() ; i++ )
	{
	    ( (LoopRowNode)(myRows.elementAt(i)) ).insertColumnAtDepth(
	        depth, columnIdx, val );
	}
    }


    /** isInLoop: Returns true if this LoopTableNode is inside a DataLoopNode,
      * or false if it is not.  (In a complete STAR tree it is always true,
      * but when building up a loop piece by piece an LoopTableNode might not
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
    
    /** Unparse prints the contents of the StarNode object out to the
      * given stream.  This is essentially the inverse of the CS term
      * to "parse", hence the name "Unparse".  The parameter given is
      * the indentation level to print things.
      */
    public void Unparse( int indent )
    {
	// TODO - do better when I understand the java library better.
    }

    /** Sets the tabulation Unparse flag for this table.
      * @param tabFlag true for tabular, false for linear.
      */
    public void setTabFlag( boolean tabFlag )
    {
	int i;

	// Set it for me:
	myTabFlag = tabFlag;

	// Set it for my children if there are any
	// inner loops:
	for( i = 0 ; i < size() ; i++ )
	{   LoopTableNode iLoop = elementAt(i).getInnerLoop();
	    if( iLoop != null )
		iLoop.setTabFlag( tabFlag );
	}
    }
    /** Gets the tabulation Unparse flag for this table.
      * @return true = tabular, false = linear
      */
    public boolean getTabFlag()
    {
	return myTabFlag;
    }
    /** Sets the indentation Unparse flag for this table.
      * @param tabFlag true for indented from margin (norma),
      *        false for up-against-margin.
      */
    public void setIndentFlag( boolean tabFlag )
    {
	myIndentFlag = tabFlag;
    }
    /** Gets the indentation Unparse flag for this table.
      * @return true for indented from margin (norma),
      *        false for up-against-margin.
      */
    public boolean getIndentFlag()
    {
	return myIndentFlag;
    }

    /** Sets the number of rows Unparsed per line when in tabular mode.
      * if <TT>getIndentFlag()</TT> is false, then this is
      * meaningless.
      * @param setTo the number of rows per line.
      */
    public void setRowsPerLine( int setTo )
    {
	myRowsPerLine = setTo;
    }
    /** Gets the number of rows to be Unparsed on one line when in
      * tabular mode.
      * @return the number of rows per line.
      */
    public int getRowsPerLine()
    {
	return myRowsPerLine;
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
	    for( i = 0 ; i < myRows.size() ; i++ )
	    {
		tmpVect = ( (LoopRowNode)myRows.elementAt(i)
			  ).searchForType(type);
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

	    if( type == getClass() )
		retVal.addElement(this);
	    for( i = 0 ; i < myRows.size() ; i++ )
	    {
		tmpVect = ( (LoopRowNode)myRows.elementAt(i)
			  ) .searchForType(type,delim);
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

    protected boolean         myTabFlag;
    protected LoopRowsVector  myRows;
    protected boolean         myIndentFlag;
    protected int             myRowsPerLine;

    //  Get the depth of nesting of this loop table.
    //  -1 = this loop table is not inside a DataLoopNode at this time.
    //   0 = this loop table is the outermost level for the loop.
    //   1 = this loop table is the next level in of nesting for the loop.
    //   2 = this loop table is the next level in of nesting.
    //   ...etc...
    protected int getMyDepth()
    {
	StarNode  par;
	int      retVal;

	for(    par = this , retVal = -1 ;
		par != null && !( par instanceof DataLoopNode ) ;
		par = par.getParent()  )
	{
	    if( par instanceof LoopTableNode )
		retVal++;
	}
	if( par == null )
	    return -1;
	else
	    return retVal;
    }

    //  Get the deepest depth of nesting of the entire loop.
    //  -1 = this is not in a loop, so the question is meaningless.
    //   0 = only outermost level of nesting exists.
    //   1 = two levels of nesting
    //   2 = three levels of nesting
    //   ...etc...
    protected int getMaxDepth()
    {
	
	StarNode  par;
	int      retVal;

	for(    par = this , retVal = -1 ;
		par != null && !( par instanceof DataLoopNode ) ;
		par = par.getParent()  )
	{ /* Do nothing body*/ }
	if( par == null )
	    return -1;
	else
	{
	    return ((DataLoopNode)par).getNames().size() - 1;
	}
    }

    //  Truncate the loop to get rid of all nesting levels at and under
    //  the level passed (outermost level is counted as zero.)
    protected void  truncateNestLevel( int depth )
    {
	// Recursive algorithm:
	if( depth == 0 )
	{
	    // Base case: Clear myself out.
	    while( myRows.size() > 0 )
		myRows.removeElementAt(0);
	}
	else
	{
	    // Recursive case - go down a level:
	    int i;
	    for( i = 0 ; i < myRows.size() ; i++ )
		if( ((LoopRowNode)myRows.elementAt(i)).getInnerLoop() != null )
		{
		    ((LoopRowNode)myRows.elementAt(i)) .
			getInnerLoop() .
			    truncateNestLevel(depth-1);
		    if( depth == 1 )
			((LoopRowNode)myRows.elementAt(i)) .
			    removeInnerLoop();
		}
	}
    }

    /** Look for the value in the nest level/column given.
      * nest level and column both start counting at zero.
      */
    protected VectorCheckType searchForValsInColumn( int     searchNest,
	                                             int     searchCol,
						     String  value )
    {
	int             i,j;
	VectorCheckType retVal = new VectorCheckType();
	VectorCheckType tmpVal;

	try
	{   retVal.addType( Class.forName( StarValidity.clsNameStarNode) );
	    retVal.freezeTypes();

	    for( i = 0 ; i < size() ; i++ )
	    {
		// This method is recusrive:
		// Base Case: I am the depth we are looking for, so look at
		// the column for the values:
		if( searchNest == 0 )
		{   if( elementAt(i).elementAt(searchCol).getValue().equals(value) )
			retVal.addElement( elementAt(i).elementAt(searchCol) );
		}
		// Recursive case: Not deep enough yet.  If this row has an
		// inner loop, then call its method with one less depth.
		else
		{
		    if( elementAt(i).getInnerLoop() != null )
		    {
			tmpVal = elementAt(i).
			            getInnerLoop().
				        searchForValsInColumn(
					    searchNest - 1, searchCol, value );

			for( j = 0 ; j < tmpVal.size() ; j++ )
			    retVal.addElement( tmpVal.elementAt(j) );
			tmpVal = null; 
		    }
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

    /** Calcualte the sizes to print this table in.
      * @param preSizes = list of pre-sizes (before decimal) for the columns
      * @param postSizes = list of post-sizes (after decimal) for the columns
      * @param nonQuotedFlags = list of booleans, true if the column has only
      *          nonquoted strings in it and can therefore be printed with less
      *          leading and trailing whitespace.
      */
    protected void calcPrintSizes(
	    Vector preSizes,
	    Vector postSizes,
	    Vector nonQuotedFlags
	    )
    {

	LoopRowNode    curRow;
	int       numCols = 0;
	int       curCol = 0;
	String    curStr;
	int       curPre;
	int       curPost;
	int       curIdx;
	int       dotIdx;
	int       outer_idx;
	int       inner_idx;
	String    tmpStr;

	if( myRows.size() > 0 )
	    numCols = elementAt(0).size();
	if( numCols == 0 )
	    return;  // give up - nothing here;

	// Make the starting stub column info for each column:
	for( curCol = 0 ; curCol < numCols ; curCol++ )
	{
	    preSizes.addElement( new Integer(0) );
	    postSizes.addElement( new Integer(-1) );
	    nonQuotedFlags.addElement( new Boolean(true) );
	}

	// For each row in the table:
	for( outer_idx = 0 ; outer_idx < myRows.size() ; outer_idx++ )
	{
	    curRow = elementAt(outer_idx);

	    // For each value in this row:
	    for( inner_idx = 0 ; inner_idx < curRow.size() ; inner_idx++ )
	    {
		curStr  = curRow.elementAt(inner_idx).getValue();
		curPre  = curRow.elementAt(inner_idx).myLongestStr();

		// Not a nondelim, so turn off the non flag for this column.
		if( curRow.elementAt(inner_idx).getDelimType() !=
			DataValueNode.NON )
		{   nonQuotedFlags.setElementAt(new Boolean(false), inner_idx);
		}
		curPost = -1;
		// The numeric check:  Is it even eligable to be a number?
		if( curRow.elementAt(inner_idx).getDelimType() ==
			DataValueNode.NON )
		{

		    tmpStr = new String ( curStr );
		    int sLen = tmpStr.length();
		    // Two things in one - checking for valid syntax and
		    // finding the decimal point.
		    // -------------------------------------------------
	    
		    curIdx = 0;
		    dotIdx = -1;
		    // Consume leading whitespace:
		    while(  curIdx < sLen &&
			    Character.isWhitespace( tmpStr.charAt(curIdx) ) )
			curIdx++;
		    // Consume optional pos/neg sign:
		    if  (  curIdx < sLen &&
			   (  tmpStr.charAt(curIdx) == '-' ||
			      tmpStr.charAt(curIdx) == '+'    )
			)
			curIdx++;
		    // Now a string of digits ended by a dot or whitespace or
		    // the end of the string.  Anything else is an indicator
		    // that this is not a true numeric value:
		    while(  curIdx < sLen &&
			    Character.isDigit( tmpStr.charAt(curIdx) ) )
			curIdx++;
		    curPre = curIdx;
		    if( curIdx < sLen && tmpStr.charAt(curIdx) == '.' )
		    {   dotIdx = curIdx;
			curIdx++;
		    }
		    curPost = 0;
		    while(  curIdx < sLen &&
			    Character.isDigit( tmpStr.charAt(curIdx) ) )
		    {
			curPost++; // count digits after the dot.
			curIdx++;
		    }
		    while(  curIdx < sLen &&
			    Character.isWhitespace( tmpStr.charAt(curIdx) ) )
			curIdx++; // trailing whitespace

		    // Okay, so the check is: if it stopped because it hit the
		    // end of the string - the string was fully numeric, else
		    // it had other garbage in it.  If the string was nothing
		    // but a dot, then calc the size as if it were non-numeric.
		    // (This has the effect that if the only 'numerics' in a
		    // column are really dots, then the dots get printed left-
		    // justified, but if any 'actual' numerics exist, then
		    // the dots get printed decimal-point justified.)
		    if( curIdx < sLen || ( tmpStr.equals(".")  )  )
		    {
			// Not numeric after all, so reset to the values
			// for text strings:
			curPost = -1;
			curPre = curRow.elementAt(inner_idx).myLongestStr();
		    }
		}
		else if( curRow.elementAt(inner_idx).getDelimType() == 
			    DataValueNode.SEMICOLON )
		{
		    curPre = 0; // If this is a semicolon delimited string, size doesn't matter.
		}

		if( ( (Integer)(preSizes.elementAt(inner_idx)) ).intValue()
			< curPre  )
		{   preSizes.setElementAt( new Integer(curPre), inner_idx );
		}
		if( ( (Integer)(postSizes.elementAt(inner_idx)) ).intValue()
			< curPost  )
		{   postSizes.setElementAt( new Integer(curPost), inner_idx );
		}
		
	    }
	}
    }
}


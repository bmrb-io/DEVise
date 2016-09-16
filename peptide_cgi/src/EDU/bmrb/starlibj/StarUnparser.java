package EDU.bmrb.starlibj;

import java.lang.*;
import java.util.*;
import java.io.*;
import EDU.bmrb.starlibj.*;

/** The inverse of class StarParse:  This class will print
  * out a star file (or subset thereof) into a previously
  * opened output stream.  Many of the output options can be
  * manipulated with get... and set... functions.
  * <P>
  * This class is thread-safe if and only if each thread that
  * uses it has its own separate instances of this class.  (There
  * are no static entities that step on each other in this class,
  * so each instance of it is independant, but there are some
  * dynamic entities that do get in each other's way if multiple
  * threads try to run methods in the same object.)
  */
public class StarUnparser implements Cloneable
{

    protected int              indentSize;
    protected int              extraColumnSpaces;
    protected boolean          formatting;
    protected PrintWriter      oWrit;
    protected SkipTextHandler  skips;
    protected int              prevCheckLineNum;
    protected boolean          suppressStops;

    /** Constructor: Pass in an already-opened output stream
      * and the output will end up there - buffered.  Output
      * will appear without flushing each line.
      * @param out The output stream to print on.
      */
    public StarUnparser( OutputStream out )
    {
	oWrit = new PrintWriter( out );
	indentSize = 4;
	formatting = true;
	suppressStops = false;
	extraColumnSpaces = 2;
    }

    /** Constructor: Pass in an already-opened output stream
      * and the output will end up there - and choose the
      * flushing flag.
      * @param out The output stream to print on.
      * @param autoFlush true = flush each line, false = don't
      *        flush output on each line.  Setting it false
      *        is the default.
      */
    public StarUnparser( OutputStream out, boolean autoFlush )
    {
	oWrit = new PrintWriter( out, autoFlush );
	indentSize = 4;
	formatting = true;
	suppressStops = false;
	extraColumnSpaces = 2;
    }

    /** Flush the output one last time before ending.
      * DOES NOT CLOSE THE STREAM, since this class
      * did not create the stream.
      */
    protected void finalize() 
    {
	// super.finalize();
	oWrit.flush();
    }

    /** copy constructor: copies an existing StarUnparser
      * object:
      */
    public StarUnparser( StarUnparser copyMe )
    {
	indentSize = copyMe.indentSize;
	formatting = copyMe.formatting;
	oWrit      = copyMe.oWrit;
	extraColumnSpaces = copyMe.extraColumnSpaces;
    }

    /** clone - needed so that this type can be stored inside
      * the container classes in java.util.*.  It mimics the
      * functionality of a copy constructor, but with a different
      * syntax.
      */
    public StarUnparser clone( StarUnparser copyMe )
    {
	return new StarUnparser( copyMe );
    }

    /** writeSkipped - writes out the skipped text that may
      * exist between the last written thing and the current
      * written thing:
      */
    public void writeSkipped( int thisLineNum )
    {
        StringBuffer writeMe = new StringBuffer();
        int          skipLineNum;
        boolean      firstTime;

        if( skips != null )
        { 
            firstTime = true;
            while( firstTime || writeMe.length() > 0 )
            {  
                firstTime = false;
		writeMe = new StringBuffer();
                skipLineNum = skips.getSkipTextBetween( prevCheckLineNum+1,
                                                        thisLineNum, writeMe );

                if( writeMe.length() > 0 )
                {   oWrit.println();
                    oWrit.print( writeMe );
                    prevCheckLineNum = skipLineNum;
                }
                // Some nodes might never have had their line numbers filled in
                // if they were added after parsing, so this check needs to be here.
                else
		{
                    if( thisLineNum > 0 )
                        prevCheckLineNum = thisLineNum;
		}
            }
        }
    }
    /** Writes out the StarNode-derived object given.
      * pass a StarFileNode and it prints the whole
      * file.  Pass a single DataValueNode and it just
      * prints that one value.  The output can happen at
      * any subset of the StarNode hierarchy.
      * @param node The StarNode to print out.
      * @param indentLvl The starting indent level to
      * @param longest The longest string in the value.
      * print at, typically zero.  The number of characters
      * indented will be = (indentLvl&nbsp;*&nbsp;getIndentSize()).
      * @see setIndentSize()
      */
    public void writeOut( StarNode node, int indentLvl )
    {
	writeOut( node, indentLvl, -1, false );
    }

    /** Writes out the StarNode-derived object given.
      * pass a StarFileNode and it prints the whole
      * file.  Pass a single DataValueNode and it just
      * prints that one value.  The output can happen at
      * any subset of the StarNode hierarchy.
      * @param node The StarNode to print out.
      * @param indentLvl The starting indent level to
      * @param longest The longest string in the value.
      * print at, typically zero.  The number of characters
      * indented will be = (indentLvl&nbsp;*&nbsp;getIndentSize()).
      * @see setIndentSize()
      */
    protected void writeOut( StarNode node, int indentLvl, int longest )
    {
        writeOut( node, indentLvl, longest, false );
    }
    
    protected void writeOut( StarNode node, int indentLvl, boolean internal )
    {
        writeOut( node, indentLvl, -1, internal );
    }
    
    protected void writeOut( StarNode node, int indentLvl, int longest, boolean internal )
    {
        // If this is the outermost call (the user calling the library)
        // then set up the inital values for getting the skiptext printing
        // to work:
        if( ! internal )
        {
            skips = node.mySkips();
            prevCheckLineNum = node.getLineNum();
        }
        
	// The order of this list is somewhat important.
	// In order to speed up the iteration, I put those
	// nodes that appear repetitively and often at the
	// top of this if-else list so they will be found
	// faster.  Nodes that appear much less often are down
	// at the bottom.

	try
	{
	    if(       Class.forName(
			StarValidity.clsNameDataValueNode
		    ).isInstance( node ) )
	    {
		writeDataValueNode( (DataValueNode)node, indentLvl, -99, -99 );
	    }
	    else if(  Class.forName(
			StarValidity.clsNameDataNameNode
		    ).isInstance( node ) )
	    {
		writeDataNameNode( (DataNameNode)node, indentLvl );
	    }
	    else if(  Class.forName(
			StarValidity.clsNameLoopRowNode
		    ).isInstance( node ) )
	    {
		writeLoopRowNode( (LoopRowNode)node, indentLvl,
			          null, null );
	    }
	    else if(  Class.forName(
			StarValidity.clsNameLoopTableNode
		    ).isInstance( node ) )
	    {
		writeLoopTableNode( (LoopTableNode)node, indentLvl );
	    }
	    else if(  Class.forName(
			StarValidity.clsNameDataItemNode
		    ).isInstance( node ) )
	    {
		writeDataItemNode( (DataItemNode)node, indentLvl, longest );
	    }
	    else if(  Class.forName(
			StarValidity.clsNameLoopNameListNode
		    ).isInstance( node ) )
	    {
		writeLoopNameListNode( (LoopNameListNode)node, indentLvl );
	    }
	    else if(  Class.forName(
			StarValidity.clsNameDataLoopNameListNode
		    ).isInstance( node ) )
	    {
		writeDataLoopNameListNode( (DataLoopNameListNode)node,
			indentLvl );
	    }
	    else if(  Class.forName(
			StarValidity.clsNameDataLoopNode
		    ).isInstance( node ) )
	    {
		writeDataLoopNode( (DataLoopNode)node, indentLvl );
	    }
	    else if(  Class.forName(
			StarValidity.clsNameSaveFrameNode
		    ).isInstance( node ) )
	    {
		writeSaveFrameNode( (SaveFrameNode)node, indentLvl );
	    }
	    else if(  Class.forName(
			StarValidity.clsNameBlockNode
		    ).isInstance( node ) )
	    {
		writeBlockNode( (BlockNode)node, indentLvl );
	    }
	    else if(  Class.forName(
			StarValidity.clsNameStarFileNode
		    ).isInstance( node ) )
	    {
		writeStarFileNode( (StarFileNode)node, indentLvl );
	    }
	    else
	    {
	    }
	}
	catch( ClassNotFoundException e )
	{
	    System.out.println( e.getMessage() );
	    e.printStackTrace();
	}
	oWrit.flush();
    }

    // Write out the indentation:
    protected void writeIndent( int indentLvl )
    {
	if( formatting )
	{
	    int i;
	    int numChars = indentLvl * indentSize;

	    // I can't find a Java equivilent to
	    // the C code: sprintf( "%20s", someStr );
	    // (Java is really bad at output formatting.)
	    for( i = 1 ; i <= numChars ; i++ )
		oWrit.print( ' ' );
	}
    }

    // Write out the comment:
    protected void writePreComment( String cmt, int indentLvl )
    {
	char ch;
	int  idx;

	// Don't print if the comment is a nullstring:
	if( cmt != null )
	{
	    if( cmt.length() > 0 )
	    {
		if( formatting )
		    writeIndent( indentLvl );
		for( idx = 0 ; idx < cmt.length() ; idx++ )
		{
		    ch = cmt.charAt(idx);
		    oWrit.print(ch);
		    if( Character.getType(ch) == Character.LINE_SEPARATOR )
		    {
			if( formatting )
			    writeIndent( indentLvl ); // indent each new line.
		    }
		}
	    }
	}
    }

    // Write out one DataValueNode:
    protected void writeDataValueNode( DataValueNode  node,
	                               int            indentLvl,
				       int            preSize,
				       int            postSize )
    {
        writeDataValueNode( node, indentLvl, preSize, postSize, false );
    }
    protected void writeDataValueNode( DataValueNode  node,
	                               int            indentLvl,
				       int            preSize,
				       int            postSize,
				       boolean        nonQuotedFlag  )
    {
	int     dotIdx;
	int     i;
	String  tmpStr;
	String  thisVal;


	if( !formatting || preSize == -99 || postSize == -99 )
	{
	    if(      node.getDelimType() == DataValueNode.NON )
            {
		oWrit.print( node.getValue() );
		oWrit.print( ' ' );
	    }
	    else if( node.getDelimType() == DataValueNode.SINGLE )
	    {
		oWrit.print( "'" + node.getValue() + "'" );
		oWrit.print( ' ' );
	    }
	    else if( node.getDelimType() == DataValueNode.DOUBLE )
	    {
		oWrit.print( "\"" + node.getValue() + "\"" );
		oWrit.print( ' ' );
	    }
	    else if( node.getDelimType() == DataValueNode.FRAMECODE )
	    {
		oWrit.print( "$" + node.getValue() );
		oWrit.print( ' ' );
	    }
	    else if( node.getDelimType() == DataValueNode.SEMICOLON )
	    {
		oWrit.println();
		oWrit.println( ";" );
		char firstChar = node.getValue().charAt( 0 );

		thisVal =  node.getValue() ;

		// Within the text of the string, watch for the case where
		// a line begins with a semicolon, and if it does, change it to
		// a space followed by a semicolon so it can be parsed back in.
		String printVal = "";
		int     idx = 0;
		boolean afterEoln = true; // act like the first char is after an eoln.
		while( idx < thisVal.length() ) {
		    char ch = thisVal.charAt( idx );
		    if( afterEoln )
		    {   if( ch == ';' )
		        {   printVal += ' ';
		        }
		    }
		    if( ch == '\n' || ch == '\r' )
		    {   afterEoln = true;
		    }
		    else 
		    {   afterEoln = false;
		    }
		    printVal += ch;

		    ++idx;
		}

		oWrit.print( printVal );

		char lastChar = node.getValue().charAt(
				    node.getValue().length()-1 );
		if( lastChar != '\n' && lastChar != '\r' )
		    oWrit.println();
		oWrit.println(";");
	    }
	}
	else
	{
            writeSkipped( node.getLineNum() );
            writePreComment( node.getPreComment(), indentLvl );


	    thisVal = node.getValue();
	    if(      node.getDelimType() == DataValueNode.NON )
	    {
		// Non-delimited values are by far the most complex case.
		// Non-delimited values could be numeric, with possible
		// digits after the decimal place, and hence there is a lot
		// of work to be done to handle the formatting of this case:

		// skip this if all values in the column are non-quoted:
		if( ! nonQuotedFlag )
		{   oWrit.print(' '); // extra space to line up with delimiters on
				      // other values.
	        }

		if(  postSize >= 0 && isItNumeric( thisVal )  )
		{
		    tmpStr = "";
		    // Find the dot location:
		    dotIdx = thisVal.indexOf( '.' );
		    if( dotIdx == -1 )
			dotIdx = thisVal.length();

		    // Print everything up to but not including the dot.
		    // (right justified) I wish Java had a better way to
		    // do this (something like C's printf %*.*s formatter
		    // is nice).  There's stuff in java.text.*, but none of
		    // it seems to work on strings, just numbers.
		    for( i = 0 ; i < preSize - dotIdx ; i++ )
			oWrit.print(' ');
		    oWrit.print( thisVal.substring( 0, dotIdx ) );

		    // If any part of it is still left after the dot,
		    // print the dot and the fractional part:
		    if( dotIdx < thisVal.length() )
		    {
			oWrit.print( thisVal.substring(
				         dotIdx, thisVal.length() ) );
			// pad the leftover field space:
			for( i = thisVal.length() - dotIdx; i < postSize+1; i++ )
			    oWrit.print(' ');
		    }
		    else // This value has no fractional part, but others in
			 // this column do, so pad with spaces to match it
			 // up.
		    {
			if( postSize >= 0 )
			{
			    // Why the heck can't I find an equivilent of
			    // printf( "%*.*s", i,j,foo ) in Java!?!!  This is
			    // really silly to have to do it this way:
			    for( i = 1 ; i <= postSize ; i++ )
				oWrit.print(' ');
			}
			if( postSize >= 1 ) // another space for the dot too
			    oWrit.print(' ');
		    }
		}
		else // This is an alphanumeric string, not a numeric.
		{
		    oWrit.print( thisVal );
		    // pad out the space.
		    for( i = thisVal.length() ; i < preSize ; i++ )
			oWrit.print(' ');

		    // If other values in this column were numeric,
		    // pad out the post-decimal point spaces too:
		    if( postSize >= 0 )
		    {
			for( i = 0 ; i < postSize ; i++ )
			    oWrit.print(' ');
		    }
		    if( postSize >= 1 ) // another space for the dot too
			oWrit.print(' ');
		}
	    }
	    else if( node.getDelimType() == DataValueNode.SINGLE )
	    {
		oWrit.print( "'" + thisVal + "'" );
		for( i = thisVal.length()+1 ; i < preSize ; i++ )
		    oWrit.print(' ');
	    }
	    else if( node.getDelimType() == DataValueNode.DOUBLE )
	    {
		oWrit.print( "\"" + thisVal + "\"" );
		for( i = thisVal.length()+1 ; i < preSize ; i++ )
		    oWrit.print(' ');
	    }
	    else if( node.getDelimType() == DataValueNode.SEMICOLON )
	    {
		oWrit.println();
		oWrit.println(';');

		// Within the text of the string, watch for the case where
		// a line begins with a semicolon, and if it does, change it to
		// a space followed by a semicolon so it can be parsed back in.
		String printVal = "";
		int     idx = 0;
		boolean afterEoln = true; // act like the first char is after an eoln.
		while( idx < thisVal.length() ) {
		    char ch = thisVal.charAt( idx );
		    if( afterEoln )
		    {   if( ch == ';')
		        {   printVal += ' ';
		        }
		    }
		    if( ch == '\n' || ch == '\r' )
		    {   afterEoln = true;
		    }
		    else 
		    {   afterEoln = false;
		    }
		    printVal += ch;

		    ++idx;
		}

		oWrit.print( printVal );

		// If it doesn't end with an end-of line, add one before the
		// closing semicolon:
		if( (thisVal.charAt( thisVal.length()-1 ) != '\n' ) &&
		    (thisVal.charAt( thisVal.length()-1 ) != '\r' )   )
		    oWrit.println();

		oWrit.println(';');
	    }
	    else if( node.getDelimType() == DataValueNode.FRAMECODE )
	    {
		oWrit.print( "$" + thisVal );
		for( i = thisVal.length() ; i < preSize ; i++ )
		    oWrit.print(' ');
	    }

	    // If this was not a nonquoted value, but OTHER values in this
	    // column were numeric nonquoted values, then space out to
	    // match up with the 'postSize' of the other values in this
	    // column (plus 1 for the dot):
	    if( node.getDelimType() != DataValueNode.NON  && postSize >= 0 )
	    {
		for( i = 1 ; i <= preSize ; i++ )
		    oWrit.print(' ');
		if( postSize >= 1 )
		    oWrit.print(' '); // one more for the decimal point that
		                      // other values in this column have.
	    }
	}
    }

    // Write out one DataNameNode:
    protected void writeDataNameNode( DataNameNode  node,
	                              int           indentLvl )
    {
	if( !formatting )
	{
	    oWrit.print( node.getLabel() + " " );
	}
	else
	{
            writeSkipped( node.getLineNum() );
	    writePreComment( node.getPreComment(), indentLvl );
	    writeIndent( indentLvl );
	    oWrit.print( node.getLabel() + " " );
	}
    }

    // Write out one DataItemnode:
    protected void writeDataItemNode( DataItemNode  node,
	                              int           indentLvl,
				      int           longest )
    {
	int  i;

	if( !formatting )
	{
	    writeDataNameNode( node.getNameNode(), 0 );
	    writeDataValueNode( node.getValueNode(), 0 , -99, -99 );
	    oWrit.println();
	}
	else
	{
            writeSkipped( node.getLineNum() );
	    writePreComment( node.getPreComment(),indentLvl );
	    
	    writeDataNameNode( node.getNameNode(), indentLvl );
	    for( i = node.getLabel().length() ; i < longest ; i++ )
		oWrit.print(' ');

	    // Extra space if a non-delimited string (to line up with the
	    // delimiters of other values:
	    if( node.getValueNode().getDelimType() == DataValueNode.NON )
		oWrit.print(' ');
	    writeDataValueNode( node.getValueNode(), indentLvl, -99, -99 );
	    oWrit.println();
	}
    }

    // Write out one DataLoopNode:
    protected void writeDataLoopNode( DataLoopNode  node,
	                              int           indentLvl )
    {
	int ind;
	if( formatting )
	{
            writeSkipped( node.getLineNum() );
	    writePreComment(node.getPreComment(), indentLvl);
	    ind = indentLvl + 1;
	}
	else
	{
	    ind = 0;
	}
	writeDataLoopNameListNode( node.getNames(), indentLvl );
	writeLoopTableNode( node.getVals(), ind );
	oWrit.println();
    }

    // Write out one DataLoopNameListNode:
    protected void writeDataLoopNameListNode( DataLoopNameListNode node,
	                                      int                  indentLvl )
    {
        writeSkipped( node.getLineNum() );
	writePreComment(node.getPreComment(), indentLvl);
	int i;
	oWrit.println();
	for( i = 0 ; i < node.size() ; i++ )
	{
	    writeLoopNameListNode( node.elementAt(i), indentLvl + i );
	}
    }

    // Write out one LoopNameListNode:
    protected void writeLoopNameListNode( LoopNameListNode  node,
	                                  int               indentLvl )
    {
	int i;

        writeSkipped( node.getLineNum() );
	writePreComment(node.getPreComment(), indentLvl);

	writeIndent(indentLvl);
	oWrit.println( "loop_" );
	for( i = 0 ; i < node.size() ; i++ )
	{
	    writeDataNameNode( node.elementAt(i), indentLvl+1 );
	    oWrit.println();

	    // Special case for NMRSTAR's category - give extra space:
	    if( node.elementAt(i).getLabel().equals( "_Saveframe_category" ) )
		oWrit.println();
	}
    }

    // Write out one LoopTableNode:
    protected void writeLoopTableNode( LoopTableNode  node,
	                               int            indentLvl )
    {
	try {
	    if( !formatting )
	    {
		int i;

		for( i = 0 ; i < node.size() ; i++ )
		{
		    writeLoopRowNode( node.elementAt(i), 0, null, null );
		    oWrit.println();
		}
		if( node.getParent() != null &&
		    Class.forName(StarValidity.pkgName()+".DataLoopNode").
			  isInstance( node.getParent() ) )
		{
		    if( ! suppressStops )
			oWrit.println( "stop_" );
		}
		else
		    oWrit.println( "stop_" );
	    }
	    else
	    {
		Vector preSizes = new Vector();
		Vector postSizes = new Vector();
		Vector nonQuotedFlags = new Vector();
		int    rowCnt;
		int    rowIdx;

		writeSkipped( node.getLineNum() );
		writePreComment( node.getPreComment(),
				 (node.getIndentFlag() ) ? indentLvl : 0 );
		node.calcPrintSizes( preSizes, postSizes, nonQuotedFlags );

		for( rowIdx = 0 ; rowIdx < node.size() ; rowIdx++ )
		{
		    if( rowIdx % node.getRowsPerLine() == 0 || rowIdx == 0 )
		    {
			oWrit.println();
			writeIndent( (node.getIndentFlag() ) ? indentLvl : 0 );
		    }
		    else
			oWrit.print( " " ); //extra space between rows on same line.
		    writeLoopRowNode( node.elementAt(rowIdx),
				      indentLvl,
				      preSizes,
				      postSizes,
				      nonQuotedFlags);
		}
		oWrit.println();

		// outdent the "stop_":
		writeIndent( (indentLvl > 0 && node.getIndentFlag() ) ?
				       (indentLvl-1) : 0 );

		if( node.getParent() != null &&
		    Class.forName(StarValidity.pkgName()+".DataLoopNode").
			  isInstance( node.getParent() ) )
		{
		    if( ! suppressStops )
			oWrit.println( "stop_" );
		}
		else
		    oWrit.println( "stop_" );
	    }
	} catch( ClassNotFoundException e )
	{
	    throw new InternalException( e.getMessage() );
	}
    }

    // Write out one LoopRowNode:
    protected void writeLoopRowNode( LoopRowNode node,
	                             int         indentLvl,
				     Vector      preSizes,
				     Vector      postSizes )
    {
        writeLoopRowNode( node, indentLvl, preSizes, postSizes, null );
    }
    // Write out one LoopRowNode:
    protected void writeLoopRowNode( LoopRowNode node,
	                             int         indentLvl,
				     Vector      preSizes,
				     Vector      postSizes,
				     Vector      nonQuotedFlags )
    {
	if( !formatting || preSizes == null || postSizes == null )
	{
	    int           i;
	    LoopTableNode inside;

	    for( i = 0 ; i < node.size() ; i++ )
	    {
		writeDataValueNode( node.elementAt(i), 0, -99, -99 );
	    }
	}
	else
	{
	    int idx;

            writeSkipped( node.getLineNum() );
	    writePreComment( node.getPreComment(), indentLvl );
	    for( idx = 0 ; idx < node.size() ; idx++ )
	    {
		writeDataValueNode( node.elementAt(idx),
			indentLvl,
			( (Integer) ( preSizes.elementAt(idx) ) ).intValue(),
			( (Integer) ( postSizes.elementAt(idx) ) ).intValue(),
			( (Boolean) ( nonQuotedFlags.elementAt(idx) ) ).booleanValue()
			);
		// tabular case:
		if(  (  node.getParent() != null ) &&
		     ( (LoopTableNode) ( node.getParent() ) ).getTabFlag()  )
		{
		    oWrit.print(" ");
		    int i;
		    for( i = 0 ; i < extraColumnSpaces ; i++ ) {
		      oWrit.print(' ');
		    }
		}
		// linear case:
		else
		{
		    oWrit.println();
		    writeIndent(indentLvl);
		}
	    }
	}
	if( node.getInnerLoop() != null )
	{
	    writeLoopTableNode( node.getInnerLoop(),
		                (formatting ? indentLvl+1 : 0 ) );
	}
    }

    // Write out one SaveFrameNode:
    protected void writeSaveFrameNode( SaveFrameNode node,
	                               int           indentLvl )
    {
	int i;
	int longestValLength = -1;

        if( indentLvl > 0 )
  	    indentLvl--;  // outdent saveframes one level.
			  // (This is not astheticly pleasing
			  // for other star files, but it is
			  // used for NMR-star files.)
        writeSkipped( node.getLineNum() );
	writePreComment( node.getPreComment(), indentLvl );
	if( formatting )
	    writeIndent(indentLvl);
	oWrit.println();
	oWrit.println( node.getLabel() );

	// Get the print sizes to make things line up nice:
	// (Find the longest string length of all the DataItemNodes'
	// values.)
	if( formatting )
	{
	    int len;

	    try
	    {
		for( i = 0 ; i < node.size() ; i++ )
		{   if( Class.forName( StarValidity.clsNameDataItemNode ).
			    isInstance( node.elementAt(i) )   )
		    {
			len = ( (DataItemNode)(node.elementAt(i)) ).
				    getLabel().length();
			if( len > longestValLength )
			    longestValLength = len;
		    }
		}
	    }
	    catch( ClassNotFoundException e )
	    {
		throw new InternalException( e.getMessage() );
	    }
	}
	for( i = 0 ; i < node.size() ; i++ )
	{
	    // Let writeOut() figure out what type it is,
	    // and how to print it:
	    writeOut( node.elementAt(i),
		      indentLvl + (formatting ? 1 : 0 ),
		      longestValLength,
                      true);
	}
	if( formatting )
	    writeIndent(indentLvl);
	oWrit.println( "save_" );
	oWrit.println();
    }

    // Write out one BlockNode:
    protected void writeBlockNode( BlockNode  node,
	                           int        indentLvl )
    {
	int i;
	int longestValLength = -1;

	// This node prints out the same way whether it is being
	// formatted or not.
        writeSkipped( node.getLineNum() );
	writePreComment( node.getPreComment(), indentLvl );
	oWrit.println( node.getLabel() );
	oWrit.println();

	// Get the print sizes to make things line up nice:
	// (Find the longest string length of all the DataItemNodes'
	// values.)
	if( formatting )
	{
	    int len;

	    try
	    {
		for( i = 0 ; i < node.size() ; i++ )
		{   if( Class.forName( StarValidity.clsNameDataItemNode ).
			    isInstance( node.elementAt(i) )   )
		    {
			len = ( (DataItemNode)(node.elementAt(i)) ).
				    getLabel().length();
			if( len > longestValLength )
			    longestValLength = len;
		    }
		}
	    }
	    catch( ClassNotFoundException e )
	    {   throw new InternalException( e.getMessage() );
	    }
	}

	for( i = 0 ; i < node.size() ; i++ )
	{
	    // Let writeOut() figure out what type it is,
	    // and how to print it:
	    writeOut( node.elementAt(i),
		      indentLvl + (formatting ? 1 : 0),
		      longestValLength,
                      true );
	}
	oWrit.println();

    }

    // Write out the whole StarFileNode:
    protected void writeStarFileNode( StarFileNode  node,
	                              int           indentLvl )
    {
	int i;

	// This node prints out the same way whether it is being
	// formatted or not.
        writeSkipped( node.getLineNum() );
	writePreComment( node.getPreComment(), indentLvl );
	for( i = 0 ; i < node.size() ; i++ )
	{
	    // Let writeOut() figure out what type it is,
	    // and how to print it:
	    writeOut
            ( node.elementAt(i), indentLvl, true  );
	}
	oWrit.println();
    }




    /**
      * Sets the number of characters to indent when printing.
      * Each time the output formatter needs to indent one
      * more level, this is the number of spaces it will
      * indent.
      */
    public void setIndentSize( int s )
    {
	indentSize = s;
    }
    /** Gets the number of characters to indent when printing.
      * Each time the output formatter needs to indent one
      * more level, this is the number of spaces it will
      * indent.
      * @return number of characters.
      */
    public int getIndentSize()
    {
	return indentSize;
    }

    /** 
      * Sets the number of extra column spaces to print
      * out between columns in tabular loop output.
      * Note that even when this is set to zero, there is still
      * one space between columns.  The typical number of
      * spaces between columns is this number plus 1.  This is
      * the number of <b>extra</b> spaces beyond the minimum.
      * (So if you want three spaces typically, set this to 2.)
      * Note that this defaults to 2 if you don't set it.
      */
    public void setExtraColumnSpaces( int numExtra )
    {
	extraColumnSpaces = numExtra;
    }

    /** Gets the value of current number of extra column spaces
      * @return the number
      * @see setExtraColumnSpaces
      */
    public int getExtraColumnSpaces()
    {
	return extraColumnSpaces;
    }

    /** Turns off (or on) the formatting code for output.  By
      * default, the formatting is always on.  The formatting
      * can be turned off if the output does not need to be
      * made 'pretty', and only needs to be syntactically
      * correct.  This is mostly useful when you are creating
      * STAR syntax that you intend to be read by another computer
      * program and you don't think a human's eyes will ever need
      * to look at the output.  Formatting slows down the output
      * because multiple passes are required to count characters
      * in values to make the values line up with each other.  Turning
      * the formatting off will speed up the output at the expense of
      * human readability.  Turning off formatting also condenses the
      * whitespace down to the smallest it can be while still preserving
      * syntax.
      * <P>
      * One example of a place where this could be useful is when two
      * programs are communicating using STAR syntax over a pipe or
      * socket.
      * <P>
      * @param isOn true (default) == formatting is on, false == turn
      *        formatting off.
      */
    public void setFormatting( boolean isOn )
    {
	formatting = isOn;
    }

    /** Gets the value of current formatting flag.
      * @return the formatting flag.
      * @see setFormatting
      */
    public boolean getFormatting()
    {
	return formatting;
    }

    /** Suppresses output of "stop_" after loops.
      * Although it is legal in STAR to end a loop
      * optionally with a "stop_", this is not allowed
      * by some subsets of STAR such as mmCif.  Using
      * this flag you can toggle whether or not the
      * "stop_" marker at the end of a loop is generated
      * when unparsing.
      *
      * The default is false, meaning stops are not suppressed,
      * and are printed.
      */
    public void setSuppressStops( boolean isOn )
    {
        suppressStops = isOn;
    }

    /** Specifies if output of "stop_" after loops is suppressed.
      * Although it is legal in STAR to end a loop
      * optionally with a "stop_", this is not allowed
      * by some subsets of STAR such as mmCif.  Using
      * this flag you can toggle whether or not the
      * "stop_" marker at the end of a loop is generated
      * when unparsing.
      *
      * The default is false, meaning stops are not suppressed,
      * and are printed.
      */
    public boolean getSuppressStops()
    {
        return suppressStops;
    }

    // A boolean check - true if the string contains nothing but a
    // valid numeric string, either as an integer or as
    // a floating point (does not handle exponential notation like
    // "1.2345e-43", though - but for what we are doing, this is fine.
    // We don't really want to format those like the rest of the numbers
    // anyway.)
    protected boolean isItNumeric( String str )
    {
	int idx;

	// This code simulates the following 'grep' regular expression:
	//     a numeric string is :  [ \t\n]*[+-]?[0-9]*\.?[0-9]*[ \t\n]*

	idx = 0;
	// [ \t\n]*
	while( idx < str.length() && Character.isWhitespace(str.charAt(idx)) )
	    idx++;
	// [+-]?
	if( idx < str.length() &&
		( str.charAt(idx) == '+' || str.charAt(idx) == '-' )  )
	    idx++;
	// [0-9]*
	while( idx < str.length() && Character.isDigit(str.charAt(idx)) )
	    idx++;
	// \.?
	if( idx < str.length() && str.charAt(idx) == '.' )
	    idx++;
	// [0-9]*
	while( idx < str.length() && Character.isDigit(str.charAt(idx)) )
	    idx++;
	// [ \t\n]*
	while( idx < str.length() && Character.isWhitespace(str.charAt(idx)) )
	    idx++;

	// If it is truly a good numeric expression, the above code
	// should have gotten us to the end of the string.  If it did
	// not, then this is not a number.  (notice that this definition
	// counts nullstrings and the single dot '.' as numerics, but
	// for our purposes, this is okay:
	if( idx == str.length() )
	    return true;
	else
	    return false;
    }

}

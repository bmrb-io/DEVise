package EDU.bmrb.starlibj;
import java.lang.*;
import java.util.*;

/** This is a set of some simple utility methods that are used throughout
  * the starlibj.  Mostly they deal with string syntax checks.  These
  * routines are used by the starlibj functions to decide if certain
  * operations should be rejected.  (For example, deciding if a string
  * is an acceptable saveframe name or tagname.)  They are left public
  * so that the caller of this package/library can use them too.
  */
public class StarValidity
{

    private static String packageNameStatic = null;

    /** Returns the String name of this package.  This is needed
      * because Java loses the 'import' information at runtime,
      * and therefore we have to give full-path names for
      * types when looking at the Class object.  (e.g. you can't
      * just say:
      * <PRE>
      *      Class.forName("Vector")... // (won't work)
      * </PRE>
      * You have to say:
      * <PRE>
      *      Class.forName("java.lang.Vector")...
      * </PRE>
      * @return The package name for this class.
      */
    public static String pkgName( )
    {
	// This really silly jumping through hoops is needed
	// because Java doesn't let you get the name of the
	// class except via an object of that class type.

	// It doesn't matter which starlibj Class we pick here,
	// because we are only interested in finding the package
	// it is inside:  I picked "StarNode" because it was
	// small and simple.

	// To save time, we want to only do this once during the
	// life of the program, so stuff the result into a
	// static string variable and use it on subsequent
	// calls:
	if( packageNameStatic == null )
	{
	    StarNode dummyObj = new StarNode();
	    String className = dummyObj.getClass().getName();
	    packageNameStatic =
		className.substring( 0, className.lastIndexOf('.') );
	}
	return packageNameStatic;
    }

    /** Returns true if the string is a valid tag name.
      * @return true if valid, false if invalid
      */
    public static boolean isValidTagName( String s )
    {
	int i;

	// Invalid if it doesn't start with an underscore.
	if( s.charAt(0) != '_' )
 	    return false;
	// Invalid if it contains whitespace within.
	for( i = 0 ; i < s.length() ; i++ )
	    if( Character.isWhitespace( s.charAt(i) ) )
		return false;
	// If it got this far, its good.
	return true;
    }

    /** Returns true if the string is a valid saveframe name.
      * @return true if valid, false if invalid
      */
    public static boolean isValidSaveName( String s )
    {
	int i;

	// Invalid if it doesn't start with "save_"
	if( ! s.substring( 0,5 ).equals("save_") )
 	    return false;
	// Invalid if it *only* contains "save_" and nothing more.
	if( s.length() <= 5 )
	    return false;
	// Invalid if it contains whitespace within.
	for( i = 5 ; i < s.length() ; i++ )
	    if( Character.isWhitespace( s.charAt(i) ) )
		return false;
	// If it got this far, its good.
	return true;
    }
    /** Returns true if the string is a valid data/global block name.
      * @return true if valid, false if invalid
      */
    public static boolean isValidBlockName( String s )
    {
	int i;

	// Special case: okay if it is "global_"
	if( s.equals("global_") )
	    return true;
	// Invalid if it doesn't start with "data_"
	if( ! s.substring( 0,5 ).equals("data_") )
 	    return false;
	// Invalid if it *only* contains "data_" and nothing more.
	if( s.length() <= 5 )
	    return false;
	// Invalid if it contains whitespace within.
	for( i = 5 ; i < s.length() ; i++ )
	    if( Character.isWhitespace( s.charAt(i) ) )
		return false;
	// If it got this far, its good.
	return true;
    }

    /** Returns true if the string given is valid for a
      * nondelimited DataValueNode (no whitespace).
      * @return true if valid, false if invalid
      */
    public static boolean isValidValueForNonDelim( String s )
    {
	int i;

	// Must have at leas some value.
	if( s.length() == 0 )
	    return false;
	// Invalid if it starts with ' or " or _ or $
	if( s.charAt(0) == '\'' )
	    return false;
	if( s.charAt(0) == '\"' )
	    return false;
	if( s.charAt(0) == '$' )
	    return false;
	if( s.charAt(0) == '_' )
	    return false;
	// Invalid if the first character is a comment starter
	//    ('#') - hashes are legal inside the value as long as
	//     it is not the first character of the string.  If it
	//     is the first character, then this really should be a
	//     comment, not a value.
	// Invalid if contains whitespace within.
	if( s.charAt(0) == '#' )
	    return false;
	for( i = 0 ; i < s.length() ; i++ )
	{
	    if( Character.isWhitespace( s.charAt(i) ) )
		return false;
	}
	// Invalid if it is a keyword:
	if( s.toLowerCase().startsWith("data_") ||
	    s.toLowerCase().startsWith("save_") ||
	    s.toLowerCase().equals("global_") ||
	    s.toLowerCase().equals("loop_") ||
	    s.toLowerCase().equals("stop_") )
	{
	    return false;
	}
	return true;
    }


    /** Returns true if the string given is valid for a
      * single-quote delimiter in a DataValueNode.
      */
    public static boolean isValidValueForSingleDelim( String s )
    {
	int i;
	// Invalid if it contains line breaks within or
	// a single-tick quote followed by space.
	for( i = 0 ; i < s.length() ; i++ )
	{
	    if( s.charAt(i) == '\n' || s.charAt(i) == '\r' )
		return false;
	    else if( s.charAt(i) == '\'' )
		if( i < s.length() - 1 )
		{
		    if( s.charAt(i+1) == ' ' )
			return false;
		}
		else
		    return true;
	}
	return true;
    }

    /** Returns true if the string given is valid for a
      * double-quote delimiter in a DataValueNode.
      */
    public static boolean isValidValueForDoubleDelim( String s )
    {
	int i;
	// Invalid if it contains line breaks within or
	// a double-tick quote followed by space.
	for( i = 0 ; i < s.length() ; i++ )
	{
	    if( s.charAt(i) == '\n' || s.charAt(i) == '\r' )
		return false;
	    else if( s.charAt(i) == '\"' )
		if( i < s.length() - 1 )
		{
		    if( s.charAt(i+1) == ' ' )
			return false;
		}
		else
		    return true;
	}
	return true;
    }

    /** Returns true if the string given is valid for a
      * framecode delimiter (dollar sign) in a DataValueNode,
      */
    public static boolean isValidValueForFrameCodeDelim( String s )
    {
	int i;

	// Invalid if it contains whitespace within.
	for( i = 0 ; i < s.length() ; i++ )
	    if( Character.isWhitespace( s.charAt(i) ) )
		return false;
	return true;
    }

    /** Determines if the string given is valid for the delimiter
      * type given (from DataValueNode).
      * @return true = valid, false = invalid. 
      * @see DataValueNode
      */
    public static boolean isValidForDelim( String s, int delim )
    {
	if( delim == DataValueNode.NON )
	    return isValidValueForNonDelim(s);
	else if( delim == DataValueNode.DOUBLE )
	    return isValidValueForDoubleDelim(s);
	else if( delim == DataValueNode.SINGLE )
	    return isValidValueForSingleDelim(s);
	else if( delim == DataValueNode.SEMICOLON )
	    return true;  // all semicolon strings are valid.
	else if( delim == DataValueNode.FRAMECODE )
	    return isValidValueForFrameCodeDelim(s);
	                           // This could probably be something better.
	else
	    return false;
    }

    /** Return the valid delim type to use
      * @return the delim type that is safe to use for this value.
      */
    public static short getValidDelimFor( String s )
    {
        if( isValidValueForNonDelim(s) )
            return DataValueNode.NON;
        else if( isValidValueForDoubleDelim(s) )
	    return DataValueNode.DOUBLE;
        else if( isValidValueForSingleDelim(s) )
	    return DataValueNode.SINGLE;
	else 
	    return DataValueNode.SEMICOLON;
    }

    /** Given a string, parse (starting at the first char and extending
      * until valid syntax is exhausted) a value string in STAR syntax.
      * @return The string and delimiter type in a ParseValFromRetVal
      *         structure.
      * @param str the string to parse through.
      * @param makeNew set to true to return a new String, false not to.
      *                 Setting it to false is useful when you want to
      *                 parse for the end of the string, but you don't
      *                 care what the string is (you are skipping past
      *                 the value.)
      */
    public static ParseValFromRetVal parseValFrom( String str, boolean makeNew )
    {
	char curChar;
	int rowOffset, colOffset;
	int idx; // string index
	int startIdx; // index of first char of value (does not
	              // include delimiters)
	int endIdx;  // index of last char of value + 1 (plus one one
	             // because that fits well with Java conventions)
	int nextIdx;  // The index that would be used for the next
	              // parse to start from to get more values in this string.
	boolean inLeadingWhitespace;
	boolean inNonQuoted;
	boolean inFramecode;
	boolean inSingle;
	boolean inDouble;
	boolean inSemicolon;
	boolean reachedEnd;
	boolean found;
	ParseValFromRetVal retVal;

	idx = 0;
	inLeadingWhitespace =true; 
	inNonQuoted = false;
	inFramecode = false;
	inSingle    = false;
	inDouble    = false;
	inSemicolon = false;
	reachedEnd  = false;
	startIdx = 0;
	endIdx = 0;
	nextIdx = 0;
	found = false;
	rowOffset = 0;
	colOffset = 0;

	while( !reachedEnd )
	{
	    if( idx >= str.length() )
	    {
		reachedEnd = true;
		endIdx = idx;
		nextIdx = endIdx;
	    }
	    else if( inLeadingWhitespace )
	    {
		if( Character.isWhitespace( str.charAt( idx ) ) )
		{
		    idx++;
		}
		else
		{
		    inLeadingWhitespace = false;
		    found = true;
		    // This is the starting character of the string,
		    // determine its delimiter type.
		    curChar = str.charAt( idx );
		    if( curChar == '"' ) // double-quoted delimiter
		    {   inDouble = true;
			idx++;
			startIdx = idx;
		    }
		    else if( curChar == '\'' )
		    {   inSingle = true;
			idx++;
			startIdx = idx;
		    }
		    else if( curChar == ';' )
		    {   inSemicolon = true;
			idx++;
			// If next char is eoln, eat it up:
			while(  str.charAt( idx ) == '\n' ||
				str.charAt( idx ) == '\r'    ) 
			{
			    idx++;
			}
			startIdx = idx;
		    }
		    else if( curChar == '$' )
		    {
			inFramecode = true;
			idx++;
			startIdx = idx;
		    }
		    else // default: non-quoted string:
		    {
			inNonQuoted = true;
			startIdx = idx;
			idx++;
		    }

		}
		continue;
	    }
	    else 
	    {
		if( inDouble && str.charAt( idx ) == '"' )
		{
		    reachedEnd = true;
		    endIdx = idx;
		    nextIdx = idx + 1;
		}
		else if( inSingle && str.charAt( idx ) == '\'' )
		{
		    reachedEnd = true;
		    endIdx = idx;
		    nextIdx = idx + 1;
		}
		else if( inSemicolon && str.charAt( idx ) == ';' &&
			 ( str.charAt( idx - 1 ) == '\n' ||
			   str.charAt( idx - 1 ) == '\r' )   )
		{
		    reachedEnd = true;
		    endIdx = idx - 1;
		    nextIdx = idx + 1;
		}
		else if(  ( inNonQuoted || inFramecode ) &&
			  Character.isWhitespace( str.charAt( idx ) )   )
		{
		    reachedEnd = true;
		    endIdx = idx;
		    nextIdx = idx;
		}
		else
		{
		    idx++;
		}
	    }
	}
	retVal = new ParseValFromRetVal();
	if( makeNew )
	{
	    retVal.delim =  inNonQuoted ? DataValueNode.NON :
			    inDouble ? DataValueNode.DOUBLE :
			    inSingle ? DataValueNode.SINGLE :
			    inSemicolon ? DataValueNode.SEMICOLON :
			    inFramecode ? DataValueNode.FRAMECODE :
				/*can't happen*/ DataValueNode.DONT_CARE ;
	    retVal.str = str.substring( startIdx, endIdx );
	}
	else
	{
	    retVal.str = null;
	}
	retVal.endingIdx = endIdx;
	retVal.nextIdx = nextIdx;
	retVal.found = found;
	return retVal;
    }

     public static String clsNameASCII_CharStream = "EDU.bmrb.starlibj.ASCII_CharStream" ;
     public static String clsNameBadValueForDelimiter = "EDU.bmrb.starlibj.BadValueForDelimiter" ;
     public static String clsNameBlockListVector = "EDU.bmrb.starlibj.BlockListVector" ;
     public static String clsNameBlockNode = "EDU.bmrb.starlibj.BlockNode" ;
     public static String clsNameCharStream = "EDU.bmrb.starlibj.CharStream" ;
     public static String clsNameDataItemNode = "EDU.bmrb.starlibj.DataItemNode" ;
     public static String clsNameDataLoopNameListNode = "EDU.bmrb.starlibj.DataLoopNameListNode" ;
     public static String clsNameDataLoopNode = "EDU.bmrb.starlibj.DataLoopNode" ;
     public static String clsNameDataNameNode = "EDU.bmrb.starlibj.DataNameNode" ;
     public static String clsNameDataValueNode = "EDU.bmrb.starlibj.DataValueNode" ;
     public static String clsNameDataValuesVector = "EDU.bmrb.starlibj.DataValuesVector" ;
     public static String clsNameHomemadeStringBuffer = "EDU.bmrb.starlibj.HomemadeStringBuffer" ;
     public static String clsNameInternalException = "EDU.bmrb.starlibj.InternalException" ;
     public static String clsNameLoopNameListNode = "EDU.bmrb.starlibj.LoopNameListNode" ;
     public static String clsNameLoopRowNode = "EDU.bmrb.starlibj.LoopRowNode" ;
     public static String clsNameLoopRowsVector = "EDU.bmrb.starlibj.LoopRowsVector" ;
     public static String clsNameLoopTableNode = "EDU.bmrb.starlibj.LoopTableNode" ;
     public static String clsNameNameListVector = "EDU.bmrb.starlibj.NameListVector" ;
     public static String clsNameNameViolatesStarSyntax = "EDU.bmrb.starlibj.NameViolatesStarSyntax" ;
     public static String clsNameOperationCausesMismatchedLoopData = "EDU.bmrb.starlibj.OperationCausesMismatchedLoopData" ;
     public static String clsNameParseException = "EDU.bmrb.starlibj.ParseException" ;
     public static String clsNameParseValFromRetVal = "EDU.bmrb.starlibj.ParseValFromRetVal" ;
     public static String clsNameRemoteInt = "EDU.bmrb.starlibj.RemoteInt" ;
     public static String clsNameSaveFrameNode = "EDU.bmrb.starlibj.SaveFrameNode" ;
     public static String clsNameSaveListVector = "EDU.bmrb.starlibj.SaveListVector" ;
     public static String clsNameStarFileNode = "EDU.bmrb.starlibj.StarFileNode" ;
     public static String clsNameStarListVector = "EDU.bmrb.starlibj.StarListVector" ;
     public static String clsNameStarNode = "EDU.bmrb.starlibj.StarNode" ;
     public static String clsNameStarParser = "EDU.bmrb.starlibj.StarParser" ;
     public static String clsNameStarParserConstants = "EDU.bmrb.starlibj.StarParserConstants" ;
     public static String clsNameStarParserTokenManager = "EDU.bmrb.starlibj.StarParserTokenManager" ;
     public static String clsNameStarUnparser = "EDU.bmrb.starlibj.StarUnparser" ;
     public static String clsNameStarValidity = "EDU.bmrb.starlibj.StarValidity" ;
     public static String clsNameStarVectorLike = "EDU.bmrb.starlibj.StarVectorLike" ;
     public static String clsNameTagsVector = "EDU.bmrb.starlibj.TagsVector" ;
     public static String clsNameTinyAbsDataValueNode = "EDU.bmrb.starlibj.TinyAbsDataValueNode" ;
     public static String clsNameTinyRelDataValueNode = "EDU.bmrb.starlibj.TinyRelDataValueNode" ;
     public static String clsNameToken = "EDU.bmrb.starlibj.Token" ;
     public static String clsNameTokenMgrError = "EDU.bmrb.starlibj.TokenMgrError" ;
     public static String clsNameTypesAreFrozen = "EDU.bmrb.starlibj.TypesAreFrozen" ;
     public static String clsNameTypesNotFrozenYet = "EDU.bmrb.starlibj.TypesNotFrozenYet" ;
     public static String clsNameVectorCheckType = "EDU.bmrb.starlibj.VectorCheckType" ;
     public static String clsNameWrongElementType = "EDU.bmrb.starlibj.WrongElementType" ;
}


package Types;

import java.io.*;
import Operators.*;
import Expressions.*;

public class StringDesc implements TypeDesc {
    static final String name = "string";

    public String getString( ) {
        return name;
    }

    public EvalOperator getOperator( String opStr, TypeDesc arg )
	throws TypeCheckException
    {
	String msg1 = "\n  This operation is not supported by our current version:"; 
        String msg2 = "\n\t  string " + opStr + " " + arg.getString( );
        String msg3 = "\n  The following operations on strings are allowed:";
	String msg4 = "\n\t  string [op] string,";
	String msg5 = "\n  where [op] could be ==, !=, <, <=, >, >=.\n\n";

        if ( ! arg.getString( ).equals(name) )
	    throw new TypeCheckException(msg1+msg2+msg3+msg4+msg5); 

        if ( opStr.equals("==") ) 
            return new StringEqualOp( );

        if ( opStr.equals("!=") ) 
            return new StringNotEqualOp( );

        if ( opStr.equals("<") ) 
            return new StringLessOp( );

        if ( opStr.equals(">") ) 
            return new StringGreaterOp( );

        if ( opStr.equals("<=") ) 
            return new StringLeqOp( );

        if ( opStr.equals(">=") ) 
            return new StringGeqOp( );

	throw new TypeCheckException(msg1+msg2+msg3+msg4+msg5); 
    }
}



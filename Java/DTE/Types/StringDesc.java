package Types;

import java.io.*;
import Operators.*;

public class StringDesc implements TypeDesc {
    static final String name = "string";

    public String getString( ) {
        return name;
    }

    public EvalOperator getOperator( String opStr, TypeDesc arg )
	throws IllegalArgumentException
    {
        if ( ! arg.getString( ).equals(name) )
	    throw new IllegalArgumentException("Incompatible type!\n"); 

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

	throw new IllegalArgumentException("Incompatible type!\n"); 
    }
}



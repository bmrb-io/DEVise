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

/*
        if ( opStr.equals("!=") ) 
            return new IntNotEqualOp( );

        if ( opStr.equals(">") ) 
            return new IntGreaterOp( );

        if ( opStr.equals("<") ) 
            return new IntLessOp( );

        if ( opStr.equals(">=") ) 
            return new IntGeqOp( );

        if ( opStr.equals("<=") ) 
            return new IntLeqOp( );

*/
	throw new IllegalArgumentException("Incompatible type!\n"); 
    }
}



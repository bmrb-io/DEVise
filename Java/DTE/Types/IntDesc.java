package Types;

import java.io.*;
import Operators.*;

public class IntDesc implements TypeDesc {
    static final String name = "int";

    public IntDesc( ){
    }

    public String getString( ) {
        return name;
    }

    public EvalOperator getOperator( String opStr, TypeDesc arg )
	throws IllegalArgumentException
    {
        if ( ! arg.getString( ).equals(name) )
	    throw new IllegalArgumentException("give me an int!\n"); 

        if ( opStr.equals("==") ) 
            return new IntEqualOp( );

        if ( opStr.equals("!=") ) 
            return new IntNotEqualOp( );
/*
        if ( opStr.equals(">") ) 
            return new IntGreaterOp( );

        if ( opStr.equals("<") ) 
            return new IntLessOp( );

        if ( opStr.equals(">=") ) 
            return new IntGeqOp( );

        if ( opStr.equals("<=") ) 
            return new IntLeqOp( );

        if ( opStr.equals("+") ) 
            return new IntAddOp( );

        if ( opStr.equals("-") ) 
            return new IntSubOp( );

        if ( opStr.equals("*") ) 
            return new IntMulOp( );

*/
        if ( opStr.equals("/") ) 
            return new IntDivOp( );

	throw new IllegalArgumentException("give me an int!\n"); 
    }
}



package Types;

import java.io.*;
import Operators.*;

public class DoubleDesc implements TypeDesc {
    static final String name = "double";

    public String getString( ) {
        return name;
    }

    public EvalOperator getOperator( String opStr, TypeDesc arg )
	throws IllegalArgumentException
    {
        if ( arg.getString( ).equals(name) )
	    throw new IllegalArgumentException("give me an double!\n"); 

        if ( opStr.equals("==") ) 
            return new DoubleEqualOp( );

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

        if ( opStr.equals("+") ) 
            return new IntAddOp( );

        if ( opStr.equals("-") ) 
            return new IntSubOp( );

        if ( opStr.equals("*") ) 
            return new IntMulOp( );
*/

        if ( opStr.equals("/") ) 
            return new DoubleDivOp( );

	throw new IllegalArgumentException("give me an int!\n"); 
    }
}



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
        if ( ! arg.getString( ).equals(name) )
	    throw new IllegalArgumentException("Type incompatible!\n"); 

        if ( opStr.equals("==") ) 
            return new DoubleEqualOp( );

        if ( opStr.equals("!=") ) 
            return new DoubleNotEqualOp( );

        if ( opStr.equals(">") ) 
            return new DoubleGreaterOp( );

        if ( opStr.equals("<") ) 
            return new DoubleLessOp( );

        if ( opStr.equals(">=") ) 
            return new DoubleGeqOp( );

        if ( opStr.equals("<=") ) 
            return new DoubleLeqOp( );

        if ( opStr.equals("+") ) 
            return new DoubleAddOp( );

        if ( opStr.equals("-") ) 
            return new DoubleSubOp( );

        if ( opStr.equals("*") ) 
            return new DoubleMulOp( );

        if ( opStr.equals("/") ) 
            return new DoubleDivOp( );

	throw new IllegalArgumentException("Type Incompatible!\n"); 
    }
}



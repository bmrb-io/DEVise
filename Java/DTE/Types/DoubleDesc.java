package Types;

import java.io.*;
import Operators.*;
import Expressions.*;

public class DoubleDesc implements TypeDesc {
    static final String name = "double";

    public String getString( ) {
        return name;
    }

    public EvalOperator getOperator( String opStr, TypeDesc arg )
	throws TypeCheckException
    {
	String msg1 = "\n  This operation is not supported by our current version:"; 
        String msg2 = "\n\t  double " + opStr + " " + arg.getString( );
        String msg3 = "\n  The following operations on doubles are allowed:";
	String msg4 = "\n\t  double [op] double,";
	String msg5 = "\n  where [op] could be ==, !=, <, <=, >, >=, +, -, *, /.\n";

        if ( ! arg.getString( ).equals(name) )
	    throw new TypeCheckException(msg1+msg2+msg3+msg4+msg5); 

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

        throw new TypeCheckException(msg1+msg2+msg3+msg4+msg5); 
    }
}



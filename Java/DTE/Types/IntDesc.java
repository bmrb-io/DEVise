package Types;

import java.io.*;
import Operators.*;
import Expressions.*;

public class IntDesc implements TypeDesc {
    static final String name = "int";

    public IntDesc( ){
    }

    public String getString( ) {
        return name;
    }

    public EvalOperator getOperator( String opStr, TypeDesc arg )
	throws TypeCheckException
    {
	String msg1 = "\n  This operation is not supported by our current version:"; 
        String msg2 = "\n\t  int " + opStr + " " + arg.getString( );
        String msg3 = "\n  The following operations on integers are allowed:";
	String msg4 = "\n\t  int [op] int,";
	String msg5 = "\n  where [op] could be =, !=, <, <=, >, >=, +, -, *, /.\n";

        if ( ! arg.getString( ).equals(name) )
	    throw new TypeCheckException(msg1+msg2+msg3+msg4+msg5); 

        if ( opStr.equals("=") ) 
            return new IntEqualOp( );

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

        if ( opStr.equals("/") ) 
            return new IntDivOp( );

	throw new TypeCheckException(msg1+msg2+msg3+msg4+msg5); 
    }
}



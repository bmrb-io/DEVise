package Types;

import java.io.*;
import Operators.*;

public class BooleanDesc implements TypeDesc {
    static final String name = "boolean";

    public BooleanDesc( ) {
    }

    public String getString( ) {
        return name;
    }

    public EvalOperator getOperator( String opStr, TypeDesc arg )
	throws IllegalArgumentException
    {
        if ( ! arg.getString( ).equals(name) )
	    throw new IllegalArgumentException("Type incompatible!"); 

        if ( opStr.equals("&&") ) 
            return new BoolAndOp( );

        if ( opStr.equals("||") ) 
            return new BoolOrOp( );

        throw new IllegalArgumentException("Type incompatible!"); 
    }
}



package Types;

import java.io.*;
import Operators.*;

public class IntDesc implements TypeDesc {
    static final String name = "boolean";

    public String getString( ) {
        return name;
    }

    public EvalOperator getOperator( String opStr, TypeDesc arg )
	throws IllegalArgumentException
    {
        if ( arg.getString( ).equals(name) )
	    throw new IllegalArgumentException("give me an boolean!\n"); 

        if ( opStr.equals("&&") ) 
            return new BoolAndOp( );

        if ( opStr.equals("||") ) 
            return new BoolOrOp( );

	throw new IllegalArgumentException("give me an boolean!\n"); 
    }
}



package Types;

import java.io.*;
import Operators.*;
import Expressions.*;

public class BooleanDesc implements TypeDesc {
    static final String name = "boolean";

    public BooleanDesc( ) {
    }

    public String getString( ) {
        return name;
    }

    public EvalOperator getOperator( String opStr, TypeDesc arg )
	throws TypeCheckException
    {
	String msg1 = "\n  This operation is not supported by our current version:"; 
        String msg2 = "\n\t  boolean " + opStr + " " + arg.getString( );
        String msg3 = "\n  The following operations on booleans are allowed:";
	String msg4 = "\n\t  boolean [op] boolean,";
	String msg5 = "\n  where [op] could be ||, &&.\n";

        if ( ! arg.getString( ).equals(name) )
	    throw new TypeCheckException(msg1+msg2+msg3+msg4+msg5); 

        if ( opStr.equals("&&") ) 
            return new BoolAndOp( );

        if ( opStr.equals("||") ) 
            return new BoolOrOp( );

	throw new TypeCheckException(msg1+msg2+msg3+msg4+msg5); 
    }
}



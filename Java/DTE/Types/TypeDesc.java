package Types;

import java.io.*;
import Operators.*;
import Expressions.*;

/** Type Descriptor is implemented as a String that contains names like
    "int", "double", etc. */

public interface TypeDesc {
    /** Return the name of the type, like "int", "double", ... */
    public String getString( );

    /** Return an EvalOperator object according to the operator 
	and operands. */
    public EvalOperator getOperator( String opStr, TypeDesc arg )
	throws TypeCheckException;
}


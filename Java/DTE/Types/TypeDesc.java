package Types;

import java.io.*;

import Operators.*;

/** Type Descriptor is implemented as a String that contains names like
    "int", "double", etc. */

public interface TypeDesc {
    public String getString( );
    public EvalOperator getOperator( String opStr, TypeDesc arg );
}


package Types;

import java.io.*;
import Operators.*;

public class IntDesc implements TypeDesc {
    static final String name = "int";

    public String getString( ) {
        return name;
    }

    public EvalOperator getOperator( String opStr, TypeDesc arg ) {
        if ( opStr.equals("=") && arg.name.equals(name) ) 
            return new IntEqualOp();
        else{
            // throw exception
        }
    }
}



package Types;

import java.io.*;

/** Type Descriptor is implemented as a String that cointains names like
    "int", "double", etc. */

public interface TypeDesc {
	EvalOperator getOperator(String opStr, TypeDesc arg);
}

public class IntDesc implements TypeDesc {
	public static final String name = "int";
	public EvalOperator getOperator(String opStr, TypeDesc arg){
		if(opStr.equals("=") && arg.name.equals(name)){
			return new IntEqualOp();
		}
		else{
			// throw exception
		}
	}
}



package Types;

import java.io.*;

public class IntDesc implements TypeDesc {
	public String name = "int";
	public ExecOperator getOperator(String opStr, TypeDesc arg){
		if(opStr.equals("=") && arg.name.equals(name)){
			return new IntEqualOp();
		}
		else{
			// throw exception
		}
	}
}

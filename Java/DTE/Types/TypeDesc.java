package Types;

import java.io.*;

/** Type Descriptor is implemented as a String that cointains names like
    "int", "double", etc. */

public class TypeDesc {
	String name;
	public TypeDesc(String typeName){
		name = typeName;
	}
	public String getString(){
		return name;
	}
}



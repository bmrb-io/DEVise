package Types;

import java.io.*;

/** This is a base class for every possible type in DTE */
public abstract class DTE_Type {

	/** This method returns an instance of a type given its descriptor */

	public static DTE_Type typeFor(TypeDesc typeName){
		if(typeName.getString().equals("int")){
			return new DTE_Int();
		}
		else if(typeName.getString().equals("double")){
			return new DTE_Double();
		}

		if(typeName.getString().equals("string")){
			return new DTE_String();
		}

		System.out.println("Unknown type: " + typeName.getString());
		return null;
	}
	abstract public boolean read(StreamTokenizer st) throws IOException;
	abstract public void print(PrintWriter ps);
	abstract public void print(PrintStream ps);
        abstract public Object clone();
  //       abstract public Object get_val();
}

package Types;

import java.io.*;

/** Class Tuple is used throughout the system to store the content of a 
    record. Tuple is implemented as an array of types.
    It is constructed from an array of type descriptors. */
    
public class Tuple implements Cloneable {
	DTE_Type[] types;

        public Tuple(){}

	public Tuple(TypeDesc[] typeStrings){
		types = new DTE_Type[typeStrings.length];
		for(int i = 0; i < typeStrings.length; i++){
			types[i] = DTE_Type.typeFor(typeStrings[i]);
		}
	}

	/** read returns false if the EOF is encountered */

	public boolean read(StreamTokenizer st) throws IOException {
		for(int i = 0; i < types.length; i++){
			if(! types[i].read(st)){
				return false;
			}
		}
		return true;
	}
	public void print(PrintWriter os){
		for(int i = 0; i < types.length; i++){
			types[i].print(os);
		}
		os.println();
		os.flush();
	}

 
	public void print(PrintStream os){
		for(int i = 0; i < types.length; i++){
			types[i].print(os);
		}
		os.println();
		os.flush();
	}
 


        public void get_field(DTE_Type[] t)
        {
	  t = types;
	}
    
        public Object clone()
        {
	  Tuple t = new Tuple();
	  t.types = new DTE_Type[types.length];
	  for(int i = 0; i < types.length; i++)
	    t.types[i] = (DTE_Type)types[i].clone();
	  return t;
	}
}




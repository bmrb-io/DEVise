import java.io.*;

/** Class Tuple is used throughout the system to store the content of a 
    record. Tuple is implemented as an array of types.
    It is constructed from an array of type descriptors. */
    
class Tuple {
	DTE_Type[] types;
	public Tuple(TypeDesc[] typeStrings){
		types = new DTE_Type[typeStrings.length];
		for(int i = 0; i < typeStrings.length; i++){
			types[i] = DTE_Type.typeFor(typeStrings[i]);
		}
	}

	/** read returns false if the EOF is encountered */

	boolean read(StreamTokenizer st) throws IOException {
		for(int i = 0; i < types.length; i++){
			if(! types[i].read(st)){
				return false;
			}
		}
		return true;
	}
	void print(PrintStream os){
		for(int i = 0; i < types.length; i++){
			types[i].print(os);
		}
		os.println();
	}
}

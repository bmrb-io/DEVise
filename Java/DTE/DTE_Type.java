import java.io.*;

/** This is a base class for every possible type in DTE */
class DTE_Type {

	/** This method returns an instance of a type given its descriptor */

	public static DTE_Type typeFor(TypeDesc typeName){
		if(typeName.getString().equals("int")){
			return new DTE_Int();
		}
		System.out.println("Unknown type: " + typeName.getString());
		return null;
	}
	public boolean read(StreamTokenizer st) throws IOException {
		return false;
	}
	public void print(PrintStream ps){
	}
}

class DTE_Int extends DTE_Type {
	public int val;
	public DTE_Int(){
		val = 0;
	}
	public DTE_Int(int i){
		val = i;
	}
	public boolean read(StreamTokenizer st) throws IOException {
		if(st.nextToken() == StreamTokenizer.TT_EOF){
			return false;
		}

		// Need to check that this is really an int.
		// return 0 if this is not an int and print the msg to stderr.
		// use st.lineno()

		val = (int) st.nval;
		return true;
	}
	public void print(PrintStream ps){
		ps.print(val);
	}
}

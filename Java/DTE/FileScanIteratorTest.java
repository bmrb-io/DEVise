import java.io.*;

/** This is a class whose only purpose is to test the FileScanIterator
    It takes a file name and a list of type names, for example:
    test.txt int double */

public class FileScanIteratorTest {
	public static void main(String[] args) throws IOException {

		String fileName = args[0];
		TypeDesc[] typeDescs = new TypeDesc[args.length - 1];
		for(int i = 0; i < args.length - 1; i++){
			typeDescs[i] = new TypeDesc(args[i + 1]);
		}
		FileScanIterator fs = new FileScanIterator(fileName, typeDescs);
		Tuple t;
		while((t = fs.getNext()) != null){
			t.print(System.out);
		}
	}
}

/** FileScanIterator returns the stream of tuples from a ASCII file
    whose name is given in "fileName" and whose content is described
    by the array of type descriptors "types". */

class FileScanIterator {
	Tuple tuple;
	StreamTokenizer st;
	public FileScanIterator(String fileName, TypeDesc[] types) 
		throws FileNotFoundException 
	{
		FileReader file = new FileReader(fileName);
		st = new StreamTokenizer(file);
		tuple = new Tuple(types);
	}

	/** getNext return valid tuple object or null to signal the end of
	iterator */

	Tuple getNext() throws IOException {
		if(tuple.read(st)){
			return tuple;
		}
		else{
			return null;
		}
	}
}

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

/** Type Descriptor is implemented as a String that cointains names like
    "int", "double", etc. */

class TypeDesc {
	String name;
	public TypeDesc(String typeName){
		name = typeName;
	}
	public String getString(){
		return name;
	}
}

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

		val = (int) st.nval;
		return true;
	}
	public void print(PrintStream ps){
		ps.print(val);
	}
}

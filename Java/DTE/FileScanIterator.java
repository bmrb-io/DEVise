import java.io.*;

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


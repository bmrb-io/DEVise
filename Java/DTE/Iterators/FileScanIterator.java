package Iterators;

import Types.*;
import java.io.*;

/** FileScanIterator returns the stream of tuples from a ASCII file
    whose name is given in "fileName" and whose content is described
    by the array of type descriptors "types". */

public class FileScanIterator implements Iterator {
	Tuple tuple;
	StreamTokenizer st;
	public FileScanIterator(String fileName, TypeDesc[] types) 
		throws FileNotFoundException 
	{
		FileReader fr = new FileReader(fileName);
		BufferedReader file = new BufferedReader(fr);
		st = new StreamTokenizer(file);
		tuple = new Tuple(types);
	}

	/** getFirst should be called before getNext */

	public Tuple getFirst() throws IOException {
		return getNext();
	}

	/** getNext returns a valid tuple object or null to signal the end of
	iterator */

	public Tuple getNext() throws IOException {
		if(tuple.read(st)){
			return tuple;
		}
		else{
			return null;
		}
	}
}


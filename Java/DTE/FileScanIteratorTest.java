import java.io.*;
import Iterators.*;
import Types.*;

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

import java.io.*;
import Iterators.*;
import Types.*;
import Expressions.*;


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

		FileWriter fw = new FileWriter("hg.txt");
		BufferedWriter bw = new BufferedWriter(fw);
		PrintWriter pw = new PrintWriter(bw);
		
		Tuple t;
		int cnt = 0;
		while((t = fs.getNext()) != null){
		        t.print(pw);
			t.print(System.out);
			Tuple a = (Tuple)t.clone();
			a.print(pw);
			a.print(System.out);
			cnt++;
		}

		t = fs.getFirst();
		t.print(pw);
		Tuple a = (Tuple)t.clone();
		a.print(pw);
		a.print(System.out);
		cnt++;
		
		while((t = fs.getNext()) != null){
		        t.print(pw);
			t.print(System.out);
			a = (Tuple)t.clone();
			a.print(pw);
			a.print(System.out);
			cnt++;
		}		
			       
		pw.flush();
		pw.close();

	      	System.out.println("read " + cnt + " tuples");

		ExecSelect es = new ExecSelect( 0, 0 );
		SymbolTable st = new SymbolTable( );
	}
}




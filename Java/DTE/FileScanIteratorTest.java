package Iterators;


import java.io.*;
import Iterators.*;
import Types.*;
import Expressions.*;
import Functions.*;


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
			cnt++;
		}
			       
		pw.flush();
		pw.close();

	      	System.out.println("read " + cnt + " tuples");


		ExecExpr[] m1 = new ExecExpr[10];
		ExecExpr[] m2 = new ExecExpr[10];	
		TypeDesc[] ty = new TypeDesc[10];

		NLjoinIterator n = new NLjoinIterator(fs,fs,m1,m2,ty,10,"hh");
		SelProj S = new SelProj(fs,m1,m2,ty);


	}
}

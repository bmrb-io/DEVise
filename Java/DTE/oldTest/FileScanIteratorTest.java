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
	String fileName2 = args[1];
	TypeDesc[] typeDescs = new TypeDesc[3];
	
	/*
	  for(int i = 0; i < args.length - 2; i++){
	  typeDescs[i] = new TypeDesc(args[i + 2]);
	  }
	*/
		
	typeDescs[0] = new IntDesc();
	typeDescs[1] = new DoubleDesc();
	typeDescs[2] = new StringDesc();
	
	
	FileScanIterator fs = new FileScanIterator(fileName, typeDescs);
	FileScanIterator fs2 = new FileScanIterator(fileName2, typeDescs);
	
	FileWriter fw = new FileWriter("hg.txt");
	BufferedWriter bw = new BufferedWriter(fw);
	PrintWriter pw = new PrintWriter(bw);
	
	
	/*		
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
	*/
	
	ExecExpr[] m1 = new ExecExpr[6];
	m1[0] = new ExecSelect(0,0);
	m1[1] = new ExecSelect(0,1);
	m1[2] = new ExecSelect(0,2);
	m1[3] = new ExecSelect(1,0);
	m1[4] = new ExecSelect(1,1);
	m1[5] = new ExecSelect(1,2);
	
	
	ExecExpr[] m2 = m1;
	
	TypeDesc[] ty = new TypeDesc[6];
	ty[0] = new IntDesc();
	ty[1] = new DoubleDesc();
	ty[2] = new StringDesc();
	ty[3] = new IntDesc();
	ty[4] = new DoubleDesc();
	ty[5] = new StringDesc();
	

	NLjoinIterator n = new NLjoinIterator(fs,fs2,m1,m2,ty,typeDescs, typeDescs, 2);
	
		//		SelProj S = new SelProj(fs,m1,m2,ty);
	
	
	Tuple c;
		while((c = n.getNext()) != null)
		    {
			c.print(pw);
		    c.print(System.out);
		    }
    }
}






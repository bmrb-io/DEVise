import java.io.*;
import java.lang.*;
import Types.*;


class testSchema {

  public static void main(String[] args){

    System.out.println("*********Test constructors of Schema****************");

    Schema s1 = new Schema(4, "int sid string sname double gpa int deptid");

    Schema s2 = new Schema(4, "int string double int", "sid sname gpa deptid");

    System.out.println("------Case 1: test constructors with the same input");
    System.out.println("Schema 1:");
    System.out.println("Number of Fields:" + s1.getnumFlds());
   
    System.out.print("Types of Attributes:");
    s1.printTypes();
    System.out.print("Names of Attributes:");
    s1.printAttrnms();

    System.out.print("Output of Schema: ");
    System.out.println( s1.getString());
    System.out.println();

    //*******************************************************
    System.out.println("Schema 2:");
    System.out.println("Number of Fields:" + s2.getnumFlds());
   
    System.out.print("Types of Attributes:");
    s2.printTypes();
    System.out.print("Names of Attributes:");
    s2.printAttrnms();

    System.out.print("Output of Schema: ");
    System.out.println( s2.getString());
    System.out.println();

    s1 = new Schema(2, "int deptid string location");

    s2 = new Schema(3, "string int int", "pname deptid courseid");

    System.out.println("------Case 2: test constructors with the different input");
    System.out.println("Schema 3:");
    System.out.println("Number of Fields:" + s1.getnumFlds());
   
    System.out.print("Types of Attributes:");
    s1.printTypes();
    System.out.print("Names of Attributes:");
    s1.printAttrnms();

    System.out.print("Output of Schema: ");
    System.out.println( s1.getString());
    System.out.println();

    //*******************************************************
    System.out.println("Schema 4:");
    System.out.println("Number of Fields:" + s2.getnumFlds());
   
    System.out.print("Types of Attributes:");
    s2.printTypes();
    System.out.print("Names of Attributes:");
    s2.printAttrnms();

    System.out.print("Output of Schema: ");
    System.out.println( s2.getString());
    System.out.println();

    System.out.println("*********Test function read() of Schema****************");
    FileReader r=null;
    try{
      r = new FileReader("descfile");
    }catch (IOException e){System.out.println("Test failed.");};

    Schema s = new Schema();
    StreamTokenizer p = new StreamTokenizer(r);
    int c =1;
    try{
      while(p.nextToken() != StreamTokenizer.TT_EOF){
	p.pushBack();
	
        try{
	  s.read(p);
	}catch(IOException e){};
	
	System.out.println("Number of Fields: " + s.getnumFlds());
	System.out.print("AttrTypes of Schema[" + c + "]:");
	s.printTypes();
        System.out.print("AttrNames of Schema[" + c + "]:");
	s.printAttrnms();
	
	System.out.print("Output of Schema[" + c + "]: ");
	System.out.println( s.getString());
	System.out.println();
	c++;
      }
    }catch(IOException e){System.out.println("Test failed");};

    System.out.println();
    System.out.println("Test Ends Successfully......");
  }

}

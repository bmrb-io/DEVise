package Types;

import java.io.*;
import java.util.*;
import java.lang.*;

public class Schema {

  TypeDesc[] typeIDs;
  String[] attributeNames;

  public Schema(){}

  public TypeDesc[] getTypeDescs(){
    return typeIDs;
  }

  public Schema(int nf,  String str ) {
    if( str != null ){

      StringTokenizer st = new StringTokenizer( str );
      int numFlds = nf;

      typeIDs = new TypeDesc[numFlds];
      attributeNames = new String[ numFlds ];

      String s;
      while( st.hasMoreTokens() ){
	for(int i=0; i<numFlds; i++){
          s = st.nextToken();
          if( s.equals("int"))
	    typeIDs[i] = new IntDesc();
          else if(s.equals("double"))
	    typeIDs[i] = new DoubleDesc();
          else if(s.equals("string"))
	    typeIDs[i] = new StringDesc();
          else if(s.equals("boolean"))
	    typeIDs[i] = new BooleanDesc();
	  else
	    typeIDs[i] = null;

          
	  attributeNames[i] = st.nextToken();
	}
      }
    }
  }

  public Schema(int numFlds, String types, String attrnms){
    if( types != null && attrnms != null){
      
      StringTokenizer st1 = new StringTokenizer( types);
      StringTokenizer st2 = new StringTokenizer( attrnms);

      if( st1.countTokens() == numFlds && st2.countTokens() == numFlds){

	typeIDs = new TypeDesc[ numFlds ];
	attributeNames = new String[ numFlds ];

        String s;
	for(int i=0; i< numFlds; i++){
          s = st1.nextToken();
	  if( s.equals("int"))
	    typeIDs[i] = new IntDesc();
          else if(s.equals("double"))
	    typeIDs[i] = new DoubleDesc();
          else if(s.equals("string"))
	    typeIDs[i] = new StringDesc();
          else if(s.equals("boolean"))
	    typeIDs[i] = new BooleanDesc();
	  else
	    typeIDs[i] = null;
	  
 
	  attributeNames[i] = st2.nextToken();
	}
      }
    }
  }
 


  public boolean read( StreamTokenizer st ) throws IOException {
    
    int count=0;
 
    if(st.nextToken() == StreamTokenizer.TT_EOF){
      return false;
    }
    
    while( (st.ttype != StreamTokenizer.TT_NUMBER)&&
	 (st.ttype != StreamTokenizer.TT_EOF) ){
      st.nextToken();
    }
  
    if( st.ttype == StreamTokenizer.TT_NUMBER ){
      typeIDs = new TypeDesc[(int)st.nval];
      attributeNames = new String[(int)st.nval];
      
      for(count=0; count<st.nval; count++){
	if(st.nextToken() == StreamTokenizer.TT_WORD){
          if( st.sval.equals("int"))
	    typeIDs[count] = new IntDesc();
          else if(st.sval.equals("double"))
	    typeIDs[count] = new DoubleDesc();
          else if(st.sval.equals("string"))
	    typeIDs[count] = new StringDesc();
          else if(st.sval.equals("boolean"))
	    typeIDs[count] = new BooleanDesc();
	  else
	    typeIDs[count] = null;
	} 
	 
	else throw new IOException("Misplaced, ATTRTYPE expected");
	
	if(st.nextToken() == StreamTokenizer.TT_WORD)
	  attributeNames[count] = new String(st.sval);
	else throw new IOException("Misplaced, ATTRNAME expeted");
      }
      return true;
      
    }
    return false;
  }
  

  public int getnumFlds() {
    if( typeIDs.length == attributeNames.length)
      return typeIDs.length;
    else 
      return 0;
  }

  public String getString(){
    String str = new String( );
    str += typeIDs.length;
  
    for(int i=0; i< typeIDs.length; i++){
      str += " ";
      str += typeIDs[i].getString();
      str += " ";
      str += attributeNames[i];
    }
  
    return str;  
  }
  
 public String getTypeAttr(){
    String str = new String();
    for(int i=0; i<typeIDs.length; i++){
      str += typeIDs[i].getString() + " ";
      str += attributeNames[i] + " ";
    }
    return str;
  }
    

  public void printTypes(){
    String str = new String();
    for(int i=0; i<typeIDs.length; i++){
      str += " ";
      str += typeIDs[i].getString();
    }
    System.out.println( str);
  }

  
  public void printAttrnms(){
    String str = new String();
    for(int i=0; i<attributeNames.length; i++){
      str += " ";
      str += attributeNames[i];
    }
    System.out.println( str);
  }


  public static void main(String[] str){

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

    //****************************************************************************
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

    //****************************************************************************
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
	}catch(IOException e){
	System.out.println("Test of read() function failed...");};
	
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






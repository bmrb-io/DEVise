package Iterators; 

import java.io.*;
import java.util.*;
import java.lang.*;
import Types.*;
import Expressions.*;


public class NLjoinIterator implements Iterator 
{
  Iterator left;
  Iterator right;
  ExecExpr[] mySelect;
  ExecExpr[] myWhere;
  Tuple next;
  boolean firstPass;
  boolean firstEntry;
  Tuple outerTup;
  Tuple innerTup;

  FileWriter fw;
  BufferedWriter bw;
  PrintWriter pw;

  MemoryLoader outerRel;
  FileScanIterator innerRel;
  
  File f;
  String temp_file;
  
  static Stack stack = new Stack();
 
  static
  {
    for(int i = 0; i < 100; i++)
      stack.push("tempfileforNljoin"+i);
  }


  public NLjoinIterator(Iterator left1, Iterator right1, ExecExpr[] mySelect1, 
		ExecExpr[] myWhere1,TypeDesc[] typeStrings, 
			TypeDesc[] typeStrings1, TypeDesc[] typeStrings2,
			int bucketsize)
       throws IOException
  {
      left       = left1;
      right      = right1;
      
      mySelect   = mySelect1;
      myWhere    = myWhere1;

      firstPass  = true;
      firstEntry = true;

      outerTup   = null;
      innerTup   = null;
      next       = new Tuple(typeStrings);
      
      
      try
	  {
	      temp_file = (String)stack.pop();
	  }
      
      catch(EmptyStackException ex)
	  {
	      System.out.println("There can't be more than 100 temporary files for NLjoin.");
	      System.exit(-1);
	  } 
      
      
      fw = new FileWriter(temp_file);
      bw = new BufferedWriter(fw);
      pw = new PrintWriter(bw);
      
      innerRel = new FileScanIterator(temp_file, typeStrings2);
      outerRel = new MemoryLoader(bucketsize, left, typeStrings1);
      
      f = new File(temp_file);
  }



  public Tuple getFirst() throws IOException
  {
    return getNext();
  }


  public Tuple getNext() throws IOException
  {
    boolean cond = false;

    if(firstEntry)
      {
	outerRel.load();
	innerTup = right.getNext();
	if(innerTup != null)
	  innerTup.print(pw);
	firstEntry = false;
      }

    while(cond == false)
      {
	if(firstPass)
	  {
	    if((outerTup = outerRel.getNext())==null)
	      {
		outerTup = outerRel.getFirst();
			
		if((innerTup = right.getNext())!=null)
		  innerTup.print(pw);
		
		else
		  {
		    firstPass = false;

		    pw.flush();
		    bw.flush();
		    fw.flush();
		    pw.close();

		    innerTup = innerRel.getFirst();
		    
		    outerRel.load();
		    outerTup = outerRel.getFirst();
		  }
	      }
	  }

	else
	  {
	    if((outerTup = outerRel.getNext())==null)
	      {
		outerTup = outerRel.getFirst();
		if( (innerTup = innerRel.getNext())==null )
		  {
		    innerTup = innerRel.getFirst();
		    outerRel.load();
		    outerTup = outerRel.getFirst();
		  }
	      }
	  }


	if( outerTup==null || innerTup==null )
	 {
	   stack.push(temp_file);
	   f.delete();
	   return null;
	 }

	cond = ListOp.evaluateList(myWhere, outerTup, innerTup);
      }

    ListOp.tupleFromList(next, mySelect, outerTup, innerTup);
    return next;
  }
}

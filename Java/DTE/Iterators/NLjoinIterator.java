package Iterators; 

import java.io.*;
import java.util.*;
import java.lang.*;
import Types.*;
import Expressions.*;

/** This class works for Block-Nested loop join. */

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


  /** Argument:*/
  /**          left1:  iterator on outer relation. */
  /**          right1: iterator on outer relation. */
  /**          mySelect1: ExecExpr array, to test if the tuple satisfy the join condition. */
  /**     myWhere1:  ExecExpr array, after testing the tuple belong to the result, use this to pick out fields in returned tuple, so the number of elements of this array should be the same as that of the returned tuple.*/
  /**     typeStrings: TypeDesc array, specify the type of attributes of the returned tuple.*/
  /**     typeStrings1: TypeDesc array, specify the type of attributes of the outer relation tuple.*/
  /**     typeStrings2: TypeDesc array, specify the type of attributes of the inner relation tuple.*/
  /**     bucketsize: integer, specify how many outer relation tuples can be stored in memory.*/

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
	      //	      System.out.println(temp_file+"is allocated");
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


  /** Simply return the next tuple. Call getNext()*/  
  public Tuple getFirst() throws IOException
  {
    return getNext();
  }

  /** Return the next tuple that satisfy the join condition. Return null if no tuple left.*/
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
	   if(temp_file != null)
	     {
	       stack.push(temp_file);
	       f.delete();
	       temp_file = null;
	     }
	   
	   //	       System.out.println(temp_file+"is deleted.");
	  
	   return null;
	 }

	cond = ListOp.evaluateList(myWhere, outerTup, innerTup);
      }

    ListOp.tupleFromList(next, mySelect, outerTup, innerTup);
    return next;
  }
}

package Iterators; 

import java.io.*;
import Types.*;
import Functions.*;
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
  //  Inserter inserter;

  String temp_file;

  public NLjoinIterator(Iterator left1, Iterator right1, ExecExpr[] mySelect1, 
		ExecExpr[] myWhere1, TypeDesc[] typeStrings, int bucketsize, 
		String temp_file1) throws IOException
    {
      left       = left1;
      right      = right;

      mySelect   = mySelect1;
      myWhere    = myWhere1;

      firstPass  = true;
      firstEntry = true;

      outerTup   = null;
      innerTup   = null;
      next       = new Tuple(typeStrings);

      temp_file = temp_file1;

      fw = new FileWriter(temp_file);
      bw = new BufferedWriter(fw);
      pw = new PrintWriter(bw);
      //      inserter = new Inserter(pw);

      innerRel = new FileScanIterator( temp_file, typeStrings);
      outerRel   = new MemoryLoader(bucketsize, left, typeStrings);      
    }


  //  public ~NLJoin(){}

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
		  //		  inserter.write(innerTup);
		
		else
		  {
		    firstPass = false;
		    //		    innerRel.end_write();
		    pw.flush();
		    pw.close();
		    bw.flush();
		    bw.close();
		    fw.flush();
		    fw.close();
		    
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
	  return null;
       
	cond = ListOp.evaluateList(myWhere, outerTup, innerTup);
      }

    ListOp.tupleFromList(next, mySelect, outerTup, innerTup);
    return next;
  }
}






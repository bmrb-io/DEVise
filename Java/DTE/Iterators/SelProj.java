package Iterators; 


import java.io.*;
import Types.*;
import Expressions.*;


public class SelProj implements Iterator 
{
  Iterator     inputIt;
  ExecExpr[]   mySelect;
  ExecExpr[]   myWhere;
  Tuple        next;

  public SelProj(Iterator inputit, ExecExpr[] myselect, 
		     ExecExpr[] mywhere, TypeDesc[] typeStrings ) 
  {
    inputIt  = inputit;
    mySelect = myselect;
    myWhere  = mywhere; 
    next = new Tuple(typeStrings);
  }


  public Tuple getNext() throws IOException
  {
    boolean cond = false;
    Tuple input = new Tuple();

    while(!cond)
      {
	input = inputIt.getNext();
	if(input==null)
	  return null;
	
	cond = ListOp.evaluateList(myWhere, input, null);
      }
    
    ListOp.tupleFromList(next, mySelect, input, null);
    return next;
  }
  

  public Tuple getFirst() throws IOException
  {
    return getNext();
  }
}

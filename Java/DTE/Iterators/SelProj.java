package Iterators; 


import java.io.*;
import Types.*;
import Functions.*;
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
    
    /* error-checking NULL*/

    next = new Tuple(typeStrings);
    /* error-checking NULL*/
  }

  //  public ~SelProjExec(){}

  public Tuple getNext() throws IOException
  {
    boolean cond = false;
    TypeDesc[] t = new TypeDesc[1];
    Tuple input = new Tuple(t);

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


package Iterators; 


import java.io.*;
import Types.*;
import Expressions.*;

/** The SelProj class works for the selection and projection on a single relation.*/

public class SelProj implements Iterator 
{
  Iterator     inputIt;
  ExecExpr[]   mySelect;
  ExecExpr[]   myWhere;
  Tuple        next;

  /** Argument:*/ 
  /**          inputit:  iterator on input relation. */
  /**          myselect: ExecExpr array, to test if the tuple satisfy the join condition.*/
  /**          mywhere:  ExecExpr array, after testing the tuple belong to the result, use this to pick out fields in returned tuple, so the number of elements of this array should be the same as that of the returned tuple.*/
  /**     typeStrings: TypeDesc array, specify the type of attributes of the returned tuple.*/
 	       
  public SelProj(Iterator inputit, ExecExpr[] myselect, 
		     ExecExpr[] mywhere, TypeDesc[] typeStrings ) 
  {
    inputIt  = inputit;
    mySelect = myselect;
    myWhere  = mywhere; 
    next = new Tuple(typeStrings);
  }


/** Return the next tuple that satisfy the selection or projection condition. Return null if no tuple left.*/
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
  

  /** Simply return the next tuple. Call getNext()*/  
  public Tuple getFirst() throws IOException
  {
    return getNext();
  }
}

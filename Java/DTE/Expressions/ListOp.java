package Expressions;

import Types.*;


/** This class is used to check whether the retrieved tuples satisfy the join or selection projection condition. If so, it can produce the return tuple.*/ 

public class ListOp
{
  /** This function is used to check whether the retrieved tuples satisfy the join or selection projection condition according to the condition in the ExecExpr[] list.*/
  
  public static boolean evaluateList(ExecExpr[] list, Tuple left, Tuple right)
  {
    for(int i = 0; i < list.length; i++)
      {
	if(!(((DTE_Boolean)(list[i].evaluate(left, right)))).getValue())
	  return false;
      }
    return true;
  }
  

  /** This function is used to produce the return tuple according to the condition in the ExecExpr[] list.*/
 
  public static void tupleFromList(Tuple retVal, ExecExpr[] list,
			    Tuple left, Tuple right)
  {
    DTE_Type[] t = retVal.get_fields();

    for(int i = 0; i < list.length; i++)
	(list[i].evaluate(left, right)).copyTo(t[i]);
  }
}


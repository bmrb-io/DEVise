package Expressions;

import Types.*;


public class ListOp
{
  public static boolean evaluateList(ExecExpr[] list, Tuple left, Tuple right)
  {
    for(int i = 0; i < list.length; i++)
      {
	if(list[i].evaluate(left, right) == null)
	  return false;
      }
    return true;
  }
  

  public static void tupleFromList(Tuple retVal, ExecExpr[] list,
			    Tuple left, Tuple right)
  {
    DTE_Type[] t = retVal.get_fields();

    for(int i = 0; i < list.length; i++)
      t[i] = (DTE_Type)(list[i].evaluate(left, right)).clone();
  }
}


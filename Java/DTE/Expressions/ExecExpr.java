package Expressions;

import Types.*;

/** ExecExpr is created in the process of creating the executable structure
    from the best plan produced by the optimizer.
*/ 

public interface ExecExpr {

	/** ExecExpr always evaluates to a single types. However the
	input can be two tuples (if this expression is evaluated
	in a join method), or it can be only one tuple if this 
	expression is evaluated as a part of a unary operator,
	such as file-scan. */
 
	public void evaluate(Tuple left, Tuple right, DTE_Type result);
}

package Expressions;

import Types.*;
import Operators.*;

/** ExecOperator is created for evaluating an operator. */

public class ExecOperator implements ExecExpr{
    EvalOperator evalOp;
    ExecExpr left;
    ExecExpr right;
    DTE_Type value;

    /** Constructor with arguments: an EvalOperator and two 
	ExecExpr objects. */
    public ExecOperator( EvalOperator op, ExecExpr l, ExecExpr r ){
	left = l;
	right = r;
	evalOp = op;
	value = DTE_Type.typeFor( evalOp.getType( ) );
    }

    /** Evaluate and return a DTE_Type. */
    public DTE_Type evaluate(Tuple leftT, Tuple rightT) {
	DTE_Type arg1 = left.evaluate( leftT, rightT );
	DTE_Type arg2 = right.evaluate( leftT, rightT );

	evalOp.evaluate( arg1, arg2, value );
	return value;
    }
}

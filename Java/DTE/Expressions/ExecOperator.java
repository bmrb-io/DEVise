package Expressions;

import Types.*;
import Operators.*;

public class ExecOperator implements ExecExpr{
    EvalOperator evalOp;
    ExecExpr left;
    ExecExpr right;

    public ExecOperator( EvalOperator op, ExecExpr l, ExecExpr r ){
	left = l;
	right = r;
	evalOp = op;
    }

    public void evaluate(Tuple leftT, Tuple rightT, DTE_Type result) {
	DTE_Type arg1 = null;
        left.evaluate( leftT, rightT, arg1 );
	DTE_Type arg2 = null;
        right.evaluate( leftT, rightT, arg2 );

	evalOp.evaluate( arg1, arg2, result );
    }
}

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

    public DTE_Type evaluate(Tuple leftT, Tuple rightT) {
	DTE_Type arg1 = left.evaluate( leftT, rightT );
	DTE_Type arg2 = right.evaluate( leftT, rightT );

	DTE_Type result = DTE_Type.typeFor( evalOp.getType( ) );
	evalOp.evaluate( arg1, arg2, result );
	return result;
    }
}

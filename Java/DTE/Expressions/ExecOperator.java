package Expressions;

import Types.*;
import Operators.*;

public class ExecOperator implements ExecExpr{
    EvalOperator evalOp;
    ExecExpr left;
    ExecExpr right;
    DTE_Type value;

    public ExecOperator( EvalOperator op, ExecExpr l, ExecExpr r ){
	left = l;
	right = r;
	evalOp = op;
	value = DTE_Type.typeFor( evalOp.getType( ) );
    }

    public DTE_Type evaluate(Tuple leftT, Tuple rightT) {
	DTE_Type arg1 = left.evaluate( leftT, rightT );
	DTE_Type arg2 = right.evaluate( leftT, rightT );

	evalOp.evaluate( arg1, arg2, value );
	return value;
    }
}

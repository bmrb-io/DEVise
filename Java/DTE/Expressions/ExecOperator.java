package Expressions;

import Types.*;

public class ExecOperator implements ExecExpr{
	ExecExpr left;
	ExecExpr right;
     	String operator;
	DTE_Type value;

	public ExecOperator(ExecExpr l, ExecExpr r, String op){
		left = l;
		right = r;
		operator = op;
	}

	public DTE_Type evaluate(Tuple leftT, Tuple rightT) {
		DTE_Type arg1 = left.evaluate(leftT,  rightT);
		DTE_Type arg2 = right.evaluate(leftT, rightT);
		return add(arg1, arg2);
	}

	public DTE_Type add( DTE_Type arg1, DTE_Type arg2){
		return new DTE_Int( arg1.val + arg2.val);
	}
};


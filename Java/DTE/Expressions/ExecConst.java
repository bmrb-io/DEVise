package Expressions;

import Types.*;

public class ExecConst implements ExecExpr {
	DTE_Type value;

	public ExecConst(DTE_Type value){
		this.value = value;
	}

     	DTE_Type evaluate(Tuple left, Tuple right){
		return value;
	}
};


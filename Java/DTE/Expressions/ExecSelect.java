package Expressions;

import Types.*;

public class ExecSelect implements ExecExpr {
     	private int leftRight;
     	private int fieldNo;

     	public ExecSelect(int lr, int field){
		leftRight = lr; 
		fieldNo = field;
	}

     	public DTE_Type evaluate(Tuple left, Tuple right) {
		if(leftRight == 0)
			return left[fieldNo];
		if(leftRight == 1 )	
			return right[fieldNo];
		// else ...
     	}
};

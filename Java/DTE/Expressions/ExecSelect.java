package Expressions;

import Types.*;

public class ExecSelect implements ExecExpr {
     	private int leftRight;
     	private int fieldNo;

     	public ExecSelect(int lr, int field){
		leftRight = lr; 
		fieldNo = field;
	}

     	public void evaluate(Tuple left, Tuple right, DTE_Type result) {
		DTE_Type[] fields = { };

		if(leftRight == 0)
			fields = left.get_fields( );
		else 
			fields = right.get_fields( );

		result = fields[ fieldNo ];
     	}
}


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
		DTE_Type[] fields = { };

		if(leftRight == 0)
			fields = left.get_fields( );
		else if(leftRight == 1 )	
			fields = right.get_fields( );
		// else ...

		return fields[ fieldNo ];
     	}
};


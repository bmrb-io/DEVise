package Operators;

import Types.*;

public class IntEqualOp implements EvalOperator {
    private static final BooleanDesc type = new BooleanDesc( );

    public TypeDesc getType( ) {
	return type;
    }

    public void evaluate(DTE_Type left, DTE_Type right, DTE_Type result){
        int l = ((DTE_Int) left).getValue();
        int r = ((DTE_Int) right).getValue();
        ((DTE_Boolean)result).setValue(l == r);
    }
}

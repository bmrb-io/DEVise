package Operators;

import Types.*;

public class DoubleEqualOp implements EvalOperator {
    public TypeDesc getType( ) {
	return new BooleanDesc( );
    }

    public void evaluate(DTE_Type left, DTE_Type right, DTE_Type result){
        double l = ((DTE_Double)left).getValue();
	double r = ((DTE_Double)right).getValue();
        ((DTE_Boolean)result).setValue(l == r);
    }
}

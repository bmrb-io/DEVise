package Operators;

import Types.*;

public class DoubleMulOp implements EvalOperator {
    public TypeDesc getType( ) {
	return new DoubleDesc( );
    }

    public void evaluate(DTE_Type left, DTE_Type right, DTE_Type result){
        double l = ( (DTE_Double)left ).getValue( );
        double r = ( (DTE_Double)right ).getValue( );
        ((DTE_Double)result).setValue( l * r );
    }
}

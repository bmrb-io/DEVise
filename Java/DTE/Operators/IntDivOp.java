package Operators;

import Types.*;

public class IntDivOp implements EvalOperator {
    public TypeDesc getType( ) {
	return new DoubleDesc( );
    }

    public void evaluate(DTE_Type left, DTE_Type right, DTE_Type result){
        double l = ( (DTE_Int)left ).getValue( );
        double r = ( (DTE_Int)right ).getValue( );
        ((DTE_Double)result).setValue( l / r );
    }
}

package Operators;

import Types.*;

public class BoolAndOp implements EvalOperator {
    public TypeDesc getType( ) {
	return new BooleanDesc( );
    }

    public void evaluate(DTE_Type left, DTE_Type right, DTE_Type result){
        boolean l = ( (DTE_Boolean)left ).getValue( );
	boolean r = ( (DTE_Boolean)right ).getValue( );
        ( (DTE_Boolean)result ).setValue( l && r );
    }
}

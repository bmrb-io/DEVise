package Operators;

import Types.*;

public class IntSubOp implements EvalOperator {
    public TypeDesc getType( ) {
	return new IntDesc( );
    }

    public void evaluate(DTE_Type left, DTE_Type right, DTE_Type result){
        int l = ( (DTE_Int)left ).getValue( );
        int r = ( (DTE_Int)right ).getValue( );
        ((DTE_Int)result).setValue( l - r );
    }
}

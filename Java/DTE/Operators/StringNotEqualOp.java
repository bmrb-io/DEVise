package Operators;

import Types.*;

public class StringNotEqualOp implements EvalOperator {
    public TypeDesc getType( ) {
	return new BooleanDesc( );
    }

    public void evaluate(DTE_Type left, DTE_Type right, DTE_Type result){
	boolean b = ((DTE_String)left).equals( (DTE_String)right );
        ((DTE_Boolean)result).setValue( !b );
    }
}

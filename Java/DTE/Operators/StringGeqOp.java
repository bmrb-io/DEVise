package Operators;

import Types.*;

public class StringGeqOp implements EvalOperator {
    public TypeDesc getType( ) {
	return new BooleanDesc( );
    }

    public void evaluate(DTE_Type left, DTE_Type right, DTE_Type result){
	int i = ((DTE_String)left).compares( (DTE_String)right );
        ((DTE_Boolean)result).setValue( i>=0 );
    }
}

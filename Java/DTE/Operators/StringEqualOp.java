package Operators;

import Types.*;

public class StringEqualOp implements EvalOperator {
    public TypeDesc getType( ) {
	return new BooleanDesc( );
    }

    public void evaluate(DTE_Type left, DTE_Type right, DTE_Type result){
        String l = ( (DTE_String)left ).getValue( );
	String r = ( (DTE_String)right ).getValue( );
        ((DTE_Boolean)result).setValue( l.equals( r ) );
    }
}

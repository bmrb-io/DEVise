package Operators;

import java.io.*;

public class IntEqualOp implements EvalOperator {
    public TypeDesc getType( ) {
	return new DTE_Boolean( );
    }

    public void evaluate(DTE_Type left, DTE_Type right, DTE_Type result){
        int l = ((DTE_Int) left).getValue();
        int r = ((DTE_Int) right).getValue();
        ((DTE_Bool)result).setValue(l == r);
    }
}

package Operators;

public class IntEqualOp implements EvalOperator {
    public TypeDesc getType( ) {
	return new BooleanDesc( );
    }

    public void evaluate(DTE_Type left, DTE_Type right, DTE_Type result){
        int l = ((DTE_Int) left).getValue();
        int r = ((DTE_Int) right).getValue();
        ((DTE_Boolean)result).setValue(l == r);
    }
}

package Operators;

import Types.*;

public interface EvalOperator {
    public TypeDesc getType( );
    public void evaluate(DTE_Type left, DTE_Type right, DTE_Type result);
}

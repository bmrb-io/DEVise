package Operators;

import Types.*;

/** EvalOperator is used for Operator evaluation. For Example, if the 
    Operator is "int + int", we create an IntAddOp object which implements
    EvalOperator. */
public interface EvalOperator {
    public TypeDesc getType( );
    public void evaluate(DTE_Type left, DTE_Type right, DTE_Type result);
}

package Expressions;

import Types.*;

public class ExecConst implements ExecExpr {
    DTE_Type value;

    /** Constructor with argument: DTE_Type. */
    public ExecConst(DTE_Type value) {
        this.value = value;
    }

    /** Evaluate and return a DTE_Type. */
    public DTE_Type evaluate(Tuple left, Tuple right){
        return value;
    }
}


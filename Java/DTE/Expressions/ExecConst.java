package Expressions;

import Types.*;

public class ExecConst implements ExecExpr {
    DTE_Type value;

    public ExecConst(DTE_Type value) {
        this.value = value;
    }

    public void evaluate(Tuple left, Tuple right, DTE_Type result){
        result = value;
    }
}


package Expressions;

import Types.*;
import Operators.*;

/** class Operator is used to represent all the binary operators.
    It takes the text of the operator, like "+" and two inputs. */

public class Operator implements Expression {
    private String operator;
    private TypeDesc type;
    private Expression left;
    private Expression right;
    private EvalOperator evalOp;

    public Operator(String op, Expression l, Expression r){
        operator = op;
        left=l;
        right=r;
	type = null;
        evalOp = null;
    }

    public String toString( ) {
        return left.toString() +" "+ operator +" "+ right.toString();
    }

    public TypeDesc getTypeDesc( ) {
        return type;
    }

    public Expression typeCheck( SymbolTable st ){
        String strRep = this.toString();
        if ( st.containsKey( strRep ) )
            return st.get(strRep);

        left = left.typeCheck( st );
        right = right.typeCheck( st );

        type = left.getType();
        evalOp = type.getOperator( operator, right.getType() );	
        type = evalOp.getType();

        st.put( this );
        return this;
    }

    public ExecExpr createExec( OptNode[] opn ) {
        ExecExpr l = left.createExec( opn ); 
        ExecExpr r = right.createExec( opn );
        return new ExecOperator( evalOp, l, r );
    }
}

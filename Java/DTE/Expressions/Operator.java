package Expressions;

import Types.*;
import java.util.*;
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

    public boolean equals(Object obj){
        if(!getClass().equals(obj.getClass())){
            return false;
        }
        Operator opObj = (Operator) obj;
        return operator.equals(opObj.operator) &&
            left.equals(opObj.left) && right.equals(opObj.right);
    }

    public String toString( ) {
        return "(" + left.toString() + " " + operator + " " + 
        right.toString() + ")";
    }

    public String printTypes( ) {
        return type.getString()+"[" + left.printTypes() + " " + operator + " " +            right.printTypes() + "]";
    }

    public TypeDesc getType( ) {
        return type;
    }

    public Expression typeCheck( SymbolTable st ) throws TypeCheckException{
        String strRep = this.toString();
        if ( st.containsKey( strRep ) )
            return st.get(strRep);

        try {
	    left = left.typeCheck( st );
            right = right.typeCheck( st );
            type = left.getType();
            evalOp = type.getOperator( operator, right.getType() );	
        }
	catch( TypeCheckException e ) {
	    throw e;
        }

        type = evalOp.getType();

        st.put( this );
        return this;
    }

    public ExecExpr createExec(Vector[] projLists) throws InternalError{
        for(int i = 0; i < projLists.length; i++){
            for(int field = 0; field < projLists[i].size(); field++){
                if(equals(projLists[i].elementAt(field))){
                    return new ExecSelect(i, field);
                }
            }
        }
        ExecExpr l = left.createExec(projLists); 
        ExecExpr r = right.createExec(projLists);
        return new ExecOperator( evalOp, l, r );
    }

    public boolean exclusive(Vector aliases){
        return left.exclusive(aliases) && right.exclusive(aliases);
    }

    public void collect(Vector aliases, Vector expressions){
        if(exclusive(aliases)){
            expressions.addElement(this);        
            return;
        }
        left.collect(aliases, expressions);
        right.collect(aliases, expressions);
    }
}

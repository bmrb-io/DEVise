package Expressions;

import Types.*;
import java.util.*;
import Operators.*;

/** class Operator is used to represent all the binary operators.
    It takes the text of the operator, like "+" and two operands. */

public class Operator implements Expression {
    private String operator;
    private TypeDesc type;
    private Expression left;
    private Expression right;
    private EvalOperator evalOp;

    /** Constructor with arguments: an operator and two operands.*/
    public Operator(String op, Expression l, Expression r){
        operator = op;
        left=l;
        right=r;
        type = null;
        evalOp = null;
    }

    /** Check if two Operator objects are equal. */
    public boolean equals(Object obj){
        if(!getClass().equals(obj.getClass())){
            return false;
        }
        Operator opObj = (Operator) obj;
        return operator.equals(opObj.operator) &&
            left.equals(opObj.left) && right.equals(opObj.right);
    }

    /** Return its String representation. */
    public String toString( ) {
        return "(" + left.toString() + " " + operator + " " + 
        right.toString() + ")";
    }

    /** Print it with type information; used for printing type-checked 
	query. */
    public String printTypes( ) {
        return type.getString()+"[" + left.printTypes() + " " + operator + " " +            right.printTypes() + "]";
    }

    /** Return the type of evaluated result, e.g. "boolean" for "l > r". */
    public TypeDesc getType( ) {
        return type;
    }

    /** TypeCheck this Operator object; add it to the SymbolTable if it
	not contained there. */
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

    /** Create an ExecOperator object for query evaluation. */
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

package Expressions;

/** class Operator is used to represent all the binary operators.
    It takes the text of the operator, like "+" and two inputs. */

public class Operator implements Expression {
	private String operator;
	private Expression left;
	private Expression right;
	private EvalOperator evalOp;

	public Operator(String operator, Expression left, Expression right){
		this.operartor = operator;
		this.left=left;
		this.right=right;
	}

	public String toString( ){
		return left.toString() +  " " + operator + " " + right.toString();
	}

	public Expression typeCheck( SymbolTable st ){
		String strRep = this.toString();
		if(containsKey(strRep){
			return st.get(strRep);
		}

		left = left.typeCheck( st );
		right = right.typeCheck( st );

		evalOp = left.getTypeDesc().getOperator(
			operator, right.getTypeDesc());	
		typeDesc = evalOp.getTypeDesc();
		st.put( this );
		return this;
	}

	public ExecExpr createExec( OptNode[] opn ){
		ExecExpr l = left.createExec(opn); 
		ExecExpr r = right.createExec(opn);
		return new ExecOperator(evalOp, l, r);
	}
}

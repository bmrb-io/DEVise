package Expressions;

/** class Operator is used to represent all the binary operators.
    It takes the text of the operator, like "+" and two inputs. */

public class Operator implements Expression {
	private String operator;	
	private Expression left;
	private Expression right;

	public Operator(String operator, Expression left, Expression right){
		this.operator=operator;
		this.left=left;
		this.right=right;
	}

	public String toString( ){
		return left.toString() + operator + right.toString();
	}

	public Expression typeCheck( SymbolTable st ){
		Expression expr = st.get( this.toString() );
		if ( expr != null )
			return expr;

		left = left.typeCheck( st );
		right = right.typeCheck( st );

		st.put( this );
		return st.get( this.toString() );
	}

	public ExecExpr createExec( OptNode[] opn ){
		ExecExpr l = left.createExec(opn); 
		ExecExpr r = right.createExec(opn);
		return new ExecOperator(l, r, operator);
}

};

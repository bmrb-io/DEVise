package Expressions;

/** Constant is used to represent constants, like 1.
    It takes the type of a constant (like integer) and its value */

public class Constant implements Expression {
	private String type;
	private DTE_Type value;
	
	public Constant(String type, DTE_Type value){
		this.type = type;
		this.value = value;
	}

	public String toString(){
		return value.toString();
	}

	public Expression typeCheck(SymbolTable st){
		return this;
	}

	public ExecExpr createExec(OptNode[] opn){
		return new ExecConst(value);
	}
};


package Expressions;

import Types.*;

/** class Selection is used to represent expression like emp.salary */

public class Selection implements Expression {
	private String alias;
	private String attribute;
	private TypeDesc type;

	public Selection(String alias, String attribute){
		this.alias = alias;
		this.attribute = attribute;
	}

	public Selection(String alias, String attribute, TypeDesc type){
		this.alias = alias;
		this.attribute = attribute;
		this.type = type;
	}

	public String toString(){
		return alias + "." + attribute;
	}

	public Expression typeCheck(SymbolTable st){
		Expression expr = lookup(toString());
		if(expr != null){
			return expr;
		}
		else{
			// throw "table " + alias + " does not have attr " + attribute;
		}
	}

	public ExecExpr createExec(OptNode[] nodes){
		Expression[][] exprList = new Expression[2][];
		for(int i = 0; i < nodes.length; i++){
			exprList[i] = nodes[i].getProjectList();
		}
		for(int i = 0; i < nodes.length; i++){
			for(int field = 0; field < exprList[i].length; field++){
				if(exprList[i][field] == this){
					return new ExecSelect(i, field);
				}
			}
		}
		// throw exception, internal error
	}
};

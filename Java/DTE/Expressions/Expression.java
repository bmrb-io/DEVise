package Expressions;

import Types.*;

/** ExecExpr is created in the process of creating the executable structure
    from the best plan produced by the optimizer.
*/ 

public interface ExecExpr {

	/** ExecExpr always evaluates to a single types. However the
	input can be two tuples (if this expression is evaluated
	in a join method), or it can be only one tuple if this 
	expression is evaluated as a part of a unary operator,
	such as file-scan. */

	DTE_Type evaluate(Tuple left, Tuple right);
}

/** Expression is used to represents all the SQL expressions that can be found
    in WHERE or SELECT clause. 
    Two main operations that the Expressions should implement are typechecking
    and executable creations.
    Expressions are created by the parser.
 */

public interface Expression {
	String toString();

	/** Type check this expression, based on the information present in
	the symbol table. Symbol table contains previously type checked
	expressions. This method first tries to find this expression in
	the sybol table and, if found, it returns the referEnce to the
	symbol table object. If this expression was not found, the
	expression is typechecked, entered into the sybol table and
	returns with the reference to itself.
	Purpose of this procedure is to eliminate all the duplicate
	expressions from the query.
	*/

	Expression typeCheck(SymbolTable st);

	/** Once the expression is correctly typechecked and the query is
	optimized, this expression will generate an executable version of
	itself so that the actual operations like additions and multiplications
	can be executed at runtime.
	To create executable, one needs to know the left (and optionally right)
	input streams to this expression and that is why the arguments to
	this method are one or two OptNodes. */

	ExecExpr createExec(OptNode[] opn);
}

/** class Selection is used to represent expression like emp.salary */

public class Selection implements Expression {
	String alias;
	String attribute;

	public Selection(String alias, String attribute){
		this->alias = alias;
		this->attribute = attribute;
	}
	public String toString(){
		return alias + "." + attribute;
	}
	public Expression typeCheck(SymbolTable st){
	}
	public ExecExpr createExec(OptNode[] opn);
}

/** class Operator is used to represent all the binary operators.
    It takes the text of the operator, like "+" and two inputs. */

public class Operator implements Expression {
	public Operator(String operator, Expression left, Expression right){
	}
}

/** Constant is used to represent constants, like 1.
    It takes the type of a constant (like integer) and its value */

public class Constant implements Expression {
	public Constant(String type, DTE_Type input){
	}
}


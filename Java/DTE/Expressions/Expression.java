package Expressions;

import Types.*;

/** Expression is used to represents all the SQL expressions that can be found
    in WHERE or SELECT clause. 
    Two main operations that the Expressions should implement are typechecking
    and executable creations.
    Expressions are created by the parser.
 */

public interface Expression {
	public String toString();

	public TypeDesc getTypeDesc( );

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

	public Expression typeCheck( SymbolTable st );

	/** Once the expression is correctly typechecked and the query is
	optimized, this expression will generate an executable version of
	itself so that the actual operations like additions and multiplications
	can be executed at runtime.
	To create executable, one needs to know the left (and optionally right)
	input streams to this expression and that is why the arguments to
	this method are one or two OptNodes. */

	public ExecExpr createExec(OptNode[] opn);
}


package Expressions;

import Types.*;
import java.util.*;

/** Expression is used to represents all the SQL expressions that can be found
    in WHERE or SELECT clause. 
    Two main operations that the Expressions should implement are typechecking
    and executable creations.
    Expressions are created by the parser.
 */

public interface Expression {
	public String toString( );
	public String printTypes( );

	public TypeDesc getType( );

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

	public ExecExpr createExec(Vector[] projLists) throws InternalError;

	/** Returns true if this expression (including all of its children)
	refers only to tables listed in vector of aliases */

	public boolean exclusive(Vector aliases);

	/** Append the exclusive (as defined above) expressions
	to the vector of expressions */

	public void collect(Vector aliases, Vector expressions);
}


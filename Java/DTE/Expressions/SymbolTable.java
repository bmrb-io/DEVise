package Expressions;

import java.util.Hashtable;

/** Symbol table is used to store already resolved (type checked) expressions
    without duplicates.
*/

public class SymbolTable {
	private Hashtable table;

	public SymbolTable( ){
		table = new Hashtable( );
	}

	public boolean containsKey(String exprStr){
		return table.containsKey( exprStr );
	}

	public void put(Expression expr){

		// if ( !table.contains( expr ) )

		table.put( expr.toString( ), expr );
	}

	public Expression get(String exprStr){
		return (Expression)table.get( exprStr );	
	}
}


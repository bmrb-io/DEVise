package Expressions;

import java.util.Hashtable;

/** Symbol table is used to store already resolved (type checked) expressions
    without duplicates.
*/

public class SymbolTable {
	private Hashtable table;

	public SymbolTable( ){
		table = new hashTable( );
	}

	public SymbolTable( Schema schema ){
		this( );
		// insert Expressions from the given schema
	}

	public boolean contains(String exprStr){
		return table.containsKey( exprStr );
	}

	public void insert(Expression expr){
		if ( !table.contains( expr ) )
			table.put( expr.toString( ), expr );
	}

	public Expression lookup(String exprStr){
		return table.get( exprStr );	
	}
}


package Expressions;

import java.util.Hashtable;

/** Symbol table is used to store already resolved (type checked) expressions
    without duplicates.
*/

public class SymbolTable {
	private Hashtable table;

        /** Constructor: create an empty SymbolTable. */
	public SymbolTable( ){
		table = new Hashtable( );
	}
  
	/** Check if an Expression is in the SymbolTable, using its 
	    String representation as key. */
	public boolean containsKey(String exprStr){
		return table.containsKey( exprStr );
	}

        /** Add a new expression to the SymbolTable. */
	public void put(Expression expr){

		table.put( expr.toString( ), expr );
	}

        /** Return the Expression from the SymbolTable corresponding 
	    to a String. */
	public Expression get(String exprStr){
		return (Expression)table.get( exprStr );	
	}
}


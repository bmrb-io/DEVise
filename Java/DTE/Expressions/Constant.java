package Expressions;

import Types.*;
import java.util.*;

/** Constant is used to represent constants, like 1.
    It takes the type of a constant (like integer) and its value */

public class Constant implements Expression {
	private TypeDesc type;
	private DTE_Type value;
	
	public Constant( DTE_Type value ) {
		this.value = value;
		this.type = value.getType( );
	}

	public Constant( TypeDesc type, DTE_Type value ) {
		this.type = type;
		this.value = value;
	}

	public String toString( ) {
		return value.toString( );
	}

	public TypeDesc getType( )
	{
		return type;
	}

	public Expression typeCheck( SymbolTable st ) {
		String strRep = this.toString();
		if ( st.containsKey( strRep ) )
		    return st.get(strRep);

		st.put( this );
		return this;
	}

	public ExecExpr createExec(Vector[] projLists) throws InternalError {
		return new ExecConst( value );
	}

	public boolean exclusive(Vector aliases){
		return true;   // constants are contained in any table
	}

	public void collect(Vector aliases, Vector expressions){
		expressions.addElement(this);
	}
}


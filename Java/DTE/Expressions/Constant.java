package Expressions;

import Types.*;
import java.util.*;

/** Constant is used to represent constants, like 2, 'abc', 3.14, etc.
    It takes the type and value of a constant. */

public class Constant implements Expression {
	private TypeDesc type;
	private DTE_Type value;
	
	/** Constructor with only one argument: the value of the 
	    constant. */
	public Constant( DTE_Type value ) {
		this.value = value;
		this.type = value.getType( );
	}

	/** Constructor with two arguments: the type and value of the 
	    constant. */
	public Constant( TypeDesc type, DTE_Type value ) {
		this.type = type;
		this.value = value;
	}

	/** Return its string representation. */
	public String toString( ) {
		return value.toString( );
	}

	/** Print the constant, used for printing the type-checked 
	    query. */
	public String printTypes( ) {
		return toString( );
	}
    
	/** Return the type of the constant. */
	public TypeDesc getType( )
	{
		return type;
	}

	/** TypeCheck it, and put it into SymbolTable if it's not
	    there. */
	public Expression typeCheck( SymbolTable st ) {
		String strRep = this.toString();
		if ( st.containsKey( strRep ) )
		    return st.get(strRep);

		st.put( this );
		return this;
	}

        /** Check if two Constant objects are equal. */
	public boolean equals(Object obj){
		if(! getClass().equals(obj.getClass())){
			return false;
		}
		Constant cObj = (Constant) obj;
		return value.equals(cObj.value);
	}

        /** Create an ExecExpr object for query evaluation. */
	public ExecExpr createExec(Vector[] projLists) throws InternalError {
		return new ExecConst( value );
	}

	public boolean exclusive(Vector aliases){
		return true;   
	}

	public void collect(Vector aliases, Vector expressions){
		expressions.addElement(this);
	}
}


package Expressions;

import Types.*;

public class ExecOperator implements ExecExpr{
    ExecExpr left;
    ExecExpr right;
    String operator;

    public ExecOperator(ExecExpr l, ExecExpr r, String op){
	left = l;
	right = r;
	operator = op;
    }

    public DTE_Type evaluate(Tuple leftT, Tuple rightT) throws Exception{
	DTE_Type arg1 = left.evaluate(leftT,  rightT);
	DTE_Type arg2 = right.evaluate(leftT, rightT);

	String type1 = arg1.get_type( );
	String type2 = arg2.get_type( );
	if ( type1.equals("string") || type2.equals("string") )
	    throw new RuntimeException( "can't add strings!\n");

	if ( operator.equal("+") )
		return add(arg1, arg2);
	if ( operator.equal("-") )
		return sub(arg1, arg2);
	if ( operator.equal("*") )
		return mul(arg1, arg2);
	if ( operator.equal("/") )
		return div(arg1, arg2);
    }

    public DTE_Type add( DTE_Type arg1, DTE_Type arg2) {
	String type1 = arg1.get_type( );
	String type2 = arg2.get_type( );

	if ( type1.equals("int") )
            int a1 = arg1.get_val( ).intValue( );		
        else 
	    double a1 = arg1.get_val( ).doubleValue( );

	if ( type2.equals("int") )
            int a2 = arg2.get_val( ).intValue( );		
        else 
	    double a2 = arg2.get_val( ).doubleValue( );

        if ( type1.equals("int") && type2.equals("int") ) {
	    return new DTE_Int( a1+a2 );
	else 
	    return new DTE_Double( a1+a2 );
    }

    public DTE_Type sub( DTE_Type arg1, DTE_Type arg2) {
	String type1 = arg1.get_type( );
	String type2 = arg2.get_type( );

	if ( type1.equals("int") )
            int a1 = arg1.get_val( ).intValue( );		
        else 
	    double a1 = arg1.get_val( ).doubleValue( );

	if ( type2.equals("int") )
            int a2 = arg2.get_val( ).intValue( );		
        else 
	    double a2 = arg2.get_val( ).doubleValue( );

        if ( type1.equals("int") && type2.equals("int") ) {
	    return new DTE_Int( a1-a2 );
	else 
	    return new DTE_Double( a1-a2 );
    }

    public DTE_Type mul( DTE_Type arg1, DTE_Type arg2) {
	String type1 = arg1.get_type( );
	String type2 = arg2.get_type( );

	if ( type1.equals("int") )
            int a1 = arg1.get_val( ).intValue( );		
        else 
	    double a1 = arg1.get_val( ).doubleValue( );

	if ( type2.equals("int") )
            int a2 = arg2.get_val( ).intValue( );		
        else 
	    double a2 = arg2.get_val( ).doubleValue( );

	return new DTE_Double( a1*a2 );
    }

    public DTE_Type div( DTE_Type arg1, DTE_Type arg2) {
	String type1 = arg1.get_type( );
	String type2 = arg2.get_type( );

	if ( type1.equals("int") )
            int a1 = arg1.get_val( ).intValue( );		
        else 
	    double a1 = arg1.get_val( ).doubleValue( );

	if ( type2.equals("int") )
            int a2 = arg2.get_val( ).intValue( );		
        else 
	    double a2 = arg2.get_val( ).doubleValue( );
	return new DTE_Double( a1/a2 );
    }
}


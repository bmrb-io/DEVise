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

    public TypeDesc getType( ) {
        return type;
    }

    public Expression typeCheck(SymbolTable st) throws RuntimeException{
        String str = this.toString( );
        if ( st.containsKey( str ) )
            return st.get( str );

        str="table "+alias+" does not have attr "+attribute;
        throw new RuntimeException( str );
    }

    public ExecExpr createExec(OptNode[] nodes) throws InternalError{
        Expression[][] exprList = new Expression[2][];
        for(int i = 0; i < nodes.length; i++){
            exprList[i] = nodes[i].getProjectList();
        }

        for(int i = 0; i < nodes.length; i++)
            for(int field = 0; field < exprList[i].length; field++)
                if(exprList[i][field] == this)
                    return new ExecSelect(i, field);

        throw new InternalError("What's wrong? Let me ask Donko...\n");
    }
}




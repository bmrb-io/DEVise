package Expressions;

import Types.*;
import java.util.*;

/** class Selection is used to represent expression like emp.salary */

public class Selection implements Expression {
    private String alias;
    private String attribute;
    private TypeDesc type;

    public Selection(String alias, String attribute){
        this.alias = alias;
        this.attribute = attribute;
	this.type = null;
    }

    public Selection(String alias, String attribute, TypeDesc type){
        this.alias = alias;
        this.attribute = attribute;
        this.type = type;
    }

    public boolean equals(Object obj){
        if(! getClass().equals(obj.getClass())){
            return false;
        }
	   Selection selObj = (Selection) obj;
	   return alias.equals(selObj.alias) && attribute.equals(selObj.attribute);
    }

    public String toString(){
        return alias + "." + attribute;
    }

    public String printTypes(){
        return "["+type.getString()+": "+toString()+"]";
    }

    public TypeDesc getType( ) {
        return type;
    }

    public Expression typeCheck(SymbolTable st) throws RuntimeException{
        String str = this.toString( );
        if ( st.containsKey( str ) )
            return st.get( str );

        str="\n  Table "+alias+" does not have attribute "+attribute+".\n";
        throw new TypeCheckException( str );
    }

    public ExecExpr createExec(Vector[] projLists) throws InternalError{
        for(int i = 0; i < projLists.length; i++){
            for(int field = 0; field < projLists[i].size(); field++){
                if(equals(projLists[i].elementAt(field))){
                    return new ExecSelect(i, field);
                }
            }
        }
        throw new InternalError("What's wrong? Let me ask Donko...\n");
    }

    public boolean exclusive(Vector aliases){
        return aliases.contains(alias);    
    }

    public void collect(Vector aliases, Vector expressions){
        if(exclusive(aliases)){
	       expressions.addElement(this);
        }
    }
}




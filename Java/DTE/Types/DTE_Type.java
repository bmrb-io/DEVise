package Types;

import java.io.*;

/** This is a base class for every possible type in DTE */
public abstract class DTE_Type {
    
    /** This method returns an instance of a type given its descriptor */
    
    public static DTE_Type typeFor(TypeDesc type){
	if(type.getString( ).equals("int"))
	    return new DTE_Int();
	
	if(type.getString( ).equals("double"))
	    return new DTE_Double();
	
	if(type.getString( ).equals("string"))
	    return new DTE_String();
	
	if(type.getString( ).equals("boolean"))
	    return new DTE_Boolean();
	
	System.out.println("Unknown type: "+type.getString( ) );
	return null;
    }
    
    abstract public boolean read(StreamTokenizer st) throws IOException;
    abstract public void print(PrintWriter ps);
    abstract public void print(PrintStream ps);
    //abstract public Object clone( );
    abstract public TypeDesc getType( );
    abstract public void copyTo(DTE_Type d);
}


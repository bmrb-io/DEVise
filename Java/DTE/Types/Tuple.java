package Types;

import java.io.*;

/** Class Tuple is used throughout the system to store the content of a 
    record. Tuple is implemented as an array of types.
    It is constructed from an array of type descriptors. */
    
public class Tuple 
{
    DTE_Type[] types;
    
    public Tuple(){}
    
    public Tuple(TypeDesc[] typeStrings){
	types = new DTE_Type[typeStrings.length];
	for(int i = 0; i < typeStrings.length; i++){
	    types[i] = DTE_Type.typeFor(typeStrings[i]);
	}
    }
   
    /** Read a tuple from a StreamTokenizer object "st". Returns false if the EOF is encountered */
    
    public boolean read(StreamTokenizer st) throws IOException 
	{
	    for(int i = 0; i < types.length; i++){
		if(! types[i].read(st)){
		    return false;
		}
	    }
	    return true;
	}
    
  /** Print this tuple to a PrintWriter object "ps".*/
    public void print(PrintWriter os)
	{
	    for(int i = 0; i < types.length; i++)
		{
		    types[i].print(os);
		}
	    os.println();
	    os.flush();
	}
    
  /** Print this tuple to a PrintStream object "ps".*/
    public void print(PrintStream os)
	{
	    for(int i = 0; i < types.length; i++)
		types[i].print(os);
	    
	    os.println();
	    os.flush();
	}

  /** Return the fileds stored in the tuple, namely ' TypeDesc[] types'.*/
    public DTE_Type[] get_fields( )
	{
	    return types;
	}

    /** Copy this tuple to tuple 't'. This function assume that the two Tuple objects have the same schema. */
    public void copyTo(Tuple t)
	{
	    for(int i = 0; i < types.length; i++)
		types[i].copyTo(t.types[i]);
	}
} 

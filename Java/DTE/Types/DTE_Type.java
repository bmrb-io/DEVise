package Types;

import java.io.*;


/** This is a base class for every possible type in DTE */
public abstract class DTE_Type {

	/** This method returns an instance of a type given its descriptor */

	public static DTE_Type typeFor(TypeDesc typeName){
		if(typeName.getString().equals("int")){
			return new DTE_Int();
		}
		else if(typeName.getString().equals("double")){
			return new DTE_Double();
		}

		if(typeName.getString().equals("string")){
			return new DTE_String();
		}

		System.out.println("Unknown type: " + typeName.getString());
		return null;
	}
	abstract public boolean read(StreamTokenizer st) throws IOException;
	abstract public void print(PrintWriter ps);
	abstract public void print(PrintStream ps);
        abstract public Object clone();
}


class DTE_Int extends DTE_Type implements Cloneable 
{
  int val;
  
  public DTE_Int()
  {
    val = 0;
  }
  
  public DTE_Int(int i)
  {
    val = i;
  }
  
  public boolean read(StreamTokenizer st) throws IOException 
  {
    int status = st.nextToken();
    if(status == StreamTokenizer.TT_EOF)
      return false;
    
    if(status != StreamTokenizer.TT_NUMBER)
      {
	int lineno = st.lineno();
	System.err.println("The data on line " + lineno + " is not an integer!");
	return false;
      }
    
    // Need to check that this is really an int.
    // return 0 if this is not an int and print the msg to stderr.
    // use st.lineno()
    
    val = (int) st.nval;
    
    if((double)val != st.nval)
      {
	int lineno = st.lineno();
	System.err.println("The data on line " + lineno + " is not an integer!");
	return false;
      }
    
    return true;
  }
  
  public void print(PrintWriter ps)
  {
    ps.print(val);
  }

  public void print(PrintStream ps)
  {
    ps.print(val);   
    ps.print(" "); 
  }

  public Object clone()
  {
    return new DTE_Int(val);
  }
}


class DTE_Double extends DTE_Type implements Cloneable 
{
  double val;
  public DTE_Double()
  {
    val = 0;
  }

  public DTE_Double(double i)
  {
    val = i;
  }
  
  public boolean read(StreamTokenizer st) throws IOException 
  {
    int status = st.nextToken();
    if(status == StreamTokenizer.TT_EOF)
      return false;
    
    if(status != StreamTokenizer.TT_NUMBER)
      {
	int lineno = st.lineno();
	System.err.println("The data on line " + lineno + " is not a double!");
	return false;
      }
    
    val = st.nval;
    return true;
  }
 
  public void print(PrintWriter ps)
  {
    ps.print(val);
  }

  public void print(PrintStream ps)
  {
    ps.print(val);
    ps.print(" "); 
  }


  public Object clone()
  {
    return new DTE_Double(val);
  }
}


class DTE_String extends DTE_Type implements Cloneable 
{
  String val;
  public DTE_String()
  {
    val = null;
  }
  
  public DTE_String(String i)
  {
    val = new String(i);
  }
	
  public boolean read(StreamTokenizer st) throws IOException 
  { 
    int status = st.nextToken();
    if(status == StreamTokenizer.TT_EOF)
      return false;
    
    if(status != StreamTokenizer.TT_WORD)
      {
	int lineno = st.lineno();
	System.err.println("The data on line " + lineno + " is not a String!");
	return false;
      }
    
    val = st.sval;
    return true;
  }
  
  public void print(PrintWriter ps)
  {
    ps.print(val);
  }

  public void print(PrintStream ps)
  {
    ps.print(val);
    ps.print(" ");   
  }


  public Object clone()
  {
    return new DTE_String(val);
  }
}








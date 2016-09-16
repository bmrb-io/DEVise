package EDU.bmrb.starlibj;

import java.lang.*;
import java.util.*;


/** A VectorCheckType that is frozen to only contain
  * DataLoopNodes and DataItemNodes.
  */
public class SaveListVector extends VectorCheckType
{
    /** Constructor - makes a VectorCheckType and freezes it to
      * only accept DataLoopNodes and DataItemNodes
      */
    public SaveListVector()
    {
	super();
	setupTypes();
    }
    /** Constructor - makes a VectorCheckType and freezes it to
      * only accept DataLoopNodes and DataItemNodes.
      */
    public SaveListVector( int startCap )
    {   super(startCap);
	setupTypes();
    }
    /** Constructor - makes a VectorCheckType and freezes it to
      * only accept DataLoopNodes and DataItemNodes.
      */
    public SaveListVector( int startCap, int incr )
    {   super(startCap,incr);
	setupTypes();
    }

    private void setupTypes()
    {
	try
	{  addType( Class.forName( StarValidity.clsNameDataItemNode) );
	    addType( Class.forName( StarValidity.clsNameDataLoopNode) );
	    freezeTypes();
	}
	catch( ClassNotFoundException exc )
	{   System.err.println( "Should never happen exception: " +
		                exc.getMessage() );
	    exc.printStackTrace();
	}
    }
}



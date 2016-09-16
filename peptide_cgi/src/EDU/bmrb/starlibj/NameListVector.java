package EDU.bmrb.starlibj;

import java.lang.*;
import java.util.*;


/** This is used internally by DataLoopNameListNode.
  * <p>
  * This class is just like VectorCheckType except that it
  * is hardcoded to only accept LoopNameListNodes.
  */
public class NameListVector extends VectorCheckType
{
    /** Constructor - makes a VectorCheckType and freezes it to
      * only accept LoopNameListNodes
      */
    public NameListVector()
    {
	super();
	setupTypes();
    }
    /** Constructor - makes a VectorCheckType and freezes it to
      * only accept LoopNameListNodes.
      */
    public NameListVector( int startCap )
    {   super(startCap);
	setupTypes();
    }
    /** Constructor - makes a VectorCheckType and freezes it to
      * only accept LoopNameListNodes.
      */
    public NameListVector( int startCap, int incr )
    {   super(startCap,incr);
	setupTypes();
    }

    private void setupTypes()
    {
	try
	{   addType( Class.forName( StarValidity.clsNameLoopNameListNode) );
	}
	catch( ClassNotFoundException exc )
	{   System.err.println( "Should never happen exception: " +
		                exc.getMessage() );
	    exc.printStackTrace();
	}
	freezeTypes();
    }
}



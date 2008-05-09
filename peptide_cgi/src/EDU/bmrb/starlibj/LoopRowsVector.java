package EDU.bmrb.starlibj;

import java.lang.*;
import java.util.*;


/** A VectorCheckType that is hard-coded to only accept
  * LoopRowNodes as values.
  */
public class LoopRowsVector  extends VectorCheckType
{
    /** Constructor - makes a VectorCheckType and freezes it to
      * only accept LoopRowNodes
      */
    public LoopRowsVector()
    {
	super();
	setupTypes();
    }
    /** Constructor - makes a VectorCheckType and freezes it to
      * only accept LoopRowNodes.
      */
    public LoopRowsVector( int startCap )
    {   super(startCap);
	setupTypes();
    }
    /** Constructor - makes a VectorCheckType and freezes it to
      * only accept LoopRowNodes.
      */
    public LoopRowsVector( int startCap, int incr )
    {   super(startCap,incr);
	setupTypes();
    }

    private void setupTypes()
    {
	try
	{   addType( Class.forName( StarValidity.clsNameLoopRowNode) );
	    freezeTypes();
	}
	catch( ClassNotFoundException exc )
	{   System.err.println( "Should never happen exception: " +
		                exc.getMessage() );
	    exc.printStackTrace();
	}
    }
}



package EDU.bmrb.starlibj;

import java.lang.*;
import java.util.*;


/** This class behaves just like Vector, except that
  * it allows only BlockNodes to exist inside of it.
  */
public class StarListVector extends VectorCheckType
{
    /** Constructor - makes a VectorCheckType and freezes it to
      * only accept BlockNodes.
      */
    public StarListVector()
    {
	super();
	setupTypes();
    }
    /** Constructor - makes a VectorCheckType and freezes it to
      * only accept BlockNodes, SaveFrameNodes and DataItemNodes.
      */
    public StarListVector( int startCap )
    {   super(startCap);
	setupTypes();
    }
    /** Constructor - makes a VectorCheckType and freezes it to
      * only accept BlockNodes, SaveFrameNodes and DataItemNodes.
      */
    public StarListVector( int startCap, int incr )
    {   super(startCap,incr);
	setupTypes();
    }

    private void setupTypes()
    {
	try
	{   addType( Class.forName( StarValidity.clsNameBlockNode) );
	    freezeTypes();
	}
	catch( ClassNotFoundException exc )
	{   System.err.println( "Should never happen exception: " +
		                exc.getMessage() );
	    exc.printStackTrace();
	}
    }
}


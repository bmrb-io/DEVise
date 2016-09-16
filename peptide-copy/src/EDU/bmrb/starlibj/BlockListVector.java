package EDU.bmrb.starlibj;

import java.lang.*;
import java.util.*;
import EDU.bmrb.starlibj.*;

/** This class behaves just like Vector, except that
  * it allows only SaveFrameNodes and DataLoopNodes
  * to exist inside of it.
  */
public class BlockListVector extends VectorCheckType
{
    /** Constructor - makes a VectorCheckType and freezes it to
      * only accept DataItemNodes, SaveFrameNodes, and DataLoopNodes.
      */
    public BlockListVector()
    {
	super();
	setupTypes();
    }
    /** Constructor - makes a VectorCheckType and freezes it to
      * only accept DataItemNodes, SaveFrameNodes, and DataLoopNodes.
      */
    public BlockListVector( int startCap )
    {   super(startCap);
	setupTypes();
    }
    /** Constructor - makes a VectorCheckType and freezes it to
      * only accept DataItemNodes, SaveFrameNodes, and DataLoopNodes.
      */
    public BlockListVector( int startCap, int incr )
    {   super(startCap,incr);
	setupTypes();
    }

    private void setupTypes()
    {
	try
	{
	    addType( Class.forName( StarValidity.clsNameDataItemNode  ));
	    addType( Class.forName( StarValidity.clsNameDataLoopNode  ));
	    addType( Class.forName( StarValidity.clsNameSaveFrameNode  ));
	    freezeTypes();
	}
	catch( ClassNotFoundException exc )
	{   System.err.println( "Should never happen exception: " +
		exc.getMessage() );
	    exc.printStackTrace();
	}
    }
}


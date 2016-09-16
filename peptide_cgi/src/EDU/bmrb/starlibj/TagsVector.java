package EDU.bmrb.starlibj;

import java.lang.*;
import java.util.*;

/** This is used internally by LoopNameListNode.
  * <p>
  * This class is just like VectorCheckType except that it
  * is hardcoded to only accept DataNameNodes
  */
public class TagsVector extends VectorCheckType
{
    /** Constructor - makes a VectorCheckType and freezes it to
      * only accept DataNameNodes.
      */
    public TagsVector()
    {
	super();
	setupTypes();
    }
    /** Constructor - makes a VectorCheckType and freezes it to
      * only accept DataNameNodes.
      */
    public TagsVector( int startCap )
    {   super(startCap);
	setupTypes();
    }
    /** Constructor - makes a VectorCheckType and freezes it to
      * only accept DataNameNodes
      */
    public TagsVector( int startCap, int incr )
    {   super(startCap,incr);
	setupTypes();
    }

    private void setupTypes()
    {
	try
	{
	    addType( Class.forName(StarValidity.clsNameDataNameNode) );
	    freezeTypes();
	}
	catch( ClassNotFoundException exc )
	{
	    System.err.println( "Should never happen exception: " +
 	                        exc.getMessage() );
	    exc.printStackTrace();
	}
    }
}



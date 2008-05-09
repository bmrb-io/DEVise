package EDU.bmrb.starlibj;

import java.lang.*;
import java.util.*;


/** Just like VectorCheckType, but it is set up to only
  * accept TinyRelDataValueNodes to be in the vector, and it is
  * 'frozen' that way.
  * @see VectorCheckType
  */
public class DataValuesVector extends VectorCheckType
{
    /** Constructor - makes a VectorCheckType and freezes it to
      * only accept DataValueNodes.
      */
    public DataValuesVector()
    {
	super();
	setupTypes();
    }
    /** Constructor - makes a VectorCheckType and freezes it to
      * only accept DataValueNodes.
      */
    public DataValuesVector( int startCap )
    {   super(startCap);
	setupTypes();
    }
    /** Constructor - makes a VectorCheckType and freezes it to
      * only accept DataValueNodes.
      */
    public DataValuesVector( int startCap, int incr )
    {   super(startCap,incr);
	setupTypes();
    }

    private void setupTypes()
    {
	try
	{
	    addType( Class.forName( StarValidity.clsNameTinyRelDataValueNode ) );
	    freezeTypes();
	}
	catch( ClassNotFoundException exc )
	{   System.err.println( "Should never happen exception: " +
		    exc.getMessage() );
	    exc.printStackTrace();
	}
    }
}



package EDU.bmrb.starlibj;
import java.lang.*;
import java.util.*;

/** An attempt was made to use a string as a name for some star,
  * node but the string was not a valid name.  For example, trying
  * to use "asdf" as the name of a saveframe instead of
  * "save_asdf", or trying to use "asdf" as the name of a tag (no
  * leading underscore).
  */
public class NameViolatesStarSyntax extends RuntimeException
{
    /** @param name The string name that violated the syntax
      * @param type String describing the type of syntax it should have been.
      */
    public   NameViolatesStarSyntax( String name, String type )
    {
	super();
	msg = "The name '" + name + "' is not a valid " + type;
    }
    public String getMessage()
    {
	return msg;
    }

    protected String msg;
}

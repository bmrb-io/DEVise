package EDU.bmrb.starlibj;
import java.lang.*;
import java.util.*;

/** This is a return type for StarValidity.parseValFrom(),
  * which needs to return more than one thing - The only
  * way to do that in Java is to make a class containing all
  * the return values and have the method return an instance
  * of that class.
  */
public class ParseValFromRetVal
{
    /** The delimiter matched */
    public short   delim;

    /** The string that was matched */
    public String  str;

    /** The index of where the parse ended + 1 */
    public int     endingIdx;

    /** The index where the next parse should start, if
      * there are more DataValues embedded in this string
      */
    public int     nextIdx;

    /** Was a value found or not?  (Will be false if, for example,
      * the string being parsed was nothing but whitespace.)
      */
    public boolean found;

}

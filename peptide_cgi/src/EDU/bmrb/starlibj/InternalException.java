
package EDU.bmrb.starlibj;
import java.lang.*;
import java.util.*;

/** This is thrown when there is some internal problem with
  * the starlibj that was not expected.  Whenever this exception
  * appears it indicates an error on the part of the starlibj
  * programmer.  A bug needs to be reported.  The starlibj
  * programmer(s) put this exception in as a keep-us-honest check.
  * (If we think a condition is a "can't-happen-ever" condition,
  * we throw this exception from that point just to be sure.  If
  * we are right in our assumptions, a user will never see this
  * message.)
  */
public class InternalException extends RuntimeException
{

    public InternalException( )
    {
	additionalStr = null;
    }

    /** Allow the thrower to make additional information about the
      * exception appear in the message:
      */
    public InternalException( String msg)
    {
	additionalStr = new String(msg);
    }

    /** The message that will be printed at runtime if this exception
      * 'bubbles up' all the way to the Java Runtime Environment and
      * is not caught along the way.
      * @return the message to be printed by the JVM.
      */
    public String getMessage()
    {
	return System.getProperty("line.separator") +
	       "If you see this message, then there is a bug in the" +
	       System.getProperty("line.separator") +
	       StarValidity.pkgName() +
	       " package.  This exception is deliberately " +
	       System.getProperty("line.separator") +
	       "thrown in parts of the code where the programmer believed" +
	       System.getProperty("line.separator") +
	       "it was impossible to reach a certain line of code. " +
	       System.getProperty("line.separator") +
	       "If you see this message in a user-program, then a bug " +
	       System.getProperty("line.separator") +
	       "report should be reported to the developers.  (Include " +
	       System.getProperty("line.separator") +
	       "the following output in the message.)" +
	       System.getProperty("line.separator") +
	       (  ( additionalStr == null ) ?
		       "" :
		       ( "ADDITIONAL INFORMATION: " +
			 additionalStr +
			 System.getProperty("line.separator") )
               ) ;

	
	// (After the message above prints, the stack trace will
	// typically be printed, which the programmer can use to
	// track down the problem.)
    }

    String additionalStr;
}


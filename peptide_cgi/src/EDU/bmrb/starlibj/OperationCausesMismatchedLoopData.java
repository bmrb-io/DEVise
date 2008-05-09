package EDU.bmrb.starlibj;
import java.lang.*;
import java.util.*;

/** This is thrown when an operation would have caused the loop
  * to have data that does not match the name list.  For example,
  * if there are three tag names in the loop, then there need to be
  * exactly three values in each row.
  */
public class OperationCausesMismatchedLoopData extends RuntimeException
{
}


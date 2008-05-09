package EDU.bmrb.starlibj;

import java.lang.*;
import java.util.*;

/** This really trivial class exists solely to get around the fact
  * that Java can't pass an int by reference or pointer (only by
  * value).  Java's extremely religious devotion to its "No Pointers
  * Ever!" rule makes us have to do this silly extra work.  (It wouldn't
  * be so bad if the built-in class "Integer" allowed you to alter
  * a value after construction, but it doesn't, so we can't use that as
  * a pass-by-reference.  So any time starlibj needs to pass back an
  * integer as a parameter, it has to use this stupid little class to
  * do it.  Yes, in case you were wondering, I do find this to be
  * rather annoying.  Most of Java is good, but this particular snag
  * is really grating on my nerves.)
  */
public class RemoteInt
{
    public int num;
}



/*
  ========================================================================
  DEVise Data Visualization Software
  (c) Copyright 1992-1998
  By the DEVise Development Group
  Madison, Wisconsin
  All Rights Reserved.
  ========================================================================

  Under no circumstances is this software to be copied, distributed,
  or altered in any way without prior permission from the DEVise
  Development Group.
*/

/*
  Description of module.
 */

/*
  $Id$

  $Log$
 */
// Define exception might be recoverable
public class DEViseException extends Exception
{
    public DEViseException()
    {
        super("DEViseException has been throwed!");
    }
    
    public DEViseException(String s)
    {
        super(s);
    }
}

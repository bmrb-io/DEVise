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
// Define error happened while communicate with DEVise Server, nonrecoverable
public class DEViseNetException extends Exception
{
    public DEViseNetException()
    {
        super("DEViseNetException has been throwed!");
    }
    
    public DEViseNetException(String s)
    {
        super(s);
    }
}

// ========================================================================
// DEVise Data Visualization Software
// (c) Copyright 1999-2000
// By the DEVise Development Group
// Madison, Wisconsin
// All Rights Reserved.
// ========================================================================

// Under no circumstances is this software to be copied, distributed,
// or altered in any way without prior permission from the DEVise
// Development Group.

// ------------------------------------------------------------------------

// ADD COMMENT: overall description of the function of this class

// ------------------------------------------------------------------------

// $Id$

// $Log$
// Revision 1.4  2000/03/23 16:26:17  wenger
// Cleaned up headers and added requests for comments.
//
// Revision 1.3  1999/06/23 20:59:20  wenger
// Added standard DEVise header.
//

// ========================================================================

// YLogConsole.java

import  java.util.*;
import  java.text.*;

public class YLogConsole
{
    private int loglevel;

    public YLogConsole(int level)
    {
        loglevel = level;
    }

    public void p(String msg, int level)
    {
        if (level > loglevel) {
            return;
        } else {
	    Date date = new Date();
	    DateFormat dtf = DateFormat.getDateTimeInstance(DateFormat.MEDIUM, 
	      DateFormat.MEDIUM);
            System.out.print(msg + " [" + dtf.format(date) + "]");
        }
    }

    public void p(String msg)
    {
        p(msg, 1);
    }

    public void pn(String msg, int level)
    {
        if (level > loglevel) {
            return;
        } else {
	    Date date = new Date();
	    DateFormat dtf = DateFormat.getDateTimeInstance(DateFormat.MEDIUM, 
	      DateFormat.MEDIUM);
            System.out.println(msg + " [" + dtf.format(date) + "]");
        }
    }

    public void pn(String msg)
    {
        pn(msg, 1);
    }
}
  

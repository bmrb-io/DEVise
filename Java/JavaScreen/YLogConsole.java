// ========================================================================
// DEVise Data Visualization Software
// (c) Copyright 1999-2001
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

// ========================================================================

// YLogConsole.java

package JavaScreen;

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


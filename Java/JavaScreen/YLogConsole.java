// ========================================================================
// DEVise Data Visualization Software
// (c) Copyright 1999
// By the DEVise Development Group
// Madison, Wisconsin
// All Rights Reserved.
// ========================================================================

// Under no circumstances is this software to be copied, distributed,
// or altered in any way without prior permission from the DEVise
// Development Group.

// $Id$

// $Log$

// ========================================================================

// YLogConsole.java

public class YLogConsole
{
    private int loglevel;

    public YLogConsole(int level)
    {
        loglevel = level;
    }

    public void p(String msg, int level)
    {
        if (level > loglevel)
            return;
        else
            System.out.print(msg);
    }

    public void p(String msg)
    {
        p(msg, 1);
    }

    public void pn(String msg, int level)
    {
        if (level > loglevel)
            return;
        else
            System.out.println(msg);
    }

    public void pn(String msg)
    {
        pn(msg, 1);
    }
}
  

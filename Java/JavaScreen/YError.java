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


public class YError extends Error
{
    private static String defaultMsg = "YError has been throwed!";

    public YError()
    {
        this(null, null);
    }

    public YError(String s)
    {
        this(s, null);
    }

    public YError(String s, String w)
    {
        super( ((s == null)?defaultMsg:s) + ((w == null)?"":(" in " + w)) );
    }
}


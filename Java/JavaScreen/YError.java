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
// Revision 1.5  1999/12/10 15:37:01  wenger
// Added standard headers to source files.
//
// ========================================================================


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


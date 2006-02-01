// ========================================================================
// DEVise Data Visualization Software
// (c) Copyright 2003
// By the DEVise Development Group
// Madison, Wisconsin
// All Rights Reserved.
// ========================================================================

// Under no circumstances is this software to be copied, distributed,
// or altered in any way without prior permission from the DEVise
// Development Group.

// ------------------------------------------------------------------------

// Exception indicating an operation was cancelled (not an error or
// warning).

// ------------------------------------------------------------------------

// $Id$

// $Log$
// Revision 1.1.2.1  2003/05/08 16:40:50  wenger
// Got rid of all System.exit() calls.
//

// ========================================================================

package star2devise;

public class S2DCancel extends S2DException
{
    private static final int DEBUG = 0;

    public S2DCancel()
    {
        super("Operation cancelled");

        if (DEBUG >= 1) {
            printStackTrace();
        }
    }

    public S2DCancel(String msg)
    {
        super(msg);

        if (DEBUG >= 1) {
            printStackTrace();
        }
    }
}

// ========================================================================


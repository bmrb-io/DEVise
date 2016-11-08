// ========================================================================
// DEVise Data Visualization Software
// (c) Copyright 2003-2007
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
// Revision 1.2  2006/02/01 20:23:10  wenger
// Merged V2_1b4_br_0 thru peptide_cgi_10_8_0_base to the
// trunk.
//
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

        if (doDebugOutput(11)) {
            printStackTrace();
        }
    }

    //-------------------------------------------------------------------
    // Determine whether to do debug output based on the current debug
    // level settings and the debug level of the output.
    private static boolean doDebugOutput(int level)
    {
        if (DEBUG >= level || S2DMain._verbosity >= level) {
            if (level > 0) System.out.print("DEBUG " + level + ": ");
            return true;
        }

        return false;
    }
}

// ========================================================================


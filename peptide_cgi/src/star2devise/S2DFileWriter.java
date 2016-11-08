// ========================================================================
// DEVise Data Visualization Software
// (c) Copyright 2002-2007
// By the DEVise Development Group
// Madison, Wisconsin
// All Rights Reserved.
// ========================================================================

// Under no circumstances is this software to be copied, distributed,
// or altered in any way without prior permission from the DEVise
// Development Group.

// ------------------------------------------------------------------------

// This class creates a FileWriter, but if the output file exists, it
// deletes it first.  (This is so that we don't get goofed up if an
// output file exists, but we don't have write permission on it, for
// example if it was created by a different user.)  We assume, of course,
// that we have write permission in the relevant *directory*.

// ------------------------------------------------------------------------

// $Id$

// $Log$
// Revision 1.3.10.1  2009/08/21 19:29:38  wenger
// Peptide-CGI now creates the new "all-in-one" SPARTA visualization.
// But some existing tests fail -- DON'T MERGE UNTIL THAT IS FIXED.
// (Tagging with s2d_sparta_deltashift_br_1 before this commit,
// s2d_sparta_deltashift_br_2 after.)
//
// Revision 1.3  2007/08/20 20:26:07  wenger
// Added -verb command-line flag and property so we can turn on debug
// output without recompiling; added debug_level property corresponding
// to the existing -debug command-line flag.
//
// Revision 1.2  2006/02/01 20:23:11  wenger
// Merged V2_1b4_br_0 thru peptide_cgi_10_8_0_base to the
// trunk.
//
// Revision 1.1.2.2  2004/01/30 18:20:41  wenger
// S2D errors and warnings are now saved to files (the files are
// named according to the data name).
//
// Revision 1.1.2.1  2003/04/22 21:58:14  wenger
// Added package name to peptide-cgi java code and put everything into
// a single jar file; version is now 6.0.
//
// Revision 1.1.4.1  2003/01/14 16:51:48  wenger
// Merged V2_1b3_br_0 thru V2_1b3_br_end to V2_1b4_br.
//
// Revision 1.1.2.1  2002/11/04 18:05:50  wenger
// Forgot to commit this earlier....
//

// ========================================================================

package star2devise;

import java.io.*;

public class S2DFileWriter extends FileWriter {
    //===================================================================
    // VARIABLES

    private static final int DEBUG = 0;

    //===================================================================
    // PUBLIC METHODS

    //-------------------------------------------------------------------
    public static S2DFileWriter create(String fileName) throws IOException
    {
        S2DUtils.deleteFile(fileName);

        File file = new File(fileName);

        return new S2DFileWriter(file);
    }

    //-------------------------------------------------------------------
    public static S2DFileWriter append(String fileName) throws IOException
    {
        return new S2DFileWriter(fileName, true);
    }

    //===================================================================
    // PRIVATE METHODS

    //-------------------------------------------------------------------
    // Constructor.
    private S2DFileWriter(File file) throws IOException
    {
        super(file);
    }

    //-------------------------------------------------------------------
    // Constructor.
    private S2DFileWriter(String fileName, boolean append) throws IOException
    {
        super(fileName, append);
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

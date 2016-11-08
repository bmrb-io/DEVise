// ========================================================================
// DEVise Data Visualization Software
// (c) Copyright 2006-2010
// By the DEVise Development Group
// Madison, Wisconsin
// All Rights Reserved.
// ========================================================================

// Under no circumstances is this software to be copied, distributed,
// or altered in any way without prior permission from the DEVise
// Development Group.

// ------------------------------------------------------------------------

// This class is used to generate the summary page for the normal-size
// visualizations.

// Note that almost all of the work is done in S2DSummaryGen.

// ------------------------------------------------------------------------

// $Id$

// $Log$
// Revision 1.6.18.3  2010/12/29 17:49:37  wenger
// The multi-entry code now gets frame index, entity assembly ID, and
// peak count info from comments in the single-entry summary HTML files.
//
// Revision 1.6.18.2  2010/12/21 00:24:45  wenger
// Got rid of 'sizeString' stuff in summary HTML code; started putting
// multi-entry summary page links into a table.
//
// Revision 1.6.18.1  2010/12/07 23:43:50  wenger
// Merged s2d_multi_entry_br_0 thru s2d_multi_entry_br_1 to
// s2d_multi_entry2_br.
//
// Revision 1.6.14.2  2010/12/04 00:34:55  wenger
// Got preliminary multi-entry summary page working.
//
// Revision 1.6.14.1  2010/11/16 00:01:18  wenger
// We now create a "two-entry" summary HTML page (but it doesn't have the
// right links yet); added "two-entry" HTML pages to the tests.
//
// Revision 1.6  2010/01/21 16:32:15  wenger
// Merged s2d_pdb_only_tar_1001_br_0 thru s2d_pdb_only_tar_1001_br_end
// to trunk.
//
// Revision 1.5.16.2  2010/01/08 23:28:33  wenger
// Fixed 'Force reprocessing' functionality for restraint-only summary
// page; fixed error message in make_view for restraint-only processing.
//
// Revision 1.5.16.1  2010/01/08 16:29:30  wenger
// Added changes to make the summary html file have only the relevant
// things for restraint-only processing.
//
// Revision 1.5  2008/06/04 21:12:01  wenger
// New Peptide-CGI summary page is implemented, test work except for
// test52 for some weird reason.  (Still may need some other changes
// before release, though.)
//
// Revision 1.4  2008/04/09 19:35:42  wenger
// Added frame details to individual visualization pages in preparation
// for summary page changes; spelled out Linear Analysis of Chemical
// Shifts; removed some unneeded parameters from the S2DSummaryHtml*
// constructors.
//
// Revision 1.3  2007/08/20 20:26:10  wenger
// Added -verb command-line flag and property so we can turn on debug
// output without recompiling; added debug_level property corresponding
// to the existing -debug command-line flag.
//
// Revision 1.2  2007/03/07 16:37:59  wenger
// Phase 2 of "upload and visualize data" -- mostly working, I think,
// but still needs chem shift reference capability and hasn't been
// hooked up to Dimitri's upload scripts yet.
//
// Revision 1.1  2006/08/21 21:01:11  wenger
// Added second summary page for direct access to all large-size
// visualizations; updated all tests accordingly.
//

// ========================================================================

package star2devise;

import java.io.*;
import java.util.*;

public class S2DSummaryHtmlNormal extends S2DSummaryHtmlGen {
    //===================================================================
    // VARIABLES

    private static final int DEBUG = 0;

    //===================================================================
    // PUBLIC METHODS

    //-------------------------------------------------------------------
    // Constructor.  Opens the html file and writes the header.
    public S2DSummaryHtmlNormal(String name, String fullName,
                                String longName, String masterId, Vector localFiles,
                                String htmlDir, boolean restraintOnly) throws S2DException
    {
        super(name, fullName, longName, masterId, localFiles, htmlDir,
              restraintOnly);

        if (doDebugOutput(11)) {
            System.out.println("S2DSummaryHtmlNormal.S2DSummaryHtmlNormal(" +
                               name + ", " + masterId + ")");
        }
    }

    //-------------------------------------------------------------------
    // Constructor for multi-entry summary page.  Opens the html file and
    // writes the header.
    public S2DSummaryHtmlNormal(String name, String fullName,
                                String longName, String masterId, String extraId, Vector localFiles,
                                String htmlDir, boolean restraintOnly) throws S2DException
    {
        super(name, fullName, longName, masterId, extraId, localFiles,
              htmlDir, restraintOnly);

        if (doDebugOutput(11)) {
            System.out.println("S2DSummaryHtmlNormal.S2DSummaryHtmlNormal(" +
                               name + ", " + masterId + ", " + extraId + ")");
        }
    }

    //===================================================================
    // PROTECTED METHODS

    //-------------------------------------------------------------------
    // Write the link to the other size of visualizations.
    protected void writeSizeLink() throws IOException
    {
        _writer.write("Normal-size visualizations on this page; " +
                      "<a href=\"" + _sibling.fileNameShort() +
                      "\">large</a> visualizations\n");
    }

    //===================================================================
    // PRIVATE METHODS

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

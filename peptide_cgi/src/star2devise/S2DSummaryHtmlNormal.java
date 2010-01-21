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
    public String sizeString()
    {
        return "";
    }

    //-------------------------------------------------------------------
    // Constructor.  Opens the html file and writes the header.
    public S2DSummaryHtmlNormal(String name, String longName,
      String masterId, Vector localFiles, String htmlDir,
      boolean restraintOnly) throws S2DException
    {
	super(name, longName, masterId, localFiles, htmlDir,
	  restraintOnly);

        if (doDebugOutput(11)) {
	    System.out.println("S2DSummaryHtmlNormal.S2DSummaryHtmlNormal(" +
	      name + ", " + masterId + ")");
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

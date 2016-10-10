// ========================================================================
// DEVise Data Visualization Software
// (c) Copyright 2001-2008
// By the DEVise Development Group
// Madison, Wisconsin
// All Rights Reserved.
// ========================================================================

// Under no circumstances is this software to be copied, distributed,
// or altered in any way without prior permission from the DEVise
// Development Group.

// ------------------------------------------------------------------------

// This class is used to write the error html file if a chemical shift
// reference calculation fails.

// ------------------------------------------------------------------------

// $Id$

// $Log$
// Revision 1.2.34.1  2010/12/07 23:43:49  wenger
// Merged s2d_multi_entry_br_0 thru s2d_multi_entry_br_1 to
// s2d_multi_entry2_br.
//
// Revision 1.2.30.1  2010/11/13 00:05:24  wenger
// Basic creation of session-specific HTML pages for two-entry
// visualizations is now in place (includes removing some leftover
// provisions for the "large" specific HTML files).
//
// Revision 1.2  2008/04/09 19:35:41  wenger
// Added frame details to individual visualization pages in preparation
// for summary page changes; spelled out Linear Analysis of Chemical
// Shifts; removed some unneeded parameters from the S2DSummaryHtml*
// constructors.
//
// Revision 1.1  2007/12/20 16:49:02  wenger
// Improved ChemShiftRef error messages; ChemShift calculation failing
// is no longer considered an error at the top level of the program;
// S2DSpecificHtml methods are no longer static so new S2DCSRErrorHtml
// class could inherit from it correctly; some cache checking output
// is now printed at a lower versbosity setting.
//

// ========================================================================

package star2devise;

import java.io.*;

public class S2DCSRErrorHtml extends S2DSpecificHtml {
    //===================================================================
    // VARIABLES

    private static final int DEBUG = 0;

    protected String _bmrbId;
    protected String _pdbId;
    protected boolean _timedOut;

    //===================================================================
    // PUBLIC METHODS

    //-------------------------------------------------------------------
    // Constructor.
    public S2DCSRErrorHtml(String htmlDir, int dataType, String name,
                           int frameIndex, String title, String bmrbId, String pdbId,
                           boolean timedOut, String frameDetails)
    throws S2DError
    {
        super(htmlDir, dataType, name, frameIndex, title, frameDetails);

        if (doDebugOutput(11)) {
            System.out.println("S2DCSRErrorHtml.S2DCSRErrorHtml(" +
                               htmlDir + ", " + dataType + ", " + name + ", " +
                               frameIndex + ")");
        }

        _bmrbId = bmrbId;
        _pdbId = pdbId;
        _timedOut = timedOut;
    }

    //===================================================================
    // PROTECTED METHODS

    //-------------------------------------------------------------------
    // Get the template file name.
    protected String TemplateFileName()
    {
        String templateFile = "html_templates" + File.separator;
        if (_timedOut) {
            templateFile += "csr_timeout.base";
        } else {
            templateFile += "csr_error.base";
        }

        return templateFile;
    }

    //-------------------------------------------------------------------
    // Filter one line of the HTML template file, transforming it
    // into a line to be output.
    protected String FilterLine(String inLine)
    {
        String line = inLine;
        line = S2DUtils.replace(line, "BMRBID", _bmrbId);
        line = S2DUtils.replace(line, "PDBID", _pdbId);

        return line;
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

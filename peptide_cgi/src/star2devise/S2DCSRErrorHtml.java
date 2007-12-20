// ========================================================================
// DEVise Data Visualization Software
// (c) Copyright 2001-2007
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
      boolean timedOut)
      throws S2DError
    {
	super(htmlDir, dataType, name, frameIndex, title);

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
    protected String TemplateFileName(boolean isLarge)
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

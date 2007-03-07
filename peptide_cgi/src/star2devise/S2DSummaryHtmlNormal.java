// ========================================================================
// DEVise Data Visualization Software
// (c) Copyright 2006-2007
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
      String accessionNum, Vector localFiles, String htmlDir,
      String starFileName, String systemName, String frameTitle)
      throws S2DException
    {
	super(name, longName, accessionNum, localFiles, htmlDir,
	  starFileName, systemName, frameTitle);

        if (DEBUG >= 1) {
	    System.out.println("S2DSummaryHtmlNormal.S2DSummaryHtmlNormal(" +
	      name + ", " + accessionNum + ")");
	}
    }
}

// ========================================================================

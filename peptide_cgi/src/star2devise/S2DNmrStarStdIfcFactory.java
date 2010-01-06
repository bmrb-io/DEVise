// ========================================================================
// DEVise Data Visualization Software
// (c) Copyright 2000-2010
// By the DEVise Development Group
// Madison, Wisconsin
// All Rights Reserved.
// ========================================================================

// Under no circumstances is this software to be copied, distributed,
// or altered in any way without prior permission from the DEVise
// Development Group.

// ------------------------------------------------------------------------

// This class defines objects that are used as factories for creating
// S2DNMRStar21Ifc, S2DNmrStar30Ifc, and S2DNmrStar31Ifc objects.  (The
// methods to create the NMRStarIfc objects are non-static so that they
// can inherit from related factory classes.)

// ------------------------------------------------------------------------

// $Id$

// $Log$
// Revision 1.1.2.2  2010/01/06 22:21:57  wenger
// Did a bunch of cleanups on the distance restraint code, in preparation
// for merging it to the trunk, so I can suspend work on it and move to
// working on the "PDB-ID-only" torsion angle restraint visualizations.
//
// Revision 1.1.2.1  2010/01/04 23:59:18  wenger
// Finished moving creation of all S2DNmrStar*Ifc objects into the new
// factory classes.
//

// ========================================================================

package star2devise;

import EDU.bmrb.starlibj.*;
import java.io.*;
import java.util.*;
import java.net.*;

public class S2DNmrStarStdIfcFactory extends S2DNmrStarIfcFactory {
    //===================================================================
    // VARIABLES

    private static final int DEBUG = 0;

    //===================================================================
    // PUBLIC METHODS

    //-------------------------------------------------------------------
    public String getFileName(String bmrbId)
    {
	String name = S2DUtils.replace(S2DNames.STAR_NAME_TEMPLATE, "*",
	      bmrbId);

	return name;
    }

    //-------------------------------------------------------------------
    public String getURLName(String fileName)
    {
	String url = S2DNames.BMRB_STAR_URL + fileName;

	return url;
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

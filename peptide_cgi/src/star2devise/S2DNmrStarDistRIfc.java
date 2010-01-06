// ========================================================================
// DEVise Data Visualization Software
// (c) Copyright 2009
// By the DEVise Development Group
// Madison, Wisconsin
// All Rights Reserved.
// ========================================================================

// Under no circumstances is this software to be copied, distributed,
// or altered in any way without prior permission from the DEVise
// Development Group.

// ------------------------------------------------------------------------

//TEMPTEMP -- change
// This class contains NMR-Star access methods, etc., specific to NMR-STAR/
// torsion angle restraint files.

// ------------------------------------------------------------------------

// $Id$

// $Log$
// Revision 1.1.2.4  2010/01/04 23:59:18  wenger
// Finished moving creation of all S2DNmrStar*Ifc objects into the new
// factory classes.
//
// Revision 1.1.2.3  2010/01/04 19:31:06  wenger
// Made most S2DNmrStarIfcFactory methods non-static in preparation for
// splitting it up into various subclasses as appropriate.
//
// Revision 1.1.2.2  2010/01/04 18:57:00  wenger
// Added new S2DNmrStarIfcFactory class as part 1 of cleaning up the
// creation of various S2D*Ifc objects.
//
// Revision 1.1.2.1  2009/12/16 00:07:56  wenger
// Added S2DDistRestraint and S2DNmrStarDistRIfc classes (mostly
// stubs so far); added a bunch of notes based on today's BMRB
// staff meeting discussions; also added stubbed-in classes in
// S2DSummaryHtml, etc.
//

// ========================================================================

package star2devise;

import EDU.bmrb.starlibj.*;
import java.io.*;
import java.util.*;
import java.net.*;

public class S2DNmrStarDistRIfc extends S2DNmrStarIfc {
    //===================================================================
    // VARIABLES

    private static final int DEBUG = 0;

    //===================================================================
    // PROTECTED METHODS
    //-------------------------------------------------------------------
    /**
     * Get the NMR-STAR file version corresponding to this object.
     * @return The NMR-STAR file version corresponding to this object.
     */
    public String version()
    {
    	return "distance_restraint";
    }

    //-------------------------------------------------------------------
    // Constructor.  Constructs an S2DNmrStarDistRIfc object corresponding to
    // the STAR file represented by starTree.

    protected S2DNmrStarDistRIfc(StarNode starTree) throws S2DException
    {
        super(starTree);

        if (doDebugOutput(11)) {
	    System.out.println("S2DNmrStarDistRIfc.S2DNmrStarDistRIfc()");
	}

	setStarNames();
    }

    //===================================================================
    // PRIVATE METHODS

    // ----------------------------------------------------------------------
    // Set the tag names and values to work for NMR-Star files.
    private void setStarNames()
    {
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

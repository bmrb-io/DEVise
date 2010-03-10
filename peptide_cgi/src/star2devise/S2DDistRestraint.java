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

// This class implements the output of distance restraint data.
// For each set of distance restraint data, it creates a data file, a
// session file, an individual html file, and a link in the summary
// html file.

// ------------------------------------------------------------------------

// $Id$

// $Log$
// Revision 1.2  2010/01/06 23:03:39  wenger
// Merged s2d_dist_rest_0912_br_0 thru s2d_dist_rest_0912_br_1 to trunk.
//
// Revision 1.1.2.3  2010/01/06 22:21:56  wenger
// Did a bunch of cleanups on the distance restraint code, in preparation
// for merging it to the trunk, so I can suspend work on it and move to
// working on the "PDB-ID-only" torsion angle restraint visualizations.
//
// Revision 1.1.2.2  2010/01/05 15:33:51  wenger
// Cleaned up the pdbIdToUrl() methods for various restraint-related
// classes.
//
// Revision 1.1.2.1  2009/12/16 00:07:56  wenger
// Added S2DDistRestraint and S2DNmrStarDistRIfc classes (mostly
// stubs so far); added a bunch of notes based on today's BMRB
// staff meeting discussions; also added stubbed-in classes in
// S2DSummaryHtml, etc.
//

// ========================================================================

/*TEMP>>>
Distance restraint to-dos:
* Use data from restraint grid if possible (if it exists and we can parse
it) because it has violation information; if we can't do that, use data
from the remediated restraints.

* classification of distance restraints:
1) intra-residue
2) sequential (|i-j|=1)
3) medium-range (|i-j|<5)
4) long-range (|i-j|>=5)
5) inter-molecular

* ORs indicate ambiguous restraints; others are simple

* add graphs for number of restraints and number of violated restraints
per residue

* can have 10-30 restraints/residue

* look at old mockups of distance restraint visualization -- maybe go
back to non-Jmol if it's too hard to display restraints
<<<TEMP*/

package star2devise;

import EDU.bmrb.starlibj.SaveFrameNode;
import java.io.*;
import java.util.*;
import java.net.*;

public class S2DDistRestraint extends S2DRestraint {
    //===================================================================
    // VARIABLES

    private static final int DEBUG = 0;

    //===================================================================
    // PUBLIC METHODS

    //-------------------------------------------------------------------
    // Constructor.
    public S2DDistRestraint(String name, String longName,
      S2DNmrStarDistRIfc star, String dataDir, String sessionDir,
      S2DSummaryHtml summary, String pdbId) throws S2DException
    {
	super(name, longName, star, dataDir, sessionDir, summary, pdbId);

        if (doDebugOutput(11)) {
	    System.out.println("S2DDistRestraint.S2DDistRestraint(" +
	      name + ")");
	}

	//TEMP -- find the general_distance_constraints save frame... with a _Gen_dist_constraint.ID loop.

	//TEMP -- real code here...
    }

    //-------------------------------------------------------------------
    // Write the distance restraint values for this data.
    public void writeRestraints(int frameIndex) throws S2DException
    {
        if (doDebugOutput(11)) {
	    System.out.println("S2DDistRestraint.writeRestraints()");
	}

	//TEMP -- real code here
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

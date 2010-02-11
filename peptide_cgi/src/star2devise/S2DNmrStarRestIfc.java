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

//TEMPTEMP -- generic restraints
// This class contains NMR-Star access methods, etc., specific to NMR-STAR/
// torsion angle restraint files.

// ------------------------------------------------------------------------

// $Id$

// $Log$
// Revision 1.1.2.1  2010/02/03 21:38:59  wenger
// Made S2DNmrStarDistRIfc, S2DNmrStarRRIfc, and S2DNmrStarTarIfc
// subclasses of new S2DNmrStarRestIfc class, so that I can pass
// either a S2DNmrStarTarIfc or a S2DNmrStarRRIfc to the
// S2DTorsionAngle constructor.
//
//

// ========================================================================

package star2devise;

import EDU.bmrb.starlibj.*;
import java.io.*;
import java.util.*;
import java.net.*;

public abstract class S2DNmrStarRestIfc extends S2DNmrStarIfc {
    //===================================================================
    // VARIABLES

    public String TAR_ANGLE_AVERAGE = "";
    public String TAR_ANGLE_LOWER_BOUND = "";
    public String TAR_ANGLE_UPPER_BOUND = "";
    public String TAR_ATOM_ID_1 = "";
    public String TAR_ATOM_ID_2 = "";
    public String TAR_ATOM_ID_3 = "";
    public String TAR_ATOM_ID_4 = "";

    public String TAR_CONSTRAINT_STATS_SF_CAT = "";
    public String TAR_CONSTRAINT_STATS_CAT_NAME = "";

    public String TAR_ENT_ASSEM_1 = "";
    public String TAR_ENT_ASSEM_2 = "";
    public String TAR_ENT_ASSEM_3 = "";
    public String TAR_ENT_ASSEM_4 = "";

    public String TAR_MAX_VIOLATION = "";

    public String TAR_RES_SEQ_CODE_1 = "";
    public String TAR_RES_SEQ_CODE_2 = "";
    public String TAR_RES_SEQ_CODE_3 = "";
    public String TAR_RES_SEQ_CODE_4 = "";
    public String TAR_RES_SEQ_LABEL_1 = "";
    public String TAR_RES_SEQ_LABEL_2 = "";
    public String TAR_RES_SEQ_LABEL_3 = "";
    public String TAR_RES_SEQ_LABEL_4 = "";
    public String TAR_RESTRAINT_ID = "";

    private static final int DEBUG = 0;

    //===================================================================
    // PROTECTED METHODS
    //-------------------------------------------------------------------
    // Constructor.  Constructs an S2DNmrStarRestIfc object corresponding to
    // the STAR file represented by starTree.

    protected S2DNmrStarRestIfc(StarNode starTree) throws S2DException
    {
        super(starTree);

        if (doDebugOutput(11)) {
	    System.out.println("S2DNmrStarRestIfc.S2DNmrStarRestIfc()");
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

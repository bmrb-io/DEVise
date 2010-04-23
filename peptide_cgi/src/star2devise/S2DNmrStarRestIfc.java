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

// This class contains NMR-Star access methods, etc., general to all
// NMR-STAR restraint files.

// ------------------------------------------------------------------------

// $Id$

// $Log$
// Revision 1.2.2.6  2010/04/22 18:29:20  wenger
// Eliminated multiple counting of ambiguous restraints; various fixes
// to the distance restraint tests.
//
// Revision 1.2.2.5  2010/04/22 16:38:11  wenger
// Various minor cleanups of distance restraint code.
//
// Revision 1.2.2.4  2010/03/09 20:36:47  wenger
// Added ambiguous and non-ambiguous restraint counts to data.
//
// Revision 1.2.2.3  2010/03/03 21:52:29  wenger
// Added processing of distance constraint type (e.g., "hydrogen bond",
// "NOE", etc.).
//
// Revision 1.2.2.2  2010/02/24 20:03:06  wenger
// Got basic generation of distance restraint data from the restraints
// grid mostly working.
//
// Revision 1.2.2.1  2010/02/24 16:35:20  wenger
// Torsion angle restraints now work with new S2DNmrStarRGIfc and
// S2DNmrStarRGIfcFactory classes.
//
// Revision 1.2  2010/02/11 22:13:11  wenger
// Merged s2d_remediated_rest_1002_br_0 thru s2d_remediated_rest_1002_br_1
// to trunk (note: s2d_remediated_rest_1002_br_1 ==
// s2d_remediated_rest_1002_br_end).
//
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

    // Distance restraints.
    public String DISTR_ATOM_ID_1 = "";
    public String DISTR_ATOM_ID_2 = "";
    public String DISTR_AVERAGE = "";

    public String DISTR_CONSTRAINT_STATS_SF_CAT = "";
    public String DISTR_CONSTRAINT_STATS_CAT_NAME = "";
    public String DISTR_CONSTRAINT_TYPE = "";

    public String DISTR_ENT_ASSEM_1 = "";
    public String DISTR_ENT_ASSEM_2 = "";

    public String DISTR_MAX = "";
    public String DISTR_MAX_VIOLATION = "";
    public String DISTR_MEMBER_ID = "";
    public String DISTR_MEMBER_LOGIC_CODE = "";
    public String DISTR_MIN = "";

    public String DISTR_RES_SEQ_CODE_1 = "";
    public String DISTR_RES_SEQ_CODE_2 = "";
    public String DISTR_RES_SEQ_LABEL_1 = "";
    public String DISTR_RES_SEQ_LABEL_2 = "";
    public String DISTR_RESTRAINT_ID = "";

    // Torsion angle restraints.
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

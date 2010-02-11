// ========================================================================
// DEVise Data Visualization Software
// (c) Copyright 2009-2010
// By the DEVise Development Group
// Madison, Wisconsin
// All Rights Reserved.
// ========================================================================

// Under no circumstances is this software to be copied, distributed,
// or altered in any way without prior permission from the DEVise
// Development Group.

// ------------------------------------------------------------------------

// This class contains NMR-Star access methods, etc., specific to NMR-STAR/
// torsion angle restraint files.

// ------------------------------------------------------------------------

// $Id$

// $Log$
// Revision 1.3.4.1  2010/02/03 21:38:59  wenger
// Made S2DNmrStarDistRIfc, S2DNmrStarRRIfc, and S2DNmrStarTarIfc
// subclasses of new S2DNmrStarRestIfc class, so that I can pass
// either a S2DNmrStarTarIfc or a S2DNmrStarRRIfc to the
// S2DTorsionAngle constructor.
//
// Revision 1.3  2010/01/06 23:03:40  wenger
// Merged s2d_dist_rest_0912_br_0 thru s2d_dist_rest_0912_br_1 to trunk.
//
// Revision 1.2.2.5  2010/01/04 23:59:18  wenger
// Finished moving creation of all S2DNmrStar*Ifc objects into the new
// factory classes.
//
// Revision 1.2.2.4  2010/01/04 19:31:06  wenger
// Made most S2DNmrStarIfcFactory methods non-static in preparation for
// splitting it up into various subclasses as appropriate.
//
// Revision 1.2.2.3  2010/01/04 18:57:00  wenger
// Added new S2DNmrStarIfcFactory class as part 1 of cleaning up the
// creation of various S2D*Ifc objects.
//
// Revision 1.2.2.2  2009/12/16 00:07:56  wenger
// Added S2DDistRestraint and S2DNmrStarDistRIfc classes (mostly
// stubs so far); added a bunch of notes based on today's BMRB
// staff meeting discussions; also added stubbed-in classes in
// S2DSummaryHtml, etc.
//
// Revision 1.2.2.1  2009/12/14 23:31:03  wenger
// Added new S2DRestraint class to be parent of S2DTorsionAngle and
// (not yet created) S2DDistRestraint class; moved a bunch of the
// code from S2DTorsionAngle to S2DRestraint.
//
// Revision 1.2  2009/12/05 22:26:32  wenger
// Merged s2d_torsion_rest_0910_br_0 thru s2d_torsion_rest_0910_br_0
// to the trunk.
//
// Revision 1.1.2.6  2009/12/05 00:30:24  wenger
// Partway to getting angles assigned to correct residues -- fixed
// atom/residue pairings, removed some unused fields from the torsion
// angle output.
//
// Revision 1.1.2.5  2009/12/04 22:19:51  wenger
// Implemented cache checking for torsion angle restraints.
//
// Revision 1.1.2.4  2009/12/01 20:48:03  wenger
// Real code for getting torsion angle data by PDB ID is now in
// place -- still lots of temporary code within that, though.
//
// Revision 1.1.2.3  2009/12/01 18:32:17  wenger
// Test stub code for retreiving torsion angle data by PDB ID is now in
// place.
//
// Revision 1.1.2.2  2009/12/01 18:02:07  wenger
// Torsion angle data can now be retreived by URL from the restraint grid.
//
// Revision 1.1.2.1  2009/11/24 23:59:02  wenger
// Added the S2DNmrStarTarIfc to properly deal with the torsion angle-
// related tags.  Note: 3D tests fail because they're not yet updated
// for the new Atom_ID field.
//

// ========================================================================

package star2devise;

import EDU.bmrb.starlibj.*;
import java.io.*;
import java.util.*;
import java.net.*;

public class S2DNmrStarTarIfc extends S2DNmrStarRestIfc {
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
    	return "torsion_angle_restraint";
    }

    //-------------------------------------------------------------------
    // Constructor.  Constructs an S2DNmrStarTarIfc object corresponding to
    // the STAR file represented by starTree.

    protected S2DNmrStarTarIfc(StarNode starTree) throws S2DException
    {
        super(starTree);

        if (doDebugOutput(11)) {
	    System.out.println("S2DNmrStarTarIfc.S2DNmrStarTarIfc()");
	}

	setStarNames();
    }

    //===================================================================
    // PRIVATE METHODS

    // ----------------------------------------------------------------------
    // Set the tag names and values to work for NMR-Star files.
    private void setStarNames()
    {
        TAR_ANGLE_AVERAGE = "_TA_constraint_stats.Angle_average";
        TAR_ANGLE_LOWER_BOUND = "_TA_constraint_stats.Angle_lower_bound_val";
        TAR_ANGLE_UPPER_BOUND = "_TA_constraint_stats.Angle_upper_bound_val";
        TAR_ATOM_ID_1 = "_TA_constraint_stats.Atom_ID_1";
        TAR_ATOM_ID_2 = "_TA_constraint_stats.Atom_ID_2";
        TAR_ATOM_ID_3 = "_TA_constraint_stats.Atom_ID_3";
        TAR_ATOM_ID_4 = "_TA_constraint_stats.Atom_ID_4";

        TAR_CONSTRAINT_STATS_SF_CAT = "_TA_constraint_stats_list.Sf_category";
        TAR_CONSTRAINT_STATS_CAT_NAME = "torsion_angle_constraint_statistics";

        TAR_ENT_ASSEM_1 = "_TA_constraint_stats.Entity_assembly_ID_1";
        TAR_ENT_ASSEM_2 = "_TA_constraint_stats.Entity_assembly_ID_2";
        TAR_ENT_ASSEM_3 = "_TA_constraint_stats.Entity_assembly_ID_3";
        TAR_ENT_ASSEM_4 = "_TA_constraint_stats.Entity_assembly_ID_4";

        TAR_MAX_VIOLATION = "_TA_constraint_stats.Max_violation";

        TAR_RES_SEQ_CODE_1 = "_TA_constraint_stats.Comp_index_ID_1";
        TAR_RES_SEQ_CODE_2 = "_TA_constraint_stats.Comp_index_ID_2";
        TAR_RES_SEQ_CODE_3 = "_TA_constraint_stats.Comp_index_ID_3";
        TAR_RES_SEQ_CODE_4 = "_TA_constraint_stats.Comp_index_ID_4";
        TAR_RES_SEQ_LABEL_1 = "_TA_constraint_stats.Comp_ID_1";
        TAR_RES_SEQ_LABEL_2 = "_TA_constraint_stats.Comp_ID_2";
        TAR_RES_SEQ_LABEL_3 = "_TA_constraint_stats.Comp_ID_3";
        TAR_RES_SEQ_LABEL_4 = "_TA_constraint_stats.Comp_ID_4";
        TAR_RESTRAINT_ID = "_TA_constraint_stats.Restraint_ID";
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

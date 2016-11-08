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
// restraint grid files (for both torsion angles and distance restraints).

// ------------------------------------------------------------------------

// $Id$

// $Log$
// Revision 1.1.2.2  2010/02/24 20:03:06  wenger
// Got basic generation of distance restraint data from the restraints
// grid mostly working.
//
// Revision 1.1.2.1  2010/02/24 16:35:20  wenger
// Torsion angle restraints now work with new S2DNmrStarRGIfc and
// S2DNmrStarRGIfcFactory classes.
//

// ========================================================================

package star2devise;

import EDU.bmrb.starlibj.*;
import java.io.*;
import java.util.*;
import java.net.*;

public class S2DNmrStarRGIfc extends S2DNmrStarRestIfc {
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
        return "restraint_grid";
    }

    //-------------------------------------------------------------------
    // Constructor.  Constructs an S2DNmrStarRGIfc object corresponding to
    // the STAR file represented by starTree.

    protected S2DNmrStarRGIfc(StarNode starTree) throws S2DException
    {
        super(starTree);

        if (doDebugOutput(11)) {
            System.out.println("S2DNmrStarRGIfc.S2DNmrStarRGIfc()");
        }

        setStarNames();
    }

    //===================================================================
    // PRIVATE METHODS

    // ----------------------------------------------------------------------
    // Set the tag names and values to work for NMR-Star files.
    private void setStarNames()
    {
        // Distance restraints.
        DISTR_ATOM_ID_1 = "_Distance_constraint_stats.Atom_1_ID";
        DISTR_ATOM_ID_2 = "_Distance_constraint_stats.Atom_2_ID";
        DISTR_AVERAGE = "_Distance_constraint_stats.Distance_average";

        DISTR_CONSTRAINT_STATS_SF_CAT =
            "_Distance_constraint_stats_list.Sf_category";
        DISTR_CONSTRAINT_STATS_CAT_NAME = "distance_constraint_statistics";

        DISTR_ENT_ASSEM_1 =
            "_Distance_constraint_stats.Atom_1_entity_assembly_ID";
        DISTR_ENT_ASSEM_2 =
            "_Distance_constraint_stats.Atom_2_entity_assembly_ID";

        DISTR_MAX = "_Distance_constraint_stats.Distance_maximum";
        DISTR_MAX_VIOLATION =
            "_Distance_constraint_stats.Max_violation";
        DISTR_MIN = "_Distance_constraint_stats.Distance_minimum";

        DISTR_RES_SEQ_CODE_1 =
            "_Distance_constraint_stats.Atom_1_comp_index_ID";
        DISTR_RES_SEQ_CODE_2 =
            "_Distance_constraint_stats.Atom_2_comp_index_ID";
        DISTR_RES_SEQ_LABEL_1 = "_Distance_constraint_stats.Atom_1_comp_ID";
        DISTR_RES_SEQ_LABEL_2 = "_Distance_constraint_stats.Atom_2_comp_ID";
        DISTR_RESTRAINT_ID = "_Distance_constraint_stats.Restraint_ID";

        // Torsion angle restraints.
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

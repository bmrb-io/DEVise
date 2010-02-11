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

//TEMPTEMP -- change
// This class contains NMR-Star access methods, etc., specific to NMR-STAR/
// torsion angle restraint files.

// ------------------------------------------------------------------------

// $Id$

// $Log$
// Revision 1.1.2.6  2010/02/09 22:41:21  wenger
// Mostly done getting coordinates from remediated restraint files --
// seems to work, but still needs some checking.
//
// Revision 1.1.2.5  2010/02/05 20:46:13  wenger
// Partially implemented getting remediated restraints file template from
// properties.
//
// Revision 1.1.2.4  2010/02/03 23:13:19  wenger
// Torsion angle output from remediated restraints files now has different
// suffixes to avoid conflict with the restraint grid output; meta-data
// for remediated restraints doesn't have violations
//
// Revision 1.1.2.3  2010/02/03 22:02:47  wenger
// First version of S2DNmrStarRRIfc with actual tag values -- processing
// seems to mostly work.
//
// Revision 1.1.2.2  2010/02/03 21:38:59  wenger
// Made S2DNmrStarDistRIfc, S2DNmrStarRRIfc, and S2DNmrStarTarIfc
// subclasses of new S2DNmrStarRestIfc class, so that I can pass
// either a S2DNmrStarTarIfc or a S2DNmrStarRRIfc to the
// S2DTorsionAngle constructor.
//
// Revision 1.1.2.1  2010/02/03 20:57:00  wenger
// Blocked in code for getting remediated restraints from a specified file.
//

// ========================================================================

package star2devise;

import EDU.bmrb.starlibj.*;
import java.io.*;
import java.util.*;
import java.net.*;

public class S2DNmrStarRRIfc extends S2DNmrStarRestIfc {
    //===================================================================
    // VARIABLES
    protected String REP_CONF_ENTITY_ASSEMBLY_ID = "";

    private static final int DEBUG = 0;

    //===================================================================
    // PUBLIC METHODS
    //-------------------------------------------------------------------
//TEMPTEMP -- make sure this is correct!
    /**
     * Get entity assembly IDs for the coordinates in the given save frame.
     * @param frame: the save frame to search
     * @return an array of the entity assembly IDs
     */
    public int[] getCoordEntityAssemblyIDs(SaveFrameNode frame)
      throws S2DException
    {
	String[] entAssemIDStrs = getFrameValues(frame, ATOM_COORD_X,
	  REP_CONF_ENTITY_ASSEMBLY_ID);
    	int [] entAssemIDVals = S2DUtils.arrayStr2Int(entAssemIDStrs,
	  REP_CONF_ENTITY_ASSEMBLY_ID);

    	return entAssemIDVals;
    }

    //===================================================================
    // PROTECTED METHODS
//TEMPTEMP -- should this be public?
    //-------------------------------------------------------------------
    /**
     * Get the NMR-STAR file version corresponding to this object.
     * @return The NMR-STAR file version corresponding to this object.
     */
    public String version()
    {
    	return "remediated_restraint";
    }

    //-------------------------------------------------------------------
    // Constructor.  Constructs an S2DNmrStarRRIfc object corresponding to
    // the STAR file represented by starTree.

    protected S2DNmrStarRRIfc(StarNode starTree) throws S2DException
    {
        super(starTree);

        if (doDebugOutput(11)) {
	    System.out.println("S2DNmrStarRRIfc.S2DNmrStarRRIfc()");
	}

	setStarNames();
    }

    //===================================================================
    // PRIVATE METHODS

    // ----------------------------------------------------------------------
    // Set the tag names and values to work for NMR-Star files.
    private void setStarNames()
    {
	//TEMPTEMP -- check these...
	ATOM_COORD_MODEL_NUM = "_Atom_site.Model_ID";
	ATOM_COORD_ATOM_NAME = "_Atom_site.Label_atom_ID";
	ATOM_COORD_ATOM_TYPE = "_Atom_site.Type_symbol";
	ATOM_COORD_RES_LABEL = "_Atom_site.Label_comp_ID";
	ATOM_COORD_RES_SEQ_CODE = "_Atom_site.Label_comp_index_ID";
	ATOM_COORD_X = "_Atom_site.Cartn_x";
	ATOM_COORD_Y = "_Atom_site.Cartn_y";
	ATOM_COORD_Z = "_Atom_site.Cartn_z";

	REP_CONF_ENTITY_ASSEMBLY_ID = "_Atom_site.Label_entity_assembly_ID";//TEMPTEMP?

        TAR_ANGLE_LOWER_BOUND =
	  "_Torsion_angle_constraint.Angle_lower_bound_val";
        TAR_ANGLE_UPPER_BOUND =
	  "_Torsion_angle_constraint.Angle_upper_bound_val";
        TAR_ATOM_ID_1 = "_Torsion_angle_constraint.Atom_ID_1";
        TAR_ATOM_ID_2 = "_Torsion_angle_constraint.Atom_ID_2";
        TAR_ATOM_ID_3 = "_Torsion_angle_constraint.Atom_ID_3";
        TAR_ATOM_ID_4 = "_Torsion_angle_constraint.Atom_ID_4";

        TAR_CONSTRAINT_STATS_SF_CAT =
	  "_Torsion_angle_constraint_list.Sf_category";
        TAR_CONSTRAINT_STATS_CAT_NAME = "torsion_angle_constraints";


        TAR_ENT_ASSEM_1 = "_Torsion_angle_constraint.Entity_assembly_ID_1";
        TAR_ENT_ASSEM_2 = "_Torsion_angle_constraint.Entity_assembly_ID_2";
        TAR_ENT_ASSEM_3 = "_Torsion_angle_constraint.Entity_assembly_ID_3";
        TAR_ENT_ASSEM_4 = "_Torsion_angle_constraint.Entity_assembly_ID_4";

        TAR_RES_SEQ_CODE_1 = "_Torsion_angle_constraint.Comp_index_ID_1";
        TAR_RES_SEQ_CODE_2 = "_Torsion_angle_constraint.Comp_index_ID_2";
        TAR_RES_SEQ_CODE_3 = "_Torsion_angle_constraint.Comp_index_ID_3";
        TAR_RES_SEQ_CODE_4 = "_Torsion_angle_constraint.Comp_index_ID_4";
        TAR_RES_SEQ_LABEL_1 = "_Torsion_angle_constraint.Comp_ID_1";
        TAR_RES_SEQ_LABEL_2 = "_Torsion_angle_constraint.Comp_ID_2";
        TAR_RES_SEQ_LABEL_3 = "_Torsion_angle_constraint.Comp_ID_3";
        TAR_RES_SEQ_LABEL_4 = "_Torsion_angle_constraint.Comp_ID_4";
        TAR_RESTRAINT_ID = "_Torsion_angle_constraint.ID";
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

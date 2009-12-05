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

// This class contains NMR-Star access methods, etc., specific to NMR-STAR/
// torsion angle restraint files.

// ------------------------------------------------------------------------

// $Id$

// $Log$
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

public class S2DNmrStarTarIfc extends S2DNmrStarIfc {
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
    // PUBLIC METHODS

    //-------------------------------------------------------------------
    // Get the modification date/time of the appropriate torsion angle
    // restraint file.
    // Note: this doesn't seem to work so far -- always returns a
    // timestamp of 0.  RKW 2009-12-04.

    public static Date getModDate(String pdbId)
    {
	Date date = null;
	try {
	    URL starfile = new URL(S2DTorsionAngle.pdbIdToUrl(pdbId));
	    URLConnection connection = starfile.openConnection();

	    long timestamp = connection.getLastModified();
	    date = new Date(timestamp);
	} catch (S2DException ex) {
	    System.err.println("S2DException: " + ex.toString());
        } catch (MalformedURLException ex) {
	    System.err.println("MalformedURLException: " + ex.toString());
        } catch (IOException ex) {
	    System.err.println("IOException: " + ex.toString());
	}

	return date;
    }



    //-------------------------------------------------------------------
    // Factory method to create an S2DNmrStarTarIfc object based on
    // PDB ID.
    public static S2DNmrStarTarIfc createFromId(String pdbId)
      throws S2DException
    {
        String urlName = S2DTorsionAngle.pdbIdToUrl(pdbId);

        S2DNmrStarTarIfc ifc = createFromUrl(urlName, pdbId);

	return ifc;
    }

    //-------------------------------------------------------------------
    // Factory method to create an S2DNmrStarTarIfc object based on
    // URL.
    public static S2DNmrStarTarIfc createFromUrl(String urlName,
      String pdbId) throws S2DException
    {
        if (doDebugOutput(11)) {
	    System.out.println("S2DNmrStarTarIfc.createFromUrl(" +
	      urlName + ")");
	}

	S2DNmrStarTarIfc ifc;

        try {
	    String tmpFileName =
	      S2DTorsionAngle.filterUrlToTempFile(urlName, pdbId);

	    ifc = (S2DNmrStarTarIfc)
	      createFromFile(tmpFileName, false, true);

	} catch(Exception ex) {
	    System.err.println("Exception (" + ex.toString() +
	      ") parsing NMR-STAR file");
	    String errMsg = "Unable to get data in star file " + urlName;
	    System.err.println(errMsg);
            throw new S2DError(errMsg);
	}

	return ifc;
    }

    //===================================================================
    // PROTECTED METHODS
    //-------------------------------------------------------------------
    /**
     * Get the NMR-STAR file version corresponding to this object.
     * @return The NMR-STAR file version corresponding to this object.
     */
    public String version()
    {
    	return "TAR";
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

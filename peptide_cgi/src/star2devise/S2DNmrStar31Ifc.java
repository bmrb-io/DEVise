// ========================================================================
// DEVise Data Visualization Software
// (c) Copyright 2007-2008
// By the DEVise Development Group
// Madison, Wisconsin
// All Rights Reserved.
// ========================================================================

// Under no circumstances is this software to be copied, distributed,
// or altered in any way without prior permission from the DEVise
// Development Group.

// ------------------------------------------------------------------------

// This class contains NMR-Star access methods, etc., specific to NMR-STAR
// version 3.1 files.

// ------------------------------------------------------------------------

// $Id$

// $Log$
// Revision 1.5  2008/02/20 17:41:07  wenger
// Committing (disabled) partially-implemented S2 Order visualization
// code and tests.
//
// Revision 1.4  2007/09/26 20:47:15  wenger
// Changed x.Seq_ID to x.Comp_index_ID for NMR-STAR 3.1 processing as
// per Eldon's request; added some more debug output.
//
// Revision 1.3  2007/08/20 20:26:08  wenger
// Added -verb command-line flag and property so we can turn on debug
// output without recompiling; added debug_level property corresponding
// to the existing -debug command-line flag.
//
// Revision 1.2  2007/01/12 18:41:06  wenger
// Merged for_chemshift_br_0 thru for_chemshift_br_1 to trunk.
//
// Revision 1.1.2.2  2007/01/09 22:48:20  wenger
// Peptide-CGI now works with all data types in 4267, at least (see
// test41) -- this includes capability to specially deal with optional
// values that are "?" in the file.  Still need to find out how to deal
// with Heternonuclear NOE labels.
//
// Revision 1.1.2.1  2007/01/08 21:59:22  wenger
// First version of NMR-STAR 3.1 capability -- added test38 to test
// this.
//

// ========================================================================

package star2devise;

import EDU.bmrb.starlibj.*;
import java.io.*;
import java.util.*;
import java.net.*;

public class S2DNmrStar31Ifc extends S2DNmrStar30Ifc {
    //===================================================================
    // VARIABLES

    private static final int DEBUG = 0;

    //===================================================================
    // PUBLIC METHODS

    //-------------------------------------------------------------------
    // Determine whether the STAR file represented by starTree is an
    // NMR-STAR 3.1 file.
    public boolean isNmrStar31()
    {
        boolean result = false;

	try {
	    SaveFrameNode frame = getOneDataFrameByCat(ENTRY_SF_CAT,
	      ENTRY_INFO);

	    String version = getTagValue(frame, NMR_STAR_VERSION);

            if (version.startsWith("3.1") ||
	      // Note trailing dot below...
	      // This test probably doesn't work for all possible NMR-STAR
	      // 3.x versions, but everything except some of my existing
	      // tests should actually be 3.1 as opposed to 3.0.  wenger
	      // 2007-01-08.
	      version.startsWith("3.0.")) {
	        result = true;
	    }

	} catch(S2DException ex) {
	    System.err.println("Warning: S2DException in isNmrStar31: " +
	      ex.toString());
	}

        if (doDebugOutput(11)) {
	    System.out.println("S2DNmrStar31Ifc.isNmrStar31() returns " +
	      result);
	}

    	return result;
    }

    //===================================================================
    // PROTECTED METHODS

    //-------------------------------------------------------------------
    // Constructor.  Constructs an S2DNmrStar30Ifc object corresponding to
    // the STAR file represented by starTree.
    protected S2DNmrStar31Ifc(StarNode starTree) throws S2DException
    {
        super(starTree);

        if (doDebugOutput(11)) {
	    System.out.println("S2DNmrStar31Ifc.S2DNmrStar31Ifc()");
	}

	setStarNames();
    }

    //===================================================================
    // PRIVATE METHODS

    // ----------------------------------------------------------------------
    // Set the tag names and values to work for NMR-Star files.
    private void setStarNames()
    {
	ATOM_COORD_RES_SEQ_CODE = "_Rep_conf.Comp_index_ID";

	CHEM_SHIFT_AMBIG_CODE = "_Atom_chem_shift.Ambiguity_code";
	CHEM_SHIFT_RES_SEQ_CODE = "_Atom_chem_shift.Comp_index_ID";
	CHEM_SHIFT_VALUE = "_Atom_chem_shift.Val";
	COUPLING_RES_SEQ_CODE_1 = "_Coupling_constant.Comp_index_ID_1";
	COUPLING_RES_SEQ_CODE_2 = "_Coupling_constant.Comp_index_ID_2";

	FIGURE_OF_MERIT = "_Atom_chem_shift.Assign_fig_of_merit";

	HETERONUCLEAR_NOE = "heteronucl_NOEs";
	HET_NOE_RES_SEQ_CODE = "_Heteronucl_NOE.Comp_index_ID_1";
	HET_NOE_VALUE = "_Heteronucl_NOE.Val";
	HET_NOE_VALUE_ERR = "_Heteronucl_NOE.Val_err";

	//TEMPTEMP -- make sure that all of these are correct
	ORDER_ATOM_NAME = "_Order_param.Atom_ID";
	ORDER_PARAMETERS = "order_parameters";
	ORDER_RES_LABEL = "_Order_param.Comp_ID";
	ORDER_RES_SEQ_CODE = "_Order_param.Seq_ID";
	ORDER_SF_CAT = "_Order_parameter_list.Sf_category";
	ORDER_VALUE = "_Order_param.Order_param_val";
	ORDER_VALUE_ERR = "_Order_param.Order_param_val_fit_err";

	T1_ATOM_NAME = "_T1.Atom_ID";
	T1_RES_SEQ_CODE = "_T1.Comp_index_ID";
        T1_VALUE = "_T1.Val";
        T1_VALUE_ERR = "_T1.Val_err";
	// Note: for some reason the T1 tags got changed between 3.0 and
	// 3.1 but the some of the T2 tags didn't.  wenger 2007-09-26.
	T2_ATOM_NAME = "_T2.Atom_ID";
	T2_RES_SEQ_CODE = "_T2.Comp_index_ID";
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

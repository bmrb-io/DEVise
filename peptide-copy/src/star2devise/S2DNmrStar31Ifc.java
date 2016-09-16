// ========================================================================
// DEVise Data Visualization Software
// (c) Copyright 2007-2010
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
// Revision 1.11.22.1  2013/10/22 19:45:09  wenger
// Merged peak_lists_br_0 thru peak_lists_br_2 to peak_lists2_br.
//
// Revision 1.11.20.4  2013/07/25 21:50:23  wenger
// Various peak list cleanups; added test_peak9 to test conversion on
// a format we don't recognize.
//
// Revision 1.11.20.3  2013/07/16 22:54:56  wenger
// Fixed up the frame details for peak lists.
//
// Revision 1.11.20.2  2013/05/07 15:33:48  wenger
// We now print out the peak text value for testing; added more peak tests.
//
// Revision 1.11.20.1  2013/04/19 21:51:26  wenger
// Started on peak list coding (on branch):  we find the peak list
// frames (for both 2.1 and 3.1 files).
//
// Revision 1.11  2010/12/07 17:41:15  wenger
// Did another version history purge.
//
// Revision 1.10  2010/11/01 00:51:12  wenger
// Merged sample_cond2_br_0 thru sample_cond2_br_1 to trunk.
//
// ...

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
    /**
     * Get the NMR-STAR file version corresponding to this object.
     * @return The NMR-STAR file version corresponding to this object.
     */
    public String version()
    {
	String result = "NMR-STAR 3.1";

	if (_versionStr != null) {
	    result += " (" + _versionStr + ")";
	}

    	return result;
    }

    //-------------------------------------------------------------------
    // Determine whether the STAR file represented by starTree is an
    // NMR-STAR 3.1 file.
    public boolean isNmrStar31()
    {
        boolean result = false;

	try {
	    SaveFrameNode frame = getOneDataFrameByCat(ENTRY_SF_CAT,
	      ENTRY_INFO);

	    _versionStr = getTagValue(frame, NMR_STAR_VERSION);

            if (_versionStr.startsWith("3.1") ||
	      // Note trailing dot below...
	      // This test probably doesn't work for all possible NMR-STAR
	      // 3.x versions, but everything except some of my existing
	      // tests should actually be 3.1 as opposed to 3.0.  wenger
	      // 2007-01-08.
	      _versionStr.startsWith("3.0.") ||
	      _versionStr.startsWith("production.3.0.")) {
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
    // Constructor.  Constructs an S2DNmrStar31Ifc object corresponding to
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
	ORDER_ENTITY_ASSEMBLY_ID = "_Order_param.Entity_assembly_ID";
	ORDER_RES_LABEL = "_Order_param.Comp_ID";
	ORDER_RES_SEQ_CODE = "_Order_param.Seq_ID";
	ORDER_SF_CAT = "_Order_parameter_list.Sf_category";
	ORDER_VALUE = "_Order_param.Order_param_val";
	ORDER_VALUE_ERR = "_Order_param.Order_param_val_fit_err";

	PEAK_LIST = "spectral_peak_list";
	PEAK_LIST_DETAILS = "_Spectral_peak_list.Details";
	PEAK_LIST_ID = "_Peak_general_char.Peak_ID";
	PEAK_LIST_SF_CAT = "_Spectral_peak_list.Sf_category";
	PEAK_LIST_TEXT = "_Spectral_peak_list.Text_data";

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

// ========================================================================
// DEVise Data Visualization Software
// (c) Copyright 2007
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

        if (DEBUG >= 1) {
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

        if (DEBUG >= 1) {
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
	CHEM_SHIFT_AMBIG_CODE = "_Atom_chem_shift.Ambiguity_code";

	CHEM_SHIFT_VALUE = "_Atom_chem_shift.Val";

	ENTITY_DB_ACC_CODE = "_Assembly_db_link.Accession_code";
	ENTITY_DB_NAME = "_Assembly_db_link.Database_code";

	FIGURE_OF_MERIT = "_Atom_chem_shift.Assign_fig_of_merit";

	HETERONUCLEAR_NOE = "heteronucl_NOEs";
	HET_NOE_VALUE = "_Heteronucl_NOE.Val";
	HET_NOE_VALUE_ERR = "_Heteronucl_NOE.Val_err";

	T1_ATOM_NAME = "_T1.Atom_ID";
        T1_VALUE = "_T1.Val";
        T1_VALUE_ERR = "_T1.Val_err";

	T2_ATOM_NAME = "_T2.Atom_ID";
    }
}

// ========================================================================

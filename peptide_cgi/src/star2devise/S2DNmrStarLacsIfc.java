// ========================================================================
// DEVise Data Visualization Software
// (c) Copyright 2005-2007
// By the DEVise Development Group
// Madison, Wisconsin
// All Rights Reserved.
// ========================================================================

// Under no circumstances is this software to be copied, distributed,
// or altered in any way without prior permission from the DEVise
// Development Group.

// ------------------------------------------------------------------------

// This class contains NMR-Star access methods, etc., specific to NMR-STAR/
// LACS files.

// ------------------------------------------------------------------------

// $Id$

// $Log$
// Revision 1.3.4.1  2008/11/05 18:15:38  wenger
// Make S2DStarIfc and S2DNmrStarIfc abstract, and got rid of a bunch
// of things in S2DStarIfc that should only be in S2DNmrStarIfc.
// (Note that I could move a bunch more methods from S2DStarIfc
// to S2DNmrStarIfc, but I'm going to wait until after I merge the
// current branch.)
//
// Revision 1.3  2007/08/20 20:26:08  wenger
// Added -verb command-line flag and property so we can turn on debug
// output without recompiling; added debug_level property corresponding
// to the existing -debug command-line flag.
//
// Revision 1.2  2006/02/01 21:34:32  wenger
// Merged peptide_cgi_10_8_0_br_0 thru peptide_cgi_10_8_0_br_2
// to the trunk.
//
// Revision 1.1.2.2  2005/11/02 23:21:31  wenger
// Changed LACS-related STAR file tags to be properly defined;
// horizontal line in LACS visualizations is at *minus* y offset.
//
// Revision 1.1.2.1  2005/10/14 21:19:30  wenger
// Most LACS processing now in place -- still needs lots of cleanup,
// though.
//

// ========================================================================

package star2devise;

import EDU.bmrb.starlibj.*;
import java.io.*;
import java.util.*;
import java.net.*;

public class S2DNmrStarLacsIfc extends S2DNmrStarIfc {
    //===================================================================
    // VARIABLES

    private static final int DEBUG = 0;

    //===================================================================
    // PUBLIC METHODS

    //===================================================================
    // PROTECTED METHODS
    //-------------------------------------------------------------------
    /**
     * Get the NMR-STAR file version corresponding to this object.
     * @return The NMR-STAR file version corresponding to this object.
     */
    public String version()
    {
        return "LACS";
    }

    //-------------------------------------------------------------------
    // Constructor.  Constructs an S2DNmrStarLacsIfc object corresponding to
    // the STAR file represented by starTree.

    protected S2DNmrStarLacsIfc(StarNode starTree) throws S2DException
    {
        super(starTree);

        if (doDebugOutput(11)) {
            System.out.println("S2DNmrStarLacsIfc.S2DNmrStarLacsIfc()");
        }

        setStarNames();
    }

    //===================================================================
    // PRIVATE METHODS

    // ----------------------------------------------------------------------
    // Set the tag names and values to work for NMR-Star files.
    private void setStarNames()
    {
        LACS_DESIGNATOR = "_LACS.Designator";
        LACS_LINE1_X1 = "_LACS_plot.Line_1_terminator_val_x_1";
        LACS_LINE1_X2 = "_LACS_plot.Line_1_terminator_val_x_2";
        LACS_LINE1_Y1 = "_LACS_plot.Line_1_terminator_val_y_1";
        LACS_LINE1_Y2 = "_LACS_plot.Line_1_terminator_val_y_2";
        LACS_LINE2_X1 = "_LACS_plot.Line_2_terminator_val_x_1";
        LACS_LINE2_X2 = "_LACS_plot.Line_2_terminator_val_x_2";
        LACS_LINE2_Y1 = "_LACS_plot.Line_2_terminator_val_y_1";
        LACS_LINE2_Y2 = "_LACS_plot.Line_2_terminator_val_y_2";
        LACS_OUTPUT = "LACS_output";
        LACS_PLOT_SF_CAT = "_LACS_plot.Sf_category";
        LACS_RES_LABEL = "_LACS.Comp_ID";
        LACS_RES_NUM = "_LACS.Comp_index_ID";
        LACS_X_NAME = "_LACS_plot.X_coord_name";
        LACS_X_VALUE = "_LACS.X_coord_val";
        LACS_Y_NAME = "_LACS_plot.Y_coord_name";
        LACS_Y_VALUE = "_LACS.Y_coord_val";
        LACS_Y_OFFSET = "_LACS_plot.Y_axis_chem_shift_offset";
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

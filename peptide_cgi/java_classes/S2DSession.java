// ========================================================================
// DEVise Data Visualization Software
// (c) Copyright 2001
// By the DEVise Development Group
// Madison, Wisconsin
// All Rights Reserved.
// ========================================================================

// Under no circumstances is this software to be copied, distributed,
// or altered in any way without prior permission from the DEVise
// Development Group.

// ------------------------------------------------------------------------

// This class is used to convert a "base" session file to a session file
// specific to a particular data set.

// ------------------------------------------------------------------------

// $Id$

// $Log$
// Revision 1.5  2001/04/17 17:09:10  wenger
// Added display of H vs. N chem shifts.
//
// Revision 1.4  2001/04/16 19:49:11  wenger
// Added display of all chem shifts by amino acid.
//
// Revision 1.3  2001/03/08 20:33:24  wenger
// Merged changes from no_collab_br_0 thru no_collab_br_2 from the branch
// to the trunk.
//
// Revision 1.2.2.1  2001/02/09 16:57:13  wenger
// Added heteronuclear NOE; made T1 and T2 relaxation errors optional
// (conversion doesn't fail if they are not found); added an X margin of
// 0.5 in all sessions; updated star file list; misc. minor cleanups.
//
// Revision 1.2  2001/01/19 15:39:06  wenger
// Added T1 and T2 relaxation; removed some unnecessary variables from
// coupling constants; added schema files to installation, unified T1
// and T2 relaxation schema.
//
// Revision 1.1  2001/01/17 20:00:07  wenger
// Restructured the peptide-cgi code to make it much more maintainable.
//

// ========================================================================

import java.io.*;

public class S2DSession {
    //===================================================================
    // VARIABLES

    private static final int DEBUG = 0;

    //===================================================================
    // PUBLIC METHODS

    //-------------------------------------------------------------------
    static void write(String sessionDir, int dataType, int accessionNum,
      int frameIndex) throws S2DException
    {
        write(sessionDir, dataType, accessionNum, frameIndex, null);
    }

    //-------------------------------------------------------------------
    static void write(String sessionDir, int dataType, int accessionNum,
      int frameIndex, String title) throws S2DException
    {
        if (DEBUG >= 1) {
	    System.out.println("S2DSession.write(" + sessionDir + ", " +
	      dataType + ", " + accessionNum + ", " + frameIndex + ")");
	}

	String baseName;
	String dataSuffix;

	// The "main" data source in the base file.
	String searchString1 = null;

	// The residue count data source in the base file.
	String searchString2 = null;

	switch (dataType) {
	case S2DUtils.TYPE_DELTASHIFT:
	    baseName = "deltashift.base";
	    dataSuffix = S2DNames.DELTASHIFT_SUFFIX;
	    searchString1 = "4081d1";
	    break;

	case S2DUtils.TYPE_CSI:
	    baseName = "csi.base";
	    dataSuffix = S2DNames.CSI_SUFFIX;
	    searchString1 = "4081c1";
	    break;

	case S2DUtils.TYPE_PCT_ASSIGN:
	    baseName = "percent.base";
	    dataSuffix = S2DNames.PERCENT_ASSIGN_SUFFIX;
	    searchString1 = "4081p1";
	    break;

	case S2DUtils.TYPE_ALL_CHEM_SHIFTS:
	    baseName = "allshift.base";
	    dataSuffix = S2DNames.ALL_CHEM_SHIFT_SUFFIX;
	    searchString1 = "4267as1";
	    searchString2 = "4267rc1";
	    break;

	case S2DUtils.TYPE_HVSN_CHEM_SHIFTS:
	    baseName = "h_vs_n.base";
	    dataSuffix = S2DNames.HVSN_CHEM_SHIFT_SUFFIX;
	    searchString1 = "4267hn1";
	    searchString2 = "4267rc1";
	    break;

        case S2DUtils.TYPE_COUPLING:
	    baseName = "coupling.base";
	    dataSuffix = S2DNames.COUPLING_SUFFIX;
	    searchString1 = "4096g1";
	    break;

/*TEMP
        case S2DUtils.TYPE_HXRATES:
	    baseName = "hxrates.base";
	    dataSuffix = S2DNames.HX_RATE_SUFFIX;
	    searchString1 = "4096r";
	    break;

        case S2DUtils.TYPE_ORDER:
	    baseName = "order.base";
	    dataSuffix = S2DNames.
	    searchString1 = "";
	    break;
TEMP*/

        case S2DUtils.TYPE_T1_RELAX:
	    baseName = "relax.base";
	    dataSuffix = S2DNames.T1_SUFFIX;
	    searchString1 = "4267t11";
	    break;

        case S2DUtils.TYPE_T2_RELAX:
	    baseName = "relax.base";
	    dataSuffix = S2DNames.T2_SUFFIX;
	    searchString1 = "4267t11";
	    break;

        case S2DUtils.TYPE_HETNOE:
	    baseName = "het_noe.base";
	    dataSuffix = S2DNames.HETERONUCLEAR_NOE_SUFFIX;
	    searchString1 = "4267n1";
	    break;

	default:
	    throw new S2DError("Illegal data type: " + dataType);
	}

        String outFileName = sessionDir + "/" + accessionNum + dataSuffix +
	  frameIndex + ".ds";

	try {
            FileWriter writer = new FileWriter(outFileName);
	    BufferedReader reader = new BufferedReader(new FileReader(
	      sessionDir + "/" + baseName));

	    writer.write("# DEVise session file generated by S2DSession\n\n");

	    // The "main" data source for the file we're writing.
            String replaceString1 = accessionNum + dataSuffix + frameIndex;

	    // The residue count data source for the file we're writing.
            String replaceString2 = null;
	    if (searchString2 != null) {
	        replaceString2 = accessionNum + S2DNames.RES_COUNT_SUFFIX +
		  frameIndex;
	    }

            String line;
	    while ((line = reader.readLine()) != null) {
	        String line2 = S2DUtils.replace(line, searchString1,
		  replaceString1);

                if (searchString2 != null) {
	            line2 = S2DUtils.replace(line2, searchString2,
		      replaceString2);
		}

		if (title != null) {
		    line2 = S2DUtils.replace(line2, "dummy title", title);
		}

	        writer.write(line2 + "\n");
	    }

	    reader.close();
	    writer.close();
        } catch(IOException ex) {
	    System.err.println("IOException writing session file: " +
	      ex.getMessage());
	    throw new S2DError("Can't write session file " + outFileName);
	}
    }
}

// ========================================================================

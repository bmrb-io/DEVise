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

// This class is used to convert a "base" specific html files to an html file
// specific to a particular data set.

// ------------------------------------------------------------------------

// $Id$

// $Log$
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

public class S2DSpecificHtml {
    //===================================================================
    // VARIABLES

    private static final int DEBUG = 0;

    //===================================================================
    // PUBLIC METHODS
    static void write(String dataDir, int dataType, int accessionNum,
      int frameIndex) throws S2DException
    {
        if (DEBUG >= 1) {
	    System.out.println("S2DSpecificHtml.write(" + dataDir + ", " +
	      dataType + ", " + accessionNum + ", " + frameIndex + ")");
	}

	String baseName = "specific_html.base";
	String dataSuffix;
	String searchString1 = "4264d1";
	String searchString2 = "4264";
	String searchString3 = "help_d.html";

	//TEMP -- should this be made into a method in S2DUtils?
	switch (dataType) {
	case S2DUtils.TYPE_DELTASHIFT:
	    dataSuffix = S2DNames.DELTASHIFT_SUFFIX;
	    break;

	case S2DUtils.TYPE_CSI:
	    dataSuffix = S2DNames.CSI_SUFFIX;
	    break;

	case S2DUtils.TYPE_PCT_ASSIGN:
	    dataSuffix = S2DNames.PERCENT_ASSIGN_SUFFIX;
	    break;

	case S2DUtils.TYPE_ALL_CHEM_SHIFTS:
	    dataSuffix = S2DNames.ALL_CHEM_SHIFT_SUFFIX;
	    break;

	case S2DUtils.TYPE_HVSN_CHEM_SHIFTS:
	    dataSuffix = S2DNames.HVSN_CHEM_SHIFT_SUFFIX;
	    break;

        case S2DUtils.TYPE_COUPLING:
	    dataSuffix = S2DNames.COUPLING_SUFFIX;
	    break;

/*TEMP
        case S2DUtils.TYPE_HXRATES:
	    dataSuffix = S2DNames.HX_RATE_SUFFIX;
	    break;

        case S2DUtils.TYPE_ORDER:
	    dataSuffix = S2DNames.
	    break;
TEMP*/

        case S2DUtils.TYPE_T1_RELAX:
	    dataSuffix = S2DNames.T1_SUFFIX;
	    break;

        case S2DUtils.TYPE_T2_RELAX:
	    dataSuffix = S2DNames.T2_SUFFIX;
	    break;

        case S2DUtils.TYPE_HETNOE:
	    dataSuffix = S2DNames.HETERONUCLEAR_NOE_SUFFIX;
	    break;

	default:
	    throw new S2DError("Illegal data type: " + dataType);
	}

        String outFileName = dataDir + "/" + accessionNum + dataSuffix +
	  frameIndex + ".html";

	try {
            FileWriter writer = new FileWriter(outFileName);
	    BufferedReader reader = new BufferedReader(
	      new FileReader(dataDir + "/" + baseName));

            String replaceString1 = accessionNum + dataSuffix + frameIndex;
            String replaceString2 = Integer.toString(accessionNum);
            String replaceString3 = "help_" + dataSuffix + ".html";

            String line0;
	    while ((line0 = reader.readLine()) != null) {
		String line2 = S2DUtils.replace(line0, searchString1,
		  replaceString1);
		String line3 = S2DUtils.replace(line2, searchString2,
		  replaceString2);
		String line4 = S2DUtils.replace(line3, searchString3,
		  replaceString3);
	        writer.write(line4 + "\n");
	    }

	    reader.close();
	    writer.close();
        } catch(IOException ex) {
	    System.err.println("IOException writing specific html file: " +
	      ex.getMessage());
	    throw new S2DError("Can't write specific html file" +
	      outFileName);
	}
    }
}

// ========================================================================

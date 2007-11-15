// ========================================================================
// DEVise Data Visualization Software
// (c) Copyright 2001-2007
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
// Revision 1.8  2007/08/20 20:26:09  wenger
// Added -verb command-line flag and property so we can turn on debug
// output without recompiling; added debug_level property corresponding
// to the existing -debug command-line flag.
//
// Revision 1.7  2007/03/07 16:37:58  wenger
// Phase 2 of "upload and visualize data" -- mostly working, I think,
// but still needs chem shift reference capability and hasn't been
// hooked up to Dimitri's upload scripts yet.
//
// Revision 1.6  2007/03/05 18:14:27  wenger
// First phase of support for "upload and visualize data" setup -- still
// needs quite a few changes.
//
// Revision 1.5  2006/05/26 16:50:55  wenger
// Merged peptide_cgi_jmol_proto_br_0 thru peptide_cgi_jmol_proto_br_1
// to the trunk.
//
// ...

// ========================================================================

package star2devise;

import java.io.*;

public class S2DSpecificHtml {
    //===================================================================
    // VARIABLES

    private static final int DEBUG = 0;

    // Whether we're processing for "upload and visualize data".
    private static boolean _isUvd = false;

    //===================================================================
    // PUBLIC METHODS

    //-------------------------------------------------------------------
    // Set whether this is UVD processing.
    static void setIsUvd(boolean isUvd)
    {
        _isUvd = isUvd;
    }

    //-------------------------------------------------------------------
    static void write(String htmlDir, int dataType, String name,
      int frameIndex, String title) throws S2DException
    {
        if (doDebugOutput(11)) {
	    System.out.println("S2DSpecificHtml.write(" + htmlDir + ", " +
	      dataType + ", " + name + ", " + frameIndex + ")");
	}

	// Write the "normal size" file.
	String templateFile = "html_templates" + File.separator;
	if (dataType == S2DUtils.TYPE_ATOMIC_COORDS) {
	    templateFile += "specific_html_jmol.base";
	} else {
	    templateFile += "specific_html.base";
	}
        writeOne(templateFile, htmlDir, "", dataType, name, frameIndex,
	  title);

	// Write the "large size" file.
	templateFile = "html_templates" + File.separator;
	if (dataType == S2DUtils.TYPE_ATOMIC_COORDS) {
	    templateFile += "specific_html_large_jmol.base";
	} else {
	    templateFile += "specific_html_large.base";
	}
        writeOne(templateFile, htmlDir, "_large", dataType, name,
	  frameIndex, title);
    }

    //===================================================================
    // PRIVATE METHODS

    //-------------------------------------------------------------------
    private static void writeOne(String templateFile, String htmlDir,
      String sizeSuffix, int dataType, String name, int frameIndex,
      String title) throws S2DException
    {
	String dataSuffix;
	String searchString1 = "4264d1";
	String searchString2 = "bmr4264.str";
	String searchString3 = "help_d.html";
	String searchString4 = "Dummy title";

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

	case S2DUtils.TYPE_ATOMIC_COORDS:
	    dataSuffix = S2DNames.ATOMIC_COORD_SUFFIX;
	    break;

	case S2DUtils.TYPE_CHEM_SHIFT_REF1:
	    dataSuffix = S2DNames.CSR1_SUFFIX;
	    break;

	case S2DUtils.TYPE_CHEM_SHIFT_REF2:
	    dataSuffix = S2DNames.CSR2_SUFFIX;
	    break;

	case S2DUtils.TYPE_CHEM_SHIFT_REF3:
	    dataSuffix = S2DNames.CSR3_SUFFIX;
	    break;

	case S2DUtils.TYPE_PISTACHIO:
	    dataSuffix = S2DNames.PISTACHIO_SUFFIX;
	    break;

	case S2DUtils.TYPE_AMBIGUITY:
	    dataSuffix = S2DNames.AMBIGUITY_SUFFIX;
	    break;

	case S2DUtils.TYPE_LACS:
	    dataSuffix = S2DNames.LACS_SUFFIX;
	    break;

	default:
	    throw new S2DError("Illegal data type: " + dataType);
	}

        String outFileName = htmlDir + File.separator + name + dataSuffix +
	  frameIndex + sizeSuffix + ".html";

	try {
            FileWriter writer = S2DFileWriter.create(outFileName);
	    BufferedReader reader = new BufferedReader(
	      new FileReader(templateFile));

            String replaceString1 = name + dataSuffix + frameIndex;
            String replaceString2 = name;
            String replaceString3 = "help_" + dataSuffix + ".html";
            String replaceString4 = title;

            String line0;
	    while ((line0 = reader.readLine()) != null) {
		String line2 = S2DUtils.replace(line0, searchString1,
		  replaceString1);
		String line3 = S2DUtils.replace(line2, searchString2,
		  replaceString2);
		String line4 = S2DUtils.replace(line3, searchString3,
		  replaceString3);
		line4 = S2DUtils.replace(line4, searchString4,
		  replaceString4);
		if (dataType == S2DUtils.TYPE_ATOMIC_COORDS ||
		  dataType == S2DUtils.TYPE_PISTACHIO ||
		  dataType == S2DUtils.TYPE_AMBIGUITY) {
		    // Change JS background color so gaps between views
		    // show up.
		    line4 = S2DUtils.replace(line4, "0+0+0", "64+96+0");
		}
		line4 = S2DUtils.replace(line4, "COMMENT_EMAIL",
		  S2DNames.COMMENT_EMAIL);
		if (_isUvd) {
		    line4 = S2DUtils.replace(line4, "../..", "../../..");
		    line4 = S2DUtils.replace(line4, "dynamic_sessions",
		      "dynamic_sessions/.uvd");
		}
	        writer.write(line4 + "\n");
	    }

	    reader.close();
	    writer.close();
        } catch(IOException ex) {
	    System.err.println("IOException: " +
	      ex.toString() + " writing specific html file");
	    throw new S2DError("Can't write specific html file: " +
	      outFileName);
	}
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

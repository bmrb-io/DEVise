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

        case S2DUtils.TYPE_RELAX:
	    dataSuffix = S2DNames.
	    break;
TEMP*/

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

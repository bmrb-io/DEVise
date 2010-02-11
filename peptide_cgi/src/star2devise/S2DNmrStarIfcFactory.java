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

// This class is an abstract superclass of a set of factory classes used to
// create various types of S2DNmrStar*Ifc objects.

// ------------------------------------------------------------------------

// $Id$

// $Log$
// Revision 1.2.4.2  2010/02/10 16:23:40  wenger
// Removed a bunch of temporary stuff.
//
// Revision 1.2.4.1  2010/02/03 20:57:00  wenger
// Blocked in code for getting remediated restraints from a specified file.
//
// Revision 1.2  2010/01/06 23:03:40  wenger
// Merged s2d_dist_rest_0912_br_0 thru s2d_dist_rest_0912_br_1 to trunk.
//
// Revision 1.1.2.4  2010/01/06 22:21:56  wenger
// Did a bunch of cleanups on the distance restraint code, in preparation
// for merging it to the trunk, so I can suspend work on it and move to
// working on the "PDB-ID-only" torsion angle restraint visualizations.
//
// Revision 1.1.2.3  2010/01/04 23:59:18  wenger
// Finished moving creation of all S2DNmrStar*Ifc objects into the new
// factory classes.
//
// Revision 1.1.2.2  2010/01/04 19:31:06  wenger
// Made most S2DNmrStarIfcFactory methods non-static in preparation for
// splitting it up into various subclasses as appropriate.
//
// Revision 1.1.2.1  2010/01/04 18:57:00  wenger
// Added new S2DNmrStarIfcFactory class as part 1 of cleaning up the
// creation of various S2D*Ifc objects.
//

// ========================================================================

package star2devise;

import EDU.bmrb.starlibj.*;
import java.io.*;
import java.util.*;
import java.net.*;

public abstract class S2DNmrStarIfcFactory {
    //===================================================================
    // VARIABLES

    private static final int DEBUG = 0;

    //===================================================================
    // PUBLIC METHODS

    //-------------------------------------------------------------------
    public abstract String getFileName(String bmrbId);

    //-------------------------------------------------------------------
    public abstract String getURLName(String fileName);

    //-------------------------------------------------------------------
    // Get the modification date/time of the appropriate NMR-Star file.
    public Date getModDate(String bmrbId)
    {
        if (doDebugOutput(12)) {
	    System.out.println("S2DNmrStarIfcFactory.getModDate(" +
	      bmrbId + ")");
	}

	Date date = null;
	try {
	    URL starfile = new URL(getURLName(getFileName(bmrbId)));
	    URLConnection connection = starfile.openConnection();

	    long timestamp = connection.getLastModified();
	    date = new Date(timestamp);
        } catch (MalformedURLException ex) {
	    System.err.println("MalformedURLException: " + ex.toString());
        } catch (IOException ex) {
	    System.err.println("IOException: " + ex.toString());
	}

        if (doDebugOutput(12)) {
	    System.out.println("  Mod date is : " + date);
	}

	return date;
    }

    //-------------------------------------------------------------------
    // Get the modification date/time of the appropriate file.
    public static Date getModDateFile(String filename)
    {
        if (doDebugOutput(12)) {
	    System.out.println("S2DNmrStarIfcFactory.getModDateFile(" +
	      filename + ")");
	}

	Date date = null;
	File tmpFile = new File(filename);
	long timestamp = tmpFile.lastModified();
	date = new Date(timestamp);

        if (doDebugOutput(12)) {
	    System.out.println("  Mod date is : " + date);
	}

	return date;
    }

    //-------------------------------------------------------------------
    // Factory method to create an S2DNmrStarIfc object based on accession
    // number.
    public S2DNmrStarIfc createFromId(String accessionNum,
      boolean useLocalFile) throws S2DException
    {
        if (doDebugOutput(11)) {
	    System.out.println("S2DNmrStarIfcFactory.createFromId(" +
	      accessionNum + ", " + useLocalFile + ")");
	}

	String fileName = getFileName(accessionNum);

	S2DNmrStarIfc ifc;

        try {
	    InputStream is = null;
	    if (useLocalFile) {
		System.out.println("Note: using local copy of star file");
	        is = new FileInputStream(fileName);
	    } else {
                URL starfile = new URL(getURLName(fileName));
	        is = starfile.openStream();
	    }

	    ifc = create(is);
	    is.close();

	    ifc._description = "BMRB " + accessionNum;
	    if (useLocalFile) ifc._description += " (local)";

	    ifc._fileName = fileName;
	    ifc._useLocalFile = useLocalFile;

        } catch(java.io.IOException ex) {
            System.err.println("Unable to open or read " + ex.toString());
            if (doDebugOutput(11)) ex.printStackTrace();
            throw new S2DError("Unable to get data in star file " +
              fileName);
	} catch(Exception ex) {
	    System.err.println("Exception (" + ex.toString() +
	      ") parsing NMR-STAR file");
	    String errMsg = "Unable to get data in star file " + fileName;
	    System.err.println(errMsg);
            throw new S2DError(errMsg);
	}

	return ifc;
    }

    //-------------------------------------------------------------------
    // Factory method to create an S2DNmrStarIfc object based on file name.
    public S2DNmrStarIfc createFromFile(String fileName)
      throws S2DException
    {
        if (doDebugOutput(11)) {
	    System.out.println("S2DNmrStarIfcFactory.createFromFile(" +
	      fileName + ")");
	}

	S2DNmrStarIfc ifc;

        try {
	    InputStream is = new FileInputStream(fileName);

	    ifc = create(is);
	    is.close();

	    ifc._description = fileName;

	    ifc._fileName = fileName;

        } catch(java.io.IOException ex) {
            System.err.println("Unable to open or read " + ex.toString());
            if (doDebugOutput(11)) ex.printStackTrace();
            throw new S2DError("Unable to get data in star file " +
              fileName);
	} catch(Exception ex) {
	    System.err.println("Exception (" + ex.toString() +
	      ") parsing NMR-STAR file");
	    String errMsg = "Unable to get data in star file " + fileName;
	    System.err.println(errMsg);
            throw new S2DError(errMsg);
	}

	return ifc;
    }

    //===================================================================
    // PROTECTED METHODS
    //-------------------------------------------------------------------
    // Create an S2DNmrStarIfc object from the given InputStream.
    // This method decides what version of NMR-STAR file the InputStream
    // corresponds to, and constructs an object appropriately.
    protected S2DNmrStarIfc create(InputStream is) throws S2DException
    {
        if (doDebugOutput(11)) {
	    System.out.println("S2DNmrStarIfcFactory.create()");
	}

	StarNode starTree = S2DStarIfc.parseStar(is);

	S2DNmrStarIfc ifc = null;

	S2DNmrStar21Ifc ifc21 = new S2DNmrStar21Ifc(starTree);
	S2DNmrStar30Ifc ifc30 = new S2DNmrStar30Ifc(starTree);
	S2DNmrStar31Ifc ifc31 = new S2DNmrStar31Ifc(starTree);

	if (ifc21.isNmrStar21()) {
            if (doDebugOutput(1)) {
	        System.out.println("File is NMR-STAR 2.1");
	    }
	    ifc = ifc21;

	} else if (ifc31.isNmrStar31()) {
            if (doDebugOutput(1)) {
	        System.out.println("File is NMR-STAR 3.1");
	    }
	    ifc31.checkForProteins();
	    ifc = ifc31;

	} else if (ifc30.isNmrStar30()) {
            if (doDebugOutput(1)) {
	        System.out.println("File is NMR-STAR 3.0");
	    }
	    ifc30.checkForProteins();
	    ifc = ifc30;

	} else {
	    System.err.println("Warning: possibly unknown or unsupported " +
	      "NMR-STAR version; trying NMR-STAR 2.1");
	    ifc = ifc21;
	}

	return ifc;
    }

    //===================================================================
    // PRIVATE METHODS

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

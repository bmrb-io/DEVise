//TEMP -- maybe combine this with S2DNmrStarRRIfcFactory?
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

// This class defines objects that are used as factories for creating
// S2DNMRStarDistRIfc objects.  (The methods to create the
// S2DNMRStarDistRIfc objects are non-static so that they can inherit from
// related factory classes.)

// ------------------------------------------------------------------------

// $Id$

// $Log$
// Revision 1.2.4.2  2010/02/11 20:28:09  wenger
// Added some tests of the make_view and make_uvd scripts, fixed a bug in
// make_view that this found...
//
// Revision 1.2.4.1  2010/02/05 22:51:37  wenger
// Implemented to-do item 115: PDB IDs differing only by case are now
// considered the same (implemented by internally changing all PDB IDs
// to upper case).
//
// Revision 1.2  2010/01/06 23:03:39  wenger
// Merged s2d_dist_rest_0912_br_0 thru s2d_dist_rest_0912_br_1 to trunk.
//
// Revision 1.1.2.4  2010/01/06 22:21:56  wenger
// Did a bunch of cleanups on the distance restraint code, in preparation
// for merging it to the trunk, so I can suspend work on it and move to
// working on the "PDB-ID-only" torsion angle restraint visualizations.
//
// Revision 1.1.2.3  2010/01/05 16:09:46  wenger
// Got parsing of distance restraint files to work for URL and PDB ID,
// added appropriate tests.
//
// Revision 1.1.2.2  2010/01/05 15:33:52  wenger
// Cleaned up the pdbIdToUrl() methods for various restraint-related
// classes.
//
// Revision 1.1.2.1  2010/01/04 23:59:18  wenger
// Finished moving creation of all S2DNmrStar*Ifc objects into the new
// factory classes.
//

// ========================================================================

package star2devise;

import EDU.bmrb.starlibj.*;
import java.io.*;
import java.util.*;
import java.net.*;

public class S2DNmrStarDistRIfcFactory extends S2DNmrStarIfcFactory {
    //===================================================================
    // VARIABLES

    private static final int DEBUG = 0;

    //===================================================================
    // PUBLIC METHODS

    //-------------------------------------------------------------------
    public String getFileName(String pdbId)
    {
	//TEMP -- is this what we want here??
	return pdbId;
    }

    //-------------------------------------------------------------------
    public String getURLName(String fileName)
    {
	String url = null;

	try {
	    url = pdbIdToUrl(fileName);
        } catch (S2DException ex) {
	    System.err.println("S2DException: " + ex.toString());
	}

	return url;
    }

    //-------------------------------------------------------------------
    /**
     * Get the URL name for the document containing the distance
     * restraint data for the given PDB ID.
     * @param The PDB ID we need data for.
     * @return The URL name for the document containing the actual
     *   distance restraint data.
     */
    public static String pdbIdToUrl(String pdbId) throws S2DException
    {
	pdbId = pdbId.toLowerCase();
    	String url = "http://www.bmrb.wisc.edu/ftp/pub/bmrb/" +
	  "nmr_pdb_integrated_data/coordinates_restraints_chemshifts/" +
	  "all/nmr-star/" + pdbId + "/" + pdbId + "_linked.str";//TEMP!!!

        return url;
    }

    //-------------------------------------------------------------------
    // Factory method to create an S2DNmrStarDistRIfc object based on
    // PDB ID.
    public S2DNmrStarDistRIfc createFromId(String pdbId)
      throws S2DException
    {
        String urlName = getURLName(getFileName(pdbId));

        S2DNmrStarDistRIfc ifc = createFromUrl(urlName, pdbId);

	return ifc;
    }

    //TEMP -- move this to S2DNmrStarIfcFactory?
    //-------------------------------------------------------------------
    // Factory method to create an S2DNmrStarDistRIfc object based on
    // URL.
    public S2DNmrStarDistRIfc createFromUrl(String urlName,
      String pdbId) throws S2DException
    {
        if (doDebugOutput(11)) {
	    System.out.println("S2DNmrStarDistRIfcFactory.createFromUrl(" +
	      urlName + ")");
	}

	S2DNmrStarDistRIfc ifc;

        try {
	    URL starfile = new URL(urlName);
	    InputStream is = starfile.openStream();

	    ifc = (S2DNmrStarDistRIfc)create(is);
	    is.close();

	    ifc._description = "PDB " + pdbId;

	    ifc._fileName = urlName;
	    ifc._useLocalFile = false;

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
    // Create an S2DNmrStarIfc object from the given InputStream.
    // This method decides what version of NMR-STAR file the InputStream
    // corresponds to, and constructs an object appropriately.
    protected S2DNmrStarIfc create(InputStream is) throws S2DException
    {
        if (doDebugOutput(11)) {
	    System.out.println("S2DNmrStarDistRIfcFactory.create()");
	}

	StarNode starTree = S2DStarIfc.parseStar(is);

	S2DNmrStarIfc ifc = new S2DNmrStarDistRIfc(starTree);

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

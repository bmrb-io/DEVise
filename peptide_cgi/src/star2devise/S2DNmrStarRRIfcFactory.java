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
// S2DNMRStarRRIfc objects (these objects are the interface to remediated
// restraint files).  (The methods to create the S2DNMRStarRRIfc
// objects are non-static so that they can inherit from related factory
// classes.)

// ------------------------------------------------------------------------

// $Id$

// $Log$
// Revision 1.1.2.3  2010/02/05 20:46:13  wenger
// Partially implemented getting remediated restraints file template from
// properties.
//
// Revision 1.1.2.2  2010/02/03 21:16:09  wenger
// Blocked in code for getting remediated restraints from a specified
// PDB ID.
//
// Revision 1.1.2.1  2010/02/03 20:57:00  wenger
// Blocked in code for getting remediated restraints from a specified file.
//

// ========================================================================

package star2devise;

import EDU.bmrb.starlibj.*;
import java.io.*;
import java.util.*;
import java.net.*;

public class S2DNmrStarRRIfcFactory extends S2DNmrStarIfcFactory {
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
     * Get the URL name for the document containing the remediated
     * restraints data.
     * @param The PDB ID we need data for.
     * @return The URL name for the document containing the actual
     *   remediated restraints data.
     */
    protected static String pdbIdToUrl(String pdbId)
      throws S2DException
    {
        if (doDebugOutput(11)) {
	    System.out.println("S2DNmrStarRRIfcFactory.pdbIdToUrl(" +
	      pdbId + ")");
	}

	pdbId = pdbId.toLowerCase();

        String url = S2DUtils.replace(
	  S2DNames.REMEDIATED_RESTRAINTS_TEMPLATE, "*", pdbId);

	return url;
    }

    //-------------------------------------------------------------------
    // Factory method to create an S2DNmrStarRRIfc object based on
    // PDB ID.
    public S2DNmrStarRRIfc createFromId(String pdbId)
      throws S2DException
    {
        String urlName = getURLName(getFileName(pdbId));

        S2DNmrStarRRIfc ifc = createFromUrl(urlName);

	return ifc;
    }

    //-------------------------------------------------------------------
    // Factory method to create an S2DNmrStarRRIfc object based on
    // URL.
    public S2DNmrStarRRIfc createFromUrl(String urlName) throws S2DException
    {
        if (doDebugOutput(11)) {
	    System.out.println("S2DNmrStarRRIfcFactory.createFromUrl(" +
	      urlName + ")");
	}

	S2DNmrStarRRIfc ifc;

        try {
	    URL url = new URL(urlName);
	    InputStream is = url.openStream();
	    
	    ifc = (S2DNmrStarRRIfc)create(is);

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
	    System.out.println("S2DNmrStarRRIfcFactory.create()");
	}

	StarNode starTree = S2DStarIfc.parseStar(is);

	S2DNmrStarIfc ifc = new S2DNmrStarRRIfc(starTree);

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

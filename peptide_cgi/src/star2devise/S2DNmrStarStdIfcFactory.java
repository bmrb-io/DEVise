// ========================================================================
// DEVise Data Visualization Software
// (c) Copyright 2000-2014
// By the DEVise Development Group
// Madison, Wisconsin
// All Rights Reserved.
// ========================================================================

// Under no circumstances is this software to be copied, distributed,
// or altered in any way without prior permission from the DEVise
// Development Group.

// ------------------------------------------------------------------------

// This class defines objects that are used as factories for creating
// S2DNMRStar21Ifc, S2DNmrStar30Ifc, and S2DNmrStar31Ifc objects.  (The
// methods to create the NMRStarIfc objects are non-static so that they
// can inherit from related factory classes.)

// ------------------------------------------------------------------------

// $Id$

// $Log$
// Revision 1.4  2012/01/20 16:09:21  wenger
// Merged s2d_mailfix_br_0 thru s2d_mailfix_br_1 to trunk.
//
// Revision 1.3.24.1  2012/01/09 22:32:30  wenger
// S2d no longer sends error emails in a number of cases that are
// instances of faulty inputs rather than actual software errors
// (see to-do 139).  (Note:  this still needs some work/more checking.)
//
// Revision 1.3  2010/03/11 20:31:29  wenger
// Implemented to-do 126 (multiple NMR-STAR file paths), except that
// not all config files are updated yet; added checks that the URL
// exists to all methods for getting URLs.  Also changed a few tests
// affected by using the 3.1 files instead of 2.1 files.
//
// Revision 1.2  2010/01/06 23:03:40  wenger
// Merged s2d_dist_rest_0912_br_0 thru s2d_dist_rest_0912_br_1 to trunk.
//
// Revision 1.1.2.2  2010/01/06 22:21:57  wenger
// Did a bunch of cleanups on the distance restraint code, in preparation
// for merging it to the trunk, so I can suspend work on it and move to
// working on the "PDB-ID-only" torsion angle restraint visualizations.
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

public class S2DNmrStarStdIfcFactory extends S2DNmrStarIfcFactory {
    //===================================================================
    // VARIABLES

    private static final int DEBUG = 0;

    private String _urlName = null;

    //===================================================================
    // PUBLIC METHODS

    //-------------------------------------------------------------------
    public String getFileName(String bmrbId)
    {
        String name = S2DUtils.replace(S2DNames.STAR_NAME_TEMPLATE, "*",
                                       bmrbId);

        return name;
    }

    //-------------------------------------------------------------------
    public String getURLName(String fileName) throws S2DException
    {
        if (_urlName == null) {
            StringTokenizer st =
                new StringTokenizer(S2DNames.BMRB_STAR_URL, ";", false);
            while (_urlName == null && st.hasMoreTokens()) {
                String tmpUrl = st.nextToken().trim() + fileName;
                try {
                    S2DUtils.tryUrl(tmpUrl);
                    _urlName = tmpUrl;
                } catch (Exception ex) {
                    // Just continue to try the next one.
                }
            }
        }

        if (_urlName == null || _urlName.equals("")) {
            _urlName = "";
            S2DMain._noMail = true;
            throw new S2DError("Unable to find valid URL for " +
                               fileName);
        }

        return _urlName;
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

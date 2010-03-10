// ========================================================================
// DEVise Data Visualization Software
// (c) Copyright 2009-2010
// By the DEVise Development Group
// Madison, Wisconsin
// All Rights Reserved.
// ========================================================================

// Under no circumstances is this software to be copied, distributed,
// or altered in any way without prior permission from the DEVise
// Development Group.

// ------------------------------------------------------------------------

//TEMPTEMP -- change!
// This class implements the output of torsion angle restraint data.
// For each set of torsion angle data, it creates a data file, a session
// file, an individual html file, and a link in the summary html file.

// Note: the code that deals with the restraint grid web site should
// be cleaned up and use a real html parser...

// ------------------------------------------------------------------------

// $Id$

// $Log$
// Revision 1.3  2010/02/11 22:13:11  wenger
// Merged s2d_remediated_rest_1002_br_0 thru s2d_remediated_rest_1002_br_1
// to trunk (note: s2d_remediated_rest_1002_br_1 ==
// s2d_remediated_rest_1002_br_end).
//
// Revision 1.2.4.1  2010/02/10 20:25:15  wenger
// Put remediated restraint stuff into config files, etc.
//
// Revision 1.2  2010/01/06 23:03:40  wenger
// Merged s2d_dist_rest_0912_br_0 thru s2d_dist_rest_0912_br_1 to trunk.
//
// Revision 1.1.2.3  2010/01/05 15:33:52  wenger
// Cleaned up the pdbIdToUrl() methods for various restraint-related
// classes.
//
// Revision 1.1.2.2  2009/12/16 00:07:56  wenger
// Added S2DDistRestraint and S2DNmrStarDistRIfc classes (mostly
// stubs so far); added a bunch of notes based on today's BMRB
// staff meeting discussions; also added stubbed-in classes in
// S2DSummaryHtml, etc.
//
// Revision 1.1.2.1  2009/12/14 23:31:03  wenger
// Added new S2DRestraint class to be parent of S2DTorsionAngle and
// (not yet created) S2DDistRestraint class; moved a bunch of the
// code from S2DTorsionAngle to S2DRestraint.
//

// ========================================================================

package star2devise;

import EDU.bmrb.starlibj.SaveFrameNode;
import java.io.*;
import java.util.*;
import java.net.*;

public class S2DRestraint {
    //===================================================================
    // VARIABLES

    private static final int DEBUG = 0;

    protected String _name;
    protected String _longName;
    protected String _dataDir;
    protected String _sessionDir;
    protected S2DSummaryHtml _summary;
    protected String _pdbId;
    protected String _starVersion;

    //===================================================================
    // PUBLIC METHODS

    //-------------------------------------------------------------------
//TEMPTEMP -- change to be either angle or distance!
    /**
     * Take the URL of a document containing dihedral angle violation
     * data (filtered FRED) and strip out all of the html surrounding
     * the actual NMR-STAR file, putting the NMR-STAR file into
     * a temporary file.
     * @param The URL name for the document containing the actual
     *   dihedral angle violation data.
     * @param The PDB ID we need data for.
     * @return The name of the temporary file created.
     */
    public static String filterUrlToTempFile(String urlName, String pdbId)
      throws S2DException
    {
        if (doDebugOutput(11)) {
	    System.out.println("S2DRestraint.filterUrlToTempFile(" +
	      urlName + ")");
	}

    	File tmpFile;
	try {
    	    tmpFile = File.createTempFile("tar_", ".str", new File("tmp"));
	    tmpFile.deleteOnExit();

	    FileWriter writer = new FileWriter(tmpFile);

	    URL url = new URL(urlName);
	    InputStream is = url.openStream();
	    InputStreamReader isr = new InputStreamReader(is);
	    BufferedReader reader = new BufferedReader(isr);

	    // Extract the NMR-STAR file embedded within the html page.
	    boolean foundStart = false;
	    String startString = "data_" + pdbId.toLowerCase();
	    String line;
	    while ((line = reader.readLine()) != null) {
		// Do case-insensitive searches.
		String searchLine = line.toLowerCase();

		if (!foundStart) {
		    int begin = searchLine.indexOf(startString);
		    if (begin >= 0) {
		        foundStart = true;
			writer.write(
			  line.substring(begin) + "\n");
		    }
		} else {
		    if (searchLine.indexOf("</pre>") >= 0) {
		        // Hit the end of the actual STAR file part.
			break;
		    }
	            writer.write(line + "\n");
		}
	    }

	    reader.close();
	    writer.close();

        } catch (IOException ex) {
	    throw new S2DError("Error creating temp file for torsion " +
	      "angle data: " + ex.toString());
	}

    	return tmpFile.getPath();
    }

    //-------------------------------------------------------------------
    // Constructor.
    public S2DRestraint(String name, String longName, S2DNmrStarIfc star,
      String dataDir, String sessionDir, S2DSummaryHtml summary,
      String pdbId) throws S2DException
    {
        if (doDebugOutput(11)) {
	    System.out.println("S2DRestraint.S2DRestraint(" + name + ")");
	}
        _name = name;
        _longName = longName;
        _dataDir = dataDir;
        _sessionDir = sessionDir;
        _summary = summary;

	_pdbId = pdbId;

	_starVersion = star.version();
    }

    //===================================================================
    // PROTECTED METHODS

    //-------------------------------------------------------------------
    /**
     * Get the URL name for dihedral angle violations (filtered
     * FRED data) for the given PDB ID.
     * @param The PDB ID we need data for.
     * @return The URL name corresponding to the dihedral angle violations.
     */
    protected static String getViolationUrl(String pdbId) throws S2DException
    {
        if (doDebugOutput(12)) {
	    System.out.println("S2DRestraint.getViolationUrl(" + pdbId + ")");
	}

	String restraintGridTemplate = S2DNames.RESTRAINT_GRID_ROOT +
	  S2DNames.TORSION_ANGLE_SEARCH_TEMPLATE;
	String requestUrlName = S2DUtils.replace(restraintGridTemplate,
	  "*", pdbId);

        if (doDebugOutput(12)) {
	    System.out.println("  requestUrlName: " + requestUrlName);
	}

	try {
	    URL url = new URL(requestUrlName);
	    InputStream is = url.openStream();
	    InputStreamReader isr = new InputStreamReader(is);
	    BufferedReader reader = new BufferedReader(isr);

	    String violationUrlName = null;

	    boolean foundViolation = false;
	    boolean foundDihedralAngle = false;
	    String line;
	    while ((line = reader.readLine()) != null) {
	        // Do case-insensitive searches.
	        String searchLine = line.toLowerCase();

	        // Need to find "violation", "dihedral angle", and "href" in
	        // the same table row.
	        if (searchLine.indexOf("</tr>") >= 0) {
	            foundViolation = false;
		    foundDihedralAngle = false;
	        } else if (!foundViolation) {
	            if (searchLine.indexOf("violation") >= 0) {
		        foundViolation = true;
		    }
	        } else if (!foundDihedralAngle) {
	            if (searchLine.indexOf("dihedral angle") >= 0) {
		        foundDihedralAngle = true;
		    }
	        } else if (searchLine.indexOf("href") >= 0) {
                    if (doDebugOutput(15)) {
                        System.out.println("  line: <" + line + ">");
	            }
	            violationUrlName = line.substring(line.indexOf('"') + 1);
		    violationUrlName = violationUrlName.substring(0,
		      violationUrlName.indexOf('"'));
                    if (doDebugOutput(12)) {
	                System.out.println("  violationUrlName: " +
		          violationUrlName);
	            }
		    break;
	        }
	    }

	    reader.close();

	    if (violationUrlName == null) {
	        throw new S2DError("No violation URL found for PDB " +
	          pdbId);
	    }

	    violationUrlName = S2DNames.RESTRAINT_GRID_ROOT +
	      violationUrlName;

	    return violationUrlName;

	} catch (Exception ex) {
	    throw new S2DError("Error (" + ex.toString() +
	      " getting violation URL for PDB " + pdbId);
	}
    }

    //-------------------------------------------------------------------
    /**
     * Get the MR Block ID name for the given dihedral angle violation
     * URL.
     * @param The name of the dihedral angle violation URL.
     * @param The PDB ID we need data for.
     * @return The MR Block ID (URL).
     */
    protected static String getMrBlockId(String violationUrlName,
      String pdbId) throws S2DException
    {
        if (doDebugOutput(12)) {
	    System.out.println("S2DRestraint.getMrBlockId(" +
	      violationUrlName + ")");
	}

	try {
	    URL violationUrl = new URL(violationUrlName);
	    InputStream is = violationUrl.openStream();
	    InputStreamReader isr = new InputStreamReader(is);
	    BufferedReader reader = new BufferedReader(isr);

	    String mrBlockIdName = null;

	    boolean foundMrBlockId = false;
	    boolean foundFirstHref = false;
	    String line;
	    while ((line = reader.readLine()) != null) {
	        // Do case-insensitive searches.
	        String searchLine = line.toLowerCase();

		// Find the second href after mrblock_id.
		if (!foundMrBlockId) {
	            if (searchLine.indexOf("mrblock_id") >= 0) {
		        foundMrBlockId = true;
		    }
		} else if (!foundFirstHref) {
		    if (searchLine.indexOf("href") >= 0) {
		        foundFirstHref = true;
		    }
		} else if (searchLine.indexOf("href") >= 0) {
                    if (doDebugOutput(15)) {
                        System.out.println("  line: <" + line + ">");
	            }
	            mrBlockIdName = line.substring(line.indexOf('"') + 1);
		    mrBlockIdName = mrBlockIdName.substring(0,
		      mrBlockIdName.indexOf('"'));
                    if (doDebugOutput(12)) {
	                System.out.println("  mrBlockIdName: " +
		          mrBlockIdName);
	            }
		    break;
		}
            }

	    if (mrBlockIdName == null) {
	        throw new S2DError("No MR Block ID found for PDB " +
	          pdbId);
	    }

	    mrBlockIdName = S2DNames.RESTRAINT_GRID_ROOT +
	      mrBlockIdName;

	    return mrBlockIdName;

        } catch (Exception ex) {
	    throw new S2DError("Error (" + ex.toString() +
	      " getting MR block ID for PDB " + pdbId);
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

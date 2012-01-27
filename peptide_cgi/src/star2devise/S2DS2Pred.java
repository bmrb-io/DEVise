// ========================================================================
// DEVise Data Visualization Software
// (c) Copyright 2011-2012
// By the DEVise Development Group
// Madison, Wisconsin
// All Rights Reserved.
// ========================================================================

// Under no circumstances is this software to be copied, distributed,
// or altered in any way without prior permission from the DEVise
// Development Group.

// ------------------------------------------------------------------------

// This class implements the creation of S2 experimental vs. predicted
// value visualizations.  For each set of S2 experimental vs. predicted
// data, it creates a data file, a session file, an individual html file,
// and a link in the summary html file.

// ------------------------------------------------------------------------

// $Id$

// $Log$
// Revision 1.4  2011/10/10 23:43:39  wenger
// Reduced edited movie time from .1 to .02, and set the resolution to
// 400x400 to speed up generation time (just took 2:44 in a test).
//
// Revision 1.3.2.1  2011/09/21 20:46:10  wenger
// The s2predicted session movie buttons are now updated for the correct
// PDB ID -- I should probably have configuration for the URL, though,
// and also only show the button if the movie is available.
//
// Revision 1.3  2011/05/23 19:47:17  wenger
// Added S2 predicted vs. experimental help page; fixed some minor problems
// with the s2 predicted vs. experiemental html page.
//
// Revision 1.2  2011/05/19 19:46:09  wenger
// Merged s2d_mol_dyn_br_0 thru s2d_mol_dyn_br_2 to trunk.
//
// Revision 1.1.2.11  2011/05/13 19:33:28  wenger
// Minor (hopefully final) cleanups.
//
// Revision 1.1.2.10  2011/05/13 18:05:34  wenger
// Changed s2predict PDB IDs to show up as upper-case.
//
// Revision 1.1.2.9  2011/04/12 21:03:51  wenger
// Some cleanup to the s2predict-related code.
//
// Revision 1.1.2.8  2011/04/11 22:21:09  wenger
// Added PDB ID to info in s2predict visualizations.
//
// Revision 1.1.2.7  2011/04/08 22:32:25  wenger
// Finding s2predict output files now mostly in place.
//
// Revision 1.1.2.6  2011/04/08 17:48:10  wenger
// Writing s2predicted links in the summary html page is now partially
// working.
//
// Revision 1.1.2.5  2011/04/06 19:53:36  wenger
// We now create the specific html files for the s2predicted visualizations.
//
// Revision 1.1.2.4  2011/04/06 18:48:26  wenger
// We now create the s2predicted sessions from the template.
//
// Revision 1.1.2.3  2011/04/05 21:30:31  wenger
// Reversed the sense of the s2predict output to match the S2 values in
// BMRB entries; also added test_s2p1 to the test_all script.
//
// Revision 1.1.2.2  2011/04/01 22:27:06  wenger
// Added single-letter residue codes to the residue list files; removed
// a bunch of debug output from the s2predict code.
//
// Revision 1.1.2.1  2011/04/01 18:50:35  wenger
// Basic output of the s2predict data for DEVise is in place.
//

// ========================================================================

package star2devise;

import java.io.*;
import java.util.*;
import java.net.*;
import  java.util.regex.Pattern;
import  java.util.regex.Matcher;

public class S2DS2Pred {
    //===================================================================
    // VARIABLES

    private static final int DEBUG = 0;

    private String _name;
    private String _longName;
    private String _pdbId;
    private int _coordIndex;
    private int _frameIndex;
    private String _dataDir;
    private String _sessionDir;
    private S2DSummaryHtml _summary;
    private String _entityAssemblyId;

    private class ResidueData {
        public String _entityAssemblyId;
	public String _residueSeqCode;
	public float _experimentalS2;
	public float _predictedS2;
    }
    private Vector _residueData = new Vector();

    //===================================================================
    // PUBLIC METHODS

    //-------------------------------------------------------------------
    public static class S2PData {
        public String _pdbId;
	public int _frameIndex;
    }

    //-------------------------------------------------------------------
    // Find any s2predict output files for the current entry/uploaded
    // data.
    public static Vector FindData(String s2PName) throws S2DException
    {
        if (doDebugOutput(11)) {
	    System.out.println("S2DS2Pred.FindData(" + s2PName + ")");
	}

	Vector s2PredData = new Vector();

	try {
	    URL s2PredDir = new URL(S2DNames.S2PRED_URL);
	    InputStream is = s2PredDir.openStream();
	    BufferedReader reader =
	      new BufferedReader(new InputStreamReader(is));

	    Pattern pat = Pattern.compile(".*s2pred-" + s2PName +
	      "-(.*)-(.*).out.*");
            if (doDebugOutput(21)) {
	        System.out.println("s2predict pattern: " + pat);
	    }

            String line;
	    while ((line = reader.readLine()) != null) {
		Matcher matcher = pat.matcher(line);
		if (matcher.matches()) {
                    if (doDebugOutput(21)) {
		        System.out.println("Line " + line +
			  " matches s2predict pattern");
		    }
		    S2PData data = new S2PData();
		    data._pdbId = matcher.group(1).toUpperCase();
		    data._frameIndex = Integer.parseInt(matcher.group(2));
		    s2PredData.addElement(data);
		}
	    }

	    reader.close();
	} catch(IOException ex) {
	    throw new S2DError("Error (" + ex.toString() +
	      ") reading s2predict output");
	}


	return s2PredData;
    }

    //-------------------------------------------------------------------
    // Constructor.
    public S2DS2Pred(String name, String longName, String pdbId,
      int coordIndex, int s2FrameIndex, String dataDir, String sessionDir,
      S2DSummaryHtml summary, String entityAssemblyId, String s2PName)
      throws S2DException
    {
        if (doDebugOutput(11)) {
	    System.out.println("S2DS2Pred.S2DS2Pred(" + name + ", " +
	      pdbId + ", " + coordIndex + ", " + s2FrameIndex + ", " +
	      s2PName + ")");
	}

	_name = name;
	_longName = longName;
	_pdbId = pdbId;
	_coordIndex = coordIndex;
	_frameIndex = s2FrameIndex;
	_dataDir = dataDir;
	_sessionDir = sessionDir;
	_summary = summary;
	_entityAssemblyId = entityAssemblyId;

	try {
	    String filename = getFilename(s2PName, pdbId, s2FrameIndex);
	    URL s2PredUrl = new URL(filename);
	    InputStream is = s2PredUrl.openStream();
	    BufferedReader reader =
	      new BufferedReader(new InputStreamReader(is));

            String line;
	    while ((line = reader.readLine()) != null) {
		String[] tokens = line.split("\t");
	        if (tokens.length == 3) {
		    ResidueData rd = new ResidueData();
		    try {
		        rd._entityAssemblyId = entityAssemblyId;
		        rd._residueSeqCode = tokens[0];
		        rd._experimentalS2 = Float.parseFloat(tokens[1]);
		        rd._predictedS2 = Float.parseFloat(tokens[2]);

			// For some reason, s2predict outputs everything
			// as 1-the value, instead of just the value.
			// So here we reverse the sense of the s2predict
			// output, to make it match up with the original
			// S2 values in the NMR-STAR file.
			rd._experimentalS2 = 1.0f - rd._experimentalS2;
			rd._predictedS2 = 1.0f - rd._predictedS2;
		        _residueData.addElement(rd);
		    } catch(NumberFormatException ex) {
			System.out.println("Exception (" + ex.toString() +
			  ") reading S2 values from s2predict output");
		    }
		} else {
		    System.err.println(
		      "Error parsing s2predict output line: " + line);
		}
	    }

	    reader.close();
	} catch(IOException ex) {
	    throw new S2DError("Error (" + ex.toString() +
	      ") reading s2predict output");
	}

    }

    //-------------------------------------------------------------------
    // Write the predicted vs. experimental S2 values for this data set.
    public void writeS2Pred() throws S2DException
    {
        if (doDebugOutput(11)) {
	    System.out.println("S2DS2Pred.writeS2Pred()");
	}

	try {
	    //
	    // Write the experimental and predicted S2 values to the data
	    // file.
	    //
	    FileWriter writer = S2DFileWriter.create(_dataDir +
	      File.separator + _name + S2DNames.S2PRED_SUFFIX +
	      _coordIndex + "-" + _frameIndex + S2DNames.DAT_SUFFIX);

	    writer.write("# Data: s2pred values for " + _name + "\n");
	    writer.write("# Schema: bmrb-s2pred\n");
	    writer.write("# Attributes: Entity_assembly_ID; " +
	      "Residue_seq_code; Experimental_S2; Predicted_S2\n");
	    writer.write("# Peptide-CGI version: " +
	      S2DMain.PEP_CGI_VERSION + "\n");
	    writer.write("# Generation date: " +
	      S2DMain.getTimestamp() + "\n");
	    writer.write("#\n");

	    for (int index = 0; index < _residueData.size(); index++) {
	        ResidueData rd = (ResidueData)_residueData.elementAt(index);
	        writer.write(rd._entityAssemblyId + " " +
		  rd._residueSeqCode + " " + rd._experimentalS2 + " " +
		  rd._predictedS2 + "\n");
	    }

	    writer.close();

	    //
	    // Write the session file.
	    //
	    String info = "Visualization of " + _longName;
	    if (_pdbId != null) {
	        info += " and PDB " + _pdbId;
	    }
	    String title = "S2 predicted vs. experimental values (" +
	      "entity assembly " + _entityAssemblyId + ")";
            S2DSession.write(_sessionDir, S2DUtils.TYPE_S2PRED, _name,
	      _name, _name, null, _coordIndex, _frameIndex, info, title,
	      false, "", _pdbId);

	    //
	    // Write the session-specific html file.
	    //
	    S2DSpecificHtml specHtml = new S2DSpecificHtml(
	      _summary.getHtmlDir(), S2DUtils.TYPE_S2PRED, _name,
	      _coordIndex, _frameIndex, title, "");
	    specHtml.write();

	    //
	    // Write the link in the summary html file.
	    //
	    _summary.writeS2Pred(_pdbId, _coordIndex, _frameIndex);

	} catch(IOException ex) {
	    System.err.println("IOException writing s2pred data: " +
	      ex.toString());
	    throw new S2DError("Can't write s2pred data");
	}

    }

    //===================================================================
    // PRIVATE METHODS

    //-------------------------------------------------------------------
    // Get the s2predict output filename for the given bmrbId, pdbId,
    // and S2 values frame index.
    private static String getFilename(String bmrbId, String pdbId,
      int s2FrameIndex)
    {
        String name = S2DNames.S2PRED_URL + "s2pred-" +
	  bmrbId.toLowerCase() + "-" + pdbId.toLowerCase() + "-" +
	  s2FrameIndex + ".out";
        return name;
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

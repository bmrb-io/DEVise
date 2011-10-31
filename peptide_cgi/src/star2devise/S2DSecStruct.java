// ========================================================================
// DEVise Data Visualization Software
// (c) Copyright 2011
// By the DEVise Development Group
// Madison, Wisconsin
// All Rights Reserved.
// ========================================================================

// Under no circumstances is this software to be copied, distributed,
// or altered in any way without prior permission from the DEVise
// Development Group.

// ------------------------------------------------------------------------

// This class implements getting secondary structures from the DSSP files
// on the ftp site.

// ------------------------------------------------------------------------

// $Id$

// $Log$

// ========================================================================

package star2devise;

import java.io.*;
import java.util.*;
import java.net.*;

public class S2DSecStruct {
    //===================================================================
    // VARIABLES

    private static final int DEBUG = 0;

    private String _name;
    private String _pdbId;
    private int _coordIndex;
    private int _frameIndex;
    private String _dataDir;

    private class ResidueData {
	public String _residueSeqCode;
	public String _structCode;
    }
    private Vector _residueData = new Vector();

    //===================================================================
    // PUBLIC METHODS

    //-------------------------------------------------------------------
    // Constructor.
    public S2DSecStruct(String name, String pdbId,
      int coordIndex, int s2FrameIndex, String dataDir) throws S2DException
    {
        if (doDebugOutput(11)) {
	    System.out.println("S2DSecStruct.S2DSecStruct(" + name + ", " +
	      pdbId + ", " + coordIndex + ", " + s2FrameIndex + ")");
	}

	_name = name;
	_pdbId = pdbId;
	_coordIndex = coordIndex;
	_frameIndex = s2FrameIndex;
	_dataDir = dataDir;

	try {
	    String urlName = getUrlName(_pdbId);
	    URL url = new URL(urlName);
	    InputStream is = url.openStream();
	    BufferedReader reader =
	      new BufferedReader(new InputStreamReader(is));

	    boolean foundSS = false;
            String line;
	    while ((line = reader.readLine()) != null) {
	        if (foundSS) {
		    ResidueData rd = new ResidueData();
		    rd._residueSeqCode = line.substring(0, 5).trim();
		    rd._structCode = line.substring(16, 17);
		    _residueData.addElement(rd);
                }
		if (line.startsWith("  #  RESIDUE")) foundSS = true;
	    }

	} catch (IOException ex) {
	    throw new S2DError("Error (" + ex.toString() +
	      ") reading DSSP output");
	}

    }

    //-------------------------------------------------------------------
    // Write the predicted vs. experimental S2 values for this data set.
    public void writeSecStruct() throws S2DException
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
	      File.separator + _name + S2DNames.SEC_STRUCT_SUFFIX +
	      _coordIndex + "-" + _frameIndex + S2DNames.DAT_SUFFIX);

	    writer.write("# Data: secondary structure values for " +
	      _name + "\n");
	    writer.write("# Schema: bmrb-sec_struct\n");
	    writer.write("# Attributes: Residue_seq_code; " +
	      "Sec_structure\n");
	    writer.write("# Peptide-CGI version: " +
	      S2DMain.PEP_CGI_VERSION + "\n");
	    writer.write("# Generation date: " +
	      S2DMain.getTimestamp() + "\n");
	    writer.write("#\n");

	    for (int index = 0; index < _residueData.size(); index++) {
		ResidueData rd = (ResidueData)_residueData.elementAt(index);
	        writer.write(rd._residueSeqCode + "\t");
	        writer.write(getStructName(rd._structCode) + "\n");
	    }

	    writer.close();

	} catch(IOException ex) {
	    System.err.println("IOException writing s2pred data: " +
	      ex.toString());
	    throw new S2DError("Can't write s2pred data");
	}

    }

    //===================================================================
    // PRIVATE METHODS

    //-------------------------------------------------------------------
    // Get the DSSP output URL for the given pdbId.
    private static String getUrlName(String pdbId)
    {
        String name = S2DNames.DSSP_FILE_URL + "/" +
	  pdbId.toLowerCase() + ".dssp";
        return name;
    }

    //-------------------------------------------------------------------
    // Convert DSSP secondary structure codes to names (see
    // http://swift.cmbi.ru.nl/gv/dssp/ -- "Output short").
    private static String getStructName(String structCode)
    {
	String result = "";

        if (structCode.equals("")) {
	    result = "-";

        } else if (structCode.equals("H")) {
	    result = "alpha helix";

        } else if (structCode.equals("B")) {
	    result = "beta-bridge";

        } else if (structCode.equals("E")) {
	    result = "extended strand";

        } else if (structCode.equals("G")) {
	    result = "3-helix";

        } else if (structCode.equals("I")) {
	    result = "5-helix";

        } else if (structCode.equals("T")) {
	    result = "turn";

        } else if (structCode.equals("S")) {
	    result = "bend";

	} else {
	    System.err.println(
	      "Warning: unexpected secondary structure code " + structCode);
	    result = "-";
	}

	return result;
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

// ========================================================================
// DEVise Data Visualization Software
// (c) Copyright 2001-2010
// By the DEVise Development Group
// Madison, Wisconsin
// All Rights Reserved.
// ========================================================================

// Under no circumstances is this software to be copied, distributed,
// or altered in any way without prior permission from the DEVise
// Development Group.

// ------------------------------------------------------------------------

// This class implements the output of relaxation data.  For each set of
// relaxation data, it creates a data file, a session file, an
// individual html file, and a link in the summary html file.

// ------------------------------------------------------------------------

// $Id$

// $Log$
// Revision 1.15  2010/11/01 00:51:12  wenger
// Merged sample_cond2_br_0 thru sample_cond2_br_1 to trunk.
//
// Revision 1.14.2.3  2010/10/19 00:23:20  wenger
// Split the actual sample info out from the sample conditions info,
// including modifying ambiguity code and Pistachio metadata accordingly.
//
// Revision 1.14.2.2  2010/10/16 01:32:26  wenger
// Getting sample conditions save frame names now works for 2.1 files.
//
// Revision 1.14.2.1  2010/10/15 15:29:03  wenger
// Merged sample_cond_br_0 thru sample_cond_br_1/sample_cond_br_end to
// sample_cond2_br (to get the latest code refactoring from the trunk
// into the sample conditions code).
//
// Revision 1.14  2010/10/13 20:44:02  wenger
// Finished restructuring Peptide-CGI code so that we get values from
// the STAR files in the relevant object constructors, instead of in the
// S2DMain class.
//
// ...

// ========================================================================

package star2devise;

import java.io.*;
import java.util.*;
import EDU.bmrb.starlibj.SaveFrameNode;

public class S2DRelaxation {
    //===================================================================
    // VARIABLES

    private static final int DEBUG = 0;

    private String _name;
    private String _longName;
    private String _dataDir;
    private String _sessionDir;
    private S2DSummaryHtml _summary;
    private int _entityAssemblyID;
    private String _frameDetails;
    private String _sample;
    private String _sampleConditions;

    private int _dataType;
    private String _suffix;
    private String _title;
    private String _shortName;
    private String _frequency;
    private int _freqValue;

    private String[] _resSeqCodes;
    private String[] _resLabels;
    private String[] _atomNames;
    private String[] _relaxationValues;
    private String[] _relaxationErrors;

    private String _starVersion;

    //===================================================================
    // PUBLIC METHODS

    //-------------------------------------------------------------------
    // Constructor.  (See S2DUtils for dataType.)
    public S2DRelaxation(String name, String longName, S2DNmrStarIfc star,
      SaveFrameNode frame, String dataDir, String sessionDir,
      S2DSummaryHtml summary, int dataType, String entityAssemblyID)
      throws S2DException
    {
        if (doDebugOutput(11)) {
	    System.out.println("S2DRelaxation.S2DRelaxation(" + name + ")");
	}

        _name = name;
        _longName = longName;
        _dataDir = dataDir;
        _sessionDir = sessionDir;
        _summary = summary;
	_entityAssemblyID = star.getEntityAssemblyID(frame,
	  entityAssemblyID);
	_frameDetails = star.getFrameDetails(frame);
	_sample = star.getFrameSample(frame);
	_sampleConditions = star.getFrameSampleConditions(frame);
        _starVersion = star.version();

	// Note: we should probably split this into subclasses for T1 and
	// T2, but I'm keeping things in a single class for now to do
	// restructuring more quickly.
	_dataType = dataType;
        switch (dataType) {
	case S2DUtils.TYPE_T1_RELAX:
	    initT1(star, frame, entityAssemblyID);
	    break;

	case S2DUtils.TYPE_T2_RELAX:
	    initT2(star, frame, entityAssemblyID);
	    break;

	default:
	    throw new S2DError("Illegal data type: " + dataType);
	}

	_shortName = _title + " (" + _frequency + ")";
	_title += " (" + _frequency + " MHz)";
	try { 
	    // Parse as a float, convert to int for tables in summary.
	    _freqValue = (int)Float.parseFloat(_frequency);
	} catch (NumberFormatException ex) {
	    System.err.println("Warning: exception parsing relaxation " +
	      "frequency: " + ex);
	    _freqValue = 0;
	}
    }

    //-------------------------------------------------------------------
    // Write the relaxation values for this data.
    public void writeRelaxation(int frameIndex) throws S2DException
    {
        if (doDebugOutput(11)) {
	    System.out.println("S2DRelaxation.writeRelaxation()");
	}

	try {
	    //
	    // Write the relaxation values to the data file.
	    //
            FileWriter relaxWriter = S2DFileWriter.create(_dataDir +
	      File.separator +
	      _name + _suffix + frameIndex + S2DNames.DAT_SUFFIX);
            relaxWriter.write("# Data: relaxation values for " + _name + "\n");
            relaxWriter.write("# Schema: bmrb-relax\n");
            relaxWriter.write("# Attributes: Entity_assembly_ID; " +
	      "Residue_seq_code; " +
	      "Residue_label; Atom_name; relax_value; relax_error\n");
            relaxWriter.write("# Peptide-CGI version: " +
	      S2DMain.PEP_CGI_VERSION + "\n");
            relaxWriter.write("# Generation date: " +
	      S2DMain.getTimestamp() + "\n");
            relaxWriter.write("#\n");

	    for (int index = 0; index < _resSeqCodes.length; index++) {
	        relaxWriter.write(_entityAssemblyID + " " +
		  _resSeqCodes[index] + " " +
		  _resLabels[index] + " " +
		  _atomNames[index] + " " +
		  _relaxationValues[index] + " " +
		  _relaxationErrors[index] + "\n");
	    }

	    relaxWriter.close();

	    //
	    // Write the session file.
	    //
	    String info = "Visualization of " + _longName;
	    S2DSession.write(_sessionDir, _dataType,
	      _name, frameIndex, info, _title, true, _starVersion);

	    //
	    // Write the session-specific html file.
	    //
	    String fullTitle = _title + " (entity assembly " +
	      _entityAssemblyID + ")";
	    S2DSpecificHtml specHtml = new S2DSpecificHtml(
	      _summary.getHtmlDir(), _dataType,
	      _name, frameIndex, fullTitle, _frameDetails);
	    specHtml.write();

	    //
	    // Write the link in the summary html file.
	    //
	    _summary.writeRelax(_dataType, _freqValue, _suffix, _title,
	      frameIndex, _entityAssemblyID, _resSeqCodes.length);

        } catch(IOException ex) {
	    System.err.println("IOException writing relaxation data: " +
	      ex.toString());
	    throw new S2DError("Can't write relaxation data");
	}
    }

    //-------------------------------------------------------------------
    /**
     * Add relaxation sets to the data set list.
     * @param The data set list.
     * @param The frame index.
     */
    public void addRelaxationData(Vector dataSets, int frameIndex,
      int polymerType)
    {
        // Note: attribute names must match the bmrb-relax schema.
	String dataSource = _name + _suffix + frameIndex;
	String dataName = _shortName + " [" + _entityAssemblyID + "]";
	dataSets.addElement(new S2DDatasetInfo(dataName, _frameDetails,
	  _sample, _sampleConditions, dataSource, "relax_value",
	  "bmrb-relax", "relax", _entityAssemblyID, polymerType));
    }

    //===================================================================
    // PRIVATE METHODS

    //-------------------------------------------------------------------
    // Initialize this object with T1 relaxation values.
    private void initT1(S2DNmrStarIfc star, SaveFrameNode frame,
      String entityAssemblyID) throws S2DException
    {
        _suffix = S2DNames.T1_SUFFIX;
        _title = "T1 Relaxation";

	//
	// Get the values we need from the Star file.
	//

	// If a non-blank entityAssemblyID is specified, we need to filter
	// the frame values to only take the ones corresponding to that
	// entityAssemblyID.  To do that, we get the entityAssemblyID
	// values in each row of the loop.  (entityAssemblyID will be blank
	// when processing NMR-STAR 2.1 files -- they don't have data for
	// more than one entity assembly in a single save frame).
	String[] entityAssemblyIDs = null;
	if (!entityAssemblyID.equals("")) {
	    entityAssemblyIDs = star.getFrameValues(frame,
	      star.T1_ENTITY_ASSEMBLY_ID,
	      star.T1_ENTITY_ASSEMBLY_ID);
	}

	_resSeqCodes = star.getAndFilterFrameValues(frame,
	  star.T1_VALUE, star.T1_RES_SEQ_CODE, entityAssemblyID,
	  entityAssemblyIDs);

	_resLabels = star.getAndFilterFrameValues(frame,
	  star.T1_VALUE, star.T1_RES_LABEL, entityAssemblyID,
	  entityAssemblyIDs);

	_atomNames = star.getAndFilterFrameValues(frame,
	  star.T1_VALUE, star.T1_ATOM_NAME, entityAssemblyID,
	  entityAssemblyIDs);

	_relaxationValues = star.getAndFilterFrameValues(frame,
	  star.T1_VALUE, star.T1_VALUE, entityAssemblyID,
	  entityAssemblyIDs);

	//TEMP -- 4096 has "_T1_error" instead of "_T1_value_error".
	_relaxationErrors = star.getAndFilterOptionalFrameValues(frame,
	  star.T1_VALUE, star.T1_VALUE_ERR, entityAssemblyID,
	  entityAssemblyIDs, _relaxationValues.length, "0");

        _frequency = star.getOneFrameValue(frame, star.T1_SPEC_FREQ_1H);
    }

    //-------------------------------------------------------------------
    // Initialize this object with T2 relaxation values.
    private void initT2(S2DNmrStarIfc star, SaveFrameNode frame,
      String entityAssemblyID) throws S2DException
    {
        _suffix = S2DNames.T2_SUFFIX;
        _title = "T2 Relaxation";

	//
	// Get the values we need from the Star file.
	//

	// If a non-blank entityAssemblyID is specified, we need to filter
	// the frame values to only take the ones corresponding to that
	// entityAssemblyID.  To do that, we get the entityAssemblyID
	// values in each row of the loop.  (entityAssemblyID will be blank
	// when processing NMR-STAR 2.1 files -- they don't have data for
	// more than one entity assembly in a single save frame).
	String[] entityAssemblyIDs = null;
	if (!entityAssemblyID.equals("")) {
	    entityAssemblyIDs = star.getFrameValues(frame,
	      star.T2_ENTITY_ASSEMBLY_ID,
	      star.T2_ENTITY_ASSEMBLY_ID);
	}

	_resSeqCodes = star.getAndFilterFrameValues(frame,
	  star.T2_VALUE, star.T2_RES_SEQ_CODE, entityAssemblyID,
	  entityAssemblyIDs);

	_resLabels = star.getAndFilterFrameValues(frame,
	  star.T2_VALUE, star.T2_RES_LABEL, entityAssemblyID,
	  entityAssemblyIDs);

	_atomNames = star.getAndFilterFrameValues(frame,
	  star.T2_VALUE, star.T2_ATOM_NAME, entityAssemblyID,
	  entityAssemblyIDs);

	_relaxationValues = star.getAndFilterFrameValues(frame,
	  star.T2_VALUE, star.T2_VALUE, entityAssemblyID,
	  entityAssemblyIDs);

	//TEMP -- 4096 has "_T2_error" instead of "_T2_value_error".
	_relaxationErrors = star.getAndFilterOptionalFrameValues(frame,
	  star.T2_VALUE, star.T2_VALUE_ERR, entityAssemblyID,
	  entityAssemblyIDs, _relaxationValues.length, "0");

        _frequency = star.getOneFrameValue(frame, star.T2_SPEC_FREQ_1H);
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

// ========================================================================
// DEVise Data Visualization Software
// (c) Copyright 2009
// By the DEVise Development Group
// Madison, Wisconsin
// All Rights Reserved.
// ========================================================================

// Under no circumstances is this software to be copied, distributed,
// or altered in any way without prior permission from the DEVise
// Development Group.

// ------------------------------------------------------------------------

// This class implements the calculation and output of SPARTA-calculated
// chemical shift data for proteins (we should make subclasses of this
// class if we ever output SPARTA-calcuated chemical shifts for nucleic
// acids).  For each set of chemical shift data, it creates a data
// file, a session file, an individual html file, and a link in the summary
// html file.

// ------------------------------------------------------------------------

// $Id$

// $Log$

// ========================================================================

package star2devise;

import java.io.*;
import java.util.*;

public class S2DSpartaChemShift extends S2DChemShift {
    //===================================================================
    // VARIABLES

    protected int _modelNum;
    private static final int DEBUG = 0;

    //===================================================================
    // PUBLIC METHODS

    //-------------------------------------------------------------------
    // Constructor (for SPARTA-calculated deltashifts).
    public S2DSpartaChemShift(String name, String longName, String dataDir,
      String sessionDir, S2DSummaryHtml summary, int[] resSeqCodes,
      String[] residueLabels, String[] atomNames, String[] atomTypes,
      double[] deltaShiftVals, int entityAssemblyID, int modelNum,
      String frameDetails) throws S2DException
    {
	super(name, longName, dataDir, sessionDir, summary, resSeqCodes,
	  residueLabels, atomNames, atomTypes, deltaShiftVals,
	  entityAssemblyID, modelNum, frameDetails);

        if (doDebugOutput(11)) {
	    System.out.println("S2DSpartaChemShift.S2DSpartaChemShift(" +
	      name + ")");
	}

	_modelNum = modelNum;

	// ShiftDataManager with no chemshift file creates a special
	// version for already-calculated deltashift values
	// (see ShiftDataManager.java).
        _refTable = new ShiftDataManager();

	calculateDeltaShifts();
    }

    //-------------------------------------------------------------------
    // Write the deltashifts for this data.
    public void writeDeltashifts(int frameIndex, boolean append)
      throws S2DException
    {
        if (doDebugOutput(11)) {
	    System.out.println("S2DSpartaChemShift.writeDeltashifts()");
	}

	//
	// Write the deltashift values to the appropriate data file.
	//
        FileWriter deltashiftWriter = null;
	try {
	    String fileName = _dataDir + File.separator + _name +
	      S2DNames.SPARTA_DELTASHIFT_SUFFIX + frameIndex +
	      S2DNames.DAT_SUFFIX;
	    if (append) {
                deltashiftWriter = S2DFileWriter.append(fileName);
	    } else {
                deltashiftWriter = S2DFileWriter.create(fileName);

	        deltashiftWriter.write("# Data: SPARTA-calculated " +
		  "delta shift values for " + _name + "\n");
	        deltashiftWriter.write("# Schema: bmrb-SpartaDeltaShift\n");

	        deltashiftWriter.write("# Attributes: Entity_assembly_ID; ");
	        deltashiftWriter.write("Model_number; ");
	        deltashiftWriter.write("Residue_seq_code; Residue_label; " +
	          "HA_DeltaShift; C_DeltaShift; CA_DeltaShift; CB_DeltaShift");
	        deltashiftWriter.write("; N_DeltaShift; H_DeltaShift");
	        deltashiftWriter.write("\n");

                deltashiftWriter.write("# Peptide-CGI version: " +
	          S2DMain.PEP_CGI_VERSION + "\n");
                deltashiftWriter.write("# Generation date: " +
	          S2DMain.getTimestamp() + "\n");
	        deltashiftWriter.write("#\n");
	    }

        } catch(IOException ex) {
	    System.err.println("IOException writing deltashifts: " +
	      ex.toString());
	    throw new S2DError("Can't write deltashifts");
	}

	try {
	    int dsCount = 0;
            for (int index = 0; index < _deltaShiftResLabels.length; ++index) {
	        if (!_deltaShiftResLabels[index].equals("")) {
		    dsCount++;
		    deltashiftWriter.write(_entityAssemblyID + " " +
		      _modelNum + " " +
		      index + " " +
		      _deltaShiftResLabels[index] + " " +
		      _haDeltaShifts[index] + " " +
		      _cDeltaShifts[index] + " " +
		      _caDeltaShifts[index] + " " +
		      _cbDeltaShifts[index] + " " +
		      _nDeltaShifts[index] + " " +
		      _hDeltaShifts[index] + "\n");
	        }
	    }

	    int sessionType = S2DUtils.TYPE_SPARTA_DELTASHIFT;

	    //
	    // Write the session file
	    //
	    S2DSession.write(_sessionDir, sessionType,
	      _name, frameIndex, _info, null, _hasRealCBShifts);

	    //
	    // Write the session-specific html file.
	    //
	    String title = "Chemical Shift Delta (entity assembly " +
	      _entityAssemblyID + ")";
	    title = "SPARTA-calculated " + title;
	    S2DSpecificHtml specHtml = new S2DSpecificHtml(
	      _summary.getHtmlDir(), sessionType, _name, frameIndex,
	      title, _frameDetails);
	    specHtml.write();

	    //
	    // Write the link in the summary html file.
	    //
	    if (!append) {
	        _summary.writeSpartaDeltashift(_entityAssemblyID, dsCount);
	    }

	} catch (IOException ex) {
	    System.err.println("IOException writing deltashift data: " +
	      ex.toString());
	    throw new S2DError("Unable to write deltashift data for " +
	      frameIndex);
	} finally {
	    try {
	        deltashiftWriter.close();
	    } catch (IOException ex) {
	        System.err.println("IOException: " + ex.toString());
	    }
	}
    }

    //-------------------------------------------------------------------
    /**
     * Add SPARTA-calculated delta chem shift data sets to the data set
     * list and the list of models for the SPARTA data.
     * @param The data set list.
     * @param The entity assembly ID.
     * @param Whether this is appending to the list of models.
     */
    public void addSpartaData(Vector dataSets, boolean append)
      throws S2DError
    {
        FileWriter spartaWriter = null;

	try {
	    String fileName = _dataDir + File.separator + _name +
	      S2DNames.SPARTA_DELTASHIFT_SUFFIX + _entityAssemblyID +
	      S2DNames.MODELS_SUFFIX + S2DNames.DAT_SUFFIX;
	    if (append) {
                spartaWriter = S2DFileWriter.append(fileName);
	    } else {
                spartaWriter = S2DFileWriter.create(fileName);

	        spartaWriter.write("# Data: models for SPARTA-calculated "
		  + "delta shift values for " + _name + "\n");
	        spartaWriter.write("# Schema: bmrb-SpartaModel\n");

	        spartaWriter.write("# Attributes: Label_text; " +
		  "Model_num\n");

                spartaWriter.write("# Peptide-CGI version: " +
	          S2DMain.PEP_CGI_VERSION + "\n");
                spartaWriter.write("# Generation date: " +
	          S2DMain.getTimestamp() + "\n");
	        spartaWriter.write("#\n");
	    }

	    String modelName = "" + _modelNum;
	    if (_modelNum == 0) {
	        modelName = "Avg";
	    }
	    spartaWriter.write(modelName + "\t" + _modelNum + "\n");

        } catch(IOException ex) {
	    System.err.println("IOException writing SPARTA metadata: " +
	      ex.toString());
	    throw new S2DError("Can't write SPARTA metadata");

	} finally {
	    try {
	        spartaWriter.close();
	    } catch (IOException ex) {
	        System.err.println("IOException: " + ex.toString());
	    }
	}

	// We might want to append here to the 3D data sets, but
	// because all SPARTA models are in one file it might be
	// tricky.
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

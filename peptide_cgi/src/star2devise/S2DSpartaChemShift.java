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

// This class implements the calculation and output of SPARTA-calculated
// chemical shift data for proteins (we should make subclasses of this
// class if we ever output SPARTA-calcuated chemical shifts for nucleic
// acids).  For each set of chemical shift data, it creates a data
// file, a session file, an individual html file, and a link in the summary
// html file.

// ------------------------------------------------------------------------

// $Id$

// $Log$
// Revision 1.7.6.1  2012/03/07 18:33:55  wenger
// Fixed tests for summary page changes.
//
// Revision 1.7  2011/10/10 23:43:39  wenger
// Reduced edited movie time from .1 to .02, and set the resolution to
// 400x400 to speed up generation time (just took 2:44 in a test).
//
// Revision 1.6.14.1  2011/09/21 20:46:10  wenger
// The s2predicted session movie buttons are now updated for the correct
// PDB ID -- I should probably have configuration for the URL, though,
// and also only show the button if the movie is available.
//
// Revision 1.6  2010/10/13 20:44:02  wenger
// Finished restructuring Peptide-CGI code so that we get values from
// the STAR files in the relevant object constructors, instead of in the
// S2DMain class.
//
// Revision 1.5  2010/03/10 22:36:17  wenger
// Added NMR-STAR file version to summary html page and detailed
// visualization version info (to-do 072).  (Doing this before I
// add multiple NMR-STAR paths so we can see which NMR-STAR file
// was used.)
//
// Revision 1.4  2010/02/20 00:18:36  wenger
// Finished getting SPARTA processing to work with multiple entity
// assemblies (to-do 117) and multiple chemical shift lists per entity
// assembly (to-do 118); updated test_sparta 7 and test_sparta8 accordingly.
//
// Revision 1.3  2010/02/17 21:24:40  wenger
// Fixed some errors in changing strings in the SPARTA session files.
//
// Revision 1.2  2009/10/20 22:07:56  wenger
// Reorganized the chemical shift code by moving the code that actually
// gets the data values out of S2DMain (as I did for S2DAtomicCoords).
//
// Revision 1.1  2009/10/20 16:54:10  wenger
// Created a new S2DSpartaChemShift class and cleaned up S2DChemShift
// class heirarchy in preparation for fixing things for the new SPARTA
// file format; various related cleanups (note that empty hn?.dat files
// are no longer generated for nucleic acids, resulting in changes to
// the test scripts).
//

// ========================================================================

package star2devise;

import EDU.bmrb.starlibj.SaveFrameNode;
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
    // Get the maximum model number for which there is SPARTA-calculated
    // delta shift data, for a given save frame, entity assembly ID,
    // and chem shift list ID.
    // (Note that this method will fail on a frame containing average
    // values rather than values for individual models.)
    // S2DNmrStarSpartaIfc class.
    public static int getSpartaMaxModelNum(S2DNmrStarIfc star,
      SaveFrameNode frame, String entityAssemblyID,
      String chemShiftListID) throws S2DException
    {
        if (doDebugOutput(11)) {
	    System.out.println("S2DSpartaChemShift.getSpartaMaxModelNum(" +
	      star.getFrameName(frame) + " (<" + entityAssemblyID +
	      ">, <" + chemShiftListID + ">)");
	}

	// If a non-blank entityAssemblyID is specified, we need to filter
	// the frame values to only take the ones corresponding to that
	// entityAssemblyID.  To do that, we get the entityAssemblyID
	// values in each row of the loop.  (entityAssemblyID will be blank
	// when processing NMR-STAR 2.1 files -- they don't have data for
	// more than one entity assembly in a single save frame).
	String[] entityAssemblyIDs = null;
	if (!entityAssemblyID.equals("")) {
	    entityAssemblyIDs = star.getFrameValues(frame,
	      star.DELTA_SHIFT_ENTITY_ASSEMBLY_ID,
	      star.DELTA_SHIFT_ENTITY_ASSEMBLY_ID);
	}

	// If a non-blank chemShiftListID is specified, we need to filter
	// the frame values to only take the ones corresponding to that
	// chemShiftListID.  To do that, we get the chemShiftListID
	// values in each row of the loop.
	String[] chemShiftListIDs = null;
	if (!chemShiftListID.equals("")) {
	    chemShiftListIDs = star.getAndFilterFrameValues(frame,
	      star.DELTA_CHEM_SHIFT_LIST_ID, star.DELTA_CHEM_SHIFT_LIST_ID,
	      entityAssemblyID, entityAssemblyIDs);
	}

	String[] modelNumsStr = star.getAndFilterFrameValues(frame,
	  star.DELTA_SHIFT_VALUE, star.DELTA_SHIFT_MODEL_NUM,
	  entityAssemblyID, entityAssemblyIDs);
	// Now filter by chemShiftListID if necessary.
	if (!chemShiftListID.equals("")) {
	    modelNumsStr = S2DUtils.selectMatches(chemShiftListIDs,
	      modelNumsStr, chemShiftListID);
	}
	int[] modelNums = S2DUtils.arrayStr2Int(modelNumsStr,
	  star.DELTA_SHIFT_RES_SEQ_CODE);
	modelNumsStr = null;

	int maxModelNum = 0;
	for (int index = 0; index < modelNums.length; index++) {
		maxModelNum = Math.max(maxModelNum, modelNums[index]);
	}

        if (doDebugOutput(4)) {
	    System.out.println("  maxModelNum: " + maxModelNum);
	}

	return maxModelNum;
    }


    //-------------------------------------------------------------------
    // Constructor (for SPARTA-calculated deltashifts).
    public S2DSpartaChemShift(String name, String longName,
      S2DNmrStarIfc star, SaveFrameNode frame, String dataDir,
      String sessionDir, S2DSummaryHtml summary, String entityAssemblyID,
      String chemShiftListID, int modelNum, S2DResidues residues)
      throws S2DException
    {
	super(name, longName, star, frame, dataDir, sessionDir, summary,
	  entityAssemblyID);

        if (doDebugOutput(11)) {
	    System.out.println("S2DSpartaChemShift.S2DSpartaChemShift(" +
	      name + ", <" + entityAssemblyID + ">, <" + chemShiftListID +
	      ">, " + modelNum + ")");
	}

	_chemShiftListID = chemShiftListID;
	_modelNum = modelNum;

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
	      star.DELTA_SHIFT_ENTITY_ASSEMBLY_ID,
	      star.DELTA_SHIFT_ENTITY_ASSEMBLY_ID);
	}

	String[] chemShiftListIDs = null;
	if (!chemShiftListID.equals("")) {
	    chemShiftListIDs = star.getAndFilterFrameValues(frame,
	      star.DELTA_SHIFT_VALUE, star.DELTA_CHEM_SHIFT_LIST_ID,
	      entityAssemblyID, entityAssemblyIDs);
	}

	String[] modelNumsStr = null;
	if (modelNum != 0) {
	    modelNumsStr = star.getAndFilterFrameValues(frame,
	      star.DELTA_SHIFT_VALUE, star.DELTA_SHIFT_MODEL_NUM,
	      entityAssemblyID, entityAssemblyIDs);
	}

	String[] resSeqCodesStr = star.getAndFilterFrameValues(frame,
	  star.DELTA_SHIFT_VALUE, star.DELTA_SHIFT_RES_SEQ_CODE,
	  entityAssemblyID, entityAssemblyIDs);

	_residueLabels = star.getAndFilterFrameValues(frame,
	  star.DELTA_SHIFT_VALUE, star.DELTA_SHIFT_RES_LABEL,
	  entityAssemblyID, entityAssemblyIDs);
	residues.make3Letter(_residueLabels);
	_residueLabels = S2DUtils.arrayToUpper(_residueLabels);

	_atomNames = star.getAndFilterFrameValues(frame,
	  star.DELTA_SHIFT_VALUE, star.DELTA_SHIFT_ATOM_NAME,
	  entityAssemblyID, entityAssemblyIDs);

	_atomTypes = star.getAndFilterFrameValues(frame,
	  star.DELTA_SHIFT_VALUE, star.DELTA_SHIFT_ATOM_TYPE,
	  entityAssemblyID, entityAssemblyIDs);

	String[] deltaShiftValsStr = star.getAndFilterFrameValues(frame,
	  star.DELTA_SHIFT_VALUE, star.DELTA_SHIFT_VALUE,
	  entityAssemblyID, entityAssemblyIDs);

	int entityAssemblyIDVal = star.getEntityAssemblyID(frame,
	  entityAssemblyID);

	// Filter by chem shift list ID if necessary.
    	if (!chemShiftListID.equals("")) {
	    if (modelNumsStr != null) {
                modelNumsStr = S2DUtils.selectMatches(chemShiftListIDs,
                  modelNumsStr, chemShiftListID);
            }

            resSeqCodesStr = S2DUtils.selectMatches(chemShiftListIDs,
              resSeqCodesStr, chemShiftListID);

            _residueLabels = S2DUtils.selectMatches(
              chemShiftListIDs, _residueLabels, chemShiftListID);

            _atomNames = S2DUtils.selectMatches(chemShiftListIDs,
              _atomNames, chemShiftListID);

            _atomTypes = S2DUtils.selectMatches(chemShiftListIDs,
	      _atomTypes, chemShiftListID);

            deltaShiftValsStr = S2DUtils.selectMatches(
              chemShiftListIDs, deltaShiftValsStr, chemShiftListID);
	}

	// Filter by model number if necessary.
    	if (modelNum != 0) {
            String modelStr = "" + modelNum;

            resSeqCodesStr = S2DUtils.selectMatches(modelNumsStr,
              resSeqCodesStr, modelStr);

            _residueLabels = S2DUtils.selectMatches(
              modelNumsStr, _residueLabels, modelStr);

            _atomNames = S2DUtils.selectMatches(modelNumsStr,
              _atomNames, modelStr);

            _atomTypes = S2DUtils.selectMatches(modelNumsStr,
	      _atomTypes, modelStr);

            deltaShiftValsStr = S2DUtils.selectMatches(
              modelNumsStr, deltaShiftValsStr, modelStr);
        }

	// Convert strings to numerical values as necessary.
        _resSeqCodes = S2DUtils.arrayStr2Int(
          resSeqCodesStr, star.DELTA_SHIFT_RES_SEQ_CODE);
        _chemShiftVals = S2DUtils.arrayStr2Double(
          deltaShiftValsStr, star.DELTA_SHIFT_VALUE);

	// ShiftDataManager with no chemshift file creates a special
	// version for already-calculated deltashift values
	// (see ShiftDataManager.java).
        _refTable = new ShiftDataManager();

	calculateDeltaShifts();
    }

    //-------------------------------------------------------------------
    // Write the deltashifts for this data.
    // Note: frameIndex is really entityAssemblyID for this class.
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

	        deltashiftWriter.write("# Data: SPARTA back calculated " +
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
	      _name, frameIndex, _info, null, _hasRealCBShifts,
	      _starVersion, "");

	    //
	    // Write the session-specific html file.
	    //
	    String title = "Chemical Shift Delta (entity assembly " +
	      _entityAssemblyID + ")";
	    title = "SPARTA back calculated " + title;
	    S2DSpecificHtml specHtml = new S2DSpecificHtml(
	      _summary.getHtmlDir(), sessionType, _name, frameIndex,
	      title, _frameDetails);
	    specHtml.write();

	    //
	    // Write the link in the summary html file.
	    //
	    if (!append) {
	        _summary.writeSpartaDeltashift(_entityAssemblyID,
		  frameIndex, dsCount);
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
    public void addSpartaData(Vector dataSets, int frameIndex,
      boolean append) throws S2DError
    {
        FileWriter spartaWriter = null;

	try {
	    String fileName = _dataDir + File.separator + _name +
	      S2DNames.SPARTA_DELTASHIFT_SUFFIX + frameIndex +
	      S2DNames.MODELS_SUFFIX + S2DNames.DAT_SUFFIX;
	    if (append) {
                spartaWriter = S2DFileWriter.append(fileName);
	    } else {
                spartaWriter = S2DFileWriter.create(fileName);

	        spartaWriter.write("# Data: models for SPARTA back calculated "
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

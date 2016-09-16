// ========================================================================
// DEVise Data Visualization Software
// (c) Copyright 2000-2015
// By the DEVise Development Group
// Madison, Wisconsin
// All Rights Reserved.
// ========================================================================

// Under no circumstances is this software to be copied, distributed,
// or altered in any way without prior permission from the DEVise
// Development Group.

// ------------------------------------------------------------------------

// This class implements the calculation and output of chemical shift data.
// For each set of chemical shift data, it creates a data file, a session
// file, an individual html file, and a link in the summary html file.

// ------------------------------------------------------------------------

// $Id$

// $Log$
// Revision 1.39.2.2  2015/03/04 20:46:24  wenger
// To-do 211:  Hopefully final cleanup before merge.
//
// Revision 1.39.2.1  2015/02/20 16:24:08  wenger
// To-do 211:  Re-ordered some of the data and fixed tests accordingly.
//
// Revision 1.39  2014/12/17 22:25:23  wenger
// Merged s2d_todo191_br_0 thru s2d_todo191_br_1 to trunk.
//
// Revision 1.38.6.1  2014/10/01 21:05:00  wenger
// First cut at prototypes for the changes to the simulated spectrum
// visualizations.
//
// Revision 1.38  2014/01/14 23:10:12  wenger
// Merged peak_lists2_br_0 thru peak_lists2_br_3 to trunk.
//
// Revision 1.37.8.1  2013/10/22 19:45:08  wenger
// Merged peak_lists_br_0 thru peak_lists_br_2 to peak_lists2_br.
//
// Revision 1.37.6.1  2013/06/20 20:38:59  wenger
// Dummy assignments for Cyana/xeasy implemented -- we need to read the
// chem shift save frame to make them work for real...
//
// Revision 1.37  2012/03/12 01:08:48  wenger
// Merged vis_page_fix_base thru vis_page_fix_br_1 to trunk (includes
// fixing of some tests to be more general).
//
// Revision 1.36.6.1  2012/01/20 21:00:03  wenger
// Got a bunch of the visualization summary page menus working.
//
// Revision 1.36  2011/10/10 23:43:38  wenger
// Reduced edited movie time from .1 to .02, and set the resolution to
// 400x400 to speed up generation time (just took 2:44 in a test).
//
// Revision 1.35.8.1  2011/09/21 20:46:09  wenger
// The s2predicted session movie buttons are now updated for the correct
// PDB ID -- I should probably have configuration for the URL, though,
// and also only show the button if the movie is available.
//
// Revision 1.35  2010/12/07 17:41:15  wenger
// Did another version history purge.
//
// Revision 1.34  2010/11/01 00:51:12  wenger
// Merged sample_cond2_br_0 thru sample_cond2_br_1 to trunk.
//
// Revision 1.33.2.2  2010/10/19 00:23:19  wenger
// Split the actual sample info out from the sample conditions info,
// including modifying ambiguity code and Pistachio metadata accordingly.
//
// Revision 1.33.2.1  2010/10/15 15:29:01  wenger
// Merged sample_cond_br_0 thru sample_cond_br_1/sample_cond_br_end to
// sample_cond2_br (to get the latest code refactoring from the trunk
// into the sample conditions code).
//
// Revision 1.33  2010/10/13 20:44:02  wenger
// Finished restructuring Peptide-CGI code so that we get values from
// the STAR files in the relevant object constructors, instead of in the
// S2DMain class.
//
// Revision 1.32.6.2  2010/10/11 14:38:30  wenger
// Started on method to get sample conditions for data save frames; I'm
// just committing a preliminary version until I make a change on the trunk
// to move all of the code for actually getting the relevant frame values
// down into the data-specific classes like I've already done with the
// delta shifts.
//
// Revision 1.32.6.1  2010/10/08 21:17:40  wenger
// We now put save frame details into the drill-down data for the data
// selection view in 3D visualizations; also fixed a bug in getting save
// frame details for 3.0/3.1 files.
//
// Revision 1.32  2010/03/10 22:36:16  wenger
// Added NMR-STAR file version to summary html page and detailed
// visualization version info (to-do 072).  (Doing this before I
// add multiple NMR-STAR paths so we can see which NMR-STAR file
// was used.)
//
// Revision 1.31  2010/02/20 00:18:36  wenger
// Finished getting SPARTA processing to work with multiple entity
// assemblies (to-do 117) and multiple chemical shift lists per entity
// assembly (to-do 118); updated test_sparta 7 and test_sparta8 accordingly.
//
// Revision 1.30  2010/02/17 18:48:41  wenger
// Fixed bug 093 (incorrect entity assembly IDs in 3D data sets).
//
// ...

// ========================================================================

package star2devise;

import java.io.*;
import java.util.*;
import EDU.bmrb.starlibj.SaveFrameNode;

public class S2DChemShift {
    //===================================================================
    // VARIABLES

    private static final int DEBUG = 0;

    protected String _name;
    private String _longName;
    protected String _dataDir;
    protected String _sessionDir;
    protected S2DSummaryHtml _summary;
    protected String _frameDetails;
    protected String _sample;
    protected String _sampleConditions;

    protected int[] _resSeqCodes;
    protected String[] _residueLabels;
    protected String[] _atomNames;
    protected String[] _atomTypes;
    protected double[] _chemShiftVals;
    private int[] _ambiguityVals;
    protected int _entityAssemblyID;
    protected String _chemShiftListID;

    protected String CHEMSHIFT_FILE = "chem_info" + File.separator;
    protected ShiftDataManager _refTable;

    //TEMP -- move a lot of this stuff to subclasses?
    protected String[] _deltaShiftResLabels;
    protected float[] _haDeltaShifts;
    protected float[] _cDeltaShifts;
    protected float[] _caDeltaShifts;
    protected float[] _cbDeltaShifts;
    protected float[] _nDeltaShifts; // for SPARTA only
    protected float[] _hDeltaShifts; // for SPARTA only

    protected String _starVersion;

    // Distinguish between "real" CB shifts and the values plugged in
    // with the GLY HA3->CB translation.
    protected boolean _hasRealCBShifts;

    protected String _info;

    // The set of atoms that are available for this frame/entity.
    protected HashSet _atomSet = new HashSet();

    //===================================================================
    // PUBLIC METHODS

    //-------------------------------------------------------------------
    // Factory -- experimental chemical shifts.
    public static S2DChemShift create(int polymerType, String name,
      String longName, S2DNmrStarIfc star, SaveFrameNode frame,
      String dataDir, String sessionDir, S2DSummaryHtml summary,
      String entityAssemblyID, S2DResidues residues) throws S2DException
    {
	S2DChemShift chemShift = null;

        switch (polymerType) {
	case S2DResidues.POLYMER_TYPE_PROTEIN:
	case S2DResidues.POLYMER_TYPE_UNKNOWN:
	    chemShift = new S2DProteinChemShift(name, longName, star,
	      frame, dataDir, sessionDir, summary, entityAssemblyID,
	      residues);
	    break;

	case S2DResidues.POLYMER_TYPE_DNA:
	    chemShift = new S2DDNAChemShift(name, longName, star,
	      frame, dataDir, sessionDir, summary, entityAssemblyID,
	      residues);
	    break;

	case S2DResidues.POLYMER_TYPE_RNA:
	    chemShift = new S2DRNAChemShift(name, longName, star,
	      frame, dataDir, sessionDir, summary, entityAssemblyID,
	      residues);
	    break;

	default:
	    throw new S2DError("Illegal polymer type: " + polymerType);
	}

	return chemShift;
    }

    //-------------------------------------------------------------------
    // Factory -- SPARTA-calculated delta shifts.
    public static S2DChemShift createSparta(int polymerType, String name,
      String longName, S2DNmrStarIfc star, SaveFrameNode frame,
      String dataDir, String sessionDir, S2DSummaryHtml summary,
      String entityAssemblyID, String chemShiftListID, int modelNum,
      S2DResidues residues) throws S2DException
    {
	S2DChemShift chemShift = null;

        switch (polymerType) {
	case S2DResidues.POLYMER_TYPE_PROTEIN:
	case S2DResidues.POLYMER_TYPE_UNKNOWN:
	    chemShift = new S2DSpartaChemShift(name, longName, star,
	      frame, dataDir, sessionDir, summary, entityAssemblyID,
	      chemShiftListID, modelNum, residues);
	    break;

	//TEMP -- do we need to support DNA and RNA?
	case S2DResidues.POLYMER_TYPE_DNA:
	    throw new S2DError(
	      "DNA not supported for SPARTA deltashifts");

	case S2DResidues.POLYMER_TYPE_RNA:
	    throw new S2DError(
	      "RNA not supported for SPARTA deltashifts");

	default:
	    throw new S2DError("Illegal polymer type: " + polymerType);
	}

	return chemShift;
    }

    //-------------------------------------------------------------------
    // Constructor.
    public S2DChemShift(String name, String longName, S2DNmrStarIfc star,
      SaveFrameNode frame, String dataDir, String sessionDir,
      S2DSummaryHtml summary, String entityAssemblyID) throws S2DException
    {
        if (doDebugOutput(11)) {
	    System.out.println("S2DChemShift.S2DChemShift(" + name + ", " +
	      entityAssemblyID + ")");
	}

	_name = name;
	_longName = longName;
	_dataDir = dataDir;
	_sessionDir = sessionDir;
	_summary = summary;
	_frameDetails = star.getFrameDetails(frame);
	_sample = star.getFrameSample(frame);
	_sampleConditions = star.getFrameSampleConditions(frame);

	_entityAssemblyID = star.getEntityAssemblyID(frame,
	  entityAssemblyID);

	_info = "Visualization of " + _longName;

	_starVersion = star.version();
    }

    //-------------------------------------------------------------------
    // Write the deltashifts for this data.
    public void writeDeltashifts(int frameIndex, boolean append)
      throws S2DException
    {
    }

    //-------------------------------------------------------------------
    // Write the CSI info for this data.
    public void writeCSI(int frameIndex) throws S2DException
    {
    }

    //-------------------------------------------------------------------
    // Write the percent assignments for this data.
    public void writePctAssign(int frameIndex, boolean checkPctAssign,
      String chemAssgFile)
      throws S2DException
    {
    }

    //-------------------------------------------------------------------
    // Write all chem shifts for this data.
    public void writeAllShifts(int frameIndex) throws S2DException
    {
        if (doDebugOutput(11)) {
	    System.out.println("S2DChemShift.writeAllShifts()");
	}

        FileWriter asWriter = null;
	try {
            asWriter = S2DFileWriter.create(_dataDir + File.separator +
	      _name + S2DNames.ALL_CHEM_SHIFT_SUFFIX + frameIndex +
	      S2DNames.DAT_SUFFIX);
	    asWriter.write("# Data: all chemical shifts for " + _name + "\n");
	    asWriter.write("# Schema: bmrb-AllShift\n");
	    asWriter.write("# Attributes: ChemShiftVal; AtomName; " +
	      "AtomType; Residue_seq_code; ResLabel; Entity_assembly_ID\n");
            asWriter.write("# Peptide-CGI version: " +
	      S2DMain.PEP_CGI_VERSION + "\n");
            asWriter.write("# Generation date: " +
	      S2DMain.getTimestamp() + "\n");
	    asWriter.write("#\n");

        } catch(IOException ex) {
	    System.err.println(
	      "IOException writing all chem shift values: " +
	      ex.toString());
	    throw new S2DError("Can't write all chem shift values");
	}

	try {
            //
	    // Write the chemical shift values to the data file.
	    //
	    for (int index = 0; index < _resSeqCodes.length; index++) {
	        asWriter.write(_chemShiftVals[index] + " " +
		  _atomNames[index] + " " +
		  _atomTypes[index] + " " +
		  _resSeqCodes[index] + " " +
		  _residueLabels[index] + " " +
		  _entityAssemblyID + "\n");
	    }

	    //
	    // Write the session file.
	    //
	    S2DSession.write(_sessionDir, S2DUtils.TYPE_ALL_CHEM_SHIFTS,
	      _name, frameIndex, _info, null, true, _starVersion, "");

	    //
	    // Write the session-specific html file.
	    //
	    String title = "Chemical shift distributions by amino " +
	      "acid/nucleotide (entity assembly " + _entityAssemblyID + ")";
	    S2DSpecificHtml specHtml = new S2DSpecificHtml(
	      _summary.getHtmlDir(),
	      S2DUtils.TYPE_ALL_CHEM_SHIFTS, _name, frameIndex,
	      title, _frameDetails);
	    specHtml.write();

	    //
	    // Write the link in the summary html file.
	    //
	    _summary.writeAllShifts(_entityAssemblyID, frameIndex,
	      _resSeqCodes.length);

	} catch (IOException ex) {
	    System.err.println("IOException writing all chem shifts: " +
	      ex.toString());
	    throw new S2DError("Unable to write all chem shifts for " +
	      frameIndex);
	} finally {
	    try {
	        asWriter.close();
	    } catch (IOException ex) {
	        System.err.println("IOException: " + ex.toString());
	    }
	}
    }

    //-------------------------------------------------------------------
    // Write H vs. N chem shifts for this data.
    public void writeHvsNShifts(int frameIndex) throws S2DException
    {
    }

    //-------------------------------------------------------------------
    public void writeHvsCShifts(String connectionFile, int frameIndex)
      throws S2DException
    {
    }

    //-------------------------------------------------------------------
    /**
     * Add delta chem shift data sets to the data set list.
     * @param The data set list.
     * @param The frame index.
     */
    public void addDeltaData(Vector dataSets, int frameIndex)
    {
        // Note: attribute names must match the bmrb-DeltaShift schema.
	String dataSource = _name + S2DNames.DELTASHIFT_SUFFIX + frameIndex;

	String dataName;
	if (_atomSet.contains("HA")) {
	    dataName = "HA delta chem shift [" + _entityAssemblyID + "]";
	    dataSets.addElement(new S2DDatasetInfo(dataName, _frameDetails,
	      _sample, _sampleConditions, dataSource, "HA_DeltaShift",
	      "bmrb-DeltaShift", "DeltaShift", _entityAssemblyID,
	      S2DResidues.POLYMER_TYPE_PROTEIN));
        }

	if (_atomSet.contains("C")) {
	    dataName = "C delta chem shift [" + _entityAssemblyID + "]";
	    dataSets.addElement(new S2DDatasetInfo(dataName, _frameDetails,
	      _sample, _sampleConditions, dataSource, "C_DeltaShift",
	      "bmrb-DeltaShift", "DeltaShift", _entityAssemblyID,
	      S2DResidues.POLYMER_TYPE_PROTEIN));
        }

	if (_atomSet.contains("CA")) {
	    dataName = "CA delta chem shift [" + _entityAssemblyID + "]";
	    dataSets.addElement(new S2DDatasetInfo(dataName, _frameDetails,
	      _sample, _sampleConditions, dataSource, "CA_DeltaShift",
	      "bmrb-DeltaShift", "DeltaShift", _entityAssemblyID,
	      S2DResidues.POLYMER_TYPE_PROTEIN));
        }

	if (_hasRealCBShifts) {
	    dataName = "CB delta chem shift [" + _entityAssemblyID + "]";
	    dataSets.addElement(new S2DDatasetInfo(dataName, _frameDetails,
	      _sample, _sampleConditions, dataSource, "CB_DeltaShift",
	      "bmrb-DeltaShift", "DeltaShift", _entityAssemblyID,
	      S2DResidues.POLYMER_TYPE_PROTEIN));
	}
    }

    //-------------------------------------------------------------------
    /**
     * Add chem shift index data sets to the data set list.
     * @param The data set list.
     * @param The frame index.
     */
    public void addCsiData(Vector dataSets, int frameIndex)
    {
    }

    //-------------------------------------------------------------------
    /**
     * Add percent assignment data sets to the data set list.
     * @param The data set list.
     * @param The frame index.
     */
    public void addPctAssignData(Vector dataSets, int frameIndex)
    {
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
    }

    //===================================================================
    // PROTECTED METHODS

    //-------------------------------------------------------------------
    /**
     * Get experimental values needed for chemical shift output from
     * the given STAR file and frame.
     * @param The NMR-STAR file
     * @param The frame containing chemical shifts
     * @param The entity assembly ID we're currently processing
     * @param The residues for the current entity assembly
     */
    void getExperimentalValues(S2DNmrStarIfc star, SaveFrameNode frame,
      String entityAssemblyID, S2DResidues residues) throws S2DException
    {
        if (doDebugOutput(11)) {
	    System.out.println("S2DChemShift.getExperimentalValues(" +
	      star.getFrameName(frame) + ", " + entityAssemblyID + ")");
	}

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
	      star.CHEM_SHIFT_ENTITY_ASSEMBLY_ID,
	      star.CHEM_SHIFT_ENTITY_ASSEMBLY_ID);
	}

	String[] resSeqCodesTmp = star.getAndFilterFrameValues(frame,
	  star.CHEM_SHIFT_VALUE, star.CHEM_SHIFT_RES_SEQ_CODE,
	  entityAssemblyID, entityAssemblyIDs);
	_resSeqCodes = S2DUtils.arrayStr2Int(resSeqCodesTmp,
	  star.CHEM_SHIFT_RES_SEQ_CODE);
	resSeqCodesTmp = null;

	//TEMP -- I don't understand the relationship here between residues and _residueLabels -- is residues just kind of a placeholder or something just to have the polymer type?
	_residueLabels = star.getAndFilterFrameValues(frame,
	  star.CHEM_SHIFT_VALUE, star.CHEM_SHIFT_RES_LABEL, entityAssemblyID,
	  entityAssemblyIDs);
	residues.make3Letter(_residueLabels);

	_atomNames = star.getAndFilterFrameValues(frame,
	  star.CHEM_SHIFT_VALUE, star.CHEM_SHIFT_ATOM_NAME, entityAssemblyID,
	  entityAssemblyIDs);

	try { 
	    _atomTypes = star.getAndFilterFrameValues(frame,
	      star.CHEM_SHIFT_VALUE, star.CHEM_SHIFT_ATOM_TYPE,
	      entityAssemblyID, entityAssemblyIDs);
	} catch (S2DException ex) {
	    if (doDebugOutput(0)) {
	    	System.out.println("Warning: unable to get " +
		  star.CHEM_SHIFT_ATOM_TYPE +
		  " values (" + ex.toString() + "); deriving them from " +
		  star.CHEM_SHIFT_ATOM_NAME + " values instead");
	    }

	    _atomTypes = S2DUtils.atomName2AtomType(_atomNames);
	}

	String[] chemShiftsTmp = star.getAndFilterFrameValues(frame,
	  star.CHEM_SHIFT_VALUE, star.CHEM_SHIFT_VALUE, entityAssemblyID,
	  entityAssemblyIDs);
        _chemShiftVals = S2DUtils.arrayStr2Double(chemShiftsTmp,
	  star.CHEM_SHIFT_VALUE);
	chemShiftsTmp = null;

	try {
	    String[] ambiguityTmp = star.getAndFilterFrameValues(frame,
	      star.CHEM_SHIFT_VALUE, star.CHEM_SHIFT_AMBIG_CODE,
	      entityAssemblyID, entityAssemblyIDs);
	    if (S2DUtils.entireArrayMatches(ambiguityTmp, ".")) {
	        throw new S2DWarning("Ambiguity code values are all null");
	    }
	    _ambiguityVals = S2DUtils.arrayStr2Int(ambiguityTmp,
	      star.CHEM_SHIFT_AMBIG_CODE);
	    ambiguityTmp = null;
	} catch (S2DException ex) {
            if (doDebugOutput(4)) {
	        System.out.println("No ambiguity values in this save frame (" +
		  star.getFrameName(frame) + ")");
	    }
	    _ambiguityVals = new int[_resSeqCodes.length];
	    for (int index = 0; index < _ambiguityVals.length; ++index) {
	    	_ambiguityVals[index] = 9;
	    }
	}

	_entityAssemblyID = star.getEntityAssemblyID(frame,
	  entityAssemblyID);
    }

    //-------------------------------------------------------------------
    /**
     * Find the last (highest-numbered) residue in this set of chemical
     * shift data.
     * @return The highest residue sequence code in the data.
     */
    protected int findLastResidue()
    {
	// We calculate lastResidue this way to allow for the data not
	// being strictly ordered by residue.
	int lastResidue = 0;
	for (int index = 0; index < _resSeqCodes.length; index++) {
	    lastResidue = Math.max(lastResidue, _resSeqCodes[index]);
	}

	return lastResidue;
    }

    //-------------------------------------------------------------------
    // Calculate all delta shifts for this data set.
    protected void calculateDeltaShifts() throws S2DException
    {
        if (doDebugOutput(11)) {
	    System.out.println("S2DChemShift.calculateDeltaShifts()");
	}

	int lastResidue = findLastResidue();

	// Residues normally start with 1 -- skip the first element of
	// these arrays to make things simpler.
	_deltaShiftResLabels = new String[lastResidue + 1];
	_haDeltaShifts = new float[lastResidue + 1];
	_cDeltaShifts = new float[lastResidue + 1];
	_caDeltaShifts = new float[lastResidue + 1];
	_cbDeltaShifts = new float[lastResidue + 1];
	_nDeltaShifts = new float[lastResidue + 1];
	_hDeltaShifts = new float[lastResidue + 1];

	float[] ha2DeltaShifts = new float[lastResidue + 1];
	float[] ha3DeltaShifts = new float[lastResidue + 1];

    	int[] ha2Ambiguities = new int[lastResidue + 1];
    	int[] ha3Ambiguities = new int[lastResidue + 1];

	_hasRealCBShifts = false;

	for (int index = 0; index <= lastResidue; ++index) {
	    _deltaShiftResLabels[index] = "";
	}

	int prevResSeqCode = -1; // invalid value
	for (int index = 0; index < _resSeqCodes.length; ++index) {

	    int currResSeqCode = _resSeqCodes[index];
	    String resLabel = _residueLabels[index];
	    String atomName = _atomNames[index];
	    _atomSet.add(atomName);
	    double chemShift = _chemShiftVals[index];

	    if (currResSeqCode != prevResSeqCode) {
	        _deltaShiftResLabels[currResSeqCode] = resLabel;
	        prevResSeqCode = currResSeqCode;
	    }

	    try {
	        ShiftDataManager.Pair standardValue =
		  _refTable.returnValues(resLabel, atomName);

		// Note: do the calculation in double and truncate to float
		// to avoid getting values like 0.05000000000000071.
		float deltashift = (float)(chemShift -
		  standardValue.chemshift);

		//TEMP -- need alg reference here
		//TEMP -- should we make sure the resLabel is *not* GLY for HA?
	        if (atomName.equalsIgnoreCase(S2DNames.ATOM_HA)) {
		    _haDeltaShifts[currResSeqCode] = deltashift;

		} else if (atomName.equalsIgnoreCase(S2DNames.ATOM_HA2)) {
		    ha2DeltaShifts[currResSeqCode] = deltashift;
		    if ( _ambiguityVals != null) {
		        ha2Ambiguities[currResSeqCode] = _ambiguityVals[index];
		    }

		} else if (atomName.equalsIgnoreCase(S2DNames.ATOM_HA3)) {
		    ha3DeltaShifts[currResSeqCode] = deltashift;
		    if ( _ambiguityVals != null) {
		        ha3Ambiguities[currResSeqCode] = _ambiguityVals[index];
		    }

	        } else if (atomName.equalsIgnoreCase(S2DNames.ATOM_C)) {
		    _cDeltaShifts[currResSeqCode] = deltashift;

		} else if (atomName.equalsIgnoreCase(S2DNames.ATOM_CA)) {
		    _caDeltaShifts[currResSeqCode] = deltashift;

		} else if (atomName.equalsIgnoreCase(S2DNames.ATOM_CB)) {
		    _cbDeltaShifts[currResSeqCode] = deltashift;
		    _hasRealCBShifts = true;

		} else if (atomName.equalsIgnoreCase(S2DNames.ATOM_HA3) &&
		  resLabel.equalsIgnoreCase(S2DNames.ACID_GLY)) {
		    // Special case for GLY as per info from Eldon.
		    _cbDeltaShifts[currResSeqCode] = deltashift;

		} else if (atomName.equalsIgnoreCase(S2DNames.ATOM_N)) {
		    _nDeltaShifts[currResSeqCode] = deltashift;

		} else if (atomName.equalsIgnoreCase(S2DNames.ATOM_H)) {
		    _hDeltaShifts[currResSeqCode] = deltashift;

                } else {
	            //TEMP -- should we ever get here????
	        }
	    } catch(S2DWarning ex) {
	        if (doDebugOutput(11)) {
	            System.err.println(ex.toString());
	        }
	    } catch(S2DException ex) {
	        System.err.println(ex.toString());
	    }
	}

	// Special-case calculations for GLY because it has no CB.
	// As per Wishart papers, discussions with Eldon, etc.
        for (int index = 0; index < _deltaShiftResLabels.length; ++index) {
	     if (_deltaShiftResLabels[index].equalsIgnoreCase(
	       S2DNames.ACID_GLY)) {
		if ((ha2Ambiguities[index] == 1) &&
		  (ha3Ambiguities[index] == 1)) {
		    _haDeltaShifts[index] = ha2DeltaShifts[index];
		    _cbDeltaShifts[index] = ha3DeltaShifts[index];
		} else if ((ha2Ambiguities[index] == 2) &&
		  (ha3Ambiguities[index] == 2)) {
		    _haDeltaShifts[index] = (ha2DeltaShifts[index] +
		      ha3DeltaShifts[index]) / (float)2;
		    _cbDeltaShifts[index] = (float)0.0;
		} else {
		    _haDeltaShifts[index] = (float)0.0;
		    _cbDeltaShifts[index] = (float)0.0;
		}

	    }
	}
    }

    //-------------------------------------------------------------------
    // Calculate the chemical shift index for the given values; returns
    // 0 if there is an error.
    protected int calculateCSI(String resLabel, String atomName,
      double deltaShift)
    {
        int csi = 0;
	    try {
            ShiftDataManager.Pair standardValue =
	      _refTable.returnValues(resLabel, atomName);

	    if (deltaShift > standardValue.offset) {
	        csi = 1;
	    } else if (deltaShift < -1.0 * standardValue.offset) {
	        csi = -1;
	    }
	} catch(S2DWarning ex) {
	    if (doDebugOutput(11)) {
	        System.err.println(ex.toString());
	    }
	} catch (S2DException ex) {
	    System.err.println(ex.toString());
	}

	return csi;
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

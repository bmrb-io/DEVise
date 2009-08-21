// ========================================================================
// DEVise Data Visualization Software
// (c) Copyright 2000-2009
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
// Revision 1.23  2009/04/15 16:21:04  wenger
// Merged s2d_hc_spectrum_br_0 thru s2d_hc_spectrum_br_end to trunk;
// fixed test61 and test61_3.
//
// Revision 1.22.2.1  2009/04/09 20:20:54  wenger
// HvsC simulated spectrum stuff is partly in place -- data is generated
// (but not fully tested, plus lots of temporary code still in place);
// schema and session template have been generated; processing does not
// yet generate the session file, specific HTML file, or the link in
// the summary HTML file.
//
// Revision 1.22  2009/03/25 21:49:09  wenger
// Final cleanup of some of the nucleic-acid-related code, especially
// getting polymer types correctly for mmCIF files; added nucleic acid
// tests to pre-release testing document.
//
// Revision 1.21  2009/03/24 19:04:50  wenger
// Fixed layout of nucleic acid deltashift session (made windows line
// up better, etc.); fixed nucleotide counts in summary html page, and
// changed residue to nucleotide where appropriate; fixed nucleic acid
// deltashift html pages so that they link to the nucleic-acid-specific
// help page.
//
// Revision 1.20  2009/03/16 20:33:08  wenger
// 3D data select view now only shows data that is available for the
// currently-selected entity assembly ID.
//
// Revision 1.19  2009/03/12 17:30:19  wenger
// Changed entity assembly names to things like "EA 1 (polypeptide(L))"
// as requested by Eldon; changed tests accordingly.
//
// Revision 1.18  2009/03/11 19:53:14  wenger
// Implemented two-stage selection of data sets in coordinate
// visualizations (select entity assembly, then select data set); updated
// tests accordingly.  Updated test14 because it now generates data for
// entity assembly 1.
//
// Revision 1.17  2009/02/25 21:33:14  wenger
// Added residue labels to all data that were missing them (in preparation
// for selection by nucleotide in nucleotide visualizations; also allows
// user to drill down to see residue label in protein deltashift, CSI,
// etc., visualizations); changed tests accordingly; also fixed up some
// tests that weren't checking for entity assembly IDs in data.
//
// Revision 1.16  2009/02/19 22:40:41  wenger
// DNA and RNA deltashift calculations now work (still need to check
// that all values are correct); added value checks to relevant tests.
//
// Revision 1.15  2009/02/18 21:43:04  wenger
// Added S2DNAChemShift class to clean up nucleic acid code (this class
// will do the actual calculation and writing of chemical shift deltas
// for nucleic acids); added schemas for nucleic acid deltashift
// visualizations; updated tests to reflect the fact that (at least
// initially) we're not going to generate CSI visualizations for nucleic
// acids.
//
// Revision 1.14  2009/02/18 18:10:49  wenger
// Fixed bug 065 (don't process non-polymer entities).
//
// Revision 1.13  2009/01/29 22:04:57  wenger
// Made protein, DNA, and RNA subclasses of S2DChemShift to make further
// stuff easier; added some file checking to test64 and test65 (but
// delta shifts and CSI don't work yet for nucleic acids); committing
// again with nucleic acid stuff disabled.
//
// Revision 1.12  2008/12/01 20:37:52  wenger
// Merged s2d_bug_037_br_0 thru s2d_bug_037_br_2 to trunk.
//
// Revision 1.11  2008/11/14 21:14:59  wenger
// Fixed bugs 070 and 075 (problems with percent assignment values
// sometimes being greater than 100% for NMR-STAR 3.1 files).
//
// Revision 1.10  2008/11/13 22:23:38  wenger
// Added -check_pct command-line argument to cause fatal error if
// percent assigned is > 100%, and added that flag to all tests;
// also changed test scripts to avoid overwriting output logs within
// a single test.  (Note: the following tests now fail: test1_3 test3_3
// test4_3 test14_3 test15_3 test24 test38 test39 test40 test41 test47
// test52.)
//
// Revision 1.9  2008/09/23 16:43:23  wenger
// Fixed bug 067 (bad last line of H vs N data).
//
// Revision 1.8.2.7  2008/11/19 21:27:22  wenger
// Cleaned up various notes about things to check.
//
// Revision 1.8.2.6  2008/11/17 19:28:07  wenger
// Added entity assembly IDs to summary page and specific visualization pages.
//
// Revision 1.8.2.5  2008/10/03 21:08:14  wenger
// Okay, I think the basic chemical shift stuff is finally working
// right for various combinations of multiple entities and multiple
// entity assemblies (see test57* and test58*).  Lots of the other
// stuff still needs work, though.
//
// Revision 1.8.2.4  2008/09/05 22:16:55  wenger
// Got saving of master residue list to work for 4310; removed saving
// of per-chem shift frame residue lists; some more comments about
// tests.
//
// Revision 1.8.2.3  2008/08/14 20:19:21  wenger
// Fixed problems in writing entity assembly IDs for chemical shift
// data (also includes fixing bug 067, which was already present before
// I started the current bug 037 multiple entity fixes).
//
// Revision 1.8.2.2  2008/08/13 19:06:51  wenger
// Writing entity assembly IDs now works for the chem shift data for
// 3.0/3.1 files.
//
// Revision 1.8.2.1  2008/07/30 16:13:55  wenger
// First steps towards fixing bug 037/etc -- added (dummy) entity
// assembly ID values to generated data; updated schemas and tests
// accordingly.
//
// Revision 1.8  2008/07/02 16:29:19  wenger
// S2 order parameter visualizations are done and approved by Eldon;
// tests at least partially updated for S2 order stuff;
// reversed the order of data sets in the data selection view of
// 3D visualizations (more closely matches the summary page); minor
// fix to testclean target in top-level makefile; minor fix to
// relaxation session template (bar widths now set); added indices
// to data set titles in 3D visualizations.
//
// Revision 1.7  2008/04/09 19:35:41  wenger
// Added frame details to individual visualization pages in preparation
// for summary page changes; spelled out Linear Analysis of Chemical
// Shifts; removed some unneeded parameters from the S2DSummaryHtml*
// constructors.
//
// Revision 1.6  2007/12/20 16:49:02  wenger
// Improved ChemShiftRef error messages; ChemShift calculation failing
// is no longer considered an error at the top level of the program;
// S2DSpecificHtml methods are no longer static so new S2DCSRErrorHtml
// class could inherit from it correctly; some cache checking output
// is now printed at a lower versbosity setting.
//
// Revision 1.5  2007/11/15 17:15:34  wenger
// Cleaned out cvs history in source files.
//
// Revision 1.4  2007/08/20 20:26:07  wenger
// Added -verb command-line flag and property so we can turn on debug
// output without recompiling; added debug_level property corresponding
// to the existing -debug command-line flag.
//
// Revision 1.3  2006/02/01 21:34:32  wenger
// Merged peptide_cgi_10_8_0_br_0 thru peptide_cgi_10_8_0_br_2
// to the trunk.
//
// Revision 1.2  2006/02/01 20:23:10  wenger
// Merged V2_1b4_br_0 thru peptide_cgi_10_8_0_base to the
// trunk.
//
// Revision 1.1.2.14.8.1  2006/01/24 19:34:24  wenger
// Fixed bug 049 (pending confirmation from Eldon):
// errors in the chemical shift index calculation
// (use individual CSI values instead of deltashifts, take
// ambiguity codes into account for GLY special case).
//
// ...

// ========================================================================

package star2devise;

import java.io.*;
import java.util.*;

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

    protected int[] _resSeqCodes;
    protected String[] _residueLabels;
    protected String[] _atomNames;
    protected String[] _atomTypes;
    protected double[] _chemShiftVals;
    private int[] _ambiguityVals;
    protected int _entityAssemblyID;

    protected String CHEMSHIFT_FILE = "chem_info" + File.separator;
    protected ShiftDataManager _refTable;

    //TEMP -- move a lot of this stuff to subclasses?
    private String[] _deltaShiftResLabels;
    private float[] _haDeltaShifts;
    private float[] _cDeltaShifts;
    private float[] _caDeltaShifts;
    private float[] _cbDeltaShifts;

    // Distinguish between "real" CB shifts and the values plugged in
    // with the GLY HA3->CB translation.
    private boolean _hasRealCBShifts;

    protected String _info;

    // The set of atoms that are available for this frame/entity.
    protected HashSet _atomSet = new HashSet();


    //===================================================================
    // PUBLIC METHODS

    //-------------------------------------------------------------------
    // Factory.
    public static S2DChemShift create(int polymerType, String name,
      String longName, String dataDir,
      String sessionDir, S2DSummaryHtml summary, int[] resSeqCodes,
      String[] residueLabels, String[] atomNames, String[] atomTypes,
      double[] chemShiftVals, int[] ambiguityVals,
      int entityAssemblyID, String frameDetails)
      throws S2DException
    {
	S2DChemShift chemShift;

        switch (polymerType) {
	case S2DResidues.POLYMER_TYPE_PROTEIN:
	case S2DResidues.POLYMER_TYPE_UNKNOWN:
	    chemShift = new S2DProteinChemShift(name, longName, dataDir,
	      sessionDir, summary, resSeqCodes, residueLabels, atomNames,
	      atomTypes, chemShiftVals, ambiguityVals, entityAssemblyID,
	      frameDetails);
	    break;

	case S2DResidues.POLYMER_TYPE_DNA:
	    chemShift = new S2DDNAChemShift(name, longName, dataDir,
	      sessionDir, summary, resSeqCodes, residueLabels, atomNames,
	      atomTypes, chemShiftVals, ambiguityVals, entityAssemblyID,
	      frameDetails);
	    break;

	case S2DResidues.POLYMER_TYPE_RNA:
	    chemShift = new S2DRNAChemShift(name, longName, dataDir,
	      sessionDir, summary, resSeqCodes, residueLabels, atomNames,
	      atomTypes, chemShiftVals, ambiguityVals, entityAssemblyID,
	      frameDetails);
	    break;

	default:
	    throw new S2DError("Illegal polymer type: " + polymerType);
	}

	return chemShift;
    }

    //-------------------------------------------------------------------
    // Constructor.
    public S2DChemShift(String name, String longName, String dataDir,
      String sessionDir, S2DSummaryHtml summary, int[] resSeqCodes,
      String[] residueLabels, String[] atomNames, String[] atomTypes,
      double[] chemShiftVals, int[] ambiguityVals,
      int entityAssemblyID, String frameDetails)
      throws S2DException
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
	_frameDetails = frameDetails;

	_resSeqCodes = resSeqCodes;
	_residueLabels = S2DUtils.arrayToUpper(residueLabels);
	_atomNames = atomNames;
	_atomTypes = atomTypes;
	_chemShiftVals = chemShiftVals;
	_ambiguityVals = ambiguityVals;
	_entityAssemblyID = entityAssemblyID;

	_info = "Visualization of " + _longName;
    }

    //-------------------------------------------------------------------
    // Write the deltashifts for this data.
    public void writeDeltashifts(int frameIndex) throws S2DException
    {
        if (doDebugOutput(11)) {
	    System.out.println("S2DChemShift.writeDeltashifts()");
	}

	//
	// Write the deltashift values to the appropriate data file.
	//
        FileWriter deltashiftWriter = null;
	try {
            deltashiftWriter = S2DFileWriter.create(_dataDir + File.separator +
	      _name + S2DNames.DELTASHIFT_SUFFIX + frameIndex +
	      S2DNames.DAT_SUFFIX);
	    deltashiftWriter.write("# Data: delta shift values for " +
	      _name + "\n");
	    deltashiftWriter.write("# Schema: bmrb-DeltaShift\n");
	    deltashiftWriter.write("# Attributes: Entity_assembly_ID; " +
	      "Residue_seq_code; Residue_label; " +
	      "HA_DeltaShift; C_DeltaShift; CA_DeltaShift; CB_DeltaShift\n");
            deltashiftWriter.write("# Peptide-CGI version: " +
	      S2DMain.PEP_CGI_VERSION + "\n");
            deltashiftWriter.write("# Generation date: " +
	      S2DMain.getTimestamp() + "\n");
	    deltashiftWriter.write("#\n");

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
		      index + " " +
		      _deltaShiftResLabels[index] + " " +
		      _haDeltaShifts[index] + " " +
		      _cDeltaShifts[index] + " " +
		      _caDeltaShifts[index] + " " +
		      _cbDeltaShifts[index] + "\n");
	        }
	    }

	    //
	    // Write the session file
	    //
	    S2DSession.write(_sessionDir, S2DUtils.TYPE_DELTASHIFT,
	      _name, frameIndex, _info, null, _hasRealCBShifts);

	    //
	    // Write the session-specific html file.
	    //
	    String title = "Chemical Shift Delta (entity assembly " +
	      _entityAssemblyID + ")";
	    S2DSpecificHtml specHtml = new S2DSpecificHtml(
	      _summary.getHtmlDir(),
	      S2DUtils.TYPE_DELTASHIFT, _name, frameIndex,
	      title, _frameDetails);
	    specHtml.write();

	    //
	    // Write the link in the summary html file.
	    //
	    _summary.writeDeltashift(frameIndex, _entityAssemblyID,
	      dsCount, false);

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
    // Write the CSI info for this data.
    public void writeCSI(int frameIndex) throws S2DException
    {
        if (doDebugOutput(11)) {
	    System.out.println("S2DChemShift.writeCSI()");
	}

	//
	// Calculate the CSI values and write them to the data file.
	//
        FileWriter csiWriter = null;
	try {
            csiWriter = S2DFileWriter.create(_dataDir + File.separator +
	      _name + S2DNames.CSI_SUFFIX + frameIndex +
	      S2DNames.DAT_SUFFIX);
	    csiWriter.write("# Data: chemical shift index values for " +
	      _name + "\n");
	    csiWriter.write("# Schema: bmrb-Csi\n");
	    csiWriter.write("# Attributes: Entity_assembly_ID; " +
	      "Residue_seq_code; Residue_label; " +
	      "HA_Csi; C_Csi; CA_Csi; CB_Csi; Consensus_Csi\n");
            csiWriter.write("# Peptide-CGI version: " +
	      S2DMain.PEP_CGI_VERSION + "\n");
            csiWriter.write("# Generation date: " +
	      S2DMain.getTimestamp() + "\n");
	    csiWriter.write("#\n");
        } catch(IOException ex) {
	    System.err.println("IOException writing CSI values: " +
	      ex.toString());
	    throw new S2DError("Can't write CSI values");
	}

	int csiCount = 0;

	try { //TEMP -- should the try be inside the loop?
            for (int index = 0; index < _deltaShiftResLabels.length; ++index) {
		String resLabel = _deltaShiftResLabels[index];
	        if (!resLabel.equals("")) {

		    int haCsi;
		    if (resLabel.equalsIgnoreCase(S2DNames.ACID_GLY)) {
			// Note: _haDeltaShifts value has already been
			// calculated appropriately.
		        haCsi = calculateCSI(resLabel,
		          S2DNames.ATOM_HA2, _haDeltaShifts[index]);
		    } else {
		        haCsi = calculateCSI(resLabel,
		          S2DNames.ATOM_HA, _haDeltaShifts[index]);
		    }

		    int cCsi = calculateCSI(resLabel,
		      S2DNames.ATOM_C, _cDeltaShifts[index]);

		    int caCsi = calculateCSI(resLabel,
		      S2DNames.ATOM_CA, _caDeltaShifts[index]);

		    int cbCsi;
		    if (resLabel.equalsIgnoreCase(S2DNames.ACID_GLY)) {
			// Note: _cbDeltaShifts value has already been
		        cbCsi = calculateCSI(resLabel, 
			  S2DNames.ATOM_HA3, _cbDeltaShifts[index]);
		    } else {
		        cbCsi = calculateCSI(resLabel, 
			  S2DNames.ATOM_CB, _cbDeltaShifts[index]);
		    }

		    int consCsi = haCsi - cCsi - caCsi;
		    if (consCsi < 0) {
		    	consCsi = -1;
		    } else if (consCsi > 0) {
		    	consCsi = 1;
		    }

		    csiWriter.write(_entityAssemblyID + " " +
		      index + " " +
		      resLabel + " " +
		      haCsi + " " +
		      cCsi + " " +
		      caCsi + " " +
		      cbCsi + " " +
		      consCsi + "\n");
		    csiCount++;
		}
            }

	    //
	    // Write the session file.
	    //
	    S2DSession.write(_sessionDir, S2DUtils.TYPE_CSI,
	      _name, frameIndex, _info, null, _hasRealCBShifts);

	    //
	    // Write the session-specific html file.
	    //
	    String title = "Chemical Shift Index (entity assembly " +
	      _entityAssemblyID + ")";
	    S2DSpecificHtml specHtml = new S2DSpecificHtml(
	      _summary.getHtmlDir(), S2DUtils.TYPE_CSI,
	      _name, frameIndex, title, _frameDetails);
	    specHtml.write();

	    //
	    // Write the link in the summary html file.
	    //
	    _summary.writeCSI(frameIndex, csiCount);

	} catch (IOException ex) {
	    System.err.println("IOException writing CSI data: " +
	      ex.toString());
	    throw new S2DError("Unable to write CSI data for " +
	      frameIndex);
	} finally {
	    try {
	        csiWriter.close();
	    } catch (IOException ex) {
	        System.err.println("IOException: " + ex.toString());
	    }
	}
    }

    //-------------------------------------------------------------------
    // Write the percent assignments for this data.
    public void writePctAssign(int frameIndex, boolean checkPctAssign,
      String chemAssgFile)
      throws S2DException
    {
        if (doDebugOutput(11)) {
	    System.out.println("S2DChemShift.writePctAssign()");
	}

	//
	// Calculate the percent assignment values and write them to the
	// data file.
	//
        FileWriter pctWriter = null;
	AssgDataManager assgTable = null;
	try {
            pctWriter = S2DFileWriter.create(_dataDir + File.separator +
	      _name + S2DNames.PERCENT_ASSIGN_SUFFIX + frameIndex +
	      S2DNames.DAT_SUFFIX);
	    pctWriter.write("# Data: percent assignment values for " +
	      _name + "\n");
	    pctWriter.write("# Schema: bmrb-Percent\n");
	    pctWriter.write("# Attribute: Entity_assembly_ID; " +
	      "Residue_seq_code; " +
	      "CurrResidueLabel; assigForH; assigForC; assigForN\n");
            pctWriter.write("# Peptide-CGI version: " +
	      S2DMain.PEP_CGI_VERSION + "\n");
            pctWriter.write("# Generation date: " +
	      S2DMain.getTimestamp() + "\n");
	    pctWriter.write("#\n");

	    assgTable = new AssgDataManager(chemAssgFile);
        } catch(Exception ex) {
	    System.err.println(
	      "Exception writing percent assignment values: " +
	      ex.toString());
	    throw new S2DError("Can't write percent assignment values");
	}

	int paCount = 0;

        try {
	    int index = 0;
	    while (index < _resSeqCodes.length) {
	        int resSeqCode = _resSeqCodes[index];
	        String resLabel = _residueLabels[index];

		try {
		    AssgDataManager.AssgEntry assignments =
		      assgTable.returnAssg(resLabel);

		    int starNumH = 0;
		    int starNumC = 0;
		    int starNumN = 0;

		    while (index < _resSeqCodes.length &&
		      resLabel.equalsIgnoreCase(_residueLabels[index]) &&
		      resSeqCode == _resSeqCodes[index]) {
	                String atomType = _atomTypes[index];
			if (atomType.equalsIgnoreCase(S2DNames.ATOM_H)) {
			    starNumH++;
			} else if (atomType.equalsIgnoreCase(
			  S2DNames.ATOM_C)) {
			    starNumC++;
			} else if (atomType.equalsIgnoreCase(
			  S2DNames.ATOM_N)) {
			    starNumN++;
			}

		        index++;
		    }

		    float pctH = 100 * (float)starNumH / assignments.numH;
		    float pctC = 100 * (float)starNumC / assignments.numC;
		    float pctN = 100 * (float)starNumN / assignments.numN;

		    if (checkPctAssign) {
                        if (pctH > 100.0 || pctC > 100.0 || pctN > 100.0) {
		    	    System.err.println("FATAL ERROR!: percent " +
			      "assignment greater than 100 for residue " +
			      resSeqCode);
			    System.exit(1);
		        }
		    }

		    pctWriter.write(_entityAssemblyID + " " +
		      resSeqCode + " " +
		      resLabel + " " +
		      pctH + " " +
		      pctC + " " +
		      pctN + "\n");
		    paCount++;
		} catch (S2DWarning ex) {
		    index++;
		    if (doDebugOutput(11)) {
		        System.err.println(ex.toString());
		    }
		} catch (S2DException ex) {
		    index++;
		    System.err.println(ex.toString());
		}
	    }

	    //
	    // Write the session file.
	    //
	    S2DSession.write(_sessionDir, S2DUtils.TYPE_PCT_ASSIGN,
	      _name, frameIndex, _info);

	    //
	    // Write the session-specific html file.
	    //
	    String title = "Percent Assigned Atoms (entity assembly " +
	      _entityAssemblyID + ")";
	    S2DSpecificHtml specHtml = new S2DSpecificHtml(
	      _summary.getHtmlDir(),
	      S2DUtils.TYPE_PCT_ASSIGN, _name, frameIndex,
	      title, _frameDetails);
	    specHtml.write();

	    //
	    // Write the link in the summary html file.
	    //
	    _summary.writePctAssign(frameIndex, paCount);

	} catch (IOException ex) {
	    System.err.println("IOException writing percent assignment data: " +
	      ex.toString());
	    throw new S2DError("Unable to write percent assignment data for " +
	      frameIndex);
	} finally {
	    try {
	        pctWriter.close();
	    } catch (IOException ex) {
	        System.err.println("IOException: " + ex.toString());
	    }
	}
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
	    asWriter.write("# Attributes: Entity_assembly_ID; " +
	      "Residue_seq_code; " +
	      "ResLabel; AtomName; AtomType; ChemShiftVal\n");
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
	        asWriter.write(_entityAssemblyID + " " +
		  _resSeqCodes[index] + " " +
		  _residueLabels[index] + " " +
		  _atomNames[index] + " " +
		  _atomTypes[index] + " " +
		  _chemShiftVals[index] + "\n");
	    }

	    //
	    // Write the session file.
	    //
	    S2DSession.write(_sessionDir, S2DUtils.TYPE_ALL_CHEM_SHIFTS,
	      _name, frameIndex, _info);

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
	    _summary.writeAllShifts(frameIndex, _resSeqCodes.length);

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

    //TEMP -- move to protein subclass
    //-------------------------------------------------------------------
    // Write H vs. N chem shifts for this data.
    // TEMP -- this could probably get restructured, too.
    public void writeHvsNShifts(int frameIndex) throws S2DException
    {
        if (doDebugOutput(11)) {
	    System.out.println("S2DChemShift.writeHvsNShifts()");
	}

        FileWriter hnWriter = null;
	try {
            hnWriter = S2DFileWriter.create(_dataDir + File.separator +
	      _name + S2DNames.HVSN_CHEM_SHIFT_SUFFIX + frameIndex +
	      S2DNames.DAT_SUFFIX);
	    hnWriter.write("# Data: H vs. N chemical shifts for " +
	      _name + "\n");
	    hnWriter.write("# Schema: bmrb-HvsN\n");
	    hnWriter.write("# Attributes: Entity_assembly_ID; " +
	      "Residue_seq_code; AcidName; Hshift; Nshift; Hatom; Natom\n");
            hnWriter.write("# Peptide-CGI version: " +
	      S2DMain.PEP_CGI_VERSION + "\n");
            hnWriter.write("# Generation date: " +
	      S2DMain.getTimestamp() + "\n");
	    hnWriter.write("#\n");
        } catch(IOException ex) {
	    System.err.println(
	      "IOException writing H vs. N chem shift values: " +
	      ex.toString());
	    throw new S2DError("Can't write H vs. N chem shift values");
	}

	//
	// Find the H and N chem shift values and write them out.
	//
	try {
	    HvsNInfo info = new HvsNInfo();
	    int hnCount = 0;
	    for (int index = 0; index < _resSeqCodes.length; index++) {
	        int currSeqCode = _resSeqCodes[index];
	        String currResLabel = _residueLabels[index];

	        if (currSeqCode != info.prevSeqCode) {

		    // We just finished the previous residue.
		    if (info.prevSeqCode != -1) {
		        if (writeHvsNLine(hnWriter, info)) {
		            hnCount++;
			}
		    }

		    info.prevSeqCode = currSeqCode;
		    info.prevResLabel = currResLabel;
		    info.hasH = false;
		    info.hasN = false;
		    info.hasHE1 = false;
		    info.hasNE1 = false;
                }

	        String atomName = _atomNames[index];
	        double chemShift = _chemShiftVals[index];

		if (atomName.equalsIgnoreCase(S2DNames.ATOM_H)) {
		    if (info.hasH) {
		        System.err.println("Multiple H entries in one " +
			  "residue(" + currSeqCode + ")!");
		    }
		    info.hasH = true;
		    info.hShift = chemShift;
		} else if (atomName.equalsIgnoreCase(S2DNames.ATOM_N)) {
		    if (info.hasN) {
		        System.err.println("Multiple N entries in one " +
			  "residue(" + currSeqCode + ")!");
		    }
		    info.hasN = true;
		    info.nShift = chemShift;
		} else if (_residueLabels[index].equals("TRP") &&
		  atomName.equalsIgnoreCase(S2DNames.ATOM_HE1)) {
		    if (info.hasHE1) {
		        System.err.println("Multiple HE1 entries in one " +
			  "residue(" + currSeqCode + ")!");
		    }
		    info.hasHE1 = true;
		    info.he1Shift = chemShift;
		} else if (_residueLabels[index].equals("TRP") &&
		  atomName.equalsIgnoreCase(S2DNames.ATOM_NE1)) {
		    if (info.hasNE1) {
		        System.err.println("Multiple NE1 entries in one " +
			  "residue(" + currSeqCode + ")!");
		    }
		    info.hasNE1 = true;
		    info.ne1Shift = chemShift;
	        }
            }

	    // Write out the last residue.
            if (info.prevSeqCode != -1) {
	        if (writeHvsNLine(hnWriter, info)) {
	            hnCount++;
	        }
	    }

	    if (hnCount > 0) {
	        //
	        // Write the session file.
	        //
	        S2DSession.write(_sessionDir, S2DUtils.TYPE_HVSN_CHEM_SHIFTS,
	          _name, frameIndex, _info);

	        //
	        // Write the session-specific html file.
	        //
		String title = "Simulated 1H-15N backbone HSQC " +
		  "spectrum (entity assembly " + _entityAssemblyID + ")";
	        S2DSpecificHtml specHtml = new S2DSpecificHtml(
	          _summary.getHtmlDir(),
		  S2DUtils.TYPE_HVSN_CHEM_SHIFTS, _name, frameIndex,
		  title, _frameDetails);
	        specHtml.write();

	        //
	        // Write the link in the summary html file.
	        //
	        _summary.writeHvsNShifts(frameIndex, hnCount);
	    }

	} catch (IOException ex) {
	    System.err.println("IOException writing all chem shifts: " +
	      ex.toString());
	    throw new S2DError("Unable to write all chem shifts for " +
	      frameIndex);
	} finally {
	    try {
	        hnWriter.close();
	    } catch (IOException ex) {
	        System.err.println("IOException: " + ex.toString());
	    }
	}
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
	String dataSource = _name + S2DNames.DELTASHIFT_SUFFIX +
	  frameIndex;

	String dataName;
	if (_atomSet.contains("HA")) {
	    dataName = "HA delta chem shift [" + frameIndex + "]";
	    dataSets.addElement(new S2DDatasetInfo(dataName,
	      dataSource, "HA_DeltaShift", "bmrb-DeltaShift", "DeltaShift",
	      _entityAssemblyID, S2DResidues.POLYMER_TYPE_PROTEIN));
        }

	if (_atomSet.contains("C")) {
	    dataName = "C delta chem shift [" + frameIndex + "]";
	    dataSets.addElement(new S2DDatasetInfo(dataName,
	      dataSource, "C_DeltaShift", "bmrb-DeltaShift", "DeltaShift",
	      _entityAssemblyID, S2DResidues.POLYMER_TYPE_PROTEIN));
        }

	if (_atomSet.contains("CA")) {
	    dataName = "CA delta chem shift [" + frameIndex + "]";
	    dataSets.addElement(new S2DDatasetInfo(dataName,
	      dataSource, "CA_DeltaShift", "bmrb-DeltaShift", "DeltaShift",
	      _entityAssemblyID, S2DResidues.POLYMER_TYPE_PROTEIN));
        }

	if (_hasRealCBShifts) {
	    dataName = "CB delta chem shift [" + frameIndex + "]";
	    dataSets.addElement(new S2DDatasetInfo(dataName,
	      dataSource, "CB_DeltaShift", "bmrb-DeltaShift",
	      "DeltaShift", _entityAssemblyID,
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
        // Note: attribute names must match the bmrb-Csi schema.
	String dataSource = _name + S2DNames.CSI_SUFFIX + frameIndex;

	String dataName;
	if (_atomSet.contains("HA")) {
	    dataName = "HA CSI [" + frameIndex + "]";
            dataSets.addElement(new S2DDatasetInfo(dataName, dataSource,
	      "HA_Csi", "bmrb-Csi", "Csi", _entityAssemblyID,
	      S2DResidues.POLYMER_TYPE_PROTEIN));
        }

	if (_atomSet.contains("C")) {
	    dataName = "C CSI [" + frameIndex + "]";
	    dataSets.addElement(new S2DDatasetInfo(dataName, dataSource,
	      "C_Csi", "bmrb-Csi", "Csi", _entityAssemblyID,
	      S2DResidues.POLYMER_TYPE_PROTEIN));
        }

	if (_atomSet.contains("CA")) {
	    dataName = "CA CSI [" + frameIndex + "]";
	    dataSets.addElement(new S2DDatasetInfo(dataName, dataSource,
	      "CA_Csi", "bmrb-Csi", "Csi", _entityAssemblyID,
	      S2DResidues.POLYMER_TYPE_PROTEIN));
        }

	if (_hasRealCBShifts) {
	    dataName = "CB CSI [" + frameIndex + "]";
	    dataSets.addElement(new S2DDatasetInfo(dataName, dataSource,
	      "CB_Csi", "bmrb-Csi", "Csi", _entityAssemblyID,
	      S2DResidues.POLYMER_TYPE_PROTEIN));
	}

	dataName = "Consensus CSI [" + frameIndex + ']';
	dataSets.addElement(new S2DDatasetInfo(dataName, dataSource,
	  "Consensus_Csi", "bmrb-Csi", "Csi", _entityAssemblyID,
	  S2DResidues.POLYMER_TYPE_PROTEIN));
    }

    //-------------------------------------------------------------------
    /**
     * Add percent assignment data sets to the data set list.
     * @param The data set list.
     * @param The frame index.
     */
    public void addPctAssignData(Vector dataSets, int frameIndex)
    {
        // Note: attribute names must match the bmrb-Percent schema.
	String dataSource = _name + S2DNames.PERCENT_ASSIGN_SUFFIX +
	  frameIndex;

	String dataName = "% 1H assign per res [" + frameIndex + "]";
	dataSets.addElement(new S2DDatasetInfo(dataName,
	  dataSource, "assigForH", "bmrb-Percent", "ChemShiftPercentage",
	  _entityAssemblyID, S2DResidues.POLYMER_TYPE_PROTEIN));

	dataName = "% 13C assign per res [" + frameIndex + "]";
	dataSets.addElement(new S2DDatasetInfo(dataName,
	  dataSource, "assigForC", "bmrb-Percent", "ChemShiftPercentage",
	  _entityAssemblyID, S2DResidues.POLYMER_TYPE_PROTEIN));

	dataName = "% 15N assign per res [" + frameIndex + "]";
	dataSets.addElement(new S2DDatasetInfo(dataName, 
	  dataSource, "assigForN", "bmrb-Percent", "ChemShiftPercentage",
	  _entityAssemblyID, S2DResidues.POLYMER_TYPE_PROTEIN));
    }

    //===================================================================
    // PROTECTED METHODS

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
		    ha2Ambiguities[currResSeqCode] = _ambiguityVals[index];

		} else if (atomName.equalsIgnoreCase(S2DNames.ATOM_HA3)) {
		    ha3DeltaShifts[currResSeqCode] = deltashift;
		    ha3Ambiguities[currResSeqCode] = _ambiguityVals[index];

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

    //===================================================================
    // PRIVATE METHODS

    //-------------------------------------------------------------------
    // Calculate the chemical shift index for the given values; returns
    // 0 if there is an error.
    private int calculateCSI(String resLabel, String atomName,
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

    //-------------------------------------------------------------------
    // Class to hold info for H vs N stuff for one residue.
    private class HvsNInfo {
        public int prevSeqCode = -1;
        public String prevResLabel = null;
        public boolean hasH = false;
        public boolean hasN = false;
        public double hShift = 0.0;
        public double nShift = 0.0;

        // Special case -- show side-chain HE1/NE1 for TRP.
	// 4267 residue 32 is an example of this.
        public boolean hasHE1 = false;
        public boolean hasNE1 = false;
        public double he1Shift = 0.0;
        public double ne1Shift = 0.0;
    }

    //-------------------------------------------------------------------
    /**
     * Print the H vs N line(s) for one residue.
     * @param hnWriter: the writer.
     * @param info: the H vs N info for this residue.
     * @return: true iff a line was written.
     */
    public boolean writeHvsNLine(FileWriter hnWriter, HvsNInfo info)
      throws IOException
    {
	boolean wroteLine = false;

	// Note: we can have H/N and HE1/NE1 for the same residue.
        if (info.hasH && info.hasN) {
	    hnWriter.write(_entityAssemblyID + " " +
	      info.prevSeqCode + " " +
	      info.prevResLabel + " " +
	      info.hShift + " " +
	      info.nShift + " H N \n");
	    wroteLine = true;
	}

	if (info.hasHE1 && info.hasNE1) {
	    hnWriter.write(_entityAssemblyID + " " +
	      info.prevSeqCode + " " +
	      info.prevResLabel + " " +
	      info.he1Shift + " " +
	      info.ne1Shift + " HE1 NE1\n");
	    wroteLine = true;
        }

	return wroteLine;
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

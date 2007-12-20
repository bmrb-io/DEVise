// ========================================================================
// DEVise Data Visualization Software
// (c) Copyright 2000-2007
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

    private String _name;
    private String _longName;
    private String _dataDir;
    private String _sessionDir;
    private S2DSummaryHtml _summary;

    private int[] _resSeqCodes;
    private String[] _residueLabels;
    private String[] _atomNames;
    private String[] _atomTypes;
    private double[] _chemShiftVals;
    private int[] _ambiguityVals;

    private final String CHEMSHIFT_FILE = "chem_info" + File.separator +
      "chemshift.txt";
    private ShiftDataManager _refTable;

    private String[] _deltaShiftResLabels;
    private float[] _haDeltaShifts;
    private float[] _cDeltaShifts;
    private float[] _caDeltaShifts;
    private float[] _cbDeltaShifts;

    // Distinguish between "real" CB shifts and the values plugged in
    // with the GLY HA3->CB translation.
    private boolean _hasRealCBShifts;

    private String _info;

    //===================================================================
    // PUBLIC METHODS

    //-------------------------------------------------------------------
    // Constructor.
    public S2DChemShift(String name, String longName, String dataDir,
      String sessionDir, S2DSummaryHtml summary, int[] resSeqCodes,
      String[] residueLabels, String[] atomNames, String[] atomTypes,
      double[] chemShiftVals, int[] ambiguityVals) throws S2DException
    {
        if (doDebugOutput(11)) {
	    System.out.println("S2DChemShift.S2DChemShift(" + name +
	      ")");
	}

	_name = name;
	_longName = longName;
	_dataDir = dataDir;
	_sessionDir = sessionDir;
	_summary = summary;

	_resSeqCodes = resSeqCodes;
	_residueLabels = S2DUtils.arrayToUpper(residueLabels);
	_atomNames = atomNames;
	_atomTypes = atomTypes;
	_chemShiftVals = chemShiftVals;
	_ambiguityVals = ambiguityVals;

	_refTable = new ShiftDataManager(CHEMSHIFT_FILE);

	_info = "Visualization of " + _longName;

	calculateDeltaShifts();
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
	    deltashiftWriter.write("# Attributes: Residue_seq_code; " +
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
		    deltashiftWriter.write(index + " " +
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
	    S2DSpecificHtml specHtml = new S2DSpecificHtml(
	      _summary.getHtmlDir(),
	      S2DUtils.TYPE_DELTASHIFT, _name, frameIndex,
	      "Chemical Shift Delta");
	    specHtml.write();

	    //
	    // Write the link in the summary html file.
	    //
	    _summary.writeDeltashift(frameIndex, dsCount);

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
	    csiWriter.write("# Attributes: Residue_seq_code; HA_Csi; " +
	      "C_Csi; CA_Csi; CB_Csi; Consensus_Csi\n");
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

		    csiWriter.write(index + " " + haCsi + " " +
		      cCsi + " " + caCsi + " " + cbCsi + " " + consCsi + "\n");
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
	    S2DSpecificHtml specHtml = new S2DSpecificHtml(
	      _summary.getHtmlDir(), S2DUtils.TYPE_CSI,
	      _name, frameIndex, "Chemical Shift Index");
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
    public void writePctAssign(int frameIndex) throws S2DException
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
	    pctWriter.write("# Attribute: Residue_seq_code; " +
	      "CurrResidueLabel; assigForH; assigForC; assigForN\n");
            pctWriter.write("# Peptide-CGI version: " +
	      S2DMain.PEP_CGI_VERSION + "\n");
            pctWriter.write("# Generation date: " +
	      S2DMain.getTimestamp() + "\n");
	    pctWriter.write("#\n");

	    assgTable = new AssgDataManager(S2DNames.CHEMASSG_FILE);
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

		    pctWriter.write(resSeqCode + " " + resLabel + " " +
		      (100 * (float)starNumH / assignments.numH) + " " +
		      (100 * (float)starNumC / assignments.numC) + " " +
		      (100 * (float)starNumN / assignments.numN) + " " + "\n");
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
	    S2DSpecificHtml specHtml = new S2DSpecificHtml(
	      _summary.getHtmlDir(),
	      S2DUtils.TYPE_PCT_ASSIGN, _name, frameIndex,
	      "Percent Assigned Atoms");
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
	    asWriter.write("# Attributes: Residue_seq_code; ResLabel; " +
	      "AtomName; AtomType; ChemShiftVal\n");
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
	        asWriter.write(_resSeqCodes[index] + " " +
		  _residueLabels[index] + " " + _atomNames[index] + " " +
		  _atomTypes[index] + " " +  _chemShiftVals[index] + "\n");
	    }

	    //
	    // Write the session file.
	    //
	    S2DSession.write(_sessionDir, S2DUtils.TYPE_ALL_CHEM_SHIFTS,
	      _name, frameIndex, _info);

	    //
	    // Write the session-specific html file.
	    //
	    S2DSpecificHtml specHtml = new S2DSpecificHtml(
	      _summary.getHtmlDir(),
	      S2DUtils.TYPE_ALL_CHEM_SHIFTS, _name, frameIndex,
	      "Chemical shift distributions by amino acid");
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
	    hnWriter.write("# Attributes: Residue_seq_code; AcidName; " +
	      " Hshift; Nshift\n");
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
	    int prevSeqCode = -1;
	    String prevResLabel = null;
	    boolean hasH = false;
	    boolean hasN = false;
	    double hShift = 0.0;
	    double nShift = 0.0;

	    // Special case -- show side-chain HE1/NE1 for TRP.
	    boolean hasHE1 = false;
	    boolean hasNE1 = false;
	    double he1Shift = 0.0;
	    double ne1Shift = 0.0;

	    int hnCount = 0;

	    for (int index = 0; index < _resSeqCodes.length; index++) {
	        int currSeqCode = _resSeqCodes[index];
	        String currResLabel = _residueLabels[index];

	        if (currSeqCode != prevSeqCode) {

		    // We just finished the previous residue.

		    if (prevSeqCode != -1 && hasH && hasN) {
			// We're done with prevSeqCode, so write out its
			// info.
		        hnWriter.write(prevSeqCode + " " + prevResLabel +
			  " " + hShift + " " + nShift + " H N \n");
		        hnCount++;
		    }
		    if (prevSeqCode != -1 && hasHE1 && hasNE1) {
			// We're done with prevSeqCode, so write out its
			// info.
		        hnWriter.write(prevSeqCode + " " + prevResLabel +
			  " " + he1Shift + " " + ne1Shift + " HE1 NE1\n");
		        hnCount++;
		    }

		    prevSeqCode = currSeqCode;
		    prevResLabel = currResLabel;
		    hasH = false;
		    hasN = false;
		    hasHE1 = false;
		    hasNE1 = false;
                }

	        String atomName = _atomNames[index];
	        double chemShift = _chemShiftVals[index];

		if (atomName.equalsIgnoreCase(S2DNames.ATOM_H)) {
		    if (hasH) {
		        System.err.println("Multiple H entries in one " +
			  "residue(" + currSeqCode + ")!");
		    }
		    hasH = true;
		    hShift = chemShift;
		} else if (atomName.equalsIgnoreCase(S2DNames.ATOM_N)) {
		    if (hasN) {
		        System.err.println("Multiple N entries in one " +
			  "residue(" + currSeqCode + ")!");
		    }
		    hasN = true;
		    nShift = chemShift;
		} else if (_residueLabels[index].equals("TRP") &&
		  atomName.equalsIgnoreCase(S2DNames.ATOM_HE1)) {
		    if (hasHE1) {
		        System.err.println("Multiple HE1 entries in one " +
			  "residue(" + currSeqCode + ")!");
		    }
		    hasHE1 = true;
		    he1Shift = chemShift;
		} else if (_residueLabels[index].equals("TRP") &&
		  atomName.equalsIgnoreCase(S2DNames.ATOM_NE1)) {
		    if (hasNE1) {
		        System.err.println("Multiple NE1 entries in one " +
			  "residue(" + currSeqCode + ")!");
		    }
		    hasNE1 = true;
		    ne1Shift = chemShift;
	        }
            }

	    // Write out the last residue.
	    if (prevSeqCode != -1 && hasH && hasN) {
	        hnWriter.write(prevSeqCode + " " + prevResLabel +
		  " " + hShift + " " + nShift + "\n");
	        hnCount++;
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
	        S2DSpecificHtml specHtml = new S2DSpecificHtml(
	          _summary.getHtmlDir(),
		  S2DUtils.TYPE_HVSN_CHEM_SHIFTS, _name, frameIndex,
		  "Simulated 1H-15N backbone HSQC spectrum");
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
	dataSets.addElement(new S2DDatasetInfo("HA delta chem shift",
	  dataSource, "HA_DeltaShift", "bmrb-DeltaShift", "DeltaShift"));
	dataSets.addElement(new S2DDatasetInfo("C delta chem shift",
	  dataSource, "C_DeltaShift", "bmrb-DeltaShift", "DeltaShift"));
	dataSets.addElement(new S2DDatasetInfo("CA delta chem shift",
	  dataSource, "CA_DeltaShift", "bmrb-DeltaShift", "DeltaShift"));
	if (_hasRealCBShifts) {
	    dataSets.addElement(new S2DDatasetInfo("CB delta chem shift",
	      dataSource, "CB_DeltaShift", "bmrb-DeltaShift", "DeltaShift"));
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
        dataSets.addElement(new S2DDatasetInfo("HA CSI", dataSource,
	  "HA_Csi", "bmrb-Csi", "Csi"));
	dataSets.addElement(new S2DDatasetInfo("C CSI", dataSource,
	  "C_Csi", "bmrb-Csi", "Csi"));
	dataSets.addElement(new S2DDatasetInfo("CA CSI", dataSource,
	  "CA_Csi", "bmrb-Csi", "Csi"));
	if (_hasRealCBShifts) {
	    dataSets.addElement(new S2DDatasetInfo("CB CSI", dataSource,
	      "CB_Csi", "bmrb-Csi", "Csi"));
	}
	dataSets.addElement(new S2DDatasetInfo("Consensus CSI", dataSource,
	  "Consensus_Csi", "bmrb-Csi", "Csi"));
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
	dataSets.addElement(new S2DDatasetInfo("% 1H assign per res",
	  dataSource, "assigForH", "bmrb-Percent", "ChemShiftPercentage"));
	dataSets.addElement(new S2DDatasetInfo("% 13C assign per res",
	  dataSource, "assigForC", "bmrb-Percent", "ChemShiftPercentage"));
	dataSets.addElement(new S2DDatasetInfo("% 15N assign per res",
	  dataSource, "assigForN", "bmrb-Percent", "ChemShiftPercentage"));
    }

    //===================================================================
    // PRIVATE METHODS

    //-------------------------------------------------------------------
    // Calculate all delta shifts for this data set.
    private void calculateDeltaShifts()
    {
        if (doDebugOutput(11)) {
	    System.out.println("S2DChemShift.calculateDeltaShifts()");
	}

	// We calculate lastResidue this way to allow for the data not
	// being strictly ordered by residue.
	int lastResidue = 0;
	for (int index = 0; index < _resSeqCodes.length; index++) {
	    lastResidue = Math.max(lastResidue, _resSeqCodes[index]);
	}

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

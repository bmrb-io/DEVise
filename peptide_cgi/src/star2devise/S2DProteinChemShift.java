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

// This class implements the calculation and output of chemical shift data
// for proteins.  For each set of chemical shift data, it creates a data
// file, a session file, an individual html file, and a link in the summary
// html file.

// ------------------------------------------------------------------------

// $Id$

// $Log$
// Revision 1.7  2010/02/17 18:48:41  wenger
// Fixed bug 093 (incorrect entity assembly IDs in 3D data sets).
//
// Revision 1.6  2009/11/11 20:32:41  wenger
// Added new tests for dealing properly with methyl groups in 1H-13C
// simulated spectra.  (Tests work right now with a kludged connections
// file that might break other stuff.)  Added warning to the 1H-13C
// processing for atoms that are not found in the connections list.
//
// Revision 1.5  2009/10/22 17:26:34  wenger
// Refactored the experimental chemical shifts to match the new-style
// Sparta processing where we get the appropriate data values in
// the appropriate chemical shift-related class(es) rather than in
// S2DMain.
//
// Revision 1.4  2009/10/20 16:54:10  wenger
// Created a new S2DSpartaChemShift class and cleaned up S2DChemShift
// class heirarchy in preparation for fixing things for the new SPARTA
// file format; various related cleanups (note that empty hn?.dat files
// are no longer generated for nucleic acids, resulting in changes to
// the test scripts).
//
// Revision 1.3  2009/08/25 18:15:57  wenger
// Merged s2d_sparta_deltashift_br_0 thru s2d_sparta_deltashift_br_3
// to trunk.
//
// Revision 1.2.4.3  2009/08/21 21:00:44  wenger
// Fixed errors that made some tests fail with the previous version.
//
// Revision 1.2.4.2  2009/07/15 17:36:31  wenger
// Added processing of N and HN deltashifts for SPARTA; added N and
// HN views to the session template (now split off from the "normal"
// deltashift template); partially added provision for multiple models.
//
// Revision 1.2.4.1  2009/07/01 18:06:00  wenger
// A lot of the SPARTA deltashift processing is in place -- the actual
// data isn't yet coming out right, though.
//
// Revision 1.2  2009/04/15 16:21:04  wenger
// Merged s2d_hc_spectrum_br_0 thru s2d_hc_spectrum_br_end to trunk;
// fixed test61 and test61_3.
//
// Revision 1.1.2.2  2009/04/14 22:09:07  wenger
// Session file, visualization-specific HTML file and summary page link
// are now created; removed "legend view" from session template;
// documented and cleaned up code.  (Still needs help for H vs C
// visualization.)
//
// Revision 1.1.2.1  2009/04/09 20:20:54  wenger
// HvsC simulated spectrum stuff is partly in place -- data is generated
// (but not fully tested, plus lots of temporary code still in place);
// schema and session template have been generated; processing does not
// yet generate the session file, specific HTML file, or the link in
// the summary HTML file.
//
// Revision 1.1  2009/01/29 22:05:26  wenger
// Made protein, DNA, and RNA subclasses of S2DChemShift to make further
// stuff easier; added some file checking to test64 and test65 (but
// delta shifts and CSI don't work yet for nucleic acids); committing
// again with nucleic acid stuff disabled.
//

// ========================================================================

package star2devise;

import EDU.bmrb.starlibj.SaveFrameNode;
import java.io.*;
import java.util.*;

public class S2DProteinChemShift extends S2DChemShift {
    //===================================================================
    // VARIABLES

    private static final int DEBUG = 0;

    //===================================================================
    // PUBLIC METHODS

    //-------------------------------------------------------------------
    // Constructor (for experimental chemical shifts).
    public S2DProteinChemShift(String name, String longName,
      S2DNmrStarIfc star, SaveFrameNode frame, String dataDir,
      String sessionDir, S2DSummaryHtml summary, String entityAssemblyID,
      S2DResidues residues) throws S2DException
    {
        super(name, longName, star, frame, dataDir, sessionDir, summary,
	  entityAssemblyID);

        if (doDebugOutput(11)) {
	    System.out.println("S2DProteinChemShift.S2DProteinChemShift(" +
	      name + ")");
	}

	getExperimentalValues(star, frame, entityAssemblyID, residues);

	CHEMSHIFT_FILE += "chemshift.txt";
        _refTable = new ShiftDataManager(CHEMSHIFT_FILE);

	calculateDeltaShifts();
    }

    //-------------------------------------------------------------------
    // Write the deltashifts for this data.
    public void writeDeltashifts(int frameIndex, boolean append)
      throws S2DException
    {
        if (doDebugOutput(11)) {
	    System.out.println("S2DProteinChemShift.writeDeltashifts()");
	}

	if (append) {
	    throw new S2DError("append should never be true for " +
	      "S2DProteinChemShift.writeDeltashifts()!");
	}

	//
	// Write the deltashift values to the appropriate data file.
	//
        FileWriter deltashiftWriter = null;
	try {
	    String fileName = _dataDir + File.separator + _name +
	      S2DNames.DELTASHIFT_SUFFIX + frameIndex + S2DNames.DAT_SUFFIX;
            deltashiftWriter = S2DFileWriter.create(fileName);

	    deltashiftWriter.write("# Data: delta shift values for " +
	      _name + "\n");
	    deltashiftWriter.write("# Schema: bmrb-DeltaShift\n");

	    deltashiftWriter.write("# Attributes: Entity_assembly_ID; ");
	    deltashiftWriter.write("Residue_seq_code; Residue_label; " +
	      "HA_DeltaShift; C_DeltaShift; CA_DeltaShift; CB_DeltaShift");
	    deltashiftWriter.write("\n");

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

	    int sessionType = S2DUtils.TYPE_DELTASHIFT;

	    //
	    // Write the session file
	    //
	    S2DSession.write(_sessionDir, sessionType, _name, frameIndex,
	      _info, null, _hasRealCBShifts, _starVersion);

	    //
	    // Write the session-specific html file.
	    //
	    String title = "Chemical Shift Delta (entity assembly " +
	      _entityAssemblyID + ")";
	    S2DSpecificHtml specHtml = new S2DSpecificHtml(
	      _summary.getHtmlDir(),
	      sessionType, _name, frameIndex,
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
	    System.out.println("S2DProteinChemShift.writeCSI()");
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
	    S2DSession.write(_sessionDir, S2DUtils.TYPE_CSI, _name,
	      frameIndex, _info, null, _hasRealCBShifts, _starVersion);

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
	    System.out.println("S2DProteinChemShift.writePctAssign()");
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
	      _name, frameIndex, _info, null, true, _starVersion);

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
    // Write H vs. N chem shifts for this data.
    // TEMP -- this could probably get restructured, too.
    public void writeHvsNShifts(int frameIndex) throws S2DException
    {
        if (doDebugOutput(11)) {
	    System.out.println("S2DProteinChemShift.writeHvsNShifts()");
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
	          _name, frameIndex, _info, null, true, _starVersion);

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
    // Write H vs. C chem shifts for this data.
    public void writeHvsCShifts(String connectionFile, int frameIndex)
      throws S2DException
    {
        if (doDebugOutput(11)) {
	    System.out.println("S2DProteinChemShift.writeHvsCShifts()");
	}

	S2DConnections connections = new S2DConnections(connectionFile);

        FileWriter hcWriter = null;
	try {
            hcWriter = S2DFileWriter.create(_dataDir + File.separator +
	      _name + S2DNames.HVSC_CHEM_SHIFT_SUFFIX + frameIndex +
	      S2DNames.DAT_SUFFIX);
	    hcWriter.write("# Data: H vs. C chemical shifts for " +
	      _name + "\n");
	    hcWriter.write("# Schema: bmrb-HvsC\n");
	    hcWriter.write("# Attributes: Entity_assembly_ID; " +
	      "Residue_seq_code; AcidName; Hshift; Cshift; Hatom; Catom\n");
            hcWriter.write("# Peptide-CGI version: " +
	      S2DMain.PEP_CGI_VERSION + "\n");
            hcWriter.write("# Generation date: " +
	      S2DMain.getTimestamp() + "\n");
	    hcWriter.write("#\n");

	    //
	    // Here we go through all of the available chemical shifts;
	    // for each residue we put all of the H* and C* values into
	    // a structure; when we get to the end of data for a given
	    // residue, we write out the appropriate data for that
	    // residue.
	    //
            HCResidueInfo info = null;
	    int hcCount = 0;
	    for (int index = 0; index < _resSeqCodes.length; index++) {
	        if (info == null ||
		  (_resSeqCodes[index] != info.residueSeqCode)) {
		    // New residue
	            if (info != null) {
	                hcCount += writeHCResidueInfo(hcWriter, info);
	            }
		    info = new HCResidueInfo();
		    info.residueSeqCode = _resSeqCodes[index];
		    info.residueName = _residueLabels[index];
		    info.bonds = connections.getBonds(info.residueName);
		}

		if (_atomTypes[index].equals("H")) {
		    info.hChemshifts.put(_atomNames[index],
		      new Double(_chemShiftVals[index]));
		}
		if (_atomTypes[index].equals("C")) {
		    info.cChemshifts.put(_atomNames[index],
		      new Double(_chemShiftVals[index]));
		}
	    }
	    if (info != null) {
		// Make sure we write the last residue!
	        hcCount += writeHCResidueInfo(hcWriter, info);
	    }

	    hcWriter.close();

	    if (hcCount > 0) {
	        //
	        // Write the session file.
	        //
	        S2DSession.write(_sessionDir, S2DUtils.TYPE_HVSC_CHEM_SHIFTS,
	          _name, frameIndex, _info, null, true, _starVersion);

	        //
	        // Write the session-specific html file.
	        //
		String title = "Simulated 1H-13C HSQC " +
		  "spectrum (entity assembly " + _entityAssemblyID + ")";
	        S2DSpecificHtml specHtml = new S2DSpecificHtml(
	          _summary.getHtmlDir(),
		  S2DUtils.TYPE_HVSC_CHEM_SHIFTS, _name, frameIndex,
		  title, _frameDetails);
	        specHtml.write();

	        //
	        // Write the link in the summary html file.
	        //
	        _summary.writeHvsCShifts(frameIndex, hcCount);
	    }
        } catch(IOException ex) {
	    System.err.println(
	      "IOException writing H vs. c chem shift values: " +
	      ex.toString());
	    throw new S2DError("Can't write H vs. c chem shift values");
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
	    dataName = "HA CSI [" + _entityAssemblyID + "]";
            dataSets.addElement(new S2DDatasetInfo(dataName, dataSource,
	      "HA_Csi", "bmrb-Csi", "Csi", _entityAssemblyID,
	      S2DResidues.POLYMER_TYPE_PROTEIN));
        }

	if (_atomSet.contains("C")) {
	    dataName = "C CSI [" + _entityAssemblyID + "]";
	    dataSets.addElement(new S2DDatasetInfo(dataName, dataSource,
	      "C_Csi", "bmrb-Csi", "Csi", _entityAssemblyID,
	      S2DResidues.POLYMER_TYPE_PROTEIN));
        }

	if (_atomSet.contains("CA")) {
	    dataName = "CA CSI [" + _entityAssemblyID + "]";
	    dataSets.addElement(new S2DDatasetInfo(dataName, dataSource,
	      "CA_Csi", "bmrb-Csi", "Csi", _entityAssemblyID,
	      S2DResidues.POLYMER_TYPE_PROTEIN));
        }

	if (_hasRealCBShifts) {
	    dataName = "CB CSI [" + _entityAssemblyID + "]";
	    dataSets.addElement(new S2DDatasetInfo(dataName, dataSource,
	      "CB_Csi", "bmrb-Csi", "Csi", _entityAssemblyID,
	      S2DResidues.POLYMER_TYPE_PROTEIN));
	}

	dataName = "Consensus CSI [" + _entityAssemblyID + ']';
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

	String dataName = "% 1H assign per res [" + _entityAssemblyID + "]";
	dataSets.addElement(new S2DDatasetInfo(dataName,
	  dataSource, "assigForH", "bmrb-Percent", "ChemShiftPercentage",
	  _entityAssemblyID, S2DResidues.POLYMER_TYPE_PROTEIN));

	dataName = "% 13C assign per res [" + _entityAssemblyID + "]";
	dataSets.addElement(new S2DDatasetInfo(dataName,
	  dataSource, "assigForC", "bmrb-Percent", "ChemShiftPercentage",
	  _entityAssemblyID, S2DResidues.POLYMER_TYPE_PROTEIN));

	dataName = "% 15N assign per res [" + _entityAssemblyID + "]";
	dataSets.addElement(new S2DDatasetInfo(dataName, 
	  dataSource, "assigForN", "bmrb-Percent", "ChemShiftPercentage",
	  _entityAssemblyID, S2DResidues.POLYMER_TYPE_PROTEIN));
    }

    //===================================================================
    // PRIVATE METHODS

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
    // This class holds all info for a given residue related to
    // H vs C peaks.
    private class HCResidueInfo {
	int residueSeqCode;
        String residueName;
	Vector bonds; // S2DConnections.Bond objects

	// String (atom name) -> Double (chemical shift)
	Hashtable hChemshifts = new Hashtable();

	// String (atom name) -> Double (chemical shift)
	Hashtable cChemshifts = new Hashtable();
    };

    //-------------------------------------------------------------------
    /**
     * Write out the the H vs C peaks for one residue.
     * @param The Writer to write to
     * @param The info for the given residue
     * @return The number of peaks written
     */
    private int writeHCResidueInfo(Writer writer, HCResidueInfo info)
      throws IOException
    {
	//
	// We go through the list of hydrogens for this residue, 
	// and find atoms that are bonded to the hydrogen (we index on
	// the hydrogen list because only one atom will be bonded to a
	// hydrogen, so we automatically eliminate the possibility of
	// duplicates).
	int peakCount = 0;
	Enumeration hKeys = info.hChemshifts.keys();
	while (hKeys.hasMoreElements()) {
	    String hAtomName = (String)hKeys.nextElement();
	        // At this point, cAtomName may actually be something
		// other than a carbon, but non-carbons will be eliminated
		// because they won't be found in the info.cChemshifts
		// list.
	    String cAtomName = findBondedAtom(hAtomName, info.bonds);
            if (cAtomName != null) {
	        Double hChemshift = (Double) info.hChemshifts.get(hAtomName);
	        Double cChemshift = (Double) info.cChemshifts.get(cAtomName);
		if (hChemshift != null && cChemshift != null) {
                    writer.write(_entityAssemblyID + " " +
		      info.residueSeqCode + " " + info.residueName + " " +
		      hChemshift + " " + cChemshift + " " + hAtomName + " " +
		      cAtomName + "\n");
		    peakCount++;
		}
	    } else {
		S2DWarning warning = new S2DWarning("atom " +
		  hAtomName + " not found in bonds list for residue " +
		  info.residueSeqCode + "/" + info.residueName);
	        if (doDebugOutput(1)) {
	            System.err.println(warning);
		}
	    }
	}

	return peakCount;
    }

    //-------------------------------------------------------------------
    /**
     * Find an atom (if any) bonded to the given atom.  Note that this
     * method only ever finds one bonded atom, so the given atom should
     * be a hydrogen.
     * @param The atom for which to find a bonded atom
     * @param The list of bonds for the relevant amino acid
     * @return The name of a bonded atom (or null, if none is found)
     */
    String findBondedAtom(String atomName, Vector bonds)
    {
        String result = null;

	for (int index = 0; index < bonds.size(); index++) {
	    S2DConnections.Bond bond =
	      (S2DConnections.Bond)bonds.elementAt(index);
	    if (bond._atom1.equals(atomName)) {
	        result = bond._atom2;
		break;
	    } else if (bond._atom2.equals(atomName)) {
	        result = bond._atom1;
		break;
	    }
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

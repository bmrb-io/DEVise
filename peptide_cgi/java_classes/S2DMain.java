// ========================================================================
// DEVise Data Visualization Software
// (c) Copyright 2000-2001
// By the DEVise Development Group
// Madison, Wisconsin
// All Rights Reserved.
// ========================================================================

// Under no circumstances is this software to be copied, distributed,
// or altered in any way without prior permission from the DEVise
// Development Group.

// ------------------------------------------------------------------------

// This class reads an NMR-Star file and generates data and session files
// to be used by DEVise, and a summary html page and individual html pages
// for each session.

// ------------------------------------------------------------------------

// $Id$

// $Log$
// Revision 1.10  2001/04/17 17:09:09  wenger
// Added display of H vs. N chem shifts.
//
// Revision 1.9  2001/04/16 19:49:11  wenger
// Added display of all chem shifts by amino acid.
//
// Revision 1.8  2001/03/13 14:50:10  wenger
// Added cache invalidation: cache files are not used if peptide-cgi code
// or NMR-Star file has changed since relevant cache files were generated.
//
// Revision 1.7  2001/03/08 21:10:34  wenger
// Merged changes from no_collab_br_2 thru no_collab_br_3 from the branch
// to the trunk.
//
// Revision 1.6  2001/03/08 20:33:24  wenger
// Merged changes from no_collab_br_0 thru no_collab_br_2 from the branch
// to the trunk.
//
// Revision 1.5.2.2  2001/02/22 19:52:54  wenger
// Made most coupling constant values optional; added (optional) coupling
// constant error values.
//
// Revision 1.5.2.1  2001/02/09 16:57:12  wenger
// Added heteronuclear NOE; made T1 and T2 relaxation errors optional
// (conversion doesn't fail if they are not found); added an X margin of
// 0.5 in all sessions; updated star file list; misc. minor cleanups.
//
// Revision 1.5  2001/01/30 17:44:47  wenger
// Added path for set_modes.
//
// Revision 1.4  2001/01/25 16:37:46  wenger
// Fixed a bug that could cause an infinite loop in the perecent assignment
// calculations; put filesize, cpu, and coredump limits in s2d script;
// updated star file list; got rid of some unnecessary warnings.
//
// Revision 1.3  2001/01/23 19:35:19  wenger
// Made a few minor fixes to get things to work right at BMRB.
//
// Revision 1.2  2001/01/19 15:39:06  wenger
// Added T1 and T2 relaxation; removed some unnecessary variables from
// coupling constants; added schema files to installation, unified T1
// and T2 relaxation schema.
//
// Revision 1.1  2001/01/17 20:00:07  wenger
// Restructured the peptide-cgi code to make it much more maintainable.
//

// ========================================================================

import EDU.bmrb.starlibj.SaveFrameNode;
import java.util.*;
import java.io.*;
import java.text.*;

// for chemical shifts

public class S2DMain {
    //===================================================================
    // VARIABLES

    private static final int DEBUG = 0;

    public static final String PEP_CGI_VERSION = "2.4";

    private int _accessionNum;
    private String _dataDir;
    private String _sessionDir;
    
    private boolean _force = false;

    private S2DSummaryHtml _summary;

    //===================================================================
    // PUBLIC METHODS

    //-------------------------------------------------------------------
    public static void main(String args[]) throws S2DException
    {
        S2DMain s2d = new S2DMain(args);
	try {
	    if (!s2d.useCache()) {
	        s2d.process();
	    } else {
	        if (DEBUG >= 1) {
		    System.out.println("Cache files used");
		}
	    }
	} catch (S2DWarning ex) {
	    System.err.println(ex.getMessage());
	} catch (Exception ex) {
	    ex.printStackTrace();
	    throw new S2DError("NMR-Star to DEVise conversion failed");
	}
    }

    //-------------------------------------------------------------------
    // Constructor.
    public S2DMain(String args[]) throws S2DException
    {
        if (DEBUG >= 1) {
	    String sep = "";
	    System.out.print("S2DMain.S2DMain(");
	    for (int index = 0; index < args.length; index++) {
	        System.out.print(sep + args[index]);
		sep = ", ";
	    }
	    System.out.print(")\n");
	}

	checkArgs(args);
    }

    //===================================================================
    // PRIVATE METHODS

    //-------------------------------------------------------------------
    // Check arguments to constructor, set member variables accordingly.
    private void checkArgs(String args[]) throws S2DException
    {
	String usage = "Usage: java S2DMain [-force] " +
	  "<accession number> <data directory> <session directory>";
        if (args.length < 3 || args.length > 4) {
	    System.out.println(usage);
	    throw new S2DError("Illegal arguments");
	} else {
	    int index = 0;
	    if (args.length == 4) {
		if (args[index].equals("-force")) {
		    _force = true;
		} else {
	            System.out.println(usage);
	            throw new S2DError("Illegal arguments");
		}
	        index++;
	    }
	    try {
	        _accessionNum = Integer.parseInt(args[index]);
	        index++;
	        _dataDir = args[index];
	        index++;
	        _sessionDir = args[index];
	        index++;
	    } catch(NumberFormatException ex) {
	        System.err.println("Error parsing arguments: " +
		  ex.getMessage());
	        throw new S2DError("Error parsing " + ex.getMessage());
	    }
	}
    }

    //-------------------------------------------------------------------
    // Decide whether to use cached versions of the relevant output files.
    private boolean useCache()
    {
        boolean result = false;

	check:
	if (!_force) {
	    FileReader freader;

	    //
	    // Find out whether the summary html file exists.
	    //
	    try {
	        freader = new FileReader(S2DSummaryHtml.fileName(_dataDir,
		  _accessionNum));
	    } catch (FileNotFoundException ex) {
		if (DEBUG >= 1) {
		    System.out.println("Summary html file " +
		      ex.getMessage() + " not found; cache not used");
		}
	        break check;
	    }

	    //
	    // Get summary html file version and date.
	    //
	    String fileVersion = null;
	    Date fileDate = null;
	    BufferedReader breader = new BufferedReader(freader);
	    try {
	        String line;
	        while ((line = breader.readLine()) != null) {
		    int index;
		    if ((index = line.indexOf(S2DSummaryHtml.VERSION_LABEL))
		      != -1) {

		        int index1 = line.indexOf("{", index);
		        int index2 = line.indexOf("}", index1);
			if (index1 != -1 && index2 != -1) {
			    fileVersion = line.substring(index1 + 1, index2);
			}
		    }

		    if ((index = line.indexOf(S2DSummaryHtml.GEN_DATE_LABEL))
		      != -1) {
		        int index1 = line.indexOf("{", index);
		        int index2 = line.indexOf("}", index1);
			if (index1 != -1 && index2 != -1) {
			    String dateStr = line.substring(index1 + 1, index2);
			    fileDate = DateFormat.getDateInstance().
			      parse(dateStr);
			}
		    }
	        }
	    } catch (ParseException ex) {
		System.err.println("ParseException: " + ex.getMessage());
		break check;
	    } catch (IOException ex) {
		System.err.println("IOException: " + ex.getMessage());
		break check;
	    } finally {
		try {
	            breader.close();
		} catch (IOException ex) {
		    System.err.println("IOException: " + ex.getMessage());
		    break check;
		}
	    }

	    //
	    // Compare summary file version to current peptide-cgi software
	    // version.
	    //
	    if (!PEP_CGI_VERSION.equals(fileVersion)) {
		if (DEBUG >= 1) {
		    System.out.println("Existing summary html file version (" +
		      fileVersion + ") does not " +
		      "match current software version (" + PEP_CGI_VERSION +
		      "); cache not used");
		}
	        break check;
	    }

	    //
	    // Compare the generation date of the summary html file to the
	    // modification date for the NMR-Star file.
	    //
	    Date starModDate = S2DStarIfc.getModDate(_accessionNum);
	    if (starModDate == null || starModDate.after(fileDate)) {
		if (DEBUG >= 1) {
		    System.out.println("Existing summary html file is " +
		      "older than NMR-Star file; cache not used");
		}
	        break check;
	    }

	    result = true;
	}

	return result;
    }

    //-------------------------------------------------------------------
    // Do the processing.
    private void process() throws S2DException
    {
        S2DStarIfc star = new S2DStarIfc(_accessionNum);

	_summary = new S2DSummaryHtml(_accessionNum, _dataDir,
	  star.getFileName(), star.getSystemName(),
	  star.getEntryTitle());

	//TEMP -- do I really want to skip stuff if I get an error?
	saveChemShifts(star);
	saveT1Relaxation(star);
	saveT2Relaxation(star);
	saveHetNOE(star);
	saveHExchangeRate(star);
	saveCoupling(star);
	saveHExchangeProtFact(star);
	saveS2Params(star);

	_summary.close();
	_summary = null;

	//
	// Make sure that all of the files that we generated are world-
	// readable.
	//
	try {
	    Runtime currentRT = Runtime.getRuntime();
	    Process ps = currentRT.exec("./set_modes " + _accessionNum);
	    ps.waitFor();
	    int result = ps.exitValue();
	    if (result != 0) {
	        throw new S2DWarning("Non-zero exit value from set_modes; " +
		  "see set_modes.out");
	    }
	} catch (Exception ex) {
	    System.err.println("Error setting file permissions: " +
	      ex.getMessage());
	}
    }

    //-------------------------------------------------------------------
    // Save all chem shift data for this entry.
    // Note: this can be tested with 4264.
    private void saveChemShifts(S2DStarIfc star) throws S2DException
    {
        if (DEBUG >= 2) {
	    System.out.println("  S2DMain.saveChemShifts()");
	}

	Enumeration frameList = star.getDataFramesByCat(
	  S2DNames.ASSIGNED_CHEM_SHIFTS);

	int frameIndex = 1;
        while (frameList.hasMoreElements()) {
	    SaveFrameNode frame = (SaveFrameNode)frameList.nextElement();

	    // We only want to output chemical shifts if this save frame
	    // has data for a protein.  (This can be tested with bmr4038.str
	    // and bmr4056.str.)
	    if (!star.isAProtein(frame)) {
                if (DEBUG >= 1) {
                    System.out.println("Chemical shifts not saved for " +
                      "save frame " + frame.getLabel() + " because it is " +
                      "not a protein");
                }
	    } else {
		try {
	            saveFrameChemShifts(star, frame, frameIndex);
		} catch(S2DException ex) {
		    System.err.println("Exception saving chem shifts for " +
		      "frame " + star.getFrameName(frame) + ": " +
		      ex.getMessage());
		}
		frameIndex++;
	    }
	}
    }

    //-------------------------------------------------------------------
    // Save all T1 relaxation data for this entry.
    // Note: this can be tested with 4267.
    private void saveT1Relaxation(S2DStarIfc star) throws S2DException
    {
        if (DEBUG >= 2) {
	    System.out.println("  S2DMain.saveT1Relaxation()");
	}

	Enumeration frameList = star.getDataFramesByCat(S2DNames.T1_RELAX);

	int frameIndex = 1;
        while (frameList.hasMoreElements()) {
	    SaveFrameNode frame = (SaveFrameNode)frameList.nextElement();
	    try {
	        saveFrameT1Relax(star, frame, frameIndex);
	    } catch(S2DException ex) {
		System.err.println("Exception saving T1 relaxation for " +
		  "frame " + star.getFrameName(frame) + ": " +
		  ex.getMessage());
	    }
	    frameIndex++;
        }
    }

    //-------------------------------------------------------------------
    // Save all T2 relaxation data for this entry.
    // Note: this can be tested with 4267.
    private void saveT2Relaxation(S2DStarIfc star) throws S2DException
    {
        if (DEBUG >= 2) {
	    System.out.println("  S2DMain.saveT2Relaxation()");
	}

	Enumeration frameList = star.getDataFramesByCat(S2DNames.T2_RELAX);

	int frameIndex = 1;
        while (frameList.hasMoreElements()) {
	    SaveFrameNode frame = (SaveFrameNode)frameList.nextElement();
	    try {
	        saveFrameT2Relax(star, frame, frameIndex);
	    } catch(S2DException ex) {
		System.err.println("Exception saving T2 relaxation for " +
		  "frame " + star.getFrameName(frame) + ": " +
		  ex.getMessage());
	    }
	    frameIndex++;
        }
    }

    //-------------------------------------------------------------------
    // Save all heteronuclear NOE values for this entry.
    // Note: this can be tested with 4267.
    private void saveHetNOE(S2DStarIfc star) throws S2DException
    {
        if (DEBUG >= 2) {
	    System.out.println("  S2DMain.saveHetNOE()");
	}

	// add real code here
	Enumeration frameList = star.getDataFramesByCat(
	  S2DNames.HETERONUCLEAR_NOE);

	int frameIndex = 1;
        while (frameList.hasMoreElements()) {
	    SaveFrameNode frame = (SaveFrameNode)frameList.nextElement();
	    try {
	        saveFrameHetNOE(star, frame, frameIndex);
	    } catch(S2DException ex) {
		System.err.println("Exception saving heteronuclear NOE for " +
		  "frame " + star.getFrameName(frame) + ": " +
		  ex.getMessage());
	    }
	    frameIndex++;
        }
    }

    //-------------------------------------------------------------------
    private void saveHExchangeRate(S2DStarIfc star)
    {
        if (DEBUG >= 2) {
	    System.out.println("  S2DMain.saveHExchangeRate()");
	}

	// add real code here
    }

    //-------------------------------------------------------------------
    // Save all coupling constants for this entry.
    // Note: this can be tested with 4297.
    private void saveCoupling(S2DStarIfc star) throws S2DException
    {
        if (DEBUG >= 2) {
	    System.out.println("  S2DMain.saveCoupling()");
	}

	Enumeration frameList = star.getDataFramesByCat(
	  S2DNames.COUPLING_CONSTANTS);

	int frameIndex = 1;
        while (frameList.hasMoreElements()) {
	    SaveFrameNode frame = (SaveFrameNode)frameList.nextElement();
	    try {
	        saveFrameCoupling(star, frame, frameIndex);
	    } catch(S2DException ex) {
		System.err.println("Exception saving coupling constants for " +
		  "frame " + star.getFrameName(frame) + ": " +
		  ex.getMessage());
	    }
	    frameIndex++;
        }
    }

    //-------------------------------------------------------------------
    private void saveHExchangeProtFact(S2DStarIfc star)
    {
        if (DEBUG >= 2) {
	    System.out.println("  S2DMain.saveHExchangeProtFact()");
	}

	// add real code here
    }

    //-------------------------------------------------------------------
    private void saveS2Params(S2DStarIfc star)
    {
        if (DEBUG >= 2) {
	    System.out.println("  S2DMain.saveS2Params()");
	}

	// add real code here
    }

    //-------------------------------------------------------------------
    // Save chem shifts for one save frame.
    private void saveFrameChemShifts(S2DStarIfc star, SaveFrameNode frame,
      int frameIndex) throws S2DException
    {
        if (DEBUG >= 3) {
	    System.out.println("    S2DMain.saveFrameChemShifts(" +
	      star.getFrameName(frame) + ", " + frameIndex + ")");
	}

	//
	// Get the values we need from the Star file.
	//
	String[] resSeqCodesTmp = star.getFrameValues(frame,
	  S2DNames.CHEM_SHIFT_VALUE, S2DNames.RESIDUE_SEQ_CODE);
	int[] resSeqCodes = S2DUtils.arrayStr2Int(resSeqCodesTmp);
	resSeqCodesTmp = null;

	String[] residueLabels = star.getFrameValues(frame,
	  S2DNames.CHEM_SHIFT_VALUE, S2DNames.RESIDUE_LABEL);

	String[] atomNames = star.getFrameValues(frame,
	  S2DNames.CHEM_SHIFT_VALUE, S2DNames.ATOM_NAME);

	String[] atomTypes = star.getFrameValues(frame,
	  S2DNames.CHEM_SHIFT_VALUE, S2DNames.ATOM_TYPE);

	String[] chemShiftsTmp = star.getFrameValues(frame,
	  S2DNames.CHEM_SHIFT_VALUE, S2DNames.CHEM_SHIFT_VALUE);
        double[] chemShiftVals = S2DUtils.arrayStr2Double(chemShiftsTmp);
	chemShiftsTmp = null;


	//
	// Create an S2DChemShift object with the values we just got.
	//
        S2DChemShift chemShift = new S2DChemShift(_accessionNum, _dataDir,
	  _sessionDir, _summary, resSeqCodes, residueLabels, atomNames,
	  atomTypes, chemShiftVals);


	//
	// Now go ahead and calculate and write out the chem shift values.
	//
	_summary.startFrame(star.getFrameDetails(frame));

	try {
	    chemShift.writeDeltashifts(frameIndex);
	} catch (S2DException ex) {
	    // Don't throw a new exception here because we want to write as
	    // much as we can, even if there's an error somewhere along the
	    // line.
	    System.err.println(ex.getMessage());
	}

	// We only want to output CSI values if this save frame
	// has HA chem shifts for at least 80% of the residues in
	// the entry.  (This can be tested with bmr4001.str and
	// bmr4011.str.)
	if (star.residueCount() < 0 ||
	  ((float)star.getHAChemShiftCount(frame) /
	  (float)star.residueCount() >= 0.8)) {
	    try {
	        chemShift.writeCSI(frameIndex);
	    } catch (S2DException ex) {
	        // Don't throw a new exception here because we want to write as
	        // much as we can, even if there's an error somewhere along the
	        // line.
	        System.err.println(ex.getMessage());
	    }
	} else {
	    if (DEBUG >= 1) {
	        System.out.println("CSI values not saved for " +
		  "save frame " + frame.getLabel() + " because HA " +
		    "chem shifts are not available for 80% of the residues");
            }
	}

	try {
	    chemShift.writePctAssign(frameIndex);
	} catch (S2DException ex) {
	    // Don't throw a new exception here because we want to write as
	    // much as we can, even if there's an error somewhere along the
	    // line.
	    System.err.println(ex.getMessage());
	}

	try {
	    chemShift.writeAllShifts(frameIndex);
	} catch (S2DException ex) {
	    // Don't throw a new exception here because we want to write as
	    // much as we can, even if there's an error somewhere along the
	    // line.
	    System.err.println(ex.getMessage());
	}

	try {
	    chemShift.writeHvsNShifts(frameIndex);
	} catch (S2DException ex) {
	    // Don't throw a new exception here because we want to write as
	    // much as we can, even if there's an error somewhere along the
	    // line.
	    System.err.println(ex.getMessage());
	}

	_summary.endFrame();
    }

    //-------------------------------------------------------------------
    // Save T1 relaxation values for one save frame.
    private void saveFrameT1Relax(S2DStarIfc star, SaveFrameNode frame,
      int frameIndex) throws S2DException
    {
        if (DEBUG >= 3) {
	    System.out.println("    S2DMain.saveFrameT1Relax(" +
	      star.getFrameName(frame) + ", " + frameIndex + ")");
	}

	//TEMP -- change strings to named constants
	String[] resSeqCodes = star.getFrameValues(frame,
	  "_T1_value", "_Residue_seq_code");

	String[] resLabels = star.getFrameValues(frame,
	  "_T1_value", "_Residue_label");

	String[] atomNames = star.getFrameValues(frame,
	  "_T1_value", "_Atom_name");

	String[] relaxValues = star.getFrameValues(frame,
	  "_T1_value", "_T1_value");

	//TEMP -- 4096 has "_T1_error" instead of "_T1_value_error".
	String[] relaxErrors = star.getOptionalFrameValues(frame,
	  "_T1_value", "_T1_value_error", relaxValues.length, "0");

	_summary.startFrame(star.getFrameDetails(frame));

	S2DRelaxation relaxation = new S2DRelaxation(_accessionNum, _dataDir,
	  _sessionDir, _summary, S2DUtils.TYPE_T1_RELAX,
	  star.getOneFrameValue(frame, "_Spectrometer_frequency_1H"),
	  resSeqCodes, resLabels, atomNames, relaxValues, relaxErrors);

	relaxation.writeRelaxation(frameIndex);

	_summary.endFrame();
    }

    //-------------------------------------------------------------------
    // Save T2 relaxation values for one save frame.
    private void saveFrameT2Relax(S2DStarIfc star, SaveFrameNode frame,
      int frameIndex) throws S2DException
    {
        if (DEBUG >= 3) {
	    System.out.println("    S2DMain.saveFrameT2Relax(" +
	      star.getFrameName(frame) + ", " + frameIndex + ")");
	}

	//TEMP -- change strings to named constants
	String[] resSeqCodes = star.getFrameValues(frame,
	  "_T2_value", "_Residue_seq_code");

	String[] resLabels = star.getFrameValues(frame,
	  "_T2_value", "_Residue_label");

	String[] atomNames = star.getFrameValues(frame,
	  "_T2_value", "_Atom_name");

	String[] relaxValues = star.getFrameValues(frame,
	  "_T2_value", "_T2_value");

	//TEMP -- 4096 has "_T2_error" instead of "_T2_value_error".
	String[] relaxErrors = star.getOptionalFrameValues(frame,
	  "_T2_value", "_T2_value_error", relaxValues.length, "0");

	_summary.startFrame(star.getFrameDetails(frame));

	S2DRelaxation relaxation = new S2DRelaxation(_accessionNum, _dataDir,
	  _sessionDir, _summary, S2DUtils.TYPE_T2_RELAX,
	  star.getOneFrameValue(frame, "_Spectrometer_frequency_1H"),
	  resSeqCodes, resLabels, atomNames, relaxValues, relaxErrors);

	relaxation.writeRelaxation(frameIndex);

	_summary.endFrame();
    }

    //-------------------------------------------------------------------
    // Save coupling constants for one save frame.
    private void saveFrameCoupling(S2DStarIfc star, SaveFrameNode frame,
      int frameIndex) throws S2DException
    {
        if (DEBUG >= 3) {
	    System.out.println("    S2DMain.saveFrameCoupling(" +
	      star.getFrameName(frame) + ", " + frameIndex + ")");
	}

	//
	// Get the values we need from the Star file.
	//
	//TEMP -- change strings to named constants
	String[] atom1ResSeqs = star.getFrameValues(frame,
	  "_Coupling_constant_value", "_Atom_one_residue_seq_code");

	String[] atom2ResSeqs = star.getFrameValues(frame,
	  "_Coupling_constant_value", "_Atom_two_residue_seq_code");

	String[] couplingConstValues = star.getFrameValues(frame,
	  "_Coupling_constant_value", "_Coupling_constant_value");

	String[] couplingConstCodes = star.getOptionalFrameValues(frame,
	  "_Coupling_constant_value", "_Coupling_constant_code",
	  atom1ResSeqs.length, "0");

	String[] atom1ResLabels = star.getOptionalFrameValues(frame,
	  "_Coupling_constant_value", "_Atom_one_residue_label",
	  atom1ResSeqs.length, "0");

	String[] atom1Names = star.getOptionalFrameValues(frame,
	  "_Coupling_constant_value", "_Atom_one_name",
	  atom1ResSeqs.length, "0");

	String[] atom2ResLabels = star.getOptionalFrameValues(frame,
	  "_Coupling_constant_value", "_Atom_two_residue_label",
	  atom1ResSeqs.length, "0");

	String[] atom2Names = star.getOptionalFrameValues(frame,
	  "_Coupling_constant_value", "_Atom_two_name",
	  atom1ResSeqs.length, "0");

	String[] couplingConstErrors = star.getOptionalFrameValues(frame,
	  "_Coupling_constant_value", "_Coupling_constant_value_error",
	  atom1ResSeqs.length, "0");

	_summary.startFrame(star.getFrameDetails(frame));

        S2DCoupling coupling = new S2DCoupling(_accessionNum, _dataDir,
	  _sessionDir, _summary, couplingConstCodes, atom1ResSeqs,
	  atom1ResLabels, atom1Names, atom2ResSeqs, atom2ResLabels,
	  atom2Names, couplingConstValues, couplingConstErrors);

	coupling.writeCoupling(frameIndex);

	_summary.endFrame();
    }

    //-------------------------------------------------------------------
    // Save heteronuclear NOE values for one save frame.
    private void saveFrameHetNOE(S2DStarIfc star, SaveFrameNode frame,
      int frameIndex) throws S2DException
    {
        if (DEBUG >= 3) {
	    System.out.println("    S2DMain.saveFrameHetNOE(" +
	      star.getFrameName(frame) + ", " + frameIndex + ")");
	}

	//TEMP -- change strings to named constants
	String[] resSeqCodes = star.getFrameValues(frame,
	  "_Heteronuclear_NOE_value", "_Residue_seq_code");

	String[] resLabels = star.getFrameValues(frame,
	  "_Heteronuclear_NOE_value", "_Residue_label");

	String[] hetNOEValues = star.getFrameValues(frame,
	  "_Heteronuclear_NOE_value", "_Heteronuclear_NOE_value");

	String[] hetNOEErrors = star.getFrameValues(frame,
	  "_Heteronuclear_NOE_value", "_Heteronuclear_NOE_value_error");

	_summary.startFrame(star.getFrameDetails(frame));

	S2DHetNOE hetNOE = new S2DHetNOE(_accessionNum, _dataDir,
	  _sessionDir, _summary,
	  star.getOneFrameValue(frame, "_Spectrometer_frequency_1H"),
	  star.getOneFrameValue(frame, "_Atom_one_atom_name"),
	  star.getOneFrameValue(frame, "_Atom_two_atom_name"),
	  resSeqCodes, resLabels, hetNOEValues, hetNOEErrors);

	hetNOE.writeHetNOE(frameIndex);

	_summary.endFrame();
    }
}

// ========================================================================

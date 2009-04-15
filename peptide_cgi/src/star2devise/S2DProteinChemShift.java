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

// This class implements the calculation and output of chemical shift data
// for proteins.  For each set of chemical shift data, it creates a data
// file, a session file, an individual html file, and a link in the summary
// html file.

// ------------------------------------------------------------------------

// $Id$

// $Log$
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

import java.io.*;
import java.util.*;

public class S2DProteinChemShift extends S2DChemShift {
    //===================================================================
    // VARIABLES

    private static final int DEBUG = 0;

    //===================================================================
    // PUBLIC METHODS

    //-------------------------------------------------------------------
    // Constructor.
    public S2DProteinChemShift(String name, String longName, String dataDir,
      String sessionDir, S2DSummaryHtml summary, int[] resSeqCodes,
      String[] residueLabels, String[] atomNames, String[] atomTypes,
      double[] chemShiftVals, int[] ambiguityVals,
      int entityAssemblyID, String frameDetails)
      throws S2DException
    {
	super(name, longName, dataDir, sessionDir, summary, resSeqCodes,
	  residueLabels, atomNames, atomTypes, chemShiftVals,
	  ambiguityVals, entityAssemblyID, frameDetails);

        if (doDebugOutput(11)) {
	    System.out.println("S2DProteinChemShift.S2DProteinChemShift(" +
	      name + ")");
	}

	CHEMSHIFT_FILE += "chemshift.txt";
        _refTable = new ShiftDataManager(CHEMSHIFT_FILE);

	calculateDeltaShifts();
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
	          _name, frameIndex, _info);

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

    //===================================================================
    // PRIVATE METHODS

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
	    }



	}

	//go thru list of atoms, looking for Hs only? extract bonded Cs from bonds list, find them in the hashtable if they're there?
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

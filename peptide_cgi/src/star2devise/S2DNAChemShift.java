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
// for nucleic acids (DNA and RNA).  This class implements what's
// common to both DNA and RNA, with separate classes for what's different.
// For each set of chemical shift data, it creates a data file,
// a session file, an individual html file, and a link in the summary html
// file.

// ------------------------------------------------------------------------

// $Id$

// $Log$
// Revision 1.8  2009/03/24 19:04:50  wenger
// Fixed layout of nucleic acid deltashift session (made windows line
// up better, etc.); fixed nucleotide counts in summary html page, and
// changed residue to nucleotide where appropriate; fixed nucleic acid
// deltashift html pages so that they link to the nucleic-acid-specific
// help page.
//
// Revision 1.7  2009/03/16 20:33:08  wenger
// 3D data select view now only shows data that is available for the
// currently-selected entity assembly ID.
//
// Revision 1.6  2009/03/04 15:27:20  wenger
// DNA deltashift data is now working as part of 3D visualizations,
// just started on RNA, but need to finish other changes to RNA first;
// made changes to test67* and test68* accordingly.  (Note: highlighing
// doesn't work right for DNA in 3D visualizations.)
//
// Revision 1.5  2009/02/25 21:33:14  wenger
// Added residue labels to all data that were missing them (in preparation
// for selection by nucleotide in nucleotide visualizations; also allows
// user to drill down to see residue label in protein deltashift, CSI,
// etc., visualizations); changed tests accordingly; also fixed up some
// tests that weren't checking for entity assembly IDs in data.
//
// Revision 1.4  2009/02/20 22:54:24  wenger
// RNA visualization now works, using the DNA template; added stub
// help file for nucleic acid deltashift visualizations.
//
// Revision 1.3  2009/02/20 18:41:47  wenger
// Preliminary version of DNA deltashift session template is now in place
// (still needs cleanup); Peptide-CGI code uses that session template for
// DNA processing.
//
// Revision 1.2  2009/02/19 22:40:41  wenger
// DNA and RNA deltashift calculations now work (still need to check
// that all values are correct); added value checks to relevant tests.
//
// Revision 1.1  2009/02/18 21:43:07  wenger
// Added S2DNAChemShift class to clean up nucleic acid code (this class
// will do the actual calculation and writing of chemical shift deltas
// for nucleic acids); added schemas for nucleic acid deltashift
// visualizations; updated tests to reflect the fact that (at least
// initially) we're not going to generate CSI visualizations for nucleic
// acids.
//
//

// ========================================================================

package star2devise;

import java.io.*;
import java.util.*;

public class S2DNAChemShift extends S2DChemShift {
    //===================================================================
    // VARIABLES

    private static final int DEBUG = 0;

    // List of all possible atom names for the relevant type of
    // polymer (DNA or RNA) -- set by subclass.
    protected String[] atomNameList;

    // An array (indexed by residue number) holding the delta shift data
    // we have for each residue.
    protected ResidueInfo[] _deltashiftData;

    //===================================================================
    // PUBLIC METHODS

    //-------------------------------------------------------------------
    // Constructor.
    public S2DNAChemShift(String name, String longName, String dataDir,
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
	    System.out.println("S2DNAChemShift.S2DNAChemShift(" + name +
	      ")");
	}
    }

    //-------------------------------------------------------------------
    // Write the deltashifts for this data.
    public void writeDeltaShifts(int frameIndex, String schemaName,
      String attributes, int sessionType) throws S2DException
    {
        if (doDebugOutput(11)) {
	    System.out.println("S2DNAChemShift.writeDeltashifts()");
	}

	//
	// Write the deltashift values to the appropriate data file.
	//
        FileWriter deltashiftWriter = null;
	try {
            deltashiftWriter = S2DFileWriter.create(_dataDir +
	      File.separator + _name + S2DNames.DELTASHIFT_SUFFIX +
	      frameIndex + S2DNames.DAT_SUFFIX);
	    deltashiftWriter.write("# Data: delta shift values for " +
	      _name + "\n");
	    deltashiftWriter.write("# Schema: " + schemaName + "\n");
	    deltashiftWriter.write("# Attributes: " + attributes + "\n");
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

	    for (int index = 1; index < _deltashiftData.length; index++) {
		if (_deltashiftData[index] != null) {
		    dsCount++;
	            _deltashiftData[index].write(deltashiftWriter);
		}
	    }

	    //
	    // Write the session file
	    //
	    S2DSession.write(_sessionDir, sessionType, _name, frameIndex,
	      _info);

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
	      dsCount, true);

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

    //===================================================================
    // PROTECTED METHODS
    //-------------------------------------------------------------------
    // Calculate all delta shifts for this data set.
    protected void calculateDeltaShifts() throws S2DException
    {
        if (doDebugOutput(11)) {
	    System.out.println("S2DNAChemShift.calculateDeltaShifts()");
	}
	
	int lastResidue = findLastResidue();

	// Residues normally start with 1 -- skip the first element of
	// this array to make things simpler.
	_deltashiftData = new ResidueInfo[lastResidue + 1];

	for (int index = 0; index < _resSeqCodes.length; ++index) {
	    int resSeqCode = _resSeqCodes[index];
            String resLabel = _residueLabels[index];
	    if (_deltashiftData[resSeqCode] == null) {
	        _deltashiftData[resSeqCode] = new ResidueInfo(resSeqCode,
		  resLabel);
	    }

	    String atomName = _atomNames[index];
	    _atomSet.add(atomName);
	    double chemShift = _chemShiftVals[index];

	    try {
	        ShiftDataManager.Pair standardValue =
		  _refTable.returnValues(resLabel, atomName);

                // Note: do the calculation in double and truncate to float
		// to avoid getting values like 0.05000000000000071.
		float deltashift = (float)(chemShift -
		  standardValue.chemshift);

		_deltashiftData[resSeqCode].put(atomName,
		  new Float(deltashift));

	    } catch(S2DWarning ex) {
	        if (doDebugOutput(11)) {
	            System.err.println(ex.toString());
	        }
	    } catch(S2DException ex) {
	        System.err.println(ex.toString());
	    }
	}
    }

    //-------------------------------------------------------------------
    // Class to hold all deltashift info for one residue -- maps
    // atom name to deltashift value.
    protected class ResidueInfo extends Hashtable
    {
	int _residueSequenceCode;
	String _residueLabel;

        //---------------------------------------------------------------
	/**
	 * Constructor.
	 * @param The residue sequence code for this residue.
	 */
        public ResidueInfo(int residueSequenceCode, String residueLabel) {
	    super();

	    _residueSequenceCode = residueSequenceCode;
	    _residueLabel = residueLabel;

	    // Add all possible atoms to the hash table.
	    for (int index = 0; index < atomNameList.length; index++) {
	    	Float value = new Float(0.0/*TEMP?*/);
		put(atomNameList[index], value);
	    }
	}

        //---------------------------------------------------------------
	/**
	 * Write the deltashift values for this residue.
	 * @param The FileWriter to write to.
	 */
	public void write(FileWriter writer) throws IOException
	{
	    writer.write(_entityAssemblyID + " " +
	      _residueSequenceCode + " " +
	      _residueLabel + " ");

	    for (int index = 0; index < atomNameList.length; index++) {
	    	Float value = (Float)get(atomNameList[index]);
		writer.write(value + " ");
	    }
	    writer.write("\n");
	}
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

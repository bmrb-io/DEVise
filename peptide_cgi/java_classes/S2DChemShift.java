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

// This class implements the calculation and output of chemical shift data.
// For each set of chemical shift data, it creates a data file, a session
// file, an individual html file, and a link in the summary html file.

// ------------------------------------------------------------------------

// $Id$

// $Log$
// Revision 1.7  2001/04/24 18:06:43  wenger
// More improvements to "all chem shifts" visualization; found and fixed
// bug 666.
//
// Revision 1.6  2001/04/17 17:09:09  wenger
// Added display of H vs. N chem shifts.
//
// Revision 1.5  2001/04/16 19:49:11  wenger
// Added display of all chem shifts by amino acid.
//
// Revision 1.4  2001/03/08 21:10:34  wenger
// Merged changes from no_collab_br_2 thru no_collab_br_3 from the branch
// to the trunk.
//
// Revision 1.3.2.1  2001/02/22 19:28:30  wenger
// Fixed problem with reading back deltashift values in exponential format;
// error messages when checking for a protein are printed only if debug
// output is turned on; fixed problems with set_modes script.
//
// Revision 1.3  2001/01/30 17:11:59  wenger
// Modified code to take account of the fact that Star file tags, etc.,
// are not supposed to be case-sensitive.
//
// Revision 1.2  2001/01/25 16:37:45  wenger
// Fixed a bug that could cause an infinite loop in the perecent assignment
// calculations; put filesize, cpu, and coredump limits in s2d script;
// updated star file list; got rid of some unnecessary warnings.
//
// Revision 1.1  2001/01/17 20:00:06  wenger
// Restructured the peptide-cgi code to make it much more maintainable.
//

// ========================================================================

import java.io.*;
import java.util.*;

import ShiftDataManager.Pair;
import AssgDataManager.AssgEntry;

public class S2DChemShift {
    //===================================================================
    // VARIABLES

    private static final int DEBUG = 0;

    private int _accessionNum;
    private String _dataDir;
    private String _sessionDir;
    private S2DSummaryHtml _summary;

    private int[] _resSeqCodes;
    private String[] _residueLabels;
    private String[] _atomNames;
    private String[] _atomTypes;
    private double[] _chemShiftVals;

    private final String CHEMSHIFT_FILE = "chemshift.txt";
    private ShiftDataManager _refTable;

    //===================================================================
    // PUBLIC METHODS

    //-------------------------------------------------------------------
    // Constructor.
    public S2DChemShift(int accessionNum, String dataDir, String sessionDir,
      S2DSummaryHtml summary, int[] resSeqCodes, String[] residueLabels,
      String[] atomNames, String[] atomTypes, double[] chemShiftVals)
      throws S2DException
    {
        if (DEBUG >= 1) {
	    System.out.println("S2DChemShift.S2DChemShift(" + accessionNum +
	      ")");
	}

	_accessionNum = accessionNum;
	_dataDir = dataDir;
	_sessionDir = sessionDir;
	_summary = summary;

	_resSeqCodes = resSeqCodes;
	_residueLabels = residueLabels;
	_atomNames = atomNames;
	_atomTypes = atomTypes;
	_chemShiftVals = chemShiftVals;

	_refTable = new ShiftDataManager(_dataDir + "/" + CHEMSHIFT_FILE);
    }

    //-------------------------------------------------------------------
    // Write the deltashifts for this data.
    public void writeDeltashifts(int frameIndex) throws S2DException
    {
        if (DEBUG >= 1) {
	    System.out.println("S2DChemShift.writeDeltashifts()");
	}

	//
	// Calculate the deltashift values and write them to the data
	// file.
	//
        FileWriter deltashiftWriter = null;
	try {
            deltashiftWriter = new FileWriter(_dataDir + "/" +
	      _accessionNum + S2DNames.DELTASHIFT_SUFFIX + frameIndex +
	      S2DNames.DAT_SUFFIX);
        } catch(IOException ex) {
	    System.err.println("IOException writing deltashifts: " +
	      ex.getMessage());
	    throw new S2DError("Can't write deltashifts");
	}

	int prevResSeqCode = -1; // -1 means invalid
	int currResSeqCode;

	float haDeltashift = 0;
	float cDeltashift = 0;
	float caDeltashift = 0;
	float cbDeltashift = 0;

	int dsCount = 0;

	try {
	    for (int index = 0; index < _resSeqCodes.length; index++) {
	        currResSeqCode = _resSeqCodes[index];

	        if (currResSeqCode != prevResSeqCode) {
		    if (prevResSeqCode != -1) {
	                // We're done with prevSeqCode, so write out its info.
		        deltashiftWriter.write(prevResSeqCode + " " +
			  haDeltashift + " " + cDeltashift + " " +
			  caDeltashift + " " + cbDeltashift + "\n");
	                dsCount++;
		    }

		    prevResSeqCode = currResSeqCode;

		    // Re-initialize the values.
	            haDeltashift = 0;
	            cDeltashift = 0;
	            caDeltashift = 0;
	            cbDeltashift = 0;
	        }

	        String resLabel = _residueLabels[index];
	        String atomName = _atomNames[index];
	        double chemShift = _chemShiftVals[index];

		try {
	            Pair standardValue = _refTable.returnValues(resLabel,
		      atomName);

		    // Note: do the calculation in double and truncate to float
		    // to avoid getting values like 0.05000000000000071.
		    float deltashift = (float)(chemShift -
		      standardValue.chemshift);

		    // Special cases of combining HA2 with HA and HA3 with
		    // CB as per algorithm.
		    //TEMP -- need alg reference here
	            if (atomName.equalsIgnoreCase("HA") ||
		      (atomName.equalsIgnoreCase("HA2") &&
		      resLabel.equalsIgnoreCase("GLY"))) {
		        haDeltashift = deltashift;
	            } else if (atomName.equalsIgnoreCase("C")) {
		        cDeltashift = deltashift;
		    } else if (atomName.equalsIgnoreCase("CA")) {
		        caDeltashift = deltashift;
		    } else if (atomName.equalsIgnoreCase("CB") ||
		      (atomName.equalsIgnoreCase("HA3") &&
		      resLabel.equalsIgnoreCase("GLY"))) {
		        cbDeltashift = deltashift;
		    } else {
		        //TEMP -- should we ever get here????
		    }
	        } catch(S2DWarning ex) {
		    if (DEBUG >= 1) {
		        System.err.println(ex.getMessage());
		    }
	        } catch(S2DException ex) {
		    System.err.println(ex.getMessage());
		}
	    }

	    // Write last residue.
	    //TEMP -- currResSeqCode should equal prevResSeqCode here?
	    deltashiftWriter.write(prevResSeqCode + " " + haDeltashift +
	      " " + cDeltashift + " " + caDeltashift + " " +
	      cbDeltashift + "\n");
	    dsCount++;

	    //
	    // Write the session file
	    //
	    S2DSession.write(_sessionDir, S2DUtils.TYPE_DELTASHIFT,
	      _accessionNum, frameIndex);

	    //
	    // Write the session-specific html file.
	    //
	    S2DSpecificHtml.write(_dataDir, S2DUtils.TYPE_DELTASHIFT,
	      _accessionNum, frameIndex);

	    //
	    // Write the link in the summary html file.
	    //
	    _summary.writeDeltashift(frameIndex, dsCount);

	} catch (IOException ex) {
	    System.err.println("IOException writing deltashift data: " +
	      ex.getMessage());
	    throw new S2DError("Unable to write deltashift data for " +
	      frameIndex);
	} finally {
	    try {
	        deltashiftWriter.close();
	    } catch (IOException ex) {
	        System.err.println("IOException: " + ex.getMessage());
	    }
	}
    }

    //-------------------------------------------------------------------
    // Write the CSI info for this data.
    public void writeCSI(int frameIndex) throws S2DException
    {
        if (DEBUG >= 1) {
	    System.out.println("S2DChemShift.writeCSI()");
	}

	//
	// Calculate the CSI values and write them to the data file.
	//
        FileWriter csiWriter = null;
	S2DDsReader dsLine = null;
	try {
            csiWriter = new FileWriter(_dataDir + "/" +
	      _accessionNum + S2DNames.CSI_SUFFIX + frameIndex +
	      S2DNames.DAT_SUFFIX);
	    dsLine = new S2DDsReader(new StreamTokenizer(
	      new FileReader(_dataDir + "/" + _accessionNum +
	        S2DNames.DELTASHIFT_SUFFIX + frameIndex +
		S2DNames.DAT_SUFFIX)));
        } catch(IOException ex) {
	    System.err.println("IOException writing CSI values: " +
	      ex.getMessage());
	    throw new S2DError("Can't write CSI values");
	}

	int prevCsiSeq = -1; // -1 means invalid
	int currCsiSeq;
	int currDelSeq = -1; // -1 means invalid
	boolean hasCsiInfo = false;

	int haCsi = 0;
	int cCsi = 0;
	int caCsi = 0;
	int cbCsi = 0;
	int consCsi = 0;

	int csiCount = 0;

	try {
	    for (int index = 0; index < _resSeqCodes.length; index++) {
	        currCsiSeq = _resSeqCodes[index];

	        if (currCsiSeq != prevCsiSeq) {
		    if (prevCsiSeq != -1 && hasCsiInfo) {
	                // We're done with prevCsiSeq, so write out its info.
		        csiWriter.write(prevCsiSeq + " " + haCsi + " " +
			  cCsi + " " + caCsi + " " + cbCsi + " " +
			  consCsi + "\n");
		        csiCount++;
		    }

		    while (currDelSeq < currCsiSeq) {
		        dsLine.readAndTokenLine();
			currDelSeq = dsLine.seqNumber;
		    }

		    prevCsiSeq = currCsiSeq;
		    hasCsiInfo = false;

		    // Re-initialize the values.
	            haCsi = 0;
	            cCsi = 0;
	            caCsi = 0;
	            cbCsi = 0;
	            consCsi = dsLine.csi;

                }

		if (currCsiSeq == currDelSeq) {
		    hasCsiInfo = true;

	            String resLabel = _residueLabels[index];
	            String atomName = _atomNames[index];
		    double chemShift = _chemShiftVals[index];

		    try {
	                Pair standardValue = _refTable.returnValues(resLabel,
			  atomName);

			// Note: do the calculation in double and truncate to
			// float to avoid getting values like
			// 0.05000000000000071.
		        float deltashift = (float)(chemShift -
			  standardValue.chemshift);

			int csi;
			if (deltashift > standardValue.offset) {
			    csi = 1;
			} else if (deltashift < -1.0 * standardValue.offset) {
			    csi = -1;
			} else {
			    csi = 0;
			}

		        // Special cases of combining HA2 with HA and HA3 with
		        // CB as per algorithm.
		        //TEMP -- need alg reference here
	                if (atomName.equalsIgnoreCase("HA") ||
		          (atomName.equalsIgnoreCase("HA2") &&
			  resLabel.equalsIgnoreCase("GLY"))) {
		            haCsi = csi;
	                } else if (atomName.equalsIgnoreCase("C")) {
		            cCsi = csi;
		        } else if (atomName.equalsIgnoreCase("CA")) {
		            caCsi = csi;
		        } else if (atomName.equalsIgnoreCase("CB") ||
		          (atomName.equalsIgnoreCase("HA3") &&
			  resLabel.equalsIgnoreCase("GLY"))) {
		            cbCsi = csi;
		        } else {
		            //TEMP -- should we ever get here????
		        }
	            } catch(S2DWarning ex) {
		        if (DEBUG >= 1) {
		            System.err.println(ex.getMessage());
		        }
		    } catch (S2DException ex) {
		        System.err.println(ex.getMessage());
		    }
		}
            }

	    // Write last residue.
	    //TEMP -- currResSeqCode should equal prevResSeqCode here?
	    if (hasCsiInfo) {
	        csiWriter.write(prevCsiSeq + " " + haCsi + " " +
		  cCsi + " " + caCsi + " " + cbCsi + " " +
		  consCsi + "\n");
	        csiCount++;
	    }

	    //
	    // Write the session file.
	    //
	    S2DSession.write(_sessionDir, S2DUtils.TYPE_CSI,
	      _accessionNum, frameIndex);

	    //
	    // Write the session-specific html file.
	    //
	    S2DSpecificHtml.write(_dataDir, S2DUtils.TYPE_CSI,
	      _accessionNum, frameIndex);

	    //
	    // Write the link in the summary html file.
	    //
	    _summary.writeCSI(frameIndex, csiCount);

	} catch (IOException ex) {
	    System.err.println("IOException writing CSI data: " +
	      ex.getMessage());
	    throw new S2DError("Unable to write CSI data for " +
	      frameIndex);
	} finally {
	    try {
	        csiWriter.close();
	    } catch (IOException ex) {
	        System.err.println("IOException: " + ex.getMessage());
	    }
	}
    }

    //-------------------------------------------------------------------
    // Write the percent assignments for this data.
    public void writePctAssign(int frameIndex) throws S2DException
    {
        if (DEBUG >= 1) {
	    System.out.println("S2DChemShift.writePctAssign()");
	}

	//
	// Calculate the percent assignment values and write them to the
	// data file.
	//
        FileWriter pctWriter = null;
	AssgDataManager assgTable = null;
	try {
            pctWriter = new FileWriter(_dataDir + "/" +
	      _accessionNum + S2DNames.PERCENT_ASSIGN_SUFFIX + frameIndex +
	      S2DNames.DAT_SUFFIX);
	    assgTable = new AssgDataManager(_dataDir + "/" +
	      S2DNames.CHEMASSG_FILE);
        } catch(IOException ex) {
	    System.err.println(
	      "IOException writing percent assignment values: " +
	      ex.getMessage());
	    throw new S2DError("Can't write percent assignment values");
	}

	int paCount = 0;

        try {
	    int index = 0;
	    while (index < _resSeqCodes.length) {
	        int resSeqCode = _resSeqCodes[index];
	        String resLabel = _residueLabels[index];

		try {
		    AssgEntry assignments = assgTable.returnAssg(resLabel);

		    int starNumH = 0;
		    int starNumC = 0;
		    int starNumN = 0;

		    while (index < _resSeqCodes.length &&
		      resLabel.equalsIgnoreCase(_residueLabels[index]) &&
		      resSeqCode == _resSeqCodes[index]) {
	                String atomType = _atomTypes[index];
			if (atomType.equalsIgnoreCase("H")) {
			    starNumH++;
			} else if (atomType.equalsIgnoreCase("C")) {
			    starNumC++;
			} else if (atomType.equalsIgnoreCase("N")) {
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
		    if (DEBUG >= 1) {
		        System.err.println(ex.getMessage());
		    }
		} catch (S2DException ex) {
		    index++;
		    System.err.println(ex.getMessage());
		}
	    }

	    //
	    // Write the session file.
	    //
	    S2DSession.write(_sessionDir, S2DUtils.TYPE_PCT_ASSIGN,
	      _accessionNum, frameIndex);

	    //
	    // Write the session-specific html file.
	    //
	    S2DSpecificHtml.write(_dataDir, S2DUtils.TYPE_PCT_ASSIGN,
	      _accessionNum, frameIndex);

	    //
	    // Write the link in the summary html file.
	    //
	    _summary.writePctAssign(frameIndex, paCount);

	} catch (IOException ex) {
	    System.err.println("IOException writing percent assignment data: " +
	      ex.getMessage());
	    throw new S2DError("Unable to write percent assignment data for " +
	      frameIndex);
	} finally {
	    try {
	        pctWriter.close();
	    } catch (IOException ex) {
	        System.err.println("IOException: " + ex.getMessage());
	    }
	}
    }

    //-------------------------------------------------------------------
    // Write all chem shifts for this data.
    public void writeAllShifts(int frameIndex) throws S2DException
    {
        if (DEBUG >= 1) {
	    System.out.println("S2DChemShift.writeAllShifts()");
	}

        FileWriter asWriter = null;
	try {
            asWriter = new FileWriter(_dataDir + "/" +
	      _accessionNum + S2DNames.ALL_CHEM_SHIFT_SUFFIX + frameIndex +
	      S2DNames.DAT_SUFFIX);
        } catch(IOException ex) {
	    System.err.println(
	      "IOException writing all chem shift values: " +
	      ex.getMessage());
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
	      _accessionNum, frameIndex);

	    //
	    // Write the session-specific html file.
	    //
	    S2DSpecificHtml.write(_dataDir, S2DUtils.TYPE_ALL_CHEM_SHIFTS,
	      _accessionNum, frameIndex);

	    //
	    // Write the link in the summary html file.
	    //
	    _summary.writeAllShifts(frameIndex, _resSeqCodes.length);

	} catch (IOException ex) {
	    System.err.println("IOException writing all chem shifts: " +
	      ex.getMessage());
	    throw new S2DError("Unable to write all chem shifts for " +
	      frameIndex);
	} finally {
	    try {
	        asWriter.close();
	    } catch (IOException ex) {
	        System.err.println("IOException: " + ex.getMessage());
	    }
	}
    }

    //-------------------------------------------------------------------
    // Write H vs. N chem shifts for this data.
    public void writeHvsNShifts(int frameIndex) throws S2DException
    {
        if (DEBUG >= 1) {
	    System.out.println("S2DChemShift.writeHvsNShifts()");
	}

        FileWriter hnWriter = null;
	try {
            hnWriter = new FileWriter(_dataDir + "/" +
	      _accessionNum + S2DNames.HVSN_CHEM_SHIFT_SUFFIX + frameIndex +
	      S2DNames.DAT_SUFFIX);
        } catch(IOException ex) {
	    System.err.println(
	      "IOException writing H vs. N chem shift values: " +
	      ex.getMessage());
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
			  " " + hShift + " " + nShift + "\n");
		        hnCount++;
		    }

		    prevSeqCode = currSeqCode;
		    prevResLabel = currResLabel;
		    hasH = false;
		    hasN = false;
                }

	        String atomName = _atomNames[index];
	        double chemShift = _chemShiftVals[index];

		if (atomName.equalsIgnoreCase("H")) {
		    if (hasH) {
		        System.err.println("Multiple H entries in one " +
			  "residue(" + currSeqCode + ")!");
		    }
		    hasH = true;
		    hShift = chemShift;
		} else if (atomName.equalsIgnoreCase("N")) {
		    if (hasN) {
		        System.err.println("Multiple N entries in one " +
			  "residue(" + currSeqCode + ")!");
		    }
		    hasN = true;
		    nShift = chemShift;
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
	          _accessionNum, frameIndex);

	        //
	        // Write the session-specific html file.
	        //
	        S2DSpecificHtml.write(_dataDir, S2DUtils.TYPE_HVSN_CHEM_SHIFTS,
	          _accessionNum, frameIndex);

	        //
	        // Write the link in the summary html file.
	        //
	        _summary.writeHvsNShifts(frameIndex, hnCount);
	    }

	} catch (IOException ex) {
	    System.err.println("IOException writing all chem shifts: " +
	      ex.getMessage());
	    throw new S2DError("Unable to write all chem shifts for " +
	      frameIndex);
	} finally {
	    try {
	        hnWriter.close();
	    } catch (IOException ex) {
	        System.err.println("IOException: " + ex.getMessage());
	    }
	}
    }
}

// ========================================================================

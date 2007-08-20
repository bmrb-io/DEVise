// ========================================================================
// DEVise Data Visualization Software
// (c) Copyright 2004-2007
// By the DEVise Development Group
// Madison, Wisconsin
// All Rights Reserved.
// ========================================================================

// Under no circumstances is this software to be copied, distributed,
// or altered in any way without prior permission from the DEVise
// Development Group.

// ------------------------------------------------------------------------

// This class calculates and writes out the per-residue Pistachio
// values.  It also creates the session file and visualization-specific
// html file for the Pistachio visualization.

// ------------------------------------------------------------------------

// $Id$

// $Log$
// Revision 1.3  2006/02/01 21:34:32  wenger
// Merged peptide_cgi_10_8_0_br_0 thru peptide_cgi_10_8_0_br_2
// to the trunk.
//
// Revision 1.2  2006/02/01 20:23:12  wenger
// Merged V2_1b4_br_0 thru peptide_cgi_10_8_0_base to the
// trunk.
//
// Revision 1.1.4.3.6.3  2005/09/14 15:14:22  wenger
// Config changes for Osaka, and for manatee and basslet at BMRB;
// new Pistachio test file; fixed test23 to work on non-CS machines;
// better Pistachio error message.
//
// Revision 1.1.4.3.6.2  2005/06/28 17:08:22  wenger
// Ambiguity code and figure of merit visualizations now use 2D main
// views; changed "Pistachio" to "assignment figure of merit" in
// visualizations, etc.
//
// Revision 1.1.4.3.6.1  2005/05/19 16:07:43  wenger
// Merged nmrfam_mods2_br (argh -- must have forgotten to make
// nmrfam_mods2_br_0 tag!) thru nmrfam_mods2_br_3 to
// peptide_cgi_10_8_0_br.
//
// Revision 1.1.4.3.4.1  2005/05/18 18:08:14  wenger
// Changed Pisatchio percentage calculation to be relative to the number
// of atoms that actually exist in each amino acid, not just the number
// we have figures of merit for; the Pistachio visualization now comes
// up with "Color by figure of merit" selected; minor improvements to
// ambiguity debug/error messages.
//
// Revision 1.1.4.3  2005/03/22 20:34:38  wenger
// Merged ambiguity_vis2_br_0 thru ambiguity_vis2_br_3 to V2_1b4_br.
//
// Revision 1.1.4.2.4.3  2005/03/22 16:38:33  wenger
// Minor cleanup.
//
// Revision 1.1.4.2.4.2  2005/03/11 23:33:20  wenger
// Ambiguity visualization is mostly working (incorporating some feedback
// from Eldon); still needs some cleanup to the calculations, though.
//
// Revision 1.1.4.2.4.1  2005/03/10 19:27:36  wenger
// Merged ambiguity_vis_br_0 thru ambiguity_vis_br_end to
// ambiguity_vis2_br.
//
// Revision 1.1.4.2.2.1  2005/03/10 18:34:07  wenger
// I need to commit the ambiguity stuff I've done so far so I can make
// a new ambiguity branch that has the latest Pistachio changes.
//
// Revision 1.1.4.2  2005/02/01 21:37:23  wenger
// More cleanups, especially to the Pistachio code.
//
// Revision 1.1.4.1  2005/01/31 23:02:54  wenger
// Merged pistachio_vis_br_0 thru pistachio_vis_br_1a to V2_1b4_br.
//
// Revision 1.1.2.5  2005/01/28 18:49:01  wenger
// Added "color by figure of merit" option to the Pistachio visualization.
//
// Revision 1.1.2.4  2005/01/12 20:46:41  wenger
// Pistachio processing is now integrated into the normal Peptide-CGI
// processing -- the Pistachio visualization is generated autmatically
// if the Pistachio data exists.  (Still needs some cleanup, though.)
// (We generate the Pistachio visualization by generating a temporary
// mmCIF file with coordinates -- that is then run through the normal
// coordinate processing to generate the DEVise file with Pistachio
// coordinates.)
//
// Revision 1.1.2.3  2004/12/20 22:46:16  wenger
// Improved "last residue" calculation so out-of-order residues don't
// cause array bounds exception; fixed NMR-STAR 3.0 problem with tag
// names for 3-letter residue lists (bmr4096.str.nmrstr has inconsistent
// 1-letter and 3-letter residue lists -- kludged it so tests work,
// removed the 1-letter residue list I added to bmr6318_pistachio.str.nmrstr).
//
// Revision 1.1.2.2  2004/12/01 21:56:25  wenger
// Changed "HN"s to "H"s in Pistachio code because BMRB files have "H"
// where Hamid expected "HN".
//
// Revision 1.1.2.1  2004/11/11 23:13:03  wenger
// Early prototype of Pistachio calculations and visualization;
// session, etc., are not automatically generated yet.
//

// ========================================================================

package star2devise;

import java.io.*;
import java.util.*;

public class S2DPistachio {
    //===================================================================
    // VARIABLES

    private static final int DEBUG = 0;

    private String _name;
    private String _dataDir;
    private String _sessionDir;
    private S2DSummaryHtml _summary;

    private int[] _resSeqCodes;
    private String[] _residueLabels;
    private String[] _atomNames;
    private double[] _meritVals;

    private boolean[] _residueHasData;
    private float[] _backboneGE95;
    private float[] _backboneLT95;
    private float[] _sideChainGE95;
    private float[] _sideChainLT95;
    private float[] _hGE95;
    private float[] _hLT95;

    //===================================================================
    // PUBLIC METHODS

    //-------------------------------------------------------------------
    // Constructor.
    public S2DPistachio(String name, String dataDir, String sessionDir,
      S2DSummaryHtml summary, int[] resSeqCodes, String[] residueLabels,
      String[] atomNames, double[] meritVals)
      throws S2DException
    {
        if (doDebugOutput(11)) {
	    System.out.println("S2DPistachio.S2DPistachio(" + name +
	      ")");
	}

	_name = name;
	_dataDir = dataDir;
	_sessionDir = sessionDir;
	_summary = summary;

	_resSeqCodes = resSeqCodes;
	_residueLabels = residueLabels;
	_atomNames = atomNames;
	_meritVals = meritVals;

	calculatePistachioValues();
    }

    //-------------------------------------------------------------------
    // Write the Pistachio values for this data.
    public void writePistachio(int frameIndex) throws S2DException
    {
        if (doDebugOutput(11)) {
	    System.out.println("S2DChemShift.writePistachio()");
	}

	//
	// Write the Pistachio values to the appropriate data file.
	//
        FileWriter pistachioWriter = null;
	try {
            pistachioWriter = S2DFileWriter.create(_dataDir + File.separator +
	      _name + S2DNames.PISTACHIO_SUFFIX + frameIndex +
	      S2DNames.DAT_SUFFIX);
	    pistachioWriter.write("# Data: Assignment figure of merit " +
	      "values for " + _name + "\n");
	    pistachioWriter.write("# Schema: bmrb-Pistachio\n");
	    pistachioWriter.write("# Attributes: Residue_seq_code; " +
	      "backbone >= 95%; backbone < 95%; side chain >= 95%; " +
	      "side chain < 95%; H >= 95%; H < 95%\n");
            pistachioWriter.write("# Peptide-CGI version: " +
	      S2DMain.PEP_CGI_VERSION + "\n");
            pistachioWriter.write("# Generation date: " +
	      S2DUtils.getCurrentDateStr() + "\n");
	    pistachioWriter.write("#\n");

        } catch(IOException ex) {
	    System.err.println("IOException writing Pistachio values: " +
	      ex.toString());
	    throw new S2DError("Can't write Pistachio values");
	}

	try {
            for (int index = 0; index < _residueHasData.length; ++index) {
		if (_residueHasData[index]) {
	            pistachioWriter.write(index + " " +
		      _backboneGE95[index] + " " +
		      _backboneLT95[index] + " " +
		      _sideChainGE95[index] + " " +
		      _sideChainLT95[index] + " " +
		      _hGE95[index] + " " +
		      _hLT95[index] + "\n");
	        }
	    }

	    //
	    // Write the session file
	    //
	    String info = "Visualization of " + _name +
	      " assignment figure of merit data";
	    S2DSession.write(_sessionDir, S2DUtils.TYPE_PISTACHIO,
	      _name, frameIndex, info);

	    //
	    // Write the session-specific html file.
	    //
	    S2DSpecificHtml.write(_summary.getHtmlDir(),
	      S2DUtils.TYPE_PISTACHIO, _name, frameIndex,
	      "Assignment figure of merit data");

	    //
	    // Write the link in the summary html file.
	    //
	    _summary.writePistachio(frameIndex);

	} catch (IOException ex) {
	    System.err.println("IOException writing Pistachio values: " +
	      ex.toString());
	    throw new S2DError("Unable to write Pistachio values for " +
	      frameIndex);
	} finally {
	    try {
	        pistachioWriter.close();
	    } catch (IOException ex) {
	        System.err.println("IOException: " + ex.toString());
	    }
	}
    }

    //===================================================================
    // PRIVATE METHODS

    //-------------------------------------------------------------------
    // Calculate the Pistachio values we want to visualize for this
    // data set.
    private void calculatePistachioValues() throws S2DException
    {
        if (doDebugOutput(11)) {
	    System.out.println("S2DChemShift.calculatePistachioValues()");
	}

        // We calculate lastResidue this way to allow for the data not
        // being strictly ordered by residue.
        int lastResidue = 0;
        for (int index = 0; index < _resSeqCodes.length; index++) {
            lastResidue = Math.max(lastResidue, _resSeqCodes[index]);
        }

	// Residues normally start with 1 -- skip the first element of
	// these arrays to make things simpler.
        _residueHasData = new boolean[lastResidue + 1];
        _backboneGE95 = new float[lastResidue + 1];
        _backboneLT95 = new float[lastResidue + 1];
        _sideChainGE95 = new float[lastResidue + 1];
        _sideChainLT95 = new float[lastResidue + 1];
        _hGE95 = new float[lastResidue + 1];
        _hLT95 = new float[lastResidue + 1];

	for (int index = 0; index < _residueHasData.length; ++index) {
	    _residueHasData[index] = false;
	}

	TempData td = new TempData();

	int currResSeqCode = -1; // invalid value
	int prevResSeqCode = -1; // invalid value
	String residueLabel = null;
	for (int index = 0; index < _resSeqCodes.length; ++index) {

	    currResSeqCode = _resSeqCodes[index];

	    if (currResSeqCode != prevResSeqCode) {
		if (currResSeqCode < prevResSeqCode) {
		    throw new S2DError("Residue numbers must be " +
		      "monotonically increasing for Pistachio data " +
		      "(" + currResSeqCode + " after " + prevResSeqCode +
		      ")");
		}

		if (prevResSeqCode != -1) {
		    td.save(prevResSeqCode, residueLabel);
		}

		td.reset();
		residueLabel = _residueLabels[index];

		prevResSeqCode = currResSeqCode;
	    }

	    td.addAnAtom(_atomNames[index], _meritVals[index]);
	}

	td.save(currResSeqCode, residueLabel);
    }

    // ========================================================================

    // This class holds the data for one residue as we work our way
    // through the data for that residue.
    class TempData {
	public int _backboneTotalCount = 0;
	public int _backboneGE95Count = 0;
	public int _sideChainTotalCount = 0;
	public int _sideChainGE95Count = 0;
	public int _hTotalCount = 0;
	public int _hGE95Count = 0;

	public void reset()
	{
	    _backboneTotalCount = 0;
	    _backboneGE95Count = 0;
	    _sideChainTotalCount = 0;
	    _sideChainGE95Count = 0;
	    _hTotalCount = 0;
	    _hGE95Count = 0;
	}

	public void addAnAtom(String atomName, double meritVal)
	{
	    if (S2DNames.ATOM_H.equals(atomName)) {
		++_hTotalCount;
		++_backboneTotalCount;
	        if (meritVal >= 0.95) {
		    ++_hGE95Count;
		    ++_backboneGE95Count;
		}

	    } else if (S2DNames.ATOM_C.equals(atomName) ||
	      S2DNames.ATOM_CA.equals(atomName) ||
	      S2DNames.ATOM_N.equals(atomName)) {
		++_backboneTotalCount;
	        if (meritVal >= 0.95) {
		    ++_backboneGE95Count;
		}

	    } else {
		++_sideChainTotalCount;
	        if (meritVal >= 0.95) {
		    ++_sideChainGE95Count;
		}
	    }
	}

	public void save(int resSeqCode, String residueLabel)
	{
	    if (doDebugOutput(13)) {
	        System.out.println("S2DPistachio.TempData.save(" +
	          resSeqCode + ", " + residueLabel + ")");
	    }

            _residueHasData[resSeqCode] = true;

	    try {
	        S2DAtomCounts counts = S2DAtomCounts.getInstance();

		int idealBackboneCount = counts.getPistBackCount(residueLabel);
	        if (idealBackboneCount > 0 && _backboneTotalCount > 0) {
	            _backboneGE95[resSeqCode] = (float)_backboneGE95Count /
		        (float)idealBackboneCount;
	            _backboneLT95[resSeqCode] = (float)(_backboneTotalCount -
		      _backboneGE95Count) / (float)idealBackboneCount;
	        } else {
	            _backboneGE95[resSeqCode] = 0.0f;
	            _backboneLT95[resSeqCode] = 0.0f;
	        }

		int idealSideChainCount = counts.getPistSideCount(residueLabel);
	        if (idealSideChainCount > 0 && _sideChainTotalCount > 0) {
	            _sideChainGE95[resSeqCode] = (float)_sideChainGE95Count /
		      (float)idealSideChainCount;
	            _sideChainLT95[resSeqCode] = (float)(_sideChainTotalCount -
		      _sideChainGE95Count) / (float)idealSideChainCount;
	        } else {
	            _sideChainGE95[resSeqCode] = 0.0f;
	            _sideChainLT95[resSeqCode] = 0.0f;
	        }

		int idealHCount = counts.getPistHCount(residueLabel);
	        if (idealHCount > 0 && _hTotalCount > 0) {
	            _hGE95[resSeqCode] = (float)_hGE95Count /
		      (float)idealHCount;
	            _hLT95[resSeqCode] = (float)(_hTotalCount - _hGE95Count) /
		      (float)idealHCount;
	        } else {
	            _hGE95[resSeqCode] = 0.0f;
	            _hLT95[resSeqCode] = 0.0f;
	        }
	    } catch (S2DException ex) {
	        System.err.println("Exception saving Pistachio values" + ex);
	    }
	}
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

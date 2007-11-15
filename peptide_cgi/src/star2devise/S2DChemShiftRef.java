// ========================================================================
// DEVise Data Visualization Software
// (c) Copyright 2003-2007
// By the DEVise Development Group
// Madison, Wisconsin
// All Rights Reserved.
// ========================================================================

// Under no circumstances is this software to be copied, distributed,
// or altered in any way without prior permission from the DEVise
// Development Group.

// ------------------------------------------------------------------------

// This class implements the integration of chemical shift *reference*
// data.  The actual calculations aren't done in here; they are done by
// Dan Sinclair's chemShift program, which uses molmol and NMRPipe.

// ------------------------------------------------------------------------

// $Id$

// $Log$
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
// Revision 1.1.2.30.10.1  2005/05/19 16:07:43  wenger
// Merged nmrfam_mods2_br (argh -- must have forgotten to make
// nmrfam_mods2_br_0 tag!) thru nmrfam_mods2_br_3 to
// peptide_cgi_10_8_0_br.
//
// ...

// ========================================================================

package star2devise;

import java.util.*;
import java.io.*;

public class S2DChemShiftRef
{
    //===================================================================
    // VARIABLES

    private static final int DEBUG = 0;

    private static final String MIN_CHEM_SHIFT_VERSION = "1.4.1";

    private String _name;
    private String _longName;

    // File name as seen by s2d.
    private String _s2dFileName;

    // File name as seen by ChemShift.
    private String _csFileName;

    private String _dataDir;
    private String _csrDataDir;
    private String _sessionDir;
    private String _bmrbId;
    private Vector _localFiles;
    private String _pdbId;
    private S2DSummaryHtml _summary;
    private int _frameIndex;
    private String _cmdStr;
    private S2DWaitFile _doneFile = null;
    private File _errorFile = null;
    private int _timeout;
    private boolean _ranButFailed = false;
    private boolean _csrRan = false;

    //===================================================================
    // PUBLIC METHODS

    public S2DChemShiftRef(String name, String longName, String dataDir,
	  String csrDataDir, String sessionDir, String bmrbId, Vector localFiles,
	  String pdbId, S2DSummaryHtml summary, int frameIndex, int timeout)
    {
        if (doDebugOutput(11)) {
	    System.out.println("S2DChemShiftRef.S2DChemShiftRef(" + name +
	    ", " + dataDir + ", " + csrDataDir + ", " + sessionDir + ", " +
	    bmrbId + ", " + pdbId + ", " + frameIndex + ", " + timeout + ")");
	}

	_name = name;
	_longName = longName;

	_s2dFileName = dataDir + File.separator + name +
	  S2DNames.CSR_SUFFIX + frameIndex;

	// Note: we're explicitly putting "/" rather than File.separator
	// in the paths here because even if this is running on Windows,
	// we're generating a command file that's used on Linux.  (At
	// least for now.)  (Man, this multi-platform stuff is a pain...)
	// wenger 2003-11-12.
	_csFileName = csrDataDir + "/" + name + S2DNames.CSR_SUFFIX +
	  frameIndex;

	_dataDir = dataDir;
	_csrDataDir = csrDataDir;
	_sessionDir = sessionDir;
	_bmrbId = bmrbId;
	_localFiles = localFiles;
	_pdbId = pdbId;
	_summary = summary;
	_frameIndex = frameIndex;
	_timeout = timeout;
    }

    public void run() throws S2DException
    {
        if (doDebugOutput(11)) {
	    System.out.println("S2DChemShiftRef.run()");
	}

	// Make sure the error file doesn't already exist so we don't
	// goof ourselves up.
	_errorFile = new File(_s2dFileName + ".error");
	if (_errorFile.exists()) {
	    if (!_errorFile.delete()) {
	        throw new S2DError("Unable to delete existing error file " +
		  _errorFile);
	    }
	}

	_doneFile = new S2DWaitFile(_s2dFileName + ".done");

	try {
	    //
	    // Create the arguments for chemShift.
	    //
	    String csrInput = null; // file or BMRB ID for CSR
	    String inputFile = null; // input file name, no directory info
	    String inputPath = null; // full path of input file
	    String localArg = null;
	    if (!_bmrbId.equals("")) {
	    	csrInput = _bmrbId;
		localArg = "";
	    } else if (_localFiles.size() > 0) {
		inputPath = (String)_localFiles.elementAt(0);
		inputFile = (new File(inputPath)).getName();
		csrInput = _csrDataDir + "/" + inputFile;
		localArg = "-l ";
	    } else {
		throw new S2DError("No NMR-STAR file specified");
	    }

	    // Note: we're explicitly putting "/" rather than File.separator
	    // in the paths here because even if this is running on Windows,
	    // we're generating a command file that's used on Linux.  (At
	    // least for now.)  (Man, this multi-platform stuff is a pain...)
	    // wenger 2003-11-12.
	    _cmdStr = localArg + csrInput + " " + _pdbId + " " + _csFileName;
            if (doDebugOutput(12)) {
	    	System.out.println("ChemShiftRef cmdStr = <" + _cmdStr + ">");
	    }

	    //
	    // If local mode, copy input file into _data_dir.
	    //
	    if (_bmrbId.equals("")) {
	    	S2DUtils.copyFile(inputPath, _dataDir + File.separator +
		  inputFile);
	    }

	    //
	    // Dump the chemShift arguments into the run file.
	    //
	    String runFileName = _dataDir + File.separator + "csr.run";
	    FileWriter writer = S2DFileWriter.create(runFileName);
	    writer.write(_cmdStr);
	    writer.close();

        } catch (Exception ex) {
	    System.err.println("Exception running chem shift reference " +
	      "calculation: " + ex.toString());
	    throw new S2DError("Unable to run chemical shift reference " +
	      "calculation");
	}
    }

    public void postProcess(boolean doProteinCheck) throws S2DException
    {
        if (doDebugOutput(11)) {
	    System.out.println("S2DChemShiftRef.postProcess()");
	}

	try {

	    //
	    // Make sure the external process has finished successfully.
	    //
	    _doneFile.wait(1000, _timeout * 1000);

	    _csrRan = true;

	    boolean failed = _errorFile.exists();
	    if (failed) {
		_ranButFailed = true;
		throw new S2DError("Chem shift reference calculation failed!!");
	    }

            checkFileVersions();

	    //
	    // Write the data file telling where the coordinates came from
	    // (so we can show that in the visualization).
	    //
	    Writer srcWriter = S2DFileWriter.create(_s2dFileName + "src.dat");
	    String bmrbMsg;
	    if (!_bmrbId.equals("")) {
                bmrbMsg = "Observed chem shifts from BMRB accession " + _bmrbId;
	    } else {
                bmrbMsg = "Observed chem shifts from " + _longName;
	    }
	    srcWriter.write(bmrbMsg + "; calculated chem shifts from PDB ID " +
	      _pdbId + "\n");
	    srcWriter.close();

	    //
	    // Write the session files.
	    //
	    S2DSession.write(_sessionDir, S2DUtils.TYPE_CHEM_SHIFT_REF1, _name,
	      _frameIndex, "");

	    S2DSession.write(_sessionDir, S2DUtils.TYPE_CHEM_SHIFT_REF2, _name,
	      _frameIndex, "");

	    S2DSession.write(_sessionDir, S2DUtils.TYPE_CHEM_SHIFT_REF3, _name,
	      _frameIndex, "");

	    //
	    // Write the session-specific html files.
	    //
	    S2DSpecificHtml.write(_summary.getHtmlDir(),
	      S2DUtils.TYPE_CHEM_SHIFT_REF1, _name, _frameIndex,
	      "Chemical Shift Reference Difference Histograms");

	    S2DSpecificHtml.write(_summary.getHtmlDir(),
	      S2DUtils.TYPE_CHEM_SHIFT_REF2, _name, _frameIndex,
	      "Chemical Shift Differences by Residue");

	    S2DSpecificHtml.write(_summary.getHtmlDir(),
	      S2DUtils.TYPE_CHEM_SHIFT_REF3, _name, _frameIndex,
	      "Observed vs. Calculated Chemical Shift Values");

	    //
	    // Write the link in the summary html file.
	    //
	    _summary.startFrame("Chemical Shift Referencing Visualizations" +
	      " (calculated chem shifts from PDB " + _pdbId + ")");
	    // We assume that if doProteinCheck is false, we're in Jafar mode.
	    _summary.writeChemShiftRef(_frameIndex, !doProteinCheck);
	    _summary.endFrame();

	} catch (Exception ex) {
	    System.err.println("Exception in chem shift reference " +
	      "postprocessing: " + ex.toString());
	    // Note: the S2DWarning object below is *not* supposed to be
	    // thrown...
	    new S2DWarning("Unable to complete chem shift reference " +
	      "post-processing for <" + _cmdStr + ">");
	    System.err.println("Unable to complete chem shift reference " +
	      "post-processing for <" + _cmdStr + ">");
	}
    }

    public boolean ran()
    {
        return _csrRan;
    }

    public boolean ranButFailed()
    {
        return _ranButFailed;
    }

    //===================================================================
    // PRIVATE METHODS
    private void checkFileVersions() throws S2DException
    {
        if (doDebugOutput(11)) {
	    System.out.println("S2DChemShiftRef.checkFileVersions()");
        }

	S2DCSRFileData csrData = new S2DCSRFileData();
	for (int index = 1; index <= 5; ++index) {
	    if (csrData.getCSRData(_name, _frameIndex, index, _dataDir)) {
	        if (S2DUtils.compareVersions(csrData.fileVersion,
	          MIN_CHEM_SHIFT_VERSION) < 0) {
	            throw new S2DError("ChemShift version (" +
	              csrData.fileVersion + ") is too old -- must " +
	              "be at least " + MIN_CHEM_SHIFT_VERSION);
	        }
	    } else {
	        throw new S2DError("Unable to get ChemShift version");
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

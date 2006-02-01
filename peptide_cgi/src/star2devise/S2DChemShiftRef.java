// ========================================================================
// DEVise Data Visualization Software
// (c) Copyright 2003-2005
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
// Revision 1.2  2006/02/01 20:23:10  wenger
// Merged V2_1b4_br_0 thru peptide_cgi_10_8_0_base to the
// trunk.
//
// Revision 1.1.2.30.10.1  2005/05/19 16:07:43  wenger
// Merged nmrfam_mods2_br (argh -- must have forgotten to make
// nmrfam_mods2_br_0 tag!) thru nmrfam_mods2_br_3 to
// peptide_cgi_10_8_0_br.
//
// Revision 1.1.2.30.8.1  2005/05/12 19:07:40  wenger
// Merged nmrfam_mods_br_0 thru nmrfam_mods_br_1 to new
// nmrfam_mods2_br (created to get ambiguity visualization help
// and fix to coordinate visualization help).
//
// Revision 1.1.2.30.6.1  2005/05/12 14:10:12  wenger
// Peptide-CGI now allows non-numeric BMRB IDs; changed test3 to make
// sure cache is used when it should be; added test26 to test non-
// numeric BMRB ID.
//
// Revision 1.1.2.30  2004/12/21 22:59:11  wenger
// Fixed bug where I was using data_dir instead of csr_data_dir at
// one point in the chem shift ref stuff; changed dummy_csr_server
// for test6 to catch this error; changed vsc so that default args
// can be overridden.
//
// Revision 1.1.2.29  2004/12/20 20:02:17  wenger
// Help changes as requested by Eldon; changed minimum ChemShift version
// to 1.4.1 (filename changes in csr_wrapper script); changed s2d version
// to 10.5.2.
//
// Revision 1.1.2.28  2004/12/16 22:01:55  wenger
// Chem shift reference link-only processing option is pretty much
// implemented -- the actual csr_wrapper script on leech needs some
// mods, and I need to test Peptide-CGI with the real ChemShift
// software, but other than that I think it's done.
//
// Revision 1.1.2.27  2004/12/14 19:00:24  wenger
// Minor improvements to chem shift ref error messages.
//
// Revision 1.1.2.26  2004/12/14 18:02:34  wenger
// Fixed a bug that caused PDB IDs to sometimes get put into the
// chem shift ref list twice -- caused extra work when trying to
// do chem shift ref based on PDB IDs in the BMRB entry; we also
// delete the error file in S2DChemShiftRef to be safer.  Added
// test22 and test23 to test chem shift ref processing, made improvements
// to dummy_csr_server.  Set shorter chem shift ref timeout in s2d.
//
// Revision 1.1.2.25  2004/12/08 23:07:17  wenger
// Oops -- re-fixed deltashift session template; added titles to
// specific html pages; enlarges the JavaScreen.
//
// Revision 1.1.2.24  2004/12/07 19:02:55  wenger
// Added help info to all visualization help pages (pending approval
// from Eldon); better view help in the coordinate session template;
// fixed Y axis label fonts (send to JS) in the deltashift session
// template.
//
// Revision 1.1.2.23  2004/12/03 17:18:34  wenger
// Added BMRB accession number, PDB ID (if relevant), timestamp and
// Peptide-CGI version number to Peptide-CGI visualizations.
//
// Revision 1.1.2.22  2004/11/18 19:33:17  wenger
// Added more descriptive chem shift reference links in Jafar mode,
// since the section headers don't show up in the Jafar GUI.
//
// Revision 1.1.2.21  2004/11/18 19:10:34  wenger
// Peptide-CGI now checks the version of the ChemShift software, since
// the current version of Peptide-CGI needs at least ChemShift 1.4.0.
//
// Revision 1.1.2.20  2004/11/16 23:51:28  wenger
// Chem shift ref session #1 changed to have helix & sheet in the same
// view (pile); sample chem shift ref data changed to reflect changes
// in the ChemShift program -- HA2 and HA3 are ignored, CA and CB
// averages include all residues (not just ones that are helices and
// sheets), data for CSR #1 have helix and sheet averages, etc., together
// instead of separate; more changes to the chem shift ref help.
//
// Revision 1.1.2.19  2004/11/15 19:44:31  wenger
// All NMR Browser visualization help now goes to a single html window,
// instead of popping up a new window each time; better layout for
// chem shift reference stuff in the summary page; fixed a problem in
// the CSR #3 help page.
//
// Revision 1.1.2.18  2004/11/12 21:02:20  wenger
// Implemented most of the latest chem shift reference visualization
// change requests (still needs changes in the actual chemShift
// program).
//
// Revision 1.1.2.17  2004/09/13 16:06:30  wenger
// Added BMRB ID to chem shift ref session info.
//
// Revision 1.1.2.16  2004/09/10 22:38:23  wenger
// Added X=Y line (as requested by Eldon) to chem shift reference
// visualization #3.
//
// Revision 1.1.2.15  2004/09/01 19:35:38  wenger
// Temporarily disabled chem shift reference visualization 3,
// pending approval by Eldon; I need to do a bug fix release.
//
// Revision 1.1.2.14  2004/06/23 17:39:25  wenger
// Preliminary version of chem shift reference visualization 3 in
// place; cleaned up 4317 chem shift ref test data; changed fonts
// in chem shift ref visualization 1.
//
// Revision 1.1.2.13  2004/03/04 22:42:33  wenger
// Changed chem shift ref/PDB message as requested by Eldon.
//
// Revision 1.1.2.12  2004/03/04 22:14:31  wenger
// Added PDB ID to chem shift reference visualizations, because of
// the re-try code in Peptide-CGI (we don't for sure know ahead of
// time which PDB ID the chem shift ref calculations will be based
// on).
//
// Revision 1.1.2.11  2004/02/06 01:48:57  wenger
// We now re-try the chem shift reference processing with subsequent
// PDB IDs if it fails (not times out) with the first one.
//
// Revision 1.1.2.10  2004/01/30 18:20:41  wenger
// S2D errors and warnings are now saved to files (the files are
// named according to the data name).
//
// Revision 1.1.2.9  2004/01/26 18:55:36  wenger
// Incorporated "Eldon's" chemical shift reference session; added help
// pages for both chem shift ref visualizations (with no help info
// for now).
//
// Revision 1.1.2.8  2003/11/18 00:17:53  wenger
// Changed chem shift ref session to display diffs instead of random
// coil values (as per discussion with Gabriel); added test for
// tolerating bad PDB ID; improved chem shift ref error messages.
//
// Revision 1.1.2.7  2003/11/13 16:09:56  wenger
// More changes to the latest chem shift reference code as a result of
// testing on leech.
//
// Revision 1.1.2.6  2003/11/12 17:24:29  wenger
// S2d no longer uses do_csr.bat scripts -- all that stuff is now directly
// done in Java, to get around the problem that DOS batch files don't work
// right when run from within Java.  (New code is not yet fully tested.)
//
// Revision 1.1.2.5  2003/11/10 22:03:20  wenger
// Various partly-tested changes to try to get chem shift ref integration
// working on the NMRFAM end.
//
// Revision 1.1.2.4  2003/10/30 16:57:13  wenger
// Added timeout on chemical shift reference processing.
//
// Revision 1.1.2.3  2003/10/29 18:52:26  wenger
// Chem shift reference no longer uses Process.waitFor(), since that
// seems not to work on some platforms.
//
// Revision 1.1.2.2  2003/10/29 17:14:42  wenger
// Star2devise now can do chem shift ref processing with Jafar-format
// input files.
//
// Revision 1.1.2.1  2003/10/23 18:08:56  wenger
// Basic chem shift ref stuff is in place -- session and specific html
// files are generated, summary html file has link, etc.  The main thing
// left is the mechanics of calling Dan's chem shift ref software --
// right now I am just using a script that creates dummy chem shift
// ref data files.
//

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
        if (DEBUG >= 1) {
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
        if (DEBUG >= 1) {
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
            if (DEBUG >= 2) {
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
        if (DEBUG >= 1) {
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
        if (DEBUG >= 1) {
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
}

// ========================================================================

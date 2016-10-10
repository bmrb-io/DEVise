// ========================================================================
// DEVise Data Visualization Software
// (c) Copyright 2013-2015
// By the DEVise Development Group
// Madison, Wisconsin
// All Rights Reserved.
// ========================================================================

// Under no circumstances is this software to be copied, distributed,
// or altered in any way without prior permission from the DEVise
// Development Group.

// ------------------------------------------------------------------------

// This class deals with peak list data.  There are two main functions:
//   1) Writing out peak list data in appropriate tags for entries that
//      only have the data in raw text format;
//   2) Writing out DEVise data files, session files, etc. for peak
//      list visualizations.
//
// Note that this class can handle peak dimensions from 2 to 4 -- anything
// outside that range will fail.

//TEMP -- need to preserve _Spectral_dim loops, maybe use them to index into the dimensions of the peak list data -- but numbering doesn't orrespond?  Is this adequately taken care of?

// ------------------------------------------------------------------------

// $Id$

// $Log$
// Revision 1.7  2014/11/23 22:29:30  wenger
// Did Peptide-CGI to-do 210:  Processing of Sparky peak lists without
// heights/volumes.
//
// Revision 1.6  2014/07/30 21:47:12  wenger
// Merged s2d_todo207_br_0 thru s2d_todo207_br_1 to trunk.
//
// Revision 1.5.2.4  2014/07/29 18:28:08  wenger
// Peak lists:  Finished adding _Spectral_dim loops when they don't
// exist but can be derived from the peak list text.
//
// Revision 1.5.2.3  2014/07/25 21:40:24  wenger
// Peak lists:  more work on adding _Spectral_dim loops when they're
// missing (we now output atom type and isotope); improved peak list
// tests accordingly.
//
// Revision 1.5.2.2  2014/06/10 23:07:20  wenger
// Generating _Spectral_dim loops from NmrView peak text is now mostly
// working.
//
// Revision 1.5.2.1  2014/06/09 21:04:00  wenger
// Peak list processing now reports a fatal error if there is no
// _Spectral_dim information.
//
// Revision 1.5  2014/05/08 20:01:08  wenger
// Peptide-CGI to-do 206:  multiple sets of peaks in a single save frame
// now cause an explicit fatal error.
//
// Revision 1.4  2014/04/11 21:49:12  wenger
// Merged nmrview_peaks_br_0 thru nmrview_peaks_br_1 to trunk.
//
// Revision 1.3.2.11  2014/04/11 18:28:30  wenger
// Peak list processing now works for 2D NMRView peak lists; improved
// peak list-related error and warning messages.
//
// Revision 1.3.2.10  2014/03/12 16:54:24  wenger
// NMRView peak list processing:  made a bunch of cleanups for a
// preliminary release (still need to add assignment checks to the
// relevant tests).
//
// Revision 1.3.2.9  2014/03/07 20:50:15  wenger
// NMRView peak list processing now includes chemical shifts and assignments
// (need to do a bunch of checking on this; also, assignments need to be
// converted from the author residue numbers to the BMRB residue numbers).
//
// Revision 1.3.2.8  2014/03/06 18:11:44  wenger
// NMRView peak numbers are now based on the peak numbers in the
// peak text.
//
// Revision 1.3.2.7  2014/03/05 23:13:13  wenger
// NMRView peak list processing can now handle 3 or 4 dimensions -- added
// test_peak19 and corresponding data.
//
// Revision 1.3.2.6  2014/03/04 17:55:57  wenger
// Made the NMRView peak list processing more flexible -- test_peak18
// now works.
//
// Revision 1.3.2.5  2014/03/01 00:40:09  wenger
// Added output checking for test_peak16; slight mods to code to get
// peak numbers right.
//
// Revision 1.3.2.4  2014/02/19 20:18:33  wenger
// Added a new NmrView peak list file generated "manually" from entry
// 11036 and the associated .xpk file; many of the lines fail the current
// parsing code.
//
// Revision 1.3.2.3  2014/02/19 19:15:52  wenger
// We now output peak volumes for NmrView peak lists; also fixed a typo
// in the Xeasy code (didn't affect function).
//
// Revision 1.3.2.2  2014/02/19 16:42:09  wenger
// First version of pattern that recognizes NMRView peak list data lines.
//
// Revision 1.3.2.1  2014/02/17 22:43:17  wenger
// Split S2DPeakList.getPeakTextValues() into several smaller methods
// to make things cleaner as I add support for NMRView peak lists.
//
// Revision 1.3  2014/02/06 22:52:16  wenger
// Got rid of empty loops in the peak list output.
//
// Revision 1.2  2014/01/14 23:10:12  wenger
// Merged peak_lists2_br_0 thru peak_lists2_br_3 to trunk.
//
// Revision 1.1.4.13  2014/01/14 01:09:22  wenger
// Peak lists:  More cleanups -- I think this is now ready to merge to
// the trunk.
//
// Revision 1.1.4.12  2014/01/12 03:40:52  wenger
// Peak lists:  Various cleanup to code and tests.
//
// Revision 1.1.4.11  2014/01/10 00:32:34  wenger
// Peak lists:  Started creating a simple DEVise peak list visualization
// -- not very far along yet.  I want to clean things up and merge to the
// trunk before I do much more on this.
//
// Revision 1.1.4.10  2013/11/19 21:35:35  wenger
// Peptide-CGI peak lists:  Minor updates; hopefully ready for another
// peak list release.
//
// Revision 1.1.4.9  2013/11/19 20:52:57  wenger
// Peptide-CGI peak lists:  Sparky lines with notes at the end now work;
// test_peak14 and test_peak15 have checks of output.
//
// Revision 1.1.4.8  2013/11/15 23:17:51  wenger
// Peptide-CGI peak lists:  Sparky lines of "?-?-?", etc., now match;
// tests fixed accordingly.
//
// Revision 1.1.4.7  2013/11/14 16:43:37  wenger
// Peptide-CGI peak lists:  fixed code and tests for unassigned peaks in
// Xeasy (peak index = 0).
//
// Revision 1.1.4.6  2013/11/13 23:43:09  wenger
// Peptide-CGI peak lists:  made improvments to a bunch of the peak list
// tests to make sure upcoming fixes don't break existing functionality.
// (Note:  test_peak13 with new checks worked at merged-peak_lists_br_0-thru-peak_lists_br_2-to-peak_lists2_br but fails
// at the current end of the branch.
//
// Revision 1.1.4.5  2013/11/07 20:33:45  wenger
// Peptide-CGI peak lists:  we now allow multiple entity assemblies if
// only one is a polymer; changed "xxx" to "?" for unidentified amino
// acids.
//
// Revision 1.1.4.4  2013/10/29 16:01:36  wenger
// Partially fixed test_peak14 bug -- if we can't figure out the three-
// letter amino acid codes we just output "xxx".
//
// Revision 1.1.4.3  2013/10/28 22:27:29  wenger
// Fixed the test_peak15 bug -- non-existent atoms are "0" in the input
// file instead of the column not existing.  (Debug code still in place,
// test needs checks.)
//
// Revision 1.1.4.2  2013/10/22 19:45:09  wenger
// Merged peak_lists_br_0 thru peak_lists_br_2 to peak_lists2_br.
//
// Revision 1.1.2.45  2013/09/16 18:59:04  wenger
// First version of outputting "real" DEVise peak list data files (needs
// a bunch of cleanup).
//
// Revision 1.1.2.44  2013/08/28 22:41:15  wenger
// Split up writePeakList() into separate methods for NMR-STAR and
// DEVise output.
//
// Revision 1.1.2.43  2013/08/22 19:47:52  wenger
// Cleaned up ambiguous assignment code, added corresponding checks to
// peak tests.
//
// Revision 1.1.2.42  2013/08/21 23:40:42  wenger
// Okay, parsing of ambiguous Sparky assignments seems to be working
// -- committing current version w/o cleanup.
//
// Revision 1.1.2.41  2013/08/21 20:53:35  wenger
// More clean up of the peak list code, related to ambiguous assignments
// -- now we just have to actually parse the atom strings...
//
// Revision 1.1.2.40  2013/08/20 22:32:48  wenger
// Peak list tests now all work with new internal data structures to
// handle ambiguous assignments.  Lots of cleanup still needed, though,
// plus actual parsing of ambiguous Sparky assignments.
//
// Revision 1.1.2.39  2013/08/06 17:17:35  wenger
// 2D and 4D xeasy parsing now working.
//
// Revision 1.1.2.38  2013/08/05 22:49:15  wenger
// Added extra loop columns as per request from Hongyang.
//
// Revision 1.1.2.37  2013/07/30 22:39:37  wenger
// Better error messages if the peak processing doesn't work.
//
// Revision 1.1.2.36  2013/07/30 20:32:11  wenger
// 4D Sparky parsing at least mostly working (needs fix for 16275,
// which doesn't have peak intensities(!); added new tests for 2D
// and 4D peak lists.
//
// Revision 1.1.2.35  2013/07/26 21:34:25  wenger
// Fixed peak list assignment tag order.
//
// Revision 1.1.2.34  2013/07/26 20:51:39  wenger
// Added bmrb_share configuration; removed comment line from peak output
// file.
//
// Revision 1.1.2.33  2013/07/25 21:50:23  wenger
// Various peak list cleanups; added test_peak9 to test conversion on
// a format we don't recognize.
//
// Revision 1.1.2.32  2013/07/25 18:11:19  wenger
// 1-letter to 3-letter amino acid translation working for peak lists.
//
// Revision 1.1.2.31  2013/07/24 20:42:28  wenger
// All peak save frame values except the peak list text tag are now copied
// to the output file.
//
// Revision 1.1.2.30  2013/07/18 21:42:37  wenger
// Cleaned up the spectral dim code.
//
// Revision 1.1.2.29  2013/07/18 19:41:34  wenger
// We now copy the _Spectral_dim loop in to the peak list output (still
// needs cleanups).
//
// Revision 1.1.2.28  2013/07/16 22:54:56  wenger
// Fixed up the frame details for peak lists.
//
// Revision 1.1.2.27  2013/07/12 20:54:21  wenger
// Cleaned up a bunch of the no-longer-used peak list code.
//
// Revision 1.1.2.26  2013/07/12 19:25:27  wenger
// We now output the new _Assigned_peak_chem_shift.Auth_* tags (cleanup
// still needed).
//
// Revision 1.1.2.25  2013/07/12 17:19:23  wenger
// Updated some configuration for RHEL6; notes about peak stuff from
// meeting with Eldon.
//
// Revision 1.1.2.24  2013/06/21 21:58:45  wenger
// More cleanup of assignment code.
//
// Revision 1.1.2.23  2013/06/21 21:03:13  wenger
// Okay, Cyana/xeasy assignments basically work now -- still need a bunch
// of cleanup.
//
// Revision 1.1.2.22  2013/06/20 22:44:03  wenger
// Working on Cyana/xeasy assignments -- reading chem shifts for atom
// assignments is mostly done (note that it only works if there's just
// one save frame of chem shifts -- if we have more than one, I don't
// know how to pick which one to use).
//
// Revision 1.1.2.21  2013/06/20 20:39:00  wenger
// Dummy assignments for Cyana/xeasy implemented -- we need to read the
// chem shift save frame to make them work for real...
//
// Revision 1.1.2.20  2013/06/20 18:28:05  wenger
// Fixed crash for entries that don't have assignments.
//
// Revision 1.1.2.19  2013/06/20 17:41:51  wenger
// Added master peak table to output.
//
// Revision 1.1.2.18  2013/06/06 22:05:21  wenger
// Parsing Sparky assignments and printing them is now working (except
// for a few lingering issues, such as entity assembly IDs).
//
// Revision 1.1.2.17  2013/06/06 18:17:02  wenger
// Most of the way to parsing Sparky assignments; added
// S2DUtils.string2Int() -- I'm committing now because I'm going to change
// a bunch of the code to use string2Int and string2Double.
//
// Revision 1.1.2.16  2013/06/05 23:21:51  wenger
// Fixed a problem in the main Sparky pattern; most of the way to parsing
// Sparky assignments.
//
// Revision 1.1.2.15  2013/05/31 18:55:29  wenger
// Cleaned up the peak list code somewhat.
//
// Revision 1.1.2.14  2013/05/31 17:19:20  wenger
// Peak intensity values and chem shifts (not tied to specific atoms)
// now work for the Cyana/xeasy format.
//
// Revision 1.1.2.13  2013/05/31 16:06:58  wenger
// We now dump out chem shifts in the _Peak_char tag "family" to the
// NMR-STAR peak file (for Sparky input).
//
// Revision 1.1.2.12  2013/05/21 17:44:26  wenger
// We now produce NMR-STAR output when matching the first pattern.
//
// Revision 1.1.2.11  2013/05/20 23:53:01  wenger
// We now partially create the NMR-STAR output containing the peak lists.
//
// Revision 1.1.2.10  2013/05/16 21:05:07  wenger
// Added -peakonly flag to s2d; started on code to get peak values from
// individual tags, and restructuring for -peakonly or "normal" modes;
// added some new peak list tests.
//
// Revision 1.1.2.9  2013/05/14 15:38:40  wenger
// Cleaned up pattern matching stuff.
//
// Revision 1.1.2.8  2013/05/13 23:04:57  wenger
// Patterns now match all peak tests -- still need a little cleanup.
//
// Revision 1.1.2.7  2013/05/13 03:28:18  wenger
// Improved patterns (test_peak3 still doesn't match); improved printing
// of group info.
//
// Revision 1.1.2.6  2013/05/10 22:03:34  wenger
// Debug option to print non-matching peak lines; working on fixing patterns
// to match stuff that doesn't match yet.
//
// Revision 1.1.2.5  2013/05/10 21:37:48  wenger
// Added pat2 to match the second type of peak list.
//
// Revision 1.1.2.4  2013/05/10 16:00:05  wenger
// Cleaned up pat1 stuff.
//
// Revision 1.1.2.3  2013/05/09 22:20:54  wenger
// Came up with a pattern that seems to recognize the first time of
// peak list lines.
//
// Revision 1.1.2.2  2013/05/07 15:33:48  wenger
// We now print out the peak text value for testing; added more peak tests.
//
// Revision 1.1.2.1  2013/04/22 02:54:15  wenger
// Added preliminary version of S2DPeakList class.
//

// ========================================================================

package star2devise;

import java.io.*;
import java.util.*;
import EDU.bmrb.starlibj.SaveFrameNode;
import EDU.bmrb.starlibj.StarUnparser;
import java.util.regex.Pattern;
import java.util.regex.Matcher;

public class S2DPeakList {
    //===================================================================
    // VARIABLES

    private static final int DEBUG = 0;
    private static final int MAX_DIMS = 4;

    private String _name;
    private String _longName;
    private S2DNmrStarIfc _star;
    private String _dataDir;
    private String _sessionDir;
    private S2DSummaryHtml _summary;
    private String _frameDetails;
    private String _frameName;

    private String _starVersion;
    private boolean _peakOnly;

    // True iff we got peak values from individual tags, not the peak
    // text field.
    private boolean _gotTagVals = false;

    // Patterns for matching peak list text lines.
    Pattern _sparkyPat;
    Pattern _xeasyHdrPat;
    Pattern _xeasyPat;
    Pattern _nmrviewPat1;
    Pattern _nmrviewPat2;
    // 1 -> processing NmrView header; 0 otherwise.
    int _nmrviewState = 0;

    // True iff we got peak values from the peak text field (in other
    // words, we successfully matched lines in the peak text field
    // to our expected formats).
    private boolean _gotTextVals = false;

    private int _maxPeakId = 0;

    private static class PeakInfo {
        public int _peakId;
        public double _inten = Double.NaN;
        public String _method;
        // Size of array depends on # of shifts (2-4).
        public double[] _shifts;
        public PeakAssignment _assignments;
    }
    private ArrayList _peaks = new ArrayList();

    private boolean _haveSpectralDimLoop = false;
    private String[] _spectralDimRegions = new String[MAX_DIMS];

    private static class PeakAssignment {
        // Note:  If we have fewer than four dimensions, some of these
        // will be null.
        public AtomAmbigSet[] _atomSets;

        public PeakAssignment() {
            // One AtomAmbigSet for each possible dimension.
            _atomSets = new AtomAmbigSet[MAX_DIMS];
        }

    }

    // There is one AmbigSet object for each dimension of the
    // peak.  The _atoms list contains multiple atoms only if
    // the assignment is ambiguous.
    private static class AtomAmbigSet {
        // Multiple AtomInfos here for ambiguous assignments.
        public ArrayList _atoms; // array of AtomInfo objects

        public AtomAmbigSet() {
            _atoms = new ArrayList();
        }
    }

    public static class AtomInfo {
        // Note that some members will retain the default values (Sparky
        // and xeasy assignments set different members).
        // public int _entAssemId = 0;
        // public int _entityId = 0;
        public String _authAtomNum = ".";
        public String _aminoAcid = ".";
        public int _residueNum = 0;
        public String _atomName = ".";
    }

    private S2DResidues _residues;
    private File _tmpPeakFrameFile;

    //===================================================================
    // PUBLIC METHODS

    //-------------------------------------------------------------------
    // Constructor.
    //TEMP -- probably don't need all of this stuff, at least until DEVise peak list visualizations are working
    public S2DPeakList(String name, String longName, S2DNmrStarIfc star,
                       SaveFrameNode frame, String dataDir, String sessionDir,
                       S2DSummaryHtml summary, boolean peakOnly, boolean allowSDMismatch)
    throws S2DException
    {
        if (doDebugOutput(11)) {
            System.out.println("S2DPeakList.S2DPeakList(" + name +
                               ", " + star.getFrameName(frame) + ")");
        }

        _name = name;
        _longName = longName;
        _star = star;
        _dataDir = dataDir;
        _sessionDir = sessionDir;
        _summary = summary;
        _frameDetails = star.getFrameDetails(frame);
        _starVersion = star.version();
        _frameName = star.getFrameName(frame);
        _peakOnly = peakOnly;

        _spectralDimRegions[0] = "";
        _spectralDimRegions[1] = "";
        _spectralDimRegions[2] = "";
        _spectralDimRegions[3] = "";

        //
        // See if we already have tags with the peak list values.
        //
        try {
            getPeakTagValues(frame);
            _gotTagVals = true;
            if (doDebugOutput(11)) {
                System.out.println("Got peak values from individual tags");
            }

        } catch (S2DException ex) {
            if (doDebugOutput(20)) {
                System.out.println("Peak list tags not found " +
                                   ex.toString());
            }

            getResidues();

            //
            // Get the values we need from the peak text string.
            //
            getPeakTextValues(frame);
        }

        String[] spectralDims = null;
        try {
            // Note:  If we find the _Spectral_dim.ID loop here, we just
            // copy that loop verbatim to the output file later on.
            spectralDims = star.getFrameValues(frame,
                                               star.SPEC_DIM_ID, star.SPEC_DIM_REGION);
            _haveSpectralDimLoop = true;
        } catch(S2DError err) {
            // If we found any spectral dim values in the peak list text,
            // _spectralDimRegions[0] will not be blank.
            if (_spectralDimRegions[0].equals("")) {
                throw new S2DError("Error: no _Spectral_dim data in save frame " +
                                   star.getFrameName(frame) + " (" + err.toString() + ")");
            }
        }

        // Check whether spectral dim info from the peak list text and
        // from the existing Spectral_dim loop agree, if we have both.
        if (_haveSpectralDimLoop) {
            boolean error = false;
            for (int dim = 0; dim < MAX_DIMS; ++dim) {
                if (dim >= spectralDims.length) {
                    if (_spectralDimRegions[dim].equals("")) {
                        continue;
                    } else {
                        error = true;
                        break;
                    }
                }
                if (!_spectralDimRegions[dim].equals("") &&
                        !_spectralDimRegions[dim].equals(spectralDims[dim])) {
                    error = true;
                    break;
                }
            }
            if (error) {
                System.err.println("Spectral dim mismatch:");
                for (int dim = 0; dim < MAX_DIMS; ++dim) {
                    System.out.println("  _spectralDimRegions[" + dim + "]: " +
                                       _spectralDimRegions[dim]);
                    if (dim < spectralDims.length) {
                        System.out.println("  spectralDims[" + dim + "]: " +
                                           spectralDims[dim]);
                    }
                }
                if (!allowSDMismatch) {
                    throw new S2DError("Spectral dim mismatch");
                }
            }
        }

        // If we got peak values in one of the two places above,
        // write the info from the peak save frame (except for the
        // peak text) to a temp file.  (Note that this will include
        // copying the _Spectral_dim.ID loop.)
        writePeakSaveFrame(frame);
    }

    //-------------------------------------------------------------------
    // Write the peak list values for this data.
    public void writePeakList(int frameIndex) throws S2DException
    {
        if (doDebugOutput(11)) {
            System.out.println("S2DPeakList.writePeakList()");
        }

        try {
            if (_peakOnly) {
                writePeakTags(frameIndex);

            } else {
                // if (_gotTagVals) {//TEMP?
                writeDevisePeaks(frameIndex);
                // }
            }

        } catch(IOException ex) {
            System.err.println("IOException writing peak list data: " +
                               ex.toString());
            throw new S2DError("Can't write peak list data");
        }
    }

    //-------------------------------------------------------------------
    /**
     * Add peak list sets to the data set list.
     * @param The data set list.
     * @param The frame index.
     */
    public void addPeakList(Vector dataSets, int frameIndex,
                            int polymerType)
    {
        /*TEMP -- for DEVise visualization
                // Note: attribute names must match the bmrb-s2order schema.
        	String dataSource = _name + S2DNames.ORDER_SUFFIX + frameIndex;
        	String dataName = "S2 Order Parameters [" + _entityAssemblyID + "]";
        	dataSets.addElement(new S2DDatasetInfo(dataName, _frameDetails,
        	  _sample, _sampleConditions, dataSource, "S2order_value",
        	  "bmrb-s2", "s2", _entityAssemblyID, polymerType));
        TEMP*/
    }

    //===================================================================
    // PRIVATE METHODS

    //-------------------------------------------------------------------
    // Write the peak list save frame data (except for the peak list
    // text tag) to a temporary file.
    private void writePeakSaveFrame(SaveFrameNode frame)
    throws S2DException
    {
        try {
            // Remove the peak list text entry -- we don't want that in
            // the output.
            S2DStarIfc.removeTag(frame, _star.PEAK_LIST_TEXT);

        } catch (S2DException ex) {
            throw new S2DError("Can't remove peak text tag: " + ex.toString());
        }

        try {
            _tmpPeakFrameFile = File.createTempFile("peaks", ".str",
                                                    new File("tmp"));
            if (doDebugOutput(20)) {
                System.out.println("_tmpPeakFrameFile: " + _tmpPeakFrameFile);
            } else {
                _tmpPeakFrameFile.deleteOnExit();
            }
            FileOutputStream fos = new FileOutputStream(_tmpPeakFrameFile);

            StarUnparser unp = new StarUnparser(fos);
            unp.writeOut(frame, 1);

            fos.close();

        } catch (IOException ex) {
            throw new S2DError("Can't unparse save frame: " + ex.toString());
        }
    }

    //-------------------------------------------------------------------
    // Get an S2DResidues object for this entry -- we will use that for
    // the 1-letter to 3-letter amino acid conversions.
    private void getResidues() throws S2DException
    {
        if (doDebugOutput(20)) {
            System.out.println("S2DPeakList.getResidues()");
        }

        try {
            // Note:  if there's more than one entity assembly save
            // frame we consider that an error unless only one of them
            // is for a polymer -- in that case we use that save
            // frame's residue list for the peak list.
            SaveFrameNode eaFrame = null;
            Vector frames = _star.getAllEntityAssemblyFrames();
            if (frames.size() != 1) {
                for (int index = 0; index < frames.size(); index++) {
                    SaveFrameNode tmpFrame = (SaveFrameNode)frames.elementAt(index);
                    int polymerType = _star.getPolymerType(tmpFrame);
                    if (polymerType == S2DResidues.POLYMER_TYPE_PROTEIN ||
                            polymerType == S2DResidues.POLYMER_TYPE_DNA ||
                            polymerType == S2DResidues.POLYMER_TYPE_RNA) {
                        if (eaFrame != null) {
                            throw new S2DError("Error (peak list): found more than one " +
                                               "entity assembly frame for a polymer; don't " +
                                               "know which one to use for peak list");
                        } else {
                            eaFrame = tmpFrame;
                        }
                    }
                }
            } else {
                eaFrame = (SaveFrameNode)frames.elementAt(0);
            }
            _residues = _star.getResidues(eaFrame, 1);

        } catch(S2DException ex) {
            System.err.println("Error (peak list) getting residues: " +
                               ex.toString());
        }
    }

    //-------------------------------------------------------------------
    // Get peak list values from individual tags (not the
    // _Spectral_peak_list.Text_data tag).
    // Returns true if successful, false otherwise.
    private void getPeakTagValues(SaveFrameNode frame) throws S2DException
    {
        if (doDebugOutput(20)) {
            System.out.println("S2DPeakList.getPeakTagValues()");
        }

        String[] peakIds = _star.getFrameValues(frame,
                                                _star.PEAK_LIST_ID, _star.PEAK_LIST_ID);
        //TEMP -- need to get more tag values, and store stuff into data structures
    }

    //-------------------------------------------------------------------
    // Get peak list values from the _Spectral_peak_list.Text_data tag.
    // Returns true if successful, false otherwise.
    // TEMP -- should I be using
    // _Spectral_peak_list.Number_of_spectral_dimensions? (hmm -- for
    // some peak list save frames, it's ".")
    private void getPeakTextValues(SaveFrameNode frame) throws S2DException
    {
        if (doDebugOutput(20)) {
            System.out.println("S2DPeakList.getPeakTextValues()");
        }

        boolean result = false;

        //
        // Get the values we need from the Star file.
        //
        String peakListText = "";
        try {
            peakListText = _star.getTagValue(frame, _star.PEAK_LIST_TEXT);
        } catch (S2DException ex) {
            S2DError err = new S2DError("Error getting peak text value: " +
                                        ex.toString());
            System.err.println(err);
            throw err;
        }

        //
        // Parse the peak text value.
        //
        createPatterns();

        int peakId = 1;

        String lines[] = peakListText.split("\n");
        for (int index = 0; index < lines.length; index++) {
            if (doDebugOutput(24)) {
                System.out.println("line: " + lines[index]);
            }

            try {
                if (_nmrviewState > 0) {
                    processNmrViewHdrLine(lines[index]);
                } else if (matchLine(lines[index], peakId)) {
                    ++peakId;
                }
            } catch(Exception ex) {
                System.err.println("Warning (peak list): exception (" + ex.toString() +
                                   " processing line " + lines[index]);
            }
        }

        if (!_gotTextVals) {
            System.err.println(
                "Warning (peak list):  no peak list pattern matches found in peak " +
                "text for save frame " + _frameName +
                "; probably an unsupported peak format");
        }
    }

    //-------------------------------------------------------------------
    private void createPatterns()
    {
        String ws = "\\s*";
        // Note:  spaces are part of floatPat and so on to make it easier
        // to set things up for a variable number of repetitions.
        String floatPat = "(-?\\d+\\.\\d+\\s+)";
        String intPat = "(-?\\d+\\s+)";
        String alphaPat = "([a-zA-Z\\?-]\\s+)";
        String expPat = "((-?\\d*\\.\\d*[eE][\\+-]\\d+\\s+)|(0\\s+))";
        String assignAtom = "\\w*(/\\w*)*"; // allows ambiguous assignments
        String assignPat = "(" + assignAtom + "-" + assignAtom + "(-" +
                           assignAtom + ")?(-" + assignAtom + ")?\\s+)";

        //TEMP -- add examples of matching input lines to the code

        // Regex pattern for Sparky.
        // Note:  entry 16647 will match this pattern.
        _sparkyPat = Pattern.compile(ws + assignPat + "?" +
                                     floatPat + floatPat + floatPat + "?" + floatPat + "?" +
                                     intPat + "?" + ".*");
        if (doDebugOutput(20)) {
            System.out.println("_sparkyPat <" + _sparkyPat + ">");
        }

        // Regex patterns for Cyana/xeasy -- matches 2D, 3D, and 4D.
        // 2D example: test_peak10 (entry 15025).
        // 3D example: test_peak7 (entry 16576).
        // 4D example: test_peak13 (entry 16806).
        // Note: test_peak23 (entry 6792) gets the spectral dimensions
        // from the header.
        _xeasyHdrPat = Pattern.compile("#INAME" + ws + intPat +
                                       "(\\w+)" + ".*");
        if (doDebugOutput(20)) {
            System.out.println("_xeasyHdrPat <" + _xeasyHdrPat + ">");
        }

        _xeasyPat = Pattern.compile(ws + intPat +
                                    floatPat + floatPat + floatPat + "?" + floatPat + "?" + // chem shifts
                                    intPat + alphaPat +
                                    expPat + // peak volume
                                    expPat + alphaPat + intPat +
                                    intPat + intPat + intPat + "?" + intPat + "?" + // assignments
                                    "(-?\\d+)" + "?" + ".*");
        if (doDebugOutput(20)) {
            System.out.println("_xeasyPat <" + _xeasyPat + ">");
        }

        // Regex patterns for NMRView
        _nmrviewPat1 = Pattern.compile("label dataset sw sf\\s*");
        if (doDebugOutput(20)) {
            System.out.println("_nmrviewPat1 <" + _nmrviewPat1 + ">");
        }

        String qb = "(\\{\\S*}\\s+)";
        String nvAtomPat = "(" + qb + floatPat + floatPat +
                           floatPat + "(\\S+\\s+)" + floatPat + qb + ")";
        _nmrviewPat2 = Pattern.compile(ws + intPat + nvAtomPat +
                                       nvAtomPat + nvAtomPat + "?" + nvAtomPat + "?" +
                                       floatPat + floatPat +
                                       intPat + qb + "(-?\\d+)" +
                                       "(.*)");
        if (doDebugOutput(20)) {
            System.out.println("_nmrviewPat2 <" + _nmrviewPat2 + ">");
        }
    }

    //-------------------------------------------------------------------
    private Pattern _nmrViewHdrPat = Pattern.compile(
                                         "(\\w*)\\s*(\\w*)(\\s*(\\w*))?(\\s*(\\w*))?.*");

    private void processNmrViewHdrLine(String line)
    {
        if (doDebugOutput(22)) {
            System.out.println("S2DPeakList.processNmrViewHdrLine(" +
                               line + ")");
        }

        Matcher match1 = _nmrViewHdrPat.matcher(line);

        if (match1.matches()) {
            if (doDebugOutput(22)) {
                System.out.println("Matched NmrView header line <" +
                                   line + ">");
                printMatch(match1);
            }
        }

        _spectralDimRegions[0] = match1.group(1);
        _spectralDimRegions[1] = match1.group(2);
        _spectralDimRegions[2] = match1.group(4);
        _spectralDimRegions[3] = match1.group(6);

        _nmrviewState = 0; // Done with header...
    }

    //-------------------------------------------------------------------
    // Process one line of peak list text.
    // Returns true iff the line matches one of the patterns we're
    // looking for.
    private boolean matchLine(String line, int peakId)
    {
        boolean result = false;

        // Get rid of the "?-?-?", etc, in some Sparky lines.  Doing
        // it separately avoids adding more complication to the regex.
        String line2 = stripSparkyQs(line);

        Matcher sparkyMatch = _sparkyPat.matcher(line2);
        Matcher xeasyHdrMatch = _xeasyHdrPat.matcher(line2);
        Matcher xeasyMatch = _xeasyPat.matcher(line2);
        Matcher nmrviewMatch1 = _nmrviewPat1.matcher(line2);
        Matcher nmrviewMatch2 = _nmrviewPat2.matcher(line2);

        int matchCount = 0;
        if (sparkyMatch.matches()) {
            matchCount++;
            if (doDebugOutput(22)) {
                System.out.println("  _sparkyPat matches line: " +
                                   line);
            }
            processSparkyMatch(sparkyMatch, peakId);
            result = true;

        } else if (xeasyHdrMatch.matches()) {
            matchCount++;
            if (doDebugOutput(22)) {
                System.out.println("  _xeasyHdrPat matches line: " +
                                   line);
            }
            processXeasyHdrMatch(xeasyHdrMatch, peakId);

        } else if (xeasyMatch.matches()) {
            matchCount++;
            if (doDebugOutput(22)) {
                System.out.println("  _xeasyPat matches line: " +
                                   line);
            }
            processXeasyMatch(xeasyMatch, peakId);
            result = true;

        } else if (nmrviewMatch1.matches()) {
            matchCount++;
            if (doDebugOutput(22)) {
                System.out.println("  _nmrviewPat1 matches line: " +
                                   line);
            }
            ++_nmrviewState;
            result = true;

        } else if (nmrviewMatch2.matches()) {
            matchCount++;
            if (doDebugOutput(22)) {
                System.out.println("  _nmrviewPat2 matches line: " +
                                   line);
            }
            processNmrviewData(nmrviewMatch2);
            result = true;

        } else {
            if (doDebugOutput(20)) {
                System.out.println("No match for line: " + line);
            }
        }

        if (matchCount > 1) {
            S2DWarning warning = new S2DWarning("Warning (peak list): " +
                                                _frameName + "matched multiple patterns");
            System.out.println(warning.toString());
        }
        if (matchCount > 0) {
            _gotTextVals = true;
        }

        return result;
    }

    //-------------------------------------------------------------------
    // Strip off leading "?-?", "?-?-?", or "?-?-?-?" from some Sparky
    // lines.  We do it separately because I don't want to make the regex
    // more complicated...
    private String stripSparkyQs(String line)
    {
        //TEMP -- should I make all of these patterns class variables so
        //TEMP they only get compiled once per run?
        Pattern qPat = Pattern.compile("\\s*(\\?-\\?(-\\?){0,2})(.*)");
        Matcher qMatch = qPat.matcher(line);
        if (qMatch.matches()) {
            line = qMatch.group(3);
        }

        return line;
    }

    //-------------------------------------------------------------------
    // Get data from a Sparky peak list line.
    private void processSparkyMatch(Matcher sparkyMatch, int peakId)
    {
        if (doDebugOutput(23)) {
            System.out.println("processSparkyMatch()");
            printMatch(sparkyMatch);
        }

        if (!checkPeakId(peakId)) {
            return;
        }

        PeakInfo peakInfo = new PeakInfo();
        _peaks.add(peakInfo);
        peakInfo._peakId = peakId;
        if (sparkyMatch.group(12) != null) {
            peakInfo._inten =
                S2DUtils.string2Double(sparkyMatch.group(12));
        }
        peakInfo._method = "height";

        int shiftCount = 0;
        for (int groupNum = 8; groupNum <= 11; groupNum++) {
            if (sparkyMatch.group(groupNum) != null) {
                shiftCount++;
            }
        }
        peakInfo._shifts = new double[shiftCount];
        for (int shiftNum = 0; shiftNum < shiftCount; shiftNum++) {
            peakInfo._shifts[shiftNum] =
                S2DUtils.string2Double(sparkyMatch.group(shiftNum+8));
        }

        if (sparkyMatch.group(1) != null) {
            peakInfo._assignments =
                parseSparkyAssignments(sparkyMatch.group(1));
        }
    }

    //-------------------------------------------------------------------
    // Get data from a Cyana/xeasy header line.
    private void processXeasyHdrMatch(Matcher xeasyHdrMatch, int peakId)
    {
        if (doDebugOutput(23)) {
            System.out.println("processXeasyHdrMatch()");
            printMatch(xeasyHdrMatch);
        }

        int dim = S2DUtils.string2Int(xeasyHdrMatch.group(1).trim());

        _spectralDimRegions[dim-1] = xeasyHdrMatch.group(2);
    }

    //-------------------------------------------------------------------
    // Get data from a Cyana/xeasy peak list line.
    private void processXeasyMatch(Matcher xeasyMatch, int peakId)
    {
        if (doDebugOutput(23)) {
            System.out.println("processXeasyMatch()");
            printMatch(xeasyMatch);
        }

        if (!checkPeakId(peakId)) {
            return;
        }

        PeakInfo peakInfo = new PeakInfo();
        _peaks.add(peakInfo);
        peakInfo._peakId = peakId;
        peakInfo._inten = S2DUtils.string2Double(xeasyMatch.group(8));
        peakInfo._method = "volume";

        int shiftCount = 0;
        for (int groupNum = 2; groupNum <= 5; groupNum++) {
            if (xeasyMatch.group(groupNum) != null) {
                shiftCount++;
            }
        }
        peakInfo._shifts = new double[shiftCount];
        for (int shiftNum = 0; shiftNum < shiftCount; shiftNum++) {
            peakInfo._shifts[shiftNum] =
                S2DUtils.string2Double(xeasyMatch.group(shiftNum+2));
        }

        if (xeasyMatch.group(16) != null) {
            peakInfo._assignments =
                parseXeasyAssignments(xeasyMatch.group(16),
                                      xeasyMatch.group(17), xeasyMatch.group(18),
                                      xeasyMatch.group(19));
        }
    }

    //-------------------------------------------------------------------
    // Get data from an NmrView peak list data (not header) line.
    private void processNmrviewData(Matcher nmrviewMatch)
    {
        if (doDebugOutput(23)) {
            System.out.println("processNmrviewData()");
            printMatch(nmrviewMatch);
        }

        int peakId = S2DUtils.string2Int(nmrviewMatch.group(1).trim()) + 1;
        if (!checkPeakId(peakId)) {
            return;
        }

        PeakInfo peakInfo = new PeakInfo();
        _peaks.add(peakInfo);
        peakInfo._peakId = peakId;
        //TEMP -- make sure this really is volume
        peakInfo._inten = S2DUtils.string2Double(nmrviewMatch.group(
                              nmrviewMatch.groupCount()-5));
        peakInfo._method = "volume";

        int shiftCount = 0;
        //TEMP -- make sure these are the right groups!!
        if (nmrviewMatch.group(4) != null) ++shiftCount;
        if (nmrviewMatch.group(12) != null) ++shiftCount;
        if (nmrviewMatch.group(20) != null) ++shiftCount;
        if (nmrviewMatch.group(28) != null) ++shiftCount;
        peakInfo._shifts = new double[shiftCount];
        if (nmrviewMatch.group(4) != null) {
            peakInfo._shifts[0] =
                S2DUtils.string2Double(nmrviewMatch.group(4));
        }
        if (nmrviewMatch.group(12) != null) {
            peakInfo._shifts[1] =
                S2DUtils.string2Double(nmrviewMatch.group(12));
        }
        if (nmrviewMatch.group(20) != null) {
            peakInfo._shifts[2] =
                S2DUtils.string2Double(nmrviewMatch.group(20));
        }
        if (nmrviewMatch.group(28) != null) {
            peakInfo._shifts[3] =
                S2DUtils.string2Double(nmrviewMatch.group(28));
        }
        //TEMP -- looks like chem shift is the one after the atom name
        PeakAssignment assign = new PeakAssignment();
        assign._atomSets[0] = parseNmrviewAtom(nmrviewMatch.group(3));
        assign._atomSets[1] = parseNmrviewAtom(nmrviewMatch.group(11));
        assign._atomSets[2] = parseNmrviewAtom(nmrviewMatch.group(19));
        assign._atomSets[3] = parseNmrviewAtom(nmrviewMatch.group(27));
        peakInfo._assignments = assign;
    }

    //-------------------------------------------------------------------
    // Print all groups of a match.
    private static void printMatch(Matcher mat)
    {
        System.out.println("  Match:");
        for (int index = 1; index <= mat.groupCount(); index++ ) {
            System.out.println("    group(" + index + "): <" +
                               mat.group(index) + ">");
        }
    }

    //-------------------------------------------------------------------
    // Return the name of the partial NMR-STAR file to hold peak list
    // data in proper tags.
    private String starOutputFile()
    {
        //TEMP -- should this be in a different directory?
        return _name + "_peaks.str";
    }

    //-------------------------------------------------------------------
    // Return the name of the DEVise file to hold peak list data.
    // dataType is the data type within the general category of peak
    // list data; currently "i" for intensities is supported.
    private String deviseOutputFile(String dataType, int frameIndex)
    {
        return _dataDir + File.separator + _name +
               S2DNames.PEAKLIST_SUFFIX + dataType + frameIndex +
               S2DNames.DAT_SUFFIX;
    }

    //-------------------------------------------------------------------
    // Parse the Sparky assignments string for a peak.
    // Return an AtomInfo object for each atom.
    private PeakAssignment parseSparkyAssignments(String assignments)
    {
        if (doDebugOutput(23)) {
            System.out.println("parseSparkyAssignments(" +
                               assignments + ")");
        }

        PeakAssignment result = new PeakAssignment();

        String assignAtom = "\\w*(/\\w*)*"; // allows ambiguous assignments
        Pattern pat = Pattern.compile("((" + assignAtom + ")-(" +
                                      assignAtom + ")(-(" + assignAtom + "))?(-(" + assignAtom +
                                      "))?\\s+)");
        if (doDebugOutput(24)) {
            System.out.println("Sparky assignment pattern <: " + pat + ">");
        }
        Matcher matcher = pat.matcher(assignments);
        if (matcher.matches()) {
            if (doDebugOutput(24)) {
                printMatch(matcher);
            }

            result._atomSets[0] = parseSparkyAtomSet(matcher.group(2), null);
            result._atomSets[1] = parseSparkyAtomSet(matcher.group(4),
                                  result._atomSets[0]);
            result._atomSets[2] = parseSparkyAtomSet(matcher.group(7),
                                  result._atomSets[1]);
            result._atomSets[3] = parseSparkyAtomSet(matcher.group(10),
                                  result._atomSets[2]);
        } else {
            System.out.println("Warning (peak list): pattern <" + pat +
                               "> doesn't match assignment string <" + assignments + ">");
        }

        return result;
    }

    //-------------------------------------------------------------------
    // Parse the Sparky assignment string for one atom (including
    // ambiguous assignments).
    // @param atomStr The string holding the atom assingment.
    // @param prevAtomSet The previous atom in this peak (null for the
    //   first atom).
    // @return The atom (or atoms) described in atomStr.
    private AtomAmbigSet parseSparkyAtomSet(String atomStr,
                                            AtomAmbigSet prevAtomSet)
    {
        if (doDebugOutput(23)) {
            System.out.println("parseSparkyAtomSet(" + atomStr + ")");
        }

        if (atomStr == null || atomStr.equals("?")) {
            return null;
        }

        AtomAmbigSet result = new AtomAmbigSet();

        String assignAtom = "/?(\\w*)(.*)";
        Pattern pat = Pattern.compile(assignAtom);
        int ambigId = 1;
        while (atomStr.length() > 0) {
            Matcher matcher = pat.matcher(atomStr);
            if (matcher.matches()) {
                if (doDebugOutput(24)) {
                    printMatch(matcher);
                }
                AtomInfo prevAtom = null;
                if (prevAtomSet != null) {
                    prevAtom = (AtomInfo)prevAtomSet._atoms.get(ambigId-1);
                }
                result._atoms.add(parseSparkyAtom(matcher.group(1),
                                                  prevAtom));

                atomStr = matcher.group(2);
                ambigId++;
            } else {
                atomStr = null;
            }
        }

        return result;
    }

    //-------------------------------------------------------------------
    // Parses the Sparky assignment string for one atom.
    // The atomStr will look like "A1H" or "L88HD1", for example.
    // The first letter is the single-letter amino acid abbreviation;
    // the number is the residue number; and the characters following
    // the number are the atom.
    // Returns an AtomInfo object.
    // Note that if the residue isn't specified in the string, the
    // AtomInfo will have "." for the amino acid and 0 for the residue
    // number.
    private AtomInfo parseSparkyAtom(String atomStr, AtomInfo prevAtom)
    {
        if (doDebugOutput(23)) {
            System.out.println("parseSparkyAtom(" + atomStr + ")");
        }

        if (atomStr == null || atomStr.equals("?")) {
            return null;
        }

        AtomInfo atom = null;

        String letter = "[a-zA-z]";
        Pattern pat = Pattern.compile("((" + letter + ")(\\d+))?(" +
                                      letter + "+\\d?).*");
        if (doDebugOutput(24)) {
            System.out.println("Sparky atom pattern: <" + pat + ">");
        }
        Matcher matcher = pat.matcher(atomStr);
        if (matcher.matches()) {
            if (doDebugOutput(24)) {
                printMatch(matcher);
            }

            atom = new AtomInfo();
            if (matcher.group(2) != null) {
                atom._aminoAcid = make3Letter(matcher.group(2));
            }
            if (matcher.group(3) != null) {
                atom._residueNum = S2DUtils.string2Int(matcher.group(3));
            }
            atom._atomName = matcher.group(4);

            if (prevAtom != null) {
                if (atom._aminoAcid.equals(".")) {
                    atom._aminoAcid = prevAtom._aminoAcid;
                }
                if (atom._residueNum == 0) {
                    atom._residueNum = prevAtom._residueNum;
                }
            }

            if (doDebugOutput(23)) {
                System.out.println("  atom: " + atom. _aminoAcid +
                                   "." + atom._residueNum + "." + atom._atomName);
            }

        } else {
            System.out.println("Warning (peak list): pattern <" + pat +
                               "> doesn't match atom string <" + atomStr + ">");
        }

        return atom;
    }

    //-------------------------------------------------------------------
    // Parse the Xeasy assignments string for a peak.
    // Return an AtomInfo object for each atom.
    private PeakAssignment parseXeasyAssignments(String atomId1,
            String atomId2, String atomId3, String atomId4)
    {
        if (doDebugOutput(23)) {
            System.out.println("parseXeasyAssignments(" + atomId1 + " " +
                               atomId2 + " " + atomId3 + " " + atomId4 + ")");
        }

        PeakAssignment result = new PeakAssignment();

        result._atomSets[0] = parseXeasyAtom(atomId1);
        result._atomSets[1] = parseXeasyAtom(atomId2);
        result._atomSets[2] = parseXeasyAtom(atomId3);
        result._atomSets[3] = parseXeasyAtom(atomId4);

        return result;
    }

    //-------------------------------------------------------------------
    // Make a 1-letter amino acid name into a 3-letter name.
    private String make3Letter(String aminoAcid)
    {
        String result;

        try {
            if (_residues != null) {
                String[] tmp = new String[1];
                tmp[0] = aminoAcid;
                _residues.make3Letter(tmp);
                result = tmp[0];
            } else {
                result = "?";
            }

        } catch(S2DException ex) {
            System.err.println("Error (peak list) translating amino acid string: " +
                               ex.toString());
            result = "???";
        }

        return result;
    }

    //-------------------------------------------------------------------
    // Parse the string for an Xeasy atom assignment.  The string will
    // look like "92 ", for example.
    private AtomAmbigSet parseXeasyAtom(String atomId)
    {
        if (doDebugOutput(23)) {
            System.out.println("parseXeasyAtom(" + atomId + ")");
        }

        if (atomId == null || atomId.trim().equals("0")) {
            return null;
        }

        AtomAmbigSet result = new AtomAmbigSet();
        AtomInfo atom = new AtomInfo();
        atom._authAtomNum = atomId.trim();
        result._atoms.add(atom);

        return result;
    }

    //-------------------------------------------------------------------
    private AtomAmbigSet parseNmrviewAtom(String atomId)
    {
        if (doDebugOutput(23)) {
            System.out.println("parseNmrviewAtom(" + atomId + ")");
        }

        AtomAmbigSet result = null;

        if (atomId == null) {
            return null;
        }

        Pattern pat = Pattern.compile("\\{(\\d+)\\.(.+)\\}");
        if (doDebugOutput(24)) {
            System.out.println("NmrView atom pattern: <" + pat + ">");
        }
        Matcher matcher = pat.matcher(atomId.trim());
        if (matcher.matches()) {
            if (doDebugOutput(24)) {
                printMatch(matcher);
            }
            AtomInfo atom = new AtomInfo();
            atom._residueNum = S2DUtils.string2Int(matcher.group(1));
            atom._atomName = matcher.group(2);
            result = new AtomAmbigSet();
            result._atoms.add(atom);
        }

        return result;
    }

    //-------------------------------------------------------------------
    // Write the NMR-STAR peak tags, with values that we've extracted
    // from the peak list text tag.  We create a single output file
    // with a peak save frame corresponding to each peak save frame
    // in the input file.
    private void writePeakTags(int frameIndex) throws IOException
    {
        FileWriter writer;
        if (frameIndex == 1) {
            writer = S2DFileWriter.create(starOutputFile());
        } else {
            writer = S2DFileWriter.append(starOutputFile());
        }

        if (_gotTagVals) {
            if (doDebugOutput(22)) {
                System.out.println("Save frame " + _frameName +
                                   " already has peak list tag values");
            }
            writer.write("# Save frame " + _frameName +
                         " already has peak values in correct tags\n");
        } else if (_gotTextVals) {

            // Copy everything from the temp file up to (but not
            // including) the save_ at the end.
            if (doDebugOutput(22)) {
                System.out.println("Copying peak save frame values to output file\n");
            }
            BufferedReader reader = new BufferedReader(
                new FileReader(_tmpPeakFrameFile));
            String line;
            while ((line = reader.readLine()) != null) {
                if (line.trim().equals("save_")) {
                    break;
                }
                writer.write(line + "\n");
            }

            if (doDebugOutput(22)) {
                System.out.println("Writing peak list tag values to output file");
            }

            // Write _Spectral_dim loop here if we don't already have
            // it but got it from peak text.
            if (!_haveSpectralDimLoop) {
                writeStarPeakDim(writer, frameIndex);
            }
            writeStarPeakTable(writer, frameIndex);
            writeStarPeakInten(writer, frameIndex);
            writeStarPeakShifts(writer, frameIndex);
            writeStarPeakAssignments(writer, frameIndex);

            writer.write("save_\n\n");
        } else {
            writer.write(
                "# Unable to get peak list values from save frame " +
                _frameName + "\n");
        }
        writer.close();
    }

    //-------------------------------------------------------------------
    // Write the files with peak list data to be used by DEVise.
    // We write a number of files for different data types and save
    // frames.
    private void writeDevisePeaks(int frameIndex) throws IOException
    {
        if (doDebugOutput(21)) {
            System.out.println("S2DPeakList.writeDevisePeaks()");
        }

        //TEMP -- probably need several different DEVise files per save frame?
        //TEMP -- this section should write the files for DEVise
        //TEMP -- should there be a separate devise data file for each save frame? -- I think yes
        writeDevisePeakIntensities(frameIndex);
        //TEMP -- what do I want to write?  * peak intensities  * chem shifts?
    }

    //-------------------------------------------------------------------
    // Write the master peak list table.
    private void writeStarPeakDim(FileWriter writer, int frameIndex)
    throws IOException
    {
        writer.write("\n");
        writer.write("    loop_\n");
        writer.write("        _Spectral_dim.ID\n");
        writer.write("        _Spectral_dim.Atom_type\n");
        writer.write("        _Spectral_dim.Atom_isotope_number\n");
        writer.write("        _Spectral_dim.Spectral_region\n");
        writer.write("        _Spectral_dim.Magnetization_linkage_ID\n");
        writer.write("        _Spectral_dim.Sweep_width\n");
        writer.write("        _Spectral_dim.Encoding_code\n");
        writer.write("        _Spectral_dim.Encoded_source_dimension_ID\n");
        writer.write("        _Spectral_dim.Entry_ID\n");
        writer.write("        _Spectral_dim.Spectral_peak_list_ID\n");
        writer.write("\n");
        for (int dim = 1; dim <= MAX_DIMS; dim++) {
            String region = _spectralDimRegions[dim-1];
            if (!region.equals("")) {
                AtomTypeIso ati = region2TypeIso(region);
                writer.write("        " + dim + "   " + ati.atomType + "   " +
                             ati.isotope + "    " + region + "   .   .   .   .   " +
                             _name + "   " + frameIndex +
                             "\n");
            }
        }
        writer.write("    stop_\n");
        writer.write("\n");
    }

    //-------------------------------------------------------------------
    // Translate region to atom type and isotope.
    private static class AtomTypeIso {
        String atomType = "?";
        int isotope = 0;
    };

    private AtomTypeIso region2TypeIso(String region) {
        AtomTypeIso result = new AtomTypeIso();

        // Note:  do we need oxygen here?  wenger 2014-07-25
        if (region.startsWith("C") || region.startsWith("13C")) {
            result.atomType = "C";
            result.isotope = 13;
        } else if (region.startsWith("H") || region.startsWith("1H") ||
                   (region.startsWith("h"))) {
            result.atomType = "H";
            result.isotope = 1;
        } else if (region.startsWith("N")) {
            result.atomType = "N";
            result.isotope = 15;
        } else {
            S2DWarning warning = new S2DWarning(
                "Warning (peak list): unrecognized peak list region: " +
                region);
            System.err.println(warning.toString());
        }

        return result;
    }

    //-------------------------------------------------------------------
    // Write the master peak list table.
    private void writeStarPeakTable(FileWriter writer, int frameIndex)
    throws IOException
    {
        //
        // Don't write the loop if it will be empty.
        //
        if (doDebugOutput(24)) {
            System.out.println("Peak list count: " + _peaks.size());
        }
        if (_peaks.size() < 1) {
            return;
        }

        //
        // Actually write the loop.
        //
        writer.write("  loop_\n");
        writer.write("      _Peak.ID\n");
        writer.write("      _Peak.Figure_of_merit\n");
        writer.write("      _Peak.Details\n");
        writer.write("      _Peak.Entry_ID\n");
        writer.write("      _Peak.Spectral_peak_list_ID\n\n");

        for (int index = 0; index < _peaks.size(); index++) {
            PeakInfo pi = (PeakInfo)_peaks.get(index);
            writer.write("     " + pi._peakId + " . . " +
                         _name + " " + frameIndex + "\n");
        }

        writer.write("  stop_\n\n");
    }

    //-------------------------------------------------------------------
    // Write peak list intensities to the partial NMR-STAR peak list file.
    private void writeStarPeakInten(FileWriter writer, int frameIndex)
    throws IOException
    {
        //
        // Don't write the loop if it will be empty.
        //
        if (doDebugOutput(24)) {
            System.out.println("Peak list count: " + _peaks.size());
        }
        if (_peaks.size() < 1) {
            return;
        }

        boolean hasInten = false;
        for (int index = 0; index < _peaks.size(); index++) {
            PeakInfo pi = (PeakInfo)_peaks.get(index);
            if (!Double.isNaN(pi._inten)) {
                hasInten = true;
                break;
            }
        }
        if (!hasInten) {
            return;
        }

        //
        // Actually write the loop.
        //
        writer.write("  loop_\n");
        writer.write("      _Peak_general_char.Peak_ID\n");
        writer.write("      _Peak_general_char.Intensity_val\n");
        writer.write("      _Peak_general_char.Intensity_val_err\n");
        writer.write("      _Peak_general_char.Measurement_method\n");
        writer.write("      _Peak_general_char.Entry_ID\n");
        writer.write("      _Peak_general_char.Spectral_peak_list_ID\n\n");

        for (int index = 0; index < _peaks.size(); index++) {
            PeakInfo pi = (PeakInfo)_peaks.get(index);
            if (Double.isNaN(pi._inten)) continue;
            writer.write("     " +
                         pi._peakId + " " + // _Peak_general_char.Peak_ID
                         pi._inten + // _Peak_general_char.Intensity_val
                         " . " + // _Peak_general_char.Intensity_val_err
                         pi._method + " " + // _Peak_general_char.Measurement_method
                         _name + " " + // _Peak_general_char.Entry_ID
                         frameIndex + // _Peak_general_char.Spectral_peak_list_ID
                         "\n");
        }
        writer.write("  stop_\n\n");
    }

    //-------------------------------------------------------------------
    // Write peak list chem shifts (without atom assignments) to the
    // partial NMR-STAR peak list file.
    private void writeStarPeakShifts(FileWriter writer, int frameIndex)
    throws IOException
    {
        //
        // Don't write the loop if it will be empty.
        //
        int shiftCount = 0;
        for (int index = 0; index < _peaks.size(); index++) {
            PeakInfo pi = (PeakInfo)_peaks.get(index);
            if (pi._shifts != null) {
                shiftCount += pi._shifts.length;
            }
        }
        if (doDebugOutput(24)) {
            System.out.println("Shift count: " + shiftCount);
        }
        if (shiftCount < 1) {
            return;
        }

        //
        // Actually write the loop.
        //
        writer.write("  loop_\n");
        writer.write("      _Peak_char.Peak_ID\n");
        writer.write("      _Peak_char.Spectral_dim_ID\n");
        writer.write("      _Peak_char.Chem_shift_val\n");
        writer.write("      _Peak_char.Chem_shift_val_err\n");
        writer.write("      _Peak_char.Line_width_val\n");
        writer.write("      _Peak_char.Line_width_val_err\n");
        writer.write("      _Peak_char.Phase_val\n");
        writer.write("      _Peak_char.Phase_val_err\n");
        writer.write("      _Peak_char.Decay_rate_val\n");
        writer.write("      _Peak_char.Decay_rate_val_err\n");
        writer.write("      _Peak_char.Coupling_pattern\n");
        writer.write("      _Peak_char.Bounding_box_upper_val\n");
        writer.write("      _Peak_char.Bounding_box_lower_val\n");
        writer.write("      _Peak_char.Bounding_box_range_val\n");
        writer.write("      _Peak_char.Details\n");
        writer.write("      _Peak_char.Derivation_method_ID\n");
        writer.write("      _Peak_char.Entry_ID\n");
        writer.write("      _Peak_char.Spectral_peak_list_ID\n\n");

        for (int index = 0; index < _peaks.size(); index++) {
            PeakInfo pi = (PeakInfo)_peaks.get(index);
            if (pi._shifts != null) {
                for (int shiftNum = 0; shiftNum < pi._shifts.length;
                        shiftNum++) {
                    int dimId = shiftNum + 1;
                    writer.write("     " +
                                 pi._peakId + " " + //_Peak_char.Peak_ID
                                 dimId + " " + // _Peak_char.Spectral_dim_ID
                                 pi._shifts[shiftNum] + " " + // _Peak_char.Chem_shift_val
                                 "? " + // _Peak_char.Chem_shift_val_err
                                 "? " + // _Peak_char.Line_width_val
                                 "? " + // _Peak_char.Line_width_val_err
                                 "? " + // _Peak_char.Phase_val
                                 "? " + // _Peak_char.Phase_val_err
                                 "? " + // _Peak_char.Decay_rate_val
                                 "? " + // _Peak_char.Decay_rate_val_err
                                 "? " + // _Peak_char.Coupling_pattern
                                 "? " + // _Peak_char.Bounding_box_upper_val
                                 "? " + // _Peak_char.Bounding_box_lower_val
                                 "? " + // _Peak_char.Bounding_box_range_val
                                 "? " + // _Peak_char.Details
                                 "? " + // _Peak_char.Derivation_method_ID
                                 _name + " " + //_Peak_char.Entry_ID
                                 frameIndex + // _Peak_char.Spectral_peak_list_ID
                                 "\n");
                }
            }
        }

        writer.write("  stop_\n\n");
    }

    //-------------------------------------------------------------------
    // Write peak list chem shifts *with* atom assignments to the
    // partial NMR-STAR peak list file.
    private void writeStarPeakAssignments(FileWriter writer, int frameIndex)
    throws IOException
    {
        if (doDebugOutput(24)) {
            System.out.println("S2DPeakList.writeStarPeakAssignments(" +
                               frameIndex + ")");
        }

        //
        // Don't write the loop if it will be empty.
        //
        int assignCount = 0;
        for (int peakNum = 0; peakNum < _peaks.size(); peakNum++) {
            PeakInfo pi = (PeakInfo)_peaks.get(peakNum);
            if (pi._assignments != null) {
                for (int dimId = 1;
                        dimId <= pi._assignments._atomSets.length; dimId++) {
                    // We may have less than four dimensions...
                    AtomAmbigSet atomSet = pi._assignments._atomSets[dimId-1];
                    if (atomSet != null) {
                        assignCount += atomSet._atoms.size();
                    }
                }
            }
        }
        if (doDebugOutput(24)) {
            System.out.println("Assignment count: " + assignCount);
        }
        if (assignCount < 1) {
            return;
        }

        //
        // Actually write the loop.
        //
        writer.write("  loop_\n");
        writer.write("      _Assigned_peak_chem_shift.Peak_ID\n");
        writer.write("      _Assigned_peak_chem_shift.Spectral_dim_ID\n");
        writer.write("      _Assigned_peak_chem_shift.Peak_contribution_ID\n");
        writer.write("      _Assigned_peak_chem_shift.Set_ID\n");
        writer.write("      _Assigned_peak_chem_shift.Magnetization_linkage_ID\n");
        writer.write("      _Assigned_peak_chem_shift.Assembly_atom_ID\n");
        writer.write("      _Assigned_peak_chem_shift.Val\n");
        writer.write("      _Assigned_peak_chem_shift.Figure_of_merit\n");
        writer.write("      _Assigned_peak_chem_shift.Assigned_chem_shift_list_ID\n");
        writer.write("      _Assigned_peak_chem_shift.Atom_chem_shift_ID\n");
        writer.write("      _Assigned_peak_chem_shift.Entity_assembly_ID\n");
        writer.write("      _Assigned_peak_chem_shift.Entity_ID\n");
        writer.write("      _Assigned_peak_chem_shift.Comp_index_ID\n");
        writer.write("      _Assigned_peak_chem_shift.Comp_ID\n");
        writer.write("      _Assigned_peak_chem_shift.Atom_ID\n");
        writer.write("      _Assigned_peak_chem_shift.Auth_atom_peak_num\n");
        writer.write("      _Assigned_peak_chem_shift.Auth_entity_ID\n");
        writer.write("      _Assigned_peak_chem_shift.Auth_seq_ID\n");
        writer.write("      _Assigned_peak_chem_shift.Auth_comp_ID\n");
        writer.write("      _Assigned_peak_chem_shift.Auth_atom_ID\n");
        writer.write("      _Assigned_peak_chem_shift.Auth_amb_atom_grp_ID\n");
        writer.write("      _Assigned_peak_chem_shift.Resonance_ID\n");
        writer.write("      _Assigned_peak_chem_shift.Details\n");
        writer.write("      _Assigned_peak_chem_shift.Entry_ID\n");
        writer.write("      _Assigned_peak_chem_shift.Spectral_peak_list_ID\n\n");

        for (int peakNum = 0; peakNum < _peaks.size(); peakNum++) {
            PeakInfo pi = (PeakInfo)_peaks.get(peakNum);
            if (pi._assignments != null) {
                for (int dimId = 1;
                        dimId <= pi._assignments._atomSets.length; dimId++) {
                    // We may have less than four dimensions...
                    AtomAmbigSet atomSet = pi._assignments._atomSets[dimId-1];
                    if (atomSet != null) {
                        for (int ambigId = 1; ambigId <= atomSet._atoms.size();
                                ambigId++) {
                            writePeakAssignLine(writer, frameIndex, pi,
                                                dimId, ambigId);
                        }
                    }
                }
            }
        }

        writer.write("  stop_\n\n");
    }

    //-------------------------------------------------------------------
    /** Write one peak assignment line.
     *  @param writer The Writer to write to.
     *  @param frameIndex The peak list frame index.
     *  @param pi The PeakInfo for this peak.
     *  @param dimId The dimension ID (starts at 1).
     *  @param ambigId The ambiguity ID (starts at 1).
     */
    private void writePeakAssignLine(FileWriter writer, int frameIndex,
                                     PeakInfo pi, int dimId, int ambigId) throws IOException
    {
        if (doDebugOutput(25)) {
            System.out.println("S2DPeakList.writePeakAssignLine(" +
                               frameIndex + ", " + pi._peakId + ", " + dimId + ", " +
                               ambigId + ")");
        }

        AtomInfo atom =
            (AtomInfo)pi._assignments._atomSets[dimId-1]._atoms.get(ambigId-1);

        String residueNum;
        if (atom._residueNum != 0) {
            residueNum = "" + atom._residueNum;
        } else {
            residueNum = ".";
        }

        writer.write("     " +
                     pi._peakId + " " + // _Assigned_peak_chem_shift.Peak_ID
                     dimId + " " + // _Assigned_peak_chem_shift.Spectral_dim_ID
                     "? " + // _Assigned_peak_chem_shift.Peak_contribution_ID
                     "? " + // _Assigned_peak_chem_shift.Set_ID
                     "? " + // _Assigned_peak_chem_shift.Magnetization_linkage_ID
                     "? " + // _Assigned_peak_chem_shift.Assembly_atom_ID
                     pi._shifts[dimId-1] + " " + // _Assigned_peak_chem_shift.Val
                     "? " + // _Assigned_peak_chem_shift.Figure_of_merit
                     "? " + // _Assigned_peak_chem_shift.Assigned_chem_shift_list_ID
                     "? " + // _Assigned_peak_chem_shift.Atom_chem_shift_ID
                     "? " + // _Assigned_peak_chem_shift.Entity_assembly_ID
                     "? " + // _Assigned_peak_chem_shift.Entity_ID
                     "? " + // _Assigned_peak_chem_shift.Comp_index_ID
                     "? " + // _Assigned_peak_chem_shift.Comp_ID
                     "? " + // _Assigned_peak_chem_shift.Atom_ID
                     atom._authAtomNum + " " + // _Assigned_peak_chem_shift.Auth_atom_peak_num
                     "? " + // _Assigned_peak_chem_shift.Auth_entity_ID
                     residueNum + " " + // _Assigned_peak_chem_shift.Auth_seq_ID
                     atom._aminoAcid + " " + // _Assigned_peak_chem_shift.Auth_comp_ID
                     atom._atomName + " " + // _Assigned_peak_chem_shift.Auth_atom_ID
                     ambigId + " " + // _Assigned_peak_chem_shift.Auth_amb_atom_grp_ID
                     "? " + // _Assigned_peak_chem_shift.Resonance_ID
                     "? " + // _Assigned_peak_chem_shift.Details
                     _name + " " + // _Assigned_peak_chem_shift.Entry_ID
                     frameIndex + // _Assigned_peak_chem_shift.Spectral_peak_list_ID
                     "\n");
    }

    //-------------------------------------------------------------------
    // Write peak intensities (and some other values) for DEVise
    // visualizations.
    private void writeDevisePeakIntensities(int frameIndex) throws IOException
    {
        if (doDebugOutput(21)) {
            System.out.println("S2DPeakList.writeDevisePeakIntensities()");
        }

        FileWriter writer =
            //TEMP -- parameterize the "i"?
            S2DFileWriter.create(deviseOutputFile("i", frameIndex));

        writer.write("# Data: peak intensities for " + _name + "\n");
        writer.write("# Schema: bmrb-pkl\n");
        writer.write("# Attributes: peakNum; peakIten; chemShift1; " +
                     "chemShift2; chemShift3; chemShift4\n");
        writer.write("# Peptide-CGI version: " +
                     S2DMain.PEP_CGI_VERSION + "\n");
        writer.write("# Generation date: " +
                     S2DMain.getTimestamp() + "\n");
        writer.write("#\n");

        for (int index = 0; index < _peaks.size(); index++) {
            PeakInfo pi = (PeakInfo)_peaks.get(index);
            int shiftNum = 0;
            //TEMP -- write 0.0 for intensity if it's NaN?
            writer.write(pi._peakId + " " + pi._inten + " ");
            if (pi._shifts != null) {
                for (shiftNum = 0; shiftNum < pi._shifts.length;
                        shiftNum++) {
                    writer.write(pi._shifts[shiftNum] + " ");
                }
            }
            for (; shiftNum < MAX_DIMS; shiftNum++) {
                writer.write("0.0 ");
            }
            writer.write("\n");
        }

        writer.close();

        //TEMP -- also write session file in here?
    }

    //-------------------------------------------------------------------
    // Returns true if peakId is okay (didn't decrease).
    private boolean checkPeakId(int peakId)
    {
        if (peakId < _maxPeakId) {
            // Note:  we really should throw an exception here, but
            // it would be caught up above, so for now I'm going to call
            // exit() instead.  I guess we should clean up how the
            // exceptions work (maybe add an S2DFatalError?).
            System.err.println("Error:  current peak ID (" +
                               peakId + ") is less than maximum peak id (" +
                               _maxPeakId + ")");
            System.exit(1);

            // We don't get to here...
            return false;
        } else {
            _maxPeakId = peakId;
            return true;
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

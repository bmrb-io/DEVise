// ========================================================================
// DEVise Data Visualization Software
// (c) Copyright 2001-2015
// By the DEVise Development Group
// Madison, Wisconsin
// All Rights Reserved.
// ========================================================================

// Under no circumstances is this software to be copied, distributed,
// or altered in any way without prior permission from the DEVise
// Development Group.

// ------------------------------------------------------------------------

// This class implements the output of heteronuclear NOE data.  For each set
// of heteronuclear NOE data, it creates a data file, a session file, an
// individual html file, and a link in the summary html file.

// ------------------------------------------------------------------------

// $Id$

// $Log$
// Revision 1.17.26.2  2015/03/04 20:46:24  wenger
// To-do 211:  Hopefully final cleanup before merge.
//
// Revision 1.17.26.1  2015/02/20 16:24:08  wenger
// To-do 211:  Re-ordered some of the data and fixed tests accordingly.
//
// Revision 1.17  2011/10/10 23:43:39  wenger
// Reduced edited movie time from .1 to .02, and set the resolution to
// 400x400 to speed up generation time (just took 2:44 in a test).
//
// Revision 1.16.8.1  2011/09/21 20:46:10  wenger
// The s2predicted session movie buttons are now updated for the correct
// PDB ID -- I should probably have configuration for the URL, though,
// and also only show the button if the movie is available.
//
// Revision 1.16  2010/12/07 17:41:15  wenger
// Did another version history purge.
//
// Revision 1.15  2010/11/01 00:51:12  wenger
// Merged sample_cond2_br_0 thru sample_cond2_br_1 to trunk.
//
// Revision 1.14.2.3  2010/10/19 00:23:20  wenger
// Split the actual sample info out from the sample conditions info,
// including modifying ambiguity code and Pistachio metadata accordingly.
//
// Revision 1.14.2.2  2010/10/16 01:32:26  wenger
// Getting sample conditions save frame names now works for 2.1 files.
//
// Revision 1.14.2.1  2010/10/15 15:29:02  wenger
// Merged sample_cond_br_0 thru sample_cond_br_1/sample_cond_br_end to
// sample_cond2_br (to get the latest code refactoring from the trunk
// into the sample conditions code).
//
// Revision 1.14  2010/10/13 20:44:02  wenger
// Finished restructuring Peptide-CGI code so that we get values from
// the STAR files in the relevant object constructors, instead of in the
// S2DMain class.
//
// Revision 1.13.6.2  2010/10/11 14:38:32  wenger
// Started on method to get sample conditions for data save frames; I'm
// just committing a preliminary version until I make a change on the trunk
// to move all of the code for actually getting the relevant frame values
// down into the data-specific classes like I've already done with the
// delta shifts.
//
// Revision 1.13.6.1  2010/10/08 21:17:41  wenger
// We now put save frame details into the drill-down data for the data
// selection view in 3D visualizations; also fixed a bug in getting save
// frame details for 3.0/3.1 files.
//
// Revision 1.13  2010/03/10 22:36:16  wenger
// Added NMR-STAR file version to summary html page and detailed
// visualization version info (to-do 072).  (Doing this before I
// add multiple NMR-STAR paths so we can see which NMR-STAR file
// was used.)
//
// Revision 1.12  2010/02/17 18:48:41  wenger
// Fixed bug 093 (incorrect entity assembly IDs in 3D data sets).
//
// Revision 1.11  2009/03/12 17:30:19  wenger
// Changed entity assembly names to things like "EA 1 (polypeptide(L))"
// as requested by Eldon; changed tests accordingly.
//
// Revision 1.10  2009/03/11 19:53:14  wenger
// Implemented two-stage selection of data sets in coordinate
// visualizations (select entity assembly, then select data set); updated
// tests accordingly.  Updated test14 because it now generates data for
// entity assembly 1.
//
// ...

// ========================================================================

package star2devise;

import java.io.*;
import java.util.*;
import EDU.bmrb.starlibj.SaveFrameNode;

public class S2DHetNOE {
    //===================================================================
    // VARIABLES

    private static final int DEBUG = 0;

    private String _name;
    private String _longName;
    private String _dataDir;
    private String _sessionDir;
    private S2DSummaryHtml _summary;
    private int _entityAssemblyID;
    private String _frameDetails;
    private String _sample;
    private String _sampleConditions;

    private String _title;
    private String _shortName;

    private String[] _resSeqCodes;
    private String[] _resLabels;
    private String[] _hetNOEValues;
    private String[] _hetNOEErrors;

    private String _starVersion;

    //===================================================================
    // PUBLIC METHODS

    //-------------------------------------------------------------------
    // Constructor.
    public S2DHetNOE(String name, String longName, S2DNmrStarIfc star,
                     SaveFrameNode frame, String dataDir, String sessionDir,
                     S2DSummaryHtml summary, String entityAssemblyID) throws S2DException
    {
        if (doDebugOutput(11)) {
            System.out.println("S2DHetNOE.S2DHetNOE(" + name +
                               ")");
        }

        _name = name;
        _longName = longName;
        _dataDir = dataDir;
        _sessionDir = sessionDir;
        _summary = summary;
        _frameDetails = star.getFrameDetails(frame);
        _sample = star.getFrameSample(frame);
        _sampleConditions = star.getFrameSampleConditions(frame);
        _starVersion = star.version();

        //
        // Get the values we need from the Star file.
        // TEMP -- NMR-STAR 3.0 has two residues for each heternuclear
        // NOE, so we should probably deal with that eventually.
        //

        // If a non-blank entityAssemblyID is specified, we need to filter
        // the frame values to only take the ones corresponding to that
        // entityAssemblyID.  To do that, we get the entityAssemblyID
        // values in each row of the loop.  (entityAssemblyID will be blank
        // when processing NMR-STAR 2.1 files -- they don't have data for
        // more than one entity assembly in a single save frame).
        String[] entityAssemblyIDs = null;
        if (!entityAssemblyID.equals("")) {
            entityAssemblyIDs = star.getFrameValues(frame,
                                                    star.HET_NOE_ENTITY_ASSEMBLY_ID_1,
                                                    star.HET_NOE_ENTITY_ASSEMBLY_ID_1);
        }

//TEMP -- convert these to numerical values?
        _resSeqCodes = star.getAndFilterFrameValues(frame,
                       star.HET_NOE_VALUE, star.HET_NOE_RES_SEQ_CODE, entityAssemblyID,
                       entityAssemblyIDs);

        _resLabels = star.getAndFilterFrameValues(frame,
                     star.HET_NOE_VALUE, star.HET_NOE_RES_LABEL, entityAssemblyID,
                     entityAssemblyIDs);

        _hetNOEValues = star.getAndFilterFrameValues(frame,
                        star.HET_NOE_VALUE, star.HET_NOE_VALUE, entityAssemblyID,
                        entityAssemblyIDs);

        _hetNOEErrors = star.getAndFilterFrameValues(frame,
                        star.HET_NOE_VALUE, star.HET_NOE_VALUE_ERR, entityAssemblyID,
                        entityAssemblyIDs);

        _entityAssemblyID = star.getEntityAssemblyID(frame,
                            entityAssemblyID);

        String frequency = star.getOneFrameValue(frame,
                           star.HET_NOE_SPEC_FREQ_1H);
        String atom1Name = star.getHetNOEAtom1(frame);
        String atom2Name = star.getHetNOEAtom2(frame);

        if (atom1Name.indexOf("not available") != -1) atom1Name = "?";
        if (atom2Name.indexOf("not available") != -1) atom2Name = "?";
        _shortName = "Het NOE (" + frequency + ") " + atom1Name +
                     " " + atom2Name;
        _title = frequency + " MHz " + atom1Name + " " + atom2Name;
    }

    //-------------------------------------------------------------------
    // Write the heteronuclear NOE values for this data.
    public void writeHetNOE(int frameIndex) throws S2DException
    {
        if (doDebugOutput(11)) {
            System.out.println("S2DHetNOE.writeHetNOE()");
        }

        try {
            //
            // Write the heteronuclear NOE values to the data file.
            //
            FileWriter hetNOEWriter = S2DFileWriter.create(_dataDir +
                                      File.separator + _name + S2DNames.HETERONUCLEAR_NOE_SUFFIX +
                                      frameIndex + S2DNames.DAT_SUFFIX);

            hetNOEWriter.write("# Data: heteronuclear NOE for " + _name + "\n");
            hetNOEWriter.write("# Schema: bmrb-NOE\n");
            hetNOEWriter.write("# Attributes: NOE_value; NOE_error; " +
                               "Residue_seq_code; Residue_label; Entity_assembly_ID;\n");
            hetNOEWriter.write("# Peptide-CGI version: " +
                               S2DMain.PEP_CGI_VERSION + "\n");
            hetNOEWriter.write("# Generation date: " +
                               S2DMain.getTimestamp() + "\n");
            hetNOEWriter.write("#\n");

            for (int index = 0; index < _resSeqCodes.length; index++) {
                hetNOEWriter.write(_hetNOEValues[index] + " " +
                                   _hetNOEErrors[index] + " " +
                                   _resSeqCodes[index] + " " +
                                   _resLabels[index] + " " +
                                   _entityAssemblyID + "\n");
            }

            hetNOEWriter.close();

            //
            // Write the session file.
            //
            String info = "Visualization of " + _longName;
            S2DSession.write(_sessionDir, S2DUtils.TYPE_HETNOE,
                             _name, frameIndex, info, _title, true, _starVersion, "");

            //
            // Write the session-specific html file.
            //
            String fullTitle = _title + " (entity assembly " +
                               _entityAssemblyID + ")";
            S2DSpecificHtml specHtml = new S2DSpecificHtml(
                _summary.getHtmlDir(), S2DUtils.TYPE_HETNOE,
                _name, frameIndex, fullTitle, _frameDetails);
            specHtml.write();

            //
            // Write the link in the summary html file.
            //
            _summary.writeHetNOE(_title, frameIndex, _entityAssemblyID,
                                 _resSeqCodes.length);

        } catch(IOException ex) {
            System.err.println(
                "IOException writing heteronuclear NOE values: " +
                ex.toString());
            throw new S2DError("Can't write heteronuclear NOE values");
        }
    }

    //-------------------------------------------------------------------
    /**
     * Add heteronuclear NOE sets to the data set list.
     * @param The data set list.
     * @param The frame index.
     */
    public void addHetNOEData(Vector dataSets, int frameIndex,
                              int polymerType)
    {
        // Note: attribute names must match the bmrb-NOE schema.
        String dataSource = _name +
                            S2DNames.HETERONUCLEAR_NOE_SUFFIX + frameIndex;
        String dataName = _shortName + " [" + _entityAssemblyID + "]";
        dataSets.addElement(new S2DDatasetInfo(dataName, _frameDetails,
                                               _sample, _sampleConditions, dataSource, "NOE_value", "bmrb-NOE",
                                               "NOE", _entityAssemblyID, polymerType));
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

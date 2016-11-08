// ========================================================================
// DEVise Data Visualization Software
// (c) Copyright 2008-2015
// By the DEVise Development Group
// Madison, Wisconsin
// All Rights Reserved.
// ========================================================================

// Under no circumstances is this software to be copied, distributed,
// or altered in any way without prior permission from the DEVise
// Development Group.

// ------------------------------------------------------------------------

// This class implements the output of S2 order data.  For each set of
// S2 order data, it creates a data file, a session file, an
// individual html file, and a link in the summary html file.

// ------------------------------------------------------------------------

// $Id$

// $Log$
// Revision 1.11.26.2  2015/03/04 20:46:24  wenger
// To-do 211:  Hopefully final cleanup before merge.
//
// Revision 1.11.26.1  2015/02/20 16:24:09  wenger
// To-do 211:  Re-ordered some of the data and fixed tests accordingly.
//
// Revision 1.11  2011/10/10 23:43:39  wenger
// Reduced edited movie time from .1 to .02, and set the resolution to
// 400x400 to speed up generation time (just took 2:44 in a test).
//
// Revision 1.10.10.1  2011/09/21 20:46:10  wenger
// The s2predicted session movie buttons are now updated for the correct
// PDB ID -- I should probably have configuration for the URL, though,
// and also only show the button if the movie is available.
//
// Revision 1.10  2010/11/01 00:51:12  wenger
// Merged sample_cond2_br_0 thru sample_cond2_br_1 to trunk.
//
// Revision 1.9.2.3  2010/10/19 00:23:20  wenger
// Split the actual sample info out from the sample conditions info,
// including modifying ambiguity code and Pistachio metadata accordingly.
//
// Revision 1.9.2.2  2010/10/16 01:32:26  wenger
// Getting sample conditions save frame names now works for 2.1 files.
//
// Revision 1.9.2.1  2010/10/15 15:29:03  wenger
// Merged sample_cond_br_0 thru sample_cond_br_1/sample_cond_br_end to
// sample_cond2_br (to get the latest code refactoring from the trunk
// into the sample conditions code).
//
// Revision 1.9  2010/10/13 20:44:02  wenger
// Finished restructuring Peptide-CGI code so that we get values from
// the STAR files in the relevant object constructors, instead of in the
// S2DMain class.
//
// Revision 1.8.6.2  2010/10/11 14:38:33  wenger
// Started on method to get sample conditions for data save frames; I'm
// just committing a preliminary version until I make a change on the trunk
// to move all of the code for actually getting the relevant frame values
// down into the data-specific classes like I've already done with the
// delta shifts.
//
// Revision 1.8.6.1  2010/10/08 21:17:42  wenger
// We now put save frame details into the drill-down data for the data
// selection view in 3D visualizations; also fixed a bug in getting save
// frame details for 3.0/3.1 files.
//
// Revision 1.8  2010/03/10 22:36:17  wenger
// Added NMR-STAR file version to summary html page and detailed
// visualization version info (to-do 072).  (Doing this before I
// add multiple NMR-STAR paths so we can see which NMR-STAR file
// was used.)
//
// Revision 1.7  2010/02/17 18:48:41  wenger
// Fixed bug 093 (incorrect entity assembly IDs in 3D data sets).
//
// Revision 1.6  2009/03/12 17:30:19  wenger
// Changed entity assembly names to things like "EA 1 (polypeptide(L))"
// as requested by Eldon; changed tests accordingly.
//
// Revision 1.5  2009/03/11 19:53:15  wenger
// Implemented two-stage selection of data sets in coordinate
// visualizations (select entity assembly, then select data set); updated
// tests accordingly.  Updated test14 because it now generates data for
// entity assembly 1.
//
// Revision 1.4  2008/12/01 20:37:53  wenger
// Merged s2d_bug_037_br_0 thru s2d_bug_037_br_2 to trunk.
//
// Revision 1.3.2.3  2008/11/17 19:28:07  wenger
// Added entity assembly IDs to summary page and specific visualization pages.
//
// Revision 1.3.2.2  2008/11/11 20:47:51  wenger
// Progress on getting entity assembly IDs correct for coupling constants,
// heteronuclear NOE, T1 & T2 relaxation, and S2 order parameters
// (working for 2.1, but not yet for 3.1).
//
// Revision 1.3.2.1  2008/07/30 16:13:56  wenger
// First steps towards fixing bug 037/etc -- added (dummy) entity
// assembly ID values to generated data; updated schemas and tests
// accordingly.
//
// Revision 1.3  2008/07/02 16:29:19  wenger
// S2 order parameter visualizations are done and approved by Eldon;
// tests at least partially updated for S2 order stuff;
// reversed the order of data sets in the data selection view of
// 3D visualizations (more closely matches the summary page); minor
// fix to testclean target in top-level makefile; minor fix to
// relaxation session template (bar widths now set); added indices
// to data set titles in 3D visualizations.
//
// Revision 1.2  2008/04/09 19:35:42  wenger
// Added frame details to individual visualization pages in preparation
// for summary page changes; spelled out Linear Analysis of Chemical
// Shifts; removed some unneeded parameters from the S2DSummaryHtml*
// constructors.
//
// Revision 1.1  2008/02/20 17:41:08  wenger
// Committing (disabled) partially-implemented S2 Order visualization
// code and tests.
//

// ========================================================================

package star2devise;

import java.io.*;
import java.util.*;
import EDU.bmrb.starlibj.SaveFrameNode;

public class S2DS2Order {
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

    private String[] _resSeqCodes;
    private String[] _resLabels;
    private String[] _atomNames;
    private String[] _s2OrderValues;
    private String[] _s2OrderErrors;

    private String _starVersion;

    //===================================================================
    // PUBLIC METHODS

    //-------------------------------------------------------------------
    // Constructor.
    public S2DS2Order(String name, String longName, S2DNmrStarIfc star,
                      SaveFrameNode frame, String dataDir, String sessionDir,
                      S2DSummaryHtml summary, String entityAssemblyID) throws S2DException
    {
        if (doDebugOutput(11)) {
            System.out.println("S2DS2Order.S2DS2Order(" + name + ")");
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
                                                    star.ORDER_ENTITY_ASSEMBLY_ID,
                                                    star.ORDER_ENTITY_ASSEMBLY_ID);
        }

        _resSeqCodes = star.getAndFilterFrameValues(frame,
                       star.ORDER_VALUE, star.ORDER_RES_SEQ_CODE, entityAssemblyID,
                       entityAssemblyIDs);

        _resLabels = star.getAndFilterFrameValues(frame,
                     star.ORDER_VALUE, star.ORDER_RES_LABEL, entityAssemblyID,
                     entityAssemblyIDs);

        _atomNames = star.getAndFilterFrameValues(frame,
                     star.ORDER_VALUE, star.ORDER_ATOM_NAME, entityAssemblyID,
                     entityAssemblyIDs);

        _s2OrderValues = star.getAndFilterFrameValues(frame,
                         star.ORDER_VALUE, star.ORDER_VALUE, entityAssemblyID,
                         entityAssemblyIDs);

        _s2OrderErrors = star.getAndFilterOptionalFrameValues(frame,
                         star.ORDER_VALUE, star.ORDER_VALUE_ERR, entityAssemblyID,
                         entityAssemblyIDs, _s2OrderValues.length, "0");

        _entityAssemblyID = star.getEntityAssemblyID(frame,
                            entityAssemblyID);
    }

    //-------------------------------------------------------------------
    // Write the S2 order values for this data.
    public void writeS2Order(int frameIndex) throws S2DException
    {
        if (doDebugOutput(11)) {
            System.out.println("S2DS2Order.writeS2Order()");
        }

        try {
            //
            // Write the S2 order values to the data file.
            //
            FileWriter s2OrderWriter = S2DFileWriter.create(_dataDir +
                                       File.separator + _name + S2DNames.ORDER_SUFFIX +
                                       frameIndex + S2DNames.DAT_SUFFIX);
            s2OrderWriter.write("# Data: s2order values for " + _name + "\n");
            s2OrderWriter.write("# Schema: bmrb-s2\n");
            s2OrderWriter.write("# Attributes: s2order_value; " +
                                "s2order_error; Atom_name; Residue_seq_code; " +
                                "Residue_label; Entity_assembly_ID\n");
            s2OrderWriter.write("# Peptide-CGI version: " +
                                S2DMain.PEP_CGI_VERSION + "\n");
            s2OrderWriter.write("# Generation date: " +
                                S2DMain.getTimestamp() + "\n");
            s2OrderWriter.write("#\n");

            for (int index = 0; index < _resSeqCodes.length; index++) {
                s2OrderWriter.write(_s2OrderValues[index] + " " +
                                    _s2OrderErrors[index] + " " +
                                    _atomNames[index] + " " +
                                    _resSeqCodes[index] + " " +
                                    _resLabels[index] + " " +
                                    _entityAssemblyID + "\n");
            }

            s2OrderWriter.close();

            //
            // Write the session file.
            //
            String info = "Visualization of " + _longName;
            String title = "S2 Order Parameters (entity assembly " +
                           _entityAssemblyID + ")";
            S2DSession.write(_sessionDir, S2DUtils.TYPE_ORDER,
                             _name, frameIndex, info, title, true, _starVersion, "");

            //
            // Write the session-specific html file.
            //
            S2DSpecificHtml specHtml = new S2DSpecificHtml(
                _summary.getHtmlDir(), S2DUtils.TYPE_ORDER,
                _name, frameIndex, title, _frameDetails);
            specHtml.write();

            //
            // Write the link in the summary html file.
            //
            _summary.writeS2Order(frameIndex, _entityAssemblyID,
                                  _resSeqCodes.length);

        } catch(IOException ex) {
            System.err.println("IOException writing s2 order data: " +
                               ex.toString());
            throw new S2DError("Can't write s2 order data");
        }
    }

    //-------------------------------------------------------------------
    /**
     * Add s2 order sets to the data set list.
     * @param The data set list.
     * @param The frame index.
     */
    public void addS2Order(Vector dataSets, int frameIndex,
                           int polymerType)
    {
        // Note: attribute names must match the bmrb-s2order schema.
        String dataSource = _name + S2DNames.ORDER_SUFFIX + frameIndex;
        String dataName = "S2 Order Parameters [" + _entityAssemblyID + "]";
        dataSets.addElement(new S2DDatasetInfo(dataName, _frameDetails,
                                               _sample, _sampleConditions, dataSource, "S2order_value",
                                               "bmrb-s2", "s2", _entityAssemblyID, polymerType));
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

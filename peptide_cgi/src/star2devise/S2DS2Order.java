// ========================================================================
// DEVise Data Visualization Software
// (c) Copyright 2008-2009
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

    private int _dataType;

    private String[] _resSeqCodes;
    private String[] _resLabels;
    private String[] _atomNames;
    private String[] _s2OrderValues;
    private String[] _s2OrderErrors;

    //===================================================================
    // PUBLIC METHODS

    //-------------------------------------------------------------------
    // Constructor.  (See S2DUtils for dataType.)
    public S2DS2Order(String name, String longName, String dataDir,
      String sessionDir, S2DSummaryHtml summary, String[] resSeqCodes,
      String[] resLabels, String[] atomNames, String[] s2OrderValues,
      String[] s2OrderErrors, int entityAssemblyID, String frameDetails)
      throws S2DException
    {
        if (doDebugOutput(11)) {
	    System.out.println("S2DS2Order.S2DS2Order(" + name + ")");
	}
        _name = name;
        _longName = longName;
        _dataDir = dataDir;
        _sessionDir = sessionDir;
        _summary = summary;
	_entityAssemblyID = entityAssemblyID;
	_frameDetails = frameDetails;

        _resSeqCodes = resSeqCodes;
        _resLabels = S2DUtils.arrayToUpper(resLabels);
        _atomNames = atomNames;
        _s2OrderValues = s2OrderValues;
        _s2OrderErrors = s2OrderErrors;
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
            s2OrderWriter.write("# Schema: bmrb-s2order\n");
            s2OrderWriter.write("# Attributes: Entity_assembly_ID; " +
	      "Residue_seq_code; " +
	      "Residue_label; Atom_name; s2order_value; s2order_error\n");
            s2OrderWriter.write("# Peptide-CGI version: " +
	      S2DMain.PEP_CGI_VERSION + "\n");
            s2OrderWriter.write("# Generation date: " +
	      S2DMain.getTimestamp() + "\n");
            s2OrderWriter.write("#\n");

	    for (int index = 0; index < _resSeqCodes.length; index++) {
	        s2OrderWriter.write(_entityAssemblyID + " " +
		  _resSeqCodes[index] + " " +
		  _resLabels[index] + " " +
		  _atomNames[index] + " " +
		  _s2OrderValues[index] + " " +
		  _s2OrderErrors[index] + "\n");
	    }

	    s2OrderWriter.close();

	    //
	    // Write the session file.
	    //
	    String info = "Visualization of " + _longName;
	    String title = "S2 Order Parameters (entity assembly " +
	      _entityAssemblyID + ")";
	    S2DSession.write(_sessionDir, S2DUtils.TYPE_ORDER,
	      _name, frameIndex, info, title);

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
	String dataName = "S2 Order Parameters [" + frameIndex + "]";
	dataSets.addElement(new S2DDatasetInfo(dataName, dataSource,
	  "S2order_value", "bmrb-s2", "s2", _entityAssemblyID,
	  polymerType));
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

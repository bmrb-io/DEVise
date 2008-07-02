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

// This class implements methods for outputting the summary data to be
// used to select individual data sets in a 3D atomic coordinate
// visualization.

// ------------------------------------------------------------------------

// $Id$

// $Log$
// Revision 1.4  2007/11/15 17:15:36  wenger
// Cleaned out cvs history in source files.
//
// Revision 1.3  2007/08/20 20:26:10  wenger
// Added -verb command-line flag and property so we can turn on debug
// output without recompiling; added debug_level property corresponding
// to the existing -debug command-line flag.
//
// Revision 1.2  2006/02/01 20:23:12  wenger
// Merged V2_1b4_br_0 thru peptide_cgi_10_8_0_base to the
// trunk.
//
// Revision 1.1.2.5  2005/01/31 23:02:55  wenger
// Merged pistachio_vis_br_0 thru pistachio_vis_br_1a to V2_1b4_br.
//
// Revision 1.1.2.4  2004/12/03 17:18:34  wenger
// Added BMRB accession number, PDB ID (if relevant), timestamp and
// Peptide-CGI version number to Peptide-CGI visualizations.
//
// ...

// ========================================================================

package star2devise;

import java.util.*;
import java.io.*;

public class S2DSummaryData
{
    //===================================================================
    // VARIABLES

    private static final int DEBUG = 0;

    private String _name;
    private String _dataDir;
    private Vector _dataSets;

    //===================================================================
    // PUBLIC METHODS

    //-------------------------------------------------------------------
    /**
     * Constructor.
     * @param The data name.
     * @param The data directory.
     * @param The list of available data sets.
     */
    public S2DSummaryData(String name, String dataDir, Vector dataSets)
    {
        if (doDebugOutput(11)) {
	    System.out.println("S2DSummaryData.S2DSummaryData(" +
	      name + " " + dataDir + ")");
        }

	_name = name;
	_dataDir = dataDir;
    	_dataSets = dataSets;
    }

    //-------------------------------------------------------------------
    /**
     * Write the summary data.
     */
    public void writeSummaryData() throws S2DException
    {
        if (doDebugOutput(11)) {
	    System.out.println("S2DSummaryData.writeSummaryData()");
            System.out.println("Data sets: ");
            for (int index = _dataSets.size()-1; index >= 0; index--) {
                S2DDatasetInfo info =
		  (S2DDatasetInfo)_dataSets.elementAt(index);
                System.out.println("  " + info.getName() + "\t" +
                  info.getDataSourceName() + "\t" + info.getYAttribute());
            }
        }

	try {
	    FileWriter writer = S2DFileWriter.create(_dataDir +
	      File.separator + _name + S2DNames.SUMMARY_DATA_SUFFIX +
	        S2DNames.DAT_SUFFIX);
	    writer.write("# Data: summary of available datasets for " +
	      _name + " 3D visualization\n");
	    writer.write("# Schema: bmrb-summary\n");
	    writer.write("# Attributes: name; data_source; y_attribute\n");
            writer.write("# Peptide-CGI version: " +
	      S2DMain.PEP_CGI_VERSION + "\n");
            writer.write("# Generation date: " +
	      S2DMain.getTimestamp() + "\n");
	    writer.write("#\n");

            for (int index = _dataSets.size()-1; index >= 0; index--) {
                S2DDatasetInfo info =
		  (S2DDatasetInfo)_dataSets.elementAt(index);
                writer.write(info.getName() + "\t" +
		  info.getDataSourceName() + "\t" +
		  "$" + info.getYAttribute() + "\n");
	    }

	    writer.close();

        } catch (IOException ex) {
	    System.err.println("IOException writing summary data values: " +
	      ex.toString());
	    throw new S2DError("Can't write summary data values");

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


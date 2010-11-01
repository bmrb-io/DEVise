// ========================================================================
// DEVise Data Visualization Software
// (c) Copyright 2003-2010
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
// Revision 1.9.10.2  2010/10/19 00:23:21  wenger
// Split the actual sample info out from the sample conditions info,
// including modifying ambiguity code and Pistachio metadata accordingly.
//
// Revision 1.9.10.1  2010/10/15 15:29:03  wenger
// Merged sample_cond_br_0 thru sample_cond_br_1/sample_cond_br_end to
// sample_cond2_br (to get the latest code refactoring from the trunk
// into the sample conditions code).
//
// Revision 1.9.8.1  2010/10/08 21:17:42  wenger
// We now put save frame details into the drill-down data for the data
// selection view in 3D visualizations; also fixed a bug in getting save
// frame details for 3.0/3.1 files.
//
// Revision 1.9  2010/02/17 18:48:41  wenger
// Fixed bug 093 (incorrect entity assembly IDs in 3D data sets).
//
// Revision 1.8  2009/03/26 21:29:08  wenger
// Found and fixed bug 081 (needed to force 3D metadata to be ordered
// by entity assembly, so we don't have gaps if save frames are not
// ordered by entity assembly).
//
// Revision 1.7  2009/03/11 19:53:15  wenger
// Implemented two-stage selection of data sets in coordinate
// visualizations (select entity assembly, then select data set); updated
// tests accordingly.  Updated test14 because it now generates data for
// entity assembly 1.
//
// Revision 1.6  2008/07/23 21:22:30  wenger
// Fixed bug 061 (problems with 3D data selection view if there are too
// many data sources); visualization help still needs updates.
//
// Revision 1.5  2008/07/02 16:29:19  wenger
// S2 order parameter visualizations are done and approved by Eldon;
// tests at least partially updated for S2 order stuff;
// reversed the order of data sets in the data selection view of
// 3D visualizations (more closely matches the summary page); minor
// fix to testclean target in top-level makefile; minor fix to
// relaxation session template (bar widths now set); added indices
// to data set titles in 3D visualizations.
//
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
    private Vector _dataSets; // Vector of S2DDatasetInfo

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
            for (int index = 0; index < _dataSets.size(); index++) {
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
	    writer.write("# Attributes: Entity_assembly_name; " +
	      "Entity_assembly_ID; name; " +
	      "Details; Sample; Sample_conditions; " +
	      "data_source; y_attribute\n");
            writer.write("# Peptide-CGI version: " +
	      S2DMain.PEP_CGI_VERSION + "\n");
            writer.write("# Generation date: " +
	      S2DMain.getTimestamp() + "\n");
	    writer.write("#\n");

	    // Sort metadata (fixes bug 081).
            Collections.sort(_dataSets, new DataSetComparator());

            for (int index = 0; index < _dataSets.size(); index++) {
                S2DDatasetInfo info =
		  (S2DDatasetInfo)_dataSets.elementAt(index);
                writer.write(info.getEntityAssemblyName() + "\t" +
		  info.getEntityAssemblyID() + "\t" +
		  info.getName() + "\t" +
		  info.getDetails() + "\t" +
		  info.getSample() + "\t" +
		  info.getSampleConditions() + "\t" +
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

    //-------------------------------------------------------------------
    // Comparator for sorting S2DDatasetInfo Vectors.
    class DataSetComparator implements Comparator 
    {
        public int compare(Object o1, Object o2)
	{
	    String s1 = ((S2DDatasetInfo)o1).getEntityAssemblyName();
	    String s2 = ((S2DDatasetInfo)o2).getEntityAssemblyName();
	    return s1.compareTo(s2);
	}
    }
}

// ========================================================================


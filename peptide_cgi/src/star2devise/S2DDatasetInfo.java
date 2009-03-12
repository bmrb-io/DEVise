// ========================================================================
// DEVise Data Visualization Software
// (c) Copyright 2002-2009
// By the DEVise Development Group
// Madison, Wisconsin
// All Rights Reserved.
// ========================================================================

// Under no circumstances is this software to be copied, distributed,
// or altered in any way without prior permission from the DEVise
// Development Group.

// ------------------------------------------------------------------------

// This class stores information about individual data sets that is
// needed for the summary data set.

// ------------------------------------------------------------------------

// $Id$

// $Log$
// Revision 1.4  2009/03/11 19:53:14  wenger
// Implemented two-stage selection of data sets in coordinate
// visualizations (select entity assembly, then select data set); updated
// tests accordingly.  Updated test14 because it now generates data for
// entity assembly 1.
//
// Revision 1.3  2007/08/20 20:26:07  wenger
// Added -verb command-line flag and property so we can turn on debug
// output without recompiling; added debug_level property corresponding
// to the existing -debug command-line flag.
//
// Revision 1.2  2006/02/01 20:23:10  wenger
// Merged V2_1b4_br_0 thru peptide_cgi_10_8_0_base to the
// trunk.
//
// Revision 1.1.2.1  2003/04/22 21:58:14  wenger
// Added package name to peptide-cgi java code and put everything into
// a single jar file; version is now 6.0.
//
// Revision 1.1.4.1  2003/01/14 16:51:47  wenger
// Merged V2_1b3_br_0 thru V2_1b3_br_end to V2_1b4_br.
//
// Revision 1.1.2.3  2002/08/21 20:40:49  wenger
// Selection of data set to view now works, but there is not yet selection
// on the data linked to the 3D view.
//
// Revision 1.1.2.2  2002/08/21 15:59:38  wenger
// Changed all schemas to have Residue_seq_code as the attribute name
// we want to display for the sequence number, to make setting up the
// 4096-type 3D visualizations easier.
//
// Revision 1.1.2.1  2002/08/21 14:21:29  wenger
// Working on 4096-type 3D visualization -- added data and view to show
// list of available data sets.
//

// ========================================================================

package star2devise;

public class S2DDatasetInfo
{
    //===================================================================
    // VARIABLES

    private static final int DEBUG = 0;

    private String _name;
    private String _dataSourceName;
    private String _yAttribute;
    private String _schemaFile;
    private String _schemaType;
    private String _entityAssemblyName;
    private int _entityAssemblyID;

    //===================================================================
    // PUBLIC METHODS

    //-------------------------------------------------------------------
    public S2DDatasetInfo(String name, String dataSourceName,
      String yAttribute, String schemaFile, String schemaType,
        int entityAssemblyID, int polymerType)
    {
        if (doDebugOutput(11)) {
	    System.out.println("S2DDatasetInfo.S2DDataSetInfo(" + name +
	      ", " + dataSourceName + ", " + yAttribute);
        }

	_name = name;
	_dataSourceName = dataSourceName;
	_yAttribute = yAttribute;
	_schemaFile = schemaFile;
	_schemaType = schemaType;
	String polymerTypeName = 
	  S2DResidues.getPolymerTypeName(polymerType);
	_entityAssemblyName = "EA " + entityAssemblyID + " (" +
	  polymerTypeName + ")";
	_entityAssemblyID = entityAssemblyID;
    }

    //-------------------------------------------------------------------
    public String getName() { return _name; }

    //-------------------------------------------------------------------
    public String getDataSourceName() { return _dataSourceName; }

    //-------------------------------------------------------------------
    public String getYAttribute() { return _yAttribute; }

    //-------------------------------------------------------------------
    public String getSchemaFile() { return _schemaFile; }

    //-------------------------------------------------------------------
    public String getSchemaType() { return _schemaType; }

    //-------------------------------------------------------------------
    public String getEntityAssemblyName() { return _entityAssemblyName; }

    //-------------------------------------------------------------------
    public int getEntityAssemblyID() { return _entityAssemblyID; }

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


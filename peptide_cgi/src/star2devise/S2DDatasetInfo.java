// ========================================================================
// DEVise Data Visualization Software
// (c) Copyright 2002-2010
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
// Revision 1.6.2.4  2010/10/29 14:32:34  wenger
// Added test of sample conditions with 6838; fixed code to make sure
// we don't get multiple copies of the same sample info.
//
// Revision 1.6.2.3  2010/10/22 19:45:02  wenger
// We now truncate and add "..." to the end of the details, sample, and
// sample conditions strings if they're longer than 100 characters.
//
// Revision 1.6.2.2  2010/10/19 00:23:20  wenger
// Split the actual sample info out from the sample conditions info,
// including modifying ambiguity code and Pistachio metadata accordingly.
//
// Revision 1.6.2.1  2010/10/15 15:29:02  wenger
// Merged sample_cond_br_0 thru sample_cond_br_1/sample_cond_br_end to
// sample_cond2_br (to get the latest code refactoring from the trunk
// into the sample conditions code).
//
// Revision 1.6  2010/10/13 20:44:02  wenger
// Finished restructuring Peptide-CGI code so that we get values from
// the STAR files in the relevant object constructors, instead of in the
// S2DMain class.
//
// Revision 1.5.24.1  2010/10/08 21:17:41  wenger
// We now put save frame details into the drill-down data for the data
// selection view in 3D visualizations; also fixed a bug in getting save
// frame details for 3.0/3.1 files.
//
// Revision 1.5  2009/03/12 17:30:19  wenger
// Changed entity assembly names to things like "EA 1 (polypeptide(L))"
// as requested by Eldon; changed tests accordingly.
//
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
    private String _details;
    private String _sample;
    private String _sampleConditions;

    // See schema file bmrb-summary.
    private final int MAX_LENGTH = 100;

    //===================================================================
    // PUBLIC METHODS

    //-------------------------------------------------------------------
    // Constructor.
    public S2DDatasetInfo(String name, String details,
      String sample, String sampleConditions, String dataSourceName,
      String yAttribute, String schemaFile, String schemaType,
        int entityAssemblyID, int polymerType)
    {
        if (doDebugOutput(11)) {
	    System.out.println("S2DDatasetInfo.S2DDatasetInfo(" + name +
	      ", " + dataSourceName + ", " + yAttribute);
        }

	// Note: we might want to eventually collapse multiple consecutive
	// spaces to a single space in these fields.  wenger 2010-10-28
	_name = name;
	_details = details;
	if (_details != null) {
	    _details = _details.replace("\n", " ");
	    _details = _details.replace("\t", " ");
	    if (_details.length() > MAX_LENGTH) {
	        _details = _details.substring(0, MAX_LENGTH-3) + "...";
	    }
	}

	_sample = sample;
	if (_sample != null) {
	    _sample = _sample.replace("\n", " ");
	    _sample = _sample.replace("\t", " ");
	    if (_sample.length() > MAX_LENGTH) {
	        _sample = _sample.substring(0, MAX_LENGTH-3) + "...";
	    }
	}

	_sampleConditions = sampleConditions;
	if (_sampleConditions != null) {
	    _sampleConditions= _sampleConditions.replace("\n", " ");
	    _sampleConditions = _sampleConditions.replace("\t", " ");
	    if (_sampleConditions.length() > MAX_LENGTH) {
	        _sampleConditions =
		  _sampleConditions.substring(0, MAX_LENGTH-3) + "...";
	    }
	}

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
    public String getDetails() { return _details; }

    //-------------------------------------------------------------------
    public String getSample() { return _sample; }

    //-------------------------------------------------------------------
    public String getSampleConditions() { return _sampleConditions; }

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


// ========================================================================
// DEVise Data Visualization Software
// (c) Copyright 2000-2007
// By the DEVise Development Group
// Madison, Wisconsin
// All Rights Reserved.
// ========================================================================

// Under no circumstances is this software to be copied, distributed,
// or altered in any way without prior permission from the DEVise
// Development Group.

// ------------------------------------------------------------------------

// This class reads an NMR-Star file and generates data and session files
// to be used by DEVise, and a summary html page and individual html pages
// for each session.

// ------------------------------------------------------------------------

// $Id$

// $Log$
// Revision 1.55  2007/11/17 16:32:02  wenger
// Merged peptide_cgi_na_histograms_br_0 thru
// peptide_cgi_na_histograms_br_1 to trunk.
//
// Revision 1.54  2007/11/15 17:15:34  wenger
// Cleaned out cvs history in source files.
//
// Revision 1.53  2007/11/14 20:07:17  wenger
// Made improvements to test37 and test47; we now print all input parameters
// at verbosity level 1 instead of 2.
//
// Revision 1.52  2007/11/14 18:49:02  wenger
// Replaced all test NMR-STAR 3.1 files with the latest versions,
// changed test_all accordingly (because this fixes earlier problems).
//
// Revision 1.51.2.1  2007/10/31 16:09:25  wenger
// Updated Peptide-CGI version to 11.2.0x1 -- I figure adding the nucleic
// acid histograms is enough of a change for a minor version change.
//
// Revision 1.51  2007/10/08 19:52:50  wenger
// Version string is now 11.1.6x1 because 11.1.5 has been released.
//
// Revision 1.50  2007/10/08 18:57:40  wenger
// Ready for release -- removed 'x' part of version.
//
// ...

// ========================================================================

package star2devise;

import EDU.bmrb.starlibj.SaveFrameNode;
import java.util.*;
import java.io.*;
import java.text.*;

// for chemical shifts

public class S2DMain {
    //===================================================================
    // VARIABLES

    private static final int DEBUG = 0;
    public static int _verbosity = 0;

    public static final String PEP_CGI_VERSION = "11.2.0";
    public static final String DEVISE_MIN_VERSION = "1.9.0";

    private String _masterBmrbId = ""; // accession number the user requested

    // A list of all BMRB IDs (now stored as Strings) we need to access
    // for this entry.
    private Vector _bmrbIds = new Vector();

    // A list of all local files to process.
    private Vector _localFiles = new Vector();

    // A list of related PDB files we want to process for this entry.
    private Vector _pdbIds = new Vector(); // Vector contains Strings

    private String _dataDir;
    private String _csrDataDir = null;
    private String _htmlDir;
    private String _sessionDir;
    private String _connectionFile;

    // The name of this data set -- all output files generated will consist
    // of the name plus various suffixes.
    private String _name = "";
    private String _longName = null;
    
    private boolean _force = false;
    private boolean _useLocalFiles = false;
    private boolean _checkResList = false;

    private static final int PDB_LEVEL_NONE = 0;
    private static final int PDB_LEVEL_LINK = 1;
    private static final int PDB_LEVEL_PROCESS = 2;
    //TEMP -- _pdbLevel should really be something like _coordLevel
    private int _pdbLevel = PDB_LEVEL_LINK;

    private static final int CSR_LEVEL_NONE = 0;
    private static final int CSR_LEVEL_LINK = 1;
    private static final int CSR_LEVEL_PROCESS = 2;
    private int _csrLevel = CSR_LEVEL_NONE;

    // LACS-related info.
    private static final int LACS_LEVEL_NONE = 0;
    private static final int LACS_LEVEL_TRY = 1;
    private static final int LACS_LEVEL_MANDATORY = 2;
    private int _lacsLevel = LACS_LEVEL_NONE;
    private String _lacsFile = null;

    private boolean _haveCoords = false;

    private S2DSummaryHtml _summary;

    private static boolean _retrying;

    private String _cmdPdbId = null; // PDB ID from command line
    private String _currentPdbId = null; // PDB ID we're processing

    // PDB ID(s) for chem shift reference processing.
    private Vector _csrPdbIds = new Vector(); // Vector contains Strings.

    private int _cmdFrameIndex = -1;
    private int _currentFrameIndex = -1;

    private Vector _dataSets = new Vector();

    private boolean _doProteinCheck = true;

    private boolean _runScripts = true;

    private boolean _savedResList = false;

    private boolean _isUvd = false;

    private static final int CSR_TIMEOUT_DEFAULT = 120;
    private int _csrTimeout = CSR_TIMEOUT_DEFAULT;

    private static String _timestamp;
    private static String _shortTimestamp;

    private static int AMBIGUITY_WRAP_LENGTH = 10;
    private static int PISTACHIO_WRAP_LENGTH = 10;

    //===================================================================
    // PUBLIC METHODS

    //-------------------------------------------------------------------
    public static void main(String args[]) throws S2DException
    {
	_retrying = false;

	setTimestamps();

	try {
	    run(args);
	} catch (S2DCancel ex) {
	    // Just catch this so we don't pass it on to higher levels.
	} catch (S2DWarning ex) {
	    System.err.println("Top-level warning: ");
	    System.err.println(ex.toString());
	} catch (Exception ex) {
	    System.err.println("Top-level error: ");
	    ex.printStackTrace();
	    System.err.println(ex.toString());
	    throw new S2DError("NMR-Star to DEVise conversion failed");
	}
    }

    //-------------------------------------------------------------------
    // Do the conversion, retrying without PDB if we fail because of
    // running out of memory and we were trying to get data from PDB.
    public static void run(String args[]) throws S2DException
    {
        S2DMain s2d = new S2DMain(args);

	if (_retrying) {
	    s2d._pdbLevel = PDB_LEVEL_NONE;
	    s2d._csrLevel = CSR_LEVEL_NONE;
	    s2d._lacsLevel = LACS_LEVEL_NONE;
	}

	try {
	    if (!s2d.useCache()) {
	        s2d.process();
	    } else {
	        if (doDebugOutput(1)) {
		    System.out.println("Cache files used");
		}
	    }
        } catch (OutOfMemoryError mem) {
	    System.err.println("OutOfMemoryError: " + mem.toString());
	    if (s2d._pdbLevel == PDB_LEVEL_PROCESS) {
	        System.out.println("Retrying without accessing PDB");

		s2d = null;
		System.gc();
	        _retrying = true;
		run(args);
	    } else {
	        throw new S2DError("NMR-Star to DEVise conversion failed " +
		  "because of insufficient memory");
	    }
	}
    }

    //-------------------------------------------------------------------
    // Get a timestamp to be used as the "processing time" for all files
    // we generate.
    public static String getTimestamp() { return _timestamp; }

    //-------------------------------------------------------------------
    // Get a short timestamp (date only).
    public static String getShortTimestamp() { return _shortTimestamp; }
    
    //-------------------------------------------------------------------
    // Constructor.
    public S2DMain(String args[]) throws S2DException
    {
        if (doDebugOutput(2)) {
	    String sep = "";
	    System.out.print("S2DMain.S2DMain(");
	    for (int index = 0; index < args.length; index++) {
	        System.out.print(sep + args[index]);
		sep = ", ";
	    }
	    System.out.print(")\n");
	}

	checkArgsNoProps(args);

	// getProperties must come before checkArgs, so command-line args
	// can override defaults in properties.
	Properties props = getProperties();
	getPropertiesDynamic(props);

	checkArgs(args);
    }

    //===================================================================
    // PRIVATE METHODS

    //-------------------------------------------------------------------
    public static void setTimestamps()
    {
        Date date = new Date();
	DateFormat dtf = DateFormat.getDateTimeInstance(DateFormat.MEDIUM,
	  DateFormat.MEDIUM);
	_timestamp = dtf.format(date);

	dtf = DateFormat.getDateInstance(DateFormat.MEDIUM);
	_shortTimestamp = dtf.format(date);
    }

    //-------------------------------------------------------------------
    // Get configuration-specific properties and set variables accordingly.
    public static Properties getProperties() throws S2DException
    {
        Properties props = new Properties();
	try {
	    props.load(new BufferedInputStream(
	      new FileInputStream("s2d.props")));
        } catch (FileNotFoundException ex) {
	    throw new S2DError("Can't find s2d properties file: " +
	      ex.toString());
	} catch (IOException ex) {
	    throw new S2DError("Error reading s2d properties file: " +
	      ex.toString());
	}

	S2DNames.BMRB_STAR_URL = props.getProperty("bmrb_mirror.star_url");
	if (S2DNames.BMRB_STAR_URL == null) {
	    throw new S2DError("Unable to get value for " +
	      "bmrb_mirror.star_url property");
	}

	S2DNames.PDB_FILE_URL = props.getProperty("bmrb_mirror.pdb_file_url");
	if (S2DNames.PDB_FILE_URL == null) {
	    throw new S2DError("Unable to get value for " +
	      "bmrb_mirror.pdb_file_url property");
	}

	S2DNames.BMRB_3D_URL = props.getProperty("bmrb_mirror.3d_url");
	if (S2DNames.BMRB_3D_URL == null) {
	    throw new S2DError("Unable to get value for " +
	      "bmrb_mirror.3d_url property");
	}

	S2DNames.CGI_URL = props.getProperty("bmrb_mirror.cgi_url");
	if (S2DNames.CGI_URL == null) {
	    throw new S2DError("Unable to get value for " +
	      "bmrb_mirror.cgi_url property");
	}

	S2DNames.UVD_CGI_URL = props.getProperty("bmrb_mirror.uvd_cgi_url");

	S2DNames.STAR_NAME_TEMPLATE = props.getProperty(
	  "bmrb_mirror.star_name_template");
	if (S2DNames.STAR_NAME_TEMPLATE == null) {
	    if (doDebugOutput(2)) {
	        System.out.println("bmrb_mirror.star_name_template " +
		  "property value not defined; using default");
	    }
	    S2DNames.STAR_NAME_TEMPLATE = "bmr*.str";
	}

	S2DNames.COMMENT_EMAIL = props.getProperty(
	  "bmrb_mirror.comment_email");
	if (S2DNames.COMMENT_EMAIL == null) {
	    throw new S2DError("Unable to get value for " +
	      "bmrb_mirror.comment_email property");
	}

	S2DNames.LACS_NAME_TEMPLATE = props.getProperty(
	  "bmrb_mirror.lacs_name_template");
	if (S2DNames.LACS_NAME_TEMPLATE == null) {
	    if (doDebugOutput(2)) {
	        System.out.println("bmrb_mirror.lacs_name_template " +
		  "property value not defined; using default");
	    }
	    S2DNames.LACS_NAME_TEMPLATE = "bmr*_LACS.out";
	}

	String debugTmp = props.getProperty("bmrb_mirror.debug_level");
	if (debugTmp == null) {
	    System.err.println(new S2DWarning("Unable to get value for " +
	      "bmrb_mirror.debug_level property"));
	} else {
	    try {
		S2DException.setDebugLvl(Integer.parseInt(debugTmp));
	    } catch(NumberFormatException ex) {
	        System.err.println(new S2DWarning("Error parsing " +
		  "debug_level value " + ex.toString() +
		  "; using default"));
	    }
	}

	String verbTmp = props.getProperty("bmrb_mirror.verbosity");
	if (verbTmp == null) {
	    System.err.println(new S2DWarning("Unable to get value for " +
	      "bmrb_mirror.verbosity property"));
	} else {
	    try {
		_verbosity = Integer.parseInt(verbTmp);
	    } catch(NumberFormatException ex) {
	        System.err.println(new S2DWarning("Error parsing " +
		  "verbosity value " + ex.toString() +
		  "; using default"));
	    }
	}

        return props;
    }

    //-------------------------------------------------------------------
    // Get configuration-specific properties and set variables
    // accordingly, for things that can't be static.
    private void getPropertiesDynamic(Properties props) throws S2DException
    {
	String csrTmp = props.getProperty("bmrb_mirror.do_csr_default");
	if (csrTmp == null) {
	    System.err.println(new S2DWarning("Unable to get value for " +
	      "bmrb_mirror.do_csr_default property"));
	} else {
	    try {
	        _csrLevel = Integer.parseInt(csrTmp);
	    } catch(NumberFormatException ex) {
	        System.err.println(new S2DWarning("Error parsing " +
		  "do_csr_default value " + ex.toString() +
		  "; using default"));
	    }
	}

	String lacsTmp = props.getProperty("bmrb_mirror.lacs_level_default");
	if (lacsTmp == null) {
	    System.err.println(new S2DWarning("Unable to get value for " +
	      "bmrb_mirror.lacs_level_default property; using default"));
	} else {
	    try {
	        _lacsLevel = Integer.parseInt(lacsTmp);
	    } catch(NumberFormatException ex) {
	        System.err.println(new S2DWarning("Error parsing " +
		  "lacs_level_default value " + ex.toString() +
		  "; using default"));
	    }
	}

	S2DNames.LACS_URL = props.getProperty("bmrb_mirror.lacs_url");
	if (S2DNames.LACS_URL == null) {
	    S2DError err = new S2DError("Unable to get value for " +
	      "bmrb_mirror.lacs_url property");
	    if (_lacsLevel > LACS_LEVEL_NONE) {
	        throw err;
	    } else {
	        System.err.println(err + "; using default");
	    }
	}
    }

    //-------------------------------------------------------------------
    // Check arguments to constructor that need to be processed before
    // we try to get properties, set member variables accordingly.
    private void checkArgsNoProps(String args[]) throws S2DException
    {
	//
	// The usage message.
	//
	String usage = 
          "Usage:\n" +
          "    java S2DMain -usage\n" +
          "        or\n" +
          "    java S2DMain -version\n" +
          "        or\n" +
          "    java S2DMain -bmrbid <> [args...]\n" +
          "        or\n" +
          "    java S2DMain -pdbid <> -bmrbid <> -coord_index <> [args...]\n" +
          "        or\n" +
          "    java S2DMain -name <> -file <> [-file <>...] -coord_index <> [args...]\n" +
          "\n" +
          "Options are:\n" +
          "    -bmrbid <value>\n" +
          "        the BMRB accession number to process (e.g., 4081)\n" +
	  "        (mandatory if pdbid is specified)\n" +
	  "    -check_res_list\n" +
	  "        check the residue list against the chem shift loop residues\n" +
          "    -conn_file <filename>\n" +
          "        the connection file for the 3D structures (mandatory)\n" +
	  "    -coord_index <value>\n" +
	  "        the index of coordinate set\n" +
	  "        (mandatory if pdbid is specified)\n" +
          "    -csr_data_dir <directory>\n" +
	  "        the path to the data directory as mounted relative to\n" +
	  "        the Chem Shift Reference server (default is the same as\n" +
	  "        the argument specified for -data_dir)\n" +
          "    -csr_timeout <seconds>\n" +
          "        the timeout for chemical shift reference processing\n" +
	  "        (default is " + CSR_TIMEOUT_DEFAULT + ")\n" +
          "    -data_dir <directory>\n" +
          "        the directory in which to store the data files (mandatory)\n" +
	  "    -debug <value>\n" +
	  "        set the debug level (default is 0 -- messages but no " +
	  "stack traces)\n" +
	  "    -dev_version\n" +
	  "        show the minimum required DEVise version\n" +
          "    -do_csr <0|1|2>\n" +
          "        0: don't do chem shift reference calculations;\n" +
          "        1: create link in summary file but don't do the calculation;\n" +
          "        2: do chem shift reference calculations\n" +
          "        (default is " + _csrLevel + ")\n" +
          "    -do_lacs <0|1|2>\n" +
          "        0: don't attempt LACS processing\n" +
          "        1: attempt LACS processing, failure is not an error\n" +
          "        2: attempt LACS processing, failure is an error\n" +
          "        (default is " + _lacsLevel + " unless -lacs_file is set)\n" +
          "    -do_pdb <0|1|2>\n" +
          "        0: ignore PDB references;\n" +
          "        1: create links in summary file but don't process;\n" +
          "        2: process PDB references\n" +
          "        (default is " + _pdbLevel + " unless -pdbid is set)\n" +
	  "    -file <filename>\n" +
	  "        local file containing data to be processed\n" +
          "    -force\n" +
          "        force the processing to happen even if a suitable set of\n" +
          "        output files already exists\n" +
	  "    -html_dir <directory>\n" +
          "        the directory in which to store the html files (mandatory)\n" +
	  "    -js_data_dir <directory>\n" +
	  "        data directory *relative to the JavaScreen server's base\n" +
	  "        data directory*\n" +
	  "    -local\n" + 
	  "        use local files for BMRB and PDB data (for testing without\n" +
	  "        connecting to web sites)\n" +
	  "    -lacs_file <filename>\n" +
	  "        file containing LACS data to process\n" +
	  "    -lacs_url <url>\n" +
	  "        URL of directory containing LACS files\n" +
	  "    -name <string>\n" + 
	  "        the name of the data set being processed (used only when\n" +
	  "        a bmrbid is not specified)\n" +
	  "    -no_protein_check\n" +
	  "        turn off check that data is for a protein\n" +
	  "    -no_scripts\n" +
	  "        don't run scripts (e.g., running on Windows)\n" +
          "    -pdbid <value>\n" +
          "        the PDB ID to process (e.g., 1dfv)\n" +
          "    -session_dir <directory>\n" +
          "        the directory in which to store the session files (mandatory)\n" +
          "    -usage\n" +
          "        show this message\n" +
	  "    -uvd\n" + 
	  "        indicates that this is \"upload and visualize data\" " +
	  "processing\n" + 
	  "    -verb <value>\n" +
	  "        set the verbosity of diagnostic output (default is 0)\n" +
          "    -version\n" +
          "        show the version number\n" +
          "\n" +
	  "Examples:\n" +
          "    java S2DMain -bmrbid 4101 ...\n" +
          "    java S2DMain -pdbid 1OXM -bmrbid 4101 -coord_index 2 ...\n" +
	  "    java S2DMain -file foo_data -name foo_out ...\n";

	int index = 0;
	while (index < args.length) {
	    if ("-dev_version".equals(args[index])) {
		System.out.println("Peptide-CGI version " + PEP_CGI_VERSION +
		  " requires DEVise version " + DEVISE_MIN_VERSION +
		  " or higher");
		throw new S2DCancel();

	    } else if ("-usage".equals(args[index])) {
	        System.out.println(usage);
		throw new S2DCancel();

	    } else if ("-version".equals(args[index])) {
	        System.out.println(PEP_CGI_VERSION);
		throw new S2DCancel();
	    }

	    index++;
    	}
    }

    //-------------------------------------------------------------------
    // Check arguments to constructor, set member variables accordingly.
    private void checkArgs(String args[]) throws S2DException
    {
        if (doDebugOutput(2)) {
	    System.out.println("Arguments: ");
	    for (int index = 0; index < args.length; index++) {
	        System.out.println("  {" + args[index] + "}");
	    }
	}

	//
	// Reset static variables (for Jafar).
	//
        S2DSession.setJSDataDir(null);
	S2DException.clear();

	//
	// Process the command-line arguments.
	//
	int index = 0;
	while (index < args.length) {

	    if ("-bmrbid".equals(args[index])) {
	        index++;
		if (index >= args.length) {
		    throw new S2DError("-bmrbid argument needs value");
		}
		_name = args[index];
		S2DException.setName(_name);
		_longName = "BMRB " + args[index];
	        _masterBmrbId = args[index];
		addBmrbId(_masterBmrbId);
		if (_masterBmrbId.equals("4038")) {
		    _longName += "/4096";
		} else if (_masterBmrbId.equals("4096")) {
		    _longName += "/4038";
		}

	    } else if ("-check_res_list".equals(args[index])) {
	    	_checkResList = true;

	    } else if ("-conn_file".equals(args[index])) {
	        index++;
		if (index >= args.length) {
		    throw new S2DError("-conn_file argument needs value");
		}
		_connectionFile = args[index];

	    } else if ("-coord_index".equals(args[index])) {
	        index++;
		if (index >= args.length) {
		    throw new S2DError("-coord_index argument needs value");
		}
		try {
	            _cmdFrameIndex = Integer.parseInt(args[index]);
	        } catch(NumberFormatException ex) {
	            System.err.println("Error parsing frame index: " +
		      ex.toString());
	            throw new S2DError("Error parsing frame index " +
		      ex.toString());
	        }

	    } else if ("-csr_data_dir".equals(args[index])) {
	        index++;
		if (index >= args.length) {
		    throw new S2DError("-csr_data_dir argument needs value");
		}
	        _csrDataDir = args[index];

	    } else if ("-csr_timeout".equals(args[index])) {
	        index++;
		if (index >= args.length) {
		    throw new S2DError("-csr_timeout argument needs value");
		}
		try {
	            _csrTimeout = Integer.parseInt(args[index]);
	        } catch(NumberFormatException ex) {
	            System.err.println("Error parsing chem shift ref " +
		      "timeout: " + ex.toString());
	            throw new S2DError("Error parsing chem shift ref " +
		      "timeout: " + ex.toString());
	        }

	    } else if ("-data_dir".equals(args[index])) {
	        index++;
		if (index >= args.length) {
		    throw new S2DError("-data_dir argument needs value");
		}
	        _dataDir = args[index];

	    } else if ("-debug".equals(args[index])) {
	        index++;
		if (index >= args.length) {
		    throw new S2DError("-debug argument needs value");
		}
		try {
		    S2DException.setDebugLvl(Integer.parseInt(args[index]));
	        } catch(NumberFormatException ex) {
	            System.err.println("Error parsing debug value: " +
		      ex.toString());
	            throw new S2DError("Error parsing debug value " +
		      ex.toString());
	        }

	    } else if ("-do_csr".equals(args[index])) {
	        index++;
		if (index >= args.length) {
		    throw new S2DError("-do_csr argument needs value");
		}
		try {
	            _csrLevel = Integer.parseInt(args[index]);
	        } catch(NumberFormatException ex) {
	            System.err.println("Error parsing do_csr value: " +
		      ex.toString());
	            throw new S2DError("Error parsing do_csr value " +
		      ex.toString());
	        }

	    } else if ("-do_lacs".equals(args[index])) {
	        index++;
		if (index >= args.length) {
		    throw new S2DError("-do_lacs argument needs value");
		}
		try {
	            _lacsLevel = Integer.parseInt(args[index]);
	        } catch(NumberFormatException ex) {
	            System.err.println("Error parsing do_lacs value: " +
		      ex.toString());
	            throw new S2DError("Error parsing do_lacs value " +
		      ex.toString());
	        }

	    } else if ("-do_pdb".equals(args[index])) {
	        index++;
		if (index >= args.length) {
		    throw new S2DError("-do_pdb argument needs value");
		}
		try {
	            _pdbLevel = Integer.parseInt(args[index]);
	        } catch(NumberFormatException ex) {
	            System.err.println("Error parsing do_pdb value: " +
		      ex.toString());
	            throw new S2DError("Error parsing do_pdb value " +
		      ex.toString());
	        }

	    } else if ("-file".equals(args[index])) {
	        index++;
		if (index >= args.length) {
		    throw new S2DError("-file argument needs value");
		}
		_localFiles.addElement(args[index]);

	    } else if ("-force".equals(args[index])) {
		_force = true;

	    } else if ("-html_dir".equals(args[index])) {
	        index++;
		if (index >= args.length) {
		    throw new S2DError("-html_dir argument needs value");
		}
	        _htmlDir = args[index];

	    } else if ("-js_data_dir".equals(args[index])) {
	        index++;
		if (index >= args.length) {
		    throw new S2DError("-js_data_dir argument needs value");
		}
	        S2DSession.setJSDataDir(args[index]);

	    } else if ("-lacs_file".equals(args[index])) {
	        index++;
		if (index >= args.length) {
		    throw new S2DError("-lacs_file argument needs value");
		}
		_lacsFile = args[index];
		_lacsLevel = LACS_LEVEL_MANDATORY;

	    } else if ("-lacs_url".equals(args[index])) {
	        index++;
		if (index >= args.length) {
		    throw new S2DError("-lacs_url argument needs value");
		}
		S2DNames.LACS_URL = args[index];

	    } else if ("-local".equals(args[index])) {
		_useLocalFiles = true;

	    } else if ("-name".equals(args[index])) {
	        index++;
		if (index >= args.length) {
		    throw new S2DError("-name argument needs value");
		}
		_name = args[index];
		S2DException.setName(_name);
		_longName = args[index];

	    } else if ("-no_protein_check".equals(args[index])) {
		_doProteinCheck = false;

	    } else if ("-no_scripts".equals(args[index])) {
		_runScripts = false;

	    } else if ("-pdbid".equals(args[index])) {
	        index++;
		if (index >= args.length) {
		    throw new S2DError("-pdbid argument needs value");
		}
	        _cmdPdbId = args[index];
		_pdbIds.addElement(_cmdPdbId);
		_csrPdbIds.addElement(_cmdPdbId);

	    } else if ("-session_dir".equals(args[index])) {
	        index++;
		if (index >= args.length) {
		    throw new S2DError("-session_dir argument needs value");
		}
	        _sessionDir = args[index];

	    } else if ("-uvd".equals(args[index])) {
	        if (S2DNames.UVD_CGI_URL == null) {
	            throw new S2DError("Unable to get value for " +
	              "bmrb_mirror.uvd_cgi_url property");
	        }
		_isUvd = true;
	        S2DSpecificHtml.setIsUvd(true);
	        S2DSummaryHtmlGen.setIsUvd(true);

	    } else if ("-verb".equals(args[index])) {
	        index++;
		if (index >= args.length) {
		    throw new S2DError("-verb argument needs value");
		}
		try {
		    _verbosity = Integer.parseInt(args[index]);
	        } catch(NumberFormatException ex) {
	            System.err.println("Error parsing verbosity value: " +
		      ex.toString());
	            throw new S2DError("Error parsing verbosity value " +
		      ex.toString());
	        }

	    } else {
	        throw new S2DError("Unrecognized command-line argument: " +
		  args[index]);
	    }
	    index++;
	}

	//
	// Check for illegal combinations of arguments.
	// TEMP: can bmrbid and local file *both* be specified?  If not,
	// we should give an error if they both are.
	//
	if (_masterBmrbId.equals("") && _cmdPdbId == null &&
	  _localFiles.size() == 0) {
	    throw new S2DError(
	      "bmrbid, pdbid, or local file must be specified");
	}

	if (_masterBmrbId.equals("") && _name.equals("") &&
	  _cmdPdbId != null) {
	    throw new S2DError(
	      "bmrbid or name must be specified when pdbid is specified");
	}

	if (_cmdPdbId == null && _cmdFrameIndex != -1) {
	    throw new S2DError(
	      "coord_index must not be specified when pdbid is not specified");
	}

	if (_connectionFile == null) {
	    throw new S2DError("conn_file must be specified");
	}

	if (_dataDir == null) {
	    throw new S2DError("data_dir must be specified");
	}

	if (_sessionDir == null) {
	    throw new S2DError("session_dir must be specified");
	}

	if (_htmlDir == null) {
	    throw new S2DError("html_dir must be specified");
	}
	
	if (_name.equals("")) {
	    throw new S2DError("name must be specified if bmrbid is not");
	}

	// Name cannot contain a dot because that will end up in the
	// data source names and goof up DEVise when it tries to open
	// the session.
	if (_name.indexOf(".") >= 0) {
	    throw new S2DError("illegal name: " + _name +
	      " (cannot contain a dot ('.'))");
	}

	if (_pdbLevel < PDB_LEVEL_NONE || _pdbLevel > PDB_LEVEL_PROCESS) {
	    throw new S2DError("illegal do_pdb value: " + _pdbLevel +
	      " (must be between " + PDB_LEVEL_NONE + " and " +
	      PDB_LEVEL_PROCESS + ")");
	}

	if (_csrLevel < CSR_LEVEL_NONE || _csrLevel > CSR_LEVEL_PROCESS) {
	    throw new S2DError("illegal do_csr value: " + _csrLevel +
	      " (must be between " + CSR_LEVEL_NONE + " and " +
	      CSR_LEVEL_PROCESS + ")");
	}

	if (_lacsLevel < LACS_LEVEL_NONE ||
	  _lacsLevel > LACS_LEVEL_MANDATORY) {
	    throw new S2DError("illegal do_lacs value: " + _lacsLevel +
	      " (must be between " + LACS_LEVEL_NONE + " and " +
	      LACS_LEVEL_MANDATORY + ")");
	}
	
	//
	// Set some defaults.
	//
	if (_csrDataDir == null) {
	    _csrDataDir = _dataDir;
	}

	if (_isUvd && _lacsFile == null) {
	    //TEMP -- we probably want to make this more general, but
	    // the real simple case should do for now.
	    _lacsFile = S2DUtils.replace((String)_localFiles.elementAt(0),
	      ".str", ".LACS.str");
	}

        if (doDebugOutput(1)) {
	    System.out.println("_name = {" + _name + "}");
	    System.out.println("_longName = {" + _longName + "}");
	    System.out.println("_masterBmrbId/BMRB ID = " + _masterBmrbId);
	    System.out.println("_cmdPdbId = " + _cmdPdbId);
	    System.out.println("_connectionFile = {" + _connectionFile + "}");
	    System.out.println("_cmdFrameIndex = " + _cmdFrameIndex);
	    System.out.println("_csrDataDir = {" + _csrDataDir + "}");
	    System.out.println("_csrTimeout = " + _csrTimeout);
	    System.out.println("_dataDir = {" + _dataDir + "}");
	    System.out.println("_csrLevel = " + _csrLevel);
	    System.out.println("_pdbLevel = " + _pdbLevel);
	    System.out.println("_lacsLevel = " + _lacsLevel);
	    System.out.println("_lacsFile = " + _lacsFile);
	    if (_localFiles.size() > 0) {
	        System.out.println("localFile = {" +
		  _localFiles.elementAt(0) + "}");
	    }
	    System.out.println("_force = " + _force);
	    System.out.println("_htmlDir = {" + _htmlDir + "}");
	    System.out.println("_useLocalFiles = " + _useLocalFiles);
	    System.out.println("_doProteinCheck = " + _doProteinCheck);
	    System.out.println("_runScripts = " + _runScripts);
	    System.out.println("_sessionDir = {" + _sessionDir + "}");
	}
    }

    // Decision table for whether to use cached PDB data:

    // Coord session file exists	n y y y y y y
    // Session file out of date		- y n n n n n
    // Summary file exists		* * y y y y n
    // Summary file out of date		* * y y n n -
    // Session file newer than summary	- * y n y n -

    // Use cached data			n n n - y n n

    // * = don't care
    // - = not defined/can't happen

    //-------------------------------------------------------------------
    // Decide whether to use cached versions of the relevant output files.
    private boolean useCache()
    {
	if (doDebugOutput(2)) {
	    System.out.println("S2DMain.useCache()");
	}

        boolean result = false;

	check:
	if (!_force) {
	    //
	    // Get peptide-cgi version, generation timestamp, etc., from
	    // the appropriate summary HTML file.
	    //
	    S2DSummaryFileData summaryData = new S2DSummaryFileData();
	    boolean summaryFileOk = summaryData.getSummaryData(_name,
	      _htmlDir, _masterBmrbId);
            if (!summaryFileOk) {
	        break check;
	    }

	    //
	    // Compare summary file version to current peptide-cgi software
	    // version.
	    //
	    if (!PEP_CGI_VERSION.equals(summaryData.fileVersion)) {
		if (doDebugOutput(2)) {
		    System.out.println("Existing summary html file version (" +
		      summaryData.fileVersion + ") does not " +
		      "match current software version (" + PEP_CGI_VERSION +
		      "); cache not used");
		}
	        break check;
	    }

	    //
	    // Compare the generation date of the summary html file to the
	    // modification date for all related NMR-STAR file(s).
	    //
	    for (int index = 0; index < summaryData.bmrbIds.size(); index++) {
	        String id = (String)summaryData.bmrbIds.elementAt(index);
		if (!id.equals("")) {
	            Date starModDate = S2DNmrStarIfc.getModDate(id, false);
	            if (starModDate == null ||
		      starModDate.after(summaryData.fileDate)) {
		        if (doDebugOutput(2)) {
		            System.out.println("Existing summary html file " +
		              "is older than NMR-STAR file; cache not used");
		        }
	                break check;
	            }
		}
	    }

	    for (int index = 0; index < _localFiles.size(); index++) {
	        String filename = (String)_localFiles.elementAt(index);
		if (!filename.equals("")) {
	            Date starModDate =
		      S2DNmrStarIfc.getModDateFile(filename);
	            if (starModDate == null ||
		      starModDate.after(summaryData.fileDate)) {
		        if (doDebugOutput(2)) {
		            System.out.println("Existing summary html file " +
		              "is older than NMR-STAR file; cache not used");
		        }
	                break check;
	            }
		}
	    }

	    //
	    // Compare the generation date of the summary html file to the
	    // modification date of any related PDB files.
	    //
	    if (_pdbLevel == PDB_LEVEL_PROCESS) {
		Vector pdbIds2Check = null;
	        if (_cmdPdbId != null) {
		        // PDB ID was specified on the command line -- check
		        // only that PDB ID.
		    pdbIds2Check = new Vector();
		    pdbIds2Check.addElement(_cmdPdbId);
		} else {
		        // We're processing all related PDB IDs -- check
		        // them all.
		    pdbIds2Check = summaryData.pdbIds;
		}

	        for (int index = 0; index < pdbIds2Check.size(); index++) {
	            String id = (String)pdbIds2Check.elementAt(index);
		    if (doDebugOutput(2)) {
                        System.out.println("  Checking PDB ID " + id);
		    }
	            Date pdbModDate = S2DmmCifIfc.getModDate(id);
		        // Note: if a PDB ID was specified on the command
			// line we really should compare the PDB file date
			// against the session-specific html file here,
			// instead of the summary html file, but I'm
			// leaving it for now.  wenger 2004-11-03.
	            if (pdbModDate == null ||
		      pdbModDate.after(summaryData.fileDate)) {
		        if (doDebugOutput(2)) {
		            System.out.println("Existing summary html file " +
			      "is older than PDB file; cache not used");
		        }
	                break check;
	            }
	        }
	    }

	    if (_cmdPdbId != null) {
		if (_pdbLevel == PDB_LEVEL_PROCESS) {
		    if (!sessionFileUseCache(_name,
		      S2DNames.ATOMIC_COORD_SUFFIX + _cmdFrameIndex,
		      _sessionDir, summaryData)) {
		        break check;
		    }
		}

		if (_csrLevel == CSR_LEVEL_PROCESS) {
		    if (!sessionFileUseCache(_name,
		      S2DNames.CSR_SUFFIX + "1" + _cmdFrameIndex,
		      _sessionDir, summaryData)) {
		        break check;
		    }
		}
	    }

	    result = true;
	}

	if (doDebugOutput(2)) {
	    System.out.println("S2DMain.useCache() returns " + result);
	}

	return result;
    }

    //-------------------------------------------------------------------
    // Determine whether the given session file allows us to use the
    // cache.  (Note that if *any* test shows the cache to be invalid,
    // we consider the cache invalid.)
    private boolean sessionFileUseCache(String name, String suffix,
      String sessionDir, S2DSummaryFileData summaryData)
    {
        //
	// Get peptide-cgi version and generation timestamp from
	// the appropriate session file.
	//
	S2DSessionFileData sessionData = new S2DSessionFileData();
	boolean sessionFileOk = sessionData.getSessionData(name, suffix,
	  sessionDir);
        if (!sessionFileOk) {
	    return false;
	}

	//
	// Compare session file version to current peptide-cgi software
	// version.
	//
	if (!PEP_CGI_VERSION.equals(sessionData.fileVersion)) {
	    if (doDebugOutput(2)) {
	        System.out.println("Existing session file version (" +
	          sessionData.fileVersion + ") does not " +
	          "match current software version (" + PEP_CGI_VERSION +
	          "); cache not used");
	    }
	    return false;
	}

	//
	// If the session file is not newer than the summary file,
	// don't use the cached coordinate data.
	//
	if (!sessionData.fileDate.after(summaryData.fileDate)) {
	    if (doDebugOutput(2)) {
	        System.out.println("Existing session is not newer " +
		  "than summary file; cache not used");
	    }
	    return false;
	}

	return true;
    }

    //-------------------------------------------------------------------
    // Add an accession number to the list to be processed (avoiding
    // duplicates).
    private void addBmrbId(String bmrbId)
    {
        if (doDebugOutput(3)) {
            System.out.println("addBmrbId(" + bmrbId + ")");
        }

        if (!_bmrbIds.contains(bmrbId)) {
	    _bmrbIds.add(bmrbId);
	}
    }

    //-------------------------------------------------------------------
    // Add PDB IDs to the list of PDB files to be processed.
    private void addPDB(Vector ids)
    {
        if (doDebugOutput(3)) {
            System.out.println("addPDB()");
        }

	for (int index = 0; index < ids.size(); index++) {
	    String id = (String)ids.elementAt(index);

	    // Use only the first PDB ID.
	    //TEMP -- comment this out to process all relevant PDB IDs.
	    // if (_pdbIds.size() > 0) break;

	    // Avoid duplicates.
	    if (!_pdbIds.contains(id)) {
	        _pdbIds.addElement(id);
	    }
	}

	// If there's no chem shift reference PDB ID already specified, 
	// save all of the PDB IDs in the chem shift ref list.
	if (_csrPdbIds.size() == 0) {
	    for (int index = 0; index < _pdbIds.size(); index++) {
	        String id = (String)_pdbIds.elementAt(index);
		_csrPdbIds.addElement(id);
	    }
	}
	
	if (doDebugOutput(2)) {
	    System.out.print("Coordinate PDB IDs: ");
	    for (int index = 0; index < _pdbIds.size(); index++) {
	        System.out.print((String)_pdbIds.elementAt(index) + ", ");
	    }
	    System.out.println("");

	    System.out.print("Chem shift ref PDB IDs: ");
	    for (int index = 0; index < _csrPdbIds.size(); index++) {
	        System.out.print((String)_csrPdbIds.elementAt(index) + ", ");
	    }
	    System.out.println("");
	}
    }

    //-------------------------------------------------------------------
    // Do the processing.
    // Note: this whole method could use a bunch of cleaing up in terms
    // of how we decide to do the different types of processing (regular
    // BMRB, PDB coordinates, chem shift reference).
    private void process() throws S2DException
    {
        if (doDebugOutput(3)) {
            System.out.println("process()");
        }

	if (_cmdFrameIndex != -1) {
	    // If we get here, it means that coord_index was specified on the
	    // command line.

	    if (_cmdFrameIndex < 0) {
		throw new S2DError("coord_index not specified!");
	    }

	    // Note: we create a "junk" summary file here because some
	    // of the existing code can't write the atomic coordinate
	    // stuff without writing to a summary file.  wenger 2003-09-12.
	    String tmpSummaryFile = _name + "tmp" + _cmdFrameIndex;
	    _summary = new S2DSummaryHtml(tmpSummaryFile, _longName,
	      _masterBmrbId, _localFiles, _htmlDir, "starFileName",
	      "systemName", "frameTitle");

	    //
	    // Do the coordinate processing.
	    //
	    if (_pdbLevel == PDB_LEVEL_PROCESS) {
		_currentFrameIndex = _cmdFrameIndex;
	        process1PDB(null, _cmdPdbId);
	    }

	    //
	    // Do the chem shift reference processing.
	    //
	    if (_csrLevel == CSR_LEVEL_PROCESS) {
	        S2DChemShiftRef csr = new S2DChemShiftRef(_name, _longName,
		  _dataDir, _csrDataDir, _sessionDir, _masterBmrbId,
		  _localFiles, _cmdPdbId, _summary, _cmdFrameIndex,
		  _csrTimeout);
	        csr.run();
	        csr.postProcess(_doProteinCheck);
		if (!csr.ran() || csr.ranButFailed()) {
		    throw new S2DError(
		      "Chem shift reference calculation failed");
		}
	    }

	    _summary.close(null, null);

	    // Note: we should delete this "junk" summary file here, but
	    // File.delete() doesn't seem to work!  wenger 2003-09-12.

	    runSetModes();

	    return;
	}

        _currentFrameIndex = 1;

	//
	// Create the summary HTML file.
	//
	S2DNmrStarIfc masterStar = null;
	if (!_masterBmrbId.equals("")) {
            masterStar = S2DNmrStarIfc.createFromID(_masterBmrbId,
	      _useLocalFiles, false);

	    _summary = new S2DSummaryHtml(_name, _longName, _masterBmrbId,
	      _localFiles, _htmlDir, masterStar.getFileName(),
	      masterStar.getSystemName(), masterStar.getEntryTitle());
	} else {
	    _summary = new S2DSummaryHtml(_name, _longName, "99999",
	      _localFiles, _htmlDir, _name, ""/*TEMP?*/, ""/*TEMP?*/);
	}

	//
	// Initiate chemical shift reference processing.
	//
	S2DChemShiftRef csr = null;
	if (_csrPdbIds.size() != 0 && _csrLevel == CSR_LEVEL_PROCESS) {
	    csr = new S2DChemShiftRef(_name, _longName, _dataDir, _csrDataDir,
	      _sessionDir, _masterBmrbId, _localFiles,
	      (String)_csrPdbIds.elementAt(0), _summary, 1, _csrTimeout);
	    csr.run();
	}

	//
	// Process all BMRB entries.
	//

	// Note: star seems to hold the *last* NMR-STAR file we process,
	// which we need for processing PDB entries later on.
        S2DStarIfc star = null;

	if (!_masterBmrbId.equals("")) {
	    star = processAllBMRBs(masterStar);
	}

	//
	// Process all local files.
	//
	if (_localFiles.size() > 0) {
	    star = processLocalFiles();
	}

	//
	// Initiate chemical shift reference processing, if we didn't
	// before and got a PDB ID from the NMR-STAR file.
	//
	if (csr == null && _csrPdbIds.size() != 0 &&
	  _csrLevel == CSR_LEVEL_PROCESS) {
	    csr = new S2DChemShiftRef(_name, _longName, _dataDir, _csrDataDir,
	      _sessionDir, _masterBmrbId, _localFiles,
	      (String)_csrPdbIds.elementAt(0), _summary, 1, _csrTimeout);
	    csr.run();
	}

	//TEMP -- maybe process additional CSRs here??

	//
	// Generate the summary data needed for selecting a data set in
	// the 3D visualizations.
	//
	S2DSummaryData summDat = new S2DSummaryData(_name, _dataDir,
	  _dataSets);
	summDat.writeSummaryData();

	if (_haveCoords) {
	    // If we have coordinates here we don't want to use the
	    // coordinates from PDB.
	    _pdbIds.removeAllElements();
	}

	//
	// Process all PDB entries.
	//
        processAllPDBs(star);

	//
	// Do the chemical shift reference post-processing if necessary.
	//
	if (csr != null) {
	    csr.postProcess(_doProteinCheck);
	    if (csr.ranButFailed()) {
		//
		// The chemical shift processing ran but failed.  Try it
		// on the next PDB ID.
		// Note: we really should move this off into a separate
		// thread to speed up processing (we'll spend a lot of time
		// just waiting for the chem shift reference processing
		// here), but I'm doing it the simple way first.
		// wenger 2004-02-05.
		//
		boolean retry = true;
		for (int csrIndex = 1; csrIndex < _csrPdbIds.size() && retry;
		  ++csrIndex) {
		    String csrPdb = (String)_csrPdbIds.elementAt(csrIndex);
	            csr = new S2DChemShiftRef(_name, _longName, _dataDir,
		      _csrDataDir, _sessionDir, _masterBmrbId, _localFiles,
	              csrPdb, _summary, csrIndex + 1, _csrTimeout);
	            csr.run();
	            csr.postProcess(_doProteinCheck);
		    retry = csr.ranButFailed();
		}
	    }
	}

	//
	// Here we're creating the link-only chem shift reference
	// entries on the summary page.
	//
	if (_csrLevel == CSR_LEVEL_LINK) {
	    for (int csrIndex = 0; csrIndex < _csrPdbIds.size(); csrIndex++) {
		String csrPdb = (String)_csrPdbIds.elementAt(csrIndex);
	        try {
		    _summary.startFrame(
		      "Chemical Shift Referencing Visualizations" +
		      " (calculated chem shifts from PDB " + csrPdb + ")");

	            _summary.writeChemShiftRefCGI(csrPdb, csrIndex + 1);
                } catch(IOException ex) {
                    System.err.println(
		      "IOException writing chem shift ref link: " +
		      ex.toString());
	            throw new S2DError("Can't write chem shift ref link");
	        } finally {
	            _summary.endFrame();
	        }
	    }
	}

	processLACS();

	_summary.close(_bmrbIds, _pdbIds);
	_summary = null;

	runSetModes();
    }

    //-------------------------------------------------------------------
    // Process all specified BMRB entries.
    private S2DNmrStarIfc processAllBMRBs(S2DNmrStarIfc masterStar)
      throws S2DException
    {
        S2DNmrStarIfc star = masterStar;

        for (int index = 0; index < _bmrbIds.size(); index++) {
	    String bmrbId = (String)_bmrbIds.elementAt(index);

	    // TEMP -- remove hard-coded links once we establish some
	    // kind of convention for the _Related_BMRB_accession_number
	    // info.

	    // Note: 4038 has chem shifts for 4096.
            if (bmrbId.equals("4038")) {
	        addBmrbId("4096");
	    } else if (bmrbId.equals("4096")) {
	        addBmrbId("4038");
	    }

	    if (index > 0) { // avoid parsing the "master" file twice
                star = S2DNmrStarIfc.createFromID(bmrbId, _useLocalFiles,
		  false);
	    }

            process1NmrStar(star);
        }

	return star;
    }

    //-------------------------------------------------------------------
    // Process all specified local files.
    // TEMP -- as of 2003-04-09, this really only works right on a single
    // file. wenger
    private S2DStarIfc processLocalFiles() throws S2DException
    {
	S2DNmrStarIfc star = null;

        for (int index = 0; index < _localFiles.size(); index++) {
	    star = S2DNmrStarIfc.createFromFile(
	      (String)_localFiles.elementAt(index), false);

	    process1NmrStar(star);
	}

	return star;
    }

    //-------------------------------------------------------------------
    // Process all specified PDB entries.
    private void processAllPDBs(S2DStarIfc star)
    {
        if (doDebugOutput(2)) {
	    System.out.println("processAllPDBs()");
	}

	for (int index = 0; index < _pdbIds.size(); index++) {
	    String id = (String)_pdbIds.elementAt(index);
	    if (_pdbLevel == PDB_LEVEL_LINK ||
	      _pdbLevel == PDB_LEVEL_PROCESS) {
		try {
	            process1PDB(star, id);
		} catch(S2DException ex) {
		    System.err.println("Unable to process PDB ID " + id);
		    _summary.writeMessage("Unable to get atomic coordinates" +
		      " from related PDB ID " + id, false);
		}
	    } else {
	        if (_retrying) {
		    _summary.writeMessage("Unable to get atomic " +
		      " coordinates from related PDB ID " + id + " because " +
		      "of insufficient memory", false);
	        } else {
		    _summary.writeMessage("Atomic coordinates from related " +
		      "PDB ID " + id + " not read", false);
	        }
	    }
	}
    }

    //-------------------------------------------------------------------
    // Process the LACS file (if any).
    private void processLACS() throws S2DException
    {
        if (doDebugOutput(2)) {
	    System.out.println("processLACS()");
	    System.out.println("_lacsFile = " + _lacsFile);
	}

	if (_lacsLevel >= LACS_LEVEL_TRY) {
	    try {
	        S2DNmrStarIfc lacsStar;
	        if (_lacsFile != null) {
	            lacsStar = S2DNmrStarIfc.createFromFile(_lacsFile, true);
	        } else {
	            lacsStar = S2DNmrStarIfc.createFromID(_masterBmrbId,
		      _useLocalFiles, true);
	        }

		saveLACS(lacsStar);
	    } catch(S2DException ex) {
	        System.err.println("Error processing LACS data: " + ex);
		S2DError err = new S2DError("LACS processing failed");
	        if (_lacsLevel >= LACS_LEVEL_MANDATORY) {
		    throw err;
		} else {
		    System.err.println("Warning: " + err);
		}
	    }
	}
    }

    //-------------------------------------------------------------------
    // Make sure that all of the files that we generated are world-readable.
    private void runSetModes()
    {
	if (_runScripts) {
	    try {
	        Runtime currentRT = Runtime.getRuntime();
		String directory = _isUvd ? "uvd/" : "";
	        Process ps = currentRT.exec("." + File.separator +
		  "set_modes " + directory + _name);
	        ps.waitFor();
	        int result = ps.exitValue();
	        if (result != 0) {
	            throw new S2DWarning("Non-zero exit value from " +
		      "set_modes; see tmp/set_modes.out");
	        }
	    } catch (Exception ex) {
	        System.err.println("Error setting file permissions: " +
	          ex.toString());
	    }
	}
    }

    //-------------------------------------------------------------------
    // Process a given NMR-STAR file entry.
    private void process1NmrStar(S2DNmrStarIfc star) throws S2DException
    {
        Vector ids = star.getPdbIds(_doProteinCheck);
        addPDB(ids);

        //TEMP -- do I really want to skip stuff if I get an error?

        saveChemShifts(star);
        saveT1Relaxation(star);
        saveT2Relaxation(star);
        saveHetNOE(star);
        saveHExchangeRate(star);
        saveCoupling(star);
        saveHExchangeProtFact(star);
        saveS2Params(star);

        save3DDataSources();
        saveAtomicCoords(star);

	try {
	    ensureResidueList(star);
	} catch (S2DException ex) {
	    System.err.println("Exception in ensureResidueList(): " + ex);
	}
    }

    //-------------------------------------------------------------------
    // Save all chem shift data for this entry.
    // Note: this can be tested with 4264.
    private void saveChemShifts(S2DStarIfc star) throws S2DException
    {
        if (doDebugOutput(3)) {
	    System.out.println("  S2DMain.saveChemShifts()");
	}

	Enumeration frameList = star.getDataFramesByCat(
	  star.CHEM_SHIFT_SF_CAT, star.ASSIGNED_CHEM_SHIFTS);

	int frameIndex = 1;
        while (frameList.hasMoreElements()) {
	    SaveFrameNode frame = (SaveFrameNode)frameList.nextElement();

	    Vector entityIDList = star.getUniqueChemShiftEntityIDs(frame);
	    for (int index = 0; index < entityIDList.size(); index++) {
	        String entityID = (String)entityIDList.get(index);

	        // We only want to output chemical shifts corresponding to a
	        // protein.  (This can be tested with bmr4038.str and bmr4056.str.)
	        if (_doProteinCheck && !star.refersToProtein(frame, entityID)) {
                    if (doDebugOutput(2)) {
                        System.out.println("Chemical shifts not saved for " +
                          "save frame " + frame.getLabel() + " (" + entityID +
			  ") because it is not a protein");
                    }
	        } else {
		    //TEMP -- do I really want to catch the error here (and in
		    // other similar methods below)?
		    try {
	                S2DResidues residues = saveFrameResCounts(star, frame,
		          entityID, frameIndex);
	                saveFrameChemShifts(star, frame, entityID, frameIndex);
	                saveFramePistachio(star, frame, entityID, residues,
		          frameIndex);
	                saveFrameAmbiguity(star, frame, entityID, residues,
		          frameIndex);
		    } catch(S2DException ex) {
		        System.err.println("Exception saving chem shifts for " +
		          "frame " + star.getFrameName(frame) + " (" + entityID +
			  "): " + ex.toString());
		    }
		    frameIndex++;
	        }
	    }
	}
    }

    //-------------------------------------------------------------------
    // Ensure that we've saved a residue list if at all possible.  In
    // most cases, a residue list will already have been saved when
    // this method is called, but if not we try to save one now.
    private void ensureResidueList(S2DStarIfc star) throws S2DException
    {
        if (doDebugOutput(3)) {
	    System.out.println("  S2DMain.ensureResidueList()");
	}

	if (!_savedResList) {
	    //TEMP -- this fails if there is more than one entity
            saveResList (star, null, 1);
	}
    }

    //-------------------------------------------------------------------
    // Save all T1 relaxation data for this entry.
    // Note: this can be tested with 4267.
    private void saveT1Relaxation(S2DStarIfc star)
    {
        if (doDebugOutput(3)) {
	    System.out.println("  S2DMain.saveT1Relaxation()");
	}

	Enumeration frameList = star.getDataFramesByCat(star.T1_RELAX_SF_CAT,
	  star.T1_RELAX);

	int frameIndex = 1;
        while (frameList.hasMoreElements()) {
	    SaveFrameNode frame = (SaveFrameNode)frameList.nextElement();
	    try {
	        saveFrameT1Relax(star, frame, frameIndex);
	    } catch(S2DException ex) {
		System.err.println("Exception saving T1 relaxation for " +
		  "frame " + star.getFrameName(frame) + ": " +
		  ex.toString());
	    }
	    frameIndex++;
        }
    }

    //-------------------------------------------------------------------
    // Save all T2 relaxation data for this entry.
    // Note: this can be tested with 4267.
    private void saveT2Relaxation(S2DStarIfc star)
    {
        if (doDebugOutput(3)) {
	    System.out.println("  S2DMain.saveT2Relaxation()");
	}

	Enumeration frameList = star.getDataFramesByCat(star.T2_RELAX_SF_CAT,
	  star.T2_RELAX);

	int frameIndex = 1;
        while (frameList.hasMoreElements()) {
	    SaveFrameNode frame = (SaveFrameNode)frameList.nextElement();
	    try {
	        saveFrameT2Relax(star, frame, frameIndex);
	    } catch(S2DException ex) {
		System.err.println("Exception saving T2 relaxation for " +
		  "frame " + star.getFrameName(frame) + ": " +
		  ex.toString());
	    }
	    frameIndex++;
        }
    }

    //-------------------------------------------------------------------
    // Save all heteronuclear NOE values for this entry.
    // Note: this can be tested with 4267.
    private void saveHetNOE(S2DStarIfc star)
    {
        if (doDebugOutput(3)) {
	    System.out.println("  S2DMain.saveHetNOE()");
	}

	Enumeration frameList = star.getDataFramesByCat(star.HET_NOE_SF_CAT,
	  star.HETERONUCLEAR_NOE);

	int frameIndex = 1;
        while (frameList.hasMoreElements()) {
	    SaveFrameNode frame = (SaveFrameNode)frameList.nextElement();
	    try {
	        saveFrameHetNOE(star, frame, frameIndex);
	    } catch(S2DException ex) {
		System.err.println("Exception saving heteronuclear NOE for " +
		  "frame " + star.getFrameName(frame) + ": " +
		  ex.toString());
	    }
	    frameIndex++;
        }
    }

    //-------------------------------------------------------------------
    private void saveHExchangeRate(S2DStarIfc star)
    {
        if (doDebugOutput(3)) {
	    System.out.println("  S2DMain.saveHExchangeRate()");
	}

	// add real code here
    }

    //-------------------------------------------------------------------
    // Save all coupling constants for this entry.
    // Note: this can be tested with 4297.
    private void saveCoupling(S2DStarIfc star)
    {
        if (doDebugOutput(3)) {
	    System.out.println("  S2DMain.saveCoupling()");
	}

	Enumeration frameList = star.getDataFramesByCat(star.COUPLING_SF_CAT,
	  star.COUPLING_CONSTANTS);

	int frameIndex = 1;
        while (frameList.hasMoreElements()) {
	    SaveFrameNode frame = (SaveFrameNode)frameList.nextElement();
	    try {
	        saveFrameCoupling(star, frame, frameIndex);
	    } catch(S2DException ex) {
		System.err.println("Exception saving coupling constants for " +
		  "frame " + star.getFrameName(frame) + ": " +
		  ex.toString());
	    }
	    frameIndex++;
        }
    }

    //-------------------------------------------------------------------
    private void saveHExchangeProtFact(S2DStarIfc star)
    {
        if (doDebugOutput(3)) {
	    System.out.println("  S2DMain.saveHExchangeProtFact()");
	}

	// add real code here
    }

    //-------------------------------------------------------------------
    private void saveS2Params(S2DStarIfc star)
    {
        if (doDebugOutput(3)) {
	    System.out.println("  S2DMain.saveS2Params()");
	}

	// add real code here
    }

    //-------------------------------------------------------------------
    // Write the data source definitions needed for 3D data.
    private void save3DDataSources() throws S2DException
    {
        if (doDebugOutput(3)) {
	    System.out.println("  S2DMain.save3DDataSources()");
	}

	try {
	    Writer writer = S2DFileWriter.create(_sessionDir +
	      File.separator + _name + S2DNames.ATOMIC_COORD_DATA_DEF);
	    S2DSession.write3DDataSources(writer, _dataSets);
	    writer.close();
	} catch (IOException ex) {
	    throw new S2DError("Unable to save 3D data sources: " +
	      ex.toString());
	}
    }

    //-------------------------------------------------------------------
    private void saveAtomicCoords(S2DStarIfc star)
    {
        if (doDebugOutput(3)) {
	    System.out.println("  S2DMain.saveAtomicCoords()");
	}

	Enumeration frameList = star.getDataFramesByCat(
	  star.ATOM_COORD_SF_CAT, star.ATOM_COORD_SAVE_FRAME);

        while (frameList.hasMoreElements()) {
	    SaveFrameNode frame = (SaveFrameNode)frameList.nextElement();
	    try {
	        saveFrameAtomicCoords(star, frame, _currentFrameIndex, null,
		  false);

		_haveCoords = true;
	    } catch(S2DException ex) {
		System.err.println("Exception saving atomic coordinates for " +
		  "frame " + star.getFrameName(frame) + ": " +
		  ex.toString());
	    }
	    _currentFrameIndex++;
        }
    }

    //-------------------------------------------------------------------
    private void saveLACS(S2DStarIfc star) throws S2DException
    {
        if (doDebugOutput(3)) {
	    System.out.println("  S2DMain.saveLACS()");
	}

        Enumeration frameList = star.getDataFramesByCat(
	  star.LACS_PLOT_SF_CAT, star.LACS_OUTPUT);

	int frameIndex = 1;
        while (frameList.hasMoreElements()) {
	    SaveFrameNode frame = (SaveFrameNode)frameList.nextElement();

	    saveFrameLACS(star, frame, frameIndex);
	    frameIndex++;
	}
    }

    //-------------------------------------------------------------------
    // Save residue counts (and the residue list) corresponding to
    // the given save frame.  (The save frame passed in here should
    // be a save frame containing data such as chemical shifts.)
    private S2DResidues saveFrameResCounts(S2DStarIfc star,
      SaveFrameNode chemShiftFrame, String entityID, int frameIndex)
      throws S2DException
    {
        if (doDebugOutput(4)) {
	    System.out.println("    S2DMain.saveFrameResCounts(" +
	      star.getFrameName(chemShiftFrame) + " (" + entityID +
	      "), " + frameIndex + ")");
	}

	SaveFrameNode tmpFrame = null;
	try {
	    // We try this first even if protein checking is turned off
	    // in case we have a complete NMR-STAR file with multiple
	    // monmeric_polymer save frames -- that way we get the right
	    // one.
	    tmpFrame = star.getEntityFrame(chemShiftFrame, entityID);
	} catch (S2DException ex) {
	    if (_doProteinCheck) {
	        throw ex;
	    }
	}

        S2DResidues residues = saveResList(star, tmpFrame, frameIndex);

	if (_checkResList) {
	    //
	    // Compare the residue list against the residues in the chem
	    // shift loop.
	    // Note: residue numbers start with 1, but we can't always
	    // rely on every residue having an entry in the chem shift loop.
	    //
	    String[] entityIDs = null;
	    if (!entityID.equals("")) {
	        entityIDs = star.getAllChemShiftEntityIDs(chemShiftFrame);
	    }

	    String[] resSeqCodesTmp = star.getAndFilterFrameValues(
	      chemShiftFrame, star.CHEM_SHIFT_VALUE,
	      star.CHEM_SHIFT_RES_SEQ_CODE, entityID, entityIDs);
	    int[] resSeqCodes = S2DUtils.arrayStr2Int(resSeqCodesTmp,
	      star.CHEM_SHIFT_RES_SEQ_CODE);
	    resSeqCodesTmp = null;

	    String[] residueLabels = star.getAndFilterFrameValues(
	      chemShiftFrame, star.CHEM_SHIFT_VALUE,
	      star.CHEM_SHIFT_RES_LABEL, entityID, entityIDs);
	    S2DResidues.make3Letter(residueLabels);

            for (int index = 0; index < resSeqCodes.length; ++index) {
	        int resNum = resSeqCodes[index];
	        if (!residueLabels[index].equals(
		  residues.getResLabel(resNum))) {
		    S2DError err = new S2DError("Residue mismatch at residue " +
		      resNum + ": " + residues.getResLabel(resNum) +
		      " in residue list vs. " + residueLabels[index] +
		      " in chem shift loop");
		    System.err.println(err);
		    System.exit(1);
	        }
	    }

	}

        return residues;
    }

    //-------------------------------------------------------------------
    // Save the residue count and residue list for the given save frame.
    // (The save frame passed in here must be the save frame that
    // actually contains the residue lists.)
    private S2DResidues saveResList(S2DStarIfc star,
      SaveFrameNode monoPolyFrame, int frameIndex) throws S2DException
    {
        if (doDebugOutput(4)) {
	    System.out.println("    S2DMain.saveResList(" +
	      star.getFrameName(monoPolyFrame) + ", " + frameIndex + ")");
	}

        if (monoPolyFrame == null) {
	    monoPolyFrame = star.getEntityFrame();
        }

        S2DResidues residues = star.getResidues(monoPolyFrame);

        S2DResCount resCount = new S2DResCount(_name, _dataDir,
	  residues._resSeqCodes, residues._resLabels);

	resCount.write(frameIndex);

        S2DResList resList = new S2DResList(_name, _dataDir,
	  residues._resSeqCodes, residues._resLabels);

	resList.write(frameIndex);

	_savedResList = true;

	return residues;
    }

    //-------------------------------------------------------------------
    // Save chem shifts for one save frame.
    private void saveFrameChemShifts(S2DStarIfc star, SaveFrameNode frame,
      String entityID, int frameIndex) throws S2DException
    {
        if (doDebugOutput(4)) {
	    System.out.println("    S2DMain.saveFrameChemShifts(" +
	      star.getFrameName(frame) + " (" + entityID + "), " +
	      frameIndex + ")");
	}

	//
	// Get the values we need from the Star file.
	//
	String[] entityIDs = null;
	if (!entityID.equals("")) {
	    entityIDs = star.getAllChemShiftEntityIDs(frame);
	}

	String[] resSeqCodesTmp = star.getAndFilterFrameValues(frame,
	  star.CHEM_SHIFT_VALUE, star.CHEM_SHIFT_RES_SEQ_CODE, entityID,
	  entityIDs);
	int[] resSeqCodes = S2DUtils.arrayStr2Int(resSeqCodesTmp,
	  star.CHEM_SHIFT_RES_SEQ_CODE);
	resSeqCodesTmp = null;

	String[] residueLabels = star.getAndFilterFrameValues(frame,
	  star.CHEM_SHIFT_VALUE, star.CHEM_SHIFT_RES_LABEL, entityID,
	  entityIDs);
	S2DResidues.make3Letter(residueLabels);

	String[] atomNames = star.getAndFilterFrameValues(frame,
	  star.CHEM_SHIFT_VALUE, star.CHEM_SHIFT_ATOM_NAME, entityID,
	  entityIDs);

	String[] atomTypes = star.getAndFilterFrameValues(frame,
	  star.CHEM_SHIFT_VALUE, star.CHEM_SHIFT_ATOM_TYPE, entityID,
	  entityIDs);

	String[] chemShiftsTmp = star.getAndFilterFrameValues(frame,
	  star.CHEM_SHIFT_VALUE, star.CHEM_SHIFT_VALUE, entityID,
	  entityIDs);
        double[] chemShiftVals = S2DUtils.arrayStr2Double(chemShiftsTmp,
	  star.CHEM_SHIFT_VALUE);
	chemShiftsTmp = null;

	int[] ambiguityVals;
	try {
	    String[] ambiguityTmp = star.getAndFilterFrameValues(frame,
	      star.CHEM_SHIFT_VALUE, star.CHEM_SHIFT_AMBIG_CODE, entityID,
	      entityIDs);
	    ambiguityVals = S2DUtils.arrayStr2Int(ambiguityTmp,
	      star.CHEM_SHIFT_AMBIG_CODE);
	    ambiguityTmp = null;
	} catch (S2DException ex) {
            if (doDebugOutput(4)) {
	        System.out.println("No ambiguity values in this save frame (" +
		  star.getFrameName(frame) + ")");
	    }
	    ambiguityVals = new int[entityIDs.length];
	    for (int index = 0; index < ambiguityVals.length; ++index) {
	    	ambiguityVals[index] = 9;
	    }
	}

	//
	// Create an S2DChemShift object with the values we just got.
	//
        S2DChemShift chemShift = new S2DChemShift(_name, _longName, _dataDir,
	  _sessionDir, _summary, resSeqCodes, residueLabels, atomNames,
	  atomTypes, chemShiftVals, ambiguityVals);


	//
	// Now go ahead and calculate and write out the chem shift values.
	//
	_summary.startFrame(star.getFrameDetails(frame));

	try {
	    chemShift.writeDeltashifts(frameIndex);
	    chemShift.addDeltaData(_dataSets, frameIndex);
	} catch (S2DException ex) {
	    // Don't throw a new exception here because we want to write as
	    // much as we can, even if there's an error somewhere along the
	    // line.
	    System.err.println(ex.toString());
	}

	// We only want to output CSI values if this save frame
	// has HA chem shifts for at least 80% of the residues in
	// the entry.  (This can be tested with bmr4001.str and
	// bmr4011.str.)
	int residueCount = -1;
	if (_doProteinCheck) {
	    residueCount = star.residueCount(frame, entityID);
	}

	if (residueCount < 0 ||
	  ((float)star.getHAChemShiftCount(frame) /
	  (float)residueCount >= 0.8)) {
	    try {
	        chemShift.writeCSI(frameIndex);
	        chemShift.addCsiData(_dataSets, frameIndex);
	    } catch (S2DException ex) {
	        // Don't throw a new exception here because we want to write as
	        // much as we can, even if there's an error somewhere along the
	        // line.
	        System.err.println(ex.toString());
	    }
	} else {
	    if (doDebugOutput(2)) {
	        System.out.println("CSI values not saved for " +
		  "save frame " + frame.getLabel() + " because HA " +
		    "chem shifts are not available for 80% of the residues");
            }
	}

	try {
	    chemShift.writePctAssign(frameIndex);
	    chemShift.addPctAssignData(_dataSets, frameIndex);
	} catch (S2DException ex) {
	    // Don't throw a new exception here because we want to write as
	    // much as we can, even if there's an error somewhere along the
	    // line.
	    System.err.println(ex.toString());
	}

	try {
	    chemShift.writeAllShifts(frameIndex);
	} catch (S2DException ex) {
	    // Don't throw a new exception here because we want to write as
	    // much as we can, even if there's an error somewhere along the
	    // line.
	    System.err.println(ex.toString());
	}

	try {
	    chemShift.writeHvsNShifts(frameIndex);
	} catch (S2DException ex) {
	    // Don't throw a new exception here because we want to write as
	    // much as we can, even if there's an error somewhere along the
	    // line.
	    System.err.println(ex.toString());
	}

	_summary.endFrame();
    }

    //-------------------------------------------------------------------
    // Save T1 relaxation values for one save frame.
    private void saveFrameT1Relax(S2DStarIfc star, SaveFrameNode frame,
      int frameIndex) throws S2DException
    {
        if (doDebugOutput(4)) {
	    System.out.println("    S2DMain.saveFrameT1Relax(" +
	      star.getFrameName(frame) + ", " + frameIndex + ")");
	}

	//
	// Get the values we need from the Star file.
	//
	String[] resSeqCodes = star.getFrameValues(frame,
	  star.T1_VALUE, star.T1_RES_SEQ_CODE);

	String[] resLabels = star.getFrameValues(frame,
	  star.T1_VALUE, star.T1_RES_LABEL);

	String[] atomNames = star.getFrameValues(frame,
	  star.T1_VALUE, star.T1_ATOM_NAME);

	String[] relaxValues = star.getFrameValues(frame,
	  star.T1_VALUE, star.T1_VALUE);

	//TEMP -- 4096 has "_T1_error" instead of "_T1_value_error".
	String[] relaxErrors = star.getOptionalFrameValues(frame,
	  star.T1_VALUE, star.T1_VALUE_ERR, relaxValues.length, "0");

	//
	// Create an S2DRelaxation object with the values we just got.
	//
	S2DRelaxation relaxation = new S2DRelaxation(_name, _longName,
	  _dataDir, _sessionDir, _summary, S2DUtils.TYPE_T1_RELAX,
	  star.getOneFrameValue(frame, star.T1_SPEC_FREQ_1H),
	  resSeqCodes, resLabels, atomNames, relaxValues, relaxErrors);

	//
	// Now go ahead and calculate and write out the relaxation values.
	//
	_summary.startFrame(star.getFrameDetails(frame));

	try {
	    relaxation.writeRelaxation(frameIndex);
	    relaxation.addRelaxationData(_dataSets, frameIndex);
	} finally {
	    _summary.endFrame();
	}
    }

    //-------------------------------------------------------------------
    // Save T2 relaxation values for one save frame.
    private void saveFrameT2Relax(S2DStarIfc star, SaveFrameNode frame,
      int frameIndex) throws S2DException
    {
        if (doDebugOutput(4)) {
	    System.out.println("    S2DMain.saveFrameT2Relax(" +
	      star.getFrameName(frame) + ", " + frameIndex + ")");
	}

	//
	// Get the values we need from the Star file.
	//
	String[] resSeqCodes = star.getFrameValues(frame,
	  star.T2_VALUE, star.T2_RES_SEQ_CODE);

	String[] resLabels = star.getFrameValues(frame,
	  star.T2_VALUE, star.T2_RES_LABEL);

	String[] atomNames = star.getFrameValues(frame,
	  star.T2_VALUE, star.T2_ATOM_NAME);

	String[] relaxValues = star.getFrameValues(frame,
	  star.T2_VALUE, star.T2_VALUE);

	//TEMP -- 4096 has "_T2_error" instead of "_T2_value_error".
	String[] relaxErrors = star.getOptionalFrameValues(frame,
	  star.T2_VALUE, star.T2_VALUE_ERR, relaxValues.length, "0");

	//
	// Create an S2DRelaxation object with the values we just got.
	//
	S2DRelaxation relaxation = new S2DRelaxation(_name, _longName,
	  _dataDir, _sessionDir, _summary, S2DUtils.TYPE_T2_RELAX,
	  star.getOneFrameValue(frame, star.T2_SPEC_FREQ_1H),
	  resSeqCodes, resLabels, atomNames, relaxValues, relaxErrors);

	//
	// Now go ahead and calculate and write out the relaxation values.
	//
	_summary.startFrame(star.getFrameDetails(frame));

	try {
	    relaxation.writeRelaxation(frameIndex);
	    relaxation.addRelaxationData(_dataSets, frameIndex);
	} finally {
	    _summary.endFrame();
	}
    }

    //-------------------------------------------------------------------
    // Save coupling constants for one save frame.
    private void saveFrameCoupling(S2DStarIfc star, SaveFrameNode frame,
      int frameIndex) throws S2DException
    {
        if (doDebugOutput(4)) {
	    System.out.println("    S2DMain.saveFrameCoupling(" +
	      star.getFrameName(frame) + ", " + frameIndex + ")");
	}

	//
	// Get the values we need from the Star file.
	//
	String[] atom1ResSeqs = star.getFrameValues(frame,
	  star.COUPLING_CONSTANT_VALUE, star.COUPLING_RES_SEQ_CODE_1);

	String[] atom2ResSeqs = star.getFrameValues(frame,
	  star.COUPLING_CONSTANT_VALUE, star.COUPLING_RES_SEQ_CODE_2);

	String[] couplingConstValues = star.getFrameValues(frame,
	  star.COUPLING_CONSTANT_VALUE, star.COUPLING_CONSTANT_VALUE);

	String[] couplingConstCodes = star.getOptionalFrameValues(frame,
	  star.COUPLING_CONSTANT_VALUE, star.COUPLING_CONSTANT_CODE,
	  atom1ResSeqs.length, "0");

	String[] atom1ResLabels = star.getOptionalFrameValues(frame,
	  star.COUPLING_CONSTANT_VALUE, star.COUPLING_RES_LABEL_1,
	  atom1ResSeqs.length, "0");

	String[] atom1Names = star.getOptionalFrameValues(frame,
	  star.COUPLING_CONSTANT_VALUE, star.COUPLING_ATOM_NAME_1,
	  atom1ResSeqs.length, "0");

	String[] atom2ResLabels = star.getOptionalFrameValues(frame,
	  star.COUPLING_CONSTANT_VALUE, star.COUPLING_RES_LABEL_2,
	  atom1ResSeqs.length, "0");

	String[] atom2Names = star.getOptionalFrameValues(frame,
	  star.COUPLING_CONSTANT_VALUE, star.COUPLING_ATOM_NAME_2,
	  atom1ResSeqs.length, "0");

	String[] couplingConstErrors = star.getOptionalFrameValues(frame,
	  star.COUPLING_CONSTANT_VALUE,
	  star.COUPLING_CONSTANT_VALUE_ERR, atom1ResSeqs.length, "0");

	//
	// Create an S2DCoupling object with the values we just got.
	//
        S2DCoupling coupling = new S2DCoupling(_name, _longName, _dataDir,
	  _sessionDir, _summary, couplingConstCodes, atom1ResSeqs,
	  atom1ResLabels, atom1Names, atom2ResSeqs, atom2ResLabels,
	  atom2Names, couplingConstValues, couplingConstErrors);

	//
	// Now go ahead and calculate and write out the coupling contants.
	//
	_summary.startFrame(star.getFrameDetails(frame));

	try {
	    coupling.writeCoupling(frameIndex);
	    coupling.addCouplingData(_dataSets, frameIndex);
	} finally {
	    _summary.endFrame();
	}
    }

    //-------------------------------------------------------------------
    // Save heteronuclear NOE values for one save frame.
    private void saveFrameHetNOE(S2DStarIfc star, SaveFrameNode frame,
      int frameIndex) throws S2DException
    {
        if (doDebugOutput(4)) {
	    System.out.println("    S2DMain.saveFrameHetNOE(" +
	      star.getFrameName(frame) + ", " + frameIndex + ")");
	}

	//
	// Get the values we need from the Star file.
	// TEMP -- NMR-STAR 3.0 has two residues for each heternuclear
	// NOE, so we should probably deal with that eventually.
	//
//TEMPTEMP -- convert these to numerical values?
	String[] resSeqCodes = star.getFrameValues(frame,
	  star.HET_NOE_VALUE, star.HET_NOE_RES_SEQ_CODE);

	String[] resLabels = star.getFrameValues(frame,
	  star.HET_NOE_VALUE, star.HET_NOE_RES_LABEL);

	String[] hetNOEValues = star.getFrameValues(frame,
	  star.HET_NOE_VALUE, star.HET_NOE_VALUE);

	String[] hetNOEErrors = star.getFrameValues(frame,
	  star.HET_NOE_VALUE, star.HET_NOE_VALUE_ERR);

	//
	// Create an S2DHetNOE object with the values we just got.
	//
	// are dealt with really differently in 2.1 and 3.0
	S2DHetNOE hetNOE = new S2DHetNOE(_name, _longName, _dataDir,
	  _sessionDir, _summary,
	  star.getOneFrameValue(frame, star.HET_NOE_SPEC_FREQ_1H),
	  star.getHetNOEAtom1(frame), star.getHetNOEAtom2(frame),
	  resSeqCodes, resLabels, hetNOEValues, hetNOEErrors);

	//
	// Now go ahead and calculate and write out the heteronuclear NOE
	// values.
	//
	_summary.startFrame(star.getFrameDetails(frame));

	try {
	    hetNOE.writeHetNOE(frameIndex);
	    hetNOE.addHetNOEData(_dataSets, frameIndex);
	} finally {
	    _summary.endFrame();
	}
    }

    //-------------------------------------------------------------------
    // Save atomic coordinates for one save frame.
    private void saveFrameAtomicCoords(S2DStarIfc star, SaveFrameNode frame,
      int frameIndex, S2DAtomDataTable pt, boolean for2DView)
      throws S2DException
    {
        if (doDebugOutput(4)) {
	    System.out.println("    S2DMain.saveFrameAtomicCoords(" +
	      star.getFrameName(frame) + ", " + frameIndex + ")");
	}

	//
	// If this is a PDB (mmCIF) file, figure out which coordinates
	// correspond to the first model, and only get that data.
	//
	int model1AtomCount = -1;
	if (!star.ATOM_COORD_MODEL_NUM.equals("")) {
	    String[] modelNums = star.getFrameValues(frame,
	      star.ATOM_COORD_X, star.ATOM_COORD_MODEL_NUM);
	    for (int index = 0; index < modelNums.length; index++) {
	        if (!modelNums[index].equals("1")) {
		    model1AtomCount = index;
		    break;
		}
	    }

	    if (doDebugOutput(2)) {
	        System.out.println("Model 1 atom count = " + model1AtomCount);
	    }
	}

	//
	// Get the values we need from the Star file.
	//
	String[] resSeqCodes = star.getFrameValues(frame,
	  star.ATOM_COORD_X, star.ATOM_COORD_RES_SEQ_CODE, model1AtomCount);

	String[] resLabels = star.getFrameValues(frame,
	  star.ATOM_COORD_X, star.ATOM_COORD_RES_LABEL, model1AtomCount);

	String[] atomNames = star.getFrameValues(frame,
	  star.ATOM_COORD_X, star.ATOM_COORD_ATOM_NAME, model1AtomCount);

        atomNames = star.translateAtomNomenclature(resLabels, atomNames);

	String[] atomTypes = star.getFrameValues(frame,
	  star.ATOM_COORD_X, star.ATOM_COORD_ATOM_TYPE, model1AtomCount);

	String[] atomCoordXTmp = star.getFrameValues(frame,
	  star.ATOM_COORD_X, star.ATOM_COORD_X, model1AtomCount);
        double[] atomCoordX = S2DUtils.arrayStr2Double(atomCoordXTmp,
	  star.ATOM_COORD_X);
        atomCoordXTmp = null;

	String[] atomCoordYTmp = star.getFrameValues(frame,
	  star.ATOM_COORD_Y, star.ATOM_COORD_Y, model1AtomCount);
        double[] atomCoordY = S2DUtils.arrayStr2Double(atomCoordYTmp,
	  star.ATOM_COORD_Y);
        atomCoordYTmp = null;

	String[] atomCoordZTmp = star.getFrameValues(frame,
	  star.ATOM_COORD_Z, star.ATOM_COORD_Z, model1AtomCount);
        double[] atomCoordZ = S2DUtils.arrayStr2Double(atomCoordZTmp,
	  star.ATOM_COORD_Z);
        atomCoordZTmp = null;

	//
	// Create an S2DAtomicCoords object with the values we just got.
	//
	S2DAtomicCoords atomicCoords = new S2DAtomicCoords(_name,
	  _longName, _dataDir, _sessionDir, _summary, resSeqCodes,
	  resLabels, atomNames, atomTypes, atomCoordX, atomCoordY,
	  atomCoordZ, _connectionFile, _dataSets, _currentPdbId);

	//
	// Now go ahead and calculate and write out the atomic coordinates.
	//
	if (pt == null) {
	    AtomicCoordSummaryPre(star, frame);
	}

	try {
	    if (for2DView) {
	        atomicCoords.writeBonds(frameIndex, pt, for2DView);
	    } else {
	        atomicCoords.writeAtoms(frameIndex);
	    }
	} finally {
	    if (pt == null) {
	        _summary.endFrame();
	    }
	}
    }

    //-------------------------------------------------------------------
    // Save Pistachio data for one save frame.
    //TEMP -- needs entity argument
    private void saveFramePistachio(S2DStarIfc star, SaveFrameNode frame,
      String entityID, S2DResidues residues, int frameIndex) throws S2DException
    {
        if (doDebugOutput(4)) {
	    System.out.println("    S2DMain.saveFramePistachio(" +
	      star.getFrameName(frame) + " (" + entityID + "), " +
	      frameIndex + ")");
	}

	//
	// Get the values we need from the Star file.
	//
	if (star.FIGURE_OF_MERIT.equals("")) {
	    // File is not NMR-STAR 3.0
	    return;
	}

//TEMPTEMP -- change CHEM_SHIFT_VALUE to FIGURE_OF_MERIT in all here
	String[] entityIDs = null;
	if (!entityID.equals("")) {
	    entityIDs = star.getAllChemShiftEntityIDs(frame);
	}

	String[] meritValsTmp;
	try {
	    meritValsTmp = star.getAndFilterFrameValues(frame,
	      star.CHEM_SHIFT_VALUE, star.FIGURE_OF_MERIT, entityID,
	      entityIDs);
	} catch (S2DException ex) {
            if (doDebugOutput(4)) {
	        System.out.println("No Pistachio values in this save frame (" +
		  star.getFrameName(frame) + ")");
	    }
	    return;
	}
	double[] meritVals = S2DUtils.arrayStr2Double(meritValsTmp,
	  star.FIGURE_OF_MERIT);
	meritValsTmp = null;

	String[] resSeqCodesTmp = star.getAndFilterFrameValues(frame,
	  star.CHEM_SHIFT_VALUE, star.CHEM_SHIFT_RES_SEQ_CODE, entityID,
	  entityIDs);
	int[] resSeqCodes = S2DUtils.arrayStr2Int(resSeqCodesTmp,
	  star.CHEM_SHIFT_RES_SEQ_CODE);
	resSeqCodesTmp = null;

	String[] residueLabels = star.getAndFilterFrameValues(frame,
	  star.CHEM_SHIFT_VALUE, star.CHEM_SHIFT_RES_LABEL, entityID,
	  entityIDs);
	S2DResidues.make3Letter(residueLabels);

	String[] atomNames = star.getAndFilterFrameValues(frame,
	  star.CHEM_SHIFT_VALUE, star.CHEM_SHIFT_ATOM_NAME, entityID,
	  entityIDs);

	//
	// Create an S2DPistachio object with the values we just got.
	//
        S2DPistachio pistachio = new S2DPistachio(_name, _dataDir,
	  _sessionDir, _summary, resSeqCodes, residueLabels, atomNames,
	  meritVals);

	//
	// The S2DPistachioTable object holds the figures of merit for
	// each atom, for the "color by figure of merit" option.  This
	// is passed along when we write out the coordinates so that the
	// figure of merit values and corresponding color strings can also
	// be written along with the coordinates.
	//
	S2DPistachioTable pt = new S2DPistachioTable(resSeqCodes,
	  atomNames, meritVals);

	//
	// Create an S2DDummyCoords object and write out the
	// coordinates.
	//
        S2DDummyCoords dCoords = S2DDummyCoords.getInstance();
	String tmpFile = _dataDir + File.separator + _name + "pstmp";
	dCoords.writeCoords(residues, tmpFile, PISTACHIO_WRAP_LENGTH);

	S2DmmCifIfc cif = new S2DmmCifIfc(tmpFile);
	saveFrameAtomicCoords(cif, null, frameIndex, pt, true);

	try {
	    S2DUtils.deleteFile(tmpFile);
	} catch (IOException ex) {
	    System.err.println("Warning: unable to delete temporary " +
	      "dummy mmCIF file: " + ex);
	}

	//
	// Now go ahead and write out the Pistachio values.
	//
	_summary.startFrame("Assignment figure of merit data");

	try {
	    pistachio.writePistachio(frameIndex);
	} finally {
	    _summary.endFrame();
	}
    }

    //-------------------------------------------------------------------
    // Save ambiguity data for one save frame.
    private void saveFrameAmbiguity(S2DStarIfc star, SaveFrameNode frame,
      String entityID, S2DResidues residues, int frameIndex)
      throws S2DException
    {
        if (doDebugOutput(4)) {
	    System.out.println("    S2DMain.saveFrameAmbiguity(" +
	      star.getFrameName(frame) + " (" + entityID + "), " +
	      frameIndex + ")");
	}

//TEMPTEMP -- change CHEM_SHIFT_VALUE here?
	//
	// Get the values we need from the Star file.
	//
	String[] entityIDs = null;
	if (!entityID.equals("")) {
	    entityIDs = star.getAllChemShiftEntityIDs(frame);
	}

	String[] ambiguityTmp;
	try {
	    ambiguityTmp = star.getAndFilterFrameValues(frame,
	      star.CHEM_SHIFT_VALUE, star.CHEM_SHIFT_AMBIG_CODE, entityID,
	      entityIDs);
	} catch (S2DException ex) {
            if (doDebugOutput(4)) {
	        System.out.println("No ambiguity values in this save frame (" +
		  star.getFrameName(frame) + ")");
	    }
	    return;
	}
	int[] ambiguityVals = S2DUtils.arrayStr2Int(ambiguityTmp,
	  star.CHEM_SHIFT_AMBIG_CODE);
	ambiguityTmp = null;

	String[] resSeqCodesTmp = star.getAndFilterFrameValues(frame,
	  star.CHEM_SHIFT_VALUE, star.CHEM_SHIFT_RES_SEQ_CODE, entityID,
	  entityIDs);
	int[] resSeqCodes = S2DUtils.arrayStr2Int(resSeqCodesTmp,
	  star.CHEM_SHIFT_RES_SEQ_CODE);
	resSeqCodesTmp = null;

	String[] residueLabels = star.getAndFilterFrameValues(frame,
	  star.CHEM_SHIFT_VALUE, star.CHEM_SHIFT_RES_LABEL, entityID,
	  entityIDs);
	S2DResidues.make3Letter(residueLabels);

	String[] atomNames = star.getAndFilterFrameValues(frame,
	  star.CHEM_SHIFT_VALUE, star.CHEM_SHIFT_ATOM_NAME, entityID,
	  entityIDs);

	//
	// Create an S2DAmbiguity object with the values we just got.
	//
        S2DAmbiguity ambiguity = new S2DAmbiguity(_name, _dataDir,
	  _sessionDir, _summary, resSeqCodes, residueLabels, ambiguityVals);

	//
	// The S2DAmbiguityTable object holds the ambiguity values for
	// each atom, for the "color by ambiguity value" option.  This
	// is passed along when we write out the coordinates so that the
	// ambiguity values and corresponding color strings can also be
	// written along with the coordinates.
	//
	S2DAmbiguityTable at = new S2DAmbiguityTable(resSeqCodes, atomNames,
	  ambiguityVals);

	//
	// Create an S2DDummyCoords object and write out the
	// coordinates.
	//
        S2DDummyCoords dCoords = S2DDummyCoords.getInstance();
	String tmpFile = _dataDir + File.separator + _name + "pstmp";
	dCoords.writeCoords(residues, tmpFile, AMBIGUITY_WRAP_LENGTH);

	S2DmmCifIfc cif = new S2DmmCifIfc(tmpFile);
	saveFrameAtomicCoords(cif, null, frameIndex, at, true);

	try {
	    S2DUtils.deleteFile(tmpFile);
	} catch (IOException ex) {
	    System.err.println("Warning: unable to delete temporary " +
	      "dummy mmCIF file: " + ex);
	}

	//
	// Now go ahead and write out the ambiguity values.
	//
	_summary.startFrame(star.getFrameDetails(frame));

	try {
	    ambiguity.writeAmbiguity(frameIndex);
	} finally {
	    _summary.endFrame();
	}
    }

    //-------------------------------------------------------------------
    // Save LACS data for one save frame.
    private void saveFrameLACS(S2DStarIfc star, SaveFrameNode frame,
      int frameIndex) throws S2DException
    {
        if (doDebugOutput(4)) {
	    System.out.println("    S2DMain.saveFrameLACS(" +
	      star.getFrameName(frame) + ", " + frameIndex + ")");
	}

	S2DLacs lacs = new S2DLacs(_name, _longName, _dataDir,
	  _sessionDir, _summary);

        //
        // Get the values we need from the Star file.
        //

	lacs._xCoordName = star.getTagValue(frame, star.LACS_X_NAME);
	lacs._yCoordName = star.getTagValue(frame, star.LACS_Y_NAME);

	lacs._line1._point1._x = Double.parseDouble(star.getTagValue(frame,
	  star.LACS_LINE1_X1));
	lacs._line1._point1._y = Double.parseDouble(star.getTagValue(frame,
	  star.LACS_LINE1_Y1));
	lacs._line1._point2._x = Double.parseDouble(star.getTagValue(frame,
	  star.LACS_LINE1_X2));
	lacs._line1._point2._y = Double.parseDouble(star.getTagValue(frame,
	  star.LACS_LINE1_Y2));

	lacs._line2._point1._x = Double.parseDouble(star.getTagValue(frame,
	  star.LACS_LINE2_X1));
	lacs._line2._point1._y = Double.parseDouble(star.getTagValue(frame,
	  star.LACS_LINE2_Y1));
	lacs._line2._point2._x = Double.parseDouble(star.getTagValue(frame,
	  star.LACS_LINE2_X2));
	lacs._line2._point2._y = Double.parseDouble(star.getTagValue(frame,
	  star.LACS_LINE2_Y2));

	lacs._yOffset = Double.parseDouble(star.getTagValue(frame,
	  star.LACS_Y_OFFSET));

	lacs.calculateLines();

        String[] resSeqCodesTmp = star.getFrameValues(frame,
          star.LACS_RES_NUM, star.LACS_RES_NUM);
	lacs._resSeqCodes = S2DUtils.arrayStr2Int(resSeqCodesTmp,
	  star.LACS_RES_NUM);
	resSeqCodesTmp = null;

        lacs._resLabels = star.getFrameValues(frame,
          star.LACS_RES_NUM, star.LACS_RES_LABEL);

        String[] xCoordsTmp = star.getFrameValues(frame,
	  star.LACS_RES_NUM, star.LACS_X_VALUE);
	lacs._xCoords = S2DUtils.arrayStr2Double(xCoordsTmp,
	  star.LACS_X_VALUE);
	xCoordsTmp = null;

        String[] yCoordsTmp = star.getFrameValues(frame,
	  star.LACS_RES_NUM, star.LACS_Y_VALUE);
	lacs._yCoords = S2DUtils.arrayStr2Double(yCoordsTmp,
	  star.LACS_Y_VALUE);
	yCoordsTmp = null;

        String[] desigsTmp = star.getFrameValues(frame,
	  star.LACS_RES_NUM, star.LACS_DESIGNATOR);
	lacs._desigs = S2DUtils.arrayStr2Int(desigsTmp, star.LACS_DESIGNATOR);
	desigsTmp = null;

	//
	// Now go ahead and calculate and write out the LACS values.
	//
	_summary.startFrame(star.getFrameDetails(frame));

	try {
	    lacs.writeLACS(frameIndex);
	} finally {
	    _summary.endFrame();
	}
    }

    //-------------------------------------------------------------------
    // Generate the pre-link summary page output for atomic coordinates.
    private void AtomicCoordSummaryPre(S2DStarIfc star,
      SaveFrameNode frame) throws S2DException
    {
        if (doDebugOutput(4)) {
            System.out.println("AtomicCoordSummaryPre(" + star + ", " +
	      frame + ")");
        }

	String details = star.getFrameDetails(frame);
	if (details == null) {
	    details = "Representative structure";
        }
	if (_currentPdbId != null) {
	    details += " from PDB ID " + _currentPdbId;
	}
	_summary.startFrame(details);
    }

    //-------------------------------------------------------------------
    // Process a PDB entry (get the atomic coordinates).
    private void process1PDB(S2DStarIfc star, String pdbId) throws S2DException
    {
        if (doDebugOutput(2)) {
	    System.out.println("process1PDB(" + pdbId + ")");
	}

        _currentPdbId = pdbId;

	if (_pdbLevel == PDB_LEVEL_LINK) {
	    AtomicCoordSummaryPre(star, null);
	    try {
	        _summary.writeAtomicCoordsCGI(_currentPdbId,
		  _currentFrameIndex);
            } catch(IOException ex) {
                System.err.println(
		  "IOException writing atomic coordinates: " +
		  ex.toString());
	        throw new S2DError("Can't write atomic coordinates");
	    } finally {
	        _summary.endFrame();
	    }
	} else {
	    S2DmmCifIfc cif = new S2DmmCifIfc(pdbId, _useLocalFiles);
	    saveFrameAtomicCoords(cif, null, _currentFrameIndex, null, false);
	}
	_currentFrameIndex++;

        _currentPdbId = null;
    }

    //-------------------------------------------------------------------
    // Determine whether to do debug output based on the current debug
    // level settings and the debug level of the output.
    private static boolean doDebugOutput(int level)
    {
    	if (DEBUG >= level || _verbosity >= level) {
	    if (level > 0) System.out.print("DEBUG " + level + ": ");
	    return true;
	}

	return false;
    }
}

// ========================================================================

// ========================================================================
// DEVise Data Visualization Software
// (c) Copyright 2000-2010
// By the DEVise Development Group
// Madison, Wisconsin
// All Rights Reserved.
// ========================================================================

// Under no circumstances is this software to be copied, distributed,
// or altered in any way without prior permission from the DEVise
// Development Group.

// ------------------------------------------------------------------------

// This class contains utility methods for Star to DEVise conversions.

// ------------------------------------------------------------------------

// $Id$

// $Log$
// Revision 1.22  2010/02/20 00:19:29  wenger
// Oops -- minor typo in last commit.
//
// Revision 1.21  2010/02/20 00:18:36  wenger
// Finished getting SPARTA processing to work with multiple entity
// assemblies (to-do 117) and multiple chemical shift lists per entity
// assembly (to-do 118); updated test_sparta 7 and test_sparta8 accordingly.
//
// Revision 1.20  2010/02/11 22:13:11  wenger
// Merged s2d_remediated_rest_1002_br_0 thru s2d_remediated_rest_1002_br_1
// to trunk (note: s2d_remediated_rest_1002_br_1 ==
// s2d_remediated_rest_1002_br_end).
//
// Revision 1.19.6.1  2010/02/03 23:13:20  wenger
// Torsion angle output from remediated restraints files now has different
// suffixes to avoid conflict with the restraint grid output; meta-data
// for remediated restraints doesn't have violations
//
// Revision 1.19  2009/12/05 22:26:32  wenger
// Merged s2d_torsion_rest_0910_br_0 thru s2d_torsion_rest_0910_br_0
// to the trunk.
//
// Revision 1.18.4.4  2009/12/05 21:17:03  wenger
// Converted dots to zeros in numerical torsion angle data so DEVise
// can read the data correctly.
//
// Revision 1.18.4.3  2009/11/30 18:09:01  wenger
// Got rid of sessions and specific html pages for (now obsolete) torsion
// angle violation visualizations.
//
// Revision 1.18.4.2  2009/10/29 20:06:43  wenger
// For torsion angles, the session files and specific html files are
// now created; there are some cursor behavior problems in the session
// files that need to be fixed, and also they are not loading right
// in the JavaScreen (I wonder if I need to have some kind of different
// setup in the JS client to handle the different highlighting).
// (There also seem to be problems with DEVise drawing the high/low
// symbols in some cases!)
//
// Revision 1.18.4.1  2009/10/22 18:28:31  wenger
// Early phases of setting up "atom IDs" (entity assembly ID/residue
// sequence code/atom name) values to facilitate linking of torsion
// angle data to coordinate data.
//
// Revision 1.18  2009/08/25 18:15:57  wenger
// Merged s2d_sparta_deltashift_br_0 thru s2d_sparta_deltashift_br_3
// to trunk.
//
// Revision 1.17  2009/07/20 22:33:48  wenger
// Implemented Peptide-CGI to-do 093 (derive Atom_type values from
// Atom_ID values if the Atom_type values don't exist).
//
// Revision 1.16.4.2  2009/07/22 20:20:23  wenger
// Fixed residue numbering in SPARTA delta shift visualizations;
// changed "theoretical" to "SPARTA-calculated" and changed method
// names, etc., to match.
//
// Revision 1.16.4.1  2009/07/01 20:57:51  wenger
// Data is now generated for SPARTA deltashift values; the link in
// the summary page is not written yet, though.
//
// Revision 1.16  2009/04/15 16:21:04  wenger
// Merged s2d_hc_spectrum_br_0 thru s2d_hc_spectrum_br_end to trunk;
// fixed test61 and test61_3.
//
// Revision 1.15.2.1  2009/04/14 22:09:07  wenger
// Session file, visualization-specific HTML file and summary page link
// are now created; removed "legend view" from session template;
// documented and cleaned up code.  (Still needs help for H vs C
// visualization.)
//
// Revision 1.15  2009/03/24 19:04:50  wenger
// Fixed layout of nucleic acid deltashift session (made windows line
// up better, etc.); fixed nucleotide counts in summary html page, and
// changed residue to nucleotide where appropriate; fixed nucleic acid
// deltashift html pages so that they link to the nucleic-acid-specific
// help page.
//
// Revision 1.14  2009/02/20 22:54:24  wenger
// RNA visualization now works, using the DNA template; added stub
// help file for nucleic acid deltashift visualizations.
//
// Revision 1.13  2009/02/20 18:41:47  wenger
// Preliminary version of DNA deltashift session template is now in place
// (still needs cleanup); Peptide-CGI code uses that session template for
// DNA processing.
//
// Revision 1.12  2009/02/18 21:43:07  wenger
// Added S2DNAChemShift class to clean up nucleic acid code (this class
// will do the actual calculation and writing of chemical shift deltas
// for nucleic acids); added schemas for nucleic acid deltashift
// visualizations; updated tests to reflect the fact that (at least
// initially) we're not going to generate CSI visualizations for nucleic
// acids.
//
// Revision 1.11  2008/06/27 15:23:33  wenger
// Added the option to explicitly call the garbage collector a number
// of times during processing; added memory usage printout at debug
// level 2.
//
// Revision 1.10  2008/06/17 23:07:53  wenger
// Fixed to-do 073:  we no longer generate figure of merit or ambiguity
// code visualizations if the values are all null (".").
//
// Revision 1.9  2008/02/20 17:41:08  wenger
// Committing (disabled) partially-implemented S2 Order visualization
// code and tests.
//
// Revision 1.8  2007/12/20 16:49:03  wenger
// Improved ChemShiftRef error messages; ChemShift calculation failing
// is no longer considered an error at the top level of the program;
// S2DSpecificHtml methods are no longer static so new S2DCSRErrorHtml
// class could inherit from it correctly; some cache checking output
// is now printed at a lower versbosity setting.
//
// Revision 1.7  2007/11/15 17:15:36  wenger
// Cleaned out cvs history in source files.
//
// Revision 1.6  2007/10/02 18:54:24  wenger
// More improvements to error and warning messages, including printing
// fewer at the default verbosity setting.
//
// Revision 1.5  2007/10/02 17:39:53  wenger
// More debug output; updated the version history with changes from last
// commit.
//
// Revision 1.4  2007/08/20 20:26:10  wenger
// Added -verb command-line flag and property so we can turn on debug
// output without recompiling; added debug_level property corresponding
// to the existing -debug command-line flag.
//
// Revision 1.3  2006/02/01 21:34:32  wenger
// Merged peptide_cgi_10_8_0_br_0 thru peptide_cgi_10_8_0_br_2
// to the trunk.
//
// ...

// ========================================================================

package star2devise;

import java.io.*;
import java.util.*;
import java.text.DateFormat;
import java.net.*;

public class S2DUtils
{
    //===================================================================
    // VARIABLES

    private static final int DEBUG = 0;

    public static final int TYPE_INVALID = 0, TYPE_DELTASHIFT = 1,
      TYPE_CSI = 2, TYPE_PCT_ASSIGN = 3, TYPE_COUPLING = 4,
      TYPE_HXRATES = 5, TYPE_ORDER = 6, TYPE_T1_RELAX = 7, TYPE_T2_RELAX = 8,
      TYPE_HETNOE = 9, TYPE_ALL_CHEM_SHIFTS = 10, TYPE_HVSN_CHEM_SHIFTS = 11,
      TYPE_ATOMIC_COORDS = 12, TYPE_CHEM_SHIFT_REF1 = 13,
      TYPE_CHEM_SHIFT_REF2 = 14, TYPE_CHEM_SHIFT_REF3 = 15,
      TYPE_PISTACHIO = 16, TYPE_AMBIGUITY = 17, TYPE_LACS = 18,
      TYPE_DNA_DELTASHIFT = 19, TYPE_RNA_DELTASHIFT = 20,
      TYPE_HVSC_CHEM_SHIFTS = 21, TYPE_SPARTA_DELTASHIFT = 22,
      TYPE_TORSION_ANGLE = 23, TYPE_RRTORSION_ANGLE = 24;

    //===================================================================
    // PUBLIC METHODS

    //-------------------------------------------------------------------
    public static String[] createStringArray(int size, String value)
    {
    	String[] result = new String[size];
	for (int index = 0; index < size; index++) {
	    result[index] = value;
	}

	return result;
    }

    //-------------------------------------------------------------------
    public static double[] arrayStr2Double(String[] values, String name)
    {
	int count = values.length;
	double[] results = new double[count];

        for (int index = 0; index < count; index++) {
	    try {
	        results[index] = new Double(values[index]).doubleValue();
	    } catch(NumberFormatException ex) {
		// Note: the S2DWarning object below is *not* supposed to be
		// thrown...
		S2DWarning warning = new S2DWarning(
		  "Exception parsing double (" + name + "[" + index +
		  "]): " + ex.toString());
		if (doDebugOutput(1)) {
		    System.err.println(warning);
		}
	        results[index] = Double.NaN;
	    }
	}

        return results;
    }

    //-------------------------------------------------------------------
    public static int[] arrayStr2Int(String[] values, String name)
    {
	int count = values.length;
	int[] results = new int[count];

        for (int index = 0; index < count; index++) {
	    try {
	        results[index] = Integer.parseInt(values[index]);
	    } catch(NumberFormatException ex) {
		S2DWarning warning = new S2DWarning(
		  "Exception parsing int (" + name + "[" + index +
		  "]): " + ex.toString());
		if (doDebugOutput(1)) {
		    System.err.println(warning);
		}
	        results[index] = 0;
	    }
	}

        return results;
    }

    //-------------------------------------------------------------------
    public static String[] arrayToUpper(String[] values)
    {
	int count = values.length;
	String[] results = new String[count];

        for (int index = 0; index < count; index++) {
	    results[index] = values[index].toUpperCase();
	}
        
        return results;
    }

    //-------------------------------------------------------------------
    // Change any array values that are "." to "0".
    public static String[] arrayDot2Zero(String[] values)
    {
	int count = values.length;
	String[] results = new String[count];

        for (int index = 0; index < count; index++) {
	    if (values[index].equals(".")) {
	        results[index] = "0";
	    } else {
	        results[index] = values[index];
	    }
	}
        
        return results;
    }

    //-------------------------------------------------------------------
    /**
     * Convert atom name values to atom types.
     * @param An array of atom names.
     * @return An array of atom types.
     */
    public static String[] atomName2AtomType(String[] atomNames)
      throws S2DException
    {
    	String[] atomTypes = new String[atomNames.length];
	for (int index = 0; index < atomNames.length; index++) {
	    atomTypes[index] = atomNames[index].substring(0, 1);
	    if (!atomTypes[index].equals(S2DNames.ATOM_H) &&
	      !atomTypes[index].equals(S2DNames.ATOM_C) &&
	      !atomTypes[index].equals(S2DNames.ATOM_O) &&
	      !atomTypes[index].equals(S2DNames.ATOM_N) &&
	      !atomTypes[index].equals(S2DNames.ATOM_S) &&
	      !atomTypes[index].equals(S2DNames.ATOM_P)) {
	    	throw new S2DError("Illegal atom type: " +
		  atomTypes[index] + " derived from atom name " +
		  atomNames[index]);
	    }
	}

	return atomTypes;
    }

    //-------------------------------------------------------------------
    /**
     * Select all values in values for which the corresponding keys value
     * matches matchWith.
     * @param An array of values to match with.
     * @param An array of values to be selected from.
     * @param The value to match with.
     * @return An array of values selected from the values array
     *  (a one-element array with an element of "" if no matches).
     */
    public static String[] selectMatches(String[] keys, String[] values,
      String matchWith) throws S2DException
    {
        if (keys.length != values.length) {
	    throw new S2DError("Arrays of unequal length in " +
	      "S2DUtils.selectMatches");
	}

        int matchCount = 0;
	for (int index = 0; index < values.length; index++ ) {
	    if (keys[index].equals(matchWith)) matchCount++;
	}

        if (matchCount < 1) {
	    if (doDebugOutput(11)) {
	        System.err.println(new S2DWarning(
		  "S2DUtils.selectMatches(): match value <" +
		  matchWith + "> selects no values"));
	    }
	}

	String[] result;
	if (matchCount == values.length) {
	    result = values;
	} else {
	    result = new String[matchCount];

	    int outIndex = 0;
	    for (int index = 0; index < values.length; index++ ) {
	    if (keys[index].equals(matchWith)) {
	            result[outIndex++] = values[index];
	        }
	    }
	}

	return result;
    }

    //-------------------------------------------------------------------
    /**
     * Determine whether all values in an array match a given target
     * value.
     * @param An array of strings to test.
     * @param The target value to check against.
     * @return True iff all values in the array match the target value.
     */
    public static boolean entireArrayMatches(String[] values, String target)
    {
        for (int index = 0; index < values.length; index++) {
	    if (!values[index].equals(target)) {
	        return false;
	    }
	}

	return true;
    }

    //-------------------------------------------------------------------
    public static String replace(String str, String pattern, String replace)
    {
        int s = 0;
        int e = 0;
        StringBuffer result = new StringBuffer();

        while ((e = str.indexOf(pattern, s)) >= 0) {
            result.append(str.substring(s, e));
            result.append(replace);
            s = e+pattern.length();
        }
        result.append(str.substring(s));
        return result.toString();
    }

    //-------------------------------------------------------------------
    public static void copyFile(String inFile, String outFile)
      throws IOException
    {
        if (doDebugOutput(11)) {
	    System.out.println("S2DUtils.copyFile(" + inFile + ", " +
	      outFile + ")");
	}

	Reader reader = new FileReader(inFile);
	Writer writer = S2DFileWriter.create(outFile);

	char[] buffer = new char[8192];
	int totalBytes = 0;

	while (true) {
	    int tmpCount = reader.read(buffer);
	    if (tmpCount == -1) break;
	    writer.write(buffer, 0, tmpCount);
	    totalBytes += tmpCount;

	    if (doDebugOutput(12)) {
	        System.out.println("  Copied " + totalBytes +
		  " bytes so far");
	    }
	}

	if (doDebugOutput(11)) {
	    System.out.println("  Copied " + totalBytes + " bytes total");
	}

	writer.close();
	reader.close();
    }

    //-------------------------------------------------------------------
    public static void deleteFile(String fileName) throws IOException
    {
        File file = new File(fileName);
	if (file.exists()) {
	    if (!file.delete()) {
	        throw new IOException("Failed to delete existing file " +
		  fileName);
	    }
	}
    }

    //-------------------------------------------------------------------
    public static String getCurrentDateStr()
    {
        Date date = new Date();
	DateFormat dtf = DateFormat.getDateTimeInstance(DateFormat.MEDIUM,
	  DateFormat.MEDIUM);
        String str = dtf.format(date);

	return str;
    }

    //-------------------------------------------------------------------
    /**
     * Compare two version strings.
     * @param The first version string.
     * @param The second version string.
     * @return -1 if the first version is less than the second; 0 if
     *   the versions are equal; 1 if version1 is greater than version2.
     */
    public static int compareVersions(String version1, String version2)
      throws S2DError
    {
    	MyVersion v1 = new MyVersion(version1);
    	MyVersion v2 = new MyVersion(version2);

	if (v1.major > v2.major) {
	    return 1;
	} else if (v1.major < v2.major) {
	    return -1;
	} else if (v1.minor > v2.minor) {
	    return 1;
	} else if (v1.minor < v2.minor) {
	    return -1;
	} else if (v1.rev > v2.rev) {
	    return 1;
	} else if (v1.rev < v2.rev) {
	    return -1;
        } else {
	    return 0;
	}
    }

    //-------------------------------------------------------------------
    // Translate a data type into the corresponding output file suffix.
    public static String dataType2Suffix(int dataType) throws S2DError
    {
	String dataSuffix;

	switch (dataType) {
	case S2DUtils.TYPE_DELTASHIFT:
	case S2DUtils.TYPE_DNA_DELTASHIFT:
	case S2DUtils.TYPE_RNA_DELTASHIFT:
	    dataSuffix = S2DNames.DELTASHIFT_SUFFIX;
	    break;

	case S2DUtils.TYPE_CSI:
	    dataSuffix = S2DNames.CSI_SUFFIX;
	    break;

	case S2DUtils.TYPE_PCT_ASSIGN:
	    dataSuffix = S2DNames.PERCENT_ASSIGN_SUFFIX;
	    break;

	case S2DUtils.TYPE_ALL_CHEM_SHIFTS:
	    dataSuffix = S2DNames.ALL_CHEM_SHIFT_SUFFIX;
	    break;

	case S2DUtils.TYPE_HVSN_CHEM_SHIFTS:
	    dataSuffix = S2DNames.HVSN_CHEM_SHIFT_SUFFIX;
	    break;

	case S2DUtils.TYPE_HVSC_CHEM_SHIFTS:
	    dataSuffix = S2DNames.HVSC_CHEM_SHIFT_SUFFIX;
	    break;

        case S2DUtils.TYPE_COUPLING:
	    dataSuffix = S2DNames.COUPLING_SUFFIX;
	    break;

/*TEMP
        case S2DUtils.TYPE_HXRATES:
	    dataSuffix = S2DNames.HX_RATE_SUFFIX;
	    break;
TEMP*/

        case S2DUtils.TYPE_ORDER:
	    dataSuffix = S2DNames.ORDER_SUFFIX;
	    break;

        case S2DUtils.TYPE_T1_RELAX:
	    dataSuffix = S2DNames.T1_SUFFIX;
	    break;

        case S2DUtils.TYPE_T2_RELAX:
	    dataSuffix = S2DNames.T2_SUFFIX;
	    break;

        case S2DUtils.TYPE_HETNOE:
	    dataSuffix = S2DNames.HETERONUCLEAR_NOE_SUFFIX;
	    break;

	case S2DUtils.TYPE_ATOMIC_COORDS:
	    dataSuffix = S2DNames.ATOMIC_COORD_SUFFIX;
	    break;

	case S2DUtils.TYPE_CHEM_SHIFT_REF1:
	    dataSuffix = S2DNames.CSR1_SUFFIX;
	    break;

	case S2DUtils.TYPE_CHEM_SHIFT_REF2:
	    dataSuffix = S2DNames.CSR2_SUFFIX;
	    break;

	case S2DUtils.TYPE_CHEM_SHIFT_REF3:
	    dataSuffix = S2DNames.CSR3_SUFFIX;
	    break;

	case S2DUtils.TYPE_PISTACHIO:
	    dataSuffix = S2DNames.PISTACHIO_SUFFIX;
	    break;

	case S2DUtils.TYPE_AMBIGUITY:
	    dataSuffix = S2DNames.AMBIGUITY_SUFFIX;
	    break;

	case S2DUtils.TYPE_LACS:
	    dataSuffix = S2DNames.LACS_SUFFIX;
	    break;

	case S2DUtils.TYPE_SPARTA_DELTASHIFT:
	    dataSuffix = S2DNames.SPARTA_DELTASHIFT_SUFFIX;
	    break;

	case S2DUtils.TYPE_TORSION_ANGLE:
	    dataSuffix = S2DNames.TAR_SUFFIX;
	    break;

	case S2DUtils.TYPE_RRTORSION_ANGLE:
	    dataSuffix = S2DNames.RRTAR_SUFFIX;
	    break;

	default:
	    throw new S2DError("Illegal data type: " + dataType);
	}

	return dataSuffix;
    }

    //-------------------------------------------------------------------
    // Print information about the current memory usage.
    public static void printMemory(String label)
    {
	System.out.println(label);
    	Runtime rt = Runtime.getRuntime();
	System.out.println("  Total memory: " + rt.totalMemory());
	System.out.println("  Free memory:  " + rt.freeMemory());
	System.out.println("  Max memory:   " + rt.maxMemory());
    }

    //-------------------------------------------------------------------
    // Create an AtomId string (uniquely identifies an atom).
    public static String createAtomId(int entityAssemblyId,
      String residueSeqCode, String atomName)
    {
        String entStr = "";
	entStr += entityAssemblyId;

	return createAtomId(entStr, residueSeqCode, atomName);
    }

    //-------------------------------------------------------------------
    // Create an AtomId string (uniquely identifies an atom).
    public static String createAtomId(String entityAssemblyId,
      String residueSeqCode, String atomName)
    {
        return entityAssemblyId + "_" + residueSeqCode + "_" + atomName;
    }

    //-------------------------------------------------------------------
    /**
     * Try a URL (find out whether it actually exists).  If the URL
     * exists, we simply return; otherwise we throw an S2DError.
     * @param The string representing the URL.
     */
    public static void tryUrl(String urlName) throws S2DError
    {
        if (doDebugOutput(11)) {
            System.out.println("Trying URL: <" + urlName + ">");
        }
        try {
            URL url = new URL(urlName);
            InputStream is = url.openStream();
            is.close();
        } catch(Exception ex) {
            if (doDebugOutput(11)) {
                System.out.println("Unable to open stream from URL " +
                urlName + " : " + ex.toString());
            }
	    throw new S2DError("URL " + urlName + " is not valid");
        }
    }

    //===================================================================
    // PRIVATE METHODS
    private static class MyVersion
    {
        public int major;
	public int minor;
	public int rev;

	public MyVersion(String versionString) throws S2DError
	{
            String[] substrs = versionString.split("[\\.x]");
	    if (substrs.length < 2) {
	        throw new S2DError("Incomplete version string: " +
		  versionString);
	    }

	    major = Integer.parseInt(substrs[0]);
	    minor = Integer.parseInt(substrs[1]);
	    rev = Integer.parseInt(substrs[2]);
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

// ========================================================================
// DEVise Data Visualization Software
// (c) Copyright 2000-2008
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
      TYPE_PISTACHIO = 16, TYPE_AMBIGUITY = 17, TYPE_LACS = 18;

    //===================================================================
    // PUBLIC METHODS

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
    /**
     * Select all values in values for which the corresponding keys value
     * matches matchWith.
     * @param An array of values to match with.
     * @param An array of values to be selected from.
     * @param The value to match with.
     * @return An array of values selected from the values array.
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

	default:
	    throw new S2DError("Illegal data type: " + dataType);
	}

	return dataSuffix;
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

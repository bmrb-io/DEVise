// ========================================================================
// DEVise Data Visualization Software
// (c) Copyright 2000-2001
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
// Revision 1.3.2.1  2001/02/09 16:57:13  wenger
// Added heteronuclear NOE; made T1 and T2 relaxation errors optional
// (conversion doesn't fail if they are not found); added an X margin of
// 0.5 in all sessions; updated star file list; misc. minor cleanups.
//
// Revision 1.3  2001/01/19 15:39:07  wenger
// Added T1 and T2 relaxation; removed some unnecessary variables from
// coupling constants; added schema files to installation, unified T1
// and T2 relaxation schema.
//
// Revision 1.2  2001/01/17 19:55:46  wenger
// Restructured the peptide-cgi code to make it much more maintainable.
//
// Revision 1.1  2000/08/11 21:41:34  wenger
// Fixed problems with the code assuming that BMRB accession numbers always
// have four digits.
//

// ========================================================================


public class S2DUtils
{
    //===================================================================
    // VARIABLES

    public static final int TYPE_INVALID = 0, TYPE_DELTASHIFT = 1,
      TYPE_CSI = 2, TYPE_PCT_ASSIGN = 3, TYPE_COUPLING = 4,
      TYPE_HXRATES = 5, TYPE_ORDER = 6, TYPE_T1_RELAX = 7, TYPE_T2_RELAX = 8,
      TYPE_HETNOE = 9;

    public static final String starPrefix = "bmr";
    public static final String starSuffix = ".str";

    //===================================================================
    // PUBLIC METHODS

    //-------------------------------------------------------------------
    // Convert a Star file name to the accession number.
    public static String starName2Num(String fileName)
      throws S2DException
    {
	String result = null;

	int prefInd = fileName.indexOf(starPrefix);
	int suffInd = fileName.indexOf(starSuffix);
	int numInd = prefInd + starPrefix.length();

	if (prefInd != 0 || suffInd == -1 || (numInd >= suffInd)) {
	    throw new S2DError("File name (" + fileName +
	      ") may not be a valid BMRB Star file name");
	} else {
	    result = fileName.substring(numInd, suffInd);
        }

	return result;
    }

    //-------------------------------------------------------------------
    public static double[] arrayStr2Double(String[] values)
    {
	int count = values.length;
	double[] results = new double[count];

        for (int index = 0; index < count; index++) {
	    try {
	        results[index] = new Double(values[index]).doubleValue();
	    } catch(NumberFormatException ex) {
	        System.err.println("Exception parsing double: " +
		  ex.getMessage());
	        results[index] = 0.0;
	    }
	}

        return results;
    }

    //-------------------------------------------------------------------
    public static int[] arrayStr2Int(String[] values)
    {
	int count = values.length;
	int[] results = new int[count];

        for (int index = 0; index < count; index++) {
	    try {
	        results[index] = Integer.parseInt(values[index]);
	    } catch(NumberFormatException ex) {
	        System.err.println("Exception parsing int: " +
		  ex.getMessage());
	        results[index] = 0;
	    }
	}

        return results;
    }

    //-------------------------------------------------------------------
    static String replace(String str, String pattern, String replace)
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
}

// ========================================================================

// ========================================================================
// DEVise Data Visualization Software
// (c) Copyright 2000
// By the DEVise Development Group
// Madison, Wisconsin
// All Rights Reserved.
// ========================================================================

// Under no circumstances is this software to be copied, distributed,
// or altered in any way without prior permission from the DEVise
// Development Group.

// ------------------------------------------------------------------------

// Utility methods for Star to DEVise conversions.

// ------------------------------------------------------------------------

// $Id$

// $Log$

// ========================================================================


public class S2DUtils
{
    public static final String starPrefix = "bmr";
    public static final String starSuffix = ".str";

    // Convert a Star file name to the accession number.
    public static String starName2Num(String fileName)
      throws S2DException
    {
	String result = null;

	int prefInd = fileName.indexOf(starPrefix);
	int suffInd = fileName.indexOf(starSuffix);
	int numInd = prefInd + starPrefix.length();

	if (prefInd != 0 || suffInd == -1 || (numInd >= suffInd)) {
	    throw new S2DException("File name (" + fileName +
	      ") may not be a valid BMRB Star file name");
	} else {
	    result = fileName.substring(numInd, suffInd);
        }

	return result;
    }
}

// ========================================================================

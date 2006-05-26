// ========================================================================
// DEVise Data Visualization Software
// (c) Copyright 2006
// By the DEVise Development Group
// Madison, Wisconsin
// All Rights Reserved.
// ========================================================================

// Under no circumstances is this software to be copied, distributed,
// or altered in any way without prior permission from the DEVise
// Development Group.

// ------------------------------------------------------------------------

// This class is used to create data to feed to Jmol.  It translates
// DEViseGData objects to a String in PDB file format.

// ------------------------------------------------------------------------

// $Id$

// $Log$
// Revision 1.1.2.1  2006/02/23 16:57:42  wenger
// Cleaned up JavaScreen code that sends data to Jmol, including
// adding new DEViseJmolData class.
//

// ========================================================================

// DEViseJmolData.java

//TEMP package edu.wisc.cs.devise.js.jsc;
package JavaScreen;

import  java.util.*;

public class DEViseJmolData
{
    //===================================================================
    // VARIABLES

    private static final int DEBUG = 0; // 0 - 3

    //===================================================================
    // PUBLIC METHODS

    //-------------------------------------------------------------------
    // Create a string (in the form of a PDB file) to feed to Jmol.
    // The string represents the data given by the GData.
    // Note: see jmol/adapter/smarter/PdbReader.java.
    public static String createJmolData(Vector gDatas)
    {
    	if (DEBUG >= 1) {
	    System.out.println("DEViseJmolData.createJmolData()");
	}

	String jmolData = "";

	for (int index = 0; index < gDatas.size(); index++) {

	    DEViseGData gd = (DEViseGData)gDatas.elementAt(index);

	    if (gd.symbolType != DEViseGData._symOval) {
	    	System.err.println("ERROR: illegal symbol type (" +
		  gd.symbolType + ") for Jmol view");
	        continue;
	    }

	    //
	    // Note: column spacing is critical!!
	    //
	    String tmpData = "ATOM";
	    tmpData += lengthenStringPrepend("" + gd.atomNum, 7);
	    tmpData += " "; // space before atom name

	    // If the atom name has four characters (e.g., 1HH1) the
	    // H, not the leading 1, has to line up with the first
	    // character of the other atom names (e.g., NH1).
	    if (gd.atomName.length() < 4) {
	        gd.atomName = " " + gd.atomName;
	    }
	    tmpData += lengthenStringAppend(gd.atomName, 4);
	    tmpData += " "; // space before residue label

	    tmpData += gd.residueLabel + " ";
	    tmpData += " "; // chain ID
	    tmpData += lengthenStringPrepend("" + gd.residueNum, 4);
	    tmpData += "    "; // insertion code
	    tmpData += lengthenStringPrepend("" + gd.x0, 8);
	    tmpData += lengthenStringPrepend("" + gd.y0, 8);
	    tmpData += lengthenStringPrepend("" + gd.z0, 8);
	    tmpData += "                      "; // occupancy and b factor
	    tmpData += lengthenStringPrepend(gd.atomType, 2);

    	    if (DEBUG >= 3) {
		System.out.println(tmpData);
	    }

            jmolData += tmpData + "\n";
	}

	return jmolData;
    }

    //===================================================================
    // PRIVATE METHODS

    //-------------------------------------------------------------------
    // Lengthen a string to the given length by prepending spaces.
    private static String lengthenStringPrepend(String str1, int length)
    {
    	while (str1.length() < length) {
	    str1 = " " + str1;
	}

	return str1;
    }

    //-------------------------------------------------------------------
    // Lengthen a string to the given length by appending spaces.
    private static String lengthenStringAppend(String str1, int length)
    {
    	while (str1.length() < length) {
	    str1 += " ";
	}

	return str1;
    }
}

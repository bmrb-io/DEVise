// ========================================================================
// DEVise Data Visualization Software
// (c) Copyright 2006-2009
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
// Revision 1.5.4.1  2009/06/11 20:56:59  wenger
// Made changes to the JavaScreen client to allow highlighting by
// specific atom as well as the existing capability of highlighting
// by residue (this is for the torsion angle restraints visualization).
//
// Revision 1.5  2009/03/04 17:42:01  wenger
// Changed the code that converts GData to PDB for Jmol slightly to
// work with DNA and RNA (the formatting was incorrect if the residue
// labels weren't three characters).  This makes highlighting work
// for DNA and RNA.
//
// Revision 1.4  2008/09/05 20:34:48  wenger
// (Hopefully, at least) fininshed JavaScreen changes for multiple
// entity assemblies -- we now send alphabetic chain IDs to Jmol,
// so that we can at least have 26 entity assemblies.
//
// Revision 1.3  2008/09/03 19:15:59  wenger
// Initial changes to JavaScreen client to support entity assembly
// IDs in 3D Jmol visualizations.  (Still needs some cleanup.)
//
// Revision 1.2  2006/05/26 16:22:16  wenger
// Merged devise_jmol_br_0 thru devise_jmol_br_1 to the trunk.
//
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
	    String atomName = gd.atomName;
	    if (atomName.length() < 4) {
	        atomName = " " + atomName;
	    }
	    tmpData += lengthenStringAppend(atomName, 4);
	    tmpData += " "; // space before residue label

	    // Get spacing right for DNA and RNA.
	    String resLabel = gd.residueLabel;
	    while (resLabel.length() < 3) {
	        resLabel += " ";
	    }
	    tmpData += resLabel + " ";

	    tmpData += entAssem2Chain(gd.entityAssemblyID); // chain ID
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

    //-------------------------------------------------------------------
    // Convert a numerical entity assembly ID to a chain letter ID.
    // (This is done so we can have up to 26 entity assemblies designated
    // with a single character.)  We pass the chain ID to Jmol;
    // everything else uses the numerical entity assembly IDs.
    public static String entAssem2Chain(int entAssemID)
    {
    	String[] chains = { "A", "B", "C", "D", "E", "F", "G", "H", "I",
	  "J", "K", "L", "M", "N", "O", "P", "Q", "R", "S", "T", "U",
	  "V", "W", "X", "Y", "Z" };

	String result = chains[entAssemID - 1];

	return result;
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

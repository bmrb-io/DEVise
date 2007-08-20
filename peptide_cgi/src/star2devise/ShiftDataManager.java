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

// Manages chemical shift info.

// ------------------------------------------------------------------------

// $Id$

// $Log$
// Revision 1.2  2006/02/01 20:23:14  wenger
// Merged V2_1b4_br_0 thru peptide_cgi_10_8_0_base to the
// trunk.
//
// Revision 1.1.2.4  2005/01/31 21:35:44  wenger
// Slight mods to atomic coordinates template for better highlight
// selection; s2d chem shift ref timeout increased to 120 sec., added
// try-again message if timed out; cleaned up warning/error messages
// somewhat.
//
// Revision 1.1.2.3  2004/03/04 18:26:42  wenger
// Fixed bug 030 (static variable problems in Jafar); removed
// S2DTmpFile (no longer used).
//
// Revision 1.1.2.2  2004/01/22 16:43:49  wenger
// Finished to-do item 020 (replace "/" with File.separator), other
// minor cleanups.
//
// Revision 1.1.2.1  2003/04/22 21:58:17  wenger
// Added package name to peptide-cgi java code and put everything into
// a single jar file; version is now 6.0.
//
// Revision 1.6.20.1  2003/01/14 16:51:50  wenger
// Merged V2_1b3_br_0 thru V2_1b3_br_end to V2_1b4_br.
//
// Revision 1.6.18.1  2002/10/31 22:17:36  wenger
// Changed Exception output to use toString() instead of getMessage()
// because that (at least sometimes) produces more complete information.
//
// Revision 1.6  2001/01/25 16:37:46  wenger
// Fixed a bug that could cause an infinite loop in the perecent assignment
// calculations; put filesize, cpu, and coredump limits in s2d script;
// updated star file list; got rid of some unnecessary warnings.
//
// Revision 1.5  2001/01/17 19:55:46  wenger
// Restructured the peptide-cgi code to make it much more maintainable.
//
// Revision 1.4  2000/08/03 19:11:52  wenger
// Added S2DException class; better tolerance for certain missing data.
//
// Revision 1.3  2000/08/02 17:47:45  wenger
// Greatly improved error handling.
//
// Revision 1.2  2000/07/27 16:11:23  wenger
// Added standard DEVise headers.
//

// ========================================================================

package star2devise;

import java.io.*;

//The following class maintains data contained in the shift.txt table
// Converted from dsilva's C++ code to Java by Richard Chang, 06/25/1999
public class ShiftDataManager
{
    private static final int DEBUG = 0;

    private static final int MAX_DATA_ENTRIES = 500;
    private String filename;

    // Residue label name
    private String [] aminoAcidType = new String[MAX_DATA_ENTRIES];

    // Atom name
    private String [] atomName = new String[MAX_DATA_ENTRIES];

    // Standard value of chemical shift
    private double [] chemicalShiftCorr = new double[MAX_DATA_ENTRIES];

    // The range column vals
    private double [] range = new double[MAX_DATA_ENTRIES];

    // Total number of entries in the table
    private int totalNumEntries; 


    public class Pair
    {
	public double chemshift;
	public double offset;
    }


    // Constructor which initializes the object by reading the data file 
    ShiftDataManager( String filename )
      throws S2DException
    {
	if (doDebugOutput(11)) {
	    System.out.println("ShiftDataManager(" + filename + ")");
	}

	try
	{
	    StreamTokenizer inFile = 
		new StreamTokenizer(new FileReader(filename));
	    totalNumEntries = 0;
	    this.filename = filename;
	    
	    // This assumes that each entry corresponds to one line
	    while (inFile.nextToken() != inFile.TT_EOF)
	    {
		aminoAcidType[totalNumEntries] = inFile.sval;
		inFile.nextToken();
		atomName[totalNumEntries] = inFile.sval;
		inFile.nextToken();
		chemicalShiftCorr[totalNumEntries] = inFile.nval;
		inFile.nextToken();
		range[totalNumEntries] = inFile.nval;
		totalNumEntries++;
		
	    }

	} catch (FileNotFoundException e) {
	    System.err.println("File not found: " + e.toString() );
	    throw new S2DError("Unable to open or read shift data file " +
	      filename);
	} catch (IOException e) {
	    System.err.println("IO Exception: " + e.toString() );
	    throw new S2DError("Unable to open or read shift data file " +
	      filename);
	}
    } // end constructor

    
    // A function that takes as input arguments a residue name and an
    // atom name and returns a pair of values -- standard value of
    // chemical shift and the range. Also the reference parameter.
    public Pair returnValues(String in_aminoAcidType, String in_atomName)
      throws S2DException
    {
	if (doDebugOutput(11)) {
	    System.out.println("ShiftDataManager.returnValues(" +
	      in_aminoAcidType + ", " + in_atomName + ")");
	}

	Pair retValues = new Pair();
	
	// Initialize with default error values
	retValues.chemshift = -1.0;
	retValues.offset    = -1.0;
	    
	int position = 0;

	boolean foundAminoAcidType = false;
	while((!foundAminoAcidType) && position < totalNumEntries)
	{
	    if(in_aminoAcidType.compareTo(aminoAcidType[position]) == 0) {
		foundAminoAcidType = true;
	    } else {
		position++;
	    }
	}

	if(foundAminoAcidType)
	{
	    boolean foundAtomName = false;
	    while(position < totalNumEntries && (!foundAtomName) &&
	      in_aminoAcidType.compareTo(aminoAcidType[position]) == 0) {
	        if(in_atomName.compareTo(atomName[position]) == 0) {
	            foundAtomName = true;
	        } else {
		    position++;
	        }
	    }
		
	    if(!foundAtomName) {
		if (doDebugOutput(11)) {
		    throw new S2DWarning("Warning: atom " +
		      in_atomName + " corresponding to amino acid " +
		      in_aminoAcidType + " not found in chemical " +
		      "shift reference table file " + filename);
		}
	    }
	    
	} else {
	    throw new S2DWarning("Amino acid type " + in_aminoAcidType +
	      "  not found..... \n");
	}
	    
	retValues.chemshift = chemicalShiftCorr[position];
	retValues.offset  = range[position];
	return retValues;
    } // end function returnValues

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

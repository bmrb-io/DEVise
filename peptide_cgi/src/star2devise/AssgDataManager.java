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

// Reads assignment file, generates numbers for percentage assignments
// of residues.

// ------------------------------------------------------------------------

// $Id$

// $Log$
// Revision 1.5  2007/11/15 17:15:34  wenger
// Cleaned out cvs history in source files.
//
// Revision 1.4  2007/08/21 18:56:29  wenger
// Improved debug output -- better verbosity levels, etc.
//
// Revision 1.3  2007/08/20 20:26:05  wenger
// Added -verb command-line flag and property so we can turn on debug
// output without recompiling; added debug_level property corresponding
// to the existing -debug command-line flag.
//
// Revision 1.2  2006/02/01 20:23:09  wenger
// Merged V2_1b4_br_0 thru peptide_cgi_10_8_0_base to the
// trunk.
//
// Revision 1.1.2.3  2005/01/31 21:35:44  wenger
// Slight mods to atomic coordinates template for better highlight
// selection; s2d chem shift ref timeout increased to 120 sec., added
// try-again message if timed out; cleaned up warning/error messages
// somewhat.
//
// ...

// ========================================================================

package star2devise;

import java.io.*;
// import java.util.StringTokenizer;

public class AssgDataManager
{
    private static final int DEBUG = 0;

    public class AssgEntry
    {
	String residueLabel;
	int numH;
	int numC;
	int numN;
    }

    private static final int MAX_ENTRIES = 30;

    private static String filename;

    private AssgEntry [] assgInfo = new AssgEntry[MAX_ENTRIES];
    private int totalNumEntries;

    //Reads in the input assignment file and stores in memory data structure
    public AssgDataManager( String filename )
      throws S2DException
    {
        if (doDebugOutput(11)) {
	    System.out.println("AssgDataManager(" + filename + ")");
	}

	try
	{
	    StreamTokenizer inFile 
		= new StreamTokenizer (new FileReader( filename ));
            inFile.slashSlashComments(true);

	    totalNumEntries = 0;
	    this.filename = filename;
	    
	    // This assumes that each entry corresponds to one line
	    while (inFile.nextToken() != inFile.TT_EOF)
	    {
		assgInfo[totalNumEntries] = new AssgEntry();

		assgInfo[totalNumEntries].residueLabel = inFile.sval;

		inFile.nextToken();
		assgInfo[totalNumEntries].numH = (int)inFile.nval;

		inFile.nextToken();
		assgInfo[totalNumEntries].numC = (int)inFile.nval;

		inFile.nextToken();
		assgInfo[totalNumEntries].numN = (int)inFile.nval;

		if(totalNumEntries++ == MAX_ENTRIES-1) 
		{
		    throw new S2DError("Overflow of assgInfo array.");
		}
		
	    }
	    
	} catch (FileNotFoundException e) 
	{
	    System.err.println("File not found: " + e.toString() );
	    throw new S2DError("Unable to read assignment file " +
	      filename);
	} catch (IOException e)
	{
	    System.err.println("IO Exception: " + e.toString() );
	    throw new S2DError("Unable to read assignment file " +
	      filename);
	}
    }
    

    // Given a particular residue label, returns (by reference) total
    // Hydrogen, Carbon and Nitrogen assignments
    public AssgEntry returnAssg(String residueLabel) 
      throws S2DException
    {
        if (doDebugOutput(15)) {
	    System.out.println("AssgDataManager.returnAssg(" +
	      residueLabel + ")");
	}
	
	AssgEntry totalAssignments = new AssgEntry();
	boolean found = false;
	int position = 0;
	totalAssignments.numH = -1;
	totalAssignments.numC = -1;
	totalAssignments.numN = -1;
	
	while(!found && position < totalNumEntries)
	{
	    if (residueLabel.compareTo(assgInfo[position].residueLabel) == 0) {
		totalAssignments.numH = assgInfo[position].numH;
		totalAssignments.numC = assgInfo[position].numC;
		totalAssignments.numN = assgInfo[position].numN;
		found = true;
		
	    } else {
		position++;
	    }
	}
	
	if (!found) {
	    throw new S2DWarning("Warning: " + residueLabel +
	      " entry not found in assignment " +
	      "reference table file " + filename );
	}

	return totalAssignments;
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

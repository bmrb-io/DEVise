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

// Reads assignment file, generates numbers for percentage assignments
// of residues.

// ------------------------------------------------------------------------

// $Id$

// $Log$
// Revision 1.4  2000/08/03 19:11:51  wenger
// Added S2DException class; better tolerance for certain missing data.
//
// Revision 1.3  2000/08/02 17:47:44  wenger
// Greatly improved error handling.
//
// Revision 1.2  2000/07/27 16:11:23  wenger
// Added standard DEVise headers.
//

// ========================================================================

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
        if (DEBUG >= 1) {
	    System.out.println("AssgDataManager(" + filename + ")");
	}

	try
	{
	    StreamTokenizer inFile 
		= new StreamTokenizer (new FileReader( filename ));
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
		    System.err.println("Overflow of assgInfo array.");
		    System.exit(0);
		}
		
	    }
	    
	} catch (FileNotFoundException e) 
	{
	    System.err.println("File not found: " + e.getMessage() );
	    throw new S2DError("Unable to read assignment file " +
	      filename);
	} catch (IOException e)
	{
	    System.err.println("IO Exception: " + e.getMessage() );
	    throw new S2DError("Unable to read assignment file " +
	      filename);
	}
    }
    

    // Given a particular residue label, returns (by reference) total
    // Hydrogen, Carbon and Nitrogen assignments
    public AssgEntry returnAssg(String residueLabel) 
      throws S2DException
    {
        if (DEBUG >= 1) {
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
	    throw new S2DWarning("WARNING!!! " + residueLabel +
	      " entry  not found in assignment " +
	      "reference table file " + filename );
	}

	return totalAssignments;
    }
}

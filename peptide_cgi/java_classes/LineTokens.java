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

// Reads a chemical shift file for calculating delta shift info.

// ------------------------------------------------------------------------

// $Id$

// $Log$
// Revision 1.2  2000/07/27 16:11:23  wenger
// Added standard DEVise headers.
//

// ========================================================================

import java.io.*;
import java.text.ParseException;

public class LineTokens {
	private static final int DEBUG = 0;

	public int seqNumber;
	public int csi;
	private StreamTokenizer st;


	LineTokens (StreamTokenizer st) {
	    if (DEBUG >= 1) {
	        System.out.println("LineTokens()");
	    }

	    this.st = st;
	}


	public int readAndTokenLine()
	  throws Exception
	{
	    if (DEBUG >= 1) {
	        System.out.println("LineTokens.readAndTokenLine()");
	    }

	    try {
		if (st.nextToken() == st.TT_EOF)
		    return st.TT_EOF;
		else if (st.ttype != st.TT_NUMBER)
		    throw new ParseException(
			     "Expected number for sequence code, not string", 0);
		else
		    seqNumber = (int)st.nval;
		
		// compute consensus info here		
		if (st.nextToken() != st.TT_NUMBER)
		    throw new ParseException(
			     "Expected number for ha_deltashift, not string", 0);
		else		    
		    csi = (int)st.nval;
		
		if (st.nextToken() != st.TT_NUMBER)
		    throw new ParseException(
			     "Expected number for c_deltashift, not string", 0);
		else		    
		    csi -= (int)st.nval;

		if (st.nextToken() != st.TT_NUMBER)
		    throw new ParseException(
			     "Expected number for ca_deltashift, not string", 0);
		else		    
		    csi -= (int)st.nval;

		st.nextToken();	// skip cb_deltashift
		
		if (csi < 0)
		    csi = -1;
		else if (csi > 0)
		    csi = 1;
		else
		    csi = 0;
		

	    } catch (ParseException e) {
		System.err.println("Parse Exception: "
				   + e.getMessage() );
	        throw new Exception("Error reading or tokenizing line");
	    } catch (IOException e) {
		System.err.println("IO Exception: "
				   + e.getMessage() );
	        throw new Exception("Error reading or tokenizing line");
	    }
	    
	    return st.ttype;
	}
    }

// ========================================================================
// DEVise Data Visualization Software
// (c) Copyright 2001
// By the DEVise Development Group
// Madison, Wisconsin
// All Rights Reserved.
// ========================================================================

// Under no circumstances is this software to be copied, distributed,
// or altered in any way without prior permission from the DEVise
// Development Group.

// ------------------------------------------------------------------------

// Reads a delta shift file for calculating chemical shift info.

// Note: this class is a slightly-modified version of the old LineTokens
// class.

// ------------------------------------------------------------------------

// $Id$

// $Log$
// Revision 1.1.2.1  2001/02/22 19:28:30  wenger
// Fixed problem with reading back deltashift values in exponential format;
// error messages when checking for a protein are printed only if debug
// output is turned on; fixed problems with set_modes script.
//

// ========================================================================

import java.io.*;
import java.text.ParseException;

public class S2DDsReader {
    private static final int DEBUG = 0;

    public int seqNumber;
    public int csi;
    private StreamTokenizer st;

    private double nval;
    private String sval;
    private int ttype;

    S2DDsReader (StreamTokenizer st) {
        if (DEBUG >= 1) {
            System.out.println("S2DDsReader()");
        }

        this.st = st;
    }

    public int readAndTokenLine()
      throws S2DException
    {
        if (DEBUG >= 1) {
            System.out.println("S2DDsReader.readAndTokenLine()");
        }

        try {
            if (nextToken() == st.TT_EOF) {
                return st.TT_EOF;
            } else if (ttype != st.TT_NUMBER) {
                throw new ParseException(
                "Expected number for sequence code, not string " +
                "(" + sval + ") at line " + st.lineno(), 0);
            } else {
                seqNumber = (int)nval;
            }

            // compute consensus info here		
            if (nextToken() != st.TT_NUMBER) {
                throw new ParseException(
		  "Expected number for ha_deltashift, not string " +
                  "(" + sval + ") at line " + st.lineno(), 0);
            } else {
                csi = (int)nval;
            }

            if (nextToken() != st.TT_NUMBER) {
                throw new ParseException(
                  "Expected number for c_deltashift, not string " +
                  "(" + sval + ") at line " + st.lineno(), 0);
            } else {
                csi -= (int)nval;
            }

            if (nextToken() != st.TT_NUMBER) {
                throw new ParseException(
                  "Expected number for ca_deltashift, not string " +
                  "(" + sval + ") at line " + st.lineno(), 0);
            } else {
                csi -= (int)nval;
            }

            nextToken();	// skip cb_deltashift

            if (csi < 0) {
                csi = -1;
            } else if (csi > 0) {
                csi = 1;
            } else {
                csi = 0;
            }


        } catch (ParseException e) {
            System.err.println("Parse Exception: " + e.getMessage() );
            throw new S2DError("Error reading or tokenizing line");
        } catch (IOException e) {
            System.err.println("IO Exception: " + e.getMessage() );
            throw new S2DError("Error reading or tokenizing line");
        }

        return ttype;
    }

    // This method is here because the stupid StreamTokenizer class doesn't
    // understand exponential notation (e.g., 4.9E7).
    int nextToken() throws IOException
    {
	ttype = st.nextToken();
	nval = st.nval;
	sval = st.sval;

	if (ttype == st.TT_NUMBER) {
	    // Check for following exponent.
	    boolean hasExp = false;

	    if (st.nextToken() == st.TT_WORD) {
		if (st.sval.charAt(0) == 'e' || st.sval.charAt(0) == 'E') {
		    hasExp = true;
		}
	    }

	    if (hasExp) {
		// Calculate the new double value.
	        String tmpS = (new Double(nval)).toString() + st.sval;
		nval = (new Double(tmpS)).doubleValue();
	    } else {
	        st.pushBack();
	    }
	}

        return ttype;
    }
}

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

// Reads a star file via the web, generates DEVise-format summary info.

// ------------------------------------------------------------------------

// $Id$

// $Log$

// ========================================================================

public class Summarize
{

    /*
     * the only parameter is the accession file name, ie bmr4038.str
     * assume each name starts with bmr and ends with .str
     */
    public static void main(String [] args)
    {	
	if (args.length != 1)
	    {	
		System.err.println("usage: java Summarize <file name>");
		System.exit(1);
	    }

// 	String the_number = args[0].substring( 3, 7);

	WebStar2Devise starfile = 
	    new WebStar2Devise(args[0]);

	starfile.summarize("summary");
	
    } // end main


}

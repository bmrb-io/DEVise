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

// Gets backbone information from a local star file.

// ------------------------------------------------------------------------

// $Id$

// $Log$
// Revision 1.2  2000/07/27 16:11:23  wenger
// Added standard DEVise headers.
//

// ========================================================================

import EDU.bmrb.starlibj.*;

public class GetBackbone
{

    /*
     * the only parameter is the accession file name, ie bmr4038.str
     * assume each name starts with bmr and ends with .str
     */
    public static void main(String [] args)
    {	
	String file_name = null;
	String output_file = null;
        if (args.length == 0) {
            file_name = "DEVise_4096_3d.str";
	    output_file = "backbone.dat";

	} else if (args.length != 2)
            {   
                System.err.println("usage: java Backbone <source file>" +
				   " <output file>");
                System.exit(1);
            }
        else {
            file_name = args[0];
	    output_file = args[1];
	}

        Star2Devise starfile = Star2Devise.LocalStar2Devise(file_name);
        System.out.println(file_name + " parsed successfully.");

	//TEMP -- remove hard-coded 4096s
//  	if (starfile.calcChemShifts("4096"))
//  	    System.out.println
//  		("Chemical shift indices computed successfully.");

// 	if (starfile.summarize("4096"))
//  	    System.out.println
//  		("summary files computed successfully.");
	    
 	if (starfile.findBackbone("save_GMH4CO_average_refined_structure",
  				  output_file))
  	    System.out.println("CA backbone computed successfully.");

	if (starfile.countConstraints("4096"))
 	    System.out.println
 		("constraints counted successfully.");
	
    } // end main

}

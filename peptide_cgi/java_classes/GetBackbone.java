// Richard Chang
// 06/11/1999

// Processes NMR-Star Files for DEVise.

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

        LocalStar2Devise starfile = 
            new LocalStar2Devise(file_name);
        System.out.println(file_name + " parsed successfully.");

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

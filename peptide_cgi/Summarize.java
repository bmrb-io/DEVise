// Richard Chang
// 06/11/1999

// Processes NMR-Star Files for DEVise.

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

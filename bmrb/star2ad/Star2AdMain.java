// ========================================================================
// DEVise Data Visualization Software
// (c) Copyright 2002
// By the DEVise Development Group 
// Madison, Wisconsin
// All Rights Reserved. 
// ========================================================================

// Under no circumstances is this software to be copied, distributed,
// or altered in any way without prior permission from the DEVise 
// Development Group.

// ------------------------------------------------------------------------

// This class is an application used to test translation.
// It parse input file generating a StarNode and then it
// starts translation using Star2AdTranslator class.

// ------------------------------------------------------------------------

// $Id$

// 2002/09/10 Patrícia Kayser - kayser@cos.ufrj.br
// Third version: it allows more than one column name on "-c" option.
// Two more options were include: "-al" and "-nl" to translate, respectively
// all and no loops. See usage string for more details.
// example: -al -c save_entry_information:_Revision_date 
//          -c save_chemical_shift_assignment_data_set_one:_Atom_name,_Atom_type

// 2002/09/05 Patrícia Kayser - kayser@cos.ufrj.br
// Second version: now it translate only selected loop inside a save frame
// using "-c" option. It can use only saveframe:columnName for each "-c".
// Input/output filename can also be informed by command line using "-f".
// example of parameters: -f bmr1133 -c save_entry_information:_Revision_date
// Actually, for Star2Main it does not matter how constraints are expressed.
// Only restriction: "-f" parameter must be a pair separed by ':' char

// 2002/08/28  Patrícia Kayser - kayser@cos.ufrj.br
// First version: translates all input file without allowing constraints
// about loops. 
// Missing information to user about usage: assumes a default input file.

// ------------------------------------------------------------------------

package bmrb.star2ad;

import EDU.bmrb.starlibj.*;
import java.util.*;
import java.io.*;
import java.net.*;

public class Star2AdMain {
    //===================================================================
    // VARIABLES
    //static String   inputFile  = "C:\\kayser\\madison\\condor-biostat\\starlibj\\bmrb\\star2ad\\new1133.str";
    //static String   outputFile = "C:\\kayser\\madison\\condor-biostat\\starlibj\\bmrb\\star2ad\\new1133.ads";
    static String   inputFile = "";
    static String   outputFile = "star.ads";
    static Star2AdConstraintManager constraintMng;

    //===================================================================
    // PUBLIC METHODS
    //-------------------------------------------------------------------
    void printHelp() {
    	String helpMsg = "";
	System.out.println(helpMsg);
    }

    //-------------------------------------------------------------------
    void printUsage() {
	String classeName = "bmrb.star2ad.Star2AdMain"; 
	String usageDetails = 
	    "-f <filename>: input file (can be a URL).\n" +
	    "-o <filename>: output file (default is \"star.ads\").\n" +
	    "-c constraints constraints is one or more expressions in the form "+
            "save_frame_name:expression. An expression can be one or more "+
            "loop column names.\n"+
            "-nl: no loops are translated (default). When used with -c option, "+
            "only the specified loops are translated.\n"+
            "-al: all loops are translated. When used with -c options, "+
            "save frames without constraints have all loops translated "+
            "while save frames with constraints have only specified loops translated.\n"+
	    "-help: display more information about this program.\n" +
	    "-usage: print this message\n";
            // It can also be any expression envolving loop column name, the operator 
            // <, >, <=, >= or !=, and a numeric constant
	
	System.out.println("USAGE:");
	System.out.println("   java "+classeName+" -f filename [-c constraints] [-nl | -al] [-help] [-usage]");
	System.out.println("\n"+usageDetails);
    }
    //-------------------------------------------------------------------
    public  void getParameters(String args[]) {
        constraintMng = new Star2AdConstraintManager();
        constraintMng.setNL();
        
        int i=0;
        try {
            while (i < args.length) {
                if (args[i].equals("-f")){
                    i++;
                    inputFile  = args[i];
                } else if (args[i].equals("-o")) {
                    i++;
                    outputFile = args[i];
                } else if (args[i].equals("-c")) {
                    i++;
                    // e.g. entry_information:_Revision_date
                    StringTokenizer st = new StringTokenizer(args[i],":");
                    String saveFrameName=st.nextToken(); 
                    String constraint=st.nextToken();
                    constraintMng.setConstraint(saveFrameName, constraint);
                } else if (args[i].equals("-al")) {
                    constraintMng.setAL();
                } else if (args[i].equals("-nl")) {
                    constraintMng.setNL();
                } else if (args[i].equals("-help")) {
                    printHelp();
		    System.exit(0);
                } else if (args[i].equals("-usage")) {
		    printUsage();
		    System.exit(0);
                } else {
                    throw(new Exception());
                }
                i++;
            }
        } catch (Exception e) {
            printUsage();
            System.exit(1);
        }

	if (inputFile.equals("")) {
	   printUsage();
           System.exit(1);
	}
    }    

    //-------------------------------------------------------------------
    /**
     * Parsing is done in three steps: <BR>
     * Step 1. Make a new instance of StarParser class, 
     *   giving the constructor a reference to the stream 
     *   to be parsed. <BR>
     * Step 2. Call the StarFileNodeParse() method to 
     *    parse in a complete Star File <BR>
     * Step 3. Call that object's popResult() method to see 
     *    the final result of the parse (a StarFileNode).
     *    This method will return the object that was parsed. 
     *    The object parsed is the last thing left on the stack.
     *    This method will also pop the object off the stack, 
     *    so only the first call to this method after
     *    a parse will work properly.
     */
    public StarNode parseFile(String fileName) {
       StarParser   aParser = null;
       StarNode aStarTree = null;

       // Step 1
       try {
	   InputStream is;
	   if (fileName.startsWith("http") || fileName.startsWith("ftp")) {
	       is = (new URL(fileName)).openStream();
	   } else {
	       is = new java.io.FileInputStream(fileName);
	   }
           aParser = new StarParser(is);
       } catch (Exception e) {
           System.out.println("Error creating StarParser with"+
			      " input stream "+fileName);
           System.out.println(e);
	   System.exit(1);
       }

       // for testing purposes - setVerbose: "Call this method 
       // before parsing to turn on some debugging printing. 
       // Mostly this is for internal debugging."
       //
       //       aParser.setVerbose(3);

       // Step 2
       try {
          aParser.StarFileNodeParse(aParser);
	     // 'aParser' needs to be sent as a parameter because
	     // javacc makes static methods that have no 'this'.
       } catch (Exception e) {
	   System.out.println("Error parsing with StarFileNodeParse"+
			      " method");
	   System.exit(1);
       }

       // Step 3
       try {
          aStarTree = aParser.popResult();
       } catch (Exception e) {
          System.out.println("Error trying to create StarParser");
       }

       return aStarTree;
    }

    //-------------------------------------------------------------------
    public static void main(String args[]) {
       StarNode aStarTree;
       Star2AdTranslator translator;

       Star2AdMain s = new Star2AdMain();

       s.getParameters(args);

       System.out.println("<NMR-STAR to ClassAds Translator>");

       aStarTree = s.parseFile(inputFile);
       translator = new Star2AdTranslator(aStarTree, outputFile);
       translator.setConstraintMng(constraintMng);
       translator.start();

       System.out.println("\n<NMR-STAR2ClassAd>\tTranslation done."); 
       System.exit(0);
    }
 
}

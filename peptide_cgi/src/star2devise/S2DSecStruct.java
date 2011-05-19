// ========================================================================
// DEVise Data Visualization Software
// (c) Copyright 2011
// By the DEVise Development Group
// Madison, Wisconsin
// All Rights Reserved.
// ========================================================================

// Under no circumstances is this software to be copied, distributed,
// or altered in any way without prior permission from the DEVise
// Development Group.

// ------------------------------------------------------------------------

// This cl
// This class implements the output of secondary structure information.

// ------------------------------------------------------------------------

// $Id$

// $Log$
// Revision 1.1.2.1  2011/04/06 16:05:37  wenger
// Added creation of an empty secondary structure file, and modified the
// s2predict template to use the correct secondary structure data source.
//

// ========================================================================

package star2devise;

import java.io.*;
import java.util.*;
import java.net.*;

public class S2DSecStruct {
    //===================================================================
    // VARIABLES

    private static final int DEBUG = 11;//TEMPTEMP 0;

    private String _name;
    private String _longName;
    private int _coordIndex;
    private String _dataDir;

    //===================================================================
    // PUBLIC METHODS

    //-------------------------------------------------------------------
    // Constructor.
    public S2DSecStruct(String name, String longName, int coordIndex,
      String dataDir, String entityAssemblyId) throws S2DException
    {
        if (doDebugOutput(11)) {
	    System.out.println("S2DSecStruct.S2DSecStruct(" + name +
	      ", " + coordIndex + ")");
	}

	_name = name;
	_longName = longName;
	_coordIndex = coordIndex;
	_dataDir = dataDir;

        //TEMPTEMP -- get values here
    }

    //-------------------------------------------------------------------
    // Write the S2 order values for this data.TEMPTEMP
    public void writeSecStruct() throws S2DException
    {
        if (doDebugOutput(11)) {
	    System.out.println("S2DSecStruct.writeSecStruct()");
	}

	try {
	    //
	    // Write the secondary structure values to the data
	    // file.
	    //
	    FileWriter writer = S2DFileWriter.create(_dataDir +
	      File.separator + _name + "ss" + _coordIndex +
	      S2DNames.DAT_SUFFIX);//TEMPTEMP

	    writer.write("# Data: s2secstruct values for " + _name + "\n");
	    writer.write("# Schema: bmrb-sec_struct.new\n");//TEMPTEMP
	    writer.write("# Attributes: " +
	      //TEMPTEPM? "Entity_assembly_ID; " +
	      "ResSeqId1; ResSeqId2; StructType; Color\n");
	    writer.write("# Peptide-CGI version: " +
	      S2DMain.PEP_CGI_VERSION + "\n");
	    writer.write("# Generation date: " +
	      S2DMain.getTimestamp() + "\n");
	    writer.write("#\n");

	    writer.close();

	} catch(IOException ex) {
	    System.err.println("IOException writing s2secstruct data: " +
	      ex.toString());
	    throw new S2DError("Can't write s2secstruct data");
	}

    }

    //===================================================================
    // PRIVATE METHODS

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

// ========================================================================

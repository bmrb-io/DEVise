// ========================================================================
// DEVise Data Visualization Software
// (c) Copyright 2004-2007
// By the DEVise Development Group
// Madison, Wisconsin
// All Rights Reserved.
// ========================================================================

// Under no circumstances is this software to be copied, distributed,
// or altered in any way without prior permission from the DEVise
// Development Group.

// ------------------------------------------------------------------------

// This class is used to simulate the calling of s2d in Jafar, where
// S2DMain.main() is called repeatedly without the class being reloaded.

// ------------------------------------------------------------------------

// $Id$

// $Log$
// Revision 1.2  2006/02/01 20:23:11  wenger
// Merged V2_1b4_br_0 thru peptide_cgi_10_8_0_base to the
// trunk.
//
// Revision 1.1.2.1  2004/03/04 16:44:14  wenger
// Added Jafar simulation to help with tracking down Jafar-
// specific problems; fixes to install script; minor improvements
// to s2d script.
//

// ========================================================================

package star2devise;

import java.io.*;
import java.util.*;

public class S2DJafarSim
{
    private static final int DEBUG = 0;

    //===================================================================
    // PUBLIC METHODS

    //-------------------------------------------------------------------
    public static void main(String args[]) throws S2DException
    {
        Vector myArgs = null;

        // Replicate test2
        myArgs = new Vector();
        setFixedArgs(myArgs);
        myArgs.addElement("-file");
        myArgs.addElement("bmr4267.str");
        myArgs.addElement("-name");
        myArgs.addElement("test2_out");
        myArgs.addElement("-force");

        runS2D(myArgs);

        try {
            Thread.sleep(2 * 60 * 1000);
        } catch(InterruptedException ex) {
            System.out.println("Sleep was interrupted: " + ex);
        }

        // Replicate test13
        myArgs = new Vector();
        setFixedArgs(myArgs);
        myArgs.addElement("-file");
        myArgs.addElement("Jafar1.str");
        myArgs.addElement("-name");
        myArgs.addElement("test13_out");
        myArgs.addElement("-do_csr");
        myArgs.addElement("2");
        myArgs.addElement("-force");

        runS2D(myArgs);
    }

    //===================================================================
    // PRIVATE METHODS

    //-------------------------------------------------------------------
    private static void setFixedArgs(Vector myArgs)
    {
        // Fixed arguments (from vsc script).
        myArgs.addElement("-data_dir");
        myArgs.addElement("data_dir/vis_server");
        myArgs.addElement("-js_data_dir");
        myArgs.addElement("bmrb/dynamic_data/vis_server");
        myArgs.addElement("-session_dir");
        myArgs.addElement("session_dir/vis_server");
        myArgs.addElement("-html_dir");
        myArgs.addElement("html_dir");
        myArgs.addElement("-conn_file");
        myArgs.addElement("chem_info/connections");
        myArgs.addElement("-no_protein_check");
        myArgs.addElement("-do_pdb");
        myArgs.addElement("2");
    }

    //-------------------------------------------------------------------
    private static void runS2D(Vector myArgs) throws S2DException
    {
        String [] args = new String[myArgs.size()];
        for (int index = 0; index < myArgs.size(); ++index) {
            args[index] = (String)myArgs.elementAt(index);
        }

        S2DMain.main(args);
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

// ========================================================================

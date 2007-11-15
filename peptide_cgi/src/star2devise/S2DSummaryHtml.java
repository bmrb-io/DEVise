// ========================================================================
// DEVise Data Visualization Software
// (c) Copyright 2000-2007
// By the DEVise Development Group
// Madison, Wisconsin
// All Rights Reserved.
// ========================================================================

// Under no circumstances is this software to be copied, distributed,
// or altered in any way without prior permission from the DEVise
// Development Group.

// ------------------------------------------------------------------------

// This class is used to generate the summary html files used to access
// NMR-Star data with the JavaScreen.  The summary html files contain
// links to the individual html files for the data that's available for
// a given entry.

// Most of the code that used to be in here got moved to S2DSummaryGen
// to make it easier to generate two summary files (one for the "normal"
// visualizations, one for the large visualizations).

// ------------------------------------------------------------------------

// $Id$

// $Log$
// Revision 1.7  2007/08/20 20:26:10  wenger
// Added -verb command-line flag and property so we can turn on debug
// output without recompiling; added debug_level property corresponding
// to the existing -debug command-line flag.
//
// Revision 1.6  2007/03/07 16:37:58  wenger
// Phase 2 of "upload and visualize data" -- mostly working, I think,
// but still needs chem shift reference capability and hasn't been
// hooked up to Dimitri's upload scripts yet.
//
// Revision 1.5  2006/08/21 21:01:11  wenger
// Added second summary page for direct access to all large-size
// visualizations; updated all tests accordingly.
//
// Revision 1.4  2006/05/11 21:10:25  wenger
// Fixed problems with some html page titles being based on the BMRB
// ID rather than the name, etc.  Fixed test36 to work with new LACS
// file names.
//
// ...

// ========================================================================

package star2devise;

import java.io.*;
import java.util.*;

public class S2DSummaryHtml {
    //===================================================================
    // VARIABLES

    private static final int DEBUG = 0;

    public static final String VERSION_LABEL = "PepCgi_version";
    public static final String GEN_DATE_LABEL = "Generation_date";
    public static final String BMRB_ID_LABEL = "Related_BMRB_ID";
    public static final String PDB_ID_LABEL = "Related_PDB_ID";

    private String _htmlDir = null;

    S2DSummaryHtmlNormal _normal;
    S2DSummaryHtmlLarge _large;

    // This is set to true if any link is written to the summary html file;
    // if we don't write any links, a note is inserted that no data is
    // available.
    private boolean _wroteLink = false;

    //===================================================================
    // PUBLIC METHODS

    //-------------------------------------------------------------------
    public static String fileName(String htmlDir, String name)
    {
	return fileName(htmlDir, name, "");
    }

    //-------------------------------------------------------------------
    public static String fileName(String htmlDir, String name,
      String sizeString)
    {
	return htmlDir + File.separator + fileNameShort(name, sizeString);
    }

    //-------------------------------------------------------------------
    public static String fileNameShort(String name, String sizeString)
    {
	return name + S2DNames.SUMMARY_HTML_SUFFIX + sizeString +
	  S2DNames.HTML_SUFFIX;
    }

    //-------------------------------------------------------------------
    public String getHtmlDir()
    {
        return _htmlDir;
    }

    //-------------------------------------------------------------------
    // Constructor.  Opens the html file and writes the header.
    public S2DSummaryHtml(String name, String longName, String accessionNum,
      Vector localFiles, String htmlDir, String starFileName,
      String systemName, String frameTitle) throws S2DException
    {
        if (doDebugOutput(11)) {
	    System.out.println("S2DSummaryHtml.S2DSummaryHtml(" +
	      name + ", " + accessionNum + ")");
	}

	_htmlDir = htmlDir;

	_normal = new S2DSummaryHtmlNormal(name, longName, accessionNum,
	  localFiles, htmlDir, starFileName, systemName, frameTitle);
	_large = new S2DSummaryHtmlLarge(name, longName, accessionNum,
	  localFiles, htmlDir, starFileName, systemName, frameTitle);

	_normal.setSibling(_large);
	_large.setSibling(_normal);

	_normal.initialize(systemName, frameTitle);
	_large.initialize(systemName, frameTitle);
    }

    //-------------------------------------------------------------------
    // Finalizer.
    protected void finalize() throws S2DException
    {
        if (doDebugOutput(11)) {
	    System.out.println("S2DSummaryHtml.finalize()");
	}

        close(null, null);
    }

    //-------------------------------------------------------------------
    // Writes out the tail of the html and closes the file.
    // Now saves related BMRB and PDB IDs in this file, so we can check
    // whether related files have changed without parsing the main
    // BMRB file.
    public void close(Vector bmrbIds, Vector pdbIds) throws S2DException
    {
        if (doDebugOutput(11)) {
	    System.out.println("S2DSummaryHtml.close()");
	}

	_normal.close(bmrbIds, pdbIds);
	_large.close(bmrbIds, pdbIds);
    }

    //-------------------------------------------------------------------
    // Writes the info for the start of a save frame.
    public void startFrame(String frameDetails) throws S2DException
    {
        if (doDebugOutput(11)) {
	    System.out.println("S2DSummaryHtml.startFrame(" +
	      frameDetails + ")");
	}

    	_normal.startFrame(frameDetails);
    	_large.startFrame(frameDetails);
    }

    //-------------------------------------------------------------------
    // Writes the info for the end of a save frame.
    public void endFrame() throws S2DException
    {
        if (doDebugOutput(11)) {
	    System.out.println("S2DSummaryHtml.endFrame()");
	}

    	_normal.endFrame();
    	_large.endFrame();
    }

    //-------------------------------------------------------------------
    // Writes the deltashift link.
    public void writeDeltashift(int frameIndex, int count) throws IOException
    {
        if (doDebugOutput(12)) {
	    System.out.println("S2DSummaryHtml.writeDeltashift()");
	}

    	_normal.writeDeltashift(frameIndex, count);
    	_large.writeDeltashift(frameIndex, count);
    }

    //-------------------------------------------------------------------
    // Writes the CSI link.
    public void writeCSI(int frameIndex, int count) throws IOException
    {
        if (doDebugOutput(12)) {
	    System.out.println("S2DSummaryHtml.writeCSI()");
	}

    	_normal.writeCSI(frameIndex, count);
    	_large.writeCSI(frameIndex, count);
    }

    //-------------------------------------------------------------------
    // Writes the percent assignment link.
    public void writePctAssign(int frameIndex, int count) throws IOException
    {
        if (doDebugOutput(12)) {
	    System.out.println("S2DSummaryHtml.writePctAssign()");
	}

    	_normal.writePctAssign(frameIndex, count);
    	_large.writePctAssign(frameIndex, count);
    }

    //-------------------------------------------------------------------
    // Writes the coupling constant link.
    public void writeCoupling(int frameIndex, int count) throws IOException
    {
        if (doDebugOutput(12)) {
	    System.out.println("S2DSummaryHtml.writeCoupling()");
	}

	_normal.writeCoupling(frameIndex, count);
	_large.writeCoupling(frameIndex, count);
    }

    //-------------------------------------------------------------------
    // Writes the relaxation link.
    public void writeRelax(String suffix, String name, int frameIndex,
      int count) throws IOException
    {
        if (doDebugOutput(12)) {
	    System.out.println("S2DSummaryHtml.writeRelax()");
	}

	_normal.writeRelax(suffix, name, frameIndex, count);
	_large.writeRelax(suffix, name, frameIndex, count);
    }

    //-------------------------------------------------------------------
    // Writes the heteronuclear NOE link.
    public void writeHetNOE(String name, int frameIndex, int count)
      throws IOException
    {
        if (doDebugOutput(12)) {
	    System.out.println("S2DSummaryHtml.writeHetNOE()");
	}

	_normal.writeHetNOE(name, frameIndex, count);
	_large.writeHetNOE(name, frameIndex, count);
    }

    //-------------------------------------------------------------------
    // Writes the all chemical shifts link.
    public void writeAllShifts(int frameIndex, int count)
      throws IOException
    {
        if (doDebugOutput(12)) {
	    System.out.println("S2DSummaryHtml.writeAllShifts()");
	}

	_normal.writeAllShifts(frameIndex, count);
	_large.writeAllShifts(frameIndex, count);
    }

    //-------------------------------------------------------------------
    // Writes the H vs. N chemical shifts link.
    public void writeHvsNShifts(int frameIndex, int count)
      throws IOException
    {
        if (doDebugOutput(12)) {
	    System.out.println("S2DSummaryHtml.writeHvsNShifts()");
	}

	_normal.writeHvsNShifts(frameIndex, count);
	_large.writeHvsNShifts(frameIndex, count);
    }

    //-------------------------------------------------------------------
    // Writes the atomic coordinates link.
    public void writeAtomicCoords(String pdbId, int frameIndex,
      int resCount, int atomCount) throws IOException
    {
        if (doDebugOutput(12)) {
	    System.out.println("S2DSummaryHtml.writeAtomicCoords()");
	}

	_normal.writeAtomicCoords(pdbId, frameIndex, resCount, atomCount);
	_large.writeAtomicCoords(pdbId, frameIndex, resCount, atomCount);
    }

    //-------------------------------------------------------------------
    // Writes the atomic coordinates link, where the link is a CGI script
    // invocation (we haven't already processed the PDB entry with
    // the atomic coordinates).
    public void writeAtomicCoordsCGI(String pdbId, int frameIndex)
      throws IOException
    {
        if (doDebugOutput(12)) {
	    System.out.println("S2DSummaryHtml.writeAtomicCoordsCGI()");
	}

	_normal.writeAtomicCoordsCGI(pdbId, frameIndex);
	_large.writeAtomicCoordsCGI(pdbId, frameIndex);
    }

    //-------------------------------------------------------------------
    public void writeTooManyAtoms(int atomCount, int maxAtoms)
      throws IOException
    {
	_normal.writeTooManyAtoms(atomCount, maxAtoms);
	_large.writeTooManyAtoms(atomCount, maxAtoms);
    }

    //-------------------------------------------------------------------
    // Writes the chem shift reference link.
    public void writeChemShiftRef(int frameIndex, boolean fullNames)
      throws IOException
    {
        if (doDebugOutput(12)) {
	    System.out.println("S2DSummaryHtml.writeChemShiftRef()");
	}

	_normal.writeChemShiftRef(frameIndex, fullNames);
	_large.writeChemShiftRef(frameIndex, fullNames);
    }

    //-------------------------------------------------------------------
    // Writes the chem shift reference link, where the link is a CGI script
    // invocation (we haven't already done the chem shift reference
    // processing).
    public void writeChemShiftRefCGI(String pdbId, int frameIndex)
      throws IOException
    {
        if (doDebugOutput(12)) {
	    System.out.println("S2DSummaryHtml.writeChemShiftRefCGI()");
	}

	_normal.writeChemShiftRefCGI(pdbId, frameIndex);
	_large.writeChemShiftRefCGI(pdbId, frameIndex);
    }

    //-------------------------------------------------------------------
    // Writes the Pistachio link.
    public void writePistachio(int frameIndex) throws IOException
    {
        if (doDebugOutput(12)) {
	    System.out.println("S2DSummaryHtml.writePistachio()");
	}

	_normal.writePistachio(frameIndex);
	_large.writePistachio(frameIndex);
    }

    //-------------------------------------------------------------------
    // Writes the ambiguity link.
    public void writeAmbiguity(int frameIndex) throws IOException
    {
        if (doDebugOutput(12)) {
	    System.out.println("S2DSummaryHtml.writeAmbiguity()");
	}

	_normal.writeAmbiguity(frameIndex);
	_large.writeAmbiguity(frameIndex);
    }

    //-------------------------------------------------------------------
    // Writes the LACS link.
    public void writeLACS(String title, int frameIndex)
      throws IOException
    {
        if (doDebugOutput(12)) {
	    System.out.println("S2DSummaryHtml.writeLACS()");
	}

	_normal.writeLACS(title, frameIndex);
	_large.writeLACS(title, frameIndex);
    }

    //-------------------------------------------------------------------
    // Write a message to the summary html file.
    public void writeMessage(String msg, boolean horRule)
    {
	_normal.writeMessage(msg, horRule);
	_large.writeMessage(msg, horRule);
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

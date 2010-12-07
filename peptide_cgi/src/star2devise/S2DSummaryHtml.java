// ========================================================================
// DEVise Data Visualization Software
// (c) Copyright 2000-2010
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

// Each object of this class contains an instance of S2DSummaryHtmlNormal
// and an instance of S2DSummaryHtmlLarge.  Basically, most calls to an
// object of this class get passed on to the corresponding methods of
// the S2DSummaryHtmlNormal and S2DSummaryHtmlLarge objects.

// ------------------------------------------------------------------------

// $Id$

// $Log$
// Revision 1.25  2010/07/07 20:54:13  wenger
// Changed Peptide-CGI to work with new JavaScreen re-sizing feature
// (since the user can now re-size the JS, we don't generate html
// pages for different sizes of visualization; this also includes
// eliminating the different-size pages for the histograms).
//
// Revision 1.24  2010/06/07 16:49:11  wenger
// We now delete html files for an entry when the session files are
// deleted (so you don't accidentally end up with an html file for,
// say, coordinates, for which no session exists); this involved moving
// all of the html files into a separate directory for each entry,
// so they can be easily deleted.
//
// Revision 1.23  2010/04/23 16:51:08  wenger
// Merged s2d_dist_rest_1002_br_0 thru s2d_dist_rest_1002_br_1 to trunk.
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

    private String _htmlDir = null;

    S2DSummaryHtmlNormal _normal;

    // This is set to true if any link is written to the summary html file;
    // if we don't write any links, a note is inserted that no data is
    // available.
    private boolean _wroteLink = false;

    //===================================================================
    // PUBLIC METHODS

    //-------------------------------------------------------------------
    public static String directory(String htmlDir, String name)
    {
    	return htmlDir + File.separator + name;
    }

    //-------------------------------------------------------------------
    public static String fileName(String htmlDir, String name)
    {
	return fileName(htmlDir, name, "");
    }

    //-------------------------------------------------------------------
    public static String fileName(String htmlDir, String name,
      String sizeString)
    {
	return directory(htmlDir, name) + File.separator +
	  fileNameShort(name, sizeString);
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
    public S2DSummaryHtml(String name, String longName, String masterId,
      Vector localFiles, String htmlDir, String starFileName,
      String systemName, String frameTitle, boolean restraintOnly)
      throws S2DException
    {
        if (doDebugOutput(11)) {
	    System.out.println("S2DSummaryHtml.S2DSummaryHtml(" +
	      name + ", " + masterId + ")");
	}

	_htmlDir = htmlDir;

	_normal = new S2DSummaryHtmlNormal(name, longName, masterId,
	  localFiles, htmlDir, restraintOnly);

	_normal.initialize(systemName, frameTitle);
    }

    //-------------------------------------------------------------------
    // Finalizer.
    protected void finalize() throws S2DException
    {
        if (doDebugOutput(11)) {
	    System.out.println("S2DSummaryHtml.finalize()");
	}

        close(null, null, "");
    }

    //-------------------------------------------------------------------
    // Writes out the tail of the html and closes the file.
    // Now saves related BMRB and PDB IDs in this file, so we can check
    // whether related files have changed without parsing the main
    // BMRB file.
    public void close(Vector bmrbIds, Vector pdbIds, String starVersion)
      throws S2DException
    {
        if (doDebugOutput(11)) {
	    System.out.println("S2DSummaryHtml.close()");
	}

	_normal.close(bmrbIds, pdbIds, starVersion);
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
    }

    //-------------------------------------------------------------------
    // Writes the info for the end of a save frame.
    public void endFrame() throws S2DException
    {
        if (doDebugOutput(11)) {
	    System.out.println("S2DSummaryHtml.endFrame()");
	}

    	_normal.endFrame();
    }

    //-------------------------------------------------------------------
    // Writes the deltashift link.
    public void writeDeltashift(int frameIndex, int entityAssemblyID,
      int count, boolean isNucleicAcid) throws IOException
    {
        if (doDebugOutput(12)) {
	    System.out.println("S2DSummaryHtml.writeDeltashift()");
	}

    	_normal.writeDeltashift(frameIndex, entityAssemblyID, count,
	  isNucleicAcid);
    }

    //-------------------------------------------------------------------
    // Writes the SPARTA-calculated deltashift link.
    public void writeSpartaDeltashift(int entityAssemblyID,
      int frameIndex, int count) throws IOException
    {
	if (doDebugOutput(12)) {
	    System.out.println("S2DSummaryHtml.writeSpartaDeltashift()");
	}

    	_normal.writeSpartaDeltashift(entityAssemblyID, frameIndex,
	  count);
    }

    //-------------------------------------------------------------------
    // Writes the CSI link.
    public void writeCSI(int frameIndex, int count) throws IOException
    {
        if (doDebugOutput(12)) {
	    System.out.println("S2DSummaryHtml.writeCSI()");
	}

    	_normal.writeCSI(frameIndex, count);
    }

    //-------------------------------------------------------------------
    // Writes the percent assignment link.
    public void writePctAssign(int frameIndex, int count) throws IOException
    {
        if (doDebugOutput(12)) {
	    System.out.println("S2DSummaryHtml.writePctAssign()");
	}

    	_normal.writePctAssign(frameIndex, count);
    }

    //-------------------------------------------------------------------
    // Writes the coupling constant link.
    public void writeCoupling(int frameIndex, int entityAssemblyID,
      int count) throws IOException
    {
        if (doDebugOutput(12)) {
	    System.out.println("S2DSummaryHtml.writeCoupling()");
	}

	_normal.writeCoupling(frameIndex, entityAssemblyID, count);
    }

    //-------------------------------------------------------------------
    // Writes the relaxation link.
    public void writeRelax(int dataType, int frequency, String suffix,
      String name, int frameIndex, int entityAssemblyID, int count)
      throws IOException
    {
        if (doDebugOutput(12)) {
	    System.out.println("S2DSummaryHtml.writeRelax()");
	}

	_normal.writeRelax(dataType, frequency, suffix, name,
	  frameIndex, entityAssemblyID, count);
    }

    //-------------------------------------------------------------------
    // Writes the heteronuclear NOE link.
    public void writeHetNOE(String name, int frameIndex,
      int entityAssemblyID, int count) throws IOException
    {
        if (doDebugOutput(12)) {
	    System.out.println("S2DSummaryHtml.writeHetNOE()");
	}

	_normal.writeHetNOE(name, frameIndex, entityAssemblyID, count);
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
    }

    //-------------------------------------------------------------------
    // Writes the H vs. C chemical shifts link.
    public void writeHvsCShifts(int frameIndex, int count)
      throws IOException
    {
        if (doDebugOutput(12)) {
	    System.out.println("S2DSummaryHtml.writeHvsCShifts()");
	}

	_normal.writeHvsCShifts(frameIndex, count);
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
    }

    //-------------------------------------------------------------------
    public void writeTooManyAtoms(int atomCount, int maxAtoms)
      throws IOException
    {
	_normal.writeTooManyAtoms(atomCount, maxAtoms);
    }

    //-------------------------------------------------------------------
    // Writes the chem shift reference link.
    public void writeChemShiftRef(String pdbId, int frameIndex)
      throws IOException
    {
        if (doDebugOutput(12)) {
	    System.out.println("S2DSummaryHtml.writeChemShiftRef()");
	}

	_normal.writeChemShiftRef(pdbId, frameIndex);
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
    }

    //-------------------------------------------------------------------
    // Writes the Pistachio link.
    public void writePistachio(int frameIndex) throws IOException
    {
        if (doDebugOutput(12)) {
	    System.out.println("S2DSummaryHtml.writePistachio()");
	}

	_normal.writePistachio(frameIndex);
    }

    //-------------------------------------------------------------------
    // Writes the ambiguity link.
    public void writeAmbiguity(int frameIndex) throws IOException
    {
        if (doDebugOutput(12)) {
	    System.out.println("S2DSummaryHtml.writeAmbiguity()");
	}

	_normal.writeAmbiguity(frameIndex);
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
    }

    //-------------------------------------------------------------------
    // Writes the S2 order parameter link.
    public void writeS2Order(int frameIndex, int entityAssemblyID,
      int count) throws IOException
    {
        if (doDebugOutput(12)) {
	    System.out.println("S2DSummaryHtml.writeS2Order()");
	}

	_normal.writeS2Order(frameIndex, entityAssemblyID, count);
    }

    //-------------------------------------------------------------------
    // Writes the torsion angle link, where we've actually done the
    // torsion angle processing.
    public void writeTorsionAngle(String pdbId, int frameIndex,
      boolean isRR) throws IOException
    {
        if (doDebugOutput(12)) {
	    System.out.println("S2DSummaryHtml.writeTorsionAngle()");
	}

	_normal.writeTorsionAngle(pdbId, frameIndex, isRR);
    }

    //-------------------------------------------------------------------
    // Writes the torsion angle link, where the link is a CGI script
    // invocation (we haven't already done the torsion angle processing).
    public void writeTorsionAngleCGI(String pdbId, String tarUrl,
      int frameIndex, boolean isRR) throws IOException
    {
        if (doDebugOutput(12)) {
	    System.out.println("S2DSummaryHtml.writeTorsionAngleCGI()");
	}

	_normal.writeTorsionAngleCGI(pdbId, tarUrl, frameIndex, isRR);
    }

    //-------------------------------------------------------------------
    // Writes the distance restraint link, where we've actually done the
    // distance restraint processing.
    public void writeDistRestraint(String pdbId, int frameIndex,
      boolean isRR) throws IOException
    {
        if (doDebugOutput(12)) {
	    System.out.println("S2DSummaryHtml.writeDistRestraint()");
	}

	_normal.writeDistRestraint(pdbId, frameIndex, isRR);
    }

    //-------------------------------------------------------------------
    // Writes the distance restraint link, where the link is a CGI script
    // invocation (we haven't already done the distance restraint
    // processing).
    public void writeDistRestraintCGI(String pdbId, String tarUrl,
      int frameIndex, boolean isRR) throws IOException
    {
        if (doDebugOutput(12)) {
	    System.out.println("S2DSummaryHtml.writeDistRestraintCGI()");
	}

	_normal.writeDistRestraintCGI(pdbId, tarUrl, frameIndex, isRR);
    }

    //-------------------------------------------------------------------
    // Write a message to the summary html file.
    public void writeMessage(String msg, boolean horRule)
    {
	_normal.writeMessage(msg, horRule);
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

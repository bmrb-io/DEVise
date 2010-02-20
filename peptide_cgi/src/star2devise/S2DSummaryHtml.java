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
// Revision 1.20  2010/02/11 22:13:11  wenger
// Merged s2d_remediated_rest_1002_br_0 thru s2d_remediated_rest_1002_br_1
// to trunk (note: s2d_remediated_rest_1002_br_1 ==
// s2d_remediated_rest_1002_br_end).
//
// Revision 1.19.2.1  2010/02/04 00:08:27  wenger
// Now writing remediated restraint links in summary page -- CGI links
// don't seem to work right yet.
//
// Revision 1.19  2010/01/21 16:32:15  wenger
// Merged s2d_pdb_only_tar_1001_br_0 thru s2d_pdb_only_tar_1001_br_end
// to trunk.
//
// Revision 1.18.2.2  2010/01/08 23:28:32  wenger
// Fixed 'Force reprocessing' functionality for restraint-only summary
// page; fixed error message in make_view for restraint-only processing.
//
// Revision 1.18.2.1  2010/01/08 16:29:30  wenger
// Added changes to make the summary html file have only the relevant
// things for restraint-only processing.
//
// Revision 1.18  2010/01/06 23:03:40  wenger
// Merged s2d_dist_rest_0912_br_0 thru s2d_dist_rest_0912_br_1 to trunk.
//
// Revision 1.17.2.1  2009/12/16 00:07:56  wenger
// Added S2DDistRestraint and S2DNmrStarDistRIfc classes (mostly
// stubs so far); added a bunch of notes based on today's BMRB
// staff meeting discussions; also added stubbed-in classes in
// S2DSummaryHtml, etc.
//
// Revision 1.17  2009/12/05 22:26:32  wenger
// Merged s2d_torsion_rest_0910_br_0 thru s2d_torsion_rest_0910_br_0
// to the trunk.
//
// Revision 1.16.4.4  2009/12/05 21:45:39  wenger
// Hopefully final cleanup before merge(!).
//
// Revision 1.16.4.3  2009/12/02 21:38:45  wenger
// Added the new TAR_LEVEL_LINK_CHECK processing level, and made it
// the default; partially implemented passing the torsion angle URL
// thru the CGI script arguments as an optimization.  Added the capability
// of running some subsets of the tests in test_all.
//
// Revision 1.16.4.2  2009/12/02 17:26:53  wenger
// The torsion angle CGI links now work for the "standard" NMR Browser
// setup (haven't tested it yet with the visualization server).
// A bunch of clean up still needed...  Added a new test for "all-in-one"
// processing, but that doesn't work yet.
//
// Revision 1.16.4.1  2009/11/17 23:25:18  wenger
// Writing of torsion angle sessions, etc., now mostly working (links in
// summary page are still not right, lots of other cleanup, finding
// data in restraint grid still needed).
//
// Revision 1.16  2009/08/25 18:15:57  wenger
// Merged s2d_sparta_deltashift_br_0 thru s2d_sparta_deltashift_br_3
// to trunk.
//
// Revision 1.15.4.4  2009/08/21 19:29:38  wenger
// Peptide-CGI now creates the new "all-in-one" SPARTA visualization.
// But some existing tests fail -- DON'T MERGE UNTIL THAT IS FIXED.
// (Tagging with s2d_sparta_deltashift_br_1 before this commit,
// s2d_sparta_deltashift_br_2 after.)
//
// Revision 1.15.4.3  2009/07/28 22:40:01  wenger
// Added processing of SPARTA-calculated delta shift average values.
//
// Revision 1.15.4.2  2009/07/22 20:20:22  wenger
// Fixed residue numbering in SPARTA delta shift visualizations;
// changed "theoretical" to "SPARTA-calculated" and changed method
// names, etc., to match.
//
// Revision 1.15.4.1  2009/07/06 20:37:23  wenger
// Summary pages now have links for SPARTA-calculated deltashifts.
//
// Revision 1.15  2009/05/11 22:22:46  wenger
// Added "Force reprocessing" button to summary pages (mainly for
// testing).
//
// Revision 1.14  2009/04/15 16:21:04  wenger
// Merged s2d_hc_spectrum_br_0 thru s2d_hc_spectrum_br_end to trunk;
// fixed test61 and test61_3.
//
// Revision 1.13.2.1  2009/04/14 22:09:07  wenger
// Session file, visualization-specific HTML file and summary page link
// are now created; removed "legend view" from session template;
// documented and cleaned up code.  (Still needs help for H vs C
// visualization.)
//
// Revision 1.13  2009/03/24 19:04:50  wenger
// Fixed layout of nucleic acid deltashift session (made windows line
// up better, etc.); fixed nucleotide counts in summary html page, and
// changed residue to nucleotide where appropriate; fixed nucleic acid
// deltashift html pages so that they link to the nucleic-acid-specific
// help page.
//
// Revision 1.12  2008/12/01 20:37:53  wenger
// Merged s2d_bug_037_br_0 thru s2d_bug_037_br_2 to trunk.
//
// Revision 1.11.2.1  2008/11/17 19:28:07  wenger
// Added entity assembly IDs to summary page and specific visualization pages.
//
// Revision 1.11  2008/07/02 16:29:20  wenger
// S2 order parameter visualizations are done and approved by Eldon;
// tests at least partially updated for S2 order stuff;
// reversed the order of data sets in the data selection view of
// 3D visualizations (more closely matches the summary page); minor
// fix to testclean target in top-level makefile; minor fix to
// relaxation session template (bar widths now set); added indices
// to data set titles in 3D visualizations.
//
// Revision 1.10  2008/06/04 21:12:01  wenger
// New Peptide-CGI summary page is implemented, test work except for
// test52 for some weird reason.  (Still may need some other changes
// before release, though.)
//
// Revision 1.9  2008/04/09 19:35:42  wenger
// Added frame details to individual visualization pages in preparation
// for summary page changes; spelled out Linear Analysis of Chemical
// Shifts; removed some unneeded parameters from the S2DSummaryHtml*
// constructors.
//
// Revision 1.8  2007/11/15 17:15:36  wenger
// Cleaned out cvs history in source files.
//
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
	_large = new S2DSummaryHtmlLarge(name, longName, masterId,
	  localFiles, htmlDir, restraintOnly);

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
    public void writeDeltashift(int frameIndex, int entityAssemblyID,
      int count, boolean isNucleicAcid) throws IOException
    {
        if (doDebugOutput(12)) {
	    System.out.println("S2DSummaryHtml.writeDeltashift()");
	}

    	_normal.writeDeltashift(frameIndex, entityAssemblyID, count,
	  isNucleicAcid);
    	_large.writeDeltashift(frameIndex, entityAssemblyID, count,
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
    	_large.writeSpartaDeltashift(entityAssemblyID, frameIndex,
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
    public void writeCoupling(int frameIndex, int entityAssemblyID,
      int count) throws IOException
    {
        if (doDebugOutput(12)) {
	    System.out.println("S2DSummaryHtml.writeCoupling()");
	}

	_normal.writeCoupling(frameIndex, entityAssemblyID, count);
	_large.writeCoupling(frameIndex, entityAssemblyID, count);
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
	_large.writeRelax(dataType, frequency, suffix, name,
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
	_large.writeHetNOE(name, frameIndex, entityAssemblyID, count);
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
    // Writes the H vs. C chemical shifts link.
    public void writeHvsCShifts(int frameIndex, int count)
      throws IOException
    {
        if (doDebugOutput(12)) {
	    System.out.println("S2DSummaryHtml.writeHvsCShifts()");
	}

	_normal.writeHvsCShifts(frameIndex, count);
	_large.writeHvsCShifts(frameIndex, count);
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
    public void writeChemShiftRef(String pdbId, int frameIndex)
      throws IOException
    {
        if (doDebugOutput(12)) {
	    System.out.println("S2DSummaryHtml.writeChemShiftRef()");
	}

	_normal.writeChemShiftRef(pdbId, frameIndex);
	_large.writeChemShiftRef(pdbId, frameIndex);
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
    // Writes the S2 order parameter link.
    public void writeS2Order(int frameIndex, int entityAssemblyID,
      int count) throws IOException
    {
        if (doDebugOutput(12)) {
	    System.out.println("S2DSummaryHtml.writeS2Order()");
	}

	_normal.writeS2Order(frameIndex, entityAssemblyID, count);
	_large.writeS2Order(frameIndex, entityAssemblyID, count);
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
	_large.writeTorsionAngle(pdbId, frameIndex, isRR);
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
	_large.writeTorsionAngleCGI(pdbId, tarUrl, frameIndex, isRR);
    }

    //-------------------------------------------------------------------
    // Writes the distance restraint link, where we've actually done the
    // distance restraint processing.
    public void writeDistRestraint(String pdbId, int frameIndex)
      throws IOException
    {
        if (doDebugOutput(12)) {
	    System.out.println("S2DSummaryHtml.writeDistRestraint()");
	}

	_normal.writeDistRestraint(pdbId, frameIndex);
	_large.writeDistRestraint(pdbId, frameIndex);
    }

    //-------------------------------------------------------------------
    // Writes the distance restraint link, where the link is a CGI script
    // invocation (we haven't already done the distance restraint
    // processing).
    public void writeDistRestraintCGI(String pdbId, String tarUrl,
      int frameIndex) throws IOException
    {
        if (doDebugOutput(12)) {
	    System.out.println("S2DSummaryHtml.writeDistRestraintCGI()");
	}

	_normal.writeDistRestraintCGI(pdbId, tarUrl, frameIndex);
	_large.writeDistRestraintCGI(pdbId, tarUrl, frameIndex);
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

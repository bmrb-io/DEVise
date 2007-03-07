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
// Revision 1.5  2006/08/21 21:01:11  wenger
// Added second summary page for direct access to all large-size
// visualizations; updated all tests accordingly.
//
// Revision 1.4  2006/05/11 21:10:25  wenger
// Fixed problems with some html page titles being based on the BMRB
// ID rather than the name, etc.  Fixed test36 to work with new LACS
// file names.
//
// Revision 1.3  2006/02/01 21:34:32  wenger
// Merged peptide_cgi_10_8_0_br_0 thru peptide_cgi_10_8_0_br_2
// to the trunk.
//
// Revision 1.2  2006/02/01 20:23:13  wenger
// Merged V2_1b4_br_0 thru peptide_cgi_10_8_0_base to the
// trunk.
//
// Revision 1.1.2.22.6.4  2005/11/02 21:13:24  wenger
// Changed ovals to text Os in LACs visualization so they stay the
// same size in *pixels*; fixed up titles for LACS stuff in summary
// html page, specific html pages, and sessions.
//
// Revision 1.1.2.22.6.3  2005/10/14 21:19:31  wenger
// Most LACS processing now in place -- still needs lots of cleanup,
// though.
//
// Revision 1.1.2.22.6.2  2005/06/28 17:08:22  wenger
// Ambiguity code and figure of merit visualizations now use 2D main
// views; changed "Pistachio" to "assignment figure of merit" in
// visualizations, etc.
//
// Revision 1.1.2.22.6.1  2005/05/19 16:07:43  wenger
// Merged nmrfam_mods2_br (argh -- must have forgotten to make
// nmrfam_mods2_br_0 tag!) thru nmrfam_mods2_br_3 to
// peptide_cgi_10_8_0_br.
//
// Revision 1.1.2.22.4.1  2005/05/12 19:07:41  wenger
// Merged nmrfam_mods_br_0 thru nmrfam_mods_br_1 to new
// nmrfam_mods2_br (created to get ambiguity visualization help
// and fix to coordinate visualization help).
//
// Revision 1.1.2.22.2.1  2005/05/12 14:10:12  wenger
// Peptide-CGI now allows non-numeric BMRB IDs; changed test3 to make
// sure cache is used when it should be; added test26 to test non-
// numeric BMRB ID.
//
// Revision 1.1.2.22  2005/04/05 19:06:53  wenger
// More ambiguity vis changes: "color by ambiguity code" is initially
// selected; changes to some of the ambiguity-related strings.
//
// Revision 1.1.2.21  2005/03/22 20:34:38  wenger
// Merged ambiguity_vis2_br_0 thru ambiguity_vis2_br_3 to V2_1b4_br.
//
// Revision 1.1.2.20  2005/03/22 19:22:54  wenger
// Fixes to atomic coordinates and Pistachio visualizations: data X select
// cursor can't go outside view; we show the full data range in the data
// view at startup.  Fixed some typos in the atomic coordinate help page.
// Added a help page for the Pistachio visualization (without any info).
// Changed makefile to force Java 1.3-compatible class files (because of
// nasty new Java 1.5).
//
// Revision 1.1.2.19.4.1  2005/03/10 19:27:36  wenger
// Merged ambiguity_vis_br_0 thru ambiguity_vis_br_end to
// ambiguity_vis2_br.
//
// Revision 1.1.2.19.2.1  2005/03/10 18:34:08  wenger
// I need to commit the ambiguity stuff I've done so far so I can make
// a new ambiguity branch that has the latest Pistachio changes.
//
// Revision 1.1.2.19  2005/01/31 23:02:55  wenger
// Merged pistachio_vis_br_0 thru pistachio_vis_br_1a to V2_1b4_br.
//
// Revision 1.1.2.18  2004/12/16 22:01:55  wenger
// Chem shift reference link-only processing option is pretty much
// implemented -- the actual csr_wrapper script on leech needs some
// mods, and I need to test Peptide-CGI with the real ChemShift
// software, but other than that I think it's done.
//
// Revision 1.1.2.17  2004/12/03 17:18:34  wenger
// Added BMRB accession number, PDB ID (if relevant), timestamp and
// Peptide-CGI version number to Peptide-CGI visualizations.
//
// Revision 1.1.2.16  2004/11/18 19:33:18  wenger
// Added more descriptive chem shift reference links in Jafar mode,
// since the section headers don't show up in the Jafar GUI.
//
// Revision 1.1.2.15  2004/11/15 19:44:32  wenger
// All NMR Browser visualization help now goes to a single html window,
// instead of popping up a new window each time; better layout for
// chem shift reference stuff in the summary page; fixed a problem in
// the CSR #3 help page.
//
// Revision 1.1.2.14.2.1  2005/01/12 20:46:42  wenger
// Pistachio processing is now integrated into the normal Peptide-CGI
// processing -- the Pistachio visualization is generated autmatically
// if the Pistachio data exists.  (Still needs some cleanup, though.)
// (We generate the Pistachio visualization by generating a temporary
// mmCIF file with coordinates -- that is then run through the normal
// coordinate processing to generate the DEVise file with Pistachio
// coordinates.)
//
// Revision 1.1.2.14  2004/09/25 15:57:51  wenger
// Peptide-CGI version and generation time are now visible as opposed to
// being comments.
//
// Revision 1.1.2.13  2004/09/14 22:07:14  wenger
// Added PDB ID to 3D structure link strings so it shows up in
// the Jafar GUI.
//
// Revision 1.1.2.12  2004/09/10 22:38:23  wenger
// Added X=Y line (as requested by Eldon) to chem shift reference
// visualization #3.
//
// Revision 1.1.2.11  2004/09/01 19:35:38  wenger
// Temporarily disabled chem shift reference visualization 3,
// pending approval by Eldon; I need to do a bug fix release.
//
// Revision 1.1.2.10  2004/06/23 17:39:26  wenger
// Preliminary version of chem shift reference visualization 3 in
// place; cleaned up 4317 chem shift ref test data; changed fonts
// in chem shift ref visualization 1.
//
// Revision 1.1.2.9  2004/06/16 19:23:31  wenger
// Added headers to data files generated by Peptide-CGI.
//
// Revision 1.1.2.8  2004/01/26 18:55:37  wenger
// Incorporated "Eldon's" chemical shift reference session; added help
// pages for both chem shift ref visualizations (with no help info
// for now).
//
// Revision 1.1.2.7  2003/12/12 19:11:45  wenger
// Fixed bug 026 (structure link is still shown in summary page even if
// no bonds are written).
//
// Revision 1.1.2.6  2003/11/30 23:39:07  wenger
// Fixed up chem shift ref output in summary html file; added more
// debug code to S2DResList.
//
// Revision 1.1.2.5  2003/10/23 18:08:57  wenger
// Basic chem shift ref stuff is in place -- session and specific html
// files are generated, summary html file has link, etc.  The main thing
// left is the mechanics of calling Dan's chem shift ref software --
// right now I am just using a script that creates dummy chem shift
// ref data files.
//
// Revision 1.1.2.4  2003/06/11 15:51:32  wenger
// BMRB entry number is now on the summary page.
//
// Revision 1.1.2.3  2003/05/14 21:45:52  wenger
// Changed "/"s in file names to File.separator (hope I got them all!).
//
// Revision 1.1.2.2  2003/05/14 20:10:48  wenger
// Split generated html and data files into separate directories (for
// better security); moved html and session templates out of main
// html and session directories.
//
// Revision 1.1.2.1  2003/04/22 21:58:16  wenger
// Added package name to peptide-cgi java code and put everything into
// a single jar file; version is now 6.0.
//
// Revision 1.12.6.2  2003/04/09 18:02:14  wenger
// First version of visualization-server capability now in place; still
// needs some fixes.
//
// Revision 1.12.6.1  2003/01/14 16:51:49  wenger
// Merged V2_1b3_br_0 thru V2_1b3_br_end to V2_1b4_br.
//
// Revision 1.12.4.8  2002/11/01 17:40:15  wenger
// Fixed bug 013 (s2d fails if no write permission on output files).
//
// Revision 1.12.4.7  2002/10/31 22:17:36  wenger
// Changed Exception output to use toString() instead of getMessage()
// because that (at least sometimes) produces more complete information.
//
// Revision 1.12.4.6  2002/10/28 22:22:02  wenger
// Finished separating PDB entry processing from BMRB entry processing
// (the summary page is returned without getting any PDB entries; the
// PDB processing occurs when the user clicks the link in the summary
// page).
//
// Revision 1.12.4.5  2002/10/23 19:01:28  wenger
// PDB-only processing now works (I still need to set things up properly
// for the link in the summary page, though); S2DCompressedURL now reads
// more than one byte at a time; test_s2d works with new command-line
// arguments.
//
// Revision 1.12.4.4  2002/09/11 20:04:30  wenger
// More improvements to 3D visualizations (added better help, changed
// some cursor constraints and automatic updates); removed special-case
// link to prototype 4096 3D visualization from summary HTML page.
//
// Revision 1.12.4.3  2002/08/05 22:12:57  wenger
// Put a limit of 10,000 atoms on 3D structures (for now, at least),
// because too many atoms (e.g., BMRB 4081 with 64,752) crashes the
// JavaScreen.
//
// Revision 1.12.4.2  2002/08/05 21:23:22  wenger
// We only try to get the first set of atomic coordinates from the PDB;
// associated BMRB and PDB IDs are saved in the summary HTML file;
// increased s2d file size limit because some PDB files are really big
// once they're uncompressed; various other minor improvements.
//
// Revision 1.12.4.1  2002/07/19 18:38:12  wenger
// Merged V2_1b2_br_0 thru V2_1b2_br_end to V2_1b3_br.
//
// Revision 1.12.2.1  2002/07/01 20:07:37  wenger
// Getting coordinates from PDB works in stand-alone mode, but still has
// problems running as a CGI script; needs provisions to check up-to-
// dateness vs. PDB files, provision for various error conditions.
//
// Revision 1.12  2002/06/17 19:41:40  wenger
// Merged V1_7b0_br_1 thru V1_7b0_br_2 to trunk.
//
// Revision 1.11.8.2  2002/06/05 18:56:09  wenger
// Improved summary page labels as per feedback from Eldon; fixed bug
// 006 (visual filters not set correctly when some sessions opened).
//
// Revision 1.11.8.1  2002/05/13 15:33:25  wenger
// Added "values" to the peptide-cgi summary page where appropriate.
//
// Revision 1.11  2001/10/10 22:37:33  wenger
// First version of atomic coordinate extraction and visualization (atoms,
// not bonds, only tested on 4096 -- coordinates must be in same NMR-STAR
// file as other data); fixed various minor problems with peptide-cgi code.
//
// Revision 1.10  2001/07/25 19:56:23  wenger
// Started adding configuration setup stuff; s2d gets URLs from properties
// file.
//
// Revision 1.9  2001/05/08 18:24:18  wenger
// Fixed problem getting residue count if a star file contains info for
// more than one protein; added residue counts to 'all shifts' and 'H
// vs. N' visualizations.
//
// Revision 1.8  2001/04/30 17:45:23  wenger
// Added special link to 3D 4096 visualization to 4096 summary page;
// added "No chemical shift data available" message to appropriate
// summary pages.
//
// Revision 1.7  2001/04/17 17:09:10  wenger
// Added display of H vs. N chem shifts.
//
// Revision 1.6  2001/04/16 19:49:11  wenger
// Added display of all chem shifts by amino acid.
//
// Revision 1.5  2001/03/13 14:50:10  wenger
// Added cache invalidation: cache files are not used if peptide-cgi code
// or NMR-Star file has changed since relevant cache files were generated.
//
// Revision 1.4  2001/03/08 21:10:35  wenger
// Merged changes from no_collab_br_2 thru no_collab_br_3 from the branch
// to the trunk.
//
// Revision 1.3  2001/03/08 20:33:24  wenger
// Merged changes from no_collab_br_0 thru no_collab_br_2 from the branch
// to the trunk.
//
// Revision 1.2.2.2  2001/02/21 18:13:46  wenger
// Eliminated "Details not available for this save frame." message; error
// details are not propagated to html output if Star-to-DEVise conversion
// fails.
//
// Revision 1.2.2.1  2001/02/09 16:57:13  wenger
// Added heteronuclear NOE; made T1 and T2 relaxation errors optional
// (conversion doesn't fail if they are not found); added an X margin of
// 0.5 in all sessions; updated star file list; misc. minor cleanups.
//
// Revision 1.2  2001/01/19 15:39:07  wenger
// Added T1 and T2 relaxation; removed some unnecessary variables from
// coupling constants; added schema files to installation, unified T1
// and T2 relaxation schema.
//
// Revision 1.1  2001/01/17 20:00:08  wenger
// Restructured the peptide-cgi code to make it much more maintainable.
//

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
        if (DEBUG >= 1) {
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
        if (DEBUG >= 1) {
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
        if (DEBUG >= 1) {
	    System.out.println("S2DSummaryHtml.close()");
	}

	_normal.close(bmrbIds, pdbIds);
	_large.close(bmrbIds, pdbIds);
    }

    //-------------------------------------------------------------------
    // Writes the info for the start of a save frame.
    public void startFrame(String frameDetails) throws S2DException
    {
        if (DEBUG >= 1) {
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
        if (DEBUG >= 1) {
	    System.out.println("S2DSummaryHtml.endFrame()");
	}

    	_normal.endFrame();
    	_large.endFrame();
    }

    //-------------------------------------------------------------------
    // Writes the deltashift link.
    public void writeDeltashift(int frameIndex, int count) throws IOException
    {
        if (DEBUG >= 2) {
	    System.out.println("S2DSummaryHtml.writeDeltashift()");
	}

    	_normal.writeDeltashift(frameIndex, count);
    	_large.writeDeltashift(frameIndex, count);
    }

    //-------------------------------------------------------------------
    // Writes the CSI link.
    public void writeCSI(int frameIndex, int count) throws IOException
    {
        if (DEBUG >= 2) {
	    System.out.println("S2DSummaryHtml.writeCSI()");
	}

    	_normal.writeCSI(frameIndex, count);
    	_large.writeCSI(frameIndex, count);
    }

    //-------------------------------------------------------------------
    // Writes the percent assignment link.
    public void writePctAssign(int frameIndex, int count) throws IOException
    {
        if (DEBUG >= 2) {
	    System.out.println("S2DSummaryHtml.writePctAssign()");
	}

    	_normal.writePctAssign(frameIndex, count);
    	_large.writePctAssign(frameIndex, count);
    }

    //-------------------------------------------------------------------
    // Writes the coupling constant link.
    public void writeCoupling(int frameIndex, int count) throws IOException
    {
        if (DEBUG >= 2) {
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
        if (DEBUG >= 2) {
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
        if (DEBUG >= 2) {
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
        if (DEBUG >= 2) {
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
        if (DEBUG >= 2) {
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
        if (DEBUG >= 2) {
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
        if (DEBUG >= 2) {
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
        if (DEBUG >= 2) {
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
        if (DEBUG >= 2) {
	    System.out.println("S2DSummaryHtml.writeChemShiftRefCGI()");
	}

	_normal.writeChemShiftRefCGI(pdbId, frameIndex);
	_large.writeChemShiftRefCGI(pdbId, frameIndex);
    }

    //-------------------------------------------------------------------
    // Writes the Pistachio link.
    public void writePistachio(int frameIndex) throws IOException
    {
        if (DEBUG >= 2) {
	    System.out.println("S2DSummaryHtml.writePistachio()");
	}

	_normal.writePistachio(frameIndex);
	_large.writePistachio(frameIndex);
    }

    //-------------------------------------------------------------------
    // Writes the ambiguity link.
    public void writeAmbiguity(int frameIndex) throws IOException
    {
        if (DEBUG >= 2) {
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
        if (DEBUG >= 2) {
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
}

// ========================================================================

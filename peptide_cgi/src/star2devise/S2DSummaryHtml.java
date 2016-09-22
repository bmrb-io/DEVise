// ========================================================================
// DEVise Data Visualization Software
// (c) Copyright 2000-2013
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
// Revision 1.29.8.1  2013/10/22 19:45:09  wenger
// Merged peak_lists_br_0 thru peak_lists_br_2 to peak_lists2_br.
//
// Revision 1.29.6.1  2013/06/06 19:31:42  wenger
// Changed the mode to make more use of the S2DUtils.string2Double()
// and S2DUtils.string2Int() methods.
//
// Revision 1.29  2012/03/12 01:08:48  wenger
// Merged vis_page_fix_base thru vis_page_fix_br_1 to trunk (includes
// fixing of some tests to be more general).
//
// Revision 1.28.8.2  2012/01/20 21:23:13  wenger
// Make some fixes to the T1/T2 relaxation menus.
//
// Revision 1.28.8.1  2012/01/20 21:00:04  wenger
// Got a bunch of the visualization summary page menus working.
//
// Revision 1.28  2011/05/19 19:46:09  wenger
// Merged s2d_mol_dyn_br_0 thru s2d_mol_dyn_br_2 to trunk.
//
// Revision 1.27.2.2  2011/05/13 17:02:45  wenger
// Fixed up configuration for BMRB to point to "real" s2predict output files.
//
// Revision 1.27.2.1  2011/04/08 17:48:10  wenger
// Writing s2predicted links in the summary html page is now partially
// working.
//
// Revision 1.27  2011/01/07 22:10:29  wenger
// Merged s2d_multi_entry2_br_0 thru s2d_multi_entry2_br_1 to trunk.
//
// Revision 1.26.2.7  2011/01/05 20:57:40  wenger
// Fixed a bug that caused improper processing for the uploaded data/
// entry combination.
//
// Revision 1.26.2.6  2011/01/05 15:33:17  wenger
// More cleanup, including at least temporarily(?) just eliminating
// some things for the multi-entry visualizations, such as system name
// and frame title.
//
// Revision 1.26.2.5  2011/01/03 21:47:33  wenger
// Cleaned up the getInfo()-related stuff in S2DSummaryHtml.
//
// Revision 1.26.2.4  2010/12/29 17:49:37  wenger
// The multi-entry code now gets frame index, entity assembly ID, and
// peak count info from comments in the single-entry summary HTML files.
//
// Revision 1.26.2.3  2010/12/28 23:15:29  wenger
// We now print comments in the single-entry summary HTML pages that we
// will use to figure out what data we have for the multi-entry processing
// (reading the comments is not implemented yet).
//
// Revision 1.26.2.2  2010/12/21 00:24:45  wenger
// Got rid of 'sizeString' stuff in summary HTML code; started putting
// multi-entry summary page links into a table.
//
// Revision 1.26.2.1  2010/12/07 23:43:49  wenger
// Merged s2d_multi_entry_br_0 thru s2d_multi_entry_br_1 to
// s2d_multi_entry2_br.
//
// Revision 1.26  2010/12/07 17:41:16  wenger
// Did another version history purge.
//
// Revision 1.25.10.2  2010/12/04 00:34:55  wenger
// Got preliminary multi-entry summary page working.
//
// Revision 1.25.10.1  2010/11/16 00:01:17  wenger
// We now create a "two-entry" summary HTML page (but it doesn't have the
// right links yet); added "two-entry" HTML pages to the tests.
//
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
    public static String fileName(String htmlDir, String name,
      String fullName)
    {
    return directory(htmlDir, name) + File.separator +
      fileNameShort(fullName);
    }

    //-------------------------------------------------------------------
    public static String fileNameShort(String fullName)
    {
    return fullName + S2DNames.SUMMARY_HTML_SUFFIX +
      S2DNames.HTML_SUFFIX;
    }

    //-------------------------------------------------------------------
    public String getHtmlDir()
    {
        return _htmlDir;
    }

    //-------------------------------------------------------------------
    // Constructor.  Opens the html file and writes the header.
    //TEMP -- starFileName is not used
    public S2DSummaryHtml(String name, String fullName, String longName,
      String masterId, Vector localFiles, String htmlDir,
      String starFileName, String systemName, String frameTitle,
      boolean restraintOnly) throws S2DException
    {
        if (doDebugOutput(11)) {
        System.out.println("S2DSummaryHtml.S2DSummaryHtml(" +
          name + ", " + masterId + ")");
    }

    _htmlDir = htmlDir;

    _normal = new S2DSummaryHtmlNormal(name, fullName, longName,
      masterId, localFiles, htmlDir, restraintOnly);

    _normal.initialize(systemName, frameTitle);
    }

    //-------------------------------------------------------------------
    // Constructor for multi-entry summary page.  Opens the html file and
    // writes the header.
    //TEMP -- starFileName is not used
    public S2DSummaryHtml(String name, String fullName, String longName,
      String masterId, String extraId, Vector localFiles, String htmlDir,
      String starFileName, String systemName, String frameTitle,
      boolean restraintOnly) throws S2DException
    {
        if (doDebugOutput(11)) {
        System.out.println("S2DSummaryHtml.S2DSummaryHtml(" +
          name + ", " + masterId + ", " + extraId + ")");
    }

    _htmlDir = htmlDir;

    _normal = new S2DSummaryHtmlNormal(name, fullName, longName,
      masterId, extraId, localFiles, htmlDir, restraintOnly);

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
    public void writeCSI(int entityAssemblyID, int frameIndex, int count)
      throws IOException
    {
        if (doDebugOutput(12)) {
        System.out.println("S2DSummaryHtml.writeCSI()");
    }

        _normal.writeCSI(entityAssemblyID, frameIndex, count);
    }

    //-------------------------------------------------------------------
    // Writes the percent assignment link.
    public void writePctAssign(int entityAssemblyID, int frameIndex,
      int count) throws IOException
    {
        if (doDebugOutput(12)) {
        System.out.println("S2DSummaryHtml.writePctAssign()");
    }

        _normal.writePctAssign(entityAssemblyID, frameIndex, count);
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
    public void writeAllShifts(int entityAssemblyID, int frameIndex,
      int count) throws IOException
    {
        if (doDebugOutput(12)) {
        System.out.println("S2DSummaryHtml.writeAllShifts()");
    }

    _normal.writeAllShifts(entityAssemblyID, frameIndex, count);
    }

    //-------------------------------------------------------------------
    // Writes the H vs. N chemical shifts link.
    public void writeHvsNShifts(int frameIndex, int entityAssemblyID,
      int count) throws IOException
    {
        if (doDebugOutput(12)) {
        System.out.println("S2DSummaryHtml.writeHvsNShifts()");
    }

    _normal.writeHvsNShifts(frameIndex, entityAssemblyID, count);
    }

    //-------------------------------------------------------------------
    // Writes the H vs. C chemical shifts link.
    public void writeHvsCShifts(int frameIndex, int entityAssemblyID,
      int count) throws IOException
    {
        if (doDebugOutput(12)) {
        System.out.println("S2DSummaryHtml.writeHvsCShifts()");
    }

    _normal.writeHvsCShifts(frameIndex, entityAssemblyID, count);
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
    public void writeAmbiguity(int frameIndex, int entityAssemblyID)
      throws IOException
    {
        if (doDebugOutput(12)) {
        System.out.println("S2DSummaryHtml.writeAmbiguity()");
    }

    _normal.writeAmbiguity(frameIndex, entityAssemblyID);
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
    // Writes the predicted vs. experimental S2 link.
    public void writeS2Pred(String pdbId, int coordIndex, int frameIndex)
      throws IOException
    {
        if (doDebugOutput(12)) {
        System.out.println("S2DSummaryHtml.writeS2Pred()");
    }

    _normal.writeS2Pred(pdbId, coordIndex, frameIndex);
    }

    //-------------------------------------------------------------------
    // Writes the predicted vs. experimental S2 link, where the link is
    // a CGI script invocation (we haven't already done the s2predict
    // processing).
    public void writeS2PredCGI(String pdbId, int coordIndex,
      int frameIndex) throws IOException
    {
        if (doDebugOutput(12)) {
        System.out.println("S2DSummaryHtml.writeS2PredCGI()");
    }

    _normal.writeS2PredCGI(pdbId, coordIndex, frameIndex);
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
    // Writes the two-entry H vs. N chemical shifts link.
    protected void write2EntHvsNShifts(int frameIndex1, int frameIndex2,
      int eaId1, int eaId2, int peakCount1, int peakCount2)
      throws IOException
    {
        if (doDebugOutput(12)) {
        System.out.println("S2DSummaryHtml.write2EntHvsNShifts()");
    }

        _normal.write2EntHvsNShifts(frameIndex1, frameIndex2, eaId1,
      eaId2, peakCount1, peakCount2);
    }

    //-------------------------------------------------------------------
    // Writes the two-entry H vs. C chemical shifts link.
    protected void write2EntHvsCShifts(int frameIndex1, int frameIndex2,
      int eaId1, int eaId2, int peakCount1, int peakCount2)
      throws IOException
    {
        if (doDebugOutput(12)) {
        System.out.println("S2DSummaryHtml.write2EntHvsCShifts()");
    }

        _normal.write2EntHvsCShifts(frameIndex1, frameIndex2, eaId1,
      eaId2, peakCount1, peakCount2);
    }

    //-------------------------------------------------------------------
    // Write a message to the summary html file.
    public void writeMessage(String msg, boolean horRule)
    {
    _normal.writeMessage(msg, horRule);
    }

    //-------------------------------------------------------------------
    // Information about data sets.
    public static class DatasetInfo {
        int _entityAssemblyId;
    int _frameIndex;
    int _peakCount;
    }

    //-------------------------------------------------------------------
    // Get info about what data sets of the given dataType are available
    // for the specified entry (we get this information by parsing
    // special comments in the summary HTML file).
    public static Vector getInfo(String htmlDir, String name,
      int dataType)
    {
    if (doDebugOutput(11)) {
        System.out.println("S2DSummaryHtml.getInfo(" + htmlDir +
          ", " + name + ", " + dataType + ")");
    }

        Vector result = new Vector();

    try {
        String infoTag = "";

        switch (dataType) {
        case S2DUtils.TYPE_HVSN_CHEM_SHIFTS:
            infoTag = "Info_HvsN";
            break;

        case S2DUtils.TYPE_HVSC_CHEM_SHIFTS:
            infoTag = "Info_HvsC";
            break;

        default:
        throw new S2DError("Unsupported data type: " + dataType);
        }

        String fn = fileName(htmlDir, name, name);

        BufferedReader reader = new BufferedReader(new FileReader(fn));

        String line;
        while ((line = reader.readLine()) != null) {
        if (line.indexOf(infoTag) >= 0) {
            DatasetInfo info = new DatasetInfo();
                    info._entityAssemblyId = getIntValue(line, "eaId");
                info._frameIndex = getIntValue(line, "frameIndex");
                info._peakCount = getIntValue(line, "peakCount");
            result.addElement(info);
        }
        }

            reader.close();
    } catch (Exception ex) {
        System.err.println("Error (" + ex.toString() +
          ") getting data set info for " + name);
    }

    if (doDebugOutput(12)) {
        for (int index = 0; index < result.size(); index++) {
        DatasetInfo info = (DatasetInfo)result.elementAt(index);
            System.out.println("  " + info._entityAssemblyId + ", " +
          info._frameIndex + ", " + info._peakCount);
        }
    }

    return result;
    }

    //===================================================================
    // PRIVATE METHODS

    //-------------------------------------------------------------------
    // Get an integer value from line, where line is of the form
    // "...<tag>:<value>..."
    private static int getIntValue(String line, String valueTag)
      throws S2DException
    {
    valueTag += ":";
        int index1 = line.indexOf(valueTag);
    if (index1 < 0) {
        throw new S2DError("No " + valueTag + " tag found in " + line);
    }
    String str1 = line.substring(index1 + valueTag.length());
    int index2 = str1.indexOf(" ");
    String str2;
    if ( index1 < 0) {
        str2 = str1;
    } else {
        str2 = str1.substring(0, index2);
    }
        int result = S2DUtils.string2Int(str2);

    return result;
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

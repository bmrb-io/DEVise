// ========================================================================
// DEVise Data Visualization Software
// (c) Copyright 2009-2010
// By the DEVise Development Group
// Madison, Wisconsin
// All Rights Reserved.
// ========================================================================

// Under no circumstances is this software to be copied, distributed,
// or altered in any way without prior permission from the DEVise
// Development Group.

// ------------------------------------------------------------------------

// This class implements the output of distance restraint data.
// For each set of distance restraint data, it creates a data file, a
// session file, an individual html file, and a link in the summary
// html file.

// ------------------------------------------------------------------------

// $Id$

// $Log$
// Revision 1.5  2010/04/27 18:47:07  wenger
// Fixed bugs Eldon found in testing of whether restraints info exists
// (for *_LEVEL_LINK_CHECK level of restraint processing).
//
// Revision 1.4  2010/04/23 16:51:05  wenger
// Merged s2d_dist_rest_1002_br_0 thru s2d_dist_rest_1002_br_1 to trunk.
//
// Revision 1.3  2010/03/10 22:36:16  wenger
// Added NMR-STAR file version to summary html page and detailed
// visualization version info (to-do 072).  (Doing this before I
// add multiple NMR-STAR paths so we can see which NMR-STAR file
// was used.)
//
// Revision 1.2.6.36  2010/04/22 23:32:06  wenger
// Fixed selection to work for ambiguous/non-ambiguous (forgot to add
// in the appropriate GAttr link in the session template before(!)).
//
// Revision 1.2.6.35  2010/04/22 18:29:20  wenger
// Eliminated multiple counting of ambiguous restraints; various fixes
// to the distance restraint tests.
//
// Revision 1.2.6.34  2010/04/22 16:38:10  wenger
// Various minor cleanups of distance restraint code.
//
// Revision 1.2.6.33  2010/04/21 21:03:05  wenger
// Largely fixed up tests for distance restraint processing -- still
// need to do some manual checking to make sure that values are
// correct; some minor cleanup of the distance restraint data
// files.
//
// Revision 1.2.6.32  2010/04/20 21:45:55  wenger
// Changed distance restraint data view title to include "Restraints per
// residue" so people have a better idea what is actually graphed.
//
// Revision 1.2.6.31  2010/04/20 19:22:59  wenger
// We now deal with pseudoatoms -- for restraints with pseudoatoms,
// we generate one record in the restraint data file for each combination
// of "real" atoms represented by pseudoatoms -- so we end up drawing
// a connection between all possible atoms in the 3D view.
//
// Revision 1.2.6.30  2010/04/19 20:37:37  wenger
// Changed distance restraint code to allow the combination of
// violated and ambiguous restraints -- the data is output for that,
// but I haven't updated the session template yet.
//
// Revision 1.2.6.29  2010/04/19 18:35:43  wenger
// Cleaned up the distance restraint meta-data-writing code in preparation
// for allowing selection by both violated/non-violated and ambiguous/
// non-ambiguous.
//
// Revision 1.2.6.28  2010/04/07 21:28:00  wenger
// Simplified restraint-counting code in preparation for dealing with
// *combinations* of violated and ambiguous restraints; changed coord_index
// in test_dist3rgi and test_dist3rri to match the entry.
//
// Revision 1.2.6.27  2010/04/06 19:04:19  wenger
// Partially completed restructuring the per-residue restraint count
// code in preparation for dealing with combinations of violated and
// ambiguous restraints.
//
// Revision 1.2.6.26  2010/03/31 20:56:34  wenger
// Got selection of which restraints to display *mostly* working (the
// "All" selections for violation and length type currently don't show
// anything); also, I think I need to add a separat selection for
// ambiguous and non-ambiguous.
//
// Revision 1.2.6.25  2010/03/09 20:36:47  wenger
// Added ambiguous and non-ambiguous restraint counts to data.
//
// Revision 1.2.6.24  2010/03/09 19:18:58  wenger
// Got rid of some notes about problems I've now fixed.
//
// Revision 1.2.6.23  2010/03/08 22:50:53  wenger
// Fixed titles in distance restraint data views.
//
// Revision 1.2.6.22  2010/03/08 21:33:33  wenger
// Fixed display of non-violated restraints (got rid of expression in
// parent value, and did the actual calculation in the Peptide-CGI code --
// see DEVise bug 999).
//
// Revision 1.2.6.21  2010/03/08 20:33:34  wenger
// Added distance restraint-processing code to make_view; fixed a couple
// of views in the session template that weren't supposed to show up in
// the JavaScreen; fixed the distance restraint links in the summary page.
//
// Revision 1.2.6.20  2010/03/08 19:16:23  wenger
// Now writing distance restraint links in summary html pages.
//
// Revision 1.2.6.19  2010/03/05 23:30:02  wenger
// We now create the session-specific HTML files for distance
// restraint visualizations.
//
// Revision 1.2.6.18  2010/03/05 23:12:35  wenger
// Creation of distance restraint session files is now (at least mostly)
// working.
//
// Revision 1.2.6.17  2010/03/05 21:02:31  wenger
// Basic selection views are now set up in the distance restraint session
// template; added some indexes to meta-data to make proper view setup
// easier; first version of schemas.
//
// Revision 1.2.6.16  2010/03/05 19:15:23  wenger
// We're now writing the (hopefully-correct) distance restraint meta-data.
//
// Revision 1.2.6.15  2010/03/03 23:22:59  wenger
// Created the findLengthType method to clean up the code (reduce
// indentation).
//
// Revision 1.2.6.14  2010/03/03 22:36:26  wenger
// Cross-residue restraints are now added to the counts for both
// residues.
//
// Revision 1.2.6.13  2010/03/03 21:52:29  wenger
// Added processing of distance constraint type (e.g., "hydrogen bond",
// "NOE", etc.).
//
// Revision 1.2.6.12  2010/03/03 20:49:46  wenger
// Distance restraint code now works with multiple save frames of
// distance restraints (as found in remediated restraint files).
//
// Revision 1.2.6.11  2010/03/03 19:39:05  wenger
// Changed _restraintTypes to _restraintLengths, since the restraint
// type is going to become something like "NOE", "hydrogen bond", etc.
//
// Revision 1.2.6.10  2010/03/03 19:07:47  wenger
// We now output total and violated restraint counts for intra-residue,
// sequential, etc., so we can select on those in the visualization.
//
// Revision 1.2.6.9  2010/02/25 21:43:32  wenger
// Added "Violated" or "Not_violated" strings for each individual
// restraint.
//
// Revision 1.2.6.8  2010/02/25 21:22:42  wenger
// We now calculate the count of violated restraints per residue.
//
// Revision 1.2.6.7  2010/02/25 21:03:39  wenger
// First partial implementation of writing out per-residue distance
// restraint data.
//
// Revision 1.2.6.6  2010/02/25 17:49:07  wenger
// Added notes from today's meeting with Eldon.
//
// Revision 1.2.6.5  2010/02/24 22:37:03  wenger
// Implemented basic distance restraint processing of remediated
// restraints files, added a test using remediated restraints via
// the BMRB web site.
//
// Revision 1.2.6.4  2010/02/24 21:02:18  wenger
// Added calculation of restraint types (for restraints grid).
//
// Revision 1.2.6.3  2010/02/24 20:03:06  wenger
// Got basic generation of distance restraint data from the restraints
// grid mostly working.
//
// Revision 1.2.6.2  2010/02/24 16:35:20  wenger
// Torsion angle restraints now work with new S2DNmrStarRGIfc and
// S2DNmrStarRGIfcFactory classes.
//
// Revision 1.2.6.1  2010/02/24 16:03:15  wenger
// More work on distance restraints: added -do_rrdist argument, partially
// completed code to get distance restraints URL from restraints grid;
// committing in preparation for combining S2DNmrStarDistRIfc and
// S2DNmrStarTarIfc into a single S2DNmrStarRGIfc class.
//
// Revision 1.2  2010/01/06 23:03:39  wenger
// Merged s2d_dist_rest_0912_br_0 thru s2d_dist_rest_0912_br_1 to trunk.
//
// Revision 1.1.2.3  2010/01/06 22:21:56  wenger
// Did a bunch of cleanups on the distance restraint code, in preparation
// for merging it to the trunk, so I can suspend work on it and move to
// working on the "PDB-ID-only" torsion angle restraint visualizations.
//
// Revision 1.1.2.2  2010/01/05 15:33:51  wenger
// Cleaned up the pdbIdToUrl() methods for various restraint-related
// classes.
//
// Revision 1.1.2.1  2009/12/16 00:07:56  wenger
// Added S2DDistRestraint and S2DNmrStarDistRIfc classes (mostly
// stubs so far); added a bunch of notes based on today's BMRB
// staff meeting discussions; also added stubbed-in classes in
// S2DSummaryHtml, etc.
//

// ========================================================================

package star2devise;

import EDU.bmrb.starlibj.SaveFrameNode;
import java.io.*;
import java.util.*;
import java.net.*;
import java.text.Collator;

public class S2DDistRestraint extends S2DRestraint {
    //===================================================================
    // VARIABLES

    private static final int DEBUG = 0;

    //TEMP -- make sure the part about CING is right
    public static final String STR_RESTRANTS_GRID = "Distance " +
      "restraints from restraints grid (violations calculated by CING)";
    public static final String STR_REMEDIATED_RESTRANTS = "Distance " +
      "restraints from remediated restraints";

    // Per-restraint data.
    private class RestraintInfo {
        // Note: we're keeping a lot of numerical values as Strings here, since
        // we're just spitting them back out as strings, so we avoid two
        // conversions.  Residue sequence codes are numerical so we can do
        // numerical comparisons, though. wenger 2010-02-24.

        public String _entityAssembly1Id;
	public String _resSeq1Code;
	public String _resSeq1Label;
        public String _entityAssembly2Id;
	public String _resSeq2Code;
	public String _resSeq2Label;

	public String _restraintType;
	public String _restraintLengthType;

            // combination of entity assembly ID, residue # and atom name
	public String _atom1Id;
	public String _atom1Name;

            // combination of entity assembly ID, residue # and atom name
	public String _atom2Id;
	public String _atom2Name;

	public String _lowerBound;
	public String _upperBound;
	public String _averageValue;
	public String _maxViolation;
	public boolean _isViolated = false;
	public boolean _isAmbiguous = false;

	public RestraintInfo() {
	}

	public RestraintInfo(RestraintInfo resInfo) {
            this._entityAssembly1Id = resInfo._entityAssembly1Id;
	    this._resSeq1Code = resInfo._resSeq1Code;
	    this._resSeq1Label = resInfo._resSeq1Label;
            this._entityAssembly2Id = resInfo._entityAssembly2Id;
	    this._resSeq2Code = resInfo._resSeq2Code;
	    this._resSeq2Label = resInfo._resSeq2Label;
	    this._restraintType = resInfo._restraintType;
	    this._restraintLengthType = resInfo._restraintLengthType;
	    this._atom1Id = resInfo._atom1Id;
	    this._atom1Name = resInfo._atom1Name;
	    this._atom2Id = resInfo._atom2Id;
	    this._atom2Name = resInfo._atom2Name;
	    this._lowerBound = resInfo._lowerBound;
	    this._upperBound = resInfo._upperBound;
	    this._averageValue = resInfo._averageValue;
	    this._maxViolation = resInfo._maxViolation;
	    this._isViolated = resInfo._isViolated;
	    this._isAmbiguous = resInfo._isAmbiguous;
	}

	public String toString()
	{
	    String violationStr = _isViolated ? "Violated" : "Non-violated";
	    String ambigStr = _isAmbiguous ?  "Ambiguous" : "Non-ambiguous";
	    return "" +
	      _entityAssembly1Id + "\t" +
	      _resSeq1Code + "\t" +
	      _resSeq1Label + "\t" +
	      _entityAssembly1Id + "_" + _resSeq1Code + "\t" +
	      _entityAssembly2Id + "\t" +
	      _resSeq2Code + "\t" +
	      _resSeq2Label + "\t" +
	      _entityAssembly2Id + "_" + _resSeq2Code + "\t" +
	      _lowerBound + "\t" +
	      _upperBound + "\t" +
	      _atom1Id + "\t" +
	      _atom1Name + "\t" +
	      _atom2Id + "\t" +
	      _atom2Name + "\t" +
	      _restraintType + "\t" +
	      _restraintLengthType + "\t" +
	      _averageValue + "\t" +
	      _maxViolation + "\t" +
	      violationStr + "\t" +
	      ambigStr;
	}
    }

        // Vector of RestraintInfo objects.
    private Vector _restraints = new Vector();

    private boolean _hasAverages;
    private boolean _hasViolations;
    private boolean _hasAmbiguous;

    private class PerLengthInfo {
	public int _violAllAmbigAll = 0;
	public int _violYesAmbigAll = 0;
	public int _violNoAmbigAll = 0;
	public int _violAllAmbigYes = 0;
	public int _violYesAmbigYes = 0;
	public int _violNoAmbigYes = 0;
	public int _violAllAmbigNo = 0;
	public int _violYesAmbigNo = 0;
	public int _violNoAmbigNo = 0;

        public void updateCounts(RestraintInfo restraint)
	{
	    _violAllAmbigAll++;
	    if (restraint._isViolated) {
		_violYesAmbigAll++;
	        if (restraint._isAmbiguous) {
		    _violAllAmbigYes++;
		    _violYesAmbigYes++;
		} else {
		    _violAllAmbigNo++;
		    _violYesAmbigNo++;
		}
	    } else {
		_violNoAmbigAll++;
	        if (restraint._isAmbiguous) {
		    _violAllAmbigYes++;
		    _violNoAmbigYes++;
		} else {
		    _violAllAmbigNo++;
		    _violNoAmbigNo++;
		}
	    }
	}

	public String toString()
	{
	    return "" +
	      _violAllAmbigAll + " " +
	      _violYesAmbigAll + " " +
	      _violNoAmbigAll + " " +
	      _violAllAmbigYes + " " +
	      _violYesAmbigYes + " " +
	      _violNoAmbigYes + " " +
	      _violAllAmbigNo + " " +
	      _violYesAmbigNo + " " +
	      _violNoAmbigNo;
	}
    };

    // Per-residue data.
    private class ResidueInfo {
	public PerLengthInfo _all = new PerLengthInfo();
	public PerLengthInfo _intraRes = new PerLengthInfo();
	public PerLengthInfo _sequential = new PerLengthInfo();
	public PerLengthInfo _medium = new PerLengthInfo();
	public PerLengthInfo _long = new PerLengthInfo();
	public PerLengthInfo _interMol = new PerLengthInfo();

	public String toString()
	{
	    return _all.toString() + " " + _intraRes.toString() + " " +
	      _sequential.toString() + " " + _medium.toString() + " " +
	      _long.toString() + " " + _interMol.toString();
	}
    }
    private Hashtable _entityAssemblies = new Hashtable();

    private S2DPseudoAtoms _pseudoAtoms;

    //===================================================================
    // PUBLIC METHODS

    //-------------------------------------------------------------------
    // Constructor.
    public S2DDistRestraint(String name, String longName,
      S2DNmrStarRestIfc star, String dataDir, String sessionDir,
      S2DSummaryHtml summary, String pdbId) throws S2DException
    {
	super(name, longName, star, dataDir, sessionDir, summary, pdbId);

        if (doDebugOutput(11)) {
	    System.out.println("S2DDistRestraint.S2DDistRestraint(" +
	      name + ")");
	}

	_pseudoAtoms = new S2DPseudoAtoms();

	Enumeration frameList = star.getDataFramesByCat(
	  star.DISTR_CONSTRAINT_STATS_SF_CAT,
	  star.DISTR_CONSTRAINT_STATS_CAT_NAME);

	_hasAverages = false;
	_hasViolations = false;
	_hasAmbiguous = false;
	int restraintIndex = 0;
	int frameIndex = 1;
        while (frameList.hasMoreElements()) {
	    SaveFrameNode frame = (SaveFrameNode)frameList.nextElement();

	    // Remediated restraints files have general_distance_constraints
	    // save frames that don't have the data we want...
	    if (!S2DStarUtil.loopExists(frame, star.DISTR_RESTRAINT_ID)) {
                if (doDebugOutput(11)) {
		    System.out.println("Skipping save frame " +
		      star.getFrameName(frame) + " because it has no " +
		      star.DISTR_RESTRAINT_ID + " values");
		}
	        continue;
	    }

	    String restraintType = null;
	    if (!star.DISTR_CONSTRAINT_TYPE.equals("")) {
	        restraintType = star.getOneFrameValueOrNull(frame,
	          star.DISTR_CONSTRAINT_TYPE);
            }
	    if (restraintType == null) restraintType = "unknown";

            String[] entityAssembly1Ids = star.getFrameValues(frame,
	      star.DISTR_RESTRAINT_ID, star.DISTR_ENT_ASSEM_1);
            String[] entityAssembly2Ids = star.getFrameValues(frame,
	      star.DISTR_RESTRAINT_ID, star.DISTR_ENT_ASSEM_2);

	    int restraintCount = entityAssembly1Ids.length;

	    String[] ambigCodes = null;
	    String[] ambigMemberIds = null;
	    if (!star.DISTR_MEMBER_LOGIC_CODE.equals("")) {
	        ambigCodes = star.getFrameValues(frame,
		  star.DISTR_RESTRAINT_ID, star.DISTR_MEMBER_LOGIC_CODE);
	        ambigMemberIds = star.getFrameValues(frame,
		  star.DISTR_RESTRAINT_ID, star.DISTR_MEMBER_ID);
	        _hasAmbiguous = true;
	    }

            String[] resSeq1Codes = star.getFrameValues(frame,
	      star.DISTR_RESTRAINT_ID, star.DISTR_RES_SEQ_CODE_1);
            String[] resSeq1Labels = star.getFrameValues(frame,
	      star.DISTR_RESTRAINT_ID, star.DISTR_RES_SEQ_LABEL_1);

            String[] resSeq2Codes = star.getFrameValues(frame,
	      star.DISTR_RESTRAINT_ID, star.DISTR_RES_SEQ_CODE_2);
            String[] resSeq2Labels = star.getFrameValues(frame,
	      star.DISTR_RESTRAINT_ID, star.DISTR_RES_SEQ_LABEL_2);

            String[] lowerBounds = star.getFrameValues(frame,
	      star.DISTR_RESTRAINT_ID, star.DISTR_MIN);
            lowerBounds = S2DUtils.arrayDot2Zero(lowerBounds);

            String[] upperBounds = star.getFrameValues(frame,
	      star.DISTR_RESTRAINT_ID, star.DISTR_MAX);
            upperBounds = S2DUtils.arrayDot2Zero(upperBounds);

            String[] atom1Names = star.getFrameValues(frame,
	      star.DISTR_RESTRAINT_ID, star.DISTR_ATOM_ID_1);

            String[] atom2Names = star.getFrameValues(frame,
	      star.DISTR_RESTRAINT_ID, star.DISTR_ATOM_ID_2);

	    String[] averageValues;
	    if (!star.DISTR_AVERAGE.equals("")) {
                averageValues = star.getFrameValues(frame,
	          star.DISTR_RESTRAINT_ID, star.DISTR_AVERAGE);
                averageValues = S2DUtils.arrayDot2Zero(averageValues);
	        _hasAverages = true;
	    } else {
                averageValues = S2DUtils.createStringArray(
		  restraintCount, "0.0");
	    }

	    String[] maxViolations;
	    if (!star.DISTR_MAX_VIOLATION.equals("")) {
                maxViolations = star.getFrameValues(frame,
	          star.DISTR_RESTRAINT_ID, star.DISTR_MAX_VIOLATION);
                maxViolations = S2DUtils.arrayDot2Zero(maxViolations);
	        _hasViolations = true;
	    } else {
                maxViolations = S2DUtils.createStringArray(
		  restraintCount, "0.0");
	    }

	    //
            // Calculate various values for each restraint in this
	    // save frame.
	    //
	    for (int index = 0; index < restraintCount; index++) {
		RestraintInfo restraintInfo = new RestraintInfo();

		restraintInfo._entityAssembly1Id =
		  entityAssembly1Ids[index];
		restraintInfo._resSeq1Code =
		  resSeq1Codes[index];
		restraintInfo._resSeq1Label =
		  resSeq1Labels[index];
		restraintInfo._entityAssembly2Id =
		  entityAssembly2Ids[index];
		restraintInfo._resSeq2Code =
		  resSeq2Codes[index];
		restraintInfo._resSeq2Label =
		  resSeq2Labels[index];

		restraintInfo._restraintType = restraintType;

		restraintInfo._atom1Id = S2DUtils.createAtomId(
		  entityAssembly1Ids[index], resSeq1Codes[index],
		  atom1Names[index]);
		restraintInfo._atom2Id = S2DUtils.createAtomId(
		  entityAssembly2Ids[index], resSeq2Codes[index],
		  atom2Names[index]);

		restraintInfo._atom1Name = atom1Names[index];
		restraintInfo._atom2Name = atom2Names[index];

		restraintInfo._lowerBound = lowerBounds[index];
		restraintInfo._upperBound = upperBounds[index];
		restraintInfo._averageValue = averageValues[index];
		restraintInfo._maxViolation = maxViolations[index];

		//
		// Get the per-residue data structure(s).
		//
	        ResidueInfo residue1Info = getResidueInfo(
		  entityAssembly1Ids[index], resSeq1Codes[index]); 
		ResidueInfo residue2Info = null;
		if (entityAssembly1Ids[index].equals(
		  entityAssembly2Ids[index]) &&
		  resSeq1Codes[index].equals(resSeq2Codes[index])) {
		    // This is a dummy that we'll throw away; just doing
		    // this here to avoid ifs in code below.
		    residue2Info = new ResidueInfo();
		} else {
	            residue2Info = getResidueInfo(entityAssembly2Ids[index],
		      resSeq2Codes[index]); 
		}

		//
		// Figure out whether the restraint is violated.
		//
		try {
		    double maxViolation =
		      Double.parseDouble(restraintInfo._maxViolation);
		    if (maxViolation > 0.0) {
		        restraintInfo._isViolated = true;
		    }
		} catch (NumberFormatException ex) {
		    System.err.println(new S2DWarning(
		      "Exception parsing maxViolation value: " +
		      ex.toString()));
		}

		//
		// Figure out whether the restraint is ambiguous.
		//
		if (ambigCodes != null) {
		    if (ambigCodes[index].equals("OR")) {
		        restraintInfo._isAmbiguous = true;
		    }
		}

		// Make sure ambiguous restraints only get counted once.
		if (ambigMemberIds == null ||
		  ambigMemberIds[index].equals("1")) {
		    residue1Info._all.updateCounts(restraintInfo);
		    residue2Info._all.updateCounts(restraintInfo);
		}

		//
                // Determine restraint length types and update the
		// counts for the relevant length.
		//
                findLengthType(entityAssembly1Ids[index],
		  entityAssembly2Ids[index], resSeq1Codes[index],
		  resSeq2Codes[index], restraintInfo, residue1Info,
		  residue2Info);

		//
		// Deal with pseudoatoms.
		//
		Vector atom1List = _pseudoAtoms.get(
		  restraintInfo._atom1Name, resSeq1Labels[index]);
		Vector atom2List = _pseudoAtoms.get(
		  restraintInfo._atom2Name, resSeq2Labels[index]);
		if (atom1List == null && atom2List == null) {
		    // No pseudo-atoms.
		    _restraints.add(restraintIndex, restraintInfo);
	            restraintIndex++;
		} else {
		    restraintIndex = processPseudoAtoms(restraintIndex,
		      restraintInfo, atom1List, atom2List,
		      entityAssembly1Ids[index], resSeq1Codes[index],
		      entityAssembly2Ids[index], resSeq2Codes[index]);
		}
	    }

	    frameIndex++;
    	}

	if (frameIndex == 1) {
	    throw new S2DError("No distance restraint save frames found");
	}
    }

    //-------------------------------------------------------------------
    // Write the distance restraint values for this data.
    public void writeRestraints(int frameIndex, boolean isRR)
      throws S2DException
    {
        if (doDebugOutput(11)) {
	    System.out.println("S2DDistRestraint.writeRestraints()");
	}

	try {
	    //
	    // Write the actual data.
	    //
	    String suffix = isRR ? S2DNames.RRDISTR_SUFFIX :
	      S2DNames.DISTR_SUFFIX;
            FileWriter distWriter = S2DFileWriter.create(_dataDir +
	      File.separator + _name + suffix + frameIndex +
	      S2DNames.DAT_SUFFIX);

	    // Write header.
	    distWriter.write("# Data: distance restraints for " +
	      _name + "\n");
	    distWriter.write("# Schema: bmrb-DistanceRestraint\n");
	    distWriter.write("# Attributes: Entity_assembly1_ID; " +
	      "Residue1_seq_code; Residue1_label; EA_res1; " +
	      "Entity_assembly2_ID; Residue2_seq_code; Residue2_label; " +
	      "EA_res2; Distance_lower_bound; Distance_upper_bound; " +
	      "atom1Id; atom1Name; atom2Id; atom2Name; restraintType; " +
	      "restraintLengthType; averageValue; maxViolation; " +
	      "isViolated; isAmbiguous\n");
            distWriter.write("# Peptide-CGI version: " +
	      S2DMain.PEP_CGI_VERSION + "\n");
            distWriter.write("# Generation date: " +
	      S2DMain.getTimestamp() + "\n");
	    distWriter.write("#\n");

	    for (int index = 0; index < _restraints.size(); index++) {
	        RestraintInfo restraintInfo =
		  (RestraintInfo)_restraints.elementAt(index);
		distWriter.write(restraintInfo.toString() + "\n");
	    }

	    distWriter.close();

	    //
	    // Write per-residue data.
	    //
	    suffix = isRR ? S2DNames.RRDISTRC_SUFFIX :
	      S2DNames.DISTRC_SUFFIX;
            distWriter = S2DFileWriter.create(_dataDir +
	      File.separator + _name + suffix + frameIndex +
	      S2DNames.DAT_SUFFIX);

	    // Write header.
	    distWriter.write("# Data: distance restraints for " +
	      _name + "\n");
	    distWriter.write("# Schema: bmrb-DistRestraintCounts\n");
	    distWriter.write("# Attributes: Entity_assembly_ID; " +
	      "Residue_seq_code; EA_res; All_viol-all_ambig-all; " +
	      "All_viol-yes_ambig-all; All_viol-no_ambig-all; " +
	      "All_viol-all_ambig-yes; All_viol-yes_ambig-yes; " +
	      "All_viol-no_ambig-yes; All_viol-all_ambig-no; " +
	      "All_viol-yes_ambig-no; All_viol-no_ambig-no;  " +
	      "Intra-res_viol-all_ambig-all; Intra-res_viol-yes_ambig-all; " +
	      "Intra-res_viol-no_ambig-all; Intra-res_viol-all_ambig-yes; " +
	      "Intra-res_viol-yes_ambig-yes; Intra-res_viol-no_ambig-yes; " +
	      "Intra-res_viol-all_ambig-no; Intra-res_viol-yes_ambig-no; " +
	      "Intra-res_viol-no_ambig-no; Sequential_viol-all_ambig-all; " +
	      "Sequential_viol-yes_ambig-all; Sequential_viol-no_ambig-all; " +
	      "Sequential_viol-all_ambig-yes; Sequential_viol-yes_ambig-yes; " +
	      "Sequential_viol-no_ambig-yes; Sequential_viol-all_ambig-no; " +
	      "Sequential_viol-yes_ambig-no; Sequential_viol-no_ambig-no; " +
	      "Medium_viol-all_ambig-all; Medium_viol-yes_ambig-all; " +
	      "Medium_viol-no_ambig-all; Medium_viol-all_ambig-yes; " +
	      "Medium_viol-yes_ambig-yes; Medium_viol-no_ambig-yes; " +
	      "Medium_viol-all_ambig-no; Medium_viol-yes_ambig-no; " +
	      "Medium_viol-no_ambig-no; Long_viol-all_ambig-all; " +
	      "Long_viol-yes_ambig-all; Long_viol-no_ambig-all; " +
	      "Long_viol-all_ambig-yes; Long_viol-yes_ambig-yes; " +
	      "Long_viol-no_ambig-yes; Long_viol-all_ambig-no; " +
	      "Long_viol-yes_ambig-no; Long_viol-no_ambig-no; " +
	      "Inter-mol_viol-all_ambig-all; Inter-mol_viol-yes_ambig-all; " +
	      "Inter-mol_viol-no_ambig-all; Inter-mol_viol-all_ambig-yes; " +
	      "Inter-mol_viol-yes_ambig-yes; Inter-mol_viol-no_ambig-yes; " +
	      "Inter-mol_viol-all_ambig-no; Inter-mol_viol-yes_ambig-no; " +
	      "Inter-mol_viol-no_ambig-no\n");
            distWriter.write("# Peptide-CGI version: " +
	      S2DMain.PEP_CGI_VERSION + "\n");
            distWriter.write("# Generation date: " +
	      S2DMain.getTimestamp() + "\n");
	    distWriter.write("#\n");

	    // Go through the _entityAssemblies hash table to write the actual
	    // data.
            //TEMP -- this is not output in sorted order
	    Enumeration entities = _entityAssemblies.keys();
	    while (entities.hasMoreElements()) {
		String entity = (String)entities.nextElement();
	        Hashtable entityHT = (Hashtable)_entityAssemblies.get(entity);
		Enumeration residues = entityHT.keys();
		while (residues.hasMoreElements()) {
		    String residue = (String)residues.nextElement();
		    ResidueInfo residueInfo = getResidueInfo(entity, residue);

		    distWriter.write(entity + " " +
		      residue + " " +
		      entity + "_" + residue + " " +
		      residueInfo.toString() +
		      "\n");
		}
	    }

	    distWriter.close();

	    //
	    // Write the meta-data.
	    //
	    suffix = isRR ? S2DNames.RRDISTR_MD_SUFFIX :
	      S2DNames.DISTR_MD_SUFFIX;
            distWriter = S2DFileWriter.create(_dataDir + File.separator +
	      _name + suffix + frameIndex + S2DNames.DAT_SUFFIX);

	    distWriter.write("# Data: distance restraint meta-data for " +
	      _name + "\n");
	    distWriter.write("# Schema: bmrb-DistRestraintMeta\n");
            distWriter.write("# Attributes: Entity_assembly_name; " +
	      "Entity_assembly_ID; All_or_violated; All_or_ambiguous " +
	      "Restraint_length_type; ChildViewYAttribute; " +
	      "Viol_opt_index; Ambig_opt_index; Length_opt_index; Title\n");
            distWriter.write("# Peptide-CGI version: " +
	      S2DMain.PEP_CGI_VERSION + "\n");
            distWriter.write("# Generation date: " +
	      S2DMain.getTimestamp() + "\n");
	    distWriter.write("#\n");

	    // Get a sorted list of the entity assembly IDs we have
	    // data for.
            Vector entityAssemblyIds = new Vector();
	    Enumeration keys = _entityAssemblies.keys();
	    while (keys.hasMoreElements()) {
	        entityAssemblyIds.add(keys.nextElement());
	    }
	    Collections.sort(entityAssemblyIds, Collator.getInstance());

	    // Now set up a list of the different options for violations
	    // and length types.
	    Vector options = new Vector();
	    addOptions(options, VIOLATED_ALL, AMBIGUOUS_ALL);
	    if (_hasViolations) {
	        addOptions(options, VIOLATED_YES, AMBIGUOUS_ALL);
	        addOptions(options, VIOLATED_NO, AMBIGUOUS_ALL);
		if (_hasAmbiguous) {
	            addOptions(options, VIOLATED_YES, AMBIGUOUS_YES);
	            addOptions(options, VIOLATED_NO, AMBIGUOUS_YES);
	            addOptions(options, VIOLATED_YES, AMBIGUOUS_NO);
	            addOptions(options, VIOLATED_NO, AMBIGUOUS_NO);
		}
	    }
	    if (_hasAmbiguous) {
	        addOptions(options, VIOLATED_ALL, AMBIGUOUS_YES);
	        addOptions(options, VIOLATED_ALL, AMBIGUOUS_NO);
	    }

	    for (int index = 0; index < entityAssemblyIds.size(); index++) {
		String eANum = (String)entityAssemblyIds.elementAt(index);
	        String eAName = "EA " + eANum;

		for (int optIndex = 0; optIndex < options.size();
		  optIndex++) {
		    Option option = (Option)options.elementAt(optIndex);
                    distWriter.write(eAName + "\t" +
		      eANum + "\t" +
		      option._violOpt + "\t" +
		      option._ambigOpt + "\t" +
		      option._lengthOpt + "\t" +
		      option._yAttr + "\t" +
		      option._violOptIndex + "\t" +
		      option._ambigOptIndex + "\t" +
		      option._lengthOptIndex + "\t" +
		      "Restraints per residue (" + option._title +
		        " [" + eANum + "])" +
		      "\n");
		}
	    }

	    distWriter.close();

	    //
	    // Write the session files.
	    //
	    int type = isRR ? S2DUtils.TYPE_RRDIST_RESTR :
	      S2DUtils.TYPE_DIST_RESTR;
	    String info = "Visualization of " + _longName;
	    if (_pdbId != null) {
	        info += " and PDB " + _pdbId;
	    }
	    S2DSession.write(_sessionDir, type, _name, frameIndex,
	      info, null, true, _starVersion);

	    //
	    // Write the session-specific html files.
	    //
	    String title = isRR ? STR_REMEDIATED_RESTRANTS :
	      STR_RESTRANTS_GRID;
	    S2DSpecificHtml specHtml = new S2DSpecificHtml(
	      _summary.getHtmlDir(), type, _name, frameIndex,
	      title, null);
	    specHtml.write();

	    //
	    // Write the link in the summary html file.
	    //
	    _summary.writeDistRestraint(_pdbId, frameIndex, isRR);

        } catch(IOException ex) {
	    System.err.println(
	      "IOException writing distance restraint values: " +
	      ex.toString());
	    throw new S2DError("Can't write distance restraint values");
	}
    }

    //===================================================================
    // PRIVATE METHODS

    //-------------------------------------------------------------------
    // Get the ResidueInfo object for the given entityAssemblyID and
    // resSeqCode, creating one if we don't have one yet.
    private ResidueInfo getResidueInfo(String entityAssemblyID,
      String resSeqCode)
    {
        Hashtable entityHT =
	  (Hashtable)_entityAssemblies.get(entityAssemblyID);
	if (entityHT == null) {
	    entityHT = new Hashtable();
	    _entityAssemblies.put(entityAssemblyID, entityHT);
	}

	ResidueInfo residueInfo = (ResidueInfo)entityHT.get(resSeqCode);
	if (residueInfo == null) {
	    residueInfo = new ResidueInfo();
	    entityHT.put(resSeqCode, residueInfo);
	}

	return residueInfo;
    }

    //-------------------------------------------------------------------
    // Determine the restraint length type for the given restraint
    // information, and update restraintInfo, residue1Info, and
    // residue2Info accordingly.
    //TEMP -- make this just use the values in restraintInfo?
    private static void findLengthType(String entityAssembly1Id,
      String entityAssembly2Id, String resSeq1Code,
      String resSeq2Code, RestraintInfo restraintInfo,
      ResidueInfo residue1Info, ResidueInfo residue2Info)
    {
	//
	// Determine restraint length types.
	//
	if (entityAssembly1Id.equals(entityAssembly2Id)) {
	    try {
	        int resSeq1CodeInt = Integer.parseInt(resSeq1Code);
	        int resSeq2CodeInt = Integer.parseInt(resSeq2Code);
	        int diff = Math.abs(resSeq1CodeInt - resSeq2CodeInt);
	        if (diff == 0) {
	            restraintInfo._restraintLengthType = "Intra-residue";
		    residue1Info._intraRes.updateCounts(restraintInfo);
		    residue2Info._intraRes.updateCounts(restraintInfo);
	        } else if (diff == 1) {
	            restraintInfo._restraintLengthType = "Sequential";
		    residue1Info._sequential.updateCounts(restraintInfo);
		    residue2Info._sequential.updateCounts(restraintInfo);
	        } else if (diff < 5) {
	            restraintInfo._restraintLengthType = "Medium-range";
		    residue1Info._medium.updateCounts(restraintInfo);
		    residue2Info._medium.updateCounts(restraintInfo);
	        } else {
	            restraintInfo._restraintLengthType = "Long-range";
		    residue1Info._long.updateCounts(restraintInfo);
		    residue2Info._long.updateCounts(restraintInfo);
	        }
	    } catch (NumberFormatException ex) {
	        System.err.println(new S2DWarning("Exception parsing int " +
		  "(resSeq1Code or resSeq2Code): " + ex.toString()));
	        restraintInfo._restraintLengthType = "unknown";
	    }
	} else {
	    restraintInfo._restraintLengthType = "Intermolecular";
	    residue1Info._interMol.updateCounts(restraintInfo);
	    residue2Info._interMol.updateCounts(restraintInfo);
	}
    }

    //-------------------------------------------------------------------
    //
    private int processPseudoAtoms(int restraintIndex,
      RestraintInfo restraintInfo, Vector atom1List, Vector atom2List,
      String entityAssembly1Id, String resSeq1Code,
      String entityAssembly2Id, String resSeq2Code)
    {
        if (atom1List == null) {
	    atom1List = new Vector();
	    atom1List.add(restraintInfo._atom1Name);
	}

	if (atom2List == null) {
	    atom2List = new Vector();
	    atom2List.add(restraintInfo._atom2Name);
	}
	
	for (int index1 = 0; index1 < atom1List.size(); index1++) {
	    for (int index2 = 0; index2 < atom2List.size(); index2++) {
	        RestraintInfo tmpResInfo = new RestraintInfo(restraintInfo);

		// Modify the RestraintInfo object to contain the
		// "real" atoms...
		tmpResInfo._atom1Name = (String)atom1List.elementAt(index1);
		tmpResInfo._atom1Id = S2DUtils.createAtomId(
		  entityAssembly1Id, resSeq1Code, tmpResInfo._atom1Name);

		tmpResInfo._atom2Name = (String)atom2List.elementAt(index2);
		tmpResInfo._atom2Id = S2DUtils.createAtomId(
		  entityAssembly2Id, resSeq2Code, tmpResInfo._atom2Name);

		_restraints.add(restraintIndex, tmpResInfo);
		restraintIndex++;
	    }
	}

	return restraintIndex;
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

    //-------------------------------------------------------------------
    // Class to hold info for one meta-data option (combiation of
    // selections for violated/non-violated/all, ambiguous/non-ambiguous/
    // all, intra-residue/etc/all).
    private class Option {
        public String _violOpt;
        public String _ambigOpt;
        public String _lengthOpt;
        public String _yAttr;
        public int _violOptIndex;
        public int _ambigOptIndex;
        public int _lengthOptIndex;
        public String _title; // not including entity assembly ID
        Option(String violOpt, String ambigOpt, String lengthOpt,
	  String yAttr, int violOptIndex, int ambigOptIndex,
	  int lengthOptIndex, String title) {
            _violOpt = violOpt;
            _ambigOpt = ambigOpt;
            _lengthOpt = lengthOpt;
            _yAttr = yAttr;
            _violOptIndex = violOptIndex;
            _ambigOptIndex = ambigOptIndex;
            _lengthOptIndex = lengthOptIndex;
            _title = title;
        }
    }
    
    private static final int VIOLATED_ALL = 0;
    private static final int VIOLATED_YES = 1;
    private static final int VIOLATED_NO = 2;

    private static final int AMBIGUOUS_ALL = 0;
    private static final int AMBIGUOUS_YES = 1;
    private static final int AMBIGUOUS_NO = 2;

    //-------------------------------------------------------------------
    private void addOptions(Vector options, int violated, int ambiguous)
      throws S2DException
    {
	String violatedStr = "";
	String violatedAttr = "";
        switch (violated) {
        case VIOLATED_ALL:
	    violatedStr = "All";
	    violatedAttr = "viol-all";
	    break;

        case VIOLATED_YES:
	    violatedStr = "Violated";
	    violatedAttr = "viol-yes";
	    break;

        case VIOLATED_NO:
	    violatedStr = "Non-violated";
	    violatedAttr = "viol-no";
	    break;

	default:
	    throw new S2DError("Illegal violated value: " + violated);
	}

	String ambiguousStr = "";
	String ambigAttr = "";
        switch (ambiguous) {
        case AMBIGUOUS_ALL:
	    ambiguousStr = "All";
	    ambigAttr = "ambig-all";
	    break;

        case AMBIGUOUS_YES:
	    ambiguousStr = "Ambiguous";
	    ambigAttr = "ambig-yes";
	    break;

        case AMBIGUOUS_NO:
	    ambiguousStr = "Non-ambiguous";
	    ambigAttr = "ambig-no";
	    break;

	default:
	    throw new S2DError("Illegal ambiguous value: " + ambiguous);
	}
	
	String attrName;
	String length;
	String title;

        //TEMP -- move some of this stuff into the Option constructor?
	attrName = "$All";
	attrName += "_" + violatedAttr + "_" + ambigAttr;
	length = "All";
	title = length;
	if (violated != VIOLATED_ALL) {
	    title += " " + violatedStr.toLowerCase();
	}
	if (ambiguous != AMBIGUOUS_ALL) {
	    title += " " + ambiguousStr.toLowerCase();
	}
	options.add(new Option(violatedStr, ambiguousStr, length,
	  attrName, violated, ambiguous, 0, title));

	attrName = "$Intra-res";
	attrName += "_" + violatedAttr + "_" + ambigAttr;
	length = "Intra-residue";
	title = length;
	if (violated != VIOLATED_ALL) {
	    title += " " + violatedStr.toLowerCase();
	}
	if (ambiguous != AMBIGUOUS_ALL) {
	    title += " " + ambiguousStr.toLowerCase();
	}
	options.add(new Option(violatedStr, ambiguousStr, length, attrName,
	  violated, ambiguous, 1, title));

	attrName = "$Sequential";
	attrName += "_" + violatedAttr + "_" + ambigAttr;
	length = "Sequential";
	title = length;
	if (violated != VIOLATED_ALL) {
	    title += " " + violatedStr.toLowerCase();
	}
	if (ambiguous != AMBIGUOUS_ALL) {
	    title += " " + ambiguousStr.toLowerCase();
	}
	options.add(new Option(violatedStr, ambiguousStr, length, attrName,
	  violated, ambiguous, 2, title));

	attrName = "$Medium";
	attrName += "_" + violatedAttr + "_" + ambigAttr;
	length = "Medium-range";
	title = length;
	if (violated != VIOLATED_ALL) {
	    title += " " + violatedStr.toLowerCase();
	}
	if (ambiguous != AMBIGUOUS_ALL) {
	    title += " " + ambiguousStr.toLowerCase();
	}
	options.add(new Option(violatedStr, ambiguousStr, length, attrName,
	  violated, ambiguous, 3, title));

	attrName = "$Long";
	attrName += "_" + violatedAttr + "_" + ambigAttr;
	length = "Long-range";
	title = length;
	if (violated != VIOLATED_ALL) {
	    title += " " + violatedStr.toLowerCase();
	}
	if (ambiguous != AMBIGUOUS_ALL) {
	    title += " " + ambiguousStr.toLowerCase();
	}
	options.add(new Option(violatedStr, ambiguousStr, length, attrName,
	  violated, ambiguous, 4, title));

	attrName = "$Inter-mol";
	attrName += "_" + violatedAttr + "_" + ambigAttr;
	length = "Intermolecular";
	title = length;
	if (violated != VIOLATED_ALL) {
	    title += " " + violatedStr.toLowerCase();
	}
	if (ambiguous != AMBIGUOUS_ALL) {
	    title += " " + ambiguousStr.toLowerCase();
	}
	options.add(new Option(violatedStr, ambiguousStr, length, attrName,
	  violated, ambiguous, 5, title));
    }
}

// ========================================================================

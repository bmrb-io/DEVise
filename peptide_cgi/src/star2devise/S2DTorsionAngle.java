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

// This class implements the output of torsion angle restraint data.
// For each set of torsion angle data, it creates a data file, a session
// file, an individual html file, and a link in the summary html file.

// Note: the code that deals with the restraint grid web site should
// be cleaned up and use a real html parser...

// ------------------------------------------------------------------------

// $Id$

// $Log$
// Revision 1.10.8.1  2011/09/21 20:46:10  wenger
// The s2predicted session movie buttons are now updated for the correct
// PDB ID -- I should probably have configuration for the URL, though,
// and also only show the button if the movie is available.
//
// Revision 1.10  2010/12/07 17:41:16  wenger
// Did another version history purge.
//
// Revision 1.9  2010/07/15 21:37:33  wenger
// Fixed Peptide-CGI bug 111.
//
// Revision 1.8  2010/04/23 16:51:09  wenger
// Merged s2d_dist_rest_1002_br_0 thru s2d_dist_rest_1002_br_1 to trunk.
//
// Revision 1.7  2010/03/10 22:36:17  wenger
// Added NMR-STAR file version to summary html page and detailed
// visualization version info (to-do 072).  (Doing this before I
// add multiple NMR-STAR paths so we can see which NMR-STAR file
// was used.)
//
// ...

// ========================================================================

package star2devise;

import EDU.bmrb.starlibj.SaveFrameNode;
import java.io.*;
import java.util.*;
import java.net.*;
import java.text.Collator;

public class S2DTorsionAngle extends S2DRestraint {
    //===================================================================
    // VARIABLES

    private static final int DEBUG = 0;

    public static final String STR_RESTRANTS_GRID = "Torsion angle " +
      "restraints from restraints grid (violations calculated by CING)";
    public static final String STR_REMEDIATED_RESTRANTS = "Torsion angle " +
      "restraints from remediated restraints";

    //
    // Note: we're keeping a lot of numerical values as Strings here, since
    // we're just spitting them back out as strings, so we avoid two
    // conversions.  wenger 2009-12-04.
    //
    private String[] _entityAssembly1Ids;
    private String[] _entityAssembly2Ids;
    private String[] _entityAssembly3Ids;
    private String[] _entityAssembly4Ids;
    private String[] _angleEntityAssemblyIds;

    private String[] _resSeq1Codes;
    private String[] _resSeq1Labels;
    private String[] _resSeq2Codes;
    private String[] _resSeq2Labels;
    private String[] _resSeq3Codes;
    private String[] _resSeq3Labels;
    private String[] _resSeq4Codes;
    private String[] _resSeq4Labels;
    private String[] _angleResSeqCodes;
    private String[] _angleResSeqLabels;

    private String[] _lowerBounds;
    private String[] _upperBounds;

        // combination of entity assembly ID, residue # and atom name
    private String[] _atom1Ids;
    private String[] _atom1Names;

        // combination of entity assembly ID, residue # and atom name
    private String[] _atom2Ids;
    private String[] _atom2Names;

        // combination of entity assembly ID, residue # and atom name
    private String[] _atom3Ids;
    private String[] _atom3Names;

        // combination of entity assembly ID, residue # and atom name
    private String[] _atom4Ids;
    private String[] _atom4Names;

    private String[] _angleTypes;
    private boolean _hasAverages;
    private String[] _averageValues;
    private boolean _hasViolations;
    private String[] _maxViolations;

    // A list of unique entity assembly IDs found in the data.
    private Vector _entityAssemblyIds;

    //===================================================================
    // PUBLIC METHODS

    //-------------------------------------------------------------------
    // Constructor.
    public S2DTorsionAngle(String name, String longName,
      S2DNmrStarRestIfc star, String dataDir, String sessionDir,
      S2DSummaryHtml summary, String pdbId) throws S2DException
    {
	super(name, longName, star, dataDir, sessionDir, summary, pdbId);

        if (doDebugOutput(11)) {
	    System.out.println("S2DTorsionAngle.S2DTorsionAngle(" + name + ")");
	}

        _entityAssemblyIds = new Vector();

	Enumeration frameList = star.getDataFramesByCat(
	  star.TAR_CONSTRAINT_STATS_SF_CAT,
	  star.TAR_CONSTRAINT_STATS_CAT_NAME);

	int frameIndex = 1;
        while (frameList.hasMoreElements()) {
	    if (frameIndex > 1) {
		System.err.println(new S2DWarning("Only one save frame " +
		  "per torsion angle file is currently handled"));
	        break;
	    }

	    SaveFrameNode frame = (SaveFrameNode)frameList.nextElement();

            _entityAssembly1Ids = star.getFrameValues(frame,
	      star.TAR_RESTRAINT_ID, star.TAR_ENT_ASSEM_1);
            _entityAssembly2Ids = star.getFrameValues(frame,
	      star.TAR_RESTRAINT_ID, star.TAR_ENT_ASSEM_2);
            _entityAssembly3Ids = star.getFrameValues(frame,
	      star.TAR_RESTRAINT_ID, star.TAR_ENT_ASSEM_3);
            _entityAssembly4Ids = star.getFrameValues(frame,
	      star.TAR_RESTRAINT_ID, star.TAR_ENT_ASSEM_4);

	    int restraintCount = _entityAssembly1Ids.length;

            _resSeq1Codes = star.getFrameValues(frame,
	      star.TAR_RESTRAINT_ID, star.TAR_RES_SEQ_CODE_1);
            _resSeq1Labels = star.getFrameValues(frame,
	      star.TAR_RESTRAINT_ID, star.TAR_RES_SEQ_LABEL_1);

            _resSeq2Codes = star.getFrameValues(frame,
	      star.TAR_RESTRAINT_ID, star.TAR_RES_SEQ_CODE_2);
            _resSeq2Labels = star.getFrameValues(frame,
	      star.TAR_RESTRAINT_ID, star.TAR_RES_SEQ_LABEL_2);

            _resSeq3Codes = star.getFrameValues(frame,
	      star.TAR_RESTRAINT_ID, star.TAR_RES_SEQ_CODE_3);
            _resSeq3Labels = star.getFrameValues(frame,
	      star.TAR_RESTRAINT_ID, star.TAR_RES_SEQ_LABEL_3);

            _resSeq4Codes = star.getFrameValues(frame,
	      star.TAR_RESTRAINT_ID, star.TAR_RES_SEQ_CODE_4);
            _resSeq4Labels = star.getFrameValues(frame,
	      star.TAR_RESTRAINT_ID, star.TAR_RES_SEQ_LABEL_4);

            _lowerBounds = star.getFrameValues(frame,
	      star.TAR_RESTRAINT_ID, star.TAR_ANGLE_LOWER_BOUND);
            _lowerBounds = S2DUtils.arrayDot2Zero(_lowerBounds);

            _upperBounds = star.getFrameValues(frame,
	      star.TAR_RESTRAINT_ID, star.TAR_ANGLE_UPPER_BOUND);
            _upperBounds = S2DUtils.arrayDot2Zero(_upperBounds);

            _atom1Names = star.getFrameValues(frame,
	      star.TAR_RESTRAINT_ID, star.TAR_ATOM_ID_1);

            _atom2Names = star.getFrameValues(frame,
	      star.TAR_RESTRAINT_ID, star.TAR_ATOM_ID_2);

            _atom3Names = star.getFrameValues(frame,
	      star.TAR_RESTRAINT_ID, star.TAR_ATOM_ID_3);

            _atom4Names = star.getFrameValues(frame,
	      star.TAR_RESTRAINT_ID, star.TAR_ATOM_ID_4);

	    if (!star.TAR_ANGLE_AVERAGE.equals("")) {
                _averageValues = star.getFrameValues(frame,
	          star.TAR_RESTRAINT_ID, star.TAR_ANGLE_AVERAGE);
                _averageValues = S2DUtils.arrayDot2Zero(_averageValues);
	        _hasAverages = true;
	    } else {
                _averageValues = S2DUtils.createStringArray(
		  restraintCount, "0.0");
	        _hasAverages = false;
	    }

	    if (!star.TAR_MAX_VIOLATION.equals("")) {
                _maxViolations = star.getFrameValues(frame,
	          star.TAR_RESTRAINT_ID, star.TAR_MAX_VIOLATION);
                _maxViolations = S2DUtils.arrayDot2Zero(_maxViolations);
	        _hasViolations = true;
	    } else {
                _maxViolations = S2DUtils.createStringArray(
		  restraintCount, "0.0");
	        _hasViolations = false;
	    }

            _atom1Ids = new String[restraintCount];
	    for (int index = 0; index < restraintCount; index++) {
	        _atom1Ids[index] = S2DUtils.createAtomId(
		  _entityAssembly1Ids[index], _resSeq1Codes[index],
		  _atom1Names[index]);
	    }

            _atom2Ids = new String[restraintCount];
	    for (int index = 0; index < restraintCount; index++) {
	        _atom2Ids[index] = S2DUtils.createAtomId(
		  _entityAssembly2Ids[index], _resSeq2Codes[index],
		  _atom2Names[index]);
	    }

            _atom3Ids = new String[restraintCount];
	    for (int index = 0; index < restraintCount; index++) {
	        _atom3Ids[index] = S2DUtils.createAtomId(
		  _entityAssembly3Ids[index], _resSeq3Codes[index],
		  _atom3Names[index]);
	    }

            _atom4Ids = new String[restraintCount];
	    for (int index = 0; index < restraintCount; index++) {
	        _atom4Ids[index] = S2DUtils.createAtomId(
		  _entityAssembly4Ids[index], _resSeq4Codes[index],
		  _atom4Names[index]);
	    }

	    //
            // Determine angle types.
	    //

	    // Values will get assigned to these according to the type of angle.
            _angleEntityAssemblyIds = new String[restraintCount];
            _angleResSeqCodes = new String[restraintCount];
            _angleResSeqLabels = new String[restraintCount];

            _angleTypes = new String[restraintCount];
	    for (int index = 0; index < restraintCount; index++) {
		_angleTypes[index] = null;
	        if (_atom1Names[index].equals("C")) {
		    if (_atom2Names[index].equals("N") &&
		      _atom3Names[index].equals("CA") &&
		      _atom4Names[index].equals("C")) {
		        _angleTypes[index] = "phi";
                        _angleEntityAssemblyIds[index] =
			  _entityAssembly2Ids[index];
                        _angleResSeqCodes[index] = _resSeq2Codes[index];
                        _angleResSeqLabels[index] = _resSeq2Labels[index];
		    }

		} else if (_atom1Names[index].equals("N")) {
		    if (_atom2Names[index].equals("CA")) {
		        if (_atom3Names[index].equals("CB")) {
			    _angleTypes[index] = "chi-1";
                            _angleEntityAssemblyIds[index] =
			      _entityAssembly2Ids[index];
                            _angleResSeqCodes[index] = _resSeq2Codes[index];
                            _angleResSeqLabels[index] = _resSeq2Labels[index];

			} else if (_atom3Names[index].equals("C") &&
			  _atom4Names[index].equals("N")) {
			    _angleTypes[index] = "psi";
                            _angleEntityAssemblyIds[index] =
			      _entityAssembly2Ids[index];
                            _angleResSeqCodes[index] = _resSeq2Codes[index];
                            _angleResSeqLabels[index] = _resSeq2Labels[index];
			}
		    }

		} else if (_atom1Names[index].equals("CA") &&
		  _atom2Names[index].equals("C") &&
		  _atom3Names[index].equals("N") &&
		  _atom4Names[index].equals("CA")) {
		    _angleTypes[index] = "omega";
		    // From http://spin.niddk.nih.gov/NMRPipe/ref/tclfn/dyngetomega.html:
		    // Omega(i) Torsion is Defined as: CA(i-1) C(i-1) N(i) CA(i)
                    _angleEntityAssemblyIds[index] =
		      _entityAssembly3Ids[index];
                    _angleResSeqCodes[index] = _resSeq3Codes[index];
                    _angleResSeqLabels[index] = _resSeq3Labels[index];
		}

		if (_angleTypes[index] == null) {
		    // If we get here, we fell through the above ifs
		    // without assigning an angle type.
		    _angleTypes[index] = "other";
		    // Not sure which residue should get assigned here...
                    _angleEntityAssemblyIds[index] =
		      _entityAssembly2Ids[index];
                    _angleResSeqCodes[index] = _resSeq2Codes[index];
                    _angleResSeqLabels[index] = _resSeq2Labels[index];
		}

	        if (!_entityAssemblyIds.contains(
	          _angleEntityAssemblyIds[index])) {
	            _entityAssemblyIds.add(_angleEntityAssemblyIds[index]);
	        }
	    }

	    frameIndex++;
    	}

	if (frameIndex == 1) {
	    throw new S2DError("No torsion angle save frames found");
	}
    }

    //-------------------------------------------------------------------
    // Write the torsion angle restraint values for this data.
    public void writeRestraints(int frameIndex, boolean isRR)
      throws S2DException
    {
        if (doDebugOutput(11)) {
	    System.out.println("S2DTorsionAngle.writeRestraints()");
	}

	try {
	    //
	    // Write the actual data.
	    //
	    String suffix = isRR ? S2DNames.RRTAR_SUFFIX :
	      S2DNames.TAR_SUFFIX;
            FileWriter tarWriter = S2DFileWriter.create(_dataDir +
	      File.separator + _name + suffix + frameIndex +
	      S2DNames.DAT_SUFFIX);

	    // Write header.
	    tarWriter.write("# Data: torsion angle restraints for " +
	      _name + "\n");
	    tarWriter.write("# Schema: bmrb-TorsionAngle\n");
	    tarWriter.write("# Attributes: Entity_assembly_ID; " +
	      "Residue_seq_code; Residue_label; " +
	      "Torsion_angle_lower_bound; " +
	      "Torsion_angle_upper_bound; atom1Id; " +
	      "atom1Name; atom2Id; atom2Name; atom3Id; atom3Name; " +
	      "atom4Id; atom4Name; angleType; averageValue; " +
	      "maxViolation\n");
            tarWriter.write("# Peptide-CGI version: " +
	      S2DMain.PEP_CGI_VERSION + "\n");
            tarWriter.write("# Generation date: " +
	      S2DMain.getTimestamp() + "\n");
	    tarWriter.write("#\n");

	    for (int index = 0; index < _entityAssembly1Ids.length; index++) {
	        tarWriter.write(_angleEntityAssemblyIds[index] + " " +
		  _angleResSeqCodes[index] + " " +
		  _angleResSeqLabels[index] + " " +
		  _lowerBounds[index] + " " +
		  _upperBounds[index] + " " +
		  _atom1Ids[index] + " " +
		  _atom1Names[index] + " " +
		  _atom2Ids[index] + " " +
		  _atom2Names[index] + " " +
		  _atom3Ids[index] + " " +
		  _atom3Names[index] + " " +
		  _atom4Ids[index] + " " +
		  _atom4Names[index] + " " +
		  _angleTypes[index] + " " +
		  _averageValues[index] + " " +
		  _maxViolations[index] + " " +
		  "\n");
	    }

	    tarWriter.close();

	    //
	    // Write the meta-data.
	    //
	    suffix = isRR ? S2DNames.RRTAR_MD_SUFFIX :
	      S2DNames.TAR_MD_SUFFIX;
            tarWriter = S2DFileWriter.create(_dataDir + File.separator +
	      _name + suffix + frameIndex + S2DNames.DAT_SUFFIX);

	    tarWriter.write("# Data: torsion angle meta-data for " +
	      _name + "\n");
	    tarWriter.write("# Schema: bmrb-tar-meta\n");
            tarWriter.write("# Attributes: Entity_assembly_name; " +
	      "Entity_assembly_ID; name; angle_type; " +
	      "ViewUnselectedDataChild; ViewSelectedDataChild; " +
	      "ViewDataDotsChild; ChildViewYAttribute\n");
            tarWriter.write("# Peptide-CGI version: " +
	      S2DMain.PEP_CGI_VERSION + "\n");
            tarWriter.write("# Generation date: " +
	      S2DMain.getTimestamp() + "\n");
	    tarWriter.write("#\n");

	    String upLowAvg = _hasAverages ? "lower/upper/avg" : "lower/upper";

	    // Sort by entity assembly ID (fixes bug 091).
	    Collections.sort(_entityAssemblyIds, Collator.getInstance());

	    for (int index = 0; index < _entityAssemblyIds.size(); index++) {
		String eANum = (String)_entityAssemblyIds.elementAt(index);
	        String eAName = "EA " + eANum;//TEMP -- add type of structure here

	        // Phi angle.
                tarWriter.write(eAName + "\t" +
	          eANum + "\t" +
	          "Phi angle " + upLowAvg + " [" + eANum + "]\t" +
	          "phi\t" +
                  "ViewAngleUnselectedDataChild\t" +
	          "ViewAngleSelectedDataChild\t" +
	          "ViewAngleDataDotsChild\t" +
	          "$averageValue\n");

		if (_hasViolations) {
                    tarWriter.write(eAName + "\t" +
	              eANum + "\t" +
	              "Phi angle violation [" + eANum + "]\t" +
	              "phi\t" +
                      "ViewViolationUnselectedDataChild\t" +
	              "ViewViolationSelectedDataChild\t" +
	              "ViewViolationDataDotsChild\t" +
	              "$maxViolation\n");
	        }

	        // Psi angle.
                tarWriter.write(eAName + "\t" +
	          eANum + "\t" +
	          "Psi angle " + upLowAvg + " [" + eANum + "]\t" +
	          "psi\t" +
                  "ViewAngleUnselectedDataChild\t" +
	          "ViewAngleSelectedDataChild\t" +
	          "ViewAngleDataDotsChild\t" +
	          "$averageValue\n");

		if (_hasViolations) {
                    tarWriter.write(eAName + "\t" +
	              eANum + "\t" +
	              "Psi angle violation [" + eANum + "]\t" +
	              "psi\t" +
                      "ViewViolationUnselectedDataChild\t" +
	              "ViewViolationSelectedDataChild\t" +
	              "ViewViolationDataDotsChild\t" +
	              "$maxViolation\n");
	        }

	        // Omega angle.
                tarWriter.write(eAName + "\t" +
	          eANum + "\t" +
	          "Omega angle " + upLowAvg + " [" + eANum + "]\t" +
	          "omega\t" +
                  "ViewAngleUnselectedDataChild\t" +
	          "ViewAngleSelectedDataChild\t" +
	          "ViewAngleDataDotsChild\t" +
	          "$averageValue\n");

		if (_hasViolations) {
                    tarWriter.write(eAName + "\t" +
	              eANum + "\t" +
	              "Omega angle violation [" + eANum + "]\t" +
	              "omega\t" +
                      "ViewViolationUnselectedDataChild\t" +
	              "ViewViolationSelectedDataChild\t" +
	              "ViewViolationDataDotsChild\t" +
	              "$maxViolation\n");
	        }

	        // Chi-1 angle.
                tarWriter.write(eAName + "\t" +
	          eANum + "\t" +
	          "Chi-1 angle " + upLowAvg + " [" + eANum + "]\t" +
	          "chi-1\t" +
                  "ViewAngleUnselectedDataChild\t" +
	          "ViewAngleSelectedDataChild\t" +
	          "ViewAngleDataDotsChild\t" +
	          "$averageValue\n");

		if (_hasViolations) {
                    tarWriter.write(eAName + "\t" +
	              eANum + "\t" +
	              "Chi-1 angle violation [" + eANum + "]\t" +
	              "chi-1\t" +
                      "ViewViolationUnselectedDataChild\t" +
	              "ViewViolationSelectedDataChild\t" +
	              "ViewViolationDataDotsChild\t" +
	              "$maxViolation\n");
	        }

	        // Other angle.
                tarWriter.write(eAName + "\t" +
	          eANum + "\t" +
	          "Other angle " + upLowAvg + " [" + eANum + "]\t" +
	          "other\t" +
                  "ViewAngleUnselectedDataChild\t" +
	          "ViewAngleSelectedDataChild\t" +
	          "ViewAngleDataDotsChild\t" +
	          "$averageValue\n");

		if (_hasViolations) {
                    tarWriter.write(eAName + "\t" +
	              eANum + "\t" +
	              "Other angle violation [" + eANum + "]\t" +
	              "other\t" +
                      "ViewViolationUnselectedDataChild\t" +
	              "ViewViolationSelectedDataChild\t" +
	              "ViewViolationDataDotsChild\t" +
	              "$maxViolation\n");
	        }
	    }

	    tarWriter.close();

	    //
	    // Write the session files.
	    //
	    int type = isRR ? S2DUtils.TYPE_RRTORSION_ANGLE :
	      S2DUtils.TYPE_TORSION_ANGLE;
	    String info = "Visualization of " + _longName;
	    if (_pdbId != null) {
	        info += " and PDB " + _pdbId;
	    }
	    S2DSession.write(_sessionDir, type, _name, frameIndex,
	      info, null, true, _starVersion, "");

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
	    _summary.writeTorsionAngle(_pdbId, frameIndex, isRR);

        } catch(IOException ex) {
	    System.err.println(
	      "IOException writing torsion angle restraint values: " +
	      ex.toString());
	    throw new S2DError("Can't write TAR values");
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

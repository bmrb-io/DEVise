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
// Revision 1.3.2.1  2010/01/12 22:40:53  wenger
// Added a bunch more tests for PDB-only torsion angle restraints, and
// finished the partially-completed ones I already had.
//
// Revision 1.3  2010/01/06 23:03:40  wenger
// Merged s2d_dist_rest_0912_br_0 thru s2d_dist_rest_0912_br_1 to trunk.
//
// Revision 1.2.2.3  2010/01/05 15:33:52  wenger
// Cleaned up the pdbIdToUrl() methods for various restraint-related
// classes.
//
// Revision 1.2.2.2  2009/12/16 00:07:56  wenger
// Added S2DDistRestraint and S2DNmrStarDistRIfc classes (mostly
// stubs so far); added a bunch of notes based on today's BMRB
// staff meeting discussions; also added stubbed-in classes in
// S2DSummaryHtml, etc.
//
// Revision 1.2.2.1  2009/12/14 23:31:03  wenger
// Added new S2DRestraint class to be parent of S2DTorsionAngle and
// (not yet created) S2DDistRestraint class; moved a bunch of the
// code from S2DTorsionAngle to S2DRestraint.
//
// Revision 1.2  2009/12/05 22:26:32  wenger
// Merged s2d_torsion_rest_0910_br_0 thru s2d_torsion_rest_0910_br_0
// to the trunk.
//
// Revision 1.1.4.27  2009/12/05 21:45:39  wenger
// Hopefully final cleanup before merge(!).
//
// Revision 1.1.4.26  2009/12/05 21:17:03  wenger
// Converted dots to zeros in numerical torsion angle data so DEVise
// can read the data correctly.
//
// Revision 1.1.4.25  2009/12/05 20:46:21  wenger
// Mostly finished dealing correctly with multiple entity assemblies --
// required changes to torsion angle meta-data, and a new link in the
// session template.
//
// Revision 1.1.4.24  2009/12/05 18:44:49  wenger
// Angles are now assigned to correct residues.
//
// Revision 1.1.4.23  2009/12/05 00:30:24  wenger
// Partway to getting angles assigned to correct residues -- fixed
// atom/residue pairings, removed some unused fields from the torsion
// angle output.
//
// Revision 1.1.4.22  2009/12/02 21:38:46  wenger
// Added the new TAR_LEVEL_LINK_CHECK processing level, and made it
// the default; partially implemented passing the torsion angle URL
// thru the CGI script arguments as an optimization.  Added the capability
// of running some subsets of the tests in test_all.
//
// Revision 1.1.4.21  2009/12/01 23:25:25  wenger
// Cleaned up a bunch of the temporary code for finding the torsion
// angle violations in the restraint grid; tested it at Osaka and
// Florence, added torsion-angle-related items to config files.
//
// Revision 1.1.4.20  2009/12/01 20:48:03  wenger
// Real code for getting torsion angle data by PDB ID is now in
// place -- still lots of temporary code within that, though.
//
// Revision 1.1.4.19  2009/12/01 18:02:07  wenger
// Torsion angle data can now be retreived by URL from the restraint grid.
//
// Revision 1.1.4.18  2009/12/01 15:56:57  wenger
// Got filtering of the html file returned by the restraint grid working;
// actually getting that file from the restraint grid via the URL isn't
// working yet (specifying a URL to a local file does work).
//
// Revision 1.1.4.17  2009/11/30 21:47:50  wenger
// Fixed title, etc., in html pages for torsion angle visualizations.
//
// Revision 1.1.4.16  2009/11/30 19:58:48  wenger
// Fixed visualization info for the torsion angle sessions; removed
// now-obsolete session template for separate violation visualization.
//
// Revision 1.1.4.15  2009/11/30 18:09:01  wenger
// Got rid of sessions and specific html pages for (now obsolete) torsion
// angle violation visualizations.
//
// Revision 1.1.4.14  2009/11/30 17:54:03  wenger
// Changed torsion angle links in summary page to reflect angle/violation
// unification.
//
// Revision 1.1.4.13  2009/11/30 17:09:16  wenger
// Changed order of torsion angle meta-data so that violations appear
// just below the corresponding lower/upper/average values.
//
// Revision 1.1.4.12  2009/11/26 00:30:47  wenger
// Did some cleaning up of the torsion angle meta-data.
//
// Revision 1.1.4.11  2009/11/25 22:46:05  wenger
// Okay, I think I have the "unified" visualization working.  I should
// do some cleaning up of names in the session template, though, to match
// the scheme I ended up with.  I also have to take out the creation of
// separate session, links, etc., for violations.
//
// Revision 1.1.4.10  2009/11/25 22:00:51  wenger
// Working on unifying the "angle" and "violation" visualizations, using
// two sets of child views -- one for the angles and one for the violations.
// So far I have things pretty much working except for the 3D highlighting.
// I think I can fix that by using the same child view for the highlight
// links leader view, and switching the Y attribute with the parent view.
// I've tagged things s2d_torsion_rest_0910_br_pre-unification before
// this commit, in case I have to back out these changes.
//
// Revision 1.1.4.9  2009/11/24 23:59:03  wenger
// Added the S2DNmrStarTarIfc to properly deal with the torsion angle-
// related tags.  Note: 3D tests fail because they're not yet updated
// for the new Atom_ID field.
//
// Revision 1.1.4.8  2009/11/24 16:22:34  wenger
// Got rid of a bunch of temporary notes that are no longer needed.
//
// Revision 1.1.4.7  2009/11/17 23:25:19  wenger
// Writing of torsion angle sessions, etc., now mostly working (links in
// summary page are still not right, lots of other cleanup, finding
// data in restraint grid still needed).
//
// Revision 1.1.4.6  2009/10/29 20:06:43  wenger
// For torsion angles, the session files and specific html files are
// now created; there are some cursor behavior problems in the session
// files that need to be fixed, and also they are not loading right
// in the JavaScreen (I wonder if I need to have some kind of different
// setup in the JS client to handle the different highlighting).
// (There also seem to be problems with DEVise drawing the high/low
// symbols in some cases!)
//
// Revision 1.1.4.5  2009/10/23 19:49:42  wenger
// Got torsion_angle_violation.base session working with
// auto-generated data.
//
// Revision 1.1.4.4  2009/10/22 20:26:14  wenger
// Pretty much got the torsion_angle.base session working with auto-
// generated data (the session is modified from the p4020ta1.ds
// prototype session).  Note that the angle type selection works
// differently (via GAttr links rather than data source changes;
// because of that, the parent/child view setup is only retained
// for the sake of changing the view titles.
//
// Revision 1.1.4.3  2009/10/22 18:28:31  wenger
// Early phases of setting up "atom IDs" (entity assembly ID/residue
// sequence code/atom name) values to facilitate linking of torsion
// angle data to coordinate data.
//
// Revision 1.1.4.2  2009/10/22 17:11:31  wenger
// "Bounced" s2d_torsion_rest_0909_br off the trunk -- created new
// s2d_torsion_rest_0910_br, merged s2d_torsion_rest_0909_br_0 thru
// s2d_torsion_rest_0909_br_end to the new branch.
//
// Revision 1.1.2.4  2009/10/22 15:45:08  wenger
// Changed restraint data file to the filtered FRED file to get
// violations, etc.; I think the right data is now spit out except
// for the atom numbers.
//
// Revision 1.1.2.3  2009/10/21 22:33:35  wenger
// More partial changes -- committing again because I think maybe
// I *still* have the wrong restraints file...
//
// Revision 1.1.2.2  2009/10/21 20:24:12  wenger
// Fixed the 1brv_ta.str file -- I was using the wrong version before;
// we output some real data in S2DTorsionAngle.
//
// Revision 1.1.2.1  2009/10/08 16:20:24  wenger
// Partway along the processing of torsion angle restraints -- just
// committing for safety.
//

// ========================================================================

package star2devise;

import EDU.bmrb.starlibj.SaveFrameNode;
import java.io.*;
import java.util.*;
import java.net.*;

public class S2DTorsionAngle extends S2DRestraint {
    //===================================================================
    // VARIABLES

    private static final int DEBUG = 0;

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
    private String[] _averageValues;
    private String[] _maxViolations;

    private Vector _entityAssemblyIds;

    //===================================================================
    // PUBLIC METHODS

    //-------------------------------------------------------------------
    // Constructor.
    public S2DTorsionAngle(String name, String longName,
      S2DNmrStarTarIfc star, String dataDir, String sessionDir,
      S2DSummaryHtml summary, String pdbId) throws S2DException
    {
	super(name, longName, dataDir, sessionDir, summary, pdbId);

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

            _averageValues = star.getFrameValues(frame,
	      star.TAR_RESTRAINT_ID, star.TAR_ANGLE_AVERAGE);
            _averageValues = S2DUtils.arrayDot2Zero(_averageValues);

            _maxViolations = star.getFrameValues(frame,
	      star.TAR_RESTRAINT_ID, star.TAR_MAX_VIOLATION);
            _maxViolations = S2DUtils.arrayDot2Zero(_maxViolations);

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

		} else {
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
    }

    //-------------------------------------------------------------------
    // Write the torsion angle restraint values for this data.
    public void writeRestraints(int frameIndex) throws S2DException
    {
        if (doDebugOutput(11)) {
	    System.out.println("S2DTorsionAngle.writeRestraints()");
	}

	try {
	    //
	    // Write the actual data.
	    //
            FileWriter tarWriter = S2DFileWriter.create(_dataDir +
	      File.separator + _name + S2DNames.TAR_SUFFIX +
	      frameIndex + S2DNames.DAT_SUFFIX);

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
            tarWriter = S2DFileWriter.create(_dataDir + File.separator +
	      _name + S2DNames.TAR_MD_SUFFIX + frameIndex +
	      S2DNames.DAT_SUFFIX);

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

	    for (int index = 0; index < _entityAssemblyIds.size(); index++) {
		String eANum = (String)_entityAssemblyIds.elementAt(index);
	        String eAName = "EA " + eANum;//TEMP -- add type of structure here

	        // Phi angle.
                tarWriter.write(eAName + "\t" +
	          eANum + "\t" +
	          "Phi angle lower/upper/avg\t" +
	          "phi\t" +
                  "ViewAngleUnselectedDataChild\t" +
	          "ViewAngleSelectedDataChild\t" +
	          "ViewAngleDataDotsChild\t" +
	          "$averageValue\n");

                tarWriter.write(eAName + "\t" +
	          eANum + "\t" +
	          "Phi angle violation\t" +
	          "phi\t" +
                  "ViewViolationUnselectedDataChild\t" +
	          "ViewViolationSelectedDataChild\t" +
	          "ViewViolationDataDotsChild\t" +
	          "$maxViolation\n");

	        // Psi angle.
                tarWriter.write(eAName + "\t" +
	          eANum + "\t" +
	          "Psi angle lower/upper/avg\t" +
	          "psi\t" +
                  "ViewAngleUnselectedDataChild\t" +
	          "ViewAngleSelectedDataChild\t" +
	          "ViewAngleDataDotsChild\t" +
	          "$averageValue\n");

                tarWriter.write(eAName + "\t" +
	          eANum + "\t" +
	          "Psi angle violation\t" +
	          "psi\t" +
                  "ViewViolationUnselectedDataChild\t" +
	          "ViewViolationSelectedDataChild\t" +
	          "ViewViolationDataDotsChild\t" +
	          "$maxViolation\n");

	        // Omega angle.
                tarWriter.write(eAName + "\t" +
	          eANum + "\t" +
	          "Omega angle lower/upper/avg\t" +
	          "omega\t" +
                  "ViewAngleUnselectedDataChild\t" +
	          "ViewAngleSelectedDataChild\t" +
	          "ViewAngleDataDotsChild\t" +
	          "$averageValue\n");

                tarWriter.write(eAName + "\t" +
	          eANum + "\t" +
	          "Omega angle violation\t" +
	          "omega\t" +
                  "ViewViolationUnselectedDataChild\t" +
	          "ViewViolationSelectedDataChild\t" +
	          "ViewViolationDataDotsChild\t" +
	          "$maxViolation\n");

	        // Chi-1 angle.
                tarWriter.write(eAName + "\t" +
	          eANum + "\t" +
	          "Chi-1 angle lower/upper/avg\t" +
	          "chi-1\t" +
                  "ViewAngleUnselectedDataChild\t" +
	          "ViewAngleSelectedDataChild\t" +
	          "ViewAngleDataDotsChild\t" +
	          "$averageValue\n");

                tarWriter.write(eAName + "\t" +
	          eANum + "\t" +
	          "Chi-1 angle violation\t" +
	          "chi-1\t" +
                  "ViewViolationUnselectedDataChild\t" +
	          "ViewViolationSelectedDataChild\t" +
	          "ViewViolationDataDotsChild\t" +
	          "$maxViolation\n");

	        // Other angle.
                tarWriter.write(eAName + "\t" +
	          eANum + "\t" +
	          "Other angle lower/upper/avg\t" +
	          "other\t" +
                  "ViewAngleUnselectedDataChild\t" +
	          "ViewAngleSelectedDataChild\t" +
	          "ViewAngleDataDotsChild\t" +
	          "$averageValue\n");

                tarWriter.write(eAName + "\t" +
	          eANum + "\t" +
	          "Other angle violation\t" +
	          "other\t" +
                  "ViewViolationUnselectedDataChild\t" +
	          "ViewViolationSelectedDataChild\t" +
	          "ViewViolationDataDotsChild\t" +
	          "$maxViolation\n");
	    }

	    tarWriter.close();

	    //
	    // Write the session files.
	    //
	    String info = "Visualization of " + _longName;
	    if (_pdbId != null) {
	        info += " and PDB " + _pdbId;
	    }
	    S2DSession.write(_sessionDir, S2DUtils.TYPE_TORSION_ANGLE,
	      _name, frameIndex, info, null);

	    //
	    // Write the session-specific html files.
	    //
	    S2DSpecificHtml specHtml = new S2DSpecificHtml(
	      _summary.getHtmlDir(), S2DUtils.TYPE_TORSION_ANGLE,
	      _name, frameIndex, "Torsion angle restraints", null);
	    specHtml.write();

	    //
	    // Write the link in the summary html file.
	    //
	    _summary.writeTorsionAngle(_pdbId, frameIndex);

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

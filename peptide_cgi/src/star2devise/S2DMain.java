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

// This class reads an NMR-Star file and generates data and session files
// to be used by DEVise, and a summary html page and individual html pages
// for each session.

// ------------------------------------------------------------------------

// $Id$

// $Log$
// Revision 1.207  2010/06/04 17:03:24  wenger
// Changed version to 11.8.2 for release.
//
// Revision 1.206  2010/06/02 16:06:01  wenger
// Reduced applet height in all HTML templates, and added info about
// switching sessions in the JS; increased the font size of the links
// to the visualization summary page and entry "main" page; slightly
// changed the link text.
//
// Revision 1.205  2010/06/01 15:31:07  wenger
// All visualization summary pages for released BMRB entries now have
// links to the "main" page for the entry.
//
// Revision 1.204  2010/06/01 15:00:01  wenger
// Implemented to-do 132 (links from individual visualization pages back
// to the summary pages).
//
// Revision 1.203  2010/05/28 14:26:32  wenger
// More fixes to set_modes script.
//
// Revision 1.202  2010/05/26 17:31:39  wenger
// We now delete existing sessions when doing the "main" processing for
// an entry, so the user can't switch to invalid sessions in the JavaScreen
// (since we're now enabling session-switching).
//
// Revision 1.201  2010/05/26 16:05:22  wenger
// Fixed problems with incorrect links to different-size visualization
// pages in visualization-specific html pages.
//
// Revision 1.200  2010/05/26 15:29:43  wenger
// Moved acdd files to entry-specific subdirectories and changed tests
// accordingly; fixed some problems with set_modes.
//
// Revision 1.199  2010/05/24 20:06:15  wenger
// Changed Peptide-CGI to put all sessions for a given entry in a
// subdirectory, and use the JSA instead of the JSB, so it's possible to
// switch between related sessions in the JavaScreen (see to-do 136).
// (Note: using the JSA is also a first step in allowing arbitrary
// re-sizing.)
//
// Revision 1.198  2010/05/10 21:24:49  wenger
// Fixed bug 097 (added Google Analytics scripts to histogram pages).
//
// Revision 1.197  2010/05/05 19:07:00  wenger
// Changed version to 11.8.2x1 and added 11.8.2 version history section.
//
// Revision 1.196  2010/05/05 14:50:58  wenger
// Changed version to 11.8.1 for release.
//
// Revision 1.195  2010/04/29 23:06:26  wenger
// Fixed tests for distance restraint data changes to fix bug 096.
//
// Revision 1.194  2010/04/29 22:39:32  wenger
// Fixed Peptide-CGI bug 096 (problem with distance restraint selection).
// Note that the tests aren't updated yet to correspond to the fixed data,
// and the distance restraint visualization generates DEVise error messages
// that don't seem to cause actual problems (see DEVise bug 1001).
//
// Revision 1.193  2010/04/27 18:47:07  wenger
// Fixed bugs Eldon found in testing of whether restraints info exists
// (for *_LEVEL_LINK_CHECK level of restraint processing).
//
// Revision 1.192  2010/04/26 17:32:07  wenger
// Incremented version and added new version history section.
//
// Revision 1.191  2010/04/23 21:16:03  wenger
// Changed version to 11.8.0 for release.
//
// Revision 1.190  2010/04/23 16:51:06  wenger
// Merged s2d_dist_rest_1002_br_0 thru s2d_dist_rest_1002_br_1 to trunk.
//
// Revision 1.189  2010/03/15 22:16:43  wenger
// Updated version to 11.7.5 and added new version history section.
//
// Revision 1.188  2010/03/15 20:58:40  wenger
// Minor change to SPARTA part of test procedure; version is 11.7.4
// for release.
//
// Revision 1.187  2010/03/12 20:57:34  wenger
// Changed x part of version.
//
// Revision 1.186  2010/03/10 22:36:16  wenger
// Added NMR-STAR file version to summary html page and detailed
// visualization version info (to-do 072).  (Doing this before I
// add multiple NMR-STAR paths so we can see which NMR-STAR file
// was used.)
//
// Revision 1.185.2.27  2010/04/22 23:32:06  wenger
// Fixed selection to work for ambiguous/non-ambiguous (forgot to add
// in the appropriate GAttr link in the session template before(!)).
//
// Revision 1.185.2.26  2010/04/22 18:49:59  wenger
// We now get the bmrb_mirror.do_rrdist_default property value; added
// appropriate values to the configuration.
//
// Revision 1.185.2.25  2010/04/22 16:38:10  wenger
// Various minor cleanups of distance restraint code.
//
// Revision 1.185.2.24  2010/04/21 21:03:05  wenger
// Largely fixed up tests for distance restraint processing -- still
// need to do some manual checking to make sure that values are
// correct; some minor cleanup of the distance restraint data
// files.
//
// Revision 1.185.2.23  2010/04/20 21:45:55  wenger
// Changed distance restraint data view title to include "Restraints per
// residue" so people have a better idea what is actually graphed.
//
// Revision 1.185.2.22  2010/04/20 19:22:59  wenger
// We now deal with pseudoatoms -- for restraints with pseudoatoms,
// we generate one record in the restraint data file for each combination
// of "real" atoms represented by pseudoatoms -- so we end up drawing
// a connection between all possible atoms in the 3D view.
//
// Revision 1.185.2.21  2010/04/19 21:00:15  wenger
// Changed distance restraint session template to allow selection
// by both violated/non-violated and ambiguous/non-ambiguous.
//
// Revision 1.185.2.20  2010/04/19 18:35:43  wenger
// Cleaned up the distance restraint meta-data-writing code in preparation
// for allowing selection by both violated/non-violated and ambiguous/
// non-ambiguous.
//
// Revision 1.185.2.19  2010/04/19 16:17:21  wenger
// Slight session template changes to get coloring restraints by ambiguous/
// non-ambiguous working; slight fixes to some of the distance restraint
// test scripts.
//
// Revision 1.185.2.18  2010/04/19 14:49:48  wenger
// Changed session code to match previous change in distance restraint
// session template (coord_index changed from 2 to 1).
//
// Revision 1.185.2.17  2010/04/08 19:49:48  wenger
// Oops -- forgot to change coordinates data source from coord_index 2
// to 1; changed main 3D view name to include 'wireframe' for JS client.
//
// Revision 1.185.2.16  2010/04/08 18:45:41  wenger
// Temporarily changed the Jmol page templates to use cmdport 6266 to
// run with the 'test' DEVise server; also changed torsion angle
// and distance restrain visualizations to use the Jmol version of the
// html templates (which should have been done before).
//
// Revision 1.185.2.15  2010/04/07 21:28:00  wenger
// Simplified restraint-counting code in preparation for dealing with
// *combinations* of violated and ambiguous restraints; changed coord_index
// in test_dist3rgi and test_dist3rri to match the entry.
//
// Revision 1.185.2.14  2010/04/01 15:40:52  wenger
// Oops -- forgot to switch the distance restraint template back to 3D
// views; also added atom IDs to the highlight view mapping so they get
// passed to the JS.
//
// Revision 1.185.2.13  2010/03/09 20:36:47  wenger
// Added ambiguous and non-ambiguous restraint counts to data.
//
// Revision 1.185.2.12  2010/03/09 00:14:09  wenger
// Got rid of the data_pile_supplement_link -- it seems like having
// the combination of multiple visual links (with the actual pile
// link) is what's causing problems with the data view Y axis.
// (This disadvantage of getting rid of this link is that if we
// unpile things, the views won't be properly linked anymore.)
//
// Revision 1.185.2.11  2010/03/08 22:50:53  wenger
// Fixed titles in distance restraint data views.
//
// Revision 1.185.2.10  2010/03/08 22:23:38  wenger
// Fixed cache checking for distance restraints; added some debug output
// to make_view and make_uvd.
//
// Revision 1.185.2.9  2010/03/08 21:33:33  wenger
// Fixed display of non-violated restraints (got rid of expression in
// parent value, and did the actual calculation in the Peptide-CGI code --
// see DEVise bug 999).
//
// Revision 1.185.2.8  2010/03/08 19:16:24  wenger
// Now writing distance restraint links in summary html pages.
//
// Revision 1.185.2.7  2010/03/05 23:12:36  wenger
// Creation of distance restraint session files is now (at least mostly)
// working.
//
// Revision 1.185.2.6  2010/03/03 17:44:41  wenger
// Fixed the fact that coordinates from the remediated restraints file
// weren't getting processed for distance restraints.
//
// Revision 1.185.2.5  2010/02/25 21:03:39  wenger
// First partial implementation of writing out per-residue distance
// restraint data.
//
// Revision 1.185.2.4  2010/02/24 22:37:03  wenger
// Implemented basic distance restraint processing of remediated
// restraints files, added a test using remediated restraints via
// the BMRB web site.
//
// Revision 1.185.2.3  2010/02/24 20:03:06  wenger
// Got basic generation of distance restraint data from the restraints
// grid mostly working.
//
// Revision 1.185.2.2  2010/02/24 16:35:20  wenger
// Torsion angle restraints now work with new S2DNmrStarRGIfc and
// S2DNmrStarRGIfcFactory classes.
//
// Revision 1.185.2.1  2010/02/24 16:03:15  wenger
// More work on distance restraints: added -do_rrdist argument, partially
// completed code to get distance restraints URL from restraints grid;
// committing in preparation for combining S2DNmrStarDistRIfc and
// S2DNmrStarTarIfc into a single S2DNmrStarRGIfc class.
//
// Revision 1.185  2010/02/23 20:04:28  wenger
// Changed version to 11.7.4x1; added 11.7.4 version history section.
//
// Revision 1.184  2010/02/22 22:41:06  wenger
// Changed version to 11.7.3 for release.
//
// Revision 1.183  2010/02/20 00:18:36  wenger
// Finished getting SPARTA processing to work with multiple entity
// assemblies (to-do 117) and multiple chemical shift lists per entity
// assembly (to-do 118); updated test_sparta 7 and test_sparta8 accordingly.
//
// Revision 1.182  2010/02/17 23:48:20  wenger
// Added checking to test_sparta7; fixed a couple of bugs in the SPARTA
// code.
//
// Revision 1.181  2010/02/17 21:58:12  wenger
// Changed the code and older test files to match the latest SPARTA
// file versions.
//
// Revision 1.180  2010/02/17 21:24:40  wenger
// Fixed some errors in changing strings in the SPARTA session files.
//
// Revision 1.179  2010/02/17 18:48:41  wenger
// Fixed bug 093 (incorrect entity assembly IDs in 3D data sets).
//
// Revision 1.178  2010/02/16 23:49:27  wenger
// Fixed bugs 091 and 092 (problems with torsion angle visualizations).
//
// Revision 1.177  2010/02/15 19:35:31  wenger
// At least mostly fixed bug 090 (by forcing the data_select_cursor to
// be within the destination view's visual filter); *maybe* we want to
// add a data_select_fix_cursor like the 3D sessions (hell, I'm not
// even sure exactly what that's doing any more).  Also fixed a bug that
// I introduced into make_view, and added a new link in uvd_works.html
// that produces torsion angle visualizations.
//
// Revision 1.176  2010/02/11 22:13:10  wenger
// Merged s2d_remediated_rest_1002_br_0 thru s2d_remediated_rest_1002_br_1
// to trunk (note: s2d_remediated_rest_1002_br_1 ==
// s2d_remediated_rest_1002_br_end).
//
// Revision 1.175.2.18  2010/02/11 20:28:08  wenger
// Added some tests of the make_view and make_uvd scripts, fixed a bug in
// make_view that this found...
//
// Revision 1.175.2.17  2010/02/11 17:30:07  wenger
// Minor changes to make sure remediated restraint coordinate processing
// doesn't overwrite "regular" coordinate session, etc.
//
// Revision 1.175.2.16  2010/02/11 16:53:32  wenger
// Added cache checking for remediated restraint files.
//
// Revision 1.175.2.15  2010/02/10 20:25:15  wenger
// Put remediated restraint stuff into config files, etc.
//
// Revision 1.175.2.14  2010/02/10 18:33:39  wenger
// Oops -- fixed logic bug in the remediated restraint coordinate
// processing.
//
// Revision 1.175.2.13  2010/02/10 16:23:39  wenger
// Removed a bunch of temporary stuff.
//
// Revision 1.175.2.12  2010/02/09 22:44:16  wenger
// Renamed process1RRPDB() to process1RRCoordSet() to make the
// functionality clearer.
//
// Revision 1.175.2.11  2010/02/09 22:41:20  wenger
// Mostly done getting coordinates from remediated restraint files --
// seems to work, but still needs some checking.
//
// Revision 1.175.2.10  2010/02/05 22:51:37  wenger
// Implemented to-do item 115: PDB IDs differing only by case are now
// considered the same (implemented by internally changing all PDB IDs
// to upper case).
//
// Revision 1.175.2.9  2010/02/05 20:46:12  wenger
// Partially implemented getting remediated restraints file template from
// properties.
//
// Revision 1.175.2.8  2010/02/04 18:38:46  wenger
// Got PDB-only calculations working with remediated restraints; got
// CGI links in summary page for remediated working.
//
// Revision 1.175.2.7  2010/02/04 00:08:27  wenger
// Now writing remediated restraint links in summary page -- CGI links
// don't seem to work right yet.
//
// Revision 1.175.2.6  2010/02/03 23:13:19  wenger
// Torsion angle output from remediated restraints files now has different
// suffixes to avoid conflict with the restraint grid output; meta-data
// for remediated restraints doesn't have violations
//
// Revision 1.175.2.5  2010/02/03 22:02:47  wenger
// First version of S2DNmrStarRRIfc with actual tag values -- processing
// seems to mostly work.
//
// Revision 1.175.2.4  2010/02/03 21:38:58  wenger
// Made S2DNmrStarDistRIfc, S2DNmrStarRRIfc, and S2DNmrStarTarIfc
// subclasses of new S2DNmrStarRestIfc class, so that I can pass
// either a S2DNmrStarTarIfc or a S2DNmrStarRRIfc to the
// S2DTorsionAngle constructor.
//
// Revision 1.175.2.3  2010/02/03 20:57:00  wenger
// Blocked in code for getting remediated restraints from a specified file.
//
// Revision 1.175.2.2  2010/02/03 17:59:19  wenger
// Added some extra checking for legal processing levels.
//
// Revision 1.175.2.1  2010/02/03 16:46:34  wenger
// Started adding arguments and tests for remediated restraint torsion
// angle processing.
//
// Revision 1.175  2010/01/22 23:42:18  wenger
// Changed version to 11.7.3x1 and added 11.7.3 version history section.
//
// Revision 1.174  2010/01/22 20:45:35  wenger
// Changed version to 11.7.2 for release.
//
// Revision 1.173  2010/01/21 16:32:15  wenger
// Merged s2d_pdb_only_tar_1001_br_0 thru s2d_pdb_only_tar_1001_br_end
// to trunk.
//
// Revision 1.172.2.4  2010/01/19 19:07:29  wenger
// Minor cleanups of PDB-only processing code.
//
// Revision 1.172.2.3  2010/01/08 23:28:32  wenger
// Fixed 'Force reprocessing' functionality for restraint-only summary
// page; fixed error message in make_view for restraint-only processing.
//
// Revision 1.172.2.2  2010/01/08 16:29:29  wenger
// Added changes to make the summary html file have only the relevant
// things for restraint-only processing.
//
// Revision 1.172.2.1  2010/01/07 23:26:10  wenger
// First cut at "PDB-only" torsion angle restraint processing -- basically
// just cut out some steps for this case.  It *seems* to work okay -- comes
// up with a visualization that works, but I want to do some more checking
// before I say it's ready.
//
// Revision 1.172  2010/01/06 23:03:39  wenger
// Merged s2d_dist_rest_0912_br_0 thru s2d_dist_rest_0912_br_1 to trunk.
//
// Revision 1.171  2009/12/23 15:37:10  wenger
// Changed version to 11.7.1x2, added new version history section.
//
// Revision 1.170  2009/12/22 19:40:30  wenger
// Changed version to 11.7.1 for release.
//
// Revision 1.169  2009/12/22 16:16:41  wenger
// Updated SPARTA test files to the latest version.
//
// Revision 1.168.2.10  2010/01/06 22:21:56  wenger
// Did a bunch of cleanups on the distance restraint code, in preparation
// for merging it to the trunk, so I can suspend work on it and move to
// working on the "PDB-ID-only" torsion angle restraint visualizations.
//
// Revision 1.168.2.9  2010/01/05 15:33:52  wenger
// Cleaned up the pdbIdToUrl() methods for various restraint-related
// classes.
//
// Revision 1.168.2.8  2010/01/04 23:59:17  wenger
// Finished moving creation of all S2DNmrStar*Ifc objects into the new
// factory classes.
//
// Revision 1.168.2.7  2010/01/04 19:31:06  wenger
// Made most S2DNmrStarIfcFactory methods non-static in preparation for
// splitting it up into various subclasses as appropriate.
//
// Revision 1.168.2.6  2010/01/04 18:56:59  wenger
// Added new S2DNmrStarIfcFactory class as part 1 of cleaning up the
// creation of various S2D*Ifc objects.
//
// Revision 1.168.2.5  2009/12/29 23:51:40  wenger
// Removed some debug code; added comments about improving the organization
// of of the factory methods for NMRStar*Ifc classes.
//
// Revision 1.168.2.4  2009/12/29 22:46:10  wenger
// Added property and code to get default distance restraint level;
// moved the usage message code to after we get properties so default
// values in usage message are correct.
//
// Revision 1.168.2.3  2009/12/29 21:52:21  wenger
// Added command-line arguments related to distance restraints; fixed
// some errors in the usage message I found in the course of doing that.
// Added skeleton of first distance restraint test.
//
// Revision 1.168.2.2  2009/12/16 00:07:56  wenger
// Added S2DDistRestraint and S2DNmrStarDistRIfc classes (mostly
// stubs so far); added a bunch of notes based on today's BMRB
// staff meeting discussions; also added stubbed-in classes in
// S2DSummaryHtml, etc.
//
// Revision 1.168.2.1  2009/12/14 23:31:03  wenger
// Added new S2DRestraint class to be parent of S2DTorsionAngle and
// (not yet created) S2DDistRestraint class; moved a bunch of the
// code from S2DTorsionAngle to S2DRestraint.
//
// Revision 1.168  2009/12/11 23:25:54  wenger
// Deleted a bunch of obsolete config files, changed various settings to
// new paths (new location for histogram data, new location for NMR-STAR
// files, BMRB gets PDB files locally, Osaka gets PDB files from PDBj, etc.).
//
// Revision 1.167  2009/12/10 20:35:05  wenger
// Added DEVise/JavaScreen tutorial video links to html pages we generate.
//
// Revision 1.166  2009/12/10 00:01:37  wenger
// Got the new -pdb_file argument working -- test works, just needs a bit
// more manual checking to make sure everything is okay.
//
// Revision 1.165  2009/12/07 21:43:55  wenger
// Changed version to 11.7.1x1, added new version history section.
//
// Revision 1.164  2009/12/07 17:44:19  wenger
// Changed version to 11.7.0 for release.
//
// Revision 1.163  2009/12/05 22:26:31  wenger
// Merged s2d_torsion_rest_0910_br_0 thru s2d_torsion_rest_0910_br_0
// to the trunk.
//
// Revision 1.162  2009/11/20 21:18:29  wenger
// Changed version to 11.6.4x1, new version history section, small change to
// release document.
//
// Revision 1.161  2009/11/20 16:56:13  wenger
// Set version to 11.6.3 for release; updated testing document.
//
// Revision 1.160  2009/11/16 19:57:55  wenger
// Added atom_coord_jmol_cursor_fix.base, which has a possible change
// to make the data cursor behavior better.
//
// Revision 1.159  2009/11/12 23:04:22  wenger
// New 3.0 SPARTA files from Jon, updated tests accordingly.
//
// Revision 1.158  2009/10/28 19:25:42  wenger
// Finished fixing things up for the new 3.0 SPARTA format (except that
// the actual SPARTA output is still incorrect, so the tests are kind
// of kludged for now -- they should be fixed when the actual SPARTA
// output is fixed).
//
// Revision 1.157  2009/10/22 17:26:34  wenger
// Refactored the experimental chemical shifts to match the new-style
// Sparta processing where we get the appropriate data values in
// the appropriate chemical shift-related class(es) rather than in
// S2DMain.
//
// Revision 1.156.2.25  2009/12/05 21:45:39  wenger
// Hopefully final cleanup before merge(!).
//
// Revision 1.156.2.24  2009/12/04 22:19:51  wenger
// Implemented cache checking for torsion angle restraints.
//
// Revision 1.156.2.23  2009/12/04 18:55:45  wenger
// Forced Y axis range for "angle" views to always be -180 to 180, as
// per request from Eldon.
//
// Revision 1.156.2.22  2009/12/04 16:47:53  wenger
// Added a property for default torsion angle restraint processing
// level; started on torsion angle help page.
//
// Revision 1.156.2.21  2009/12/02 22:43:44  wenger
// Added the new TAR_LEVEL_LINK_CHECK processing level, and made it
// the default; partially implemented passing the torsion angle URL
// thru the CGI script arguments as an optimization.  Added the capability
// of running some subsets of the tests in test_all; and added a couple
// more torsion angle tests (dealing with multiple entity assemblies,
// and "other" angles).
//
// Revision 1.156.2.20  2009/12/02 21:38:45  wenger
// Added the new TAR_LEVEL_LINK_CHECK processing level, and made it
// the default; partially implemented passing the torsion angle URL
// thru the CGI script arguments as an optimization.  Added the capability
// of running some subsets of the tests in test_all.
//
// Revision 1.156.2.19  2009/12/02 19:18:12  wenger
// Improved the CGI methods for summary html pages to use named constants
// from S2DMain for the values for do_pdb, do_csr, and do_tar; added
// processing time note for torsion angle links.
//
// Revision 1.156.2.18  2009/12/02 18:32:45  wenger
// Cleaned up the mixing of coordinate/torsion angle code in S2DMain;
// "all-in-one" processing now works (see test_tar2p).
//
// Revision 1.156.2.17  2009/12/02 17:26:53  wenger
// The torsion angle CGI links now work for the "standard" NMR Browser
// setup (haven't tested it yet with the visualization server).
// A bunch of clean up still needed...  Added a new test for "all-in-one"
// processing, but that doesn't work yet.
//
// Revision 1.156.2.16  2009/12/01 23:25:25  wenger
// Cleaned up a bunch of the temporary code for finding the torsion
// angle violations in the restraint grid; tested it at Osaka and
// Florence, added torsion-angle-related items to config files.
//
// Revision 1.156.2.15  2009/12/01 18:32:16  wenger
// Test stub code for retreiving torsion angle data by PDB ID is now in
// place.
//
// Revision 1.156.2.14  2009/12/01 18:02:07  wenger
// Torsion angle data can now be retreived by URL from the restraint grid.
//
// Revision 1.156.2.13  2009/12/01 15:56:57  wenger
// Got filtering of the html file returned by the restraint grid working;
// actually getting that file from the restraint grid via the URL isn't
// working yet (specifying a URL to a local file does work).
//
// Revision 1.156.2.12  2009/11/30 21:47:50  wenger
// Fixed title, etc., in html pages for torsion angle visualizations.
//
// Revision 1.156.2.11  2009/11/30 19:58:48  wenger
// Fixed visualization info for the torsion angle sessions; removed
// now-obsolete session template for separate violation visualization.
//
// Revision 1.156.2.10  2009/11/30 17:54:03  wenger
// Changed torsion angle links in summary page to reflect angle/violation
// unification.
//
// Revision 1.156.2.9  2009/11/30 17:09:15  wenger
// Changed order of torsion angle meta-data so that violations appear
// just below the corresponding lower/upper/average values.
//
// Revision 1.156.2.8  2009/11/26 00:30:47  wenger
// Did some cleaning up of the torsion angle meta-data.
//
// Revision 1.156.2.7  2009/11/25 22:46:05  wenger
// Okay, I think I have the "unified" visualization working.  I should
// do some cleaning up of names in the session template, though, to match
// the scheme I ended up with.  I also have to take out the creation of
// separate session, links, etc., for violations.
//
// Revision 1.156.2.6  2009/11/24 23:59:02  wenger
// Added the S2DNmrStarTarIfc to properly deal with the torsion angle-
// related tags.  Note: 3D tests fail because they're not yet updated
// for the new Atom_ID field.
//
// Revision 1.156.2.5  2009/11/24 16:22:34  wenger
// Got rid of a bunch of temporary notes that are no longer needed.
//
// Revision 1.156.2.4  2009/11/17 23:38:46  wenger
// Added some notes based on discussions at today's BMRB staff meeting.
//
// Revision 1.156.2.3  2009/11/17 23:25:18  wenger
// Writing of torsion angle sessions, etc., now mostly working (links in
// summary page are still not right, lots of other cleanup, finding
// data in restraint grid still needed).
//
// Revision 1.156.2.2  2009/11/16 21:50:42  wenger
// Modified the torsion angle session template to take advantage
// of the new HighLow symbol 'isAngle' mapping attribute.
//
// Revision 1.156.2.1  2009/10/22 17:11:31  wenger
// "Bounced" s2d_torsion_rest_0909_br off the trunk -- created new
// s2d_torsion_rest_0910_br, merged s2d_torsion_rest_0909_br_0 thru
// s2d_torsion_rest_0909_br_end to the new branch.
//
// Revision 1.156  2009/10/20 22:07:56  wenger
// Reorganized the chemical shift code by moving the code that actually
// gets the data values out of S2DMain (as I did for S2DAtomicCoords).
//
// Revision 1.155  2009/10/20 16:54:10  wenger
// Created a new S2DSpartaChemShift class and cleaned up S2DChemShift
// class heirarchy in preparation for fixing things for the new SPARTA
// file format; various related cleanups (note that empty hn?.dat files
// are no longer generated for nucleic acids, resulting in changes to
// the test scripts).
//
// Revision 1.154  2009/10/07 21:39:43  wenger
// Code reorganization as a test for the torsion angle code: for atomic
// coordinates, moved all of the code that actually gets the values from
// the STAR file from S2DMain into the S2DAtomicCoordinates constructor.
// I think this really makes a lot of sense -- it simplifies S2DMain,
// which is too complicated anyhow, and moves the knowledge about what
// data we need down into the relevant object.  I should probably make
// this change for all of the similar pieces of code.
//
// Revision 1.153.2.3  2009/10/21 20:24:11  wenger
// Fixed the 1brv_ta.str file -- I was using the wrong version before;
// we output some real data in S2DTorsionAngle.
//
// Revision 1.153.2.2  2009/10/08 16:20:23  wenger
// Partway along the processing of torsion angle restraints -- just
// committing for safety.
//
// Revision 1.153.2.1  2009/09/29 18:01:25  wenger
// Merged s2d_torsion_rest_0905_br_0 thru s2d_torsion_rest_0905_br_end
// to s2d_torsion_rest_0909_br.
//
// Revision 1.153  2009/09/25 21:30:32  wenger
// Changed version to 11.6.3x1, added 11.6.3 section to version
// history.
//
// Revision 1.152  2009/09/25 20:05:39  wenger
// Changed version to 11.6.2 for release.
//
// Revision 1.151  2009/09/25 14:48:44  wenger
// Fixed a bug I introduced in the DNA/RNA deltashift template when
// I fixed the colors; cleaned up a few commente in other session
// templates.
//
// Revision 1.150  2009/09/22 21:59:54  wenger
// Added the Google Analytics script to all of the rest of the HTML
// pages we generate (it's already in the summary page).
//
// Revision 1.149  2009/09/22 19:01:21  wenger
// In the 3D, ambiguity, and assignment figure of merit sessions, the
// DEVise cursor in the data view is now automatically sized at
// startup to occupy the middle half of the view's axis ranges in both
// the X and Y directions.
//
// Revision 1.148  2009/09/22 00:00:19  wenger
// Changed 3D session to show the top, rather than the bottom, of the
// available data sources at startup (but switching entity assemblies
// can still put the cursors back to the bottom for some reason -- maybe
// related to the algorithm for keeping cursors within the destination
// view?).
//
// Revision 1.147  2009/09/21 23:36:50  wenger
// Did a bunch of fixing up of colors for color and black/white print
// modes.  Also made "hidden" views brown to stand out when editing.
//
// Revision 1.146  2009/08/27 15:04:05  wenger
// Changed version to 11.6.2x1, added 11.6.2 version history section.
//
// Revision 1.145  2009/08/26 18:01:32  wenger
// Changed version to 11.6.1 for release.
//
// Revision 1.144  2009/08/25 23:19:14  wenger
// Added special code to set the SPARTA file for visualization
// server mode (uvd); added a corresponding test.
//
// Revision 1.143  2009/08/25 18:15:57  wenger
// Merged s2d_sparta_deltashift_br_0 thru s2d_sparta_deltashift_br_3
// to trunk.
//
// Revision 1.142  2009/08/21 21:40:32  wenger
// Changed "amino acid" to "amino acid/nucleotide" in chemical shift
// by AA/nucleotide html page.
//
// Revision 1.141  2009/07/20 22:33:48  wenger
// Implemented Peptide-CGI to-do 093 (derive Atom_type values from
// Atom_ID values if the Atom_type values don't exist).
//
// Revision 1.140  2009/07/16 19:23:39  wenger
// Fixed spelling of 'consensus' in CSI session.
//
// Revision 1.139  2009/07/09 22:43:34  wenger
// General fixes to Peptide-CGI tests -- check for beginning and end
// of lines in most checks for specific text; added output file lists
// to some tests that were missing them; a few other fixes.
//
// Revision 1.138.2.20  2009/08/25 17:52:03  wenger
// Very minor code cleanups, added SPARTA stuff to pre-release manual
// testing procedure.
//
// Revision 1.138.2.19  2009/08/24 18:50:28  wenger
// Minor improvements to some debug output.
//
// Revision 1.138.2.18  2009/08/24 15:52:59  wenger
// Minor cleanup of SPARTA code.
//
// Revision 1.138.2.17  2009/08/21 21:00:44  wenger
// Fixed errors that made some tests fail with the previous version.
//
// Revision 1.138.2.16  2009/08/21 19:29:38  wenger
// Peptide-CGI now creates the new "all-in-one" SPARTA visualization.
// But some existing tests fail -- DON'T MERGE UNTIL THAT IS FIXED.
// (Tagging with s2d_sparta_deltashift_br_1 before this commit,
// s2d_sparta_deltashift_br_2 after.)
//
// Revision 1.138.2.15  2009/08/19 20:11:05  wenger
// Changed SPARTA processing to deal with SPARTA data being in a
// separate file from the main BMRB entry (requested by Eldon
// yesterday).  (This includes modifying existing tests and adding
// a new test.)
//
// Revision 1.138.2.14  2009/08/12 20:15:16  wenger
// Changed _Delta_CS_Residue_author_seq_code to _Delta_CS_Residue_seq_code
// as per earlier email from Eldon.
//
// Revision 1.138.2.13  2009/08/07 20:05:41  wenger
// Changed sd16385.str and sd16385_3.str  to new versions of SPARTA format;
// changed the code to work on the new format; removed test_sparta1
// and test_sparta2 from test_all because their data is the old format
// and no longer works.
//
// Revision 1.138.2.12  2009/07/29 15:38:22  wenger
// Fixed warnings about not finding _Mol_system_component_name in
// deltashift save frames.
//
// Revision 1.138.2.11  2009/07/28 22:40:01  wenger
// Added processing of SPARTA-calculated delta shift average values.
//
// Revision 1.138.2.10  2009/07/22 20:20:22  wenger
// Fixed residue numbering in SPARTA delta shift visualizations;
// changed "theoretical" to "SPARTA-calculated" and changed method
// names, etc., to match.
//
// Revision 1.138.2.9  2009/07/22 19:29:14  wenger
// Added capability for processing SPARTA-calculated delta shifts for
// multiple models.  (This generates separate visualizations for each
// model -- I have to combine them, and probably also deal with the
// possibility of having SPARTA data for different entities/entity
// assemblies.)
//
// Revision 1.138.2.8  2009/07/15 21:11:27  wenger
// Added new SPARTA tests of sd16385.str and sd16385_3.str (they
// have some new tag names, etc.); added beginning and end of line
// checks to SPARTA tests.
//
// Revision 1.138.2.7  2009/07/15 19:50:46  wenger
// Changed SPARTA version from 11.7.0 to 11.6.1; changed HN to H in
// SPARTA processing and visualization.
//
// Revision 1.138.2.6  2009/07/15 17:36:31  wenger
// Added processing of N and HN deltashifts for SPARTA; added N and
// HN views to the session template (now split off from the "normal"
// deltashift template); partially added provision for multiple models.
//
// Revision 1.138.2.5  2009/07/06 21:47:46  wenger
// Sparta tests now check for links in summary HTML pages; cleaned up
// some debug code.
//
// Revision 1.138.2.4  2009/07/06 20:37:23  wenger
// Summary pages now have links for SPARTA-calculated deltashifts.
//
// Revision 1.138.2.3  2009/07/01 20:57:50  wenger
// Data is now generated for SPARTA deltashift values; the link in
// the summary page is not written yet, though.
//
// Revision 1.138.2.2  2009/07/01 18:05:59  wenger
// A lot of the SPARTA deltashift processing is in place -- the actual
// data isn't yet coming out right, though.
//
// Revision 1.138.2.1  2009/06/29 21:37:18  wenger
// Added initial versions of SPARTA-deltashift tests (just running the
// files thru Peptide-CGI -- we don't actually process the SPARTA-
// calculated deltashifts yet).
//
// Revision 1.138  2009/06/23 19:32:28  wenger
// Make_view now rejects all illegal (non-numerical) BMRB accession
// numbers (to cut down on the number of error emails I've been getting
// since I added the 'Force Reprocessing' button).
//
// Revision 1.137  2009/06/04 17:07:19  wenger
// Improved error message for errors parsing residue count.
//
// Revision 1.136.2.1  2009/06/10 20:55:59  wenger
// Changed Peptide-CGI version to 11.7.0TRx1 for this branch.
//
// Revision 1.136  2009/05/15 21:04:20  wenger
// Fixed Peptide-CGI bug 071 (note that the fix only works in conjunction
// with DEVise 1.9.1x3 and later).
//
// Revision 1.135  2009/04/15 20:18:47  wenger
// Changed version to 11.6.1, added new version history section.
//
// Revision 1.134  2009/04/15 17:56:23  wenger
// Changed version to 11.6.0 for release; got rid of unused
// 11.5.1 section in version history.
//
// Revision 1.133  2009/04/15 16:21:04  wenger
// Merged s2d_hc_spectrum_br_0 thru s2d_hc_spectrum_br_end to trunk;
// fixed test61 and test61_3.
//
// Revision 1.132.2.3  2009/04/14 22:20:50  wenger
// Added help for simulated H vs C spectrum.
//
// Revision 1.132.2.2  2009/04/14 22:09:07  wenger
// Session file, visualization-specific HTML file and summary page link
// are now created; removed "legend view" from session template;
// documented and cleaned up code.  (Still needs help for H vs C
// visualization.)
//
// Revision 1.132.2.1  2009/04/09 20:20:54  wenger
// HvsC simulated spectrum stuff is partly in place -- data is generated
// (but not fully tested, plus lots of temporary code still in place);
// schema and session template have been generated; processing does not
// yet generate the session file, specific HTML file, or the link in
// the summary HTML file.
//
// Revision 1.132  2009/04/01 16:19:25  wenger
// Changed version to 11.5.1x1, added 11.5.1 version history section.
//
// Revision 1.131  2009/03/26 22:46:25  wenger
// Changed version to 11.5.0 for release.
//
// Revision 1.130  2009/03/26 21:29:07  wenger
// Found and fixed bug 081 (needed to force 3D metadata to be ordered
// by entity assembly, so we don't have gaps if save frames are not
// ordered by entity assembly).
//
// Revision 1.129  2009/03/25 21:49:09  wenger
// Final cleanup of some of the nucleic-acid-related code, especially
// getting polymer types correctly for mmCIF files; added nucleic acid
// tests to pre-release testing document.
//
// Revision 1.128  2009/03/25 15:42:51  wenger
// Changed version to 11.5.0 (because I think this is a big enough
// change for a new minor version, not just a new revision); fixed
// up help for atomic coordinates, and added help for nucleic acid
// delta shifts.
//
// Revision 1.127  2009/03/24 19:04:50  wenger
// Fixed layout of nucleic acid deltashift session (made windows line
// up better, etc.); fixed nucleotide counts in summary html page, and
// changed residue to nucleotide where appropriate; fixed nucleic acid
// deltashift html pages so that they link to the nucleic-acid-specific
// help page.
//
// Revision 1.126  2009/03/20 17:21:24  wenger
// The data scroll view X filter is now updated according to the residues
// that have data for the selected entity assembly.
//
// Revision 1.125  2009/03/18 18:36:29  wenger
// Changed atom_coord_jmol.base to work off of 15083 instead of 4267,
// so we have multiple entities, and both a protein and a nucleic acid;
// fixed an error in test4_check (still referred to 4267).
//
// Revision 1.124  2009/03/16 20:33:08  wenger
// 3D data select view now only shows data that is available for the
// currently-selected entity assembly ID.
//
// Revision 1.123  2009/03/16 16:18:29  wenger
// Data selection cursor is now forced to be something that's available
// with the currently-selected entity assembly (by using the
// data_select_fix_cursor).
//
// Revision 1.122  2009/03/12 23:02:29  wenger
// Fixed a problem with the output from test1_3 (polymer type for
// heteronuclear NOEs showing up as "unknown"), and fixed the checking
// script so that the test will fail if the problem happens again.
//
// Revision 1.121  2009/03/12 17:30:19  wenger
// Changed entity assembly names to things like "EA 1 (polypeptide(L))"
// as requested by Eldon; changed tests accordingly.
//
// Revision 1.120  2009/03/11 19:53:15  wenger
// Implemented two-stage selection of data sets in coordinate
// visualizations (select entity assembly, then select data set); updated
// tests accordingly.  Updated test14 because it now generates data for
// entity assembly 1.
//
// Revision 1.119  2009/03/04 20:06:11  wenger
// Coordinate visualization now works for RNA (also fixed color legend
// to match Jmol's color for phosphorus).
//
// Revision 1.118  2009/03/04 18:22:18  wenger
// Deltashift visualization now works for RNA.
//
// Revision 1.117  2009/03/04 17:44:36  wenger
// Added JavaScreen client version to -dev_version output.
//
// Revision 1.116  2009/03/04 15:27:20  wenger
// DNA deltashift data is now working as part of 3D visualizations,
// just started on RNA, but need to finish other changes to RNA first;
// made changes to test67* and test68* accordingly.  (Note: highlighing
// doesn't work right for DNA in 3D visualizations.)
//
// Revision 1.115  2009/03/02 18:20:03  wenger
// Re-enabled the "fix" cursors in a window that's drawn by the JS,
// but very small (workaround for bug 923).
//
// Revision 1.114  2009/02/26 18:38:11  wenger
// Added "fix" cursors so that the atom select cursors stay within the
// range of legal values when changing nucleotides, even if they are
// out of the destination view.
//
// Revision 1.113  2009/02/26 00:49:33  wenger
// Changed view background colors to show which views go together.
//
// Revision 1.112  2009/02/26 00:29:34  wenger
// Added scrolling to atom selection views.
//
// Revision 1.111  2009/02/25 21:33:14  wenger
// Added residue labels to all data that were missing them (in preparation
// for selection by nucleotide in nucleotide visualizations; also allows
// user to drill down to see residue label in protein deltashift, CSI,
// etc., visualizations); changed tests accordingly; also fixed up some
// tests that weren't checking for entity assembly IDs in data.
//
// Revision 1.110  2009/02/20 22:54:24  wenger
// RNA visualization now works, using the DNA template; added stub
// help file for nucleic acid deltashift visualizations.
//
// Revision 1.109  2009/02/20 21:07:28  wenger
// Updated the DNA deltashift session template.
//
// Revision 1.108  2009/02/18 21:43:04  wenger
// Added S2DNAChemShift class to clean up nucleic acid code (this class
// will do the actual calculation and writing of chemical shift deltas
// for nucleic acids); added schemas for nucleic acid deltashift
// visualizations; updated tests to reflect the fact that (at least
// initially) we're not going to generate CSI visualizations for nucleic
// acids.
//
// Revision 1.107  2009/02/18 18:11:57  wenger
// Changed version x number.
//
// Revision 1.106  2009/02/18 18:10:49  wenger
// Fixed bug 065 (don't process non-polymer entities).
//
// Revision 1.105  2009/02/05 20:24:37  wenger
// All tests now work (including new nucleic acid tests), but lots of
// cleanup to be done plus actually writing correct deltashifts for
// nucleic acids.
//
// Revision 1.104  2009/01/29 22:04:57  wenger
// Made protein, DNA, and RNA subclasses of S2DChemShift to make further
// stuff easier; added some file checking to test64 and test65 (but
// delta shifts and CSI don't work yet for nucleic acids); committing
// again with nucleic acid stuff disabled.
//
// Revision 1.103  2009/01/29 16:43:31  wenger
// A lot of the nucleic acid code is working, but I need to add in
// the detection of what type of polymer we're processing -- so I'm
// just committing this with 'protein' hard-coded in for now, so
// the existing stuff all works.
//
// Revision 1.102  2009/01/28 16:43:00  wenger
// Fixed an error in getting entry titles.
//
// Revision 1.101  2008/12/02 20:30:06  wenger
// Set version to 11.4.1x1, added corresponding section to version
// history.
//
// Revision 1.100  2008/12/02 19:24:09  wenger
// Changed version to 11.4.0 for release.
//
// Revision 1.99  2008/12/01 20:37:52  wenger
// Merged s2d_bug_037_br_0 thru s2d_bug_037_br_2 to trunk.
//
// Revision 1.98  2008/11/14 22:03:11  wenger
// Incremented x on version.
//
// Revision 1.97  2008/11/14 21:14:59  wenger
// Fixed bugs 070 and 075 (problems with percent assignment values
// sometimes being greater than 100% for NMR-STAR 3.1 files).
//
// Revision 1.96  2008/11/13 22:23:38  wenger
// Added -check_pct command-line argument to cause fatal error if
// percent assigned is > 100%, and added that flag to all tests;
// also changed test scripts to avoid overwriting output logs within
// a single test.  (Note: the following tests now fail: test1_3 test3_3
// test4_3 test14_3 test15_3 test24 test38 test39 test40 test41 test47
// test52.)
//
// Revision 1.95  2008/09/26 20:00:06  wenger
// Changed version to 11.3.4x1.
//
// Revision 1.94  2008/09/26 16:43:51  wenger
// Changed version to 11.3.3 for release.
//
// Revision 1.93  2008/09/23 16:43:23  wenger
// Fixed bug 067 (bad last line of H vs N data).
//
// Revision 1.92  2008/09/19 19:55:19  wenger
// Dealt with Peptide-CGI to-do 076 ("no Java" warnings in html pages).
//
// Revision 1.91  2008/08/29 21:04:01  wenger
// Fixed bug 068 (missing highlights in data view of 3D visualization).
//
// Revision 1.90  2008/08/29 16:35:48  wenger
// Fixed bug 069 (problem with entries that don't have any ambiguity
// codes).
//
// Revision 1.89.2.28  2008/11/25 22:48:17  wenger
// More cleanup of PDB vs. BMRB matching code.
//
// Revision 1.89.2.27  2008/11/25 20:50:43  wenger
// Commented out a lot of the debug code from the previous commit.
//
// Revision 1.89.2.26  2008/11/25 20:37:40  wenger
// Okay, at least phase 1 of the BMRB/PDB entity assembly/chain
// sequence match checking is mostly done; there's still a ton of
// debug code here, and some places where it's not 100% done, but
// it's at least close.  Test27 and test27_3 currently fail.
//
// Revision 1.89.2.25  2008/11/19 21:27:22  wenger
// Cleaned up various notes about things to check.
//
// Revision 1.89.2.24  2008/11/19 20:25:18  wenger
// Fixed problems with getEntityFrame(), added test13_3 to check changes.
//
// Revision 1.89.2.23  2008/11/19 17:21:44  wenger
// Renamed saveFrameResCounts() to getFrameResCounts() and fixed it
// up a bit (we do, in fact, want to use entityID, not entityAssemblyID,
// in that method).
//
// Revision 1.89.2.22  2008/11/19 16:49:49  wenger
// Checked various issues for multiple entities; fixed test28 because
// we now generate correct residue lists; marked bug 064 as fixed.
//
// Revision 1.89.2.21  2008/11/17 19:28:07  wenger
// Added entity assembly IDs to summary page and specific visualization pages.
//
// Revision 1.89.2.20  2008/11/13 23:26:29  wenger
// Heteronuclear NOE, T1 & T2 relaxation, and S2 order parameters now
// work for multiple entities in 3.1 files (all tests now work).
//
// Revision 1.89.2.19  2008/11/12 23:22:11  wenger
// Coupling constants now work for multiple entities in 3.1; implemented
// new S2DStarIfc.getAndFilterOptionalFrameValues() method to make
// this work; added test61 and test61_3 to test that method.
//
// Revision 1.89.2.18  2008/11/11 20:47:51  wenger
// Progress on getting entity assembly IDs correct for coupling constants,
// heteronuclear NOE, T1 & T2 relaxation, and S2 order parameters
// (working for 2.1, but not yet for 3.1).
//
// Revision 1.89.2.17  2008/11/07 18:41:02  wenger
// Got rid of some old code that has been replaced by multiple-entity
// code fixes.
//
// Revision 1.89.2.16  2008/11/06 21:29:52  wenger
// Cleaned up S2DMain.saveAllResidueLists(), cleaned up and documented
// a bunch of other new methods.
//
// Revision 1.89.2.15  2008/11/05 23:21:35  wenger
// Test7, test24, and test30 now work.  Pistachio session needs fixes,
// though.
//
// Revision 1.89.2.14  2008/11/05 18:15:37  wenger
// Make S2DStarIfc and S2DNmrStarIfc abstract, and got rid of a bunch
// of things in S2DStarIfc that should only be in S2DNmrStarIfc.
// (Note that I could move a bunch more methods from S2DStarIfc
// to S2DNmrStarIfc, but I'm going to wait until after I merge the
// current branch.)
//
// Revision 1.89.2.13  2008/11/05 00:37:43  wenger
// Fixed a bunch of problems with getting coordinates from NMR-STAR
// files (e.g., 4096) -- test4 and test4_3 now work.
//
// Revision 1.89.2.12  2008/10/30 16:18:44  wenger
// Got rid of a bunch of code that's obsolete because of the multiple-
// entity changes.
//
// Revision 1.89.2.11  2008/10/28 15:00:55  wenger
// Ambiguity code visualizations now work with multiple-entity fix, and
// work for the first time with 3.1 files.
//
// Revision 1.89.2.10  2008/10/06 20:01:09  wenger
// Pistachio processing now works again; still need to change session
// to add a link on entity assembly ID, and test this on something with
// multiple entities.
//
// Revision 1.89.2.9  2008/10/03 21:22:08  wenger
// Minor cleanup...
//
// Revision 1.89.2.8  2008/10/03 21:08:14  wenger
// Okay, I think the basic chemical shift stuff is finally working
// right for various combinations of multiple entities and multiple
// entity assemblies (see test57* and test58*).  Lots of the other
// stuff still needs work, though.
//
// Revision 1.89.2.7  2008/09/05 22:16:55  wenger
// Got saving of master residue list to work for 4310; removed saving
// of per-chem shift frame residue lists; some more comments about
// tests.
//
// Revision 1.89.2.6  2008/08/19 21:24:09  wenger
// Now generating atomic coordinate data with "real" entity assembly IDs
// (right now just a direct mapping from A->1, etc -- needs to be changed);
// changed 3D session to use "master" residue list rather than the
// individual ones.
//
// Revision 1.89.2.5  2008/08/14 20:19:21  wenger
// Fixed problems in writing entity assembly IDs for chemical shift
// data (also includes fixing bug 067, which was already present before
// I started the current bug 037 multiple entity fixes).
//
// Revision 1.89.2.4  2008/08/13 21:38:21  wenger
// Writing entity assembly IDs now works for the chem shift data for
// 2.1 files.
//
// Revision 1.89.2.3  2008/08/13 19:06:51  wenger
// Writing entity assembly IDs now works for the chem shift data for
// 3.0/3.1 files.
//
// Revision 1.89.2.2  2008/08/13 15:05:25  wenger
// Part way to saving the "master" residue list for all entity
// assemblies (works for 2.1 files, not 3/3.1).
//
// Revision 1.89.2.1  2008/07/30 16:13:55  wenger
// First steps towards fixing bug 037/etc -- added (dummy) entity
// assembly ID values to generated data; updated schemas and tests
// accordingly.
//
// Revision 1.89  2008/07/25 20:31:01  wenger
// Incremented version to 11.3.3.
//
// Revision 1.88  2008/07/24 19:24:12  wenger
// Changed version to 11.3.2 for release.
//
// Revision 1.87  2008/07/24 00:01:40  wenger
// Kind of a kludgey fix for bug 066 -- just put the missing data source
// definitions back into the session template.  This isn't a general fix
// if we get more linked entries, though.  (The problem is that the 4096
// 3D session was written before 4038 was processed, so the data from
// 4038 wasn't getting data source definitions written to the session file.)
// (Note: this bug was just introduced with the recent bug 061 fixes,
// so the buggy version was not released -- caught it in pre-release
// testing!)
//
// Revision 1.86  2008/07/03 19:34:34  wenger
// Updated Osaka config file; changed version to 11.3.2x1.
//
// Revision 1.85  2008/07/02 18:23:52  wenger
// Changed version to 11.3.1 for release; final choice of suffix
// for S2 order params.
//
// Revision 1.84  2008/07/02 16:29:19  wenger
// S2 order parameter visualizations are done and approved by Eldon;
// tests at least partially updated for S2 order stuff;
// reversed the order of data sets in the data selection view of
// 3D visualizations (more closely matches the summary page); minor
// fix to testclean target in top-level makefile; minor fix to
// relaxation session template (bar widths now set); added indices
// to data set titles in 3D visualizations.
//
// Revision 1.83  2008/06/27 15:23:33  wenger
// Added the option to explicitly call the garbage collector a number
// of times during processing; added memory usage printout at debug
// level 2.
//
// Revision 1.82  2008/06/25 18:48:07  wenger
// Multiple summary page changes:  re-ordered the data sets; added
// mailto link; added "no data available" sections so users don't
// get confused about "missing" data.
//
// Revision 1.81  2008/06/17 23:07:53  wenger
// Fixed to-do 073:  we no longer generate figure of merit or ambiguity
// code visualizations if the values are all null (".").
//
// Revision 1.80  2008/06/05 21:12:26  wenger
// Changed version to 11.3.1x1; added new version history section;
// updated config file for new Florence server.
//
// Revision 1.79  2008/06/05 19:34:44  wenger
// Changed version to 11.3.0 for release.
//
// Revision 1.78  2008/06/04 21:27:04  wenger
// Got rid of frame details from summary page, as requested by Eldon.
//
// Revision 1.77  2008/06/04 21:12:01  wenger
// New Peptide-CGI summary page is implemented, test work except for
// test52 for some weird reason.  (Still may need some other changes
// before release, though.)
//
// Revision 1.76  2008/05/09 21:37:53  wenger
// Incremented version number (to 11.2.6) and added new section to
// version history.
//
// Revision 1.75  2008/05/09 15:52:39  wenger
// Now building starlibj from source so I can compile it with a Java
// 1.3 target.
//
// Revision 1.74  2008/05/08 17:14:44  wenger
// Changed version to 11.2.5 for release.
//
// Revision 1.73  2008/04/30 15:51:00  wenger
// Updated starlibj; added a test with the visualization-server-processed
// file that exposed the problem that the new starlibj fixes.
//
// Revision 1.72  2008/04/09 19:35:41  wenger
// Added frame details to individual visualization pages in preparation
// for summary page changes; spelled out Linear Analysis of Chemical
// Shifts; removed some unneeded parameters from the S2DSummaryHtml*
// constructors.
//
// Revision 1.71  2008/03/06 18:04:31  wenger
// Updated version to 11.2.5x1, added 11.2.5 section of version history.
//
// Revision 1.70  2008/03/06 17:08:25  wenger
// Changed version to 11.2.4 for release.
//
// Revision 1.69  2008/03/05 23:08:55  wenger
// Removed all shift-click references from html pages, and replaced
// them with references to the toolbar drill-down mode.
//
// Revision 1.68  2008/02/22 19:38:58  wenger
// Changed version to 11.2.4x1 and added 11.2.4 section to version
// history.
//
// Revision 1.67  2008/02/22 19:08:48  wenger
// Changed version to 11.2.3 for release; slight update to testing document.
//
// Revision 1.66  2008/02/20 17:41:07  wenger
// Committing (disabled) partially-implemented S2 Order visualization
// code and tests.
//
// Revision 1.65  2008/02/16 00:17:48  wenger
// Greatly improved view help in all Peptide-CGI visualizations.
//
// Revision 1.64  2008/01/26 21:29:45  wenger
// Started 11.2.3 now that 11.2.2 is released.
//
// Revision 1.63  2008/01/26 19:28:56  wenger
// Changed version to 11.2.2 for release.
//
// Revision 1.62  2008/01/25 20:46:04  wenger
// Fixed DEVise bug 956/Peptide-CGI bug 059 (problem with data view
// cursor in 3D visualizations).
//
// Revision 1.61  2007/12/20 16:49:03  wenger
// Improved ChemShiftRef error messages; ChemShift calculation failing
// is no longer considered an error at the top level of the program;
// S2DSpecificHtml methods are no longer static so new S2DCSRErrorHtml
// class could inherit from it correctly; some cache checking output
// is now printed at a lower versbosity setting.
//
// Revision 1.60  2007/11/21 00:09:26  wenger
// Incremented version number.
//
// Revision 1.59  2007/11/20 18:48:19  wenger
// Changed version to 11.2.1 for release.
//
// Revision 1.58  2007/11/20 18:01:27  wenger
// Merged peptide_cgi_na_histograms_br_2 thru
// peptide_cgi_na_histograms_br_3 to trunk.
//
// Revision 1.57  2007/11/17 19:32:30  wenger
// Incremented version; added install_histogram to the distribution
// (it was missing).
//
// Revision 1.56  2007/11/17 18:33:47  wenger
// Changed version to 11.2.0 for release.
//
// Revision 1.55  2007/11/17 16:32:02  wenger
// Merged peptide_cgi_na_histograms_br_0 thru
// peptide_cgi_na_histograms_br_1 to trunk.
//
// Revision 1.54  2007/11/15 17:15:34  wenger
// Cleaned out cvs history in source files.
//
// Revision 1.53  2007/11/14 20:07:17  wenger
// Made improvements to test37 and test47; we now print all input parameters
// at verbosity level 1 instead of 2.
//
// Revision 1.52  2007/11/14 18:49:02  wenger
// Replaced all test NMR-STAR 3.1 files with the latest versions,
// changed test_all accordingly (because this fixes earlier problems).
//
// Revision 1.51.2.2  2007/11/19 19:41:16  wenger
// Added DNA histogram html files, incremented Peptide-CGI version.
//
// Revision 1.51.2.1  2007/10/31 16:09:25  wenger
// Updated Peptide-CGI version to 11.2.0x1 -- I figure adding the nucleic
// acid histograms is enough of a change for a minor version change.
//
// Revision 1.51  2007/10/08 19:52:50  wenger
// Version string is now 11.1.6x1 because 11.1.5 has been released.
//
// Revision 1.50  2007/10/08 18:57:40  wenger
// Ready for release -- removed 'x' part of version.
//
// ...

// ========================================================================

package star2devise;

import EDU.bmrb.starlibj.SaveFrameNode;
import java.util.*;
import java.io.*;
import java.text.*;

// for chemical shifts

public class S2DMain {
    //===================================================================
    // VARIABLES

    private static final int DEBUG = 0;
    public static int _verbosity = 0;
    	// Whether to do "extra" calls to System.gc().
    private static boolean _extraGC = false;

    // Change version to 11.3.1 when S2 order stuff is implemented.
    public static final String PEP_CGI_VERSION = "11.8.2x10"/*TEMP*/;
    public static final String DEVISE_MIN_VERSION = "1.9.1";
    public static final String JS_CLIENT_MIN_VERSION = "5.10.0";

    private String _masterBmrbId = ""; // accession number the user requested

    // A list of all BMRB IDs (now stored as Strings) we need to access
    // for this entry.
    private Vector _bmrbIds = new Vector();

    // A list of all local files to process.
    private Vector _localFiles = new Vector();

    // A list of related PDB files we want to process for this entry.
    private Vector _pdbIds = new Vector(); // Vector contains Strings

    private String _dataDir;
    private String _csrDataDir = null;
    private String _htmlDir;
    private String _sessionDir;
    private String _connectionFile;

    // The name of this data set -- all output files generated will consist
    // of the name plus various suffixes.
    private String _name = "";
    private String _longName = null;
    
    private boolean _force = false;
    private boolean _useLocalFiles = false;
    private boolean _checkResList = false;
    private boolean _checkPctAssign = false;

    public static final int PDB_LEVEL_NONE = 0;
    public static final int PDB_LEVEL_LINK = 1;
    public static final int PDB_LEVEL_PROCESS = 2;
    //TEMP -- _pdbLevel should really be something like _coordLevel
    private int _pdbLevel = PDB_LEVEL_LINK;

    public static final int CSR_LEVEL_NONE = 0;
    public static final int CSR_LEVEL_LINK = 1;
    public static final int CSR_LEVEL_PROCESS = 2;
    private int _csrLevel = CSR_LEVEL_NONE;

    // LACS-related info.
    public static final int LACS_LEVEL_NONE = 0;
    public static final int LACS_LEVEL_TRY = 1;
    public static final int LACS_LEVEL_MANDATORY = 2;
    private int _lacsLevel = LACS_LEVEL_NONE;
    private String _lacsFile = null;

    // SPARTA-related info.
    private String _spartaFile = null;

    // General restraint-related info.
        // This is true only if we're processing restraints without
	// any connection to a BMRB entry.
    private boolean _restraintOnly = false;

    // Torsion angle restraint-related info (for restraints grid).
    public static final int TAR_LEVEL_NONE = 0;
    // This means create the links only if the appropriate entry
    // exists in the restraint grid.
    public static final int TAR_LEVEL_LINK_CHECK = 1;
    public static final int TAR_LEVEL_LINK = 2;
    public static final int TAR_LEVEL_PROCESS = 3;
    private int _tarLevel = TAR_LEVEL_LINK_CHECK;
    private String _tarFile = null;
    private String _tarUrl = null;//TEMP -- change name?

    // Remediated restraints-related info.
    public static final int RRTAR_LEVEL_NONE = 0;
    // This means create the links only if the appropriate entry
    // exists in the restraint grid.
    public static final int RRTAR_LEVEL_LINK_CHECK = 1;
    public static final int RRTAR_LEVEL_LINK = 2;
    public static final int RRTAR_LEVEL_PROCESS = 3;
    private int _rrTarLevel = RRTAR_LEVEL_LINK_CHECK;
    private String _rrFile; // remediated restraints

    // Distance restraint-related info (for restraints grid).
    public static final int DISTR_LEVEL_NONE = 0;
    // This means create the links only if the appropriate entry
    // exists in the restraint grid.
    public static final int DISTR_LEVEL_LINK_CHECK = 1;
    public static final int DISTR_LEVEL_LINK = 2;
    public static final int DISTR_LEVEL_PROCESS = 3;
    private int _distRLevel = DISTR_LEVEL_LINK_CHECK;
    private String _distRFile = null;
    private String _distRUrl = null;//TEMP -- change name?

    // Distance restraint-related info (for remediated restraints).
    public static final int RRDISTR_LEVEL_NONE = 0;
    // This means create the links only if the appropriate entry
    // exists in the restraint grid.
    public static final int RRDISTR_LEVEL_LINK_CHECK = 1;
    public static final int RRDISTR_LEVEL_LINK = 2;
    public static final int RRDISTR_LEVEL_PROCESS = 3;
    private int _rrDistRLevel = RRDISTR_LEVEL_LINK_CHECK;

    private boolean _haveCoords = false;

    private S2DSummaryHtml _summary;

    private static boolean _retrying;

    private String _cmdPdbId = null; // PDB ID from command line
    private String _currentPdbId = null; // PDB ID we're processing

    // PDB ID(s) for chem shift reference processing.
    private Vector _csrPdbIds = new Vector(); // Vector contains Strings.

    //TEMP -- need to document this!!!
    private int _cmdFrameIndex = -1;
    //TEMP -- need to document this!!!
    private int _currentFrameIndex = -1;

    private Vector _dataSets = new Vector(); // Vector of S2DDatasetInfo

    private boolean _doProteinCheck = true;

    private boolean _runScripts = true;

    private boolean _savedResList = false;

    private boolean _isUvd = false;

    private static final int CSR_TIMEOUT_DEFAULT = 120;
    private int _csrTimeout = CSR_TIMEOUT_DEFAULT;

    private static String _timestamp;
    private static String _shortTimestamp;

    private static int AMBIGUITY_WRAP_LENGTH = 10;
    private static int PISTACHIO_WRAP_LENGTH = 10;

    //===================================================================
    // PUBLIC METHODS

    //-------------------------------------------------------------------
    public static void main(String args[]) throws S2DException
    {
	_retrying = false;

	setTimestamps();

	try {
	    run(args);
	} catch (S2DCancel ex) {
	    // Just catch this so we don't pass it on to higher levels.
	} catch (S2DWarning ex) {
	    System.err.println("Top-level warning: ");
	    System.err.println(ex.toString());
	} catch (Exception ex) {
	    System.err.println("\n*** Top-level error: ");
	    ex.printStackTrace();
	    System.err.println(ex.toString());
	    throw new S2DError("NMR-Star to DEVise conversion failed");
	}
    }

    //-------------------------------------------------------------------
    // Do the conversion, retrying without PDB if we fail because of
    // running out of memory and we were trying to get data from PDB.
    public static void run(String args[]) throws S2DException
    {
        S2DMain s2d = new S2DMain(args);

	if (_retrying) {
	    s2d._pdbLevel = PDB_LEVEL_NONE;
	    s2d._csrLevel = CSR_LEVEL_NONE;
	    s2d._lacsLevel = LACS_LEVEL_NONE;
	    s2d._tarLevel = TAR_LEVEL_NONE;
	    s2d._rrTarLevel = RRTAR_LEVEL_NONE;
	    s2d._distRLevel = DISTR_LEVEL_NONE;
	    s2d._rrDistRLevel = RRDISTR_LEVEL_NONE;
	}

	try {
	    if (!s2d.useCache()) {
	        s2d.process();
	    } else {
	        if (doDebugOutput(1)) {
		    System.out.println("Cache files used");
		}
	    }
        } catch (OutOfMemoryError mem) {
	    System.err.println("OutOfMemoryError: " + mem.toString());
	    if (s2d._pdbLevel == PDB_LEVEL_PROCESS) {
	        System.out.println("Retrying without accessing PDB");

		s2d = null;
		System.gc();
	        _retrying = true;
		run(args);
	    } else {
	        throw new S2DError("NMR-Star to DEVise conversion failed " +
		  "because of insufficient memory");
	    }
	}
    }

    //-------------------------------------------------------------------
    // Get a timestamp to be used as the "processing time" for all files
    // we generate.
    public static String getTimestamp() { return _timestamp; }

    //-------------------------------------------------------------------
    // Get a short timestamp (date only).
    public static String getShortTimestamp() { return _shortTimestamp; }
    
    //-------------------------------------------------------------------
    // Constructor.
    public S2DMain(String args[]) throws S2DException
    {
        if (doDebugOutput(2)) {
	    String sep = "";
	    System.out.print("S2DMain.S2DMain(");
	    for (int index = 0; index < args.length; index++) {
	        System.out.print(sep + args[index]);
		sep = ", ";
	    }
	    System.out.print(")\n");
	}

	checkArgsNoProps(args);

	// getProperties must come before checkArgs, so command-line args
	// can override defaults in properties.
	Properties props = getProperties();
	getPropertiesDynamic(props);

	checkArgs(args);
    }

    //===================================================================
    // PRIVATE METHODS

    //-------------------------------------------------------------------
    public static void setTimestamps()
    {
        Date date = new Date();
	DateFormat dtf = DateFormat.getDateTimeInstance(DateFormat.MEDIUM,
	  DateFormat.MEDIUM);
	_timestamp = dtf.format(date);

	dtf = DateFormat.getDateInstance(DateFormat.MEDIUM);
	_shortTimestamp = dtf.format(date);
    }

    //-------------------------------------------------------------------
    // Get configuration-specific properties and set variables accordingly.
    public static Properties getProperties() throws S2DException
    {
        Properties props = new Properties();
	try {
	    props.load(new BufferedInputStream(
	      new FileInputStream("s2d.props")));
        } catch (FileNotFoundException ex) {
	    throw new S2DError("Can't find s2d properties file: " +
	      ex.toString());
	} catch (IOException ex) {
	    throw new S2DError("Error reading s2d properties file: " +
	      ex.toString());
	}

	S2DNames.BMRB_STAR_URL = props.getProperty("bmrb_mirror.star_url");
	if (S2DNames.BMRB_STAR_URL == null) {
	    throw new S2DError("Unable to get value for " +
	      "bmrb_mirror.star_url property");
	}

	S2DNames.PDB_FILE_URL = props.getProperty("bmrb_mirror.pdb_file_url");
	if (S2DNames.PDB_FILE_URL == null) {
	    throw new S2DError("Unable to get value for " +
	      "bmrb_mirror.pdb_file_url property");
	}

	S2DNames.BMRB_3D_URL = props.getProperty("bmrb_mirror.3d_url");
	if (S2DNames.BMRB_3D_URL == null) {
	    throw new S2DError("Unable to get value for " +
	      "bmrb_mirror.3d_url property");
	}

	S2DNames.CGI_URL = props.getProperty("bmrb_mirror.cgi_url");
	if (S2DNames.CGI_URL == null) {
	    throw new S2DError("Unable to get value for " +
	      "bmrb_mirror.cgi_url property");
	}

	S2DNames.UVD_CGI_URL = props.getProperty("bmrb_mirror.uvd_cgi_url");

	S2DNames.STAR_NAME_TEMPLATE = props.getProperty(
	  "bmrb_mirror.star_name_template");
	if (S2DNames.STAR_NAME_TEMPLATE == null) {
	    if (doDebugOutput(2)) {
	        System.out.println("bmrb_mirror.star_name_template " +
		  "property value not defined; using default");
	    }
	    S2DNames.STAR_NAME_TEMPLATE = "bmr*.str";
	}

	S2DNames.COMMENT_EMAIL = props.getProperty(
	  "bmrb_mirror.comment_email");
	if (S2DNames.COMMENT_EMAIL == null) {
	    throw new S2DError("Unable to get value for " +
	      "bmrb_mirror.comment_email property");
	}

	S2DNames.LACS_NAME_TEMPLATE = props.getProperty(
	  "bmrb_mirror.lacs_name_template");
	if (S2DNames.LACS_NAME_TEMPLATE == null) {
	    if (doDebugOutput(2)) {
	        System.out.println("bmrb_mirror.lacs_name_template " +
		  "property value not defined; using default");
	    }
	    S2DNames.LACS_NAME_TEMPLATE = "bmr*_LACS.out";
	}

	String debugTmp = props.getProperty("bmrb_mirror.debug_level");
	if (debugTmp == null) {
	    System.err.println(new S2DWarning("Unable to get value for " +
	      "bmrb_mirror.debug_level property"));
	} else {
	    try {
		S2DException.setDebugLvl(Integer.parseInt(debugTmp));
	    } catch(NumberFormatException ex) {
	        System.err.println(new S2DWarning("Error parsing " +
		  "debug_level value " + ex.toString() +
		  "; using default"));
	    }
	}

	String verbTmp = props.getProperty("bmrb_mirror.verbosity");
	if (verbTmp == null) {
	    System.err.println(new S2DWarning("Unable to get value for " +
	      "bmrb_mirror.verbosity property"));
	} else {
	    try {
		_verbosity = Integer.parseInt(verbTmp);
	    } catch(NumberFormatException ex) {
	        System.err.println(new S2DWarning("Error parsing " +
		  "verbosity value " + ex.toString() +
		  "; using default"));
	    }
	}

	String extraGCTmp = props.getProperty("bmrb_mirror.extra_gc");
	if (extraGCTmp == null) {
	    System.err.println(new S2DWarning("Unable to get value for " +
	      "bmrb_mirror.extra_gc property"));
	} else {
	    try {
	        _extraGC = (Integer.parseInt(extraGCTmp) != 0);
	    } catch (NumberFormatException ex) {
	        System.err.println(new S2DWarning("Error parsing " +
		  "extra_gc value " + ex.toString() +
		  "; using default"));
	    }
        }

	S2DNames.SPARTA_NAME_TEMPLATE = props.getProperty(
	  "bmrb_mirror.sparta_name_template");
	if (S2DNames.SPARTA_NAME_TEMPLATE == null) {
	    if (doDebugOutput(2)) {
	        System.out.println("bmrb_mirror.sparta_name_template " +
		  "property value not defined; using default");
	    }
	    S2DNames.SPARTA_NAME_TEMPLATE = "bmr*_sparta.str";
	}

	S2DNames.RESTRAINT_GRID_ROOT = props.getProperty(
	  "bmrb_mirror.restraint_grid_root");
	if (S2DNames.RESTRAINT_GRID_ROOT == null) {
	    if (doDebugOutput(2)) {
	        System.out.println("bmrb_mirror.restraint_grid_root " +
		  "property value not defined; using default");
	    }
	    S2DNames.RESTRAINT_GRID_ROOT =
	      "http://restraintsgrid.bmrb.wisc.edu/NRG/";
	}

	S2DNames.TORSION_ANGLE_SEARCH_TEMPLATE = props.getProperty(
	  "bmrb_mirror.torsion_angle_search_template");
	if (S2DNames.TORSION_ANGLE_SEARCH_TEMPLATE == null) {
	    if (doDebugOutput(2)) {
	        System.out.println("bmrb_mirror.torsion_angle_search_template " +
		  "property value not defined; using default");
	    }
	    S2DNames.TORSION_ANGLE_SEARCH_TEMPLATE =
	      "MRGridServlet?pdb_id=*&bmrb_id=&block_text_type=" +
	      "4-filtered-FRED&min_items=100&db_username=wattos1";
	}

	S2DNames.REMEDIATED_RESTRAINTS_TEMPLATE = props.getProperty(
	  "bmrb_mirror.remediated_restraints_template");
	if (S2DNames.REMEDIATED_RESTRAINTS_TEMPLATE == null) {
	    if (doDebugOutput(2)) {
	        System.out.println(
		  "bmrb_mirror.remediated_restraints_template " +
		  "property value not defined; using default");
	    }
	    S2DNames.REMEDIATED_RESTRAINTS_TEMPLATE =
	      "http://www.bmrb.wisc.edu/ftp/pub/bmrb/" +
	      "nmr_pdb_integrated_data/coordinates_restraints_chemshifts" +
	      "/all/nmr-star/*/*_linked.str";
	}

        return props;
    }

    //-------------------------------------------------------------------
    // Get configuration-specific properties and set variables
    // accordingly, for things that can't be static.
    private void getPropertiesDynamic(Properties props) throws S2DException
    {
	String csrTmp = props.getProperty("bmrb_mirror.do_csr_default");
	if (csrTmp == null) {
	    System.err.println(new S2DWarning("Unable to get value for " +
	      "bmrb_mirror.do_csr_default property"));
	} else {
	    try {
	        _csrLevel = Integer.parseInt(csrTmp);
	    } catch(NumberFormatException ex) {
	        System.err.println(new S2DWarning("Error parsing " +
		  "do_csr_default value " + ex.toString() +
		  "; using default"));
	    }
	}

	String lacsTmp = props.getProperty("bmrb_mirror.lacs_level_default");
	if (lacsTmp == null) {
	    System.err.println(new S2DWarning("Unable to get value for " +
	      "bmrb_mirror.lacs_level_default property; using default"));
	} else {
	    try {
	        _lacsLevel = Integer.parseInt(lacsTmp);
	    } catch(NumberFormatException ex) {
	        System.err.println(new S2DWarning("Error parsing " +
		  "lacs_level_default value " + ex.toString() +
		  "; using default"));
	    }
	}

	S2DNames.LACS_URL = props.getProperty("bmrb_mirror.lacs_url");
	if (S2DNames.LACS_URL == null) {
	    S2DError err = new S2DError("Unable to get value for " +
	      "bmrb_mirror.lacs_url property");
	    if (_lacsLevel > LACS_LEVEL_NONE) {
	        throw err;
	    } else {
	        System.err.println(err + "; using default");
	    }
	}

	S2DNames.SPARTA_URL = props.getProperty("bmrb_mirror.sparta_url");
	if (S2DNames.SPARTA_URL == null) {
	    S2DError err = new S2DError("Unable to get value for " +
	      "bmrb_mirror.sparta_url property; using default");
	    S2DNames.SPARTA_URL = "file:./";
	}

	String tarTmp = props.getProperty("bmrb_mirror.do_tar_default");
	if (tarTmp == null) {
	    System.err.println(new S2DWarning("Unable to get value for " +
	      "bmrb_mirror.do_tar_default property"));
	} else {
	    try {
	        _tarLevel = Integer.parseInt(tarTmp);
	    } catch(NumberFormatException ex) {
	        System.err.println(new S2DWarning("Error parsing " +
		  "do_tar_default value " + ex.toString() +
		  "; using default"));
	    }
	}

	String rrTarTmp = props.getProperty("bmrb_mirror.do_rrtar_default");
	if (rrTarTmp == null) {
	    System.err.println(new S2DWarning("Unable to get value for " +
	      "bmrb_mirror.do_rrtar_default property"));
	} else {
	    try {
	        _rrTarLevel = Integer.parseInt(rrTarTmp);
	    } catch(NumberFormatException ex) {
	        System.err.println(new S2DWarning("Error parsing " +
		  "do_rrtar_default value " + ex.toString() +
		  "; using default"));
	    }
	}

	String distTmp = props.getProperty("bmrb_mirror.do_dist_default");
	if (distTmp == null) {
	    System.err.println(new S2DWarning("Unable to get value for " +
	      "bmrb_mirror.do_dist_default property"));
	} else {
	    try {
	        _distRLevel = Integer.parseInt(distTmp);
	    } catch(NumberFormatException ex) {
	        System.err.println(new S2DWarning("Error parsing " +
		  "do_dist_default value " + ex.toString() +
		  "; using default"));
	    }
	}

	String rrDistTmp = props.getProperty("bmrb_mirror.do_rrdist_default");
	if (rrDistTmp == null) {
	    System.err.println(new S2DWarning("Unable to get value for " +
	      "bmrb_mirror.do_rrdist_default property"));
	} else {
	    try {
	        _rrDistRLevel = Integer.parseInt(rrDistTmp);
	    } catch(NumberFormatException ex) {
	        System.err.println(new S2DWarning("Error parsing " +
		  "do_rrdist_default value " + ex.toString() +
		  "; using default"));
	    }
	}
    }

    //-------------------------------------------------------------------
    // Check arguments to constructor that need to be processed before
    // we try to get properties, set member variables accordingly.
    private void checkArgsNoProps(String args[]) throws S2DException
    {
	int index = 0;
	while (index < args.length) {
	    if ("-dev_version".equals(args[index])) {
		System.out.println("Peptide-CGI version " + PEP_CGI_VERSION +
		  " requires DEVise version " + DEVISE_MIN_VERSION +
		  " or higher and JavaScreen client version " +
		  JS_CLIENT_MIN_VERSION + " or higher");
		throw new S2DCancel();

	    } else if ("-version".equals(args[index])) {
	        System.out.println(PEP_CGI_VERSION);
		throw new S2DCancel();
	    }

	    index++;
    	}
    }

    //-------------------------------------------------------------------
    // Check arguments to constructor, set member variables accordingly.
    private void checkArgs(String args[]) throws S2DException
    {
        if (doDebugOutput(2)) {
	    System.out.println("Arguments: ");
	    for (int index = 0; index < args.length; index++) {
	        System.out.println("  {" + args[index] + "}");
	    }
	}

	//
	// The usage message.
	//
	String usage = 
          "Usage:\n" +
          "    java S2DMain -usage\n" +
          "        or\n" +
          "    java S2DMain -version\n" +
          "        or\n" +
          "    java S2DMain -bmrbid <> [args...]\n" +
          "        or\n" +
          "    java S2DMain -pdbid <> -bmrbid <> -coord_index <> [args...]\n" +
          "        or\n" +
          "    java S2DMain -name <> -file <> [-file <>...] -coord_index <> [args...]\n" +
          "\n" +
          "Options are:\n" +
          "    -bmrbid <value>\n" +
          "        the BMRB accession number to process (e.g., 4081)\n" +
	  "        (mandatory if pdbid is specified)\n" +
	  "    -check_pct\n" +
	  "        check percent assigned values & fail if > 100%\n" +
	  "    -check_res_list\n" +
	  "        check the residue list against the chem shift loop residues\n" +
          "    -conn_file <filename>\n" +
          "        the connection file for the 3D structures (mandatory)\n" +
	  "    -coord_index <value>\n" +
	  "        the index of coordinate set\n" +
	  "        (mandatory if pdbid is specified)\n" +
          "    -csr_data_dir <directory>\n" +
	  "        the path to the data directory as mounted relative to\n" +
	  "        the Chem Shift Reference server (default is the same as\n" +
	  "        the argument specified for -data_dir)\n" +
          "    -csr_timeout <seconds>\n" +
          "        the timeout for chemical shift reference processing\n" +
	  "        (default is " + CSR_TIMEOUT_DEFAULT + ")\n" +
          "    -data_dir <directory>\n" +
          "        the directory in which to store the data files (mandatory)\n" +
	  "    -debug <value>\n" +
	  "        set the debug level (default is 0 -- messages but no " +
	  "stack traces)\n" +
	  "    -dev_version\n" +
	  "        print the DEVise and JavaScreen client versions needed\n" +
	  "        by this version of Peptide-CGI\n" +
	  "    -dist_file <filename>\n" +
	  "        file containing distance restraint data\n" +
	  "    -dist_url <url>\n" +
	  "        URL of document containing distance restraint data\n" +
          "    -do_csr <0|1|2>\n" +
          "        0: don't do chem shift reference calculations;\n" +
          "        1: create link in summary file but don't do the calculation;\n" +
          "        2: do chem shift reference calculations\n" +
          "        (default is " + _csrLevel + ")\n" +
          "    -do_dist <0|1|2|3>\n" +
          "        0: ignore distance restraint references;\n" +
          "        1: if restraint grid entry exists create links in\n" +
	  "          summary file but don't process;\n" +
          "        1: create links in summary file but don't process;\n" +
          "        3: process distance restraint references\n" +
          "        (default is " + _distRLevel + ")\n" +
          "    -do_lacs <0|1|2>\n" +
          "        0: don't attempt LACS processing\n" +
          "        1: attempt LACS processing, failure is not an error\n" +
          "        2: attempt LACS processing, failure is an error\n" +
          "        (default is " + _lacsLevel + " unless -lacs_file is set)\n" +
          "    -do_pdb <0|1|2>\n" +
          "        0: ignore PDB references;\n" +
          "        1: create links in summary file but don't process;\n" +
          "        2: process PDB references\n" +
          "        (default is " + _pdbLevel + " unless -pdb_file is set)\n" +
          "    -do_rrdist <0|1|2|3>\n" +
          "        0: ignore distance restraint references;\n" +
          "        1: if remediated restraints entry exists create links in\n" +
	  "          summary file but don't process;\n" +
          "        1: create links in summary file but don't process;\n" +
          "        3: process distance restraint references\n" +
          "        (default is " + _rrDistRLevel + ")\n" +
          "    -do_rrtar <0|1|2|3>\n" +
          "        0: ignore torsion angle restraint references;\n" +
          "        1: if remediated restraints entry exists create links in\n" +
	  "          summary file but don't process;\n" +
          "        1: create links in summary file but don't process;\n" +
          "        3: process torsion angle restraint references\n" +
          "        (default is " + _rrTarLevel + ")\n" +
          "    -do_tar <0|1|2|3>\n" +
          "        0: ignore torsion angle restraint references;\n" +
          "        1: if restraint grid entry exists create links in\n" +
	  "          summary file but don't process;\n" +
          "        1: create links in summary file but don't process;\n" +
          "        3: process torsion angle restraint references\n" +
          "        (default is " + _tarLevel + ")\n" +
	  "    -file <filename>\n" +
	  "        local file containing data to be processed\n" +
          "    -force\n" +
          "        force the processing to happen even if a suitable set of\n" +
          "        output files already exists\n" +
	  "    -html_dir <directory>\n" +
          "        the directory in which to store the html files (mandatory)\n" +
	  "    -js_data_dir <directory>\n" +
	  "        data directory *relative to the JavaScreen server's base\n" +
	  "        data directory*\n" +
	  "    -local\n" + 
	  "        use local files for BMRB and PDB data (for testing without\n" +
	  "        connecting to web sites)\n" +
	  "    -lacs_file <filename>\n" +
	  "        file containing LACS data to process\n" +
	  "    -lacs_url <url>\n" +
	  "        URL of directory containing LACS files\n" +
	  "    -name <string>\n" + 
	  "        the name of the data set being processed (used only when\n" +
	  "        a bmrbid is not specified)\n" +
	  "    -no_protein_check\n" +
	  "        turn off check that data is for a protein\n" +
	  "    -no_scripts\n" +
	  "        don't run scripts (e.g., running on Windows)\n" +
          "    -pdb_file <filename>\n" +
          "        a PDB (really mmCIF) file containing the coordinates " +
	    "to be processed\n" +
          "    -pdbid <value>\n" +
          "        the PDB ID to process (e.g., 1dfv)\n" +
	  "    -rr_file <filename>\n" +
	  "        file containing remediated restraint data\n" +
          "    -session_dir <directory>\n" +
          "        the directory in which to store the session files (mandatory)\n" +
	  "    -sparta_file <filename>\n" +
	  "        file containing SPARTA data to process\n" +
	  "    -sparta_url <url>\n" +
	  "        URL of directory containing SPARTA files\n" +
	  "    -star_url <url>\n" +
	  "        URL of directory containing NMR-STAR files\n" +
	  "    -tar_file <filename>\n" +
	  "        file containing torsion angle restraint data\n" +
	  "    -tar_url <url>\n" +
	  "        URL of document containing torsion angle restraint data\n" +
          "    -usage\n" +
          "        show this message\n" +
	  "    -uvd\n" + 
	  "        indicates that this is \"upload and visualize data\" " +
	  "processing\n" + 
	  "    -verb <value>\n" +
	  "        set the verbosity of diagnostic output (default is 0)\n" +
          "    -version\n" +
          "        show the version number\n" +
          "\n" +
	  "Examples:\n" +
          "    java S2DMain -bmrbid 4101 ...\n" +
          "    java S2DMain -pdbid 1OXM -bmrbid 4101 -coord_index 2 ...\n" +
	  "    java S2DMain -file foo_data -name foo_out ...\n";

	//
	// Reset static variables (for Jafar).
	//
        S2DSession.setJSDataDir(null);
	S2DException.clear();

	//
	// Process the command-line arguments.
	//
	int index = 0;
	while (index < args.length) {

	    if ("-bmrbid".equals(args[index])) {
	        index++;
		if (index >= args.length) {
		    throw new S2DError("-bmrbid argument needs value");
		}
		_name = args[index];
		S2DException.setName(_name);
		_longName = "BMRB " + args[index];
	        _masterBmrbId = args[index];
		addBmrbId(_masterBmrbId);
		if (_masterBmrbId.equals("4038")) {
		    _longName += "/4096";
		} else if (_masterBmrbId.equals("4096")) {
		    _longName += "/4038";
		}

	    } else if ("-check_pct".equals(args[index])) {
	    	_checkPctAssign = true;

	    } else if ("-check_res_list".equals(args[index])) {
	    	_checkResList = true;

	    } else if ("-conn_file".equals(args[index])) {
	        index++;
		if (index >= args.length) {
		    throw new S2DError("-conn_file argument needs value");
		}
		_connectionFile = args[index];

	    } else if ("-coord_index".equals(args[index])) {
	        index++;
		if (index >= args.length) {
		    throw new S2DError("-coord_index argument needs value");
		}
		try {
	            _cmdFrameIndex = Integer.parseInt(args[index]);
	        } catch(NumberFormatException ex) {
	            System.err.println("Error parsing frame index: " +
		      ex.toString());
	            throw new S2DError("Error parsing frame index " +
		      ex.toString());
	        }

	    } else if ("-csr_data_dir".equals(args[index])) {
	        index++;
		if (index >= args.length) {
		    throw new S2DError("-csr_data_dir argument needs value");
		}
	        _csrDataDir = args[index];

	    } else if ("-csr_timeout".equals(args[index])) {
	        index++;
		if (index >= args.length) {
		    throw new S2DError("-csr_timeout argument needs value");
		}
		try {
	            _csrTimeout = Integer.parseInt(args[index]);
	        } catch(NumberFormatException ex) {
	            System.err.println("Error parsing chem shift ref " +
		      "timeout: " + ex.toString());
	            throw new S2DError("Error parsing chem shift ref " +
		      "timeout: " + ex.toString());
	        }

	    } else if ("-data_dir".equals(args[index])) {
	        index++;
		if (index >= args.length) {
		    throw new S2DError("-data_dir argument needs value");
		}
	        _dataDir = args[index];

	    } else if ("-debug".equals(args[index])) {
	        index++;
		if (index >= args.length) {
		    throw new S2DError("-debug argument needs value");
		}
		try {
		    S2DException.setDebugLvl(Integer.parseInt(args[index]));
	        } catch(NumberFormatException ex) {
	            System.err.println("Error parsing debug value: " +
		      ex.toString());
	            throw new S2DError("Error parsing debug value " +
		      ex.toString());
	        }

	    } else if ("-dist_file".equals(args[index])) {
	        index++;
		if (index >= args.length) {
		    throw new S2DError("-dist_file argument needs value");
		}
		_distRFile = args[index];

	    } else if ("-dist_url".equals(args[index])) {
	        index++;
		if (index >= args.length) {
		    throw new S2DError("-dist_url argument needs value");
		}
		_distRUrl = args[index];
		// This is to test URL strings to be able to be passed
		// in test script.
		_distRUrl = S2DUtils.replace(_distRUrl, "#38", "&");
		_distRUrl = S2DUtils.replace(_distRUrl, "#63", "?");

	    } else if ("-do_csr".equals(args[index])) {
	        index++;
		if (index >= args.length) {
		    throw new S2DError("-do_csr argument needs value");
		}
		try {
	            _csrLevel = Integer.parseInt(args[index]);
	        } catch(NumberFormatException ex) {
	            System.err.println("Error parsing do_csr value: " +
		      ex.toString());
	            throw new S2DError("Error parsing do_csr value " +
		      ex.toString());
	        }

	    } else if ("-do_dist".equals(args[index])) {
	        index++;
		if (index >= args.length) {
		    throw new S2DError("-do_dist argument needs value");
		}
		try {
	            _distRLevel = Integer.parseInt(args[index]);
		    // For now, we're forcing coordinate processing to
		    // happen whenever we do distance restraint processing.
		    // This should eventually be changed to not re-
		    // process coordinates if we already have them andy
		    // they're up-to-date.
		    if (_distRLevel == DISTR_LEVEL_PROCESS) {
		        _pdbLevel = PDB_LEVEL_PROCESS;
		    }
	        } catch(NumberFormatException ex) {
	            System.err.println("Error parsing do_dist value: " +
		      ex.toString());
	            throw new S2DError("Error parsing do_dist value " +
		      ex.toString());
	        }

	    } else if ("-do_lacs".equals(args[index])) {
	        index++;
		if (index >= args.length) {
		    throw new S2DError("-do_lacs argument needs value");
		}
		try {
	            _lacsLevel = Integer.parseInt(args[index]);
	        } catch(NumberFormatException ex) {
	            System.err.println("Error parsing do_lacs value: " +
		      ex.toString());
	            throw new S2DError("Error parsing do_lacs value " +
		      ex.toString());
	        }

	    } else if ("-do_pdb".equals(args[index])) {
	        index++;
		if (index >= args.length) {
		    throw new S2DError("-do_pdb argument needs value");
		}
		try {
	            _pdbLevel = Integer.parseInt(args[index]);
	        } catch(NumberFormatException ex) {
	            System.err.println("Error parsing do_pdb value: " +
		      ex.toString());
	            throw new S2DError("Error parsing do_pdb value " +
		      ex.toString());
	        }

	    } else if ("-do_rrdist".equals(args[index])) {
	        index++;
		if (index >= args.length) {
		    throw new S2DError("-do_rrdist argument needs value");
		}
		try {
	            _rrDistRLevel = Integer.parseInt(args[index]);
	        } catch(NumberFormatException ex) {
	            System.err.println("Error parsing do_rrdist value: " +
		      ex.toString());
	            throw new S2DError("Error parsing do_rrdist value " +
		      ex.toString());
	        }

	    } else if ("-do_rrtar".equals(args[index])) {
	        index++;
		if (index >= args.length) {
		    throw new S2DError("-do_rrtar argument needs value");
		}
		try {
	            _rrTarLevel = Integer.parseInt(args[index]);
	        } catch(NumberFormatException ex) {
	            System.err.println("Error parsing do_rrtar value: " +
		      ex.toString());
	            throw new S2DError("Error parsing do_rrtar value " +
		      ex.toString());
	        }

	    } else if ("-do_tar".equals(args[index])) {
	        index++;
		if (index >= args.length) {
		    throw new S2DError("-do_tar argument needs value");
		}
		try {
	            _tarLevel = Integer.parseInt(args[index]);
		    // For now, we're forcing coordinate processing to
		    // happen whenever we do torsion angle restraint processing.
		    // This should eventually be changed to not re-
		    // process coordinates if we already have them andy
		    // they're up-to-date.
		    if (_tarLevel == TAR_LEVEL_PROCESS) {
		        _pdbLevel = PDB_LEVEL_PROCESS;
		    }
	        } catch(NumberFormatException ex) {
	            System.err.println("Error parsing do_tar value: " +
		      ex.toString());
	            throw new S2DError("Error parsing do_tar value " +
		      ex.toString());
	        }

	    } else if ("-file".equals(args[index])) {
	        index++;
		if (index >= args.length) {
		    throw new S2DError("-file argument needs value");
		}
		_localFiles.addElement(args[index]);

	    } else if ("-force".equals(args[index])) {
		_force = true;

	    } else if ("-html_dir".equals(args[index])) {
	        index++;
		if (index >= args.length) {
		    throw new S2DError("-html_dir argument needs value");
		}
	        _htmlDir = args[index];

	    } else if ("-js_data_dir".equals(args[index])) {
	        index++;
		if (index >= args.length) {
		    throw new S2DError("-js_data_dir argument needs value");
		}
	        S2DSession.setJSDataDir(args[index]);

	    } else if ("-lacs_file".equals(args[index])) {
	        index++;
		if (index >= args.length) {
		    throw new S2DError("-lacs_file argument needs value");
		}
		_lacsFile = args[index];
		_lacsLevel = LACS_LEVEL_MANDATORY;

	    } else if ("-lacs_url".equals(args[index])) {
	        index++;
		if (index >= args.length) {
		    throw new S2DError("-lacs_url argument needs value");
		}
		S2DNames.LACS_URL = args[index];

	    } else if ("-local".equals(args[index])) {
		_useLocalFiles = true;

	    } else if ("-name".equals(args[index])) {
	        index++;
		if (index >= args.length) {
		    throw new S2DError("-name argument needs value");
		}
		_name = args[index];
		S2DException.setName(_name);
		_longName = args[index];

	    } else if ("-no_protein_check".equals(args[index])) {
		_doProteinCheck = false;

	    } else if ("-no_scripts".equals(args[index])) {
		_runScripts = false;

	    } else if ("-pdb_file".equals(args[index])) {
	        index++;
		if (index >= args.length) {
		    throw new S2DError("-pdb_file argument needs value");
		}
		String url = "file:" + args[index];
	        _cmdPdbId = url;
		_pdbIds.addElement(url);
		// ChemShift needs changes to work with local file.
		// _csrPdbIds.addElement(_cmdPdbId);
		_pdbLevel = PDB_LEVEL_PROCESS;

	    } else if ("-pdbid".equals(args[index])) {
	        index++;
		if (index >= args.length) {
		    throw new S2DError("-pdbid argument needs value");
		}
	        _cmdPdbId = args[index];
		_cmdPdbId = _cmdPdbId.toUpperCase();
		_pdbIds.addElement(_cmdPdbId);
		_csrPdbIds.addElement(_cmdPdbId);

	    } else if ("-rr_file".equals(args[index])) {
	        index++;
		if (index >= args.length) {
		    throw new S2DError("-rr_file argument needs value");
		}
		_rrFile = args[index];

	    } else if ("-session_dir".equals(args[index])) {
	        index++;
		if (index >= args.length) {
		    throw new S2DError("-session_dir argument needs value");
		}
	        _sessionDir = args[index];

	    } else if ("-sparta_file".equals(args[index])) {
	        index++;
		if (index >= args.length) {
		    throw new S2DError("-sparta_file argument needs value");
		}
		_spartaFile = args[index];

	    } else if ("-sparta_url".equals(args[index])) {
	        index++;
		if (index >= args.length) {
		    throw new S2DError("-sparta_url argument needs value");
		}
		S2DNames.SPARTA_URL = args[index];

	    } else if ("-star_url".equals(args[index])) {
	        index++;
		if (index >= args.length) {
		    throw new S2DError("-star_url argument needs value");
		}
		S2DNames.BMRB_STAR_URL = args[index];

	    } else if ("-tar_file".equals(args[index])) {
	        index++;
		if (index >= args.length) {
		    throw new S2DError("-tar_file argument needs value");
		}
		_tarFile = args[index];

	    } else if ("-tar_url".equals(args[index])) {
	        index++;
		if (index >= args.length) {
		    throw new S2DError("-tar_url argument needs value");
		}
		_tarUrl = args[index];
		// This is to test URL strings to be able to be passed
		// in test script.
		_tarUrl = S2DUtils.replace(_tarUrl, "#38", "&");
		_tarUrl = S2DUtils.replace(_tarUrl, "#63", "?");

	    } else if ("-usage".equals(args[index])) {
	        System.out.println(usage);
		throw new S2DCancel();

	    } else if ("-uvd".equals(args[index])) {
	        if (S2DNames.UVD_CGI_URL == null) {
	            throw new S2DError("Unable to get value for " +
	              "bmrb_mirror.uvd_cgi_url property");
	        }
		_isUvd = true;
	        S2DSpecificHtml.setIsUvd(true);
	        S2DSummaryHtmlGen.setIsUvd(true);

	    } else if ("-verb".equals(args[index])) {
	        index++;
		if (index >= args.length) {
		    throw new S2DError("-verb argument needs value");
		}
		try {
		    _verbosity = Integer.parseInt(args[index]);
	        } catch(NumberFormatException ex) {
	            System.err.println("Error parsing verbosity value: " +
		      ex.toString());
	            throw new S2DError("Error parsing verbosity value " +
		      ex.toString());
	        }

	    } else {
	        throw new S2DError("Unrecognized command-line argument: " +
		  args[index]);
	    }
	    index++;
	}

        // For now, we're forcing coordinate processing to
        // happen whenever we do torsion angle processing.
        // This should eventually be changed to not re-
        // process coordinates if we already have them andy
        // they're up-to-date.
        if (_tarLevel == TAR_LEVEL_PROCESS ||
	  _rrTarLevel == RRTAR_LEVEL_PROCESS) {
            _pdbLevel = PDB_LEVEL_PROCESS;
        }

	//
	// Check for illegal combinations of arguments.
	// TEMP: can bmrbid and local file *both* be specified?  If not,
	// we should give an error if they both are.
	//
	if (_masterBmrbId.equals("") && _cmdPdbId == null &&
	  _localFiles.size() == 0) {
	    throw new S2DError(
	      "bmrbid, pdbid, or local file must be specified");
	}

	if (_masterBmrbId.equals("") && _name.equals("") &&
	  _cmdPdbId != null) {
	    throw new S2DError(
	      "bmrbid or name must be specified when pdbid is specified");
	}

	if (_cmdPdbId == null && _cmdFrameIndex != -1) {
	    throw new S2DError(
	      "coord_index must not be specified when pdbid is not specified");
	}

	if (_connectionFile == null) {
	    throw new S2DError("conn_file must be specified");
	}

	if (_dataDir == null) {
	    throw new S2DError("data_dir must be specified");
	}

	if (_sessionDir == null) {
	    throw new S2DError("session_dir must be specified");
	}

	if (_htmlDir == null) {
	    throw new S2DError("html_dir must be specified");
	}
	
	if (_name.equals("")) {
	    throw new S2DError("name must be specified if bmrbid is not");
	}

	// Name cannot contain a dot because that will end up in the
	// data source names and goof up DEVise when it tries to open
	// the session.
	if (_name.indexOf(".") >= 0) {
	    throw new S2DError("illegal name: " + _name +
	      " (cannot contain a dot ('.'))");
	}

	if (_pdbLevel < PDB_LEVEL_NONE || _pdbLevel > PDB_LEVEL_PROCESS) {
	    throw new S2DError("illegal do_pdb value: " + _pdbLevel +
	      " (must be between " + PDB_LEVEL_NONE + " and " +
	      PDB_LEVEL_PROCESS + ")");
	}

	if (_csrLevel < CSR_LEVEL_NONE || _csrLevel > CSR_LEVEL_PROCESS) {
	    throw new S2DError("illegal do_csr value: " + _csrLevel +
	      " (must be between " + CSR_LEVEL_NONE + " and " +
	      CSR_LEVEL_PROCESS + ")");
	}

	if (_lacsLevel < LACS_LEVEL_NONE ||
	  _lacsLevel > LACS_LEVEL_MANDATORY) {
	    throw new S2DError("illegal do_lacs value: " + _lacsLevel +
	      " (must be between " + LACS_LEVEL_NONE + " and " +
	      LACS_LEVEL_MANDATORY + ")");
	}
	
	if (_tarLevel < TAR_LEVEL_NONE || _tarLevel > TAR_LEVEL_PROCESS) {
	    throw new S2DError("illegal do_tar value: " + _tarLevel +
	      " (must be between " + TAR_LEVEL_NONE + " and " +
	      TAR_LEVEL_PROCESS + ")");
	}

	if (_rrTarLevel < RRTAR_LEVEL_NONE ||
	  _rrTarLevel > RRTAR_LEVEL_PROCESS) {
	    throw new S2DError("illegal do_rrtar value: " + _rrTarLevel +
	      " (must be between " + RRTAR_LEVEL_NONE + " and " +
	      RRTAR_LEVEL_PROCESS + ")");
	}

	if (_distRLevel < DISTR_LEVEL_NONE ||
	  _distRLevel > DISTR_LEVEL_PROCESS) {
	    throw new S2DError("illegal do_dist value: " + _distRLevel +
	      " (must be between " + DISTR_LEVEL_NONE + " and " +
	      DISTR_LEVEL_PROCESS + ")");
	}

	if (_rrDistRLevel < RRDISTR_LEVEL_NONE ||
	  _rrDistRLevel > RRDISTR_LEVEL_PROCESS) {
	    throw new S2DError("illegal do_rrdist value: " + _rrDistRLevel +
	      " (must be between " + RRDISTR_LEVEL_NONE + " and " +
	      RRDISTR_LEVEL_PROCESS + ")");
	}

	//
	// Set some defaults.
	//
	if (_csrDataDir == null) {
	    _csrDataDir = _dataDir;
	}

	if (_isUvd && _lacsFile == null) {
	    //TEMP -- we probably want to make this more general, but
	    // the real simple case should do for now.
	    _lacsFile = S2DUtils.replace((String)_localFiles.elementAt(0),
	      ".str", ".LACS.str");
	}

	if (_isUvd && _spartaFile == null) {
	    //TEMP -- we probably want to make this more general, but
	    // the real simple case should do for now.
	    _spartaFile = S2DUtils.replace((String)_localFiles.elementAt(0),
	      ".str", ".sparta.str");
	}

	_restraintOnly = ((_tarLevel == TAR_LEVEL_PROCESS) ||
	  (_rrTarLevel == RRTAR_LEVEL_PROCESS) ||
	  (_distRLevel == DISTR_LEVEL_PROCESS) ||
	  (_rrDistRLevel == RRDISTR_LEVEL_PROCESS)) &&
	  _masterBmrbId.equals("") &&
	  (_localFiles.size() == 0);

        if (doDebugOutput(1)) {
	    System.out.println("_name = {" + _name + "}");
	    System.out.println("_longName = {" + _longName + "}");
	    System.out.println("_masterBmrbId/BMRB ID = " + _masterBmrbId);
	    System.out.println("_cmdPdbId = " + _cmdPdbId);
	    System.out.println("_connectionFile = {" + _connectionFile + "}");
	    System.out.println("_cmdFrameIndex = " + _cmdFrameIndex);
	    System.out.println("_csrDataDir = {" + _csrDataDir + "}");
	    System.out.println("_csrTimeout = " + _csrTimeout);
	    System.out.println("_dataDir = {" + _dataDir + "}");
	    System.out.println("_csrLevel = " + _csrLevel);
	    System.out.println("_pdbLevel = " + _pdbLevel);
	    System.out.println("_lacsFile = " + _lacsFile);
	    System.out.println("_lacsLevel = " + _lacsLevel);
	    System.out.println("_rrTarLevel = " + _rrTarLevel);
	    System.out.println("_rrFile = " + _rrFile);
	    System.out.println("_tarLevel = " + _tarLevel);
	    System.out.println("_tarFile = " + _tarFile);
	    System.out.println("_tarUrl = " + _tarUrl);
	    System.out.println("_distRLevel = " + _distRLevel);
	    System.out.println("_distRFile = " + _distRFile);
	    System.out.println("_distRUrl = " + _distRUrl);
	    System.out.println("_rrDistRLevel = " + _rrDistRLevel);
	    if (_localFiles.size() > 0) {
	        System.out.println("localFile = {" +
		  _localFiles.elementAt(0) + "}");
	    }
	    System.out.println("_force = " + _force);
	    System.out.println("_htmlDir = {" + _htmlDir + "}");
	    System.out.println("_useLocalFiles = " + _useLocalFiles);
	    System.out.println("_doProteinCheck = " + _doProteinCheck);
	    System.out.println("_runScripts = " + _runScripts);
	    System.out.println("_sessionDir = {" + _sessionDir + "}");
	    System.out.println("_spartaFile = " + _spartaFile);
	    System.out.println("_extraGC = " + _extraGC);
	    System.out.println("_restraintOnly = " + _restraintOnly);
	}
    }

    // Decision table for whether to use cached PDB data:

    // Coord session file exists	n y y y y y y
    // Session file out of date		- y n n n n n
    // Summary file exists		* * y y y y n
    // Summary file out of date		* * y y n n -
    // Session file newer than summary	- * y n y n -

    // Use cached data			n n n - y n n

    // * = don't care
    // - = not defined/can't happen

    //-------------------------------------------------------------------
    // Decide whether to use cached versions of the relevant output files.
    private boolean useCache()
    {
	if (doDebugOutput(2)) {
	    System.out.println("S2DMain.useCache()");
	}

        boolean result = false;

	check:
	if (!_force) {
	    //
	    // Get peptide-cgi version, generation timestamp, etc., from
	    // the appropriate summary HTML file.
	    //
	    S2DSummaryFileData summaryData = new S2DSummaryFileData();
	    boolean summaryFileOk = summaryData.getSummaryData(_name,
	      _htmlDir, _masterBmrbId);
            if (!summaryFileOk) {
	        break check;
	    }

	    //
	    // Compare summary file version to current peptide-cgi software
	    // version.
	    //
	    if (!PEP_CGI_VERSION.equals(summaryData.fileVersion)) {
		if (doDebugOutput(2)) {
		    System.out.println("Existing summary html file version (" +
		      summaryData.fileVersion + ") does not " +
		      "match current software version (" + PEP_CGI_VERSION +
		      "); cache not used");
		}
	        break check;
	    }

	    //
	    // Compare the generation date of the summary html file to the
	    // modification date for all related NMR-STAR file(s).
	    //
	    for (int index = 0; index < summaryData.bmrbIds.size(); index++) {
	        String id = (String)summaryData.bmrbIds.elementAt(index);
		if (!id.equals("")) {
	            Date starModDate =
		      (new S2DNmrStarStdIfcFactory()).getModDate(id);
	            if (starModDate == null ||
		      starModDate.after(summaryData.fileDate)) {
		        if (doDebugOutput(2)) {
		            System.out.println("Existing summary html file " +
		              "is older than NMR-STAR file; cache not used");
		        }
	                break check;
	            }
		}
	    }

	    for (int index = 0; index < _localFiles.size(); index++) {
	        String filename = (String)_localFiles.elementAt(index);
		if (!filename.equals("")) {
	            Date starModDate =
		      S2DNmrStarIfcFactory.getModDateFile(filename);
	            if (starModDate == null ||
		      starModDate.after(summaryData.fileDate)) {
		        if (doDebugOutput(2)) {
		            System.out.println("Existing summary html file " +
		              "is older than NMR-STAR file; cache not used");
		        }
	                break check;
	            }
		}
	    }

	    //
	    // Compare the generation date of the summary html file to the
	    // modification date of any related PDB files.
	    //
	    if (_pdbLevel == PDB_LEVEL_PROCESS ||
	      _tarLevel == TAR_LEVEL_PROCESS ||
	      _rrTarLevel == RRTAR_LEVEL_PROCESS) {
		Vector pdbIds2Check = null;
	        if (_cmdPdbId != null) {
		        // PDB ID was specified on the command line -- check
		        // only that PDB ID.
		    pdbIds2Check = new Vector();
		    pdbIds2Check.addElement(_cmdPdbId);
		} else {
		        // We're processing all related PDB IDs -- check
		        // them all.
		    pdbIds2Check = summaryData.pdbIds;
		}

	        for (int index = 0; index < pdbIds2Check.size(); index++) {
	            String id = (String)pdbIds2Check.elementAt(index);
		    if (!pdbIdUseCache(summaryData, id)) {
		        break check;
		    }
	        }
	    }

	    if (_cmdPdbId != null) {
		if (_pdbLevel == PDB_LEVEL_PROCESS) {
		    if (!sessionFileUseCache(_name,
		      S2DNames.ATOMIC_COORD_SUFFIX + _cmdFrameIndex,
		      _sessionDir, summaryData)) {
		        break check;
		    }
		}

		if (_csrLevel == CSR_LEVEL_PROCESS) {
		    if (!sessionFileUseCache(_name,
		      S2DNames.CSR_SUFFIX + "1" + _cmdFrameIndex,
		      _sessionDir, summaryData)) {
		        break check;
		    }
		}

		if (_distRLevel == DISTR_LEVEL_PROCESS) {
		    if (!sessionFileUseCache(_name,
		      S2DNames.DISTR_SUFFIX + _cmdFrameIndex,
		      _sessionDir, summaryData)) {
		        break check;
		    }
		}

		if (_rrDistRLevel == RRDISTR_LEVEL_PROCESS) {
		    if (!sessionFileUseCache(_name,
		      S2DNames.RRDISTR_SUFFIX + _cmdFrameIndex,
		      _sessionDir, summaryData)) {
		        break check;
		    }
		}

		if (_tarLevel == TAR_LEVEL_PROCESS) {
		    if (!sessionFileUseCache(_name,
		      S2DNames.TAR_SUFFIX + _cmdFrameIndex,
		      _sessionDir, summaryData)) {
		        break check;
		    }
		}

		if (_rrTarLevel == RRTAR_LEVEL_PROCESS) {
		    if (!sessionFileUseCache(_name,
		      S2DNames.RRTAR_SUFFIX + _cmdFrameIndex,
		      _sessionDir, summaryData)) {
		        break check;
		    }
		}
	    }


	    result = true;
	}

	if (doDebugOutput(2)) {
	    System.out.println("S2DMain.useCache() returns " + result);
	}

	return result;
    }

    //-------------------------------------------------------------------
    /**
     * Figure out whether the cache is valid relative to a given
     * PDB ID (checking coordinate and/or torsion angle data, according
     * to what processing we're doing).
     * @param The S2DSummaryFileData for the summary html file.
     * @param The PDB ID.
     * @param True iff the cache is valid for the specified PDB ID.
     */
    private boolean pdbIdUseCache(S2DSummaryFileData summaryData,
      String id)
    {
        if (doDebugOutput(2)) {
	    System.out.println("  S2DMain.pdbIdUseCache(" + id + ")");
	}

	if (_pdbLevel == PDB_LEVEL_PROCESS) {
	    Date pdbModDate = S2DmmCifIfc.getModDate(id);
	        // Note: if a PDB ID was specified on the command
	        // line we really should compare the PDB file date
	        // against the session-specific html file here,
	        // instead of the summary html file, but I'm
	        // leaving it for now.  wenger 2004-11-03.
	    if (pdbModDate == null || pdbModDate.after(summaryData.fileDate)) {
	        if (doDebugOutput(2)) {
	            System.out.println("Existing summary html file " +
	              "is older than PDB file; cache not used");
	        }
	        return false;
	    }
	}

        if (_tarLevel == TAR_LEVEL_PROCESS) {
	    Date tarModDate = (new S2DNmrStarRGIfcFactory(true)).getModDate(id);
	        // Note: if a PDB ID was specified on the command
	        // line we really should compare the PDB file date
	        // against the session-specific html file here,
	        // instead of the summary html file, but I'm
	        // leaving it for now.  wenger 2004-11-03.
	    if (tarModDate == null || tarModDate.after(summaryData.fileDate)) {
	        if (doDebugOutput(2)) {
	            System.out.println("Existing summary html file is " +
		      "older than torsion angle restraint file; cache " +
		      "not used");
	        }
	        return false;
	    }
	}

        if (_rrTarLevel == RRTAR_LEVEL_PROCESS) {
	    Date rrTarModDate = (new S2DNmrStarRRIfcFactory()).getModDate(id);
	        // Note: if a PDB ID was specified on the command
	        // line we really should compare the PDB file date
	        // against the session-specific html file here,
	        // instead of the summary html file, but I'm
	        // leaving it for now.  wenger 2004-11-03.
	    if (rrTarModDate == null ||
	      rrTarModDate.after(summaryData.fileDate)) {
	        if (doDebugOutput(2)) {
	            System.out.println("Existing summary html file is " +
		      "older than remediated restraints file; cache " +
		      "not used");
	        }
	        return false;
	    }
	}

	//TEMP -- check distance restraint file here

	return true;
    }

    //-------------------------------------------------------------------
    // Determine whether the given session file allows us to use the
    // cache.  (Note that if *any* test shows the cache to be invalid,
    // we consider the cache invalid.)
    private boolean sessionFileUseCache(String name, String suffix,
      String sessionDir, S2DSummaryFileData summaryData)
    {
        if (doDebugOutput(2)) {
	    System.out.println("  S2DMain.sessionFileUseCache(" + name +
	      ", " + suffix + ")");
	}

        //
	// Get peptide-cgi version and generation timestamp from
	// the appropriate session file.
	//
	S2DSessionFileData sessionData = new S2DSessionFileData();
	boolean sessionFileOk = sessionData.getSessionData(name, suffix,
	  sessionDir);
        if (!sessionFileOk) {
	    if (doDebugOutput(2)) {
	        System.out.println("Unable to get session file data; " +
	          "cache not used");
	    }
	    return false;
	}

	//
	// Compare session file version to current peptide-cgi software
	// version.
	//
	if (!PEP_CGI_VERSION.equals(sessionData.fileVersion)) {
	    if (doDebugOutput(2)) {
	        System.out.println("Existing session file version (" +
	          sessionData.fileVersion + ") does not " +
	          "match current software version (" + PEP_CGI_VERSION +
	          "); cache not used");
	    }
	    return false;
	}

	//
	// If the session file is not newer than the summary file,
	// don't use the cached coordinate data.
	//
	if (!sessionData.fileDate.after(summaryData.fileDate)) {
	    if (doDebugOutput(2)) {
	        System.out.println("Existing session is not newer " +
		  "than summary file; cache not used");
	    }
	    return false;
	}

	return true;
    }

    //-------------------------------------------------------------------
    // Add an accession number to the list to be processed (avoiding
    // duplicates).
    private void addBmrbId(String bmrbId)
    {
        if (doDebugOutput(3)) {
            System.out.println("addBmrbId(" + bmrbId + ")");
        }

        if (!_bmrbIds.contains(bmrbId)) {
	    _bmrbIds.add(bmrbId);
	}
    }

    //-------------------------------------------------------------------
    // Add PDB IDs to the list of PDB files to be processed.
    private void addPDB(Vector ids)
    {
        if (doDebugOutput(3)) {
            System.out.println("addPDB()");
        }

	for (int index = 0; index < ids.size(); index++) {
	    String id = ((String)ids.elementAt(index)).toUpperCase();

	    // Use only the first PDB ID.
	    //TEMP -- comment this out to process all relevant PDB IDs.
	    // if (_pdbIds.size() > 0) break;

	    // Avoid duplicates.
	    if (!_pdbIds.contains(id)) {
	        _pdbIds.addElement(id);
	    }
	}

	// If there's no chem shift reference PDB ID already specified, 
	// save all of the PDB IDs in the chem shift ref list.
	if (_csrPdbIds.size() == 0) {
	    for (int index = 0; index < _pdbIds.size(); index++) {
	        String id = ((String)_pdbIds.elementAt(index)).toUpperCase();
		_csrPdbIds.addElement(id);
	    }
	}
	
	if (doDebugOutput(2)) {
	    System.out.print("Coordinate PDB IDs: ");
	    for (int index = 0; index < _pdbIds.size(); index++) {
	        System.out.print((String)_pdbIds.elementAt(index) + ", ");
	    }
	    System.out.println("");

	    System.out.print("Chem shift ref PDB IDs: ");
	    for (int index = 0; index < _csrPdbIds.size(); index++) {
	        System.out.print((String)_csrPdbIds.elementAt(index) + ", ");
	    }
	    System.out.println("");
	}
    }

    //-------------------------------------------------------------------
    // Do the processing.
    // Note: this whole method could use a bunch of cleaing up in terms
    // of how we decide to do the different types of processing (regular
    // BMRB, PDB coordinates, chem shift reference).
    private void process() throws S2DException
    {
        if (doDebugOutput(3)) {
            System.out.println("process()");
        }

	if (_cmdFrameIndex != -1) {
	    // If we get here, it means that coord_index was specified on the
	    // command line.

	    if (_cmdFrameIndex < 0) {
		throw new S2DError("coord_index not specified!");
	    }

	    // Note: we create a "junk" summary file here because some
	    // of the existing code can't write the atomic coordinate
	    // stuff without writing to a summary file.  wenger 2003-09-12.
	    // If we're doing restraint-only processing, this is the
	    // real summary file, though.
	    String tmpSummaryFile = _name;
	    if (!_restraintOnly) {
	        tmpSummaryFile = _name + "tmp" + _cmdFrameIndex;
	    }
	    String id = _masterBmrbId;
	    if (_restraintOnly) {
	    	id = _cmdPdbId;
	    }
	    _summary = new S2DSummaryHtml(tmpSummaryFile, _longName,
	      id, _localFiles, _htmlDir, "starFileName",
	      "systemName", "frameTitle", _restraintOnly);

	    //
	    // Do the coordinate processing.
	    //
	    if (_pdbLevel == PDB_LEVEL_PROCESS) {
		_currentFrameIndex = _cmdFrameIndex;
	        process1PDB(null, _cmdPdbId);
	    }

	    //
	    // Do the chem shift reference processing.
	    //
	    if (_csrLevel == CSR_LEVEL_PROCESS) {
	        S2DChemShiftRef csr = new S2DChemShiftRef(_name, _longName,
		  _dataDir, _csrDataDir, _sessionDir, _masterBmrbId,
		  _localFiles, _cmdPdbId, _summary, _cmdFrameIndex,
		  _csrTimeout, "", "");
	        csr.run();
	        csr.postProcess(_doProteinCheck);
		if (!csr.ran() || csr.ranButFailed()) {
		    System.err.println(new S2DError(
		      "Chem shift reference calculation failed"));
		}
	    }

	    //
	    // Do the torsion angle processing.
	    //
	    if (_tarLevel == TAR_LEVEL_PROCESS) {
		_currentFrameIndex = _cmdFrameIndex;
	        process1TAR(_tarFile, _tarUrl, _cmdPdbId);
	    }

	    if (_rrTarLevel == RRTAR_LEVEL_PROCESS) {
		_currentFrameIndex = _cmdFrameIndex;
	        process1RRCoordSet(_rrFile, _cmdPdbId);
	        process1RRTAR(_rrFile, _cmdPdbId);
	    }

	    //
	    // Do the distance restraint processing.
	    //
	    if (_distRLevel == DISTR_LEVEL_PROCESS) {
		_currentFrameIndex = _cmdFrameIndex;
	        process1DistR(_distRFile, _distRUrl, _cmdPdbId);
	    }

	    if (_rrDistRLevel == RRDISTR_LEVEL_PROCESS) {
		_currentFrameIndex = _cmdFrameIndex;
	        process1RRCoordSet(_rrFile, _cmdPdbId);
	        process1RRDistR(_rrFile, _cmdPdbId);
	    }

	    _summary.close(null, null, "");

	    // Note: we should delete this "junk" summary file here, but
	    // File.delete() doesn't seem to work!  wenger 2003-09-12.

	    runSetModes();

	    return;
	}

        _currentFrameIndex = 1;

	//
	// Delete any existing session files, so the user can't change
	// to invalid ones in the JavaScreen.
	//
        deleteSessions();

	//
	// Create the summary HTML file.
	//
	S2DNmrStarIfc masterStar = null;
	if (!_masterBmrbId.equals("")) {
            masterStar = (new S2DNmrStarStdIfcFactory()).createFromId(
	      _masterBmrbId, _useLocalFiles);

	    _summary = new S2DSummaryHtml(_name, _longName, _masterBmrbId,
	      _localFiles, _htmlDir, masterStar.getFileName(),
	      masterStar.getSystemName(), masterStar.getEntryTitle(),
	      _restraintOnly);
	} else {
	    _summary = new S2DSummaryHtml(_name, _longName, "99999",
	      _localFiles, _htmlDir, _name, ""/*TEMP?*/, ""/*TEMP?*/,
	      _restraintOnly);
	}

	//
	// Initiate chemical shift reference processing.
	//
	S2DChemShiftRef csr = null;
	if (_csrPdbIds.size() != 0 && _csrLevel == CSR_LEVEL_PROCESS) {
	    csr = new S2DChemShiftRef(_name, _longName, _dataDir, _csrDataDir,
	      _sessionDir, _masterBmrbId, _localFiles,
	      (String)_csrPdbIds.elementAt(0), _summary, 1, _csrTimeout, "",
	      "");
	    csr.run();
	}

	//
	// Process all BMRB entries.
	//

	// Note: star seems to hold the *last* NMR-STAR file we process,
	// which we need for processing PDB entries later on.
        S2DStarIfc star = null;
	String starVersion = null;

	if (!_masterBmrbId.equals("")) {
	    star = processAllBMRBs(masterStar);
	    starVersion = star.version();
	}

	//
	// Process all local files.
	//
	if (_localFiles.size() > 0) {
	    star = processLocalFiles();
	    if (starVersion == null) starVersion = star.version();
	}

	//
	// Initiate chemical shift reference processing, if we didn't
	// before and got a PDB ID from the NMR-STAR file.
	//
	if (csr == null && _csrPdbIds.size() != 0 &&
	  _csrLevel == CSR_LEVEL_PROCESS) {
	    csr = new S2DChemShiftRef(_name, _longName, _dataDir, _csrDataDir,
	      _sessionDir, _masterBmrbId, _localFiles,
	      (String)_csrPdbIds.elementAt(0), _summary, 1, _csrTimeout, "",
	      star.version());
	    csr.run();
	}

	//TEMP -- maybe process additional CSRs here??

	//
	// Generate the summary data needed for selecting a data set in
	// the 3D visualizations.
	//
	S2DSummaryData summDat = new S2DSummaryData(_name, _dataDir,
	  _dataSets);
	summDat.writeSummaryData();

	if (_haveCoords) {
	    // If we have coordinates here we don't want to use the
	    // coordinates from PDB.
	    _pdbIds.removeAllElements();
	}

	//
	// Process all PDB entries.
	//
        processAllPDBs(star);

	//
	// Do the chemical shift reference post-processing if necessary.
	//
	if (csr != null) {
	    csr.postProcess(_doProteinCheck);
	    if (csr.ranButFailed()) {
		//
		// The chemical shift processing ran but failed.  Try it
		// on the next PDB ID.
		// Note: we really should move this off into a separate
		// thread to speed up processing (we'll spend a lot of time
		// just waiting for the chem shift reference processing
		// here), but I'm doing it the simple way first.
		// wenger 2004-02-05.
		//
		boolean retry = true;
		for (int csrIndex = 1; csrIndex < _csrPdbIds.size() && retry;
		  ++csrIndex) {
		    String csrPdb = (String)_csrPdbIds.elementAt(csrIndex);
	            csr = new S2DChemShiftRef(_name, _longName, _dataDir,
		      _csrDataDir, _sessionDir, _masterBmrbId, _localFiles,
	              csrPdb, _summary, csrIndex + 1, _csrTimeout, "",
		      star.version());
	            csr.run();
	            csr.postProcess(_doProteinCheck);
		    retry = csr.ranButFailed();
		}
	    }
	}

	//
	// Here we're creating the link-only chem shift reference
	// entries on the summary page.
	//
	if (_csrLevel == CSR_LEVEL_LINK) {
	    for (int csrIndex = 0; csrIndex < _csrPdbIds.size(); csrIndex++) {
		String csrPdb = (String)_csrPdbIds.elementAt(csrIndex);
	        try {
		    _summary.startFrame(
		      "Chemical Shift Referencing Visualizations" +
		      " (calculated chem shifts from PDB " + csrPdb + ")");

	            _summary.writeChemShiftRefCGI(csrPdb, csrIndex + 1);
                } catch(IOException ex) {
                    System.err.println(
		      "IOException writing chem shift ref link: " +
		      ex.toString());
	            throw new S2DError("Can't write chem shift ref link");
	        } finally {
	            _summary.endFrame();
	        }
	    }
	}

	processLACS();

	_summary.close(_bmrbIds, _pdbIds, starVersion);
	_summary = null;

	runSetModes();
    }

    //-------------------------------------------------------------------
    // Process all specified BMRB entries.
    private S2DNmrStarIfc processAllBMRBs(S2DNmrStarIfc masterStar)
      throws S2DException
    {
        S2DNmrStarIfc star = masterStar;

        for (int index = 0; index < _bmrbIds.size(); index++) {
	    String bmrbId = (String)_bmrbIds.elementAt(index);

	    // TEMP -- remove hard-coded links once we establish some
	    // kind of convention for the _Related_BMRB_accession_number
	    // info.

	    // Note: 4038 has chem shifts for 4096.
            if (bmrbId.equals("4038")) {
	        addBmrbId("4096");
	    } else if (bmrbId.equals("4096")) {
	        addBmrbId("4038");
	    }

	    if (index > 0) { // avoid parsing the "master" file twice
                star = (new S2DNmrStarStdIfcFactory()).createFromId(bmrbId,
		  _useLocalFiles);
	    }

            process1NmrStar(star);
        }

	return star;
    }

    //-------------------------------------------------------------------
    // Process all specified local files.
    // TEMP -- as of 2003-04-09, this really only works right on a single
    // file. wenger
    private S2DStarIfc processLocalFiles() throws S2DException
    {
	S2DNmrStarIfc star = null;

        for (int index = 0; index < _localFiles.size(); index++) {
	    star = (new S2DNmrStarStdIfcFactory()).createFromFile(
	      (String)_localFiles.elementAt(index));

	    process1NmrStar(star);
	}

	return star;
    }

    //-------------------------------------------------------------------
    // Process all specified PDB entries.
    private void processAllPDBs(S2DStarIfc star)
    {
        if (doDebugOutput(2)) {
	    System.out.println("processAllPDBs()");
	}

	for (int index = 0; index < _pdbIds.size(); index++) {
	    String id = (String)_pdbIds.elementAt(index);

	    // We do the same thing here for PDB_LEVEL_LINK and
	    // PDB_LEVEL_PROCESS because they are split out inside
	    // process1PDB().
	    if (_pdbLevel == PDB_LEVEL_LINK ||
	      _pdbLevel == PDB_LEVEL_PROCESS) {
		try {
	            process1PDB(star, id);
		} catch(S2DException ex) {
		    System.err.println("Unable to process PDB ID " + id +
		      " (" + ex.toString() + ")");
		    _summary.writeMessage("Unable to get atomic coordinates" +
		      " from related PDB ID " + id, false);
		}
	    } else {
	        if (_retrying) {
		    _summary.writeMessage("Unable to get atomic " +
		      " coordinates from related PDB ID " + id + " because " +
		      "of insufficient memory", false);
	        } else {
		    _summary.writeMessage("Atomic coordinates from related " +
		      "PDB ID " + id + " not read", false);
	        }
	    }

	    // We do the same thing here for TAR_LEVEL_LINK and
	    // TAR_LEVEL_PROCESS because they are split out inside
	    // process1TAR().
	    if (_tarLevel == TAR_LEVEL_LINK_CHECK ||
	      _tarLevel == TAR_LEVEL_LINK ||
	      _tarLevel == TAR_LEVEL_PROCESS) {
		try {
	            process1TAR(null, null, id);
		} catch(S2DException ex) {
		    System.err.println("Unable to process PDB ID " + id +
		      " (" + ex.toString() + ")");
		    System.err.println("Unable to get RG torsion angle " +
		      "restraints from related PDB ID " + id);
		}
	    } else {
	        if (_retrying) {
		    System.err.println("Unable to get RG torsion angle " +
		      " restraints from related PDB ID " + id + " because " +
		      "of insufficient memory");
	        } else {
		    System.err.println("RG torsion angle restraints " +
		      "from related PDB ID " + id + " not read");
	        }
	    }

	    // We do the same thing here for RRTAR_LEVEL_LINK and
	    // RRTAR_LEVEL_PROCESS because they are split out inside
	    // process1TAR().
	    if (_rrTarLevel == RRTAR_LEVEL_LINK_CHECK ||
	      _rrTarLevel == RRTAR_LEVEL_LINK ||
	      _rrTarLevel == RRTAR_LEVEL_PROCESS) {
		try {
	            process1RRCoordSet(null, id);
	            process1RRTAR(null, id);
		} catch(S2DException ex) {
		    System.err.println("Unable to process PDB ID " + id +
		      " (" + ex.toString() + ")");
		    System.err.println("Unable to get RR torsion angle " +
		      "restraints from related PDB ID " + id);
		}
	    } else {
	        if (_retrying) {
		    System.err.println("Unable to get RR torsion angle " +
		      " restraints from related PDB ID " + id + " because " +
		      "of insufficient memory");
	        } else {
		    System.err.println("RR torsion angle restraints " +
		      "from related PDB ID " + id + " not read");
	        }
	    }

	    // We do the same thing here for DISTR_LEVEL_LINK and
	    // DISTR_LEVEL_PROCESS because they are split out inside
	    // process1DistR().
	    if (_distRLevel == DISTR_LEVEL_LINK_CHECK ||
	      _distRLevel == DISTR_LEVEL_LINK ||
	      _distRLevel == DISTR_LEVEL_PROCESS) {
		try {
	            process1DistR(null, null, id);
		} catch(S2DException ex) {
		    System.err.println("Unable to process PDB ID " + id +
		      " (" + ex.toString() + ")");
		    System.err.println("Unable to get RG distance " +
		      "restraints from related PDB ID " + id);
		}
	    } else {
	        if (_retrying) {
		    System.err.println("Unable to get RG distance " +
		      " restraints from related PDB ID " + id + " because " +
		      "of insufficient memory");
	        } else {
		    System.err.println("RG distance restraints " +
		      "from related PDB ID " + id + " not read");
	        }
	    }

	    // We do the same thing here for RRDISTR_LEVEL_LINK and
	    // RRDISTR_LEVEL_PROCESS because they are split out inside
	    // process1RRDistR().
	    if (_rrDistRLevel == RRDISTR_LEVEL_LINK_CHECK ||
	      _rrDistRLevel == RRDISTR_LEVEL_LINK ||
	      _rrDistRLevel == RRDISTR_LEVEL_PROCESS) {
		try {
	            process1RRCoordSet(null, id);
	            process1RRDistR(null, id);
		} catch(S2DException ex) {
		    System.err.println("Unable to process PDB ID " + id +
		      " (" + ex.toString() + ")");
		    System.err.println("Unable to get RR distance " +
		      "restraints from related PDB ID " + id);
		}
	    } else {
	        if (_retrying) {
		    System.err.println("Unable to get RR distance " +
		      " restraints from related PDB ID " + id + " because " +
		      "of insufficient memory");
	        } else {
		    System.err.println("RR distance restraints " +
		      "from related PDB ID " + id + " not read");
	        }
	    }

	    _currentFrameIndex++;
	}
    }

    //-------------------------------------------------------------------
    // Process the LACS file (if any).
    private void processLACS() throws S2DException
    {
        if (doDebugOutput(2)) {
	    System.out.println("processLACS()");
	    System.out.println("_lacsFile = " + _lacsFile);
	}

	if (_lacsLevel >= LACS_LEVEL_TRY) {
	    try {
	        S2DNmrStarIfc lacsStar;
	        if (_lacsFile != null) {
	            lacsStar = (new S2DNmrStarLacsIfcFactory()).
		      createFromFile(_lacsFile);
	        } else {
	            lacsStar = (new S2DNmrStarLacsIfcFactory()).
		      createFromId(_masterBmrbId, _useLocalFiles);
	        }

		saveLACS(lacsStar);
	    } catch(S2DException ex) {
	        System.err.println("Error processing LACS data: " + ex);
		S2DError err = new S2DError("LACS processing failed");
	        if (_lacsLevel >= LACS_LEVEL_MANDATORY) {
		    throw err;
		} else {
		    System.err.println("Warning: " + err);
		}
	    }
	}
    }

    //-------------------------------------------------------------------
    // Make sure that all of the files that we generated are world-readable.
    private void runSetModes()
    {
	if (_runScripts) {
	    try {
	        Runtime currentRT = Runtime.getRuntime();
	        Process ps = currentRT.exec("." + File.separator +
		  "set_modes " + _name + " " + (_isUvd ? "1" : "0"));
	        ps.waitFor();
	        int result = ps.exitValue();
	        if (result != 0) {
	            throw new S2DWarning("Non-zero exit value from " +
		      "set_modes; see tmp/set_modes.out");
	        }
	    } catch (Exception ex) {
	        System.err.println("Error setting file permissions: " +
	          ex.toString());
	    }
	}
    }

    //-------------------------------------------------------------------
    // Delete any existing sessions for the current entry.
    private void deleteSessions()
    {
	if (_runScripts) {
	    try {
	        Runtime currentRT = Runtime.getRuntime();
	        Process ps = currentRT.exec("." + File.separator +
		  "delete_sessions " + _name + " " + (_isUvd ? "1" : "0"));
	        ps.waitFor();
	        int result = ps.exitValue();
	        if (result != 0) {
	            throw new S2DWarning("Non-zero exit value from " +
		      "delete_sessions; see tmp/delete_sessions.out");
	        }
	    } catch (Exception ex) {
	        System.err.println("Error deleting old session files: " +
	          ex.toString());
	    }
	}
    }

    //-------------------------------------------------------------------
    // Process a given NMR-STAR file entry.
    private void process1NmrStar(S2DNmrStarIfc star) throws S2DException
    {
	if (doDebugOutput(2)) {
	    System.out.println("S2DMain.process1NmrStar(" + star + ")");
	}

        Vector ids = star.getPdbIds(_doProteinCheck);
        addPDB(ids);

        //TEMP -- do I really want to skip stuff if I get an error?

	if (doDebugOutput(2)) S2DUtils.printMemory(
	  "Before saveAllResidueLists()");

        saveAllResidueLists(star);
	if (_extraGC) System.gc();
	if (doDebugOutput(2)) S2DUtils.printMemory(
	  "After saveAllResidueLists()");

        saveChemShifts(star);
	if (_extraGC) System.gc();
	if (doDebugOutput(2)) S2DUtils.printMemory(
	  "After saveChemShifts()");

        saveSpartaDeltaShifts(star);
	if (_extraGC) System.gc();
	if (doDebugOutput(2)) S2DUtils.printMemory(
	  "After saveDeltaShifts()");

        saveT1Relaxation(star);
	if (_extraGC) System.gc();
	if (doDebugOutput(2)) S2DUtils.printMemory(
	  "After saveT1Relaxation()");

        saveT2Relaxation(star);
	if (_extraGC) System.gc();
	if (doDebugOutput(2)) S2DUtils.printMemory(
	  "After saveT2Relaxation()");

        saveHetNOE(star);
	if (_extraGC) System.gc();
	if (doDebugOutput(2)) S2DUtils.printMemory("After saveHetNOE()");

        saveHExchangeRate(star);
	if (_extraGC) System.gc();
	if (doDebugOutput(2)) S2DUtils.printMemory(
	  "After saveHExchangeRate()");

        saveCoupling(star);
	if (_extraGC) System.gc();
	if (doDebugOutput(2)) S2DUtils.printMemory("After saveCoupling()");

        saveHExchangeProtFact(star);
	if (_extraGC) System.gc();
	if (doDebugOutput(2)) S2DUtils.printMemory(
	  "After saveHExchangeProtFact()");

        saveS2Params(star);
	if (_extraGC) System.gc();
	if (doDebugOutput(2)) S2DUtils.printMemory("After saveS2Params()");

        save3DDataSources();
	if (_extraGC) System.gc();
	if (doDebugOutput(2)) S2DUtils.printMemory(
	  "After save3DDataSources()");

        saveAtomicCoords(star);
	if (_extraGC) System.gc();
	if (doDebugOutput(2)) S2DUtils.printMemory(
	  "After saveAtomicCoords()");
    }

    //-------------------------------------------------------------------
    /**
     * Write out a list of all residues for all entity assembly IDs in
     * the given NMR-STAR file.
     * @param The star file
     */
    private void saveAllResidueLists(S2DNmrStarIfc star) throws S2DException
    {
        if (doDebugOutput(3)) {
	    System.out.println("  S2DMain.saveAllResidueLists()");
	}

	Vector frames = star.getAllEntityAssemblyFrames();

	boolean wroteAtLeastOne = false;
	try {
	    FileWriter resWriter = S2DFileWriter.create(_dataDir +
	      File.separator + _name + S2DNames.RES_LIST_SUFFIX +
	      S2DNames.DAT_SUFFIX);
	    resWriter.write("# Data: residue list for " + _name + "\n");
	    resWriter.write("# Schema: bmrb-ResList\n");
	    resWriter.write("# Attributes: Entity_assembly_ID; " +
	      "Residue_seq_code; ResLabel\n");
	    resWriter.write("# Peptide-CGI version: " +
	      S2DMain.PEP_CGI_VERSION + "\n");
	    resWriter.write("# Generation date: " +
	      S2DMain.getTimestamp() + "\n");
	    resWriter.write("#\n");

	    int entityAssemblyId = 0;
	    for (int frameNum = 0; frameNum < frames.size(); frameNum++) {
	        entityAssemblyId++;
	        SaveFrameNode frame = (SaveFrameNode)frames.elementAt(frameNum);

		// Don't do anything for non-polymers.
		int polymerType = star.getPolymerType(frame);
		if (polymerType == S2DResidues.POLYMER_TYPE_NONE) continue;

		try {
	            S2DResidues residues = star.getResidues(frame);

	            for (int resNum = 0; resNum < residues._resSeqCodes.length;
	              resNum++) {
	                resWriter.write(entityAssemblyId + "\t" +
		          residues._resSeqCodes[resNum] + "\t" +
		          residues._resLabels[resNum] + "\n");
	            }
		    // Flush here is VITAL to get stuff actually written out
		    // if a later entity assembly fails...
		    resWriter.flush();
    
	            wroteAtLeastOne = true;

		} catch (S2DException ex) {
		    System.err.println("Warning (" + ex.toString() +
		      ") writing residue list for entity assembly ID " +
		      entityAssemblyId + " (frame " + star.getFrameName(frame) + ")");
		}
	    }
	    resWriter.close();

	} catch (Exception ex) {
	    System.err.println("Exception writing all residue lists: " +
	      ex.toString());
	}
	if (!wroteAtLeastOne) {
	    throw new S2DError("Error: unable to write any residue lists");
	}
    }

    //-------------------------------------------------------------------
    // Save all chem shift data for this entry.
    // Note: this can be tested with 4264.
    private void saveChemShifts(S2DNmrStarIfc star) throws S2DException
    {
        if (doDebugOutput(3)) {
	    System.out.println("  S2DMain.saveChemShifts()");
	}

	Enumeration frameList = star.getDataFramesByCat(
	  star.CHEM_SHIFT_SF_CAT, star.ASSIGNED_CHEM_SHIFTS);

	int frameIndex = 1;
        while (frameList.hasMoreElements()) {
	    SaveFrameNode frame = (SaveFrameNode)frameList.nextElement();

	    Vector entityAssemblyIDList = star.getUniqueTagValues(
	      frame, star.CHEM_SHIFT_ENTITY_ASSEMBLY_ID);
	    for (int index = 0; index < entityAssemblyIDList.size(); index++) {
	        String entityAssemblyID =
		  (String)entityAssemblyIDList.get(index);
		String entityID = star.entAssemID2entID(entityAssemblyID);

	        // We only want to output chemical shifts corresponding to a
	        // polymer.
	        if (!star.refersToPolymer(frame, entityID)) {
                    if (doDebugOutput(2)) {
                        System.out.println("Chemical shifts not saved for " +
                          "save frame " + frame.getLabel() + " (" +
			  entityAssemblyID + ") because it is not a polymer");
                    }
		} else {
		    //TEMP -- do I really want to catch the error here (and in
		    // other similar methods below)?
		    try {
	                S2DResidues residues = getFrameResCounts(star, frame,
		          entityID, frameIndex);
	                saveFrameChemShifts(star, frame, entityAssemblyID,
			  residues, frameIndex);
	                saveFramePistachio(star, frame, entityAssemblyID,
			  residues, frameIndex);
	                saveFrameAmbiguity(star, frame, entityAssemblyID,
			  residues, frameIndex);
		    } catch(S2DException ex) {
		        System.err.println("Exception saving chem shifts for " +
		          "frame " + star.getFrameName(frame) + " (" +
			  entityAssemblyID + "): " + ex.toString());
		    }
		    frameIndex++;
	        }
	    }
	}
    }

    //-------------------------------------------------------------------
    // Save all pre-calculated SPARTA-calculated deltashifts for this
    // entry.
    private void saveSpartaDeltaShifts(S2DNmrStarIfc star)
      throws S2DException
    {
        if (doDebugOutput(3)) {
	    System.out.println("  S2DMain.saveSpartaDeltaShifts()");
	}

	boolean checkResListTmp = _checkResList;
	try {
	    // Leave this for now -- we may need to get rid of it when
	    // SPARTA can deal with multiple entities.
	    _checkResList = false;

	    S2DNmrStarIfc spartaStar = null;
	    if (_spartaFile != null) {
	        spartaStar = (new S2DNmrStarSpartaIfcFactory()).
		  createFromFile(_spartaFile);
	    } else {
	        spartaStar = (new S2DNmrStarSpartaIfcFactory()).
		  createFromId(_masterBmrbId, _useLocalFiles);
	    }

	    // Note: average must be done first, because the
	    // single-mode code expects to append to files
	    // created by the average.
            doSaveSpartaDeltaShifts(star, spartaStar, true);
            doSaveSpartaDeltaShifts(star, spartaStar, false);
	} catch (Exception ex) {
	    System.err.println("Error processing SPARTA-calculated " +
	      "deltashifts: " + ex.toString());
	} finally {
	    _checkResList = checkResListTmp;
	}
    }

    //-------------------------------------------------------------------
    // Save SPARTA delta shifts for either all models or the average
    // (split out like this because the save frames have different
    // category names).
    private void doSaveSpartaDeltaShifts(S2DNmrStarIfc mainStar,
      S2DNmrStarIfc spartaStar, boolean isAvg)
      throws S2DException
    {
        if (doDebugOutput(3)) {
	    System.out.println("  S2DMain.doSaveSpartaDeltaShifts(" +
	      isAvg + ")");
	}

	boolean append = !isAvg;

	SaveFrameNode frame = spartaStar.getSpartaFrame(isAvg);

	if (frame == null) {
	    String type = isAvg ? "average" : "single";
	    throw new S2DError("No SPARTA (" + type + ") save frame found!");
	}

	Vector entityAssemblyIDList = spartaStar.getUniqueTagValues(
	  frame, spartaStar.DELTA_SHIFT_ENTITY_ASSEMBLY_ID);
	Vector chemShiftListIDList = spartaStar.getUniqueTagValues(
	  frame, spartaStar.DELTA_CHEM_SHIFT_LIST_ID);

	int frameIndex = 1;
	for (int index = 0; index < entityAssemblyIDList.size();
	  index++) {
	    String entityAssemblyID =
	      (String)entityAssemblyIDList.get(index);
	    String entityID = mainStar.entAssemID2entID(entityAssemblyID);

	    // I'm pretty sure this will fail if the main STAR file
	    // and the SPARTA star file are different versions.
	    // wenger 2009-08-19
	    S2DResidues residues = getFrameResCounts(mainStar, frame,
	      entityID, 0);

	    for (int cslIndex = 0; cslIndex < chemShiftListIDList.size();
	      cslIndex++) {
	        String chemShiftListID =
	          (String)chemShiftListIDList.get(cslIndex);

	        int minModelNum = 0;
	        int maxModelNum = 0;
	        if (!isAvg) {
		    minModelNum = 1;
		    maxModelNum = getSpartaMaxModelNum(spartaStar, frame,
		      entityAssemblyID, chemShiftListID);
	        }
	        for (int modelNum = minModelNum; modelNum <= maxModelNum;
	          modelNum++) {
	            try {
	                saveFrameSpartaDeltaShifts(spartaStar, frame,
		          entityAssemblyID, chemShiftListID, modelNum,
			  residues, frameIndex, append);
		    } catch (Exception ex) {
		        System.err.println("Warning: error saving SPARTA " +
		          "delta shifts for save frame " +
		          spartaStar.getFrameName(frame) + " (" +
		          entityAssemblyID + ", " + modelNum + "): " +
		      ex.toString());
		    }
	        }

	        frameIndex++;
            }
	}
    }

    //-------------------------------------------------------------------
    // Save all T1 relaxation data for this entry.
    // Note: this can be tested with 4267.
    private void saveT1Relaxation(S2DNmrStarIfc star) throws S2DException
    {
        if (doDebugOutput(3)) {
	    System.out.println("  S2DMain.saveT1Relaxation()");
	}

	Enumeration frameList = star.getDataFramesByCat(star.T1_RELAX_SF_CAT,
	  star.T1_RELAX);

	int frameIndex = 1;
        while (frameList.hasMoreElements()) {
	    SaveFrameNode frame = (SaveFrameNode)frameList.nextElement();
	    Vector entityAssemblyIDList = star.getUniqueTagValues(
	      frame, star.T1_ENTITY_ASSEMBLY_ID);
	    for (int index = 0; index < entityAssemblyIDList.size(); index++) {
		String entityAssemblyID =
		  (String)entityAssemblyIDList.get(index);

		String entityID = star.entAssemID2entID(entityAssemblyID);
	        if (!star.refersToPolymer(frame, entityID)) {
                    if (doDebugOutput(2)) {
                        System.out.println("T1 relaxation not saved for " +
                          "save frame " + frame.getLabel() + " (" +
			  entityAssemblyID + ") because it is not a polymer");
                    }
		    continue;
		}

	        try {
	            saveFrameT1Relax(star, frame, entityAssemblyID,
		      frameIndex);
	        } catch(S2DException ex) {
		    System.err.println("Exception saving T1 relaxation " +
		      "for frame " + star.getFrameName(frame) +
		      ": " + ex.toString());
	        }

	        frameIndex++;
	    }
        }
    }

    //-------------------------------------------------------------------
    // Save all T2 relaxation data for this entry.
    // Note: this can be tested with 4267.
    private void saveT2Relaxation(S2DNmrStarIfc star) throws S2DException
    {
        if (doDebugOutput(3)) {
	    System.out.println("  S2DMain.saveT2Relaxation()");
	}

	Enumeration frameList = star.getDataFramesByCat(star.T2_RELAX_SF_CAT,
	  star.T2_RELAX);

	int frameIndex = 1;
        while (frameList.hasMoreElements()) {
	    SaveFrameNode frame = (SaveFrameNode)frameList.nextElement();
	    Vector entityAssemblyIDList = star.getUniqueTagValues(
	      frame, star.T2_ENTITY_ASSEMBLY_ID);
	    for (int index = 0; index < entityAssemblyIDList.size(); index++) {
		String entityAssemblyID =
		  (String)entityAssemblyIDList.get(index);

		String entityID = star.entAssemID2entID(entityAssemblyID);
	        if (!star.refersToPolymer(frame, entityID)) {
                    if (doDebugOutput(2)) {
                        System.out.println("T2 relaxation not saved for " +
                          "save frame " + frame.getLabel() + " (" +
			  entityAssemblyID + ") because it is not a polymer");
                    }
		    continue;
		}

	        try {
	            saveFrameT2Relax(star, frame, entityAssemblyID,
		      frameIndex);
	        } catch(S2DException ex) {
		    System.err.println("Exception saving T2 relaxation " +
		      "for frame " + star.getFrameName(frame) +
		      ": " + ex.toString());
	        }

	        frameIndex++;
	    }
        }
    }

    //-------------------------------------------------------------------
    // Save all heteronuclear NOE values for this entry.
    // Note: this can be tested with 4267.
    private void saveHetNOE(S2DNmrStarIfc star) throws S2DException
    {
        if (doDebugOutput(3)) {
	    System.out.println("  S2DMain.saveHetNOE()");
	}

	Enumeration frameList = star.getDataFramesByCat(star.HET_NOE_SF_CAT,
	  star.HETERONUCLEAR_NOE);

	int frameIndex = 1;
        while (frameList.hasMoreElements()) {
	    SaveFrameNode frame = (SaveFrameNode)frameList.nextElement();
	    Vector entityAssemblyIDList = star.getUniqueTagValues(
	      frame, star.HET_NOE_ENTITY_ASSEMBLY_ID_1);
	    for (int index = 0; index < entityAssemblyIDList.size(); index++) {
		String entityAssemblyID =
		  (String)entityAssemblyIDList.get(index);

		String entityID = star.entAssemID2entID(entityAssemblyID);
	        if (!star.refersToPolymer(frame, entityID)) {
                    if (doDebugOutput(2)) {
                        System.out.println("Heteronuclear NOE not saved for " +
                          "save frame " + frame.getLabel() + " (" +
			  entityAssemblyID + ") because it is not a polymer");
                    }
		    continue;
		}

	        try {
	            saveFrameHetNOE(star, frame, entityAssemblyID,
		      frameIndex);
	        } catch(S2DException ex) {
		    System.err.println("Exception saving heternuclear " +
		      "NOE for frame " + star.getFrameName(frame) +
		      ": " + ex.toString());
	        }

	        frameIndex++;
	    }
        }
    }

    //-------------------------------------------------------------------
    private void saveHExchangeRate(S2DNmrStarIfc star)
    {
        if (doDebugOutput(3)) {
	    System.out.println("  S2DMain.saveHExchangeRate()");
	}

	// add real code here
    }

    //-------------------------------------------------------------------
    // Save all coupling constants for this entry.
    // Note: this can be tested with 4297.
    private void saveCoupling(S2DNmrStarIfc star) throws S2DException
    {
        if (doDebugOutput(3)) {
	    System.out.println("  S2DMain.saveCoupling()");
	}

	Enumeration frameList = star.getDataFramesByCat(star.COUPLING_SF_CAT,
	  star.COUPLING_CONSTANTS);

	int frameIndex = 1;
        while (frameList.hasMoreElements()) {
	    SaveFrameNode frame = (SaveFrameNode)frameList.nextElement();
	    Vector entityAssemblyIDList = star.getUniqueTagValues(
	      frame, star.COUPLING_ENTITY_ASSEMBLY_ID_1);
	    for (int index = 0; index < entityAssemblyIDList.size(); index++) {
		String entityAssemblyID =
		  (String)entityAssemblyIDList.get(index);

		String entityID = star.entAssemID2entID(entityAssemblyID);
	        if (!star.refersToPolymer(frame, entityID)) {
                    if (doDebugOutput(2)) {
                        System.out.println("Coupling constants not saved for " +
                          "save frame " + frame.getLabel() + " (" +
			  entityAssemblyID + ") because it is not a polymer");
                    }
		    continue;
		}

	        try {
	            saveFrameCoupling(star, frame, entityAssemblyID,
		      frameIndex);
	        } catch(S2DException ex) {
		    System.err.println("Exception saving coupling " +
		      "constants for frame " + star.getFrameName(frame) +
		      ": " + ex.toString());
	        }

	        frameIndex++;
	    }
        }
    }

    //-------------------------------------------------------------------
    private void saveHExchangeProtFact(S2DNmrStarIfc star)
    {
        if (doDebugOutput(3)) {
	    System.out.println("  S2DMain.saveHExchangeProtFact()");
	}

	// add real code here
    }

    //-------------------------------------------------------------------
    // Note: this can be tested with 4689.
    private void saveS2Params(S2DNmrStarIfc star) throws S2DException
    {
        if (doDebugOutput(3)) {
	    System.out.println("  S2DMain.saveS2Params()");
	}

	Enumeration frameList = star.getDataFramesByCat(star.ORDER_SF_CAT,
	  star.ORDER_PARAMETERS);

	int frameIndex = 1;
        while (frameList.hasMoreElements()) {
	    SaveFrameNode frame = (SaveFrameNode)frameList.nextElement();
	    Vector entityAssemblyIDList = star.getUniqueTagValues(
	      frame, star.ORDER_ENTITY_ASSEMBLY_ID);
	    for (int index = 0; index < entityAssemblyIDList.size(); index++) {
		String entityAssemblyID =
		  (String)entityAssemblyIDList.get(index);

		String entityID = star.entAssemID2entID(entityAssemblyID);
	        if (!star.refersToPolymer(frame, entityID)) {
                    if (doDebugOutput(2)) {
                        System.out.println("S2 order parameters not saved for " +
                          "save frame " + frame.getLabel() + " (" +
			  entityAssemblyID + ") because it is not a polymer");
                    }
		    continue;
		}

	        try {
	            saveFrameS2Params(star, frame, entityAssemblyID,
		      frameIndex);
	        } catch(S2DException ex) {
		    System.err.println("Exception saving S2 order " +
		      "parameters for frame " + star.getFrameName(frame) +
		      ": " + ex.toString());
	        }

	        frameIndex++;
	    }
        }
    }

    //-------------------------------------------------------------------
    // Write the data source definitions needed for 3D data.
    private void save3DDataSources() throws S2DException
    {
        if (doDebugOutput(3)) {
	    System.out.println("  S2DMain.save3DDataSources()");
	}

	try {
	    Writer writer = S2DFileWriter.create(_sessionDir +
	      File.separator + _name + File.separator + _name +
	      S2DNames.ATOMIC_COORD_DATA_DEF);
	    S2DSession.write3DDataSources(writer, _dataSets);
	    writer.close();
	} catch (IOException ex) {
	    throw new S2DError("Unable to save 3D data sources: " +
	      ex.toString());
	}
    }

    //-------------------------------------------------------------------
    // Save atomic coordinates that are in the main NMR-STAR file.
    private void saveAtomicCoords(S2DNmrStarIfc star)
    {
        if (doDebugOutput(3)) {
	    System.out.println("  S2DMain.saveAtomicCoords()");
	}

	Enumeration frameList = star.getDataFramesByCat(
	  star.ATOM_COORD_SF_CAT, star.ATOM_COORD_SAVE_FRAME);

        while (frameList.hasMoreElements()) {
	    SaveFrameNode frame = (SaveFrameNode)frameList.nextElement();
	    try {
	        saveFrameAtomicCoords(star, frame, _currentFrameIndex,
		  null, false, false);

		_haveCoords = true;
	    } catch(S2DException ex) {
		System.err.println("Exception saving atomic coordinates for " +
		  "frame " + star.getFrameName(frame) + ": " +
		  ex.toString());
	    }
	    _currentFrameIndex++;
        }
    }

    //-------------------------------------------------------------------
    private void saveLACS(S2DNmrStarIfc star) throws S2DException
    {
        if (doDebugOutput(3)) {
	    System.out.println("  S2DMain.saveLACS()");
	}

        Enumeration frameList = star.getDataFramesByCat(
	  star.LACS_PLOT_SF_CAT, star.LACS_OUTPUT);

	int frameIndex = 1;
        while (frameList.hasMoreElements()) {
	    SaveFrameNode frame = (SaveFrameNode)frameList.nextElement();

	    saveFrameLACS(star, frame, frameIndex);
	    frameIndex++;
	}
    }

    //-------------------------------------------------------------------
    // Save residue counts (and the residue list) corresponding to
    // the given save frame.  (The save frame passed in here should
    // be a save frame containing data such as chemical shifts.)
    private S2DResidues getFrameResCounts(S2DStarIfc star,
      SaveFrameNode chemShiftFrame, String entityID, int frameIndex)
      throws S2DException
    {
        if (doDebugOutput(4)) {
	    System.out.println("    S2DMain.getFrameResCounts(" +
	      star.getFrameName(chemShiftFrame) + " (" + entityID +
	      "), " + frameIndex + ")");
	}

	SaveFrameNode tmpFrame = null;
	try {
	    // We try this first even if protein checking is turned off
	    // in case we have a complete NMR-STAR file with multiple
	    // monmeric_polymer save frames -- that way we get the right
	    // one.
	    tmpFrame = star.getEntityFrame(chemShiftFrame, entityID);
	} catch (S2DException ex) {
	    if (_doProteinCheck) {
		System.err.println("Can't get entity frame for frame " +
		  star.getFrameName(chemShiftFrame));
	        throw ex;
	    }
	}

        S2DResidues residues = saveResList(star, tmpFrame, frameIndex);

	if (_checkResList) {
	    //
	    // Compare the residue list against the residues in the chem
	    // shift loop.
	    // Note: residue numbers start with 1, but we can't always
	    // rely on every residue having an entry in the chem shift loop.
	    //
	    String[] entityIDs = null;
	    if (!entityID.equals("")) {
	        entityIDs = star.getAllChemShiftEntityIDs(chemShiftFrame);
	    }

	    String[] resSeqCodesTmp = star.getAndFilterFrameValues(
	      chemShiftFrame, star.CHEM_SHIFT_VALUE,
	      star.CHEM_SHIFT_RES_SEQ_CODE, entityID, entityIDs);
	    int[] resSeqCodes = S2DUtils.arrayStr2Int(resSeqCodesTmp,
	      star.CHEM_SHIFT_RES_SEQ_CODE);
	    resSeqCodesTmp = null;

	    String[] residueLabels = star.getAndFilterFrameValues(
	      chemShiftFrame, star.CHEM_SHIFT_VALUE,
	      star.CHEM_SHIFT_RES_LABEL, entityID, entityIDs);
	    residues.make3Letter(residueLabels);

            for (int index = 0; index < resSeqCodes.length; ++index) {
	        int resNum = resSeqCodes[index];
	        if (!residueLabels[index].equals(
		  residues.getResLabel(resNum))) {
		    S2DError err = new S2DError("Residue mismatch at residue " +
		      resNum + ": " + residues.getResLabel(resNum) +
		      " in residue list vs. " + residueLabels[index] +
		      " in chem shift loop");
		    throw(err);
	        }
	    }

	}

        return residues;
    }

    //TEMP -- should this really be saveResCount?
    //-------------------------------------------------------------------
    // Save the residue count and residue list for the given save frame.
    // (The save frame passed in here must be the save frame that
    // actually contains the residue lists.)
    private S2DResidues saveResList(S2DStarIfc star,
      SaveFrameNode monoPolyFrame, int frameIndex) throws S2DException
    {
        if (doDebugOutput(4)) {
	    System.out.println("    S2DMain.saveResList(" +
	      star.getFrameName(monoPolyFrame) + ", " + frameIndex + ")");
	}

	// Note: star.getEntityFrame() will throw an exception if there
	// is more than one "entity frame"; this call is needed only for
	// minimal data files for the visualization server, etc.
	// (See test13, test13_3, test46.)
        if (monoPolyFrame == null) {
	    monoPolyFrame = star.getEntityFrame();
	}

        S2DResidues residues = star.getResidues(monoPolyFrame);

        S2DResCount resCount = new S2DResCount(_name, _dataDir,
	  residues._resSeqCodes, residues._resLabels,
	  residues.getPolymerType());

	// In case we want to get the residue list without writing it out.
	if (frameIndex > 0) {
	    resCount.write(frameIndex);
	}

	return residues;
    }

    //-------------------------------------------------------------------
    // Save chem shifts for one save frame.
    private void saveFrameChemShifts(S2DNmrStarIfc star, SaveFrameNode frame,
      String entityAssemblyID, S2DResidues residues, int frameIndex)
      throws S2DException
    {
        if (doDebugOutput(4)) {
	    System.out.println("    S2DMain.saveFrameChemShifts(" +
	      star.getFrameName(frame) + " (" + entityAssemblyID + "), " +
	      frameIndex + ")");
	}

	//
	// Create an S2DChemShift object.
	//
        S2DChemShift chemShift = S2DChemShift.create(
	  residues.getPolymerType(), _name, _longName, star, frame,
	  _dataDir, _sessionDir, _summary, entityAssemblyID, residues);

	//
	// Now go ahead and calculate and write out the chem shift values.
	//
	_summary.startFrame(star.getFrameDetails(frame));

	try {
	    chemShift.writeDeltashifts(frameIndex, false);
	    chemShift.addDeltaData(_dataSets, frameIndex);
	} catch (S2DException ex) {
	    // Don't throw a new exception here because we want to write as
	    // much as we can, even if there's an error somewhere along the
	    // line.
	    System.err.println(ex.toString());
	}

	// We only want to output CSI values if this save frame
	// has HA chem shifts for at least 80% of the residues in
	// the entry.  (This can be tested with bmr4001.str and
	// bmr4011.str.)
	int residueCount = -1;
	// 2009-02-05: okay, at least temporarily we are not going to do
	// this check for non-proteins (to make things work for nucleic
	// acids).
	if (_doProteinCheck && residues.treatAsProtein()) {
	    String entityID = star.entAssemID2entID(entityAssemblyID);
	    residueCount = star.residueCount(frame, entityID);
	}

	if (residueCount < 0 ||
	  ((float)star.getHAChemShiftCount(frame) /
	  (float)residueCount >= 0.8)) {
	    try {
	        chemShift.writeCSI(frameIndex);
	        chemShift.addCsiData(_dataSets, frameIndex);
	    } catch (S2DException ex) {
	        // Don't throw a new exception here because we want to write as
	        // much as we can, even if there's an error somewhere along the
	        // line.
	        System.err.println(ex.toString());
	    }
	} else {
	    if (doDebugOutput(2)) {
	        System.out.println("CSI values not saved for " +
		  "save frame " + frame.getLabel() + " because HA " +
		    "chem shifts are not available for 80% of the residues");
            }
	}

	try {
	    chemShift.writePctAssign(frameIndex, _checkPctAssign,
	      star.CHEMASSG_FILE);
	    chemShift.addPctAssignData(_dataSets, frameIndex);
	} catch (S2DException ex) {
	    // Don't throw a new exception here because we want to write as
	    // much as we can, even if there's an error somewhere along the
	    // line.
	    System.err.println(ex.toString());
	}

	try {
	    chemShift.writeAllShifts(frameIndex);
	} catch (S2DException ex) {
	    // Don't throw a new exception here because we want to write as
	    // much as we can, even if there's an error somewhere along the
	    // line.
	    System.err.println(ex.toString());
	}

	try {
	    chemShift.writeHvsNShifts(frameIndex);
	} catch (S2DException ex) {
	    // Don't throw a new exception here because we want to write as
	    // much as we can, even if there's an error somewhere along the
	    // line.
	    System.err.println(ex.toString());
	}


	try {
	    chemShift.writeHvsCShifts(_connectionFile, frameIndex);
	} catch (S2DException ex) {
	    // Don't throw a new exception here because we want to write as
	    // much as we can, even if there's an error somewhere along the
	    // line.
	    System.err.println(ex.toString());
	}

	_summary.endFrame();
    }

    //-------------------------------------------------------------------
    // Get the maximum model number for which there is SPARTA-calculated
    // delta shift data, for a given save frame, entity assembly ID,
    // and chem shift list ID.
    // (Note that this method will fail on a frame containing average
    // values rather than values for individual models.)
    private int getSpartaMaxModelNum(S2DNmrStarIfc star,
      SaveFrameNode frame, String entityAssemblyID,
      String chemShiftListID) throws S2DException
    {
        if (doDebugOutput(4)) {
	    System.out.println("    S2DMain.getSpartaMaxModelNum(" +
	      star.getFrameName(frame) + " (<" + entityAssemblyID +
	      ">, <" + chemShiftListID + ">)");
	}

	// If a non-blank entityAssemblyID is specified, we need to filter
	// the frame values to only take the ones corresponding to that
	// entityAssemblyID.  To do that, we get the entityAssemblyID
	// values in each row of the loop.  (entityAssemblyID will be blank
	// when processing NMR-STAR 2.1 files -- they don't have data for
	// more than one entity assembly in a single save frame).
	String[] entityAssemblyIDs = null;
	if (!entityAssemblyID.equals("")) {
	    entityAssemblyIDs = star.getFrameValues(frame,
	      star.DELTA_SHIFT_ENTITY_ASSEMBLY_ID,
	      star.DELTA_SHIFT_ENTITY_ASSEMBLY_ID);
	}

	// If a non-blank chemShiftListID is specified, we need to filter
	// the frame values to only take the ones corresponding to that
	// chemShiftListID.  To do that, we get the chemShiftListID
	// values in each row of the loop.
	String[] chemShiftListIDs = null;
	if (!chemShiftListID.equals("")) {
	    chemShiftListIDs = star.getAndFilterFrameValues(frame,
	      star.DELTA_CHEM_SHIFT_LIST_ID, star.DELTA_CHEM_SHIFT_LIST_ID,
	      entityAssemblyID, entityAssemblyIDs);
	}

	String[] modelNumsStr = star.getAndFilterFrameValues(frame,
	  star.DELTA_SHIFT_VALUE, star.DELTA_SHIFT_MODEL_NUM,
	  entityAssemblyID, entityAssemblyIDs);
	// Now filter by chemShiftListID if necessary.
	if (!chemShiftListID.equals("")) {
	    modelNumsStr = S2DUtils.selectMatches(chemShiftListIDs,
	      modelNumsStr, chemShiftListID);
	}
	int[] modelNums = S2DUtils.arrayStr2Int(modelNumsStr,
	  star.DELTA_SHIFT_RES_SEQ_CODE);
	modelNumsStr = null;

	int maxModelNum = 0;
	for (int index = 0; index < modelNums.length; index++) {
		maxModelNum = Math.max(maxModelNum, modelNums[index]);
	}

        if (doDebugOutput(4)) {
	    System.out.println("  maxModelNum: " + maxModelNum);
	}

	return maxModelNum;
    }

    //-------------------------------------------------------------------
    // Save (pre-calculated) delta shifts for one save frame.
    private void saveFrameSpartaDeltaShifts(S2DNmrStarIfc star,
      SaveFrameNode frame, String entityAssemblyID,
      String chemShiftListID, int modelNum, S2DResidues residues,
      int frameIndex, boolean append) throws S2DException
    {
        if (doDebugOutput(4)) {
	    System.out.println("    S2DMain.saveFrameSpartaDeltaShifts(" +
	      star.getFrameName(frame) + " (<" + entityAssemblyID +
	      ">, <" + chemShiftListID + ">, " + modelNum + ", " +
	      frameIndex + "), " + append + ")");
	}

	//
	// Create an S2DChemShift object.
	//
        S2DChemShift chemShift = S2DChemShift.createSparta(
	  residues.getPolymerType(), _name, _longName, star, frame,
	  _dataDir, _sessionDir, _summary, entityAssemblyID,
	  chemShiftListID, modelNum, residues);

	//
	// Now go ahead and write out the delta shift values.
	//
	_summary.startFrame(star.getFrameDetails(frame));

	try {
	    chemShift.writeDeltashifts(frameIndex, append);
	    chemShift.addSpartaData(_dataSets, frameIndex, append);
	} catch (S2DException ex) {
	    // Don't throw a new exception here because we want to write as
	    // much as we can, even if there's an error somewhere along the
	    // line.
	    System.err.println(ex.toString());
	}

	_summary.endFrame();
    }

    //-------------------------------------------------------------------
    // Save T1 relaxation values for one save frame.
    private void saveFrameT1Relax(S2DNmrStarIfc star, SaveFrameNode frame,
      String entityAssemblyID, int frameIndex) throws S2DException
    {
        if (doDebugOutput(4)) {
	    System.out.println("    S2DMain.saveFrameT1Relax(" +
	      star.getFrameName(frame) + " (" + entityAssemblyID + "), " +
	      frameIndex + ")");
	}

	//
	// Get the values we need from the Star file.
	//
	//TEMP -- move this to S2DRelaxation constructor

	// If a non-blank entityAssemblyID is specified, we need to filter
	// the frame values to only take the ones corresponding to that
	// entityAssemblyID.  To do that, we get the entityAssemblyID
	// values in each row of the loop.  (entityAssemblyID will be blank
	// when processing NMR-STAR 2.1 files -- they don't have data for
	// more than one entity assembly in a single save frame).
	String[] entityAssemblyIDs = null;
	if (!entityAssemblyID.equals("")) {
	    entityAssemblyIDs = star.getFrameValues(frame,
	      star.T1_ENTITY_ASSEMBLY_ID,
	      star.T1_ENTITY_ASSEMBLY_ID);
	}

	String[] resSeqCodes = star.getAndFilterFrameValues(frame,
	  star.T1_VALUE, star.T1_RES_SEQ_CODE, entityAssemblyID,
	  entityAssemblyIDs);

	String[] resLabels = star.getAndFilterFrameValues(frame,
	  star.T1_VALUE, star.T1_RES_LABEL, entityAssemblyID,
	  entityAssemblyIDs);

	String[] atomNames = star.getAndFilterFrameValues(frame,
	  star.T1_VALUE, star.T1_ATOM_NAME, entityAssemblyID,
	  entityAssemblyIDs);

	String[] relaxValues = star.getAndFilterFrameValues(frame,
	  star.T1_VALUE, star.T1_VALUE, entityAssemblyID,
	  entityAssemblyIDs);

	//TEMP -- 4096 has "_T1_error" instead of "_T1_value_error".
	String[] relaxErrors = star.getAndFilterOptionalFrameValues(frame,
	  star.T1_VALUE, star.T1_VALUE_ERR, entityAssemblyID,
	  entityAssemblyIDs, relaxValues.length, "0");

	int entityAssemblyIDVal = star.getEntityAssemblyID(frame,
	  entityAssemblyID);

	//
	// Create an S2DRelaxation object with the values we just got.
	//
	S2DRelaxation relaxation = new S2DRelaxation(_name, _longName,
	  _dataDir, _sessionDir, _summary, S2DUtils.TYPE_T1_RELAX,
	  star.getOneFrameValue(frame, star.T1_SPEC_FREQ_1H),
	  resSeqCodes, resLabels, atomNames, relaxValues, relaxErrors,
	  entityAssemblyIDVal, star.getFrameDetails(frame));

	//
	// Now go ahead and calculate and write out the relaxation values.
	//
	_summary.startFrame(star.getFrameDetails(frame));

	try {
	    relaxation.writeRelaxation(frameIndex);
	    String entityID = star.entAssemID2entID(entityAssemblyID);
	    relaxation.addRelaxationData(_dataSets, frameIndex,
	      star.getDataPolymerType(frame, entityID));
	} finally {
	    _summary.endFrame();
	}
    }

    //-------------------------------------------------------------------
    // Save T2 relaxation values for one save frame.
    private void saveFrameT2Relax(S2DNmrStarIfc star, SaveFrameNode frame,
      String entityAssemblyID, int frameIndex) throws S2DException
    {
        if (doDebugOutput(4)) {
	    System.out.println("    S2DMain.saveFrameT2Relax(" +
	      star.getFrameName(frame) + " (" + entityAssemblyID + "), " +
	      frameIndex + ")");
	}

	//
	// Get the values we need from the Star file.
	//
	//TEMP -- move this to S2DRelaxation constructor

	// If a non-blank entityAssemblyID is specified, we need to filter
	// the frame values to only take the ones corresponding to that
	// entityAssemblyID.  To do that, we get the entityAssemblyID
	// values in each row of the loop.  (entityAssemblyID will be blank
	// when processing NMR-STAR 2.1 files -- they don't have data for
	// more than one entity assembly in a single save frame).
	String[] entityAssemblyIDs = null;
	if (!entityAssemblyID.equals("")) {
	    entityAssemblyIDs = star.getFrameValues(frame,
	      star.T2_ENTITY_ASSEMBLY_ID,
	      star.T2_ENTITY_ASSEMBLY_ID);
	}

	String[] resSeqCodes = star.getAndFilterFrameValues(frame,
	  star.T2_VALUE, star.T2_RES_SEQ_CODE, entityAssemblyID,
	  entityAssemblyIDs);

	String[] resLabels = star.getAndFilterFrameValues(frame,
	  star.T2_VALUE, star.T2_RES_LABEL, entityAssemblyID,
	  entityAssemblyIDs);

	String[] atomNames = star.getAndFilterFrameValues(frame,
	  star.T2_VALUE, star.T2_ATOM_NAME, entityAssemblyID,
	  entityAssemblyIDs);

	String[] relaxValues = star.getAndFilterFrameValues(frame,
	  star.T2_VALUE, star.T2_VALUE, entityAssemblyID,
	  entityAssemblyIDs);

	//TEMP -- 4096 has "_T2_error" instead of "_T2_value_error".
	String[] relaxErrors = star.getAndFilterOptionalFrameValues(frame,
	  star.T2_VALUE, star.T2_VALUE_ERR, entityAssemblyID,
	  entityAssemblyIDs, relaxValues.length, "0");

	int entityAssemblyIDVal = star.getEntityAssemblyID(frame,
	  entityAssemblyID);

	//
	// Create an S2DRelaxation object with the values we just got.
	//
	S2DRelaxation relaxation = new S2DRelaxation(_name, _longName,
	  _dataDir, _sessionDir, _summary, S2DUtils.TYPE_T2_RELAX,
	  star.getOneFrameValue(frame, star.T2_SPEC_FREQ_1H),
	  resSeqCodes, resLabels, atomNames, relaxValues, relaxErrors,
	  entityAssemblyIDVal, star.getFrameDetails(frame));

	//
	// Now go ahead and calculate and write out the relaxation values.
	//
	_summary.startFrame(star.getFrameDetails(frame));

	try {
	    relaxation.writeRelaxation(frameIndex);
	    String entityID = star.entAssemID2entID(entityAssemblyID);
	    relaxation.addRelaxationData(_dataSets, frameIndex,
	      star.getDataPolymerType(frame, entityID));
	} finally {
	    _summary.endFrame();
	}
    }

    //-------------------------------------------------------------------
    // Save coupling constants for one save frame.
    private void saveFrameCoupling(S2DNmrStarIfc star, SaveFrameNode frame,
      String entityAssemblyID, int frameIndex) throws S2DException
    {
        if (doDebugOutput(4)) {
	    System.out.println("    S2DMain.saveFrameCoupling(" +
	      star.getFrameName(frame) + " (" + entityAssemblyID + "), " +
	      frameIndex + ")");
	}

	//
	// Get the values we need from the Star file.
	//
	//TEMP -- move this to S2DCoupling constructor

	// If a non-blank entityAssemblyID is specified, we need to filter
	// the frame values to only take the ones corresponding to that
	// entityAssemblyID.  To do that, we get the entityAssemblyID
	// values in each row of the loop.  (entityAssemblyID will be blank
	// when processing NMR-STAR 2.1 files -- they don't have data for
	// more than one entity assembly in a single save frame).
	String[] entityAssemblyIDs = null;
	if (!entityAssemblyID.equals("")) {
	    entityAssemblyIDs = star.getFrameValues(frame,
	      star.COUPLING_ENTITY_ASSEMBLY_ID_1,
	      star.COUPLING_ENTITY_ASSEMBLY_ID_1);
	}

	String[] atom1ResSeqs = star.getAndFilterFrameValues(frame,
	  star.COUPLING_CONSTANT_VALUE, star.COUPLING_RES_SEQ_CODE_1,
	  entityAssemblyID, entityAssemblyIDs);

	String[] atom2ResSeqs = star.getAndFilterFrameValues(frame,
	  star.COUPLING_CONSTANT_VALUE, star.COUPLING_RES_SEQ_CODE_2,
	  entityAssemblyID, entityAssemblyIDs);

	String[] couplingConstValues = star.getAndFilterFrameValues(frame,
	  star.COUPLING_CONSTANT_VALUE, star.COUPLING_CONSTANT_VALUE,
	  entityAssemblyID, entityAssemblyIDs);

	String[] couplingConstCodes = star.getAndFilterOptionalFrameValues(
	  frame, star.COUPLING_CONSTANT_VALUE, star.COUPLING_CONSTANT_CODE,
	  entityAssemblyID, entityAssemblyIDs,
	  atom1ResSeqs.length, "0");

	String[] atom1ResLabels = star.getAndFilterOptionalFrameValues(frame,
	  star.COUPLING_CONSTANT_VALUE, star.COUPLING_RES_LABEL_1,
	  entityAssemblyID, entityAssemblyIDs,
	  atom1ResSeqs.length, "0");

	String[] atom1Names = star.getAndFilterOptionalFrameValues(frame,
	  star.COUPLING_CONSTANT_VALUE, star.COUPLING_ATOM_NAME_1,
	  entityAssemblyID, entityAssemblyIDs,
	  atom1ResSeqs.length, "0");

	String[] atom2ResLabels = star.getAndFilterOptionalFrameValues(frame,
	  star.COUPLING_CONSTANT_VALUE, star.COUPLING_RES_LABEL_2,
	  entityAssemblyID, entityAssemblyIDs,
	  atom1ResSeqs.length, "0");

	String[] atom2Names = star.getAndFilterOptionalFrameValues(frame,
	  star.COUPLING_CONSTANT_VALUE, star.COUPLING_ATOM_NAME_2,
	  entityAssemblyID, entityAssemblyIDs,
	  atom1ResSeqs.length, "0");

	String[] couplingConstErrors = star.getAndFilterOptionalFrameValues(
	  frame, star.COUPLING_CONSTANT_VALUE,
	  star.COUPLING_CONSTANT_VALUE_ERR,
	  entityAssemblyID, entityAssemblyIDs, atom1ResSeqs.length, "0");

	int entityAssemblyIDVal = star.getEntityAssemblyID(frame,
	  entityAssemblyID);

	//
	// Create an S2DCoupling object with the values we just got.
	//
        S2DCoupling coupling = new S2DCoupling(_name, _longName, star,
	  _dataDir, _sessionDir, _summary, couplingConstCodes, atom1ResSeqs,
	  atom1ResLabels, atom1Names, atom2ResSeqs, atom2ResLabels,
	  atom2Names, couplingConstValues, couplingConstErrors,
	  entityAssemblyIDVal, star.getFrameDetails(frame));

	//
	// Now go ahead and calculate and write out the coupling contants.
	//
	_summary.startFrame(star.getFrameDetails(frame));

	try {
	    coupling.writeCoupling(frameIndex);
	    String entityID = star.entAssemID2entID(entityAssemblyID);
	    coupling.addCouplingData(_dataSets, frameIndex,
	      star.getDataPolymerType(frame, entityID));
	} finally {
	    _summary.endFrame();
	}
    }

    //-------------------------------------------------------------------
    // Save heteronuclear NOE values for one save frame.
    private void saveFrameHetNOE(S2DNmrStarIfc star, SaveFrameNode frame,
      String entityAssemblyID, int frameIndex) throws S2DException
    {
        if (doDebugOutput(4)) {
	    System.out.println("    S2DMain.saveFrameHetNOE(" +
	      star.getFrameName(frame) + " (" + entityAssemblyID + "), " +
	      frameIndex + ")");
	}

	//
	// Get the values we need from the Star file.
	// TEMP -- NMR-STAR 3.0 has two residues for each heternuclear
	// NOE, so we should probably deal with that eventually.
	//
	//TEMP -- move this to S2DHetNOE constructor

	// If a non-blank entityAssemblyID is specified, we need to filter
	// the frame values to only take the ones corresponding to that
	// entityAssemblyID.  To do that, we get the entityAssemblyID
	// values in each row of the loop.  (entityAssemblyID will be blank
	// when processing NMR-STAR 2.1 files -- they don't have data for
	// more than one entity assembly in a single save frame).
	String[] entityAssemblyIDs = null;
	if (!entityAssemblyID.equals("")) {
	    entityAssemblyIDs = star.getFrameValues(frame,
	      star.HET_NOE_ENTITY_ASSEMBLY_ID_1,
	      star.HET_NOE_ENTITY_ASSEMBLY_ID_1);
	}

//TEMP -- convert these to numerical values?
	String[] resSeqCodes = star.getAndFilterFrameValues(frame,
	  star.HET_NOE_VALUE, star.HET_NOE_RES_SEQ_CODE, entityAssemblyID,
	  entityAssemblyIDs);

	String[] resLabels = star.getAndFilterFrameValues(frame,
	  star.HET_NOE_VALUE, star.HET_NOE_RES_LABEL, entityAssemblyID,
	  entityAssemblyIDs);

	String[] hetNOEValues = star.getAndFilterFrameValues(frame,
	  star.HET_NOE_VALUE, star.HET_NOE_VALUE, entityAssemblyID,
	  entityAssemblyIDs);

	String[] hetNOEErrors = star.getAndFilterFrameValues(frame,
	  star.HET_NOE_VALUE, star.HET_NOE_VALUE_ERR, entityAssemblyID,
	  entityAssemblyIDs);

	int entityAssemblyIDVal = star.getEntityAssemblyID(frame,
	  entityAssemblyID);

	//
	// Create an S2DHetNOE object with the values we just got.
	//
	// are dealt with really differently in 2.1 and 3.0
	S2DHetNOE hetNOE = new S2DHetNOE(_name, _longName, star, _dataDir,
	  _sessionDir, _summary,
	  star.getOneFrameValue(frame, star.HET_NOE_SPEC_FREQ_1H),
	  star.getHetNOEAtom1(frame), star.getHetNOEAtom2(frame),
	  resSeqCodes, resLabels, hetNOEValues, hetNOEErrors,
	  entityAssemblyIDVal, star.getFrameDetails(frame));

	//
	// Now go ahead and calculate and write out the heteronuclear NOE
	// values.
	//
	_summary.startFrame(star.getFrameDetails(frame));

	try {
	    hetNOE.writeHetNOE(frameIndex);
	    String entityID = star.entAssemID2entID(entityAssemblyID);
	    hetNOE.addHetNOEData(_dataSets, frameIndex,
	      star.getDataPolymerType(frame, entityID));
	} finally {
	    _summary.endFrame();
	}
    }

    //-------------------------------------------------------------------
    // Save S2 order parameters for one save frame.
    private void saveFrameS2Params(S2DNmrStarIfc star, SaveFrameNode frame,
      String entityAssemblyID, int frameIndex) throws S2DException
    {
        if (doDebugOutput(4)) {
	    System.out.println("    S2DMain.saveFrameS2Params(" +
	      star.getFrameName(frame) + " (" + entityAssemblyID + "), " +
	      frameIndex + ")");
	}

	//
	// Get the values we need from the Star file.
	//
	//TEMP -- move this to S2DS2Order constructor

	// If a non-blank entityAssemblyID is specified, we need to filter
	// the frame values to only take the ones corresponding to that
	// entityAssemblyID.  To do that, we get the entityAssemblyID
	// values in each row of the loop.  (entityAssemblyID will be blank
	// when processing NMR-STAR 2.1 files -- they don't have data for
	// more than one entity assembly in a single save frame).
	String[] entityAssemblyIDs = null;
	if (!entityAssemblyID.equals("")) {
	    entityAssemblyIDs = star.getFrameValues(frame,
	      star.ORDER_ENTITY_ASSEMBLY_ID,
	      star.ORDER_ENTITY_ASSEMBLY_ID);
	}
	String[] resSeqCodes = star.getAndFilterFrameValues(frame,
	  star.ORDER_VALUE, star.ORDER_RES_SEQ_CODE, entityAssemblyID,
	  entityAssemblyIDs);

	String[] resLabels = star.getAndFilterFrameValues(frame,
	  star.ORDER_VALUE, star.ORDER_RES_LABEL, entityAssemblyID,
	  entityAssemblyIDs);

	String[] atomNames = star.getAndFilterFrameValues(frame,
	  star.ORDER_VALUE, star.ORDER_ATOM_NAME, entityAssemblyID,
	  entityAssemblyIDs);

	String[] s2Values = star.getAndFilterFrameValues(frame,
	  star.ORDER_VALUE, star.ORDER_VALUE, entityAssemblyID,
	  entityAssemblyIDs);

	String[] s2Errors = star.getAndFilterOptionalFrameValues(frame,
	  star.ORDER_VALUE, star.ORDER_VALUE_ERR, entityAssemblyID,
	  entityAssemblyIDs, s2Values.length, "0");

	int entityAssemblyIDVal = star.getEntityAssemblyID(frame,
	  entityAssemblyID);

	//
	// Create an S2DS2Order object with the values we just got.
	//
	S2DS2Order s2Order = new S2DS2Order(_name, _longName, star,
	  _dataDir, _sessionDir, _summary, resSeqCodes, resLabels,
	  atomNames, s2Values, s2Errors, entityAssemblyIDVal,
	  star.getFrameDetails(frame));

	//
	// Now go ahead and calculate and write out the S2 order values.
	//
	_summary.startFrame(star.getFrameDetails(frame));

	try {
	    s2Order.writeS2Order(frameIndex);
	    String entityID = star.entAssemID2entID(entityAssemblyID);
	    s2Order.addS2Order(_dataSets, frameIndex,
	      star.getDataPolymerType(frame, entityID));
	} finally {
	    _summary.endFrame();
	}
    }

    //TEMP -- I'll bet Struct_type (e.g., side_chains) needs fixing for NAs
    //-------------------------------------------------------------------
    // Save atomic coordinates for one save frame.
    private void saveFrameAtomicCoords(S2DStarIfc star, SaveFrameNode frame,
      int frameIndex, S2DAtomDataTable pt, boolean for2DView, boolean isRR)
      throws S2DException
    {
        if (doDebugOutput(4)) {
	    System.out.println("    S2DMain.saveFrameAtomicCoords(" +
	      star.getFrameName(frame) + ", " + frameIndex + ")");
	}

	// Create an S2DAtomicCoordinates object with the data
	// in the current save frame.
	S2DAtomicCoords atomicCoords = new S2DAtomicCoords(_name,
	  _longName, star, frame, _dataDir, _sessionDir, _summary,
	  _connectionFile, _dataSets, _currentPdbId);

	// Don't write session file, summary file link, etc., if doing
	// coordinates from remediated restraints.
	boolean restraintOnly = isRR ? true : _restraintOnly;

	// Now go ahead and calculate and write out the atomic coordinates.
	if (pt == null && !restraintOnly) {
	    AtomicCoordSummaryPre(star, frame);
	}

	try {
	    if (for2DView) {
	        atomicCoords.writeBonds(frameIndex, pt, for2DView);
	    } else {
	        atomicCoords.writeAtoms(frameIndex, restraintOnly, isRR);
	    }
	} finally {
	    if (pt == null) {
	        _summary.endFrame();
	    }
	}
    }

    //-------------------------------------------------------------------
    // Save Pistachio data for one save frame.
    //TEMP -- needs entity argument
    private void saveFramePistachio(S2DNmrStarIfc star, SaveFrameNode frame,
      String entityAssemblyID, S2DResidues residues, int frameIndex)
      throws S2DException
    {
        if (doDebugOutput(4)) {
	    System.out.println("    S2DMain.saveFramePistachio(" +
	      star.getFrameName(frame) + " (" + entityAssemblyID + "), " +
	      frameIndex + ")");
	}

	// 2009-02-05: at least temporarily don't try to save ambiguity
	// code data for non-proteins.
	if (!residues.treatAsProtein()) {
            if (doDebugOutput(2)) {
	    	System.out.println("Not saving Pistachio data because " +
		  "structure is not a protein");
	    }
            return;
	}

	//
	// Get the values we need from the Star file.
	//
	//TEMP -- move this to S2DPistachio constructor
	if (star.FIGURE_OF_MERIT.equals("")) {
	    // File is not NMR-STAR 3.0
            if (doDebugOutput(4)) {
	        System.out.println("    Skipping Pistachio processing " +
		  "because file is not NMR-STAR 3.x");
	    }
	    return;
	}

//TEMP -- change CHEM_SHIFT_VALUE to FIGURE_OF_MERIT in all here
	String[] entityAssemblyIDs = null;
	if (!entityAssemblyID.equals("")) {
	    entityAssemblyIDs = star.getFrameValues(frame,
	      star.CHEM_SHIFT_ENTITY_ASSEMBLY_ID,
	      star.CHEM_SHIFT_ENTITY_ASSEMBLY_ID);
	}

	String[] meritValsTmp;
	try {
	    meritValsTmp = star.getAndFilterFrameValues(frame,
	      star.CHEM_SHIFT_VALUE, star.FIGURE_OF_MERIT, entityAssemblyID,
	      entityAssemblyIDs);
	    if (S2DUtils.entireArrayMatches(meritValsTmp, ".")) {
		throw new S2DWarning("Figure of merit values are all null");
	    }
	} catch (S2DException ex) {
            if (doDebugOutput(4)) {
	        System.out.println("No figure of merit values in this " +
		  "save frame (" + star.getFrameName(frame) + ")");
	    }
	    return;
	}
	double[] meritVals = S2DUtils.arrayStr2Double(meritValsTmp,
	  star.FIGURE_OF_MERIT);
	meritValsTmp = null;

	String[] resSeqCodesTmp = star.getAndFilterFrameValues(frame,
	  star.CHEM_SHIFT_VALUE, star.CHEM_SHIFT_RES_SEQ_CODE, entityAssemblyID,
	  entityAssemblyIDs);
	int[] resSeqCodes = S2DUtils.arrayStr2Int(resSeqCodesTmp,
	  star.CHEM_SHIFT_RES_SEQ_CODE);
	resSeqCodesTmp = null;

	String[] residueLabels = star.getAndFilterFrameValues(frame,
	  star.CHEM_SHIFT_VALUE, star.CHEM_SHIFT_RES_LABEL, entityAssemblyID,
	  entityAssemblyIDs);
	residues.make3Letter(residueLabels);

	String[] atomNames = star.getAndFilterFrameValues(frame,
	  star.CHEM_SHIFT_VALUE, star.CHEM_SHIFT_ATOM_NAME, entityAssemblyID,
	  entityAssemblyIDs);

	int entityAssemblyIDVal = star.getEntityAssemblyID(frame,
	  entityAssemblyID);

	//
	// Create an S2DPistachio object with the values we just got.
	//
        S2DPistachio pistachio = new S2DPistachio(_name, star, _dataDir,
	  _sessionDir, _summary, resSeqCodes, residueLabels, atomNames,
	  meritVals, entityAssemblyIDVal, star.getFrameDetails(frame));

	//
	// The S2DPistachioTable object holds the figures of merit for
	// each atom, for the "color by figure of merit" option.  This
	// is passed along when we write out the coordinates so that the
	// figure of merit values and corresponding color strings can also
	// be written along with the coordinates.
	//
	S2DPistachioTable pt = new S2DPistachioTable(resSeqCodes,
	  atomNames, meritVals);

	//
	// Create an S2DDummyCoords object and write out the
	// coordinates.
	//
        S2DDummyCoords dCoords = S2DDummyCoords.getInstance();
	String tmpFile = _dataDir + File.separator + _name + "pstmp";
	dCoords.writeCoords(residues, entityAssemblyIDVal, tmpFile,
	  PISTACHIO_WRAP_LENGTH);

	S2DmmCifIfc cif = new S2DmmCifIfc(tmpFile);
	saveFrameAtomicCoords(cif, null, frameIndex, pt, true, false);

	try {
	    S2DUtils.deleteFile(tmpFile);
	} catch (IOException ex) {
	    System.err.println("Warning: unable to delete temporary " +
	      "dummy mmCIF file: " + ex);
	}

	//
	// Now go ahead and write out the Pistachio values.
	//
	_summary.startFrame("Assignment figure of merit data");

	try {
	    pistachio.writePistachio(frameIndex);
	} finally {
	    _summary.endFrame();
	}
    }

    //-------------------------------------------------------------------
    // Save ambiguity data for one save frame.
    private void saveFrameAmbiguity(S2DNmrStarIfc star, SaveFrameNode frame,
      String entityAssemblyID, S2DResidues residues, int frameIndex)
      throws S2DException
    {
        if (doDebugOutput(4)) {
	    System.out.println("    S2DMain.saveFrameAmbiguity(" +
	      star.getFrameName(frame) + " (" + entityAssemblyID + "), " +
	      frameIndex + ")");
	}

	// 2009-02-05: at least temporarily don't try to save ambiguity
	// code data for non-proteins.
	if (!residues.treatAsProtein()) {
            if (doDebugOutput(2)) {
	    	System.out.println("Not saving ambiguity data because " +
		  "structure is not a protein");
	    }
            return;
	}

	//
	// Get the values we need from the Star file.
	//
	//TEMP -- move this to S2DAmbiguity constructor
	String[] entityAssemblyIDs = null;
	if (!entityAssemblyID.equals("")) {
	    entityAssemblyIDs = star.getFrameValues(frame,
	      star.CHEM_SHIFT_ENTITY_ASSEMBLY_ID,
	      star.CHEM_SHIFT_ENTITY_ASSEMBLY_ID);
	}

	String[] ambiguityTmp;
	try {
	    ambiguityTmp = star.getAndFilterFrameValues(frame,
	      star.CHEM_SHIFT_VALUE, star.CHEM_SHIFT_AMBIG_CODE,
	      entityAssemblyID, entityAssemblyIDs);
	    if (S2DUtils.entireArrayMatches(ambiguityTmp, ".")) {
	        throw new S2DWarning("Ambiguity code values are all null");
	    }
	} catch (S2DException ex) {
            if (doDebugOutput(4)) {
	        System.out.println("No ambiguity values in this save frame (" +
		  star.getFrameName(frame) + ")");
	    }
	    return;
	}
	int[] ambiguityVals = S2DUtils.arrayStr2Int(ambiguityTmp,
	  star.CHEM_SHIFT_AMBIG_CODE);
	ambiguityTmp = null;

	String[] resSeqCodesTmp = star.getAndFilterFrameValues(frame,
	  star.CHEM_SHIFT_VALUE, star.CHEM_SHIFT_RES_SEQ_CODE,
	  entityAssemblyID, entityAssemblyIDs);
	int[] resSeqCodes = S2DUtils.arrayStr2Int(resSeqCodesTmp,
	  star.CHEM_SHIFT_RES_SEQ_CODE);
	resSeqCodesTmp = null;

	String[] residueLabels = star.getAndFilterFrameValues(frame,
	  star.CHEM_SHIFT_VALUE, star.CHEM_SHIFT_RES_LABEL, entityAssemblyID,
	  entityAssemblyIDs);
	residues.make3Letter(residueLabels);

	String[] atomNames = star.getAndFilterFrameValues(frame,
	  star.CHEM_SHIFT_VALUE, star.CHEM_SHIFT_ATOM_NAME, entityAssemblyID,
	  entityAssemblyIDs);

	int entityAssemblyIDVal = star.getEntityAssemblyID(frame,
	  entityAssemblyID);

	//
	// Create an S2DAmbiguity object with the values we just got.
	//
        S2DAmbiguity ambiguity = new S2DAmbiguity(_name, star, _dataDir,
	  _sessionDir, _summary, resSeqCodes, residueLabels, ambiguityVals,
	  entityAssemblyIDVal, star.getFrameDetails(frame));

	//
	// The S2DAmbiguityTable object holds the ambiguity values for
	// each atom, for the "color by ambiguity value" option.  This
	// is passed along when we write out the coordinates so that the
	// ambiguity values and corresponding color strings can also be
	// written along with the coordinates.
	//
	S2DAmbiguityTable at = new S2DAmbiguityTable(resSeqCodes, atomNames,
	  ambiguityVals);

	//
	// Create an S2DDummyCoords object and write out the
	// coordinates.
	//
        S2DDummyCoords dCoords = S2DDummyCoords.getInstance();
	String tmpFile = _dataDir + File.separator + _name + "pstmp";
	dCoords.writeCoords(residues, entityAssemblyIDVal, tmpFile,
	  AMBIGUITY_WRAP_LENGTH);

	S2DmmCifIfc cif = new S2DmmCifIfc(tmpFile);
	saveFrameAtomicCoords(cif, null, frameIndex, at, true, false);

	try {
	    S2DUtils.deleteFile(tmpFile);
	} catch (IOException ex) {
	    System.err.println("Warning: unable to delete temporary " +
	      "dummy mmCIF file: " + ex);
	}

	//
	// Now go ahead and write out the ambiguity values.
	//
	_summary.startFrame(star.getFrameDetails(frame));

	try {
	    ambiguity.writeAmbiguity(frameIndex);
	} finally {
	    _summary.endFrame();
	}
    }

    //-------------------------------------------------------------------
    // Save LACS data for one save frame.
    private void saveFrameLACS(S2DStarIfc star, SaveFrameNode frame,
      int frameIndex) throws S2DException
    {
        if (doDebugOutput(4)) {
	    System.out.println("    S2DMain.saveFrameLACS(" +
	      star.getFrameName(frame) + ", " + frameIndex + ")");
	}

	S2DLacs lacs = new S2DLacs(_name, _longName, star, _dataDir,
	  _sessionDir, _summary, star.getFrameDetails(frame));

        //
        // Get the values we need from the Star file.
        //
	//TEMP -- move this to S2DLacs constructor

	lacs._xCoordName = star.getTagValue(frame, star.LACS_X_NAME);
	lacs._yCoordName = star.getTagValue(frame, star.LACS_Y_NAME);

	//TEMP -- catch NumberFormatExceptions here and report details?
	lacs._line1._point1._x = Double.parseDouble(star.getTagValue(frame,
	  star.LACS_LINE1_X1));
	lacs._line1._point1._y = Double.parseDouble(star.getTagValue(frame,
	  star.LACS_LINE1_Y1));
	lacs._line1._point2._x = Double.parseDouble(star.getTagValue(frame,
	  star.LACS_LINE1_X2));
	lacs._line1._point2._y = Double.parseDouble(star.getTagValue(frame,
	  star.LACS_LINE1_Y2));

	lacs._line2._point1._x = Double.parseDouble(star.getTagValue(frame,
	  star.LACS_LINE2_X1));
	lacs._line2._point1._y = Double.parseDouble(star.getTagValue(frame,
	  star.LACS_LINE2_Y1));
	lacs._line2._point2._x = Double.parseDouble(star.getTagValue(frame,
	  star.LACS_LINE2_X2));
	lacs._line2._point2._y = Double.parseDouble(star.getTagValue(frame,
	  star.LACS_LINE2_Y2));

	lacs._yOffset = Double.parseDouble(star.getTagValue(frame,
	  star.LACS_Y_OFFSET));

	lacs.calculateLines();

        String[] resSeqCodesTmp = star.getFrameValues(frame,
          star.LACS_RES_NUM, star.LACS_RES_NUM);
	lacs._resSeqCodes = S2DUtils.arrayStr2Int(resSeqCodesTmp,
	  star.LACS_RES_NUM);
	resSeqCodesTmp = null;

        lacs._resLabels = star.getFrameValues(frame,
          star.LACS_RES_NUM, star.LACS_RES_LABEL);

        String[] xCoordsTmp = star.getFrameValues(frame,
	  star.LACS_RES_NUM, star.LACS_X_VALUE);
	lacs._xCoords = S2DUtils.arrayStr2Double(xCoordsTmp,
	  star.LACS_X_VALUE);
	xCoordsTmp = null;

        String[] yCoordsTmp = star.getFrameValues(frame,
	  star.LACS_RES_NUM, star.LACS_Y_VALUE);
	lacs._yCoords = S2DUtils.arrayStr2Double(yCoordsTmp,
	  star.LACS_Y_VALUE);
	yCoordsTmp = null;

        String[] desigsTmp = star.getFrameValues(frame,
	  star.LACS_RES_NUM, star.LACS_DESIGNATOR);
	lacs._desigs = S2DUtils.arrayStr2Int(desigsTmp, star.LACS_DESIGNATOR);
	desigsTmp = null;

	//
	// Now go ahead and calculate and write out the LACS values.
	//
	_summary.startFrame(star.getFrameDetails(frame));

	try {
	    lacs.writeLACS(frameIndex);
	} finally {
	    _summary.endFrame();
	}
    }

    //-------------------------------------------------------------------
    // Generate the pre-link summary page output for atomic coordinates.
    private void AtomicCoordSummaryPre(S2DStarIfc star,
      SaveFrameNode frame) throws S2DException
    {
        if (doDebugOutput(4)) {
            System.out.println("AtomicCoordSummaryPre(" + star + ", " +
	      frame + ")");
        }

	String details = star.getFrameDetails(frame);
	if (details == null) {
	    details = "Representative structure";
        }
	if (_currentPdbId != null) {
	    details += " from PDB ID " + _currentPdbId;
	}
	_summary.startFrame(details);
    }

    //-------------------------------------------------------------------
    // Process a PDB entry (get the atomic coordinates).
    private void process1PDB(S2DStarIfc star, String pdbId) throws S2DException
    {
        if (doDebugOutput(2)) {
	    System.out.println("process1PDB(" + pdbId + ")");
	}

        _currentPdbId = pdbId;

	if (_pdbLevel == PDB_LEVEL_LINK) {
	    AtomicCoordSummaryPre(star, null);
	    try {
	        _summary.writeAtomicCoordsCGI(_currentPdbId,
		  _currentFrameIndex);
            } catch(IOException ex) {
                System.err.println(
		  "IOException writing atomic coordinates: " +
		  ex.toString());
	        throw new S2DError("Can't write atomic coordinates");
	    } finally {
	        _summary.endFrame();
	    }
	} else if (_pdbLevel == PDB_LEVEL_PROCESS) {
	    String resListFile = _restraintOnly ? null :
	      _dataDir + File.separator + _name +
	      S2DNames.RES_LIST_SUFFIX + S2DNames.DAT_SUFFIX;
	    S2DmmCifIfc cif = new S2DmmCifIfc(pdbId, _useLocalFiles,
	      resListFile);
	    saveFrameAtomicCoords(cif, null, _currentFrameIndex, null,
	      false, false);
	} else {
	    throw new S2DError("Illegal _pdbLevel value: " + _pdbLevel);
	}

        _currentPdbId = null;
    }

    //-------------------------------------------------------------------
    // Process coordinates from a remediated restraints file.
    // TEMP -- see if this can be combined with process1PDB
    private void process1RRCoordSet(String rrFile, String pdbId)
      throws S2DException
    {
        if (doDebugOutput(2)) {
	    System.out.println("process1RRCoordSet(" + pdbId + ")");
	}

        _currentPdbId = pdbId;

	if (_rrTarLevel == RRTAR_LEVEL_PROCESS ||
	  _rrDistRLevel == RRDISTR_LEVEL_PROCESS) {
	    String resListFile = _restraintOnly ? null :
	      _dataDir + File.separator + _name +
	      S2DNames.RES_LIST_SUFFIX + S2DNames.DAT_SUFFIX;

	    S2DNmrStarRRIfc rrStar;
	    S2DNmrStarRRIfcFactory factory = new S2DNmrStarRRIfcFactory();
	    if (rrFile != null) {
	        rrStar = (S2DNmrStarRRIfc)factory.createFromFile(rrFile);
	    } else {
	        rrStar = factory.createFromId(pdbId);
	    }
	    saveFrameAtomicCoords(rrStar, null, _currentFrameIndex,
	      null, false, true);
	}

        _currentPdbId = null;
    }

    //-------------------------------------------------------------------
    // Do torsion angle processing for one restraints grid file.
    private void process1TAR(String tarFile, String tarUrl, String pdbId)
      throws S2DException
    {
        if (doDebugOutput(2)) {
	    System.out.println("process1TAR(" + tarFile + ", " +
	      tarUrl + ", " + pdbId + ")");
	    System.out.println("_tarLevel: " + _tarLevel);
	}

        _currentPdbId = pdbId;

	if (_tarLevel == TAR_LEVEL_LINK_CHECK ||
	  _tarLevel == TAR_LEVEL_LINK) {
	    try {
		if (_tarLevel == TAR_LEVEL_LINK_CHECK) {
		    // Note: this will throw an error if the relevant
		    // entry doesn't exist in the restraints grid.
		    tarUrl = (new S2DNmrStarRGIfcFactory(true)).
		      pdbIdToUrl(pdbId);
		}
	        _summary.writeTorsionAngleCGI(_currentPdbId, tarUrl,
		  _currentFrameIndex, false);
            } catch(IOException ex) {
                System.err.println(
		  "IOException writing torsion angle restraints: " +
		  ex.toString());
	        throw new S2DError("Can't write torsion angle restraints");
	    } finally {
	        _summary.endFrame();
	    }

	} else if (_tarLevel == TAR_LEVEL_PROCESS) {
	    S2DNmrStarRGIfc rgStar;
	    S2DNmrStarRGIfcFactory factory =
	      new S2DNmrStarRGIfcFactory(true);
	    if (tarFile != null) {
	        rgStar = (S2DNmrStarRGIfc)factory.createFromFile(tarFile);
	    } else if (tarUrl != null) {
	        rgStar = factory.createFromUrl(tarUrl, pdbId);
	    } else {
	        rgStar = factory.createFromId(pdbId);
	    }

	    S2DTorsionAngle tar = new S2DTorsionAngle(_name, _longName,
	      rgStar, _dataDir, _sessionDir, _summary, _currentPdbId);

	    tar.writeRestraints(_currentFrameIndex, false);
	} else {
	    throw new S2DError("Illegal _tarLevel value: " + _tarLevel);
	}

        _currentPdbId = null;
    }

    // TEMP -- see if this can be combined with process1TAR -- maybe
    // pass in factory object, etc. (see how much commonality there is
    // once I get everything working
    //-------------------------------------------------------------------
    // Do torsion angle processing for one remediated restraints file.
    private void process1RRTAR(String rrFile, String pdbId)
      throws S2DException
    {
        if (doDebugOutput(2)) {
	    System.out.println("process1RRTAR(" + rrFile + ", " +
	      pdbId + ")");
	    System.out.println("_rrTarLevel: " + _rrTarLevel);
	}

        _currentPdbId = pdbId;

	if (_rrTarLevel == RRTAR_LEVEL_LINK_CHECK ||
	  _rrTarLevel == RRTAR_LEVEL_LINK) {
	    try {
		String rrUrl = null;
		if (_rrTarLevel == RRTAR_LEVEL_LINK_CHECK) {
		    // Note: this will throw an error if the relevant
		    // entry doesn't exist in the remediated restraints.
		    rrUrl = (new S2DNmrStarRRIfcFactory()).
		      pdbIdToUrl(pdbId);
		}
	        _summary.writeTorsionAngleCGI(_currentPdbId, rrUrl,
		  _currentFrameIndex, true);
            } catch(IOException ex) {
                System.err.println(
		  "IOException writing torsion angle restraints: " +
		  ex.toString());
	        throw new S2DError("Can't write torsion angle restraints");
	    } finally {
	        _summary.endFrame();
	    }

	} else if (_rrTarLevel == RRTAR_LEVEL_PROCESS) {
	    S2DNmrStarRRIfc rrStar;
	    S2DNmrStarRRIfcFactory factory = new S2DNmrStarRRIfcFactory();
	    if (rrFile != null) {
	        rrStar = (S2DNmrStarRRIfc)factory.createFromFile(rrFile);
	    } else {
	        rrStar = factory.createFromId(pdbId);
	    }

	    S2DTorsionAngle tar = new S2DTorsionAngle(_name, _longName,
	      rrStar, _dataDir, _sessionDir, _summary, _currentPdbId);

	    tar.writeRestraints(_currentFrameIndex, true);
	} else {
	    throw new S2DError("Illegal _rrTarLevel value: " + _rrTarLevel);
	}

        _currentPdbId = null;
    }

    //-------------------------------------------------------------------
    // Do distance restraint processing for one restraints grid file.
    private void process1DistR(String distRFile, String distRUrl,
      String pdbId) throws S2DException
    {
        if (doDebugOutput(2)) {
	    System.out.println("process1DistR(" + distRFile + ", " +
	      distRUrl + ", " + pdbId + ")");
	    System.out.println("_distRLevel: " + _distRLevel);
	}

        _currentPdbId = pdbId;

	if (_distRLevel == DISTR_LEVEL_LINK_CHECK ||
	  _distRLevel == DISTR_LEVEL_LINK) {
	    try {
		if (_distRLevel == DISTR_LEVEL_LINK_CHECK) {
		    // Note: this will throw an error if the relevant
		    // entry doesn't exist in the restraints grid.
		    distRUrl = (new S2DNmrStarRGIfcFactory(false)).
		      pdbIdToUrl(pdbId);
		}
	        _summary.writeDistRestraintCGI(_currentPdbId, distRUrl,
		  _currentFrameIndex, false);
            } catch(IOException ex) {
                System.err.println(
		  "IOException writing distance restraints: " +
		  ex.toString());
	        throw new S2DError("Can't write distance restraints");
	    } finally {
	        _summary.endFrame();
	    }

	} else if (_distRLevel == DISTR_LEVEL_PROCESS) {
	    S2DNmrStarRGIfc rgStar;
	    S2DNmrStarRGIfcFactory factory =
	      new S2DNmrStarRGIfcFactory(false);
	    if (distRFile != null) {
	        rgStar = (S2DNmrStarRGIfc)factory.createFromFile(
		  distRFile);
	    } else if (distRUrl != null) {
	        rgStar = factory.createFromUrl(distRUrl, pdbId);
	    } else {
	        rgStar = factory.createFromId(pdbId);
	    }

	    S2DDistRestraint distR = new S2DDistRestraint(_name, _longName,
	      rgStar, _dataDir, _sessionDir, _summary, _currentPdbId);

	    distR.writeRestraints(_currentFrameIndex, false);
	} else {
	    throw new S2DError("Illegal _distRLevel value: " + _distRLevel);
	}

        _currentPdbId = null;
    }

    //-------------------------------------------------------------------
    // Do distance restraint processing for one remediated restraints file.
    private void process1RRDistR(String rrFile, String pdbId)
      throws S2DException
    {
        if (doDebugOutput(2)) {
	    System.out.println("process1RRDistR(" + rrFile + ", " +
	      pdbId + ")");
	    System.out.println("_rrDistRLevel: " + _rrDistRLevel);
	}

        _currentPdbId = pdbId;

	if (_rrDistRLevel == RRDISTR_LEVEL_LINK_CHECK ||
	  _rrDistRLevel == RRDISTR_LEVEL_LINK) {
	    try {
		String rrUrl = null;
		if (_rrDistRLevel == RRDISTR_LEVEL_LINK_CHECK) {
		    // Note: this will throw an error if the relevant
		    // entry doesn't exist in the remediated restraints.
		    rrUrl = (new S2DNmrStarRRIfcFactory()).
		      pdbIdToUrl(pdbId);
		}
	        _summary.writeDistRestraintCGI(_currentPdbId, rrUrl,
		  _currentFrameIndex, true);
            } catch(IOException ex) {
                System.err.println(
		  "IOException writing distance restraints: " +
		  ex.toString());
	        throw new S2DError("Can't write distance restraints");
	    } finally {
	        _summary.endFrame();
	    }

	} else if (_rrDistRLevel == RRDISTR_LEVEL_PROCESS) {
	    S2DNmrStarRRIfc rrStar;
	    S2DNmrStarRRIfcFactory factory = new S2DNmrStarRRIfcFactory();
	    if (rrFile != null) {
	        rrStar = (S2DNmrStarRRIfc)factory.createFromFile(rrFile);
	    } else {
	        rrStar = factory.createFromId(pdbId);
	    }

	    S2DDistRestraint distR = new S2DDistRestraint(_name, _longName,
	      rrStar, _dataDir, _sessionDir, _summary, _currentPdbId);

	    distR.writeRestraints(_currentFrameIndex, true);
	} else {
	    throw new S2DError("Illegal _rrDistRLevel value: " +
	      _rrDistRLevel);
	}

        _currentPdbId = null;
    }

    //-------------------------------------------------------------------
    // Determine whether to do debug output based on the current debug
    // level settings and the debug level of the output.
    private static boolean doDebugOutput(int level)
    {
    	if (DEBUG >= level || _verbosity >= level) {
	    if (level > 0) System.out.print("DEBUG " + level + ": ");
	    return true;
	}

	return false;
    }
}

// ========================================================================

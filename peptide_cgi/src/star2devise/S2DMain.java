// ========================================================================
// DEVise Data Visualization Software
// (c) Copyright 2000-2006
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
// Revision 1.19  2006/09/20 14:50:45  wenger
// Made some minor fixes to make_view.
//
// Revision 1.18  2006/09/19 22:05:07  wenger
// Fixed a bug in the chem shift reference 1 visualization -- some of
// the views had 'shape attr 3'/'X Offset' set to a non-zero value
// (I think because of an old GAttr link) -- this caused the average
// and tolerance bars to be drawn at the wrong location when run with
// DEVise 1.9.x.  (I think in previous DEVise versions 'shape attr 3'
// had no effect on drawing Bars.)
//
// Revision 1.17  2006/09/06 16:55:00  wenger
// Increased JVM memory to 512m because that's what it took to get 6011/
// 1RJJ coordinate vis processing to work at BMRB (marlin); added test37,
// which tests 6011.
//
// Revision 1.16  2006/08/21 21:21:16  wenger
// Changed version to 11.0.1.
//
// Revision 1.15  2006/08/16 16:04:21  wenger
// Updated coordinate visualization help to match new version with
// Jmol and selection trees.
//
// Revision 1.14  2006/07/11 18:33:06  wenger
// Minor cleanups for DEVise 1.9.0 -- changed versions, etc.
//
// Revision 1.13  2006/05/26 16:50:55  wenger
// Merged peptide_cgi_jmol_proto_br_0 thru peptide_cgi_jmol_proto_br_1
// to the trunk.
//
// Revision 1.12  2006/05/11 21:10:25  wenger
// Fixed problems with some html page titles being based on the BMRB
// ID rather than the name, etc.  Fixed test36 to work with new LACS
// file names.
//
// Revision 1.11  2006/05/11 20:03:16  wenger
// Visualization info in ambiguity code, Pistachio, and LACS sessions
// now rendered on the client side (I think rendering it on the server
// side was a holdover from a bug in DEVise/JS 1.7.18).
//
// Revision 1.10  2006/05/11 19:39:58  wenger
// Disabled appropriate actions in various views that should have already
// had this done.
//
// Revision 1.9  2006/05/03 18:54:01  wenger
// Got rid of kludges in chemical shift reference sessions required
// for pre-1.7.19 DEVise versions.
//
// Revision 1.8  2006/05/02 21:40:44  wenger
// Makefile dev_test target now links the session_templates
// directory into the dev_test/session directory.
//
// Revision 1.7  2006/05/02 14:31:00  wenger
// Version changed to 10.9.2 for release.
//
// Revision 1.6  2006/04/26 19:12:42  wenger
// Peptide-CGI now gets mmCIF files from the production PDB http site
// as opposed to the beta ftp site.
//
// Revision 1.5  2006/04/12 15:55:48  wenger
// Improved some error messages, fixed a comment.
//
// Revision 1.4  2006/02/28 15:54:30  wenger
// Changed version string, slight debug output improvements.
//
// Revision 1.3.2.7  2006/05/18 20:48:16  wenger
// Added $Struct_type to 3D mapping to enable backbone/side chains/
// protons selection tree in the JavaScreen; default in DEVise session
// is to show all structure types, since selection is now in the JS.
//
// Revision 1.3.2.6  2006/03/17 19:27:09  wenger
// Added Jmol link to coordinate visualization pages.
//
// Revision 1.3.2.5  2006/03/01 16:36:58  wenger
// Fixed DEVISE_MIN_VERSION.
//
// Revision 1.3.2.4  2006/02/28 22:40:09  wenger
// Changed atom_coord_jmol.base to match implementation of Jmol
// highlighting in the JavaScreen.
//
// Revision 1.3.2.3  2006/02/23 22:07:24  wenger
// Added flag for whether or not 3D views should use Jmol.
//
// Revision 1.3.2.2  2006/02/21 19:20:20  wenger
// Peptide-CGI now translates atom names such as HH11 to
// 1HH1 in the output form Jmol.
//
// Revision 1.3.2.1  2006/02/20 21:57:40  wenger
// Peptide-CGI now generates data, sessions, etc., that use
// Jmol for 3D molecule visualization.
//
// Revision 1.3  2006/02/01 21:34:32  wenger
// Merged peptide_cgi_10_8_0_br_0 thru peptide_cgi_10_8_0_br_2
// to the trunk.
//
// Revision 1.2  2006/02/01 20:23:11  wenger
// Merged V2_1b4_br_0 thru peptide_cgi_10_8_0_base to the
// trunk.
//
// Revision 1.1.2.121.2.27  2006/01/24 22:27:01  wenger
// Verified fix of bug 049 with Eldon.
//
// Revision 1.1.2.121.2.26  2006/01/24 19:34:24  wenger
// Fixed bug 049 (pending confirmation from Eldon):
// errors in the chemical shift index calculation
// (use individual CSI values instead of deltashifts, take
// ambiguity codes into account for GLY special case).
//
// Revision 1.1.2.121.2.25  2005/11/23 21:07:24  wenger
// A bunch of changes to the Pistachio visualization:
// - Added titles to more views to clarify things;
// - Changed JS background color so inter-view borders show up
//   (also for ambiguity visualization);
// - H -> "backbone proton" in data select view;
// - Changed color select view and color legend view;
// - Unknown -> unassigned in color legend;
// - Moved unassigned to right in color legend;
// - Y axis label in data view.
//
// Revision 1.1.2.121.2.24  2005/11/04 18:57:54  wenger
// Added LACS stuff to config doc; version is now 10.9.0.
//
// Revision 1.1.2.121.2.23  2005/11/04 17:56:26  wenger
// Added command-line arguments, config, properties, etc., to
// specify LACS processing level, LACS filename template, and
// LACS URL -- so now we have the option to have the software
// automatically try to process the appropriate LACS file;
// updated some tests accordingly.  Test_all now names failed
// tests at the end.
//
// Revision 1.1.2.121.2.22  2005/11/03 17:52:51  wenger
// Added the capability to select large (1024x768 pixel) or
// small (800x600 pixel) visualizations.
//
// Revision 1.1.2.121.2.21  2005/11/02 23:21:30  wenger
// Changed LACS-related STAR file tags to be properly defined;
// horizontal line in LACS visualizations is at *minus* y offset.
//
// Revision 1.1.2.121.2.20  2005/11/02 21:13:24  wenger
// Changed ovals to text Os in LACs visualization so they stay the
// same size in *pixels*; fixed up titles for LACS stuff in summary
// html page, specific html pages, and sessions.
//
// Revision 1.1.2.121.2.19  2005/11/02 20:33:18  wenger
// LACS visualization now has correct axis labels.
//
// Revision 1.1.2.121.2.18  2005/11/02 17:00:38  wenger
// Changed LACS visualization to have horizontal and vertical reference
// lines, as requested by Hamid and Eldon; added visualization info view.
//
// Revision 1.1.2.121.2.17  2005/10/14 21:19:28  wenger
// Most LACS processing now in place -- still needs lots of cleanup,
// though.
//
// Revision 1.1.2.121.2.16  2005/10/10 16:57:17  wenger
// Changed version to 10.8.3.
//
// Revision 1.1.2.121.2.15  2005/10/07 21:13:27  wenger
// Fixed bug 047 (home on data value views in coordinate and
// similar sessions messes up filters):  added data_pile_supplement
// link and set setDoHomeOnVisLink for ViewUnselectedDataChild views
// to 1 in the relevant sessions.
//
// Revision 1.1.2.121.2.14  2005/10/07 20:52:56  wenger
// Fixed bug 045 (moving data_value_cursor sometimes changes data
// view Y axis range); found bug 047.
//
// Revision 1.1.2.121.2.13  2005/10/07 20:21:26  wenger
// More minor cleanups because of problems on basslet, including
// increasing maximum java memory to 128 MB.
//
// Revision 1.1.2.121.2.12  2005/10/05 18:02:40  wenger
// Finalized configuration for basslet.
//
// Revision 1.1.2.121.2.11  2005/09/14 17:50:59  wenger
// Better error messages if we retry because of running out of
// memory (this happened a lot (but not always) with test14 on
// basslet).
//
// Revision 1.1.2.121.2.10  2005/09/14 15:14:22  wenger
// Config changes for Osaka, and for manatee and basslet at BMRB;
// new Pistachio test file; fixed test23 to work on non-CS machines;
// better Pistachio error message.
//
// Revision 1.1.2.121.2.9  2005/08/10 18:58:20  wenger
// Added BMRB config for viewing unreleased entries; documented
// config for viewing unreleased entries; more fixes to the
// installation script; minor changes to vur (removed unneeded
// args).
//
// Revision 1.1.2.121.2.8  2005/07/27 15:58:29  wenger
// Fixed S2DNmrStarIfc.getPdbIdsFromMolSys() to work for NMR-STAR 3.0,
// added test34 which tests that; better error handling in
// S2DUtils.arrayStr2Double().
//
// Revision 1.1.2.121.2.7  2005/07/15 16:54:04  wenger
// Updated NMR-STAR version test for latest 3.0 version string format;
// added test33 with corresponding data (latest 3.0 format).
//
// Revision 1.1.2.121.2.6  2005/06/29 18:37:19  wenger
// Fixed some errors in the new 2D ambiguity and figure of merit
// visualizations.
//
// Revision 1.1.2.121.2.5  2005/06/28 17:08:22  wenger
// Ambiguity code and figure of merit visualizations now use 2D main
// views; changed "Pistachio" to "assignment figure of merit" in
// visualizations, etc.
//
// Revision 1.1.2.121.2.4  2005/05/25 15:04:59  wenger
// Added test30 (minimum possible save frames/tags for Pistachio
// processing).
//
// Revision 1.1.2.121.2.3  2005/05/19 16:33:52  wenger
// Changed version to 10.8.0.
//
// Revision 1.1.2.121.2.2  2005/05/19 16:07:43  wenger
// Merged nmrfam_mods2_br (argh -- must have forgotten to make
// nmrfam_mods2_br_0 tag!) thru nmrfam_mods2_br_3 to
// peptide_cgi_10_8_0_br.
//
// Revision 1.1.2.121.2.1  2005/05/18 19:44:51  wenger
// Some cleanup of the NMR-STAR 3.0 multi-entity code.
//
// Revision 1.1.2.121  2005/04/22 21:41:10  wenger
// Okay, chemical shift data now pretty much works with NMR-STAR
// 3.0 (although a lot of cleanup is still needed).  The other
// types of data still need to be adapted to work with the
// "multiple entities per loop" model of 3.0.
//
// Revision 1.1.2.120  2005/04/20 15:55:00  wenger
// Added -dev_version flag that gives the minimum required DEVise
// version for the current version of Peptide-CGI.
//
// Revision 1.1.2.119.4.5  2005/05/18 18:08:14  wenger
// Changed Pisatchio percentage calculation to be relative to the number
// of atoms that actually exist in each amino acid, not just the number
// we have figures of merit for; the Pistachio visualization now comes
// up with "Color by figure of merit" selected; minor improvements to
// ambiguity debug/error messages.
//
// Revision 1.1.2.119.4.4  2005/05/13 20:07:10  wenger
// Changed version to 10.7.1.
//
// Revision 1.1.2.119.4.3  2005/05/13 19:57:49  wenger
// Fixed bug 042 (unrecognized amino acid abbreviation prevents
// generation of ambiguity visualization); added related test.
//
// Revision 1.1.2.119.4.2  2005/05/13 18:27:21  wenger
// Fixed bug 040 (processing totally fails for entries that have 2 or
// more entities and no chem shifts); added a bunch of related tests.
//
// Revision 1.1.2.119.4.1  2005/05/12 19:07:40  wenger
// Merged nmrfam_mods_br_0 thru nmrfam_mods_br_1 to new
// nmrfam_mods2_br (created to get ambiguity visualization help
// and fix to coordinate visualization help).
//
// Revision 1.1.2.119.2.2  2005/05/12 17:40:33  wenger
// The format of the input file name (e.g., bmrXXXX.str, or whatever)
// and the comment email for the web pages are now configurable.
//
// Revision 1.1.2.119.2.1  2005/05/12 14:10:12  wenger
// Peptide-CGI now allows non-numeric BMRB IDs; changed test3 to make
// sure cache is used when it should be; added test26 to test non-
// numeric BMRB ID.
//
// Revision 1.1.2.119  2005/04/07 17:22:37  wenger
// Changed version to 10.7.0.
//
// Revision 1.1.2.118  2005/04/05 19:06:53  wenger
// More ambiguity vis changes: "color by ambiguity code" is initially
// selected; changes to some of the ambiguity-related strings.
//
// Revision 1.1.2.117  2005/03/22 21:47:56  wenger
// Minor cleanups to ambiguity visualization.
//
// Revision 1.1.2.116  2005/03/22 20:34:37  wenger
// Merged ambiguity_vis2_br_0 thru ambiguity_vis2_br_3 to V2_1b4_br.
//
// Revision 1.1.2.115  2005/03/22 19:22:53  wenger
// Fixes to atomic coordinates and Pistachio visualizations: data X select
// cursor can't go outside view; we show the full data range in the data
// view at startup.  Fixed some typos in the atomic coordinate help page.
// Added a help page for the Pistachio visualization (without any info).
// Changed makefile to force Java 1.3-compatible class files (because of
// nasty new Java 1.5).
//
// Revision 1.1.2.114.2.4  2005/03/21 21:07:37  wenger
// Changed ambiguity percentage calculation to be relative to the number
// of atoms that actually exist in each amino acid, not just the number
// we have chem shift values for; added a check of the residue list
// against residues in the chem shift save frame (optional, but almost
// all tests now use it); added ambiguity output files to test checks;
// added ambiguity visualization help file (not yet complete).
//
// Revision 1.1.2.114.2.3  2005/03/11 23:33:20  wenger
// Ambiguity visualization is mostly working (incorporating some feedback
// from Eldon); still needs some cleanup to the calculations, though.
//
// Revision 1.1.2.114.2.2  2005/03/10 20:22:10  wenger
// Mods to S2DPistachioTable in preparation for adding a corresponding
// S2DAmbiguityTable.
//
// Revision 1.1.2.114.2.1  2005/03/10 19:27:35  wenger
// Merged ambiguity_vis_br_0 thru ambiguity_vis_br_end to
// ambiguity_vis2_br.
//
// Revision 1.1.2.114  2005/02/28 21:52:44  wenger
// Changed Peptide-CGI javac options so it's compatible with a
// Java 1.3 JRE.
//
// Revision 1.1.2.113  2005/02/25 15:52:15  wenger
// Changed version to 10.6.0.
//
// Revision 1.1.2.112  2005/02/25 15:42:24  wenger
// Okay, this should be the final Pistachio colors...
//
// Revision 1.1.2.111  2005/02/23 17:45:56  wenger
// Added TRP side chain HE1/NE1 to "Simulated 1H-15N backbone HSQC
// spectrum" visualization.
//
// Revision 1.1.2.110  2005/02/23 16:21:07  wenger
// Added session postscript to fix bug of h_vs_n session not showing
// all data; added or improved session postscripts of other sessions,
// too (for safety).
//
// Revision 1.1.2.109  2005/02/17 16:02:38  wenger
// Okay, another try at Pistachio colors -- highlight is green, green
// in figure of merit colors replaced by purple.
//
// Revision 1.1.2.108  2005/02/16 22:33:23  wenger
// Fixed bug with incorrect figure of merit coloring for some residue
// connections.
//
// Revision 1.1.2.107  2005/02/14 21:53:42  wenger
// Added figure of merit value to Pistachio coordinate data so it
// appears in drill-down.
//
// Revision 1.1.2.106  2005/02/11 22:21:11  wenger
// Changed version to 10.6.0x10 because Pistachio stuff isn't done yet.
//
// Revision 1.1.2.105  2005/02/11 21:12:46  wenger
// More fixes to Pistachio colors; fixed a typo in the pistachio_legend
// schema.
//
// Revision 1.1.2.104  2005/02/09 20:12:37  wenger
// Changed colors for "color by figure of merit" option; set up
// print colors.
//
// Revision 1.1.2.103.2.1  2005/03/10 18:34:05  wenger
// I need to commit the ambiguity stuff I've done so far so I can make
// a new ambiguity branch that has the latest Pistachio changes.
//
// Revision 1.1.2.103  2005/02/01 21:37:23  wenger
// More cleanups, especially to the Pistachio code.
//
// Revision 1.1.2.102  2005/01/31 23:02:54  wenger
// Merged pistachio_vis_br_0 thru pistachio_vis_br_1a to V2_1b4_br.
//
// Revision 1.1.2.101  2005/01/31 21:35:44  wenger
// Slight mods to atomic coordinates template for better highlight
// selection; s2d chem shift ref timeout increased to 120 sec., added
// try-again message if timed out; cleaned up warning/error messages
// somewhat.
//
// Revision 1.1.2.100  2004/12/21 22:59:11  wenger
// Fixed bug where I was using data_dir instead of csr_data_dir at
// one point in the chem shift ref stuff; changed dummy_csr_server
// for test6 to catch this error; changed vsc so that default args
// can be overridden.
//
// Revision 1.1.2.99  2004/12/20 20:02:17  wenger
// Help changes as requested by Eldon; changed minimum ChemShift version
// to 1.4.1 (filename changes in csr_wrapper script); changed s2d version
// to 10.5.2.
//
// Revision 1.1.2.98  2004/12/17 20:04:28  wenger
// Minor cleanup to config, etc.
//
// Revision 1.1.2.97  2004/12/16 22:01:55  wenger
// Chem shift reference link-only processing option is pretty much
// implemented -- the actual csr_wrapper script on leech needs some
// mods, and I need to test Peptide-CGI with the real ChemShift
// software, but other than that I think it's done.
//
// Revision 1.1.2.96  2004/12/14 19:00:24  wenger
// Minor improvements to chem shift ref error messages.
//
// Revision 1.1.2.95  2004/12/14 18:02:35  wenger
// Fixed a bug that caused PDB IDs to sometimes get put into the
// chem shift ref list twice -- caused extra work when trying to
// do chem shift ref based on PDB IDs in the BMRB entry; we also
// delete the error file in S2DChemShiftRef to be safer.  Added
// test22 and test23 to test chem shift ref processing, made improvements
// to dummy_csr_server.  Set shorter chem shift ref timeout in s2d.
//
// Revision 1.1.2.94  2004/12/08 23:07:18  wenger
// Oops -- re-fixed deltashift session template; added titles to
// specific html pages; enlarges the JavaScreen.
//
// Revision 1.1.2.93  2004/12/07 19:02:55  wenger
// Added help info to all visualization help pages (pending approval
// from Eldon); better view help in the coordinate session template;
// fixed Y axis label fonts (send to JS) in the deltashift session
// template.
//
// Revision 1.1.2.92  2004/12/03 21:04:14  wenger
// Info view now has date without time, and no Peptide-CGI version
// number (as requested by Eldon); they are now in the view help.
//
// Revision 1.1.2.91  2004/12/03 17:18:34  wenger
// Added BMRB accession number, PDB ID (if relevant), timestamp and
// Peptide-CGI version number to Peptide-CGI visualizations.
//
// Revision 1.1.2.90  2004/11/24 16:15:12  wenger
// Final cleanup of major chem shift reference changes, version
// is now 10.5.0.
//
// Revision 1.1.2.89  2004/11/18 22:18:23  wenger
// Chem shift reference visualization 1 now shows CA and CB values
// for *all* residues, not just those that are a helix or sheet.
//
// Revision 1.1.2.88  2004/11/18 19:33:17  wenger
// Added more descriptive chem shift reference links in Jafar mode,
// since the section headers don't show up in the Jafar GUI.
//
// Revision 1.1.2.87  2004/11/18 19:10:34  wenger
// Peptide-CGI now checks the version of the ChemShift software, since
// the current version of Peptide-CGI needs at least ChemShift 1.4.0.
//
// Revision 1.1.2.86  2004/11/16 23:51:28  wenger
// Chem shift ref session #1 changed to have helix & sheet in the same
// view (pile); sample chem shift ref data changed to reflect changes
// in the ChemShift program -- HA2 and HA3 are ignored, CA and CB
// averages include all residues (not just ones that are helices and
// sheets), data for CSR #1 have helix and sheet averages, etc., together
// instead of separate; more changes to the chem shift ref help.
//
// Revision 1.1.2.85  2004/11/15 19:44:31  wenger
// All NMR Browser visualization help now goes to a single html window,
// instead of popping up a new window each time; better layout for
// chem shift reference stuff in the summary page; fixed a problem in
// the CSR #3 help page.
//
// Revision 1.1.2.84  2004/11/12 21:02:20  wenger
// Implemented most of the latest chem shift reference visualization
// change requests (still needs changes in the actual chemShift
// program).
//
// Revision 1.1.2.83.2.10  2005/01/28 18:49:01  wenger
// Added "color by figure of merit" option to the Pistachio visualization.
//
// Revision 1.1.2.83.2.9  2005/01/27 17:37:54  wenger
// Moved HA, HA2, and HA3 atoms closer to the backbone as requested by
// Eldon.
//
// Revision 1.1.2.83.2.8  2005/01/24 20:49:16  wenger
// HAs, HA2s, HA3s now treated as backbone for Pistachio; vertical
// spacing of wrapped structure is tighter; wrap length is 10
// instead of 5.
//
// Revision 1.1.2.83.2.7  2005/01/20 20:13:57  wenger
// Hs are now treated as backbone atoms for Pistachio visualizations.
//
// Revision 1.1.2.83.2.6  2005/01/20 19:45:19  wenger
// Implemented "wrapping" of the Pistachio coordinates.
//
// Revision 1.1.2.83.2.5  2005/01/12 20:46:41  wenger
// Pistachio processing is now integrated into the normal Peptide-CGI
// processing -- the Pistachio visualization is generated autmatically
// if the Pistachio data exists.  (Still needs some cleanup, though.)
// (We generate the Pistachio visualization by generating a temporary
// mmCIF file with coordinates -- that is then run through the normal
// coordinate processing to generate the DEVise file with Pistachio
// coordinates.)
//
// Revision 1.1.2.83.2.4  2005/01/11 21:24:07  wenger
// Added new S2DPistachioCoords class, but it's not really integrated
// into the processing yet.
//
// Revision 1.1.2.83.2.3  2004/12/20 22:46:15  wenger
// Improved "last residue" calculation so out-of-order residues don't
// cause array bounds exception; fixed NMR-STAR 3.0 problem with tag
// names for 3-letter residue lists (bmr4096.str.nmrstr has inconsistent
// 1-letter and 3-letter residue lists -- kludged it so tests work,
// removed the 1-letter residue list I added to bmr6318_pistachio.str.nmrstr).
//
// Revision 1.1.2.83.2.2  2004/12/01 21:56:24  wenger
// Changed "HN"s to "H"s in Pistachio code because BMRB files have "H"
// where Hamid expected "HN".
//
// Revision 1.1.2.83.2.1  2004/11/11 23:13:02  wenger
// Early prototype of Pistachio calculations and visualization;
// session, etc., are not automatically generated yet.  Note: I
// added some HN data to bmr6318_pistachio.str.nmrstr for the
// sake of my testing.
//
// Revision 1.1.2.83  2004/11/04 20:45:11  wenger
// Changed to "lazy" checking of PDB timestamps -- we only do it when
// we are actually processing the coordinate data, not every time we
// generate the summary html file.
//
// Revision 1.1.2.82  2004/10/27 20:04:13  wenger
// More changes to chem shift ref vis 2: horizontal lines at 0, average
// lines no longer white, better legend; hopefully fixed main title text
// size problems in all chem shift ref visualizations; also corrected
// comment chars in a couple of schemas.
//
// Revision 1.1.2.81  2004/10/20 22:06:09  wenger
// Changed CSR vis 2 to display chem shift difference averages for
// each atom type across all secondary structures (as requested by Eldon);
// other minor cleanups.
//
// Revision 1.1.2.80  2004/10/20 18:54:16  wenger
// Did to-do 046 (in Jafar, use only PDB IDs with >=97% identity; still
// require 100% in NMR Browser).
//
// Revision 1.1.2.79  2004/10/11 23:36:52  wenger
// Converted the chem shift reference visualizations to use view symbols,
// to make it easier to add axis labels; also fixed some color problems
// with the chem shift reference visualizations; added test16 to generate
// better data for the chem shift reference templates.  Note that the
// chem shift ref sessions have text that should be drawn by the JS but
// is not, because of DEVise/JS bug 905.
//
// Revision 1.1.2.78  2004/09/25 15:57:49  wenger
// Peptide-CGI version and generation time are now visible as opposed to
// being comments.
//
// Revision 1.1.2.77  2004/09/15 14:29:46  wenger
// Version 10.4.0 ready to go...
//
// Revision 1.1.2.76  2004/09/14 22:07:14  wenger
// Added PDB ID to 3D structure link strings so it shows up in
// the Jafar GUI.
//
// Revision 1.1.2.75  2004/09/14 16:47:15  wenger
// Deltashift and CSI sessions now only show CB shifts when there
// are "real" (not GLY HA3->CB) CB shifts to show.
//
// Revision 1.1.2.74  2004/09/13 18:40:11  wenger
// Major restructuring of S2DChemShift class -- delta shifts are now
// calculated and saved in arrays instead of being calculated while
// were writing them out, and then read back in to calculate the CSIs.
//
// Revision 1.1.2.73  2004/09/13 16:06:30  wenger
// Added BMRB ID to chem shift ref session info.
//
// Revision 1.1.2.72  2004/09/10 22:38:23  wenger
// Added X=Y line (as requested by Eldon) to chem shift reference
// visualization #3.
//
// Revision 1.1.2.71  2004/09/01 19:35:38  wenger
// Temporarily disabled chem shift reference visualization 3,
// pending approval by Eldon; I need to do a bug fix release.
//
// Revision 1.1.2.70  2004/07/02 20:58:05  wenger
// Major changes to the chem_shift_ref3 visualization (we now
// show everything in one view, color-coded by secondary structure,
// with a cursor to select by secondary structure as well);
// minor cleanups to the chem_shift_ref visualizations.
//
// Revision 1.1.2.69  2004/06/23 17:39:25  wenger
// Preliminary version of chem shift reference visualization 3 in
// place; cleaned up 4317 chem shift ref test data; changed fonts
// in chem shift ref visualization 1.
//
// Revision 1.1.2.68  2004/06/16 19:23:30  wenger
// Added headers to data files generated by Peptide-CGI.
//
// Revision 1.1.2.67  2004/06/03 16:17:26  wenger
// Fixed things so -version and -usage work even without properties
// file.
//
// Revision 1.1.2.66  2004/06/03 14:41:37  wenger
// Set up configuration to default to doing chem shift ref
// calculation at BMRB but not at other installations.
//
// Revision 1.1.2.65  2004/06/02 17:45:06  wenger
// Added more debug code related to getting PDB IDs.
//
// Revision 1.1.2.64  2004/05/25 21:40:34  wenger
// Put DEVise print mode support back into session templates.
//
// Revision 1.1.2.63  2004/04/09 18:15:15  wenger
// Added -debug flag to control output of stace traces.
//
// Revision 1.1.2.62  2004/04/09 17:23:48  wenger
// We now get PDB IDs from both the molecular_system save frame and
// the monomeric_polymer save frame.
//
// Revision 1.1.2.61  2004/03/25 20:45:29  wenger
// Added "tolerance" rectangles to first chem shift reference
// visualization (needs chemShift version 1.3.1 or higher doing the
// actual calculations); updated dummy chem shift ref data to
// actual output of chemShift; various related test script updates.
//
// Revision 1.1.2.60  2004/03/04 22:42:33  wenger
// Changed chem shift ref/PDB message as requested by Eldon.
//
// Revision 1.1.2.59  2004/03/04 22:14:32  wenger
// Added PDB ID to chem shift reference visualizations, because of
// the re-try code in Peptide-CGI (we don't for sure know ahead of
// time which PDB ID the chem shift ref calculations will be based
// on).
//
// Revision 1.1.2.58  2004/03/04 18:26:42  wenger
// Fixed bug 030 (static variable problems in Jafar); removed
// S2DTmpFile (no longer used).
//
// Revision 1.1.2.57  2004/03/04 16:44:14  wenger
// Added Jafar simulation to help with tracking down Jafar-
// specific problems; fixes to install script; minor improvements
// to s2d script.
//
// Revision 1.1.2.56  2004/03/03 17:31:36  wenger
// Fixed bug 028 (problem handling unrecognized amino acid
// abbreviations); commented out "missing atom" warnings
// in atomic coordinate code.
//
// Revision 1.1.2.55  2004/03/01 15:52:04  wenger
// Peptide-CGI specific html files no longer use the JavaScreen
// "loader" applet -- they load the JavaScreen applet directly
// (fixes "half-screen" problem seen on Windows).
//
// Revision 1.1.2.54  2004/02/06 01:48:57  wenger
// We now re-try the chem shift reference processing with subsequent
// PDB IDs if it fails (not times out) with the first one.
//
// Revision 1.1.2.53  2004/02/02 23:06:12  wenger
// Changed second chem shift ref session to have different colors for
// sheet, helix, and other secondary structures.
//
// Revision 1.1.2.52  2004/01/30 18:20:41  wenger
// S2D errors and warnings are now saved to files (the files are
// named according to the data name).
//
// Revision 1.1.2.51  2004/01/30 14:50:51  wenger
// Fixed install script so it doesn't make the bmrb/dynamic_sessions
// link for a Jafar installation.
//
// Revision 1.1.2.50  2004/01/28 22:48:42  wenger
// Made session templates compatible with DEVise 1.7.17 (so I can
// release a new Peptide-CGI at BMRB without releasing a new
// DEVise).
//
// Revision 1.1.2.49  2004/01/28 20:26:07  wenger
// Fixed bug 025 (problem with missing residue list); note that
// bmr4096.str as committed has fix to the residue list that's
// not in the real entry yet.
//
// Revision 1.1.2.48  2004/01/28 15:48:24  wenger
// Fixed up view titles and help info for chemical shift reference
// visualizations.
//
// Revision 1.1.2.47  2004/01/26 22:18:36  wenger
// Got rid of JavaScript in specific html template; added mailto
// devise-sup.
//
// Revision 1.1.2.46  2004/01/26 18:55:36  wenger
// Incorporated "Eldon's" chemical shift reference session; added help
// pages for both chem shift ref visualizations (with no help info
// for now).
//
// Revision 1.1.2.45  2004/01/22 16:43:49  wenger
// Finished to-do item 020 (replace "/" with File.separator), other
// minor cleanups.
//
// Revision 1.1.2.44  2004/01/17 20:15:36  wenger
// PDB processing works for NMR-STAR 3.0 except for part of the protein
// checking (S2DNmrStar30.getMonoPolyFrame(), waiting for more info
// from Eldon and Steve), and some problems with 4096.
//
// Revision 1.1.2.43  2004/01/16 23:09:05  wenger
// PDB processing works for NMR-STAR 3.0 except for protein checking.
//
// Revision 1.1.2.42  2004/01/16 22:26:00  wenger
// NMR-STAR 3.0 support is now working except for saving residue
// counts, etc and protein check for chem shifts; haven't tested
// processing related PDB entries for 3.0 yet.
//
// Revision 1.1.2.41  2004/01/12 21:57:27  wenger
// Part way to implementing NMR-STAR 3.0 support -- S2DNmrStarIfc has
// framework in place, but not all details.
//
// Revision 1.1.2.40  2004/01/06 18:28:17  wenger
// Set up print color modes in session templates; fixed some minor window
// misalignments; fixed a minor problem in the bmrb-summary schema.
//
// Revision 1.1.2.39  2003/12/12 19:11:45  wenger
// Fixed bug 026 (structure link is still shown in summary page even if
// no bonds are written).
//
// Revision 1.1.2.38  2003/11/30 23:39:06  wenger
// Fixed up chem shift ref output in summary html file; added more
// debug code to S2DResList.
//
// Revision 1.1.2.37  2003/11/18 00:17:53  wenger
// Changed chem shift ref session to display diffs instead of random
// coil values (as per discussion with Gabriel); added test for
// tolerating bad PDB ID; improved chem shift ref error messages.
//
// Revision 1.1.2.36  2003/11/13 16:43:54  wenger
// Added -no_scripts command-line argument to avoid trying to run
// set_modes on stupid Windows.
//
// Revision 1.1.2.35  2003/11/13 16:09:57  wenger
// More changes to the latest chem shift reference code as a result of
// testing on leech.
//
// Revision 1.1.2.34  2003/11/12 17:24:29  wenger
// S2d no longer uses do_csr.bat scripts -- all that stuff is now directly
// done in Java, to get around the problem that DOS batch files don't work
// right when run from within Java.  (New code is not yet fully tested.)
//
// Revision 1.1.2.33  2003/11/12 16:10:10  wenger
// Various changes to try to get the damn chem shift ref stuff working on
// Windows.
//
// Revision 1.1.2.32  2003/11/10 22:03:20  wenger
// Various partly-tested changes to try to get chem shift ref integration
// working on the NMRFAM end.
//
// Revision 1.1.2.31  2003/10/30 23:30:07  wenger
// Version is now 8.0x5.
//
// Revision 1.1.2.30  2003/10/30 22:18:12  wenger
// Mods to get things to work with ChemShift on leech at BMRB.
//
// Revision 1.1.2.29  2003/10/30 16:57:13  wenger
// Added timeout on chemical shift reference processing.
//
// Revision 1.1.2.28  2003/10/29 17:14:42  wenger
// Star2devise now can do chem shift ref processing with Jafar-format
// input files.
//
// Revision 1.1.2.27  2003/10/23 18:08:56  wenger
// Basic chem shift ref stuff is in place -- session and specific html
// files are generated, summary html file has link, etc.  The main thing
// left is the mechanics of calling Dan's chem shift ref software --
// right now I am just using a script that creates dummy chem shift
// ref data files.
//
// Revision 1.1.2.26  2003/10/17 21:41:47  wenger
// Added chem shift reference session template and schemas; added -do_csr
// command-line argument.
//
// Revision 1.1.2.25  2003/10/17 21:21:02  wenger
// Fixed bug in string table name in allshifts template.
//
// Revision 1.1.2.24  2003/10/08 18:44:19  wenger
// Peptide-cgi now uses gzipped mmCIF files instead of compressed ones.
//
// Revision 1.1.2.23  2003/10/08 17:45:43  wenger
// Some cleanup of the uncompress code (not tested on Windows).
//
// Revision 1.1.2.22  2003/10/08 15:47:51  wenger
// Major improvements to test scripts (such as checking for the existance
// of the proper output files), added test_all.
//
// Revision 1.1.2.21  2003/10/03 02:50:11  wenger
// I *think* I finally got things to work on Windows -- with gzip, not
// winzip to uncompress.  Still needs lots of cleanup.
//
// Revision 1.1.2.20  2003/10/02 19:21:27  wenger
// More mods for Windows (again, not fully tested -- evolving towards
// something that works); this includes putting temporary files in
// a local tmp directory (*not* /tmp).
//
// Revision 1.1.2.19  2003/10/01 20:23:08  wenger
// Mods to get winzip uncompression working on Windows (not tested yet).
//
// Revision 1.1.2.18  2003/10/01 16:57:04  wenger
// Added the capability to test s2d without actually connecting to
// BMRB and PDB web sites; also added some Windows testing capability
// (not tested yet).
//
// Revision 1.1.2.17  2003/09/26 15:17:34  wenger
// Oops!  Forgot a few extra changes relating to changing the schema
// file names from 'bmrb:*' to 'bmrb-*'.
//
// Revision 1.1.2.16  2003/09/26 14:53:29  wenger
// New starlibj.jar that works with java 1.3.
//
// Revision 1.1.2.15  2003/09/19 17:54:53  wenger
// Changed cursor for simulated 2D spectrum so that only one amino acid
// is seleted by default; got rid of colon in schema file names (becuase
// of Windows).
//
// Revision 1.1.2.14  2003/09/18 14:49:42  wenger
// Added scripts to return peptide-cgi/star2devise version; better
// cache testing for visualization server mode; s2d fails if neither
// -bmrbid or -name is specified.
//
// Revision 1.1.2.13  2003/09/12 21:25:27  wenger
// Fixed bug 022 (visualization server incorrectly tries to process
// non-PDB files).
//
// Revision 1.1.2.12  2003/09/12 16:52:32  wenger
// Fixed bug 021 (processing PDB erases summary HTML file).
//
// Revision 1.1.2.11  2003/08/12 22:41:19  wenger
// S2d now sends email on errors (-nomail arg turns this off); fixed
// potential problem with s2d.out collision.
//
// Revision 1.1.2.10  2003/07/04 16:03:13  wenger
// Updated peptide-cgi to the latest version of starlibj (fixes problem
// on most PDB mmCIF files).
//
// Revision 1.1.2.9  2003/07/03 19:51:00  wenger
// Fixed error that prevented bmr5383.str from being processed correctly
// in visualization server mode; added visualization server test data.
//
// Revision 1.1.2.8  2003/07/02 21:49:51  wenger
// Fixed peptide-cgi to work with latest version of NMR-STAR files,
// which don't have percent signs in _Sequence_identity and
// _Sequence_positive values.
//
// Revision 1.1.2.7  2003/06/11 15:51:30  wenger
// BMRB entry number is now on the summary page.
//
// Revision 1.1.2.6  2003/05/21 20:54:43  wenger
// Moved label.dat out of dynamic_sessions directory; fixed error bars
// in session templates that use them.
//
// Revision 1.1.2.5  2003/05/14 23:35:34  wenger
// Data and session files can now be in non-standard directories.
//
// Revision 1.1.2.4  2003/05/14 21:45:52  wenger
// Changed "/"s in file names to File.separator (hope I got them all!).
//
// Revision 1.1.2.3  2003/05/14 20:10:47  wenger
// Split generated html and data files into separate directories (for
// better security); moved html and session templates out of main
// html and session directories.
//
// Revision 1.1.2.2  2003/05/08 16:40:50  wenger
// Got rid of all System.exit() calls.
//
// Revision 1.1.2.1  2003/04/22 21:58:14  wenger
// Added package name to peptide-cgi java code and put everything into
// a single jar file; version is now 6.0.
//
// Revision 1.34.6.9  2003/04/09 18:02:12  wenger
// First version of visualization-server capability now in place; still
// needs some fixes.
//
// Revision 1.34.6.8  2003/03/19 17:43:02  wenger
// Fixed bug 019 (problem with residue lists in the atomic coordinate
// sessions).
//
// Revision 1.34.6.7  2003/03/18 18:06:46  wenger
// Cleanup of SD2Main.process() in preparation for local file options
// for visualization server.
//
// Revision 1.34.6.6  2003/03/11 18:09:39  wenger
// Removed entry_information saveframe dependency from s2d.
//
// Revision 1.34.6.5  2003/03/06 20:39:15  wenger
// Residue lists in the 3D visualizations now correctly come from the
// monomeric_polymer save frame, as opposed to the coordinate data.
//
// Revision 1.34.6.4  2003/03/06 19:18:25  wenger
// Restored visible borders between views in the 3D visualizations, as
// per Eldon's request.
//
// Revision 1.34.6.3  2003/02/27 22:01:43  wenger
// Fixed one view in the 3D session template that should have been
// sending GData to the JS.
//
// Revision 1.34.6.2  2003/02/26 19:30:45  wenger
// New version of starlibj, and bonds code can now deal with single-letter
// amino acid names (fixes problems with PDB 1ahd, referenced from BMRB
// 4104).
//
// Revision 1.34.6.1  2003/01/14 16:51:48  wenger
// Merged V2_1b3_br_0 thru V2_1b3_br_end to V2_1b4_br.
//
// Revision 1.34.4.27  2003/01/10 16:32:45  wenger
// Changed the session-specific html files so that the use the "main"
// collaboration and keyboard help files; changed peptide-cgi version
// to 5.4.
//
// Revision 1.34.4.26  2002/12/03 22:16:48  wenger
// Should have changed the version before with session template fix.
//
// Revision 1.34.4.25  2002/11/07 20:54:57  wenger
// Peptide-CGI now re-uses cached atomic coordinate data as appropriate
// (to-do 018).
//
// Revision 1.34.4.24  2002/11/04 16:50:58  wenger
// Removed gnu-make-specific stuff from Makefile; did to-do 014 (shows
// multiple PDB structures); various to-do list updates.
//
// Revision 1.34.4.23  2002/11/02 00:08:34  wenger
// Added generation date to session files (prep. for re-using cached
// PDB data).
//
// Revision 1.34.4.22  2002/11/01 23:43:00  wenger
// Fixed bug 015 (data values not displayed correctly in 3D visualization
// because of data source definition problems).
//
// Revision 1.34.4.21  2002/11/01 17:40:14  wenger
// Fixed bug 013 (s2d fails if no write permission on output files).
//
// Revision 1.34.4.20  2002/10/31 22:17:34  wenger
// Changed Exception output to use toString() instead of getMessage()
// because that (at least sometimes) produces more complete information.
//
// Revision 1.34.4.19  2002/10/28 22:22:01  wenger
// Finished separating PDB entry processing from BMRB entry processing
// (the summary page is returned without getting any PDB entries; the
// PDB processing occurs when the user clicks the link in the summary
// page).
//
// Revision 1.34.4.18  2002/10/23 19:01:28  wenger
// PDB-only processing now works (I still need to set things up properly
// for the link in the summary page, though); S2DCompressedURL now reads
// more than one byte at a time; test_s2d works with new command-line
// arguments.
//
// Revision 1.34.4.17  2002/10/16 19:09:01  wenger
// Changed to new command-line arguments to allow specification of PDB
// ID (separate processing not yet implemented).
//
// Revision 1.34.4.16  2002/10/07 18:09:37  wenger
// Got peptide-cgi to work on a version 1.1 JVM.
//
// Revision 1.34.4.15  2002/09/11 20:04:30  wenger
// More improvements to 3D visualizations (added better help, changed
// some cursor constraints and automatic updates); removed special-case
// link to prototype 4096 3D visualization from summary HTML page.
//
// Revision 1.34.4.14  2002/08/22 14:17:20  wenger
// Data view X axis starts out with full available range.
//
// Revision 1.34.4.13  2002/08/21 21:33:00  wenger
// Added view and cursor to control X axis range of data view.
//
// Revision 1.34.4.12  2002/08/21 20:40:49  wenger
// Selection of data set to view now works, but there is not yet selection
// on the data linked to the 3D view.
//
// Revision 1.34.4.11  2002/08/21 15:59:38  wenger
// Changed all schemas to have Residue_seq_code as the attribute name
// we want to display for the sequence number, to make setting up the
// 4096-type 3D visualizations easier.
//
// Revision 1.34.4.10  2002/08/21 14:21:29  wenger
// Working on 4096-type 3D visualization -- added data and view to show
// list of available data sets.
//
// Revision 1.34.4.9  2002/08/19 21:37:08  wenger
// Updated 3D session template (added residue scroll cursor and second
// residue selection cursor).
//
// Revision 1.34.4.8  2002/08/13 16:07:44  wenger
// Some provisions for processing multiple PDB entries; currently disabled
// because of CGI script timeout problems.
//
// Revision 1.34.4.7  2002/08/08 18:50:54  wenger
// Did to-do item 012 (now getting PDB IDs from the monomeric_polymer
// save frame instead of the molecular_system save frame; checking the
// database name, sequence length, and sequence identity of PDB entries.
//
// Revision 1.34.4.6  2002/08/07 18:04:35  wenger
// Did to-do items 011 and 013 (get only first model from PDB file, show
// PDB ID on summary page).
//
// Revision 1.34.4.5  2002/08/06 21:45:37  wenger
// Provision for checking whether summary file out of date relative
// to PDB file(s) (not fully working yet).
//
// Revision 1.34.4.4  2002/08/06 17:22:59  wenger
// Peptide-cgi retries without getting atomic coordinates from PDB if
// it tried and failed from insufficient memory.
//
// Revision 1.34.4.3  2002/08/05 22:12:56  wenger
// Put a limit of 10,000 atoms on 3D structures (for now, at least),
// because too many atoms (e.g., BMRB 4081 with 64,752) crashes the
// JavaScreen.
//
// Revision 1.34.4.2  2002/08/05 21:23:22  wenger
// We only try to get the first set of atomic coordinates from the PDB;
// associated BMRB and PDB IDs are saved in the summary HTML file;
// increased s2d file size limit because some PDB files are really big
// once they're uncompressed; various other minor improvements.
//
// Revision 1.34.4.1  2002/07/19 18:38:11  wenger
// Merged V2_1b2_br_0 thru V2_1b2_br_end to V2_1b3_br.
//
// Revision 1.34.2.2  2002/07/01 20:07:36  wenger
// Getting coordinates from PDB works in stand-alone mode, but still has
// problems running as a CGI script; needs provisions to check up-to-
// dateness vs. PDB files, provision for various error conditions.
//
// Revision 1.34.2.1  2002/06/20 20:59:46  wenger
// Merged V2_1b1_br thru V2_1b1_br_end2 to V2_1b2_br.
//
// Revision 1.34  2002/06/17 19:41:40  wenger
// Merged V1_7b0_br_1 thru V1_7b0_br_2 to trunk.
//
// Revision 1.33.2.2  2002/06/05 18:56:08  wenger
// Improved summary page labels as per feedback from Eldon; fixed bug
// 006 (visual filters not set correctly when some sessions opened).
//
// Revision 1.33.2.1  2002/05/13 15:33:25  wenger
// Added "values" to the peptide-cgi summary page where appropriate.
//
// Revision 1.33.6.1  2002/05/01 22:26:45  wenger
// Merged V2_1b0_br thru V2_1b0_br_end to V2_1b1_br.
//
// Revision 1.33.4.4  2002/04/04 15:40:40  wenger
// The basic grabbing of atomic coordinates from mmCIF files at PDB is
// now working; still needs cleanup, more testing, better error handling,
// etc.
//
// Revision 1.33.4.3  2002/04/01 19:56:43  wenger
// S2DNmrStarIfc and S2DmmCifIfc are now fully subclasses of S2DStarIfc;
// split off S2DStarUtil into a totally separate class.
//
// Revision 1.33.4.2  2002/03/30 01:10:04  wenger
// First phase of making both S2DNmrStarIfc and S2DmmCifIfc subclasses
// of S2DStarIfc.
//
// Revision 1.33.4.1  2002/03/30 00:09:47  wenger
// Oops!  Forgot to commit S2DmmCifIfc stuff before (renamed from S2DCifIfc).
//
// Revision 1.33  2002/03/27 18:23:56  wenger
// Peptide-cgi now parses local mmCIF files related to NMR-Star files;
// added URL for PDB files to config stuff.
//
// Revision 1.32  2002/02/23 19:30:19  wenger
// Peptide-cgi now identifies related PDB entries.
//
// Revision 1.31  2002/01/15 22:16:13  wenger
// Added residue selection to atomic coordiate sessions.
//
// Revision 1.30  2002/01/10 22:16:57  wenger
// Forgot to update version with addition of structure type selection.
//
// Revision 1.29  2001/12/18 18:42:59  wenger
// Changed mouse location display in peptide-cgi session templates as
// requested by John Markley.
//
// Revision 1.28  2001/12/12 21:55:18  wenger
// Changed mouse location display formats as requested by John Markley.
//
// Revision 1.27  2001/12/12 19:56:42  wenger
// Got 4038/4096 star file combination working; fixed maximum bond length.
//
// Revision 1.26  2001/12/11 20:23:34  wenger
// First version of bond-visualization generation; added RCS Id string to
// schema files; manually added environment variables to data source
// definitions.
//
// Revision 1.25  2001/12/05 20:07:51  wenger
// Partially added provision for multiple-star-file entries; fixed minor
// problem in setup makefile.
//
// Revision 1.24  2001/12/03 19:30:21  wenger
// Added JavaScreen collaboration link to html pages generated by
// peptide-cgi.
//
// Revision 1.23  2001/10/10 22:37:33  wenger
// First version of atomic coordinate extraction and visualization (atoms,
// not bonds, only tested on 4096 -- coordinates must be in same NMR-STAR
// file as other data); fixed various minor problems with peptide-cgi code.
//
// Revision 1.22  2001/09/21 15:25:57  wenger
// Updated session templates to match DEVise font scaling fix; changed
// peptide-cgi version to 2.13.
//
// Revision 1.21  2001/07/25 21:32:53  wenger
// Various distribution-related cleanups and fixes; added a few missing
// help files.
//
// Revision 1.20  2001/07/25 19:56:23  wenger
// Started adding configuration setup stuff; s2d gets URLs from properties
// file.
//
// Revision 1.19  2001/07/25 17:46:49  wenger
// Added -usage and -version args to S2DMain, changed version to 2.12;
// slightly improved s2d error messages; install script now uses find
// to change the mode of html/data files to avoid argument overflow;
// distribution now generates tar file name with version number.
//
// Revision 1.18  2001/05/21 18:57:04  wenger
// Updated peptide-cgi code to match JavaScreen with package.
//
// Revision 1.17  2001/05/14 19:36:50  wenger
// We now still generate chemical shift visualizations if we can't get
// residue counts.
//
// Revision 1.16  2001/05/14 18:08:26  wenger
// Parameterized all star file tag names, etc.
//
// Revision 1.15  2001/05/08 18:24:18  wenger
// Fixed problem getting residue count if a star file contains info for
// more than one protein; added residue counts to 'all shifts' and 'H
// vs. N' visualizations.
//
// Revision 1.14  2001/04/30 17:45:18  wenger
// Added special link to 3D 4096 visualization to 4096 summary page;
// added "No chemical shift data available" message to appropriate
// summary pages.
//
// Revision 1.13  2001/04/24 18:06:43  wenger
// More improvements to "all chem shifts" visualization; found and fixed
// bug 666.
//
// Revision 1.12  2001/04/24 15:26:11  wenger
// Reversed axes in "all chem shifts" and "H vs. N chem shift"
// visualizations, and added atom type selection to "all chem shifts";
// updated star file list.
//
// Revision 1.11  2001/04/18 15:14:50  wenger
// Various improvements to "H vs. N" and "all chem shift" visualizations
// as requested yesterday at BMRB meeting; added string files to CVS and
// the install process.
//
// Revision 1.10  2001/04/17 17:09:09  wenger
// Added display of H vs. N chem shifts.
//
// Revision 1.9  2001/04/16 19:49:11  wenger
// Added display of all chem shifts by amino acid.
//
// Revision 1.8  2001/03/13 14:50:10  wenger
// Added cache invalidation: cache files are not used if peptide-cgi code
// or NMR-Star file has changed since relevant cache files were generated.
//
// Revision 1.7  2001/03/08 21:10:34  wenger
// Merged changes from no_collab_br_2 thru no_collab_br_3 from the branch
// to the trunk.
//
// Revision 1.6  2001/03/08 20:33:24  wenger
// Merged changes from no_collab_br_0 thru no_collab_br_2 from the branch
// to the trunk.
//
// Revision 1.5.2.2  2001/02/22 19:52:54  wenger
// Made most coupling constant values optional; added (optional) coupling
// constant error values.
//
// Revision 1.5.2.1  2001/02/09 16:57:12  wenger
// Added heteronuclear NOE; made T1 and T2 relaxation errors optional
// (conversion doesn't fail if they are not found); added an X margin of
// 0.5 in all sessions; updated star file list; misc. minor cleanups.
//
// Revision 1.5  2001/01/30 17:44:47  wenger
// Added path for set_modes.
//
// Revision 1.4  2001/01/25 16:37:46  wenger
// Fixed a bug that could cause an infinite loop in the perecent assignment
// calculations; put filesize, cpu, and coredump limits in s2d script;
// updated star file list; got rid of some unnecessary warnings.
//
// Revision 1.3  2001/01/23 19:35:19  wenger
// Made a few minor fixes to get things to work right at BMRB.
//
// Revision 1.2  2001/01/19 15:39:06  wenger
// Added T1 and T2 relaxation; removed some unnecessary variables from
// coupling constants; added schema files to installation, unified T1
// and T2 relaxation schema.
//
// Revision 1.1  2001/01/17 20:00:07  wenger
// Restructured the peptide-cgi code to make it much more maintainable.
//

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

    public static final String PEP_CGI_VERSION = "11.0.2";
    public static final String DEVISE_MIN_VERSION = "1.9.0";

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
    private String _name = null;
    private String _longName = null;
    
    private boolean _force = false;
    private boolean _useLocalFiles = false;
    private boolean _checkResList = false;

    private static final int PDB_LEVEL_NONE = 0;
    private static final int PDB_LEVEL_LINK = 1;
    private static final int PDB_LEVEL_PROCESS = 2;
    //TEMP -- _pdbLevel should really be something like _coordLevel
    private int _pdbLevel = PDB_LEVEL_LINK;

    private static final int CSR_LEVEL_NONE = 0;
    private static final int CSR_LEVEL_LINK = 1;
    private static final int CSR_LEVEL_PROCESS = 2;
    private int _csrLevel = CSR_LEVEL_NONE;

    // LACS-related info.
    private static final int LACS_LEVEL_NONE = 0;
    private static final int LACS_LEVEL_TRY = 1;
    private static final int LACS_LEVEL_MANDATORY = 2;
    private int _lacsLevel = LACS_LEVEL_NONE;
    private String _lacsFile = null;

    private boolean _haveCoords = false;

    private S2DSummaryHtml _summary;

    private static boolean _retrying;

    private String _cmdPdbId = null; // PDB ID from command line
    private String _currentPdbId = null; // PDB ID we're processing

    // PDB ID(s) for chem shift reference processing.
    private Vector _csrPdbIds = new Vector(); // Vector contains Strings.

    private int _cmdFrameIndex = -1;
    private int _currentFrameIndex = -1;

    private Vector _dataSets = new Vector();

    private boolean _doProteinCheck = true;

    private boolean _runScripts = true;

    private boolean _savedResList = false;

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
	    System.err.println("Top-level error: ");
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
	}

	try {
	    if (!s2d.useCache()) {
	        s2d.process();
	    } else {
	        if (DEBUG >= 1) {
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
        if (DEBUG >= 1) {
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
	getProperties();

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
    private void getProperties() throws S2DException
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

	S2DNames.STAR_NAME_TEMPLATE = props.getProperty(
	  "bmrb_mirror.star_name_template");
	if (S2DNames.STAR_NAME_TEMPLATE == null) {
	    if (DEBUG >= 1) {
	        System.out.println("bmrb_mirror.star_name_template " +
		  "property value not defined; using default");
	    }
	    S2DNames.STAR_NAME_TEMPLATE = "bmr*.str";
	}

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

	S2DNames.COMMENT_EMAIL = props.getProperty(
	  "bmrb_mirror.comment_email");
	if (S2DNames.CGI_URL == null) {
	    throw new S2DError("Unable to get value for " +
	      "bmrb_mirror.comment_email property");
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

	S2DNames.LACS_NAME_TEMPLATE = props.getProperty(
	  "bmrb_mirror.lacs_name_template");
	if (S2DNames.LACS_NAME_TEMPLATE == null) {
	    if (DEBUG >= 1) {
	        System.out.println("bmrb_mirror.lacs_name_template " +
		  "property value not defined; using default");
	    }
	    S2DNames.LACS_NAME_TEMPLATE = "bmr*_LACS.out";
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
    }

    //-------------------------------------------------------------------
    // Check arguments to constructor that need to be processed before
    // we try to get properties, set member variables accordingly.
    private void checkArgsNoProps(String args[]) throws S2DException
    {
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
	  "        show the minimum required DEVise version\n" +
          "    -do_csr <0|1|2>\n" +
          "        0: don't do chem shift reference calculations;\n" +
          "        1: create link in summary file but don't do the calculation;\n" +
          "        2: do chem shift reference calculations\n" +
          "        (default is " + _csrLevel + ")\n" +
          "    -do_lacs <0|1|2>\n" +
          "        0: don't attempt LACS processing\n" +
          "        1: attempt LACS processing, failure is not an error\n" +
          "        2: attempt LACS processing, failure is an error\n" +
          "        (default is " + _lacsLevel + " unless -lacs_file is set)\n" +
          "    -do_pdb <0|1|2>\n" +
          "        0: ignore PDB references;\n" +
          "        1: create links in summary file but don't process;\n" +
          "        2: process PDB references\n" +
          "        (default is " + _pdbLevel + " unless -pdbid is set)\n" +
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
          "    -pdbid <value>\n" +
          "        the PDB ID to process (e.g., 1dfv)\n" +
          "    -session_dir <directory>\n" +
          "        the directory in which to store the session files (mandatory)\n" +
          "    -usage\n" +
          "        show this message\n" +
          "    -version\n" +
          "        show the version number\n" +
          "\n" +
	  "Examples:\n" +
          "    java S2DMain -bmrbid 4101 ...\n" +
          "    java S2DMain -pdbid 1OXM -bmrbid 4101 -coord_index 2 ...\n" +
	  "    java S2DMain -file foo_data -name foo_out ...\n";

	int index = 0;
	while (index < args.length) {
	    if ("-dev_version".equals(args[index])) {
		System.out.println("Peptide-CGI version " + PEP_CGI_VERSION +
		  " requires DEVise version " + DEVISE_MIN_VERSION +
		  " or higher");
		throw new S2DCancel();

	    } else if ("-usage".equals(args[index])) {
	        System.out.println(usage);
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
        if (DEBUG >= 1) {
	    System.out.println("Arguments: ");
	    for (int index = 0; index < args.length; index++) {
	        System.out.println("  <" + args[index] + ">");
	    }
	}

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

	    } else if ("-pdbid".equals(args[index])) {
	        index++;
		if (index >= args.length) {
		    throw new S2DError("-pdbid argument needs value");
		}
	        _cmdPdbId = args[index];
		_pdbIds.addElement(_cmdPdbId);
		_csrPdbIds.addElement(_cmdPdbId);

	    } else if ("-session_dir".equals(args[index])) {
	        index++;
		if (index >= args.length) {
		    throw new S2DError("-session_dir argument needs value");
		}
	        _sessionDir = args[index];

	    } else {
	        throw new S2DError("Unrecognized command-line argument: " +
		  args[index]);
	    }
	    index++;
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

	if (_masterBmrbId.equals("") && _cmdPdbId != null) {
	    throw new S2DError(
	      "bmrbid must be specified when pdbid is specified");
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
	
	if (_name == null) {
	    throw new S2DError("name must be specified if bmrbid is not");
	}

	if (_pdbLevel < PDB_LEVEL_NONE || _pdbLevel > PDB_LEVEL_PROCESS) {
	    throw new S2DError("illegal do_pdb value (must be between " +
	      PDB_LEVEL_NONE + " and " + PDB_LEVEL_PROCESS + ")");
	}

	if (_csrLevel < CSR_LEVEL_NONE || _csrLevel > CSR_LEVEL_PROCESS) {
	    throw new S2DError("illegal do_csr value (must be between " +
	      CSR_LEVEL_NONE + " and " + CSR_LEVEL_PROCESS + ")");
	}

	if (_lacsLevel < LACS_LEVEL_NONE ||
	  _lacsLevel > LACS_LEVEL_MANDATORY) {
	    throw new S2DError("illegal do_lacs value (must be between " +
	      LACS_LEVEL_NONE + " and " + LACS_LEVEL_MANDATORY + ")");
	}
	
	//
	// Set some defaults.
	//
	if (_csrDataDir == null) {
	    _csrDataDir = _dataDir;
	}

        if (DEBUG >= 1) {
	    System.out.println("_name = <" + _name + ">");
	    System.out.println("_longName = <" + _longName + ">");
	    System.out.println("_masterBmrbId/BMRB ID = " + _masterBmrbId);
	    System.out.println("_cmdPdbId = " + _cmdPdbId);
	    System.out.println("_connectionFile = <" + _connectionFile + ">");
	    System.out.println("_cmdFrameIndex = " + _cmdFrameIndex);
	    System.out.println("_csrDataDir = <" + _csrDataDir + ">");
	    System.out.println("_csrTimeout = " + _csrTimeout);
	    System.out.println("_dataDir = <" + _dataDir + ">");
	    System.out.println("_csrLevel = " + _csrLevel);
	    System.out.println("_pdbLevel = " + _pdbLevel);
	    System.out.println("_lacsLevel = " + _lacsLevel);
	    System.out.println("_lacsFile = " + _lacsFile);
	    if (_localFiles.size() > 0) {
	        System.out.println("localFile = <" +
		  _localFiles.elementAt(0) + ".");
	    }
	    System.out.println("_force = " + _force);
	    System.out.println("_htmlDir = <" + _htmlDir + ">");
	    System.out.println("_useLocalFiles = " + _useLocalFiles);
	    System.out.println("_doProteinCheck = " + _doProteinCheck);
	    System.out.println("_runScripts = " + _runScripts);
	    System.out.println("_sessionDir = <" + _sessionDir + ">");
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
	if (DEBUG >= 1) {
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
		if (DEBUG >= 1) {
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
	            Date starModDate = S2DNmrStarIfc.getModDate(id, false);
	            if (starModDate == null ||
		      starModDate.after(summaryData.fileDate)) {
		        if (DEBUG >= 1) {
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
	    if (_pdbLevel == PDB_LEVEL_PROCESS) {
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
		    if (DEBUG >= 1) {
                        System.out.println("  Checking PDB ID " + id);
		    }
	            Date pdbModDate = S2DmmCifIfc.getModDate(id);
		        // Note: if a PDB ID was specified on the command
			// line we really should compare the PDB file date
			// against the session-specific html file here,
			// instead of the summary html file, but I'm
			// leaving it for now.  wenger 2004-11-03.
	            if (pdbModDate == null ||
		      pdbModDate.after(summaryData.fileDate)) {
		        if (DEBUG >= 1) {
		            System.out.println("Existing summary html file " +
			      "is older than PDB file; cache not used");
		        }
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
	    }

	    result = true;
	}

	if (DEBUG >= 1) {
	    System.out.println("S2DMain.useCache() returns " + result);
	}

	return result;
    }

    //-------------------------------------------------------------------
    // Determine whether the given session file allows us to use the
    // cache.  (Note that if *any* test shows the cache to be invalid,
    // we consider the cache invalid.)
    private boolean sessionFileUseCache(String name, String suffix,
      String sessionDir, S2DSummaryFileData summaryData)
    {
        //
	// Get peptide-cgi version and generation timestamp from
	// the appropriate session file.
	//
	S2DSessionFileData sessionData = new S2DSessionFileData();
	boolean sessionFileOk = sessionData.getSessionData(name, suffix,
	  sessionDir);
        if (!sessionFileOk) {
	    return false;
	}

	//
	// Compare session file version to current peptide-cgi software
	// version.
	//
	if (!PEP_CGI_VERSION.equals(sessionData.fileVersion)) {
	    if (DEBUG >= 1) {
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
	    if (DEBUG >= 1) {
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
        if (DEBUG >= 2) {
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
        if (DEBUG >= 2) {
            System.out.println("addPDB()");
        }

	for (int index = 0; index < ids.size(); index++) {
	    String id = (String)ids.elementAt(index);

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
	        String id = (String)_pdbIds.elementAt(index);
		_csrPdbIds.addElement(id);
	    }
	}
	
	if (DEBUG >= 1) {
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
        if (DEBUG >= 2) {
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
	    String tmpSummaryFile = _name + "tmp" + _cmdFrameIndex;
	    _summary = new S2DSummaryHtml(tmpSummaryFile, _longName,
	      _masterBmrbId, _htmlDir, "starFileName", "systemName",
	      "frameTitle");

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
		  _csrTimeout);
	        csr.run();
	        csr.postProcess(_doProteinCheck);
		if (!csr.ran() || csr.ranButFailed()) {
		    throw new S2DError(
		      "Chem shift reference calculation failed");
		}
	    }

	    _summary.close(null, null);

	    // Note: we should delete this "junk" summary file here, but
	    // File.delete() doesn't seem to work!  wenger 2003-09-12.

	    runSetModes();

	    return;
	}

        _currentFrameIndex = 1;

	//
	// Create the summary HTML file.
	//
	S2DNmrStarIfc masterStar = null;
	if (!_masterBmrbId.equals("")) {
            masterStar = S2DNmrStarIfc.createFromID(_masterBmrbId,
	      _useLocalFiles, false);

	    _summary = new S2DSummaryHtml(_name, _longName, _masterBmrbId,
	      _htmlDir, masterStar.getFileName(),
	      masterStar.getSystemName(), masterStar.getEntryTitle());
	} else {
	    _summary = new S2DSummaryHtml(_name, _longName, "99999", _htmlDir,
	      _name, ""/*TEMP?*/, ""/*TEMP?*/);
	}

	//
	// Initiate chemical shift reference processing.
	//
	S2DChemShiftRef csr = null;
	if (_csrPdbIds.size() != 0 && _csrLevel == CSR_LEVEL_PROCESS) {
	    csr = new S2DChemShiftRef(_name, _longName, _dataDir, _csrDataDir,
	      _sessionDir, _masterBmrbId, _localFiles,
	      (String)_csrPdbIds.elementAt(0), _summary, 1, _csrTimeout);
	    csr.run();
	}

	//
	// Process all BMRB entries.
	//

	// Note: star seems to hold the *last* NMR-STAR file we process,
	// which we need for processing PDB entries later on.
        S2DStarIfc star = null;

	if (!_masterBmrbId.equals("")) {
	    star = processAllBMRBs(masterStar);
	}

	//
	// Process all local files.
	//
	if (_localFiles.size() > 0) {
	    star = processLocalFiles();
	}

	//
	// Initiate chemical shift reference processing, if we didn't
	// before and got a PDB ID from the NMR-STAR file.
	//
	if (csr == null && _csrPdbIds.size() != 0 &&
	  _csrLevel == CSR_LEVEL_PROCESS) {
	    csr = new S2DChemShiftRef(_name, _longName, _dataDir, _csrDataDir,
	      _sessionDir, _masterBmrbId, _localFiles,
	      (String)_csrPdbIds.elementAt(0), _summary, 1, _csrTimeout);
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
	              csrPdb, _summary, csrIndex + 1, _csrTimeout);
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

	_summary.close(_bmrbIds, _pdbIds);
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
                star = S2DNmrStarIfc.createFromID(bmrbId, _useLocalFiles,
		  false);
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
	    star = S2DNmrStarIfc.createFromFile(
	      (String)_localFiles.elementAt(index), false);

	    process1NmrStar(star);
	}

	return star;
    }

    //-------------------------------------------------------------------
    // Process all specified PDB entries.
    private void processAllPDBs(S2DStarIfc star)
    {
        if (DEBUG >= 1) {
	    System.out.println("processAllPDBs()");
	}

	for (int index = 0; index < _pdbIds.size(); index++) {
	    String id = (String)_pdbIds.elementAt(index);
	    if (_pdbLevel == PDB_LEVEL_LINK ||
	      _pdbLevel == PDB_LEVEL_PROCESS) {
		try {
	            process1PDB(star, id);
		} catch(S2DException ex) {
		    System.err.println("Unable to process PDB ID " + id);
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
	}
    }

    //-------------------------------------------------------------------
    // Process the LACS file (if any).
    private void processLACS() throws S2DException
    {
        if (DEBUG >= 1) {
	    System.out.println("processLACS()");
	}

	if (_lacsLevel >= LACS_LEVEL_TRY) {
	    try {
	        S2DNmrStarIfc lacsStar;
	        if (_lacsFile != null) {
	            lacsStar = S2DNmrStarIfc.createFromFile(_lacsFile, true);
	        } else {
	            lacsStar = S2DNmrStarIfc.createFromID(_masterBmrbId,
		      _useLocalFiles, true);
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
		  "set_modes " + _name);
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
    // Process a given NMR-STAR file entry.
    private void process1NmrStar(S2DNmrStarIfc star) throws S2DException
    {
        Vector ids = star.getPdbIds(_doProteinCheck);
        addPDB(ids);

        //TEMP -- do I really want to skip stuff if I get an error?

        saveChemShifts(star);
        saveT1Relaxation(star);
        saveT2Relaxation(star);
        saveHetNOE(star);
        saveHExchangeRate(star);
        saveCoupling(star);
        saveHExchangeProtFact(star);
        saveS2Params(star);

        save3DDataSources();
        saveAtomicCoords(star);

	try {
	    ensureResidueList(star);
	} catch (S2DException ex) {
	    System.err.println("Exception in ensureResidueList(): " + ex);
	}
    }

    //-------------------------------------------------------------------
    // Save all chem shift data for this entry.
    // Note: this can be tested with 4264.
    private void saveChemShifts(S2DStarIfc star) throws S2DException
    {
        if (DEBUG >= 2) {
	    System.out.println("  S2DMain.saveChemShifts()");
	}

	Enumeration frameList = star.getDataFramesByCat(
	  star.CHEM_SHIFT_SF_CAT, star.ASSIGNED_CHEM_SHIFTS);

	int frameIndex = 1;
        while (frameList.hasMoreElements()) {
	    SaveFrameNode frame = (SaveFrameNode)frameList.nextElement();

	    Vector entityIDList = star.getChemShiftEntityIDs(frame);
	    for (int index = 0; index < entityIDList.size(); index++) {
	        String entityID = (String)entityIDList.get(index);

	        // We only want to output chemical shifts corresponding to a
	        // protein.  (This can be tested with bmr4038.str and bmr4056.str.)
	        if (_doProteinCheck && !star.refersToProtein(frame, entityID)) {
                    if (DEBUG >= 1) {
                        System.out.println("Chemical shifts not saved for " +
                          "save frame " + frame.getLabel() + " (" + entityID +
			  ") because it is not a protein");
                    }
	        } else {
		    //TEMP -- do I really want to catch the error here (and in
		    // other similar methods below)?
		    try {
	                S2DResidues residues = saveFrameResCounts(star, frame,
		          entityID, frameIndex);
	                saveFrameChemShifts(star, frame, entityID, frameIndex);
	                saveFramePistachio(star, frame, entityID, residues,
		          frameIndex);
	                saveFrameAmbiguity(star, frame, entityID, residues,
		          frameIndex);
		    } catch(S2DException ex) {
		        System.err.println("Exception saving chem shifts for " +
		          "frame " + star.getFrameName(frame) + " (" + entityID +
			  "): " + ex.toString());
		    }
		    frameIndex++;
	        }
	    }
	}
    }

    //-------------------------------------------------------------------
    // Ensure that we've saved a residue list if at all possible.  In
    // most cases, a residue list will already have been saved when
    // this method is called, but if not we try to save one now.
    private void ensureResidueList(S2DStarIfc star) throws S2DException
    {
        if (DEBUG >= 2) {
	    System.out.println("  S2DMain.ensureResidueList()");
	}

	if (!_savedResList) {
	    //TEMP -- this fails if there is more than one entity
            saveResList (star, null, 1);
	}
    }

    //-------------------------------------------------------------------
    // Save all T1 relaxation data for this entry.
    // Note: this can be tested with 4267.
    private void saveT1Relaxation(S2DStarIfc star)
    {
        if (DEBUG >= 2) {
	    System.out.println("  S2DMain.saveT1Relaxation()");
	}

	Enumeration frameList = star.getDataFramesByCat(star.T1_RELAX_SF_CAT,
	  star.T1_RELAX);

	int frameIndex = 1;
        while (frameList.hasMoreElements()) {
	    SaveFrameNode frame = (SaveFrameNode)frameList.nextElement();
	    try {
	        saveFrameT1Relax(star, frame, frameIndex);
	    } catch(S2DException ex) {
		System.err.println("Exception saving T1 relaxation for " +
		  "frame " + star.getFrameName(frame) + ": " +
		  ex.toString());
	    }
	    frameIndex++;
        }
    }

    //-------------------------------------------------------------------
    // Save all T2 relaxation data for this entry.
    // Note: this can be tested with 4267.
    private void saveT2Relaxation(S2DStarIfc star)
    {
        if (DEBUG >= 2) {
	    System.out.println("  S2DMain.saveT2Relaxation()");
	}

	Enumeration frameList = star.getDataFramesByCat(star.T2_RELAX_SF_CAT,
	  star.T2_RELAX);

	int frameIndex = 1;
        while (frameList.hasMoreElements()) {
	    SaveFrameNode frame = (SaveFrameNode)frameList.nextElement();
	    try {
	        saveFrameT2Relax(star, frame, frameIndex);
	    } catch(S2DException ex) {
		System.err.println("Exception saving T2 relaxation for " +
		  "frame " + star.getFrameName(frame) + ": " +
		  ex.toString());
	    }
	    frameIndex++;
        }
    }

    //-------------------------------------------------------------------
    // Save all heteronuclear NOE values for this entry.
    // Note: this can be tested with 4267.
    private void saveHetNOE(S2DStarIfc star)
    {
        if (DEBUG >= 2) {
	    System.out.println("  S2DMain.saveHetNOE()");
	}

	Enumeration frameList = star.getDataFramesByCat(star.HET_NOE_SF_CAT,
	  star.HETERONUCLEAR_NOE);

	int frameIndex = 1;
        while (frameList.hasMoreElements()) {
	    SaveFrameNode frame = (SaveFrameNode)frameList.nextElement();
	    try {
	        saveFrameHetNOE(star, frame, frameIndex);
	    } catch(S2DException ex) {
		System.err.println("Exception saving heteronuclear NOE for " +
		  "frame " + star.getFrameName(frame) + ": " +
		  ex.toString());
	    }
	    frameIndex++;
        }
    }

    //-------------------------------------------------------------------
    private void saveHExchangeRate(S2DStarIfc star)
    {
        if (DEBUG >= 2) {
	    System.out.println("  S2DMain.saveHExchangeRate()");
	}

	// add real code here
    }

    //-------------------------------------------------------------------
    // Save all coupling constants for this entry.
    // Note: this can be tested with 4297.
    private void saveCoupling(S2DStarIfc star)
    {
        if (DEBUG >= 2) {
	    System.out.println("  S2DMain.saveCoupling()");
	}

	Enumeration frameList = star.getDataFramesByCat(star.COUPLING_SF_CAT,
	  star.COUPLING_CONSTANTS);

	int frameIndex = 1;
        while (frameList.hasMoreElements()) {
	    SaveFrameNode frame = (SaveFrameNode)frameList.nextElement();
	    try {
	        saveFrameCoupling(star, frame, frameIndex);
	    } catch(S2DException ex) {
		System.err.println("Exception saving coupling constants for " +
		  "frame " + star.getFrameName(frame) + ": " +
		  ex.toString());
	    }
	    frameIndex++;
        }
    }

    //-------------------------------------------------------------------
    private void saveHExchangeProtFact(S2DStarIfc star)
    {
        if (DEBUG >= 2) {
	    System.out.println("  S2DMain.saveHExchangeProtFact()");
	}

	// add real code here
    }

    //-------------------------------------------------------------------
    private void saveS2Params(S2DStarIfc star)
    {
        if (DEBUG >= 2) {
	    System.out.println("  S2DMain.saveS2Params()");
	}

	// add real code here
    }

    //-------------------------------------------------------------------
    // Write the data source definitions needed for 3D data.
    private void save3DDataSources() throws S2DException
    {
        if (DEBUG >= 2) {
	    System.out.println("  S2DMain.save3DDataSources()");
	}

	try {
	    Writer writer = S2DFileWriter.create(_sessionDir +
	      File.separator + _name + S2DNames.ATOMIC_COORD_DATA_DEF);
	    S2DSession.write3DDataSources(writer, _dataSets);
	    writer.close();
	} catch (IOException ex) {
	    throw new S2DError("Unable to save 3D data sources: " +
	      ex.toString());
	}
    }

    //-------------------------------------------------------------------
    private void saveAtomicCoords(S2DStarIfc star)
    {
        if (DEBUG >= 2) {
	    System.out.println("  S2DMain.saveAtomicCoords()");
	}

	Enumeration frameList = star.getDataFramesByCat(
	  star.ATOM_COORD_SF_CAT, star.ATOM_COORD_SAVE_FRAME);

        while (frameList.hasMoreElements()) {
	    SaveFrameNode frame = (SaveFrameNode)frameList.nextElement();
	    try {
	        saveFrameAtomicCoords(star, frame, _currentFrameIndex, null,
		  false);

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
    private void saveLACS(S2DStarIfc star) throws S2DException
    {
        if (DEBUG >= 2) {
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
    private S2DResidues saveFrameResCounts(S2DStarIfc star,
      SaveFrameNode chemShiftFrame, String entityID, int frameIndex)
      throws S2DException
    {
        if (DEBUG >= 3) {
	    System.out.println("    S2DMain.saveFrameResCounts(" +
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
	        entityIDs = star.getFrameValues(chemShiftFrame,
		  star.CHEM_SHIFT_VALUE, star.CHEM_SHIFT_ENTITY_ID);
	    }

	    String[] resSeqCodesTmp = star.getFrameValues(chemShiftFrame,
	      star.CHEM_SHIFT_VALUE, star.CHEM_SHIFT_RES_SEQ_CODE);
	    if (!entityID.equals("")) {
	      resSeqCodesTmp = S2DUtils.selectMatches(entityIDs,
	        resSeqCodesTmp, entityID);
	    }
	    int[] resSeqCodes = S2DUtils.arrayStr2Int(resSeqCodesTmp);
	    resSeqCodesTmp = null;

	    String[] residueLabels = star.getFrameValues(chemShiftFrame,
	      star.CHEM_SHIFT_VALUE, star.CHEM_SHIFT_RES_LABEL);
	    if (!entityID.equals("")) {
	      residueLabels = S2DUtils.selectMatches(entityIDs,
	        residueLabels, entityID);
	    }

            for (int index = 0; index < resSeqCodes.length; ++index) {
	        int resNum = resSeqCodes[index];
	        if (!residueLabels[index].equals(
		  residues.getResLabel(resNum))) {
		    S2DError err = new S2DError("Residue mismatch at residue " +
		      resNum + ": " + residues.getResLabel(resNum) +
		      " in residue list vs. " + residueLabels[index] +
		      " in chem shift loop");
		    System.err.println(err);
		    System.exit(1);
	        }
	    }

	}

        return residues;
    }

    //-------------------------------------------------------------------
    // Save the residue count and residue list for the given save frame.
    // (The save frame passed in here must be the save frame that
    // actually contains the residue lists.)
    private S2DResidues saveResList(S2DStarIfc star,
      SaveFrameNode monoPolyFrame, int frameIndex) throws S2DException
    {
        if (DEBUG >= 3) {
	    System.out.println("    S2DMain.saveResList(" +
	      star.getFrameName(monoPolyFrame) + ", " + frameIndex + ")");
	}

        if (monoPolyFrame == null) {
	    monoPolyFrame = star.getEntityFrame();
        }

        S2DResidues residues = star.getResidues(monoPolyFrame);

        S2DResCount resCount = new S2DResCount(_name, _dataDir,
	  residues._resSeqCodes, residues._resLabels);

	resCount.write(frameIndex);

        S2DResList resList = new S2DResList(_name, _dataDir,
	  residues._resSeqCodes, residues._resLabels);

	resList.write(frameIndex);

	_savedResList = true;

	return residues;
    }

    //-------------------------------------------------------------------
    // Save chem shifts for one save frame.
    private void saveFrameChemShifts(S2DStarIfc star, SaveFrameNode frame,
      String entityID, int frameIndex) throws S2DException
    {
        if (DEBUG >= 3) {
	    System.out.println("    S2DMain.saveFrameChemShifts(" +
	      star.getFrameName(frame) + " (" + entityID + "), " +
	      frameIndex + ")");
	}

	//
	// Get the values we need from the Star file.
	//
	String[] entityIDs = null;
	if (!entityID.equals("")) {
	    entityIDs = star.getFrameValues(frame, star.CHEM_SHIFT_VALUE,
	      star.CHEM_SHIFT_ENTITY_ID);
	}

	String[] resSeqCodesTmp = star.getFrameValues(frame,
	  star.CHEM_SHIFT_VALUE, star.CHEM_SHIFT_RES_SEQ_CODE);
	if (!entityID.equals("")) {
	  resSeqCodesTmp = S2DUtils.selectMatches(entityIDs,
	    resSeqCodesTmp, entityID);
	}
	int[] resSeqCodes = S2DUtils.arrayStr2Int(resSeqCodesTmp);
	resSeqCodesTmp = null;

	String[] residueLabels = star.getFrameValues(frame,
	  star.CHEM_SHIFT_VALUE, star.CHEM_SHIFT_RES_LABEL);
	if (!entityID.equals("")) {
	  residueLabels = S2DUtils.selectMatches(entityIDs,
	    residueLabels, entityID);
	}

	String[] atomNames = star.getFrameValues(frame,
	  star.CHEM_SHIFT_VALUE, star.CHEM_SHIFT_ATOM_NAME);
	if (!entityID.equals("")) {
	  atomNames = S2DUtils.selectMatches(entityIDs, atomNames,
	    entityID);
	}

	String[] atomTypes = star.getFrameValues(frame,
	  star.CHEM_SHIFT_VALUE, star.CHEM_SHIFT_ATOM_TYPE);
	if (!entityID.equals("")) {
	  atomTypes = S2DUtils.selectMatches(entityIDs, atomTypes,
	    entityID);
	}

	String[] chemShiftsTmp = star.getFrameValues(frame,
	  star.CHEM_SHIFT_VALUE, star.CHEM_SHIFT_VALUE);
	if (!entityID.equals("")) {
	  chemShiftsTmp = S2DUtils.selectMatches(entityIDs,
	    chemShiftsTmp, entityID);
	}
        double[] chemShiftVals = S2DUtils.arrayStr2Double(chemShiftsTmp);
	chemShiftsTmp = null;

	int[] ambiguityVals;
	try {
	    String[] ambiguityTmp = star.getFrameValues(frame,
	      star.CHEM_SHIFT_VALUE, star.CHEM_SHIFT_AMBIG_CODE);

	    if (!entityID.equals("")) {
	      ambiguityTmp = S2DUtils.selectMatches(entityIDs,
	        ambiguityTmp, entityID);
	    }
	    ambiguityVals = S2DUtils.arrayStr2Int(ambiguityTmp);
	    ambiguityTmp = null;
	} catch (S2DException ex) {
            if (DEBUG >= 3) {
	        System.out.println("No ambiguity values in this save frame (" +
		  star.getFrameName(frame) + ")");
	    }
	    ambiguityVals = new int[entityIDs.length];
	    for (int index = 0; index < ambiguityVals.length; ++index) {
	    	ambiguityVals[index] = 9;
	    }
	}

	//
	// Create an S2DChemShift object with the values we just got.
	//
        S2DChemShift chemShift = new S2DChemShift(_name, _longName, _dataDir,
	  _sessionDir, _summary, resSeqCodes, residueLabels, atomNames,
	  atomTypes, chemShiftVals, ambiguityVals);


	//
	// Now go ahead and calculate and write out the chem shift values.
	//
	_summary.startFrame(star.getFrameDetails(frame));

	try {
	    chemShift.writeDeltashifts(frameIndex);
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
	if (_doProteinCheck) {
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
	    if (DEBUG >= 1) {
	        System.out.println("CSI values not saved for " +
		  "save frame " + frame.getLabel() + " because HA " +
		    "chem shifts are not available for 80% of the residues");
            }
	}

	try {
	    chemShift.writePctAssign(frameIndex);
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

	_summary.endFrame();
    }

    //-------------------------------------------------------------------
    // Save T1 relaxation values for one save frame.
    private void saveFrameT1Relax(S2DStarIfc star, SaveFrameNode frame,
      int frameIndex) throws S2DException
    {
        if (DEBUG >= 3) {
	    System.out.println("    S2DMain.saveFrameT1Relax(" +
	      star.getFrameName(frame) + ", " + frameIndex + ")");
	}

	//
	// Get the values we need from the Star file.
	//
	String[] resSeqCodes = star.getFrameValues(frame,
	  star.T1_VALUE, star.T1_RES_SEQ_CODE);

	String[] resLabels = star.getFrameValues(frame,
	  star.T1_VALUE, star.T1_RES_LABEL);

	String[] atomNames = star.getFrameValues(frame,
	  star.T1_VALUE, star.T1_ATOM_NAME);

	String[] relaxValues = star.getFrameValues(frame,
	  star.T1_VALUE, star.T1_VALUE);

	//TEMP -- 4096 has "_T1_error" instead of "_T1_value_error".
	String[] relaxErrors = star.getOptionalFrameValues(frame,
	  star.T1_VALUE, star.T1_VALUE_ERR, relaxValues.length, "0");

	//
	// Create an S2DRelaxation object with the values we just got.
	//
	S2DRelaxation relaxation = new S2DRelaxation(_name, _longName,
	  _dataDir, _sessionDir, _summary, S2DUtils.TYPE_T1_RELAX,
	  star.getOneFrameValue(frame, star.T1_SPEC_FREQ_1H),
	  resSeqCodes, resLabels, atomNames, relaxValues, relaxErrors);

	//
	// Now go ahead and calculate and write out the relaxation values.
	//
	_summary.startFrame(star.getFrameDetails(frame));

	try {
	    relaxation.writeRelaxation(frameIndex);
	    relaxation.addRelaxationData(_dataSets, frameIndex);
	} finally {
	    _summary.endFrame();
	}
    }

    //-------------------------------------------------------------------
    // Save T2 relaxation values for one save frame.
    private void saveFrameT2Relax(S2DStarIfc star, SaveFrameNode frame,
      int frameIndex) throws S2DException
    {
        if (DEBUG >= 3) {
	    System.out.println("    S2DMain.saveFrameT2Relax(" +
	      star.getFrameName(frame) + ", " + frameIndex + ")");
	}

	//
	// Get the values we need from the Star file.
	//
	String[] resSeqCodes = star.getFrameValues(frame,
	  star.T2_VALUE, star.T2_RES_SEQ_CODE);

	String[] resLabels = star.getFrameValues(frame,
	  star.T2_VALUE, star.T2_RES_LABEL);

	String[] atomNames = star.getFrameValues(frame,
	  star.T2_VALUE, star.T2_ATOM_NAME);

	String[] relaxValues = star.getFrameValues(frame,
	  star.T2_VALUE, star.T2_VALUE);

	//TEMP -- 4096 has "_T2_error" instead of "_T2_value_error".
	String[] relaxErrors = star.getOptionalFrameValues(frame,
	  star.T2_VALUE, star.T2_VALUE_ERR, relaxValues.length, "0");

	//
	// Create an S2DRelaxation object with the values we just got.
	//
	S2DRelaxation relaxation = new S2DRelaxation(_name, _longName,
	  _dataDir, _sessionDir, _summary, S2DUtils.TYPE_T2_RELAX,
	  star.getOneFrameValue(frame, star.T2_SPEC_FREQ_1H),
	  resSeqCodes, resLabels, atomNames, relaxValues, relaxErrors);

	//
	// Now go ahead and calculate and write out the relaxation values.
	//
	_summary.startFrame(star.getFrameDetails(frame));

	try {
	    relaxation.writeRelaxation(frameIndex);
	    relaxation.addRelaxationData(_dataSets, frameIndex);
	} finally {
	    _summary.endFrame();
	}
    }

    //-------------------------------------------------------------------
    // Save coupling constants for one save frame.
    private void saveFrameCoupling(S2DStarIfc star, SaveFrameNode frame,
      int frameIndex) throws S2DException
    {
        if (DEBUG >= 3) {
	    System.out.println("    S2DMain.saveFrameCoupling(" +
	      star.getFrameName(frame) + ", " + frameIndex + ")");
	}

	//
	// Get the values we need from the Star file.
	//
	String[] atom1ResSeqs = star.getFrameValues(frame,
	  star.COUPLING_CONSTANT_VALUE, star.COUPLING_RES_SEQ_CODE_1);

	String[] atom2ResSeqs = star.getFrameValues(frame,
	  star.COUPLING_CONSTANT_VALUE, star.COUPLING_RES_SEQ_CODE_2);

	String[] couplingConstValues = star.getFrameValues(frame,
	  star.COUPLING_CONSTANT_VALUE, star.COUPLING_CONSTANT_VALUE);

	String[] couplingConstCodes = star.getOptionalFrameValues(frame,
	  star.COUPLING_CONSTANT_VALUE, star.COUPLING_CONSTANT_CODE,
	  atom1ResSeqs.length, "0");

	String[] atom1ResLabels = star.getOptionalFrameValues(frame,
	  star.COUPLING_CONSTANT_VALUE, star.COUPLING_RES_LABEL_1,
	  atom1ResSeqs.length, "0");

	String[] atom1Names = star.getOptionalFrameValues(frame,
	  star.COUPLING_CONSTANT_VALUE, star.COUPLING_ATOM_NAME_1,
	  atom1ResSeqs.length, "0");

	String[] atom2ResLabels = star.getOptionalFrameValues(frame,
	  star.COUPLING_CONSTANT_VALUE, star.COUPLING_RES_LABEL_2,
	  atom1ResSeqs.length, "0");

	String[] atom2Names = star.getOptionalFrameValues(frame,
	  star.COUPLING_CONSTANT_VALUE, star.COUPLING_ATOM_NAME_2,
	  atom1ResSeqs.length, "0");

	String[] couplingConstErrors = star.getOptionalFrameValues(frame,
	  star.COUPLING_CONSTANT_VALUE,
	  star.COUPLING_CONSTANT_VALUE_ERR, atom1ResSeqs.length, "0");

	//
	// Create an S2DCoupling object with the values we just got.
	//
        S2DCoupling coupling = new S2DCoupling(_name, _longName, _dataDir,
	  _sessionDir, _summary, couplingConstCodes, atom1ResSeqs,
	  atom1ResLabels, atom1Names, atom2ResSeqs, atom2ResLabels,
	  atom2Names, couplingConstValues, couplingConstErrors);

	//
	// Now go ahead and calculate and write out the coupling contants.
	//
	_summary.startFrame(star.getFrameDetails(frame));

	try {
	    coupling.writeCoupling(frameIndex);
	    coupling.addCouplingData(_dataSets, frameIndex);
	} finally {
	    _summary.endFrame();
	}
    }

    //-------------------------------------------------------------------
    // Save heteronuclear NOE values for one save frame.
    private void saveFrameHetNOE(S2DStarIfc star, SaveFrameNode frame,
      int frameIndex) throws S2DException
    {
        if (DEBUG >= 3) {
	    System.out.println("    S2DMain.saveFrameHetNOE(" +
	      star.getFrameName(frame) + ", " + frameIndex + ")");
	}

	//
	// Get the values we need from the Star file.
	// TEMP -- NMR-STAR 3.0 has two residues for each heternuclear
	// NOE, so we should probably deal with that eventually.
	//
//TEMPTEMP -- convert these to numerical values?
	String[] resSeqCodes = star.getFrameValues(frame,
	  star.HET_NOE_VALUE, star.HET_NOE_RES_SEQ_CODE);

	String[] resLabels = star.getFrameValues(frame,
	  star.HET_NOE_VALUE, star.HET_NOE_RES_LABEL);

	String[] hetNOEValues = star.getFrameValues(frame,
	  star.HET_NOE_VALUE, star.HET_NOE_VALUE);

	String[] hetNOEErrors = star.getFrameValues(frame,
	  star.HET_NOE_VALUE, star.HET_NOE_VALUE_ERR);

	//
	// Create an S2DHetNOE object with the values we just got.
	//
	// are dealt with really differently in 2.1 and 3.0
	S2DHetNOE hetNOE = new S2DHetNOE(_name, _longName, _dataDir,
	  _sessionDir, _summary,
	  star.getOneFrameValue(frame, star.HET_NOE_SPEC_FREQ_1H),
	  star.getHetNOEAtom1(frame), star.getHetNOEAtom2(frame),
	  resSeqCodes, resLabels, hetNOEValues, hetNOEErrors);

	//
	// Now go ahead and calculate and write out the heteronuclear NOE
	// values.
	//
	_summary.startFrame(star.getFrameDetails(frame));

	try {
	    hetNOE.writeHetNOE(frameIndex);
	    hetNOE.addHetNOEData(_dataSets, frameIndex);
	} finally {
	    _summary.endFrame();
	}
    }

    //-------------------------------------------------------------------
    // Save atomic coordinates for one save frame.
    private void saveFrameAtomicCoords(S2DStarIfc star, SaveFrameNode frame,
      int frameIndex, S2DAtomDataTable pt, boolean for2DView)
      throws S2DException
    {
        if (DEBUG >= 3) {
	    System.out.println("    S2DMain.saveFrameAtomicCoords(" +
	      star.getFrameName(frame) + ", " + frameIndex + ")");
	}

	//
	// If this is a PDB (mmCIF) file, figure out which coordinates
	// correspond to the first model, and only get that data.
	//
	int model1AtomCount = -1;
	if (!star.ATOM_COORD_MODEL_NUM.equals("")) {
	    String[] modelNums = star.getFrameValues(frame,
	      star.ATOM_COORD_X, star.ATOM_COORD_MODEL_NUM);
	    for (int index = 0; index < modelNums.length; index++) {
	        if (!modelNums[index].equals("1")) {
		    model1AtomCount = index;
		    break;
		}
	    }

	    if (DEBUG >= 1) {
	        System.out.println("Model 1 atom count = " + model1AtomCount);
	    }
	}

	//
	// Get the values we need from the Star file.
	//
	String[] resSeqCodes = star.getFrameValues(frame,
	  star.ATOM_COORD_X, star.ATOM_COORD_RES_SEQ_CODE, model1AtomCount);

	String[] resLabels = star.getFrameValues(frame,
	  star.ATOM_COORD_X, star.ATOM_COORD_RES_LABEL, model1AtomCount);

	String[] atomNames = star.getFrameValues(frame,
	  star.ATOM_COORD_X, star.ATOM_COORD_ATOM_NAME, model1AtomCount);

        atomNames = star.translateAtomNomenclature(resLabels, atomNames);

	String[] atomTypes = star.getFrameValues(frame,
	  star.ATOM_COORD_X, star.ATOM_COORD_ATOM_TYPE, model1AtomCount);

	String[] atomCoordXTmp = star.getFrameValues(frame,
	  star.ATOM_COORD_X, star.ATOM_COORD_X, model1AtomCount);
        double[] atomCoordX = S2DUtils.arrayStr2Double(atomCoordXTmp);
        atomCoordXTmp = null;

	String[] atomCoordYTmp = star.getFrameValues(frame,
	  star.ATOM_COORD_Y, star.ATOM_COORD_Y, model1AtomCount);
        double[] atomCoordY = S2DUtils.arrayStr2Double(atomCoordYTmp);
        atomCoordYTmp = null;

	String[] atomCoordZTmp = star.getFrameValues(frame,
	  star.ATOM_COORD_Z, star.ATOM_COORD_Z, model1AtomCount);
        double[] atomCoordZ = S2DUtils.arrayStr2Double(atomCoordZTmp);
        atomCoordZTmp = null;

	//
	// Create an S2DAtomicCoords object with the values we just got.
	//
	S2DAtomicCoords atomicCoords = new S2DAtomicCoords(_name,
	  _longName, _dataDir, _sessionDir, _summary, resSeqCodes,
	  resLabels, atomNames, atomTypes, atomCoordX, atomCoordY,
	  atomCoordZ, _connectionFile, _dataSets, _currentPdbId);

	//
	// Now go ahead and calculate and write out the atomic coordinates.
	//
	if (pt == null) {
	    AtomicCoordSummaryPre(star, frame);
	}

	try {
	    if (for2DView) {
	        atomicCoords.writeBonds(frameIndex, pt, for2DView);
	    } else {
	        atomicCoords.writeAtoms(frameIndex);
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
    private void saveFramePistachio(S2DStarIfc star, SaveFrameNode frame,
      String entityID, S2DResidues residues, int frameIndex) throws S2DException
    {
        if (DEBUG >= 3) {
	    System.out.println("    S2DMain.saveFramePistachio(" +
	      star.getFrameName(frame) + " (" + entityID + "), " +
	      frameIndex + ")");
	}

	//
	// Get the values we need from the Star file.
	//
	if (star.FIGURE_OF_MERIT.equals("")) {
	    // File is not NMR-STAR 3.0
	    return;
	}

//TEMPTEMP -- change CHEM_SHIFT_VALUE to FIGURE_OF_MERIT in all here
	String[] entityIDs = null;
	if (!entityID.equals("")) {
	    entityIDs = star.getFrameValues(frame, star.CHEM_SHIFT_VALUE,
	      star.CHEM_SHIFT_ENTITY_ID);
	}

	String[] meritValsTmp;
	try {
	    meritValsTmp = star.getFrameValues(frame,
	      star.CHEM_SHIFT_VALUE, star.FIGURE_OF_MERIT);
	} catch (S2DException ex) {
            if (DEBUG >= 3) {
	        System.out.println("No Pistachio values in this save frame (" +
		  star.getFrameName(frame) + ")");
	    }
	    return;
	}
	if (!entityID.equals("")) {
	  meritValsTmp = S2DUtils.selectMatches(entityIDs, meritValsTmp,
	    entityID);
	}
	double[] meritVals = S2DUtils.arrayStr2Double(meritValsTmp);
	meritValsTmp = null;

	String[] resSeqCodesTmp = star.getFrameValues(frame,
	  star.CHEM_SHIFT_VALUE, star.CHEM_SHIFT_RES_SEQ_CODE);
	if (!entityID.equals("")) {
	  resSeqCodesTmp = S2DUtils.selectMatches(entityIDs,
	    resSeqCodesTmp, entityID);
	}
	int[] resSeqCodes = S2DUtils.arrayStr2Int(resSeqCodesTmp);
	resSeqCodesTmp = null;

	String[] residueLabels = star.getFrameValues(frame,
	  star.CHEM_SHIFT_VALUE, star.CHEM_SHIFT_RES_LABEL);

	String[] atomNames = star.getFrameValues(frame,
	  star.CHEM_SHIFT_VALUE, star.CHEM_SHIFT_ATOM_NAME);
	if (!entityID.equals("")) {
	  atomNames = S2DUtils.selectMatches(entityIDs, atomNames,
	    entityID);
	}

	//
	// Create an S2DPistachio object with the values we just got.
	//
        S2DPistachio pistachio = new S2DPistachio(_name, _dataDir,
	  _sessionDir, _summary, resSeqCodes, residueLabels, atomNames,
	  meritVals);

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
	dCoords.writeCoords(residues, tmpFile, PISTACHIO_WRAP_LENGTH);

	S2DmmCifIfc cif = new S2DmmCifIfc(tmpFile);
	saveFrameAtomicCoords(cif, null, frameIndex, pt, true);

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
    private void saveFrameAmbiguity(S2DStarIfc star, SaveFrameNode frame,
      String entityID, S2DResidues residues, int frameIndex)
      throws S2DException
    {
        if (DEBUG >= 3) {
	    System.out.println("    S2DMain.saveFrameAmbiguity(" +
	      star.getFrameName(frame) + " (" + entityID + "), " +
	      frameIndex + ")");
	}

//TEMPTEMP -- change CHEM_SHIFT_VALUE here?
	//
	// Get the values we need from the Star file.
	//
	String[] entityIDs = null;
	if (!entityID.equals("")) {
	    entityIDs = star.getFrameValues(frame, star.CHEM_SHIFT_VALUE,
	      star.CHEM_SHIFT_ENTITY_ID);
	}

	String[] ambiguityTmp;
	try {
	    ambiguityTmp = star.getFrameValues(frame,
	      star.CHEM_SHIFT_VALUE, star.CHEM_SHIFT_AMBIG_CODE);
	} catch (S2DException ex) {
            if (DEBUG >= 3) {
	        System.out.println("No ambiguity values in this save frame (" +
		  star.getFrameName(frame) + ")");
	    }
	    return;
	}
	if (!entityID.equals("")) {
	  ambiguityTmp = S2DUtils.selectMatches(entityIDs,
	    ambiguityTmp, entityID);
	}
	int[] ambiguityVals = S2DUtils.arrayStr2Int(ambiguityTmp);
	ambiguityTmp = null;

	String[] resSeqCodesTmp = star.getFrameValues(frame,
	  star.CHEM_SHIFT_VALUE, star.CHEM_SHIFT_RES_SEQ_CODE);
	if (!entityID.equals("")) {
	  resSeqCodesTmp = S2DUtils.selectMatches(entityIDs,
	    resSeqCodesTmp, entityID);
	}
	int[] resSeqCodes = S2DUtils.arrayStr2Int(resSeqCodesTmp);
	resSeqCodesTmp = null;

	String[] residueLabels = star.getFrameValues(frame,
	  star.CHEM_SHIFT_VALUE, star.CHEM_SHIFT_RES_LABEL);
	if (!entityID.equals("")) {
	  residueLabels = S2DUtils.selectMatches(entityIDs,
	    residueLabels, entityID);
	}

	String[] atomNames = star.getFrameValues(frame,
	  star.CHEM_SHIFT_VALUE, star.CHEM_SHIFT_ATOM_NAME);
	if (!entityID.equals("")) {
	  atomNames = S2DUtils.selectMatches(entityIDs, atomNames,
	    entityID);
	}

	//
	// Create an S2DAmbiguity object with the values we just got.
	//
        S2DAmbiguity ambiguity = new S2DAmbiguity(_name, _dataDir,
	  _sessionDir, _summary, resSeqCodes, residueLabels, ambiguityVals);

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
	dCoords.writeCoords(residues, tmpFile, AMBIGUITY_WRAP_LENGTH);

	S2DmmCifIfc cif = new S2DmmCifIfc(tmpFile);
	saveFrameAtomicCoords(cif, null, frameIndex, at, true);

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
        if (DEBUG >= 3) {
	    System.out.println("    S2DMain.saveFrameLACS(" +
	      star.getFrameName(frame) + ", " + frameIndex + ")");
	}

	S2DLacs lacs = new S2DLacs(_name, _longName, _dataDir,
	  _sessionDir, _summary);

        //
        // Get the values we need from the Star file.
        //

	lacs._xCoordName = star.getTagValue(frame, star.LACS_X_NAME);
	lacs._yCoordName = star.getTagValue(frame, star.LACS_Y_NAME);

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
	lacs._resSeqCodes = S2DUtils.arrayStr2Int(resSeqCodesTmp);
	resSeqCodesTmp = null;

        lacs._resLabels = star.getFrameValues(frame,
          star.LACS_RES_NUM, star.LACS_RES_LABEL);

        String[] xCoordsTmp = star.getFrameValues(frame,
	  star.LACS_RES_NUM, star.LACS_X_VALUE);
	lacs._xCoords = S2DUtils.arrayStr2Double(xCoordsTmp);
	xCoordsTmp = null;

        String[] yCoordsTmp = star.getFrameValues(frame,
	  star.LACS_RES_NUM, star.LACS_Y_VALUE);
	lacs._yCoords = S2DUtils.arrayStr2Double(yCoordsTmp);
	yCoordsTmp = null;

        String[] desigsTmp = star.getFrameValues(frame,
	  star.LACS_RES_NUM, star.LACS_DESIGNATOR);
	lacs._desigs = S2DUtils.arrayStr2Int(desigsTmp);
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
        if (DEBUG >= 3) {
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
        if (DEBUG >= 1) {
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
	} else {
	    S2DmmCifIfc cif = new S2DmmCifIfc(pdbId, _useLocalFiles);
	    saveFrameAtomicCoords(cif, null, _currentFrameIndex, null, false);
	}
	_currentFrameIndex++;

        _currentPdbId = null;
    }
}

// ========================================================================

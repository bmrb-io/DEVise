// ========================================================================
// DEVise Data Visualization Software
// (c) Copyright 2000-2012
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
// Revision 1.289  2012/01/27 23:19:37  wenger
// Added the -s2p_name and -dyn_movie_demo arguments and generate_movies
// script to allow generation of visualizations for demo dynamics movies.
//
// Revision 1.288  2012/01/27 17:43:10  wenger
// Added the -do_dyn_movie argument and bmrb_mirror.do_dyn_movie_default
// property to control whether the molecular dynamics movies are enabled.
//
// Revision 1.287  2012/01/20 16:09:21  wenger
// Merged s2d_mailfix_br_0 thru s2d_mailfix_br_1 to trunk.
//
// Revision 1.286  2011/12/29 23:10:41  wenger
// Visualization summary page is now partly converted to the new color
// scheme and layout.
//
// Revision 1.285  2011/12/28 21:08:02  wenger
// Updated selection pages, per-visualization pages (but not histograms),
// and help pages to the new layout and color scheme.
//
// Revision 1.284.2.3  2012/01/19 22:25:56  wenger
// Finished cleaning "unnecessary mail reduction" fixes.
//
// Revision 1.284.2.2  2012/01/13 00:59:03  wenger
// Added automatic 'basic' processing, so we don't get errors if
// someone tries to do coordinate processing without having done the
// 'basic' processing first.
//
// Revision 1.284.2.1  2012/01/09 22:32:30  wenger
// S2d no longer sends error emails in a number of cases that are
// instances of faulty inputs rather than actual software errors
// (see to-do 139).  (Note:  this still needs some work/more checking.)
//
// Revision 1.284  2011/11/02 16:38:00  wenger
// Changed version to 12.1.6x1, added 12.1.6 version history section.
//
// Revision 1.283  2011/11/01 18:38:33  wenger
// Changed version to 12.1.5 for release.
//
// Revision 1.282  2011/10/31 20:18:37  wenger
// In the S2 predicted vs. experimental visualizations the secondary
// structures are now obtained from DSSP output files instead of from the
// chemical shift reference back calculations.
//
// Revision 1.281  2011/10/21 22:18:43  wenger
// Moved getting the bmrb_mirror.s2p_url property from getPropertiesDynamic()
// to getProperties() -- it should have been there originally.
//
// Revision 1.280  2011/10/20 19:09:33  wenger
// Added DOCTYPE to all html pages and templates.
//
// Revision 1.279  2011/10/19 22:07:15  wenger
// Improvments to S2 predicted vs. experimental visualizations: made the
// dynamics movie button view very thin (almost invisible) and reduced
// the gaps between other views.
//
// Revision 1.278  2011/10/13 15:04:33  wenger
// Changed version to 12.1.5x1 and added 12.1.5 version history section.
//
// Revision 1.277  2011/10/12 18:58:41  wenger
// Changed version to 12.1.4 for release.
//
// Revision 1.276  2011/10/10 23:43:39  wenger
// Reduced edited movie time from .1 to .02, and set the resolution to
// 400x400 to speed up generation time (just took 2:44 in a test).
//
// Revision 1.275  2011/09/20 19:44:54  wenger
// Changed version to 12.1.4x1, added 12.1.4 version history section.
//
// Revision 1.274  2011/09/19 19:14:42  wenger
// Fixed tutorial video links for the visualization server.
//
// Revision 1.273  2011/09/15 19:04:31  wenger
// Changed version to 12.1.3x1, added 12.1.3 version history section.
//
// Revision 1.272  2011/09/14 19:47:44  wenger
// Changed version to 12.1.2 for release.
//
// Revision 1.271  2011/09/12 19:27:34  wenger
// Implemented to-do 156 (add legend to LACS visualizations).
//
// Revision 1.270.2.6  2011/10/10 23:19:28  wenger
// Reduced edited movie time from .1 to .02, and set the resolution to
// 400x400 to speed up generation time (just took 2:44 in a test).
//
// Revision 1.270.2.5  2011/10/06 15:56:48  wenger
// Changed the s2predicted session to match the new "session-specific"
// menu option in the JS -- no longer shows the movie buttons in the
// visualization itself because of conflict with Jmol on Macs.
//
// Revision 1.270.2.4  2011/09/22 21:22:11  wenger
// We now don't show the movie buttons if a movie is not available; also
// improved the way the movie configuration works.
//
// Revision 1.270.2.3  2011/09/22 16:02:00  wenger
// Added configuration for the dynamics movie URLs.
//
// Revision 1.270.2.2  2011/09/21 20:46:10  wenger
// The s2predicted session movie buttons are now updated for the correct
// PDB ID -- I should probably have configuration for the URL, though,
// and also only show the button if the movie is available.
//
// Revision 1.270.2.1  2011/09/08 16:50:43  wenger
// Changed the JavaScreen client version required by Peptide-CGI with
// dynamics to 5.14.0, because that version has the embedded button fixes
// we need for the dynamics movies.
//
// Revision 1.270  2011/09/05 21:32:15  wenger
// Implemented to-do 075: html page generated by visualization server
// failures shows error messages and warnings.
//
// Revision 1.269  2011/07/06 16:13:25  wenger
// Changed version to 12.1.2 and added 12.1.2 version history section again.
//
// Revision 1.268  2011/06/28 02:09:07  wenger
// Fixed tutorial videos link for summary visualization pages.
//
// Revision 1.267  2011/06/28 00:18:37  wenger
// Updated version to 12.1.2 and added 12.1.2 version history section.
//
// Revision 1.266  2011/06/27 19:47:55  wenger
// Changed version to 12.1.1 for release.
//
// Revision 1.265  2011/06/24 20:40:26  wenger
// More print mode color fixes...
//
// Revision 1.264  2011/06/24 18:34:48  wenger
// Made a bunch of fixes to print mode colors.  (Although for some reason
// some of them are not working right -- the coordinate visualizations,
// for example.  They work right in the session template, but when I use
// a "real" coordinate visualization, switching to color print mode doesn't
// switch colors correctly in the data view.  Seems like it may have
// something to do with child views.)
//
// Revision 1.263  2011/06/17 20:02:18  wenger
// Partway through fixing bug 124 (simulated spectrum colors) -- working
// on H vs. N 2-entry vis.  At least close, but I may want to change things
// just a tad again.
//
// Revision 1.262  2011/06/16 23:30:56  wenger
// Fixed Peptide-CGI bug 123 (change "All" to "All by violation", etc., in
// distance restraint selection views).
//
// Revision 1.261  2011/05/24 15:51:06  wenger
// Changed version to 12.1.1, added version history section, made disabled
// test warning more prominent in test_all.
//
// Revision 1.260  2011/05/23 19:47:17  wenger
// Added S2 predicted vs. experimental help page; fixed some minor problems
// with the s2 predicted vs. experiemental html page.
//
// Revision 1.259  2011/05/20 20:41:08  wenger
// Changed version to 12.1.0 for release; made notes about parts of the
// testing doc that are no longer correct.
//
// Revision 1.258  2011/05/20 15:37:16  wenger
// Added bug 120 and temporarily disabled test22, test23, test_tar9,
// test_tar9i, test_dist2i and test_dis2imu.
//
// Revision 1.257  2011/05/19 20:50:12  wenger
// Did some post-merge cleanups of the s2predict stuff.
//
// Revision 1.256  2011/05/19 19:46:09  wenger
// Merged s2d_mol_dyn_br_0 thru s2d_mol_dyn_br_2 to trunk.
//
// Revision 1.255  2011/05/14 18:04:15  wenger
// Fixed minor typo in diagnostic output.
//
// Revision 1.254  2011/02/07 22:00:53  wenger
// Changed the legends for the simulated 1H-15N and simulated 1H-13C
// spectra to client-side rendering of the fonts, for better appearance.
//
// Revision 1.253.2.24  2011/05/14 17:56:14  wenger
// A few more cleanups in preparation for merging to the trunk and
// releasing.
//
// Revision 1.253.2.23  2011/05/13 19:33:27  wenger
// Minor (hopefully final) cleanups.
//
// Revision 1.253.2.22  2011/05/13 18:05:33  wenger
// Changed s2predict PDB IDs to show up as upper-case.
//
// Revision 1.253.2.21  2011/05/13 17:02:45  wenger
// Fixed up configuration for BMRB to point to "real" s2predict output files.
//
// Revision 1.253.2.20  2011/05/11 20:53:31  wenger
// Completed s2p tests and modified code so that they all work -- we do
// the s2predict processing before the final ChemShiftRef processing, so
// that PDB IDs found only from the s2predict output will get CSR processing
// done.
//
// Revision 1.253.2.19  2011/05/10 21:29:39  wenger
// Incorporated secondary structure into s2predict visualizations.
//
// Revision 1.253.2.18  2011/04/26 21:46:56  wenger
// Doing ChemShiftRef processing for s2 predicted vs. experimental is
// mostly working (I'm going to use the ChemShiftRef output for secondary
// structures).  (Still have to get a few cases working right.)
//
// Revision 1.253.2.17  2011/04/13 20:55:25  wenger
// Added test (not yet complete) to test s2predict processing in
// vis server mode.
//
// Revision 1.253.2.16  2011/04/13 19:42:45  wenger
// Summary page CGI link now works for s2predict processing; changed one
// test to use make_view.
//
// Revision 1.253.2.15  2011/04/13 18:24:59  wenger
// Fixed s2predict processing for PDB IDs that are not listed in the
// NMR-STAR file.
//
// Revision 1.253.2.14  2011/04/12 21:03:51  wenger
// Some cleanup to the s2predict-related code.
//
// Revision 1.253.2.13  2011/04/11 23:08:51  wenger
// At least partially fixed the correspondence between coordinate indices
// for s2predict and the "regular" atomic coordinates.
//
// Revision 1.253.2.12  2011/04/11 22:21:09  wenger
// Added PDB ID to info in s2predict visualizations.
//
// Revision 1.253.2.11  2011/04/11 21:35:54  wenger
// Made the s2predict tests more complete.
//
// Revision 1.253.2.10  2011/04/08 22:32:25  wenger
// Finding s2predict output files now mostly in place.
//
// Revision 1.253.2.9  2011/04/08 17:48:09  wenger
// Writing s2predicted links in the summary html page is now partially
// working.
//
// Revision 1.253.2.8  2011/04/07 18:23:14  wenger
// Fixed problem with 3D highlighting not working in 2dpredicted
// visualizations.
//
// Revision 1.253.2.7  2011/04/06 19:53:35  wenger
// We now create the specific html files for the s2predicted visualizations.
//
// Revision 1.253.2.6  2011/04/06 18:48:25  wenger
// We now create the s2predicted sessions from the template.
//
// Revision 1.253.2.5  2011/04/06 16:05:37  wenger
// Added creation of an empty secondary structure file, and modified the
// s2predict template to use the correct secondary structure data source.
//
// Revision 1.253.2.4  2011/04/05 21:30:31  wenger
// Reversed the sense of the s2predict output to match the S2 values in
// BMRB entries; also added test_s2p1 to the test_all script.
//
// Revision 1.253.2.3  2011/04/01 22:27:05  wenger
// Added single-letter residue codes to the residue list files; removed
// a bunch of debug output from the s2predict code.
//
// Revision 1.253.2.2  2011/04/01 18:50:35  wenger
// Basic output of the s2predict data for DEVise is in place.
//
// Revision 1.253.2.1  2011/03/30 19:53:14  wenger
// The basic framework for s2 experimental vs. predicted processing is
// now in place.
//
// Revision 1.253  2011/02/01 01:10:38  wenger
// Fixed minor errors and warnings in session templates.
//
// Revision 1.252  2011/01/12 20:38:59  wenger
// Changed version to 12.0.1, added new version history section; added
// config file for pacu.
//
// Revision 1.251  2011/01/10 23:22:47  wenger
// Changed version to 12.0.0 for release.
//
// Revision 1.250  2011/01/10 18:52:43  wenger
// Fixed problem with RNA chem shift delta visualizations caused by
// the changes for the multi-entry visualizations.
//
// Revision 1.249  2011/01/07 22:10:28  wenger
// Merged s2d_multi_entry2_br_0 thru s2d_multi_entry2_br_1 to trunk.
//
// Revision 1.248  2010/12/07 20:46:50  wenger
// Added 'CANCELLED' to result possibilities for log.
//
// Revision 1.247.2.22  2011/01/05 20:57:39  wenger
// Fixed a bug that caused improper processing for the uploaded data/
// entry combination.
//
// Revision 1.247.2.21  2011/01/05 18:21:18  wenger
// Hopefully final code cleanup, updated multi-entry help pages.
//
// Revision 1.247.2.20  2011/01/05 16:46:22  wenger
// Finished cleanup of multi-entry code in S2DMain.
//
// Revision 1.247.2.19  2011/01/05 15:33:15  wenger
// More cleanup, including at least temporarily(?) just eliminating
// some things for the multi-entry visualizations, such as system name
// and frame title.
//
// Revision 1.247.2.18  2011/01/04 19:19:37  wenger
// Added two-entry help pages (still need changes to the text) and changed
// the specific HTML code so that two-entry pages link to the right help
// pages.
//
// Revision 1.247.2.17  2011/01/04 18:28:20  wenger
// Fixed up data structure of two-entry summary page.
//
// Revision 1.247.2.16  2011/01/03 23:39:30  wenger
// Cleaned up two-entry summary page (with inefficient data structure
// for now).
//
// Revision 1.247.2.15  2011/01/03 21:16:54  wenger
// Added legends to the multi-entry visualizations -- using multiple
// views with a single record each to avoid having to make a new data
// file for the legend.
//
// Revision 1.247.2.14  2010/12/29 22:42:39  wenger
// Fixed HTML page title and session info for two-entry visualizations.
//
// Revision 1.247.2.13  2010/12/29 21:52:03  wenger
// Fixed a bug that caused two-entry sessions with 4099 as the first
// entry to not actually show the 4099 data.
//
// Revision 1.247.2.12  2010/12/28 23:15:28  wenger
// We now print comments in the single-entry summary HTML pages that we
// will use to figure out what data we have for the multi-entry processing
// (reading the comments is not implemented yet).
//
// Revision 1.247.2.11  2010/12/21 00:24:44  wenger
// Got rid of 'sizeString' stuff in summary HTML code; started putting
// multi-entry summary page links into a table.
//
// Revision 1.247.2.10  2010/12/16 18:30:51  wenger
// Some minor cleanups -- we now check for "extra" BMRB IDs duplicating
// each other and the "main" ID, etc.
//
// Revision 1.247.2.9  2010/12/16 00:11:06  wenger
// Changed how we come up with the list of available data for each
// entry so that we don't need the -force option anymore for multi-entry
// processing.
//
// Revision 1.247.2.8  2010/12/14 20:58:51  wenger
// Moved the protein chemical shift-specific multi-entry code into the
// S2DProteinChemShift class; added missing multi-entry files in some
// of the multi-entry test checks.
//
// Revision 1.247.2.7  2010/12/14 20:02:37  wenger
// Made some cleanups of the multi-entry summary HTML file; added
// test_mult4 (to make sure 4096/4038 combination works with multi-
// entry processing).
//
// Revision 1.247.2.6  2010/12/10 22:09:07  wenger
// Decided to change the version to 12.0.0 for the first multi-entry
// version.
//
// Revision 1.247.2.5  2010/12/09 20:13:07  wenger
// Got STAR file version working for multiple-entry processing.
//
// Revision 1.247.2.4  2010/12/08 22:17:22  wenger
// Fixed output of HE1/NE1 lines (added name) (!); added checks (I thought
// I already had checks for that, but apparently not).
//
// Revision 1.247.2.3  2010/12/08 21:01:46  wenger
// Got first version of html interface to initiate multi-entry processing
// working; changed s2d.log file to go into the tmp directory because of
// permission issues.
//
// Revision 1.247.2.2  2010/12/08 18:20:30  wenger
// Added Peptide-CGI version to log, and cleaned up the argument logging
// a bit.
//
// Revision 1.247.2.1  2010/12/07 23:43:49  wenger
// Merged s2d_multi_entry_br_0 thru s2d_multi_entry_br_1 to
// s2d_multi_entry2_br.
//
// Revision 1.247  2010/12/07 20:03:18  wenger
// To-do #150: added logging of how long each invocation of Peptide-CGI
// takes, and whether it uses the cache.
//
// Revision 1.246  2010/12/07 17:41:15  wenger
// Did another version history purge.
//
// Revision 1.245  2010/11/29 22:41:33  wenger
// Changed version to 11.8.9_x1, added new version history section.
//
// Revision 1.244  2010/11/25 16:57:30  wenger
// Changed version to 11.8.8 for release.
//
// Revision 1.243  2010/11/24 18:47:41  wenger
// Implemented to-do 113 (changed the coordinates link for entry 4096
// from "none" to "internal" to be clearer).
//
// Revision 1.242  2010/11/24 17:52:58  wenger
// Fixed bug 118 (Data Scroll View not working correctly in restraint
// visualizations).  The actual fix is as follows (matching what was
// already in place for the coordinates visualization; I don't know why
// the restraints weren't the same):
// 1) Change Data Scroll views to use the rl (residue list) data source
// instead of the ac (atomic coordinates) data source.
// 2) Filter the records in the Data Scroll Views by entity assembly ID.
// 3) Enable X home (keyboard and implicit) for the Data SCroll Views
// (with a margin of 0.5).
//
// Revision 1.241  2010/11/22 23:24:46  wenger
// Merged violation_select2_br_0 thru violation_select2_br_1 to trunk,
// including cleaning up a couple of leftover temporary comments.
//
// Revision 1.240.2.4  2010/11/17 20:16:08  wenger
// Eliminated the 'doesCount' attribute from restraint data (this is
// something that never got used in the session -- it was part of working
// towards having the violation magitude link also affect the restraint
// counts shown in the data window).
//
// Revision 1.240.2.3  2010/11/17 19:24:41  wenger
// Changed how links work so that only data already selected by existing
// filters (e.g., violated/non-violated) shows up in the violation
// magnitude view.
//
// Revision 1.240.2.2  2010/11/17 17:55:59  wenger
// Changed the maximum violation view to more clearly show that it's
// displaying a list of the maximum violation for the selected data,
// and changed the top and bottom margins from 0.05 to 0.01 A.
//
// Revision 1.240.2.1  2010/11/10 17:42:34  wenger
// Merged violation_select_br_1 thru violation_select_br_2 to
// violation_select2_br.
//
// Revision 1.240  2010/11/02 21:42:41  wenger
// Changed version to 11.8.8x1 and added 11.8.8 version history section.
//
// Revision 1.239  2010/11/02 01:35:19  wenger
// Changed version to 11.8.7 for release.
//
// Revision 1.238  2010/11/01 21:02:48  wenger
// Fixed the Pistachio meta-data.
//
// Revision 1.237  2010/11/01 17:54:25  wenger
// Changed 'whitespace' to 'separator' in bmrb-summary schema file to
// fix error in 4096 coordinate visualization caused by single quotes
// in sample info.
//
// Revision 1.236  2010/11/01 00:51:12  wenger
// Merged sample_cond2_br_0 thru sample_cond2_br_1 to trunk.
//
// Revision 1.235.2.8  2010/10/29 21:57:10  wenger
// Changed how things work for 3.0/3.1 files -- we follow the experiment
// ID to get the sample info if necessary; tests changed accordingly.
//
// Revision 1.235.2.7  2010/10/29 14:32:34  wenger
// Added test of sample conditions with 6838; fixed code to make sure
// we don't get multiple copies of the same sample info.
//
// Revision 1.235.2.6  2010/10/28 17:04:31  wenger
// Added sample details to the sample information we put into drill-down
// data.
//
// Revision 1.235.2.5  2010/10/27 18:29:03  wenger
// Fixed all existing tests to work with the new sample conditions drill-
// down code.
//
// Revision 1.235.2.4  2010/10/22 16:57:34  wenger
// Hopefully final cleanup of 3.0/3.1 code for sample conditions, etc.
//
// Revision 1.235.2.3  2010/10/19 19:44:19  wenger
// Sample conditions, etc., are now working for 3.0/3.1 files (still need
// cleanup).
//
// Revision 1.235.2.2  2010/10/16 01:32:26  wenger
// Getting sample conditions save frame names now works for 2.1 files.
//
// Revision 1.235.2.1  2010/10/15 15:29:02  wenger
// Merged sample_cond_br_0 thru sample_cond_br_1/sample_cond_br_end to
// sample_cond2_br (to get the latest code refactoring from the trunk
// into the sample conditions code).
//
// Revision 1.235.4.8  2010/12/04 00:34:53  wenger
// Got preliminary multi-entry summary page working.
//
// Revision 1.235.4.7  2010/11/16 22:45:42  wenger
// First steps in creating two-entry visualizations for all save frame
// combos; fixed an error that caused the second entry to get processed
// twice.
//
// Revision 1.235.4.6  2010/11/16 00:01:17  wenger
// We now create a "two-entry" summary HTML page (but it doesn't have the
// right links yet); added "two-entry" HTML pages to the tests.
//
// Revision 1.235.4.5  2010/11/15 17:39:35  wenger
// Added the entry ID/name to the H vs. C and H vs. N data, so it shows up
// in drill-down (did some funky stuff with the schemas so we don't goof
// up the other Peptide-CGI installation at CS).
//
// Revision 1.235.4.4  2010/11/13 00:05:25  wenger
// Basic creation of session-specific HTML pages for two-entry
// visualizations is now in place (includes removing some leftover
// provisions for the "large" specific HTML files).
//
// Revision 1.235.4.3  2010/11/12 22:03:18  wenger
// Basic creation of two-entry H vs. N and H vs. C sessions is now
// working (but still needs lots of enhancements).
//
// Revision 1.235.4.2  2010/11/12 17:21:28  wenger
// Added test_mult3 to test visualization server/multi-entry combination;
// some cleanup of S2DMain.
//
// Revision 1.235.4.1  2010/11/05 21:28:39  wenger
// First phase of multi-entry processing -- we can now specify more than
// one entry on the command line, and the individual processing for each
// entry is done correctly.  No multiple-entry session files are generated,
// though.
//
// Revision 1.235  2010/10/13 21:42:37  wenger
// Oops -- should have incremented version x count in previous commit.
//
// Revision 1.234  2010/10/13 20:44:02  wenger
// Finished restructuring Peptide-CGI code so that we get values from
// the STAR files in the relevant object constructors, instead of in the
// S2DMain class.
//
// Revision 1.233.2.2  2010/10/11 14:38:33  wenger
// Started on method to get sample conditions for data save frames; I'm
// just committing a preliminary version until I make a change on the trunk
// to move all of the code for actually getting the relevant frame values
// down into the data-specific classes like I've already done with the
// delta shifts.
//
// Revision 1.233.2.1  2010/10/08 21:17:41  wenger
// We now put save frame details into the drill-down data for the data
// selection view in 3D visualizations; also fixed a bug in getting save
// frame details for 3.0/3.1 files.
//
// Revision 1.233  2010/10/06 13:25:12  wenger
// We now default to printing more messages about skipping saving residue
// lists and chemical shifts for non-polymers.
//
// Revision 1.232  2010/09/29 18:14:30  wenger
// Merged violation_select_br_0 thru violation_select_br_1 to trunk.
//
// Revision 1.231.2.1  2010/11/10 17:08:01  wenger
// Committing all violation changes so I can "bounce" this branch off
// the trunk to fix problems with schema incompatibilities.
//
// Revision 1.231  2010/09/27 17:22:40  wenger
// Changed Peptide-CGI version to 11.8.7x1, added 11.8.7 version history
// section.
//
// Revision 1.230  2010/09/24 17:55:25  wenger
// Changed Peptide-CGI version to 11.8.6 for release.
//
// Revision 1.229  2010/09/23 17:58:21  wenger
// Moved the default printing of the Peptide-CGI version, because the
// printing it at the beginning goofed up the distribution scripts.
//
// Revision 1.228  2010/09/01 18:49:56  wenger
// Merged fix_3d_cursor_br_0 thru fix_3d_cursor_br_1 to trunk.
//
// Revision 1.227  2010/08/02 22:12:33  wenger
// Peptide-CGI version is now printed as part of standard output.
//
// Revision 1.226  2010/07/21 19:19:54  wenger
// Changed version to 11.8.6x1, added new version history section.
//
// Revision 1.225  2010/07/21 15:55:27  wenger
// Changed version to 11.8.5 for release.
//
// Revision 1.224  2010/07/20 17:49:06  wenger
// Fixed Peptide-CGI for changes to NMR-STAR files -- residues for DNA
// are now DA, DC, etc., instead of just A, C, etc. (note: test68/
// bmr15083.str still has old labels; test68_3/bmr15083_3.str has new
// labels); 2kid has disappeared from the remediated restraints.
//
// Revision 1.223  2010/07/15 21:37:32  wenger
// Fixed Peptide-CGI bug 111.
//
// Revision 1.222  2010/07/14 21:47:54  wenger
// Fixed Peptide-CGI bug 110.
//
// Revision 1.221.2.3  2010/08/31 17:38:39  wenger
// Changed Peptide-CGI session templates to use the new cursor features
// when relevant.
//
// Revision 1.221.2.2  2010/08/04 15:15:38  wenger
// Updated cs_test configuration to get both 3.1 and 2.1 files; added
// stuff to version history about 3D cursor fixes.
//
// Revision 1.221.2.1  2010/07/29 17:58:41  wenger
// Added specification of the JS command port to the configuration, and
// made other changes so that the "test" Peptide-CGI setup at CS can use
// the "test" JavaScreen installation.
//
// Revision 1.221  2010/07/07 20:54:13  wenger
// Changed Peptide-CGI to work with new JavaScreen re-sizing feature
// (since the user can now re-size the JS, we don't generate html
// pages for different sizes of visualization; this also includes
// eliminating the different-size pages for the histograms).
//
// Revision 1.220  2010/07/07 18:39:09  wenger
// Changed version to 11.8.5x1 and added 11.8.5 version history section.
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

    	// Set this to true for errors that should not generate
	// email.
    public static boolean _noMail = false;

    	// Whether to do "extra" calls to System.gc().
    private static boolean _extraGC = false;

    public static final String PEP_CGI_VERSION = "12.2.0x4"/*TEMP*/;
    public static final String DEVISE_MIN_VERSION = "1.11.1";
    public static final String JS_CLIENT_MIN_VERSION = "5.14.0";

    private static FileWriter _logWriter = null;

    private String _masterBmrbId = ""; // accession number the user requested

    // "Extra" BMRB ID(s) for multiple-entry visualizations.
    private Vector _extraBmrbIdList = new Vector();
    private int _extraBmrbIdIndex = 0;

    // A list of all BMRB IDs (now stored as Strings) we need to access
    // for this entry.
    private Vector _bmrbIds = new Vector();

    // A list of all local files to process.
    //TEMP -- when does this ever have more than one element???
    private Vector _localFiles = new Vector();
    // Save _localFiles list for multi-entry processing.
    private Vector _originalLocalFiles = new Vector();

    // A list of related PDB files we want to process for this entry.
    private Vector _pdbIds = new Vector(); // Vector contains Strings

    private String _dataDir;
    private String _csrDataDir = null;
    private String _htmlDir;
    private String _sessionDir;
    private String _connectionFile;

    // The name of this data set (e.g., "4001") -- all output files
    // generated will consist of the name plus various suffixes.
    private String _name = "";

    // If we're processing a single entry, _masterName is the same as
    // _name; for multiple entries, this is the name based on the "master"
    // BMRB ID or name, not the one(s) specified by -xbmrbid.
    private String _masterName = "";

    // The same as name, expect possibly with some additional info
    // (e.g., "BMRB 4001").
    private String _longName = null;

    // A combination of the long names for all entries we're processing
    // (e.g., "BMRB 4001 + BMRB 4099").
    private String _fullLongName = "";
    
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

    public static final int S2PRED_LEVEL_NONE = 0;
    public static final int S2PRED_LEVEL_LINK = 1;
    public static final int S2PRED_LEVEL_PROCESS = 2;
    private int _s2PredLevel = S2PRED_LEVEL_LINK;
    private int _s2FrameIndex = 0;
    // Name to use to find s2predict values (if not the "regular" name).
    private String _s2PName;

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

    private static Date _startDate;
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
	logLine("");
	logLine("Peptide-CGI version " + PEP_CGI_VERSION);
	logArgs(args);

	String result = "FAILURE";
	try {
	    run(args);
	    result = "SUCCESS";
	} catch (S2DCancel ex) {
	    // Just catch this so we don't pass it on to higher levels.
	    result = "CANCELLED";
	} catch (S2DWarning ex) {
	    System.err.println("Top-level warning: ");
	    System.err.println(ex.toString());
	} catch (Exception ex) {
	    System.err.println("\n*** Top-level error: ");
	    ex.printStackTrace();
	    System.err.println(ex.toString());
	    if (_noMail) {
	        System.exit(2);
	    } else {
	        throw new S2DError("NMR-Star to DEVise conversion failed");
	    }
	} finally {
	    logFinish(result);
	}
    }

    //-------------------------------------------------------------------
    // Do the conversion, retrying without PDB if we fail because of
    // running out of memory and we were trying to get data from PDB.
    public static void run(String args[]) throws S2DException
    {
        if (doDebugOutput(2)) {
	    System.out.println("S2DMain.run()");
	}

        S2DMain s2d = new S2DMain(args, false);

	if (_retrying) {
	    s2d._pdbLevel = PDB_LEVEL_NONE;
	    s2d._csrLevel = CSR_LEVEL_NONE;
	    s2d._lacsLevel = LACS_LEVEL_NONE;
	    s2d._tarLevel = TAR_LEVEL_NONE;
	    s2d._rrTarLevel = RRTAR_LEVEL_NONE;
	    s2d._distRLevel = DISTR_LEVEL_NONE;
	    s2d._rrDistRLevel = RRDISTR_LEVEL_NONE;
	    s2d._s2PredLevel = S2PRED_LEVEL_NONE;
	}

	// If we're processing coordinates, restraints, etc., make sure
	// the 'basic' processing has been done.  (Under normal conditions
	// we should only do the 'secondary' processing if we've already
	// done the 'basic' processing, but we could end up otherwise
	// if the user has an old version of a summary page cached or
	// something.  At any rate, we do see that happening sometimes.)
	// Note: test_tar1 tests this.
        if (!s2d._masterBmrbId.equals("") && s2d._cmdFrameIndex != -1) {
            if (doDebugOutput(1)) {
	        System.out.println("Doing 'basic' processing");
	    }
	    S2DMain s2dBasic = new S2DMain(args, true);
	    doProcessing(s2dBasic);
	}

	try {
	    doProcessing(s2d);
	    while (s2d.nextBmrbId()) {
	        doProcessing(s2d);
	    }
	    if (s2d.multiEntry()) {
		s2d.createMultiEntrySessions();
	    }
	    // This code should be enabled for testing only!
            if (!_retrying && false) {
	        System.out.println("Retrying for test");

		s2d = null;
		System.gc();
	        _retrying = true;
		_noMail = false;
		run(args);
            }
        } catch (OutOfMemoryError mem) {
	    System.err.println("OutOfMemoryError: " + mem.toString());
	    if (s2d._pdbLevel == PDB_LEVEL_PROCESS && !_retrying) {
	        System.out.println("Retrying without accessing PDB");

		s2d = null;
		System.gc();
	        _retrying = true;
		_noMail = false;
		run(args);
	    } else {
	        throw new S2DError("NMR-Star to DEVise conversion failed " +
		  "because of insufficient memory");
	    }
	}
    }

    //-------------------------------------------------------------------
    public static void doProcessing(S2DMain s2d) throws S2DException
    {
        if (doDebugOutput(2)) {
	    System.out.println("S2DMain.doProcessing(" + s2d._name +
	      ")");
	}

	logLine("Processing " + s2d.getName() + "...");

        if (!s2d.useCache()) {
	    logLine("...not using cache");
            s2d.process();
        } else {
	    logLine("...using cache");
            if (doDebugOutput(1)) {
                System.out.println("Cache files used");
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
    // Log the arguments of this process.
    static void logArgs(String args[])
    {
    	String line = "Arguments: ";
	for (int index = 0; index < args.length; index++) {
	    line += "<" + args[index] + "> ";
	}

	logLine(line);
    }

    //-------------------------------------------------------------------
    // Finish the logging for this process -- print the result and
    // elapsed time, and close the log file.
    static void logFinish(String result)
    {
        Date date = new Date();
	double elapsedTime =
	  (date.getTime() - _startDate.getTime()) / 1000.0;
        logLine("result: " + result + " (elapsed time: " +
	  elapsedTime + " s)");

	if (_logWriter != null) {
	    try {
	        _logWriter.close();
	    } catch (IOException ex) {
	        System.err.println("Error (" + ex.toString() +
	          ") closing log file!");
	    }
	}
    }

    //-------------------------------------------------------------------
    // Log a line to the log file.
    public static void logLine(String line)
    {
	try {
    	    if (_logWriter == null) {
	        _logWriter = new FileWriter("tmp/s2d.log", true);
	    }

	    if (line.equals("")) {
	        _logWriter.write("\n");
	    } else {
                Date date = new Date();
	        DateFormat dtf = DateFormat.getDateTimeInstance(
	          DateFormat.MEDIUM, DateFormat.MEDIUM);
	        String timestamp = dtf.format(date);
	        _logWriter.write(timestamp + ": " + line + "\n");
	    }

	} catch (Exception ex) {
	    System.err.println("Error (" + ex.toString() +
	      ") writing to log file");
	}
    }

    //-------------------------------------------------------------------
    // Constructor.
    public S2DMain(String args[], boolean basic) throws S2DException
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

	checkArgs(args, basic);

	_masterName = _name;

	System.out.println("Peptide-CGI version " + PEP_CGI_VERSION);
    }

    //-------------------------------------------------------------------
    // Get the name of the data we're currently processing.
    String getName()
    {
        return _name;
    }

    //-------------------------------------------------------------------
    // Determine whether we're doing multi-entry processing.
    public boolean multiEntry()
    {
        return _extraBmrbIdList.size() > 0;
    }

    //-------------------------------------------------------------------
    // If there is another BMRB ID in the "extra" list, set the current
    // master BMRB ID to that ID and return true.  If there are no more
    // "extra" BMRB IDs, return false.
    public boolean nextBmrbId()
    {
	boolean result = false;

	if (!_fullLongName.equals("")) {
	    _fullLongName += " + ";
	}
	_fullLongName += _longName;

	//TEMP -- hmm -- should we just create a new S2DMain object here
	// to make sure everything is reset?  Or make a sub-object with
	// all the values that need to be reset?
        _bmrbIds = new Vector();
        _pdbIds = new Vector();
	_csrPdbIds = new Vector();
        _localFiles = new Vector();
	_spartaFile = null;
	_haveCoords = false;

	// Note: when doing processing combining uploaded data and a
	// BMRB entry, the summary page for the entry will be created
	// within the uvd directory, and will therefore be inaccessible.

	if (_extraBmrbIdIndex < _extraBmrbIdList.size()) {
	    _masterBmrbId =
	      (String)_extraBmrbIdList.elementAt(_extraBmrbIdIndex);

	    _name = _masterBmrbId;
	    S2DException.setName(_name);
	    _longName = "BMRB " + _name;

	    addBmrbId(_masterBmrbId);
	    if (_masterBmrbId.equals("4038")) {
	        _longName += "/4096";
	    } else if (_masterBmrbId.equals("4096")) {
	        _longName += "/4038";
	    }

	    _extraBmrbIdIndex++;

	    result = true;
	}

	return result;
    }

    //===================================================================
    // PRIVATE METHODS

    //-------------------------------------------------------------------
    public static void setTimestamps()
    {
        _startDate = new Date();
	DateFormat dtf = DateFormat.getDateTimeInstance(DateFormat.MEDIUM,
	  DateFormat.MEDIUM);
	_timestamp = dtf.format(_startDate);

	dtf = DateFormat.getDateInstance(DateFormat.MEDIUM);
	_shortTimestamp = dtf.format(_startDate);
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

	S2DNames.JS_CMD_PORT = props.getProperty("bmrb_mirror.js_cmd_port");
	if (S2DNames.JS_CMD_PORT == null) {
	    if (doDebugOutput(2)) {
	        System.out.println("bmrb_mirror.js_cmd_port " +
		  "property value not defined; using default");
	    }
	    S2DNames.JS_CMD_PORT = "6666";
	}

	S2DNames.S2PRED_URL = props.getProperty("bmrb_mirror.s2p_url");
	if (S2DNames.S2PRED_URL == null) {
	    S2DError err = new S2DError("Unable to get value for " +
	      "bmrb_mirror.s2p_url property; using default");
	    S2DNames.S2PRED_URL = "file:./";
	}

	S2DNames.DSSP_FILE_URL = props.getProperty(
	  "bmrb_mirror.dssp_file_url");
	if (S2DNames.DSSP_FILE_URL == null) {
	    S2DError err = new S2DError("Unable to get value for " +
	      "bmrb_mirror.dssp_file_url property; using default");
	    S2DNames.S2PRED_URL = "file:./";
	}

	S2DSession.getProperties(props);

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

	String s2pTmp = props.getProperty("bmrb_mirror.do_s2p_default");
	if (s2pTmp == null) {
	    System.err.println(new S2DWarning("Unable to get value for " +
	      "bmrb_mirror.do_s2p_default property"));
	} else {
	    try {
	        _s2PredLevel = Integer.parseInt(s2pTmp);
	    } catch(NumberFormatException ex) {
	        System.err.println(new S2DWarning("Error parsing " +
		  "do_s2p_default value " + ex.toString() +
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
    private void checkArgs(String args[], boolean basic) throws S2DException
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
          "    -do_dyn_movie <0|1>\n" +
          "        0: don't create movie links\n" +
          "        1: create movie links if movie exists\n" +
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
          "    -do_s2p <0|1|2>\n" +
          "        0: ignore s2predict results;\n" +
	  "        1: create links if s2predict output exists;\n" +
	  "        2: create visualizations if s2predict output exists\n" +
          "        (default is " + _s2PredLevel + ")\n" +
          "    -do_tar <0|1|2|3>\n" +
          "        0: ignore torsion angle restraint references;\n" +
          "        1: if restraint grid entry exists create links in\n" +
	  "          summary file but don't process;\n" +
          "        1: create links in summary file but don't process;\n" +
          "        3: process torsion angle restraint references\n" +
          "        (default is " + _tarLevel + ")\n" +
	  "    -dyn_movie_demo\n" +
	  "        dynamics movies are demos only\n" +
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
	  "    -s2_frame_index <value>\n" +
	  "        index of S2 order param save frame (for s2 pred)\n" +
	  "    -s2p_name <name>\n" +
	  "        name to use to look for s2predict output (if not the\n" +
	  "        normal name)\n" +
	  "    -s2p_url <url>\n" +
	  "        URL of directory containing s2predict output files\n" +
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
		if (!basic) {
		    try {
	                _cmdFrameIndex = Integer.parseInt(args[index]);
	            } catch(NumberFormatException ex) {
	                System.err.println("Error parsing frame index: " +
		          ex.toString());
	                throw new S2DError("Error parsing frame index " +
		          ex.toString());
	            }
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
		if (!basic) {
		    try {
	                _csrLevel = Integer.parseInt(args[index]);
	            } catch(NumberFormatException ex) {
	                System.err.println("Error parsing do_csr value: " +
		          ex.toString());
	                throw new S2DError("Error parsing do_csr value " +
		          ex.toString());
	            }
		}

	    } else if ("-do_dist".equals(args[index])) {
	        index++;
		if (index >= args.length) {
		    throw new S2DError("-do_dist argument needs value");
		}
		if (!basic) {
		    try {
	                _distRLevel = Integer.parseInt(args[index]);
		        // For now, we're forcing coordinate processing to
		        // happen whenever we do distance restraint processing.
		        // This should eventually be changed to not re-
		        // process coordinates if we already have them and
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
		}

	    } else if ("-do_dyn_movie".equals(args[index])) {
	        index++;
		if (index >= args.length) {
		    throw new S2DError("-do_dyn_movie argument needs value");
		}
		if (!basic) {
		    try {
	                S2DSession._dynMovieLevel =
			  Integer.parseInt(args[index]);
	            } catch(NumberFormatException ex) {
	                System.err.println("Error parsing do_dyn_movie value: " +
		          ex.toString());
	                throw new S2DError("Error parsing do_dyn_movie value " +
		          ex.toString());
	            }
		}

	    } else if ("-do_lacs".equals(args[index])) {
	        index++;
		if (index >= args.length) {
		    throw new S2DError("-do_lacs argument needs value");
		}
		if (!basic) {
		    try {
	                _lacsLevel = Integer.parseInt(args[index]);
	            } catch(NumberFormatException ex) {
	                System.err.println("Error parsing do_lacs value: " +
		          ex.toString());
	                throw new S2DError("Error parsing do_lacs value " +
		          ex.toString());
	            }
		}

	    } else if ("-do_pdb".equals(args[index])) {
	        index++;
		if (index >= args.length) {
		    throw new S2DError("-do_pdb argument needs value");
		}
		if (!basic) {
		    try {
	                _pdbLevel = Integer.parseInt(args[index]);
	            } catch(NumberFormatException ex) {
	                System.err.println("Error parsing do_pdb value: " +
		          ex.toString());
	                throw new S2DError("Error parsing do_pdb value " +
		          ex.toString());
	            }
		}

	    } else if ("-do_rrdist".equals(args[index])) {
	        index++;
		if (index >= args.length) {
		    throw new S2DError("-do_rrdist argument needs value");
		}
		if (!basic) {
		    try {
	                _rrDistRLevel = Integer.parseInt(args[index]);
	            } catch(NumberFormatException ex) {
	                System.err.println("Error parsing do_rrdist value: " +
		          ex.toString());
	                throw new S2DError("Error parsing do_rrdist value " +
		          ex.toString());
	            }
		}

	    } else if ("-do_rrtar".equals(args[index])) {
	        index++;
		if (index >= args.length) {
		    throw new S2DError("-do_rrtar argument needs value");
		}
		if (!basic) {
		    try {
	                _rrTarLevel = Integer.parseInt(args[index]);
	            } catch(NumberFormatException ex) {
	                System.err.println("Error parsing do_rrtar value: " +
		          ex.toString());
	                throw new S2DError("Error parsing do_rrtar value " +
		          ex.toString());
	            }
		}

	    } else if ("-do_s2p".equals(args[index])) {
	        index++;
		if (index >= args.length) {
		    throw new S2DError("-do_s2p argument needs value");
		}
		if (!basic) {
		    try {
	                _s2PredLevel = Integer.parseInt(args[index]);
		        // For now, we're forcing coordinate processing to
		        // happen whenever we do s2 experimental vs. predicted
		        // processing.  This should eventually be changed to
		        // not re-process coordinates if we already have them
		        // and they're up-to-date.
		        if (_s2PredLevel == S2PRED_LEVEL_PROCESS) {
		            _pdbLevel = PDB_LEVEL_PROCESS;
		        }
	            } catch(NumberFormatException ex) {
	                System.err.println("Error parsing do_s2p value: " +
		          ex.toString());
	                throw new S2DError("Error parsing do_s2p value " +
		          ex.toString());
	            }
		}

	    } else if ("-do_tar".equals(args[index])) {
	        index++;
		if (index >= args.length) {
		    throw new S2DError("-do_tar argument needs value");
		}
		if (!basic) {
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
		}

	    } else if ("-dyn_movie_demo".equals(args[index])) {
		S2DSession._dynMovieDemo = true;

	    } else if ("-file".equals(args[index])) {
	        index++;
		if (index >= args.length) {
		    throw new S2DError("-file argument needs value");
		}
		_localFiles.addElement(args[index]);
		_originalLocalFiles.addElement(args[index]);

	    } else if ("-force".equals(args[index])) {
		if (!basic) {
		    _force = true;
		}

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
		if (!basic) {
		    String url = "file:" + args[index];
	            _cmdPdbId = url;
		    _pdbIds.addElement(url);
		    // ChemShift needs changes to work with local file.
		    // _csrPdbIds.addElement(_cmdPdbId);
		    _pdbLevel = PDB_LEVEL_PROCESS;
		}

	    } else if ("-pdbid".equals(args[index])) {
	        index++;
		if (index >= args.length) {
		    throw new S2DError("-pdbid argument needs value");
		}
		if (!basic) {
	            _cmdPdbId = args[index];
		    _cmdPdbId = _cmdPdbId.toUpperCase();
		    _pdbIds.addElement(_cmdPdbId);
		    _csrPdbIds.addElement(_cmdPdbId);
		}

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

	    } else if ("-s2_frame_index".equals(args[index])) {
	        index++;
		if (index >= args.length) {
		    throw new S2DError("-s2_frame_index argument needs value");
		}
		try {
	            _s2FrameIndex = Integer.parseInt(args[index]);
	        } catch(NumberFormatException ex) {
	            System.err.println("Error parsing s2_frame_index: " +
		      ex.toString());
	            throw new S2DError("Error parsing s2_frame_index " +
		      ex.toString());
	        }

	    } else if ("-s2p_name".equals(args[index])) {
	        index++;
		if (index >= args.length) {
		    throw new S2DError("-s2p_name argument needs value");
		}
		_s2PName = args[index];

	    } else if ("-s2p_url".equals(args[index])) {
	        index++;
		if (index >= args.length) {
		    throw new S2DError("-s2p_url argument needs value");
		}
		S2DNames.S2PRED_URL = args[index];

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

	    } else if ("-xbmrbid".equals(args[index])) {
	        index++;
		if (index >= args.length) {
		    throw new S2DError("-xbmrbid argument needs value");
		}
		if (!basic) {
		    if (_name.equals(args[index]) ||
		      _extraBmrbIdList.contains(args[index])) {
		        _noMail = true;
		        throw new S2DError("-xbmrbid value " + args[index] +
		          " duplicates previous value");
		    }
		    _extraBmrbIdList.addElement(args[index]);
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
	//TEMP -- does this duplicate the similar code where we
	// actually handle the arguments?
        if (_tarLevel == TAR_LEVEL_PROCESS ||
	  _rrTarLevel == RRTAR_LEVEL_PROCESS ||
	  _s2PredLevel == S2PRED_LEVEL_PROCESS) {
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

	if (_s2PredLevel < S2PRED_LEVEL_NONE ||
	  _pdbLevel > S2PRED_LEVEL_PROCESS) {
	    throw new S2DError("illegal do_s2p value: " + _s2PredLevel +
	      " (must be between " + S2PRED_LEVEL_NONE + " and " +
	      S2PRED_LEVEL_PROCESS + ")");
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
	    for (int xid = 0; xid < _extraBmrbIdList.size(); xid++) {
	        System.out.println("_extraBmrbId = " +
		  (String)_extraBmrbIdList.elementAt(xid));
	    }
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
	    System.out.println("_s2PredLevel = " + _s2PredLevel);
	    System.out.println("S2DNames.S2PRED_URL = " +
	      S2DNames.S2PRED_URL);
	    System.out.println("S2DSession._dynMovieLevel = " +
	      S2DSession._dynMovieLevel);
	    System.out.println("S2DSession._dynMovieUrl = " +
	      S2DSession._dynMovieUrl);
	    System.out.println("S2DSession._dynMovieGenUrl = " +
	      S2DSession._dynMovieGenUrl);
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
	      _name, _htmlDir, _masterBmrbId);
            if (!summaryFileOk) {
		if (doDebugOutput(2)) {
		    System.out.println("Error getting summary file data; " +
		      "cache not used");
		}
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
	      _rrTarLevel == RRTAR_LEVEL_PROCESS ||
	      _s2PredLevel == S2PRED_LEVEL_PROCESS) {
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

		//TEMP -- check for s2Pred stuff here?
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
	    _summary = new S2DSummaryHtml(tmpSummaryFile, tmpSummaryFile,
	      _longName, id, _localFiles, _htmlDir, "starFileName",
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

	    //
	    // Do the s2predict processing.
	    //
	    if (_s2PredLevel == S2PRED_LEVEL_PROCESS) {
		process1S2Pred(_cmdPdbId, _cmdFrameIndex, _s2FrameIndex);
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

	    _summary = new S2DSummaryHtml(_name, _name, _longName,
	      _masterBmrbId, _originalLocalFiles, _htmlDir,
	      masterStar.getFileName(), masterStar.getSystemName(),
	      masterStar.getEntryTitle(), _restraintOnly);
	} else {
	    _summary = new S2DSummaryHtml(_name, _name, _longName,
	      "99999", _originalLocalFiles, _htmlDir, _name, ""/*TEMP?*/,
	      ""/*TEMP?*/, _restraintOnly);
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
	// Process any applicable s2predict output.
	//
        processAllS2Pred();

	//
	// Initiate chemical shift reference processing, if we didn't
	// before and got a PDB ID from the NMR-STAR file.  (This is
	// in case intermediate processing steps, such as the s2predict
	// processing, added more PDB IDs.)
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
    // Process all specified BMRB entries.  Note: this only applies to
    // two entries that contain data for the same sample (e.g., 4038/
    // 4096), not in the case of two-entry visualizations in general.
    private S2DNmrStarIfc processAllBMRBs(S2DNmrStarIfc masterStar)
      throws S2DException
    {
	if (doDebugOutput(0)) {
	    System.out.println("S2DMain.processAllBMRBs()");
	}

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
    // Process all s2predict output files that apply to this data set.
    private void processAllS2Pred()
    {
        if (doDebugOutput(2)) {
	    System.out.println("processAllS2Pred()");
	}

	// Find any s2predict output files that apply to this data set.
	Vector s2PredData = new Vector();
	try {
	    String name = _s2PName != null ? _s2PName : _name;
	    s2PredData = S2DS2Pred.FindData(name);
        } catch(S2DException ex) {
	    System.err.println("Error (" + ex.toString() +
	      ") finding s2predict output files");
	}

	// If the s2predict PDB ID exists already in the list of PDB
	// IDs for this entry, we want to match the coordinate index
	// to the existing one; otherwise we give it a coordinate
	// index one past the last of the pre-existing ones.
	for (int index = 0; index < s2PredData.size(); index++) {
	    S2DS2Pred.S2PData data =
	      (S2DS2Pred.S2PData)s2PredData.elementAt(index);
	    int coordIndex = 0;
	    for (int pdbIndex = 0; pdbIndex < _pdbIds.size(); pdbIndex++) {
	        if (((String)_pdbIds.elementAt(pdbIndex)).equals(
		  data._pdbId.toUpperCase())) {
		    coordIndex = pdbIndex + 1;
		    break;
		}
	    }
	    if (coordIndex == 0) {
		// If we get here, the PDB ID is not in the list in the
		// actual NMR-STAR file.  We need to add it to the PDB
		// list so coordinates get processed for it.
	        coordIndex = _pdbIds.size() + index + 1;
		Vector pdbs = new Vector();
		pdbs.add(data._pdbId);
		addPDB(pdbs);
	    }
	    try {
	        process1S2Pred(data._pdbId, coordIndex, data._frameIndex);
	    } catch(S2DException ex) {
	        System.err.println("Error (" + ex.toString() +
		  ") processing s2predict data for " + data._pdbId +
		  ", " + coordIndex + ", " + data._frameIndex);
	    }
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
	if (doDebugOutput(0)) {
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
	  "After saveSpartaDeltaShifts()");

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
	      "Residue_seq_code; ResLabel; ResLabelSh\n");
	    resWriter.write("# Peptide-CGI version: " +
	      S2DMain.PEP_CGI_VERSION + "\n");
	    resWriter.write("# Generation date: " +
	      S2DMain.getTimestamp() + "\n");
	    resWriter.write("#\n");

	    int entityAssemblyId = 0;
	    for (int frameNum = 0; frameNum < frames.size(); frameNum++) {
	        entityAssemblyId++;
	        SaveFrameNode frame = (SaveFrameNode)frames.elementAt(frameNum);
                if (doDebugOutput(5)) {
		    System.out.println("Writing residue list for save frame "
		      + star.getFrameName(frame));
	        }	

		// Don't do anything for non-polymers.
		int polymerType = star.getPolymerType(frame);
		if (polymerType == S2DResidues.POLYMER_TYPE_NONE) {
                    // if (doDebugOutput(5)) {
		        System.out.println("Skipping residue list for " +
			  "save frame " + star.getFrameName(frame) +
			  " because it is a non-polymer");
		    // }
		    continue;
		}

		try {
	            S2DResidues residues = star.getResidues(frame);

	            for (int resNum = 0; resNum < residues._resSeqCodes.length;
	              resNum++) {
	                resWriter.write(entityAssemblyId + "\t" +
		          residues._resSeqCodes[resNum] + "\t" +
		          residues._resLabels[resNum] + "\t" +
			  residues._resLabelsSh[resNum] + "\n");
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
                    // if (doDebugOutput(2)) {
                        System.out.println("Chemical shifts not saved for " +
                          "save frame " + frame.getLabel() + " (" +
			  entityAssemblyID + ") because it is not a polymer");
                    // }
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
		    maxModelNum = S2DSpartaChemShift.getSpartaMaxModelNum(
		      spartaStar, frame, entityAssemblyID, chemShiftListID);
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
    // Create the multi-entry sessions (so far, at least, they just
    // use existing data from the individual entries).
    private void createMultiEntrySessions() throws S2DException
    {
        if (doDebugOutput(3)) {
	    System.out.println("S2DMain.createMultiEntrySessions()");
	}

	logLine("Creating multi-entry sessions");

    	if (_extraBmrbIdList.size() > 1) {
	    throw new S2DError("Too many 'extra' BMRB IDs " +
	      "specified -- current max is 1");
	}

	String extraId = (String)_extraBmrbIdList.elementAt(0);

	String fullName = _masterName + "+" + extraId;

	//TEMP -- we may want to eventually change these...
	String systemName = "";
	String frameTitle = "";
	S2DSummaryHtml summary2 = new S2DSummaryHtml(_masterName,
	  fullName, _fullLongName, _masterBmrbId, extraId,
	  _originalLocalFiles, _htmlDir, "", systemName, frameTitle, false);

        S2DProteinChemShift.writeMultiEntrySessions(_masterName,
	  fullName, extraId, _sessionDir, _htmlDir, summary2);

	//TEMP -- we may eventually want to pass something here for
	// related BMRB IDs...
	summary2.close(null, null, null);
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
	// Create an S2DRelaxation object.
	//
	S2DRelaxation relaxation = new S2DRelaxation(_name, _longName,
	  star, frame, _dataDir, _sessionDir, _summary,
	  S2DUtils.TYPE_T1_RELAX, entityAssemblyID);

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
	// Create an S2DRelaxation object.
	//
	S2DRelaxation relaxation = new S2DRelaxation(_name, _longName,
	  star, frame, _dataDir, _sessionDir, _summary,
	  S2DUtils.TYPE_T2_RELAX, entityAssemblyID);

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
	// Create an S2DCoupling object.
	//
        S2DCoupling coupling = new S2DCoupling(_name, _longName, star,
	  frame, _dataDir, _sessionDir, _summary, entityAssemblyID);
	  
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
	// Create an S2DHetNOE object.
	//
	S2DHetNOE hetNOE = new S2DHetNOE(_name, _longName, star, frame,
	  _dataDir, _sessionDir, _summary, entityAssemblyID);

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
	// Create an S2DS2Order object.
	//
	S2DS2Order s2Order = new S2DS2Order(_name, _longName, star,
	  frame, _dataDir, _sessionDir, _summary, entityAssemblyID);

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

	try {
	   //
	   // Create an S2DPistachio object.
	   //
           S2DPistachio pistachio = new S2DPistachio(_name, star, frame,
	     _dataDir, _sessionDir, _summary, entityAssemblyID, residues);

	   //
	   // The S2DPistachioTable object holds the figures of merit for
	   // each atom, for the "color by figure of merit" option.  This
	   // is passed along when we write out the coordinates so that the
	   // figure of merit values and corresponding color strings can also
	   // be written along with the coordinates.
	   //
	   S2DPistachioTable pt = pistachio.createPistachioTable();

	   //
	   // Create an S2DDummyCoords object and write out the
	   // coordinates.
	   //
	   int entityAssemblyIDVal = star.getEntityAssemblyID(frame,
	     entityAssemblyID);
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
	} catch (S2DCancel cancel) {
            if (doDebugOutput(4)) {
	        System.out.println("Pistachio processing cancelled: " +
		  cancel.toString());
	    }
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

	try {
	    //
	    // Create an S2DAmbiguity object.
	    //
            S2DAmbiguity ambiguity = new S2DAmbiguity(_name, star, frame,
	      _dataDir, _sessionDir, _summary, entityAssemblyID, residues);

	    //
	    // The S2DAmbiguityTable object holds the ambiguity values for
	    // each atom, for the "color by ambiguity value" option.  This
	    // is passed along when we write out the coordinates so that the
	    // ambiguity values and corresponding color strings can also be
	    // written along with the coordinates.
	    //
	    S2DAmbiguityTable at = ambiguity.createAmbiguityTable();

	    //
	    // Create an S2DDummyCoords object and write out the
	    // coordinates.
	    //
	    int entityAssemblyIDVal = star.getEntityAssemblyID(frame,
	      entityAssemblyID);
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
	} catch (S2DCancel cancel) {
            if (doDebugOutput(4)) {
	        System.out.println("Ambiguity code processing cancelled: " +
		  cancel.toString());
	    }
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

	//
	// Create an S2DLacs object.
	//
	S2DLacs lacs = new S2DLacs(_name, _longName, star, frame,
	  _dataDir, _sessionDir, _summary);

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
    // Do s2predict processing for one PDB/S2 save frame combination.
    private void process1S2Pred(String pdbId, int coordIndex,
      int s2FrameIndex) throws S2DException
    {
        if (doDebugOutput(2)) {
	    System.out.println("process1S2Pred(" + pdbId + ", " +
	      coordIndex + ", " + s2FrameIndex + ")");
	    System.out.println("_s2PredLevel: " + _s2PredLevel);
	}

	if (_s2PredLevel == S2PRED_LEVEL_LINK) {
	    try {
	        _summary.writeS2PredCGI(pdbId, coordIndex, s2FrameIndex);
	    } catch(IOException ex) {
                System.err.println(
		  "IOException writing predicted S2: " +
		  ex.toString());
	        throw new S2DError("Can't predicted S2");
	    } finally {
	        _summary.endFrame();
	    }

	} else if (_s2PredLevel == S2PRED_LEVEL_PROCESS) {
	    // s2predict only works for single-chain molecules.  wenger
	    // 2011-04-18
	    String entityAssemblyId = "1";
	    String name = _s2PName != null ? _s2PName : _name;
	    S2DS2Pred s2Pred = new S2DS2Pred(_name, _longName,
	      pdbId, coordIndex, s2FrameIndex, _dataDir, _sessionDir,
	      _summary, entityAssemblyId, name);

	    s2Pred.writeS2Pred();

	    S2DSecStruct ss = new S2DSecStruct(_name, 
	      pdbId, coordIndex, s2FrameIndex, _dataDir);

	    ss.writeSecStruct();

	} else {
	    throw new S2DError("Illegal _s2PredLevel value: " +
	      _s2PredLevel);
	}
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

/*
  ========================================================================
  DEVise Data Visualization Software
  (c) Copyright 1992-2001
  By the DEVise Development Group
  Madison, Wisconsin
  All Rights Reserved.
  ========================================================================

  Under no circumstances is this software to be copied, distributed,
  or altered in any way without prior permission from the DEVise
  Development Group.
*/

/*
  $Id$

  $Log$
  Revision 1.149  2001/02/20 20:02:55  wenger
  Merged changes from no_collab_br_0 thru no_collab_br_2 from the branch
  to the trunk.

  Revision 1.148.2.1  2001/02/16 21:38:00  wenger
  Updated DEVise version to 1.7.2; implemented 'forward' and 'back' (like
  a web browser) on 'sets' of visual filters.

  Revision 1.148  2001/01/08 20:32:54  wenger
  Merged all changes thru mgd_thru_dup_gds_fix on the js_cgi_br branch
  back onto the trunk.

  Revision 1.143.2.3  2001/01/05 20:49:49  wenger
  Merged changes from zero_js_cache_check thru dup_gds_fix from the trunk
  onto the js_cgi_br branch.

  Revision 1.147  2000/12/27 22:22:31  wenger
  Fixed 'duplicate GDataSock' problem caused by GDataSocks not getting
  destroyed when queries are aborted; added a little more debug output.

  Revision 1.143.2.2  2000/12/27 19:39:21  wenger
  Merged changes from js_restart_improvements thru zero_js_cache_check from
  the trunk onto the js_cgi_br branch.

  Revision 1.146  2000/11/17 22:59:06  wenger
  Fixed problems with command logging of cursor movements and pile/stack
  flips.

  Revision 1.143.2.1  2000/10/18 20:32:16  wenger
  Merged changes from fixed_bug_616 through link_gui_improvements onto
  the branch.

  Revision 1.145  2000/10/16 16:11:47  wenger
  Link creation GUI now gives a choice of positive or negative record
  links; fixed bug 622 (record link type not saved in session files);
  fixed bug 623 (record link update problem); other link-related cleanups.

  Revision 1.144  2000/10/13 19:58:31  wenger
  Got rid of GDataSocket creation errors in JavaScreen support.

  Revision 1.143  2000/07/12 20:53:42  wenger
  Added more debug code.

  Revision 1.142  2000/04/06 19:43:54  wenger
  Fixed bug 577 'c' bypasses cursor grid, etc.

  Revision 1.141  2000/03/28 17:55:57  wenger
  Oops!  Used 'f' and 'F' for flip, so 'c' or 'C' now makes cursor fill
  destination view (slightly different than before -- it previously did
  a home on the source view, but I think filling the dest. view is better).

  Revision 1.140  2000/03/14 21:51:49  wenger
  Added more invalid object checking; had to take some memory checking
  out of client-side stuff for linking reasons.

  Revision 1.139  2000/03/14 17:05:35  wenger
  Fixed bug 569 (group/ungroup causes crash); added more memory checking,
  including new FreeString() function.

  Revision 1.138  2000/03/10 16:32:00  wenger
  Found and fixed bug 572 (problem with switching stations in ASOS and
  AWON sessions).

  Revision 1.137  2000/02/24 18:50:30  wenger
  F/f ("full" cursor) in a view does home on the source views of any cursors
  for which the selected view is the destination view.

  Revision 1.136  2000/02/16 18:51:47  wenger
  Massive "const-ifying" of strings in ClassDir and its subclasses.

  Revision 1.135  2000/02/15 16:16:24  wenger
  Cursors in child views "remember" their size and location when
  switching TDatas or parent attributes.

  Revision 1.134  2000/02/09 21:29:40  wenger
  Fixed bug 562 (one problem with pop clip underflow, related to highlight
  views).

  Revision 1.133  1999/12/15 16:25:50  wenger
  Reading composite file /afs/cs.wisc.edu/p/devise/ext5/wenger/devise.dev2/solarisFixed bugs 543 and 544 (problems with cursor movement).

  Revision 1.132  1999/12/14 20:33:59  wenger
  Rubberband lines aren't allowed to go outside of data area; rubberband
  lines are not drawn in views in which zooming is disabled.

  Revision 1.131  1999/11/19 17:17:41  wenger
  Added View::SetVisualFilterCommand() method to clean up command-related
  code for filter setting.

  Revision 1.130  1999/11/18 22:49:19  wenger
  Fixed bug Omer ran into (removing a mapping from a view while a query is
  running in that view causes DEVise to crash).

  Revision 1.129  1999/10/30 19:08:11  wenger
  Kludgey fix for bug 527 (problems with Condor/UserWeek.ds session).

  Revision 1.128  1999/10/12 17:59:39  wenger
  Fixed bug in code for checking if the mouse is on a cursor that caused
  devised to crash with JavaScreen; fixed Dispatcher problem that sometimes
  caused core dump when DEVise is killed with INT signals; WindowRep
  remembers last cursor hit type to avoid changing the mouse cursor unless
  really necessary.

  Revision 1.127  1999/10/08 22:04:42  wenger
  Fixed bugs 512 and 514 (problems related to cursor moving).

  Revision 1.126  1999/10/08 19:57:57  wenger
  Fixed bugs 470 and 513 (crashes when closing a session while a query
  is running), 510 (disabling actions in piles), and 511 (problem in
  saving sessions); also fixed various problems related to cursors on
  piled views.

  Revision 1.125  1999/10/05 17:55:51  wenger
  Added debug log level.

  Revision 1.124  1999/09/08 20:56:29  wenger
  Removed all Tcl dependencies from the devised (main changes are in the
  Session class); changed version to 1.6.5.

  Revision 1.123  1999/08/17 19:47:03  wenger
  Converted Condor UserMonth session from high/low symbols to piles for
  better representation of data; fixed some cursor/pile drawing bugs and
  TData switching bugs that I found in the process.

  Revision 1.122  1999/08/12 16:03:56  wenger
  Implemented "inverse" zoom -- alt-drag zooms out instead of in.

  Revision 1.121  1999/08/10 20:13:03  wenger
  Added code for "inverse" zoom (currently disabled).

  Revision 1.120  1999/08/05 21:42:51  wenger
  Cursor improvements: cursors can now be dragged in "regular" DEVise;
  cursors are now drawn with a contrasting border for better visibility;
  fixed bug 468 (cursor color not working).

  Revision 1.119  1999/07/16 21:36:09  wenger
  Changes to try to reduce the chance of devised hanging, and help diagnose
  the problem if it does: select() in Server::ReadCmd() now has a timeout;
  DEVise stops trying to connect to Tasvir after a certain number of failures,
  and Tasvir commands are logged; errors are now logged to debug log file;
  other debug log improvements.  Changed a number of 'char *' declarations
  to 'const char *'.

  Revision 1.118  1999/06/30 17:38:50  wenger
  Data color of parent view's mapping (if specified) now controls the
  background color of view symbols; defined constant strings for GData
  attribute values to avoid potential problems.

  Revision 1.117  1999/06/25 15:58:25  wenger
  Improved debug logging, especially for JavaScreen support: JavaScreenCmd.C
  now uses DebugLog facility instead of printf; dispatcher logging is turned
  on by default, and commands and queries are logged; added -debugLog command
  line flag to turn logging on and off.

  Revision 1.116  1999/06/11 14:47:07  wenger
  Added the capability (mostly for the JavaScreen) to disable rubberband
  lines, cursor movement, drill down, and key actions in views (the code
  to send this info to the JS is still conditionaled out until the JS is
  ready for it).

  Revision 1.115  1999/06/04 16:32:31  wenger
  Fixed bug 495 (problem with cursors in piled views) and bug 496 (problem
  with key presses in piled views in the JavaScreen); made other pile-
  related improvements (basically, I removed a bunch of pile-related code
  from the XWindowRep class, and implemented that functionality in the
  PileStack class).

  Revision 1.114  1999/05/28 16:32:47  wenger
  Finished cleaning up bounding-box-related code except for PolyLineFile
  symbol type; fixed bug 494 (Vector symbols drawn incorrectly); improved
  drawing of Polyline symbols.

  Revision 1.113  1999/05/21 14:52:52  wenger
  Cleaned up GData-related code in preparation for including bounding box
  info.

  Revision 1.112  1999/05/17 20:55:21  wenger
  Partially-kludged fix for bug 488 (problems with cursors in piled views
  in the JavaScreen).

  Revision 1.111  1999/05/17 18:38:00  wenger
  Views now have GData sending configuration that is only employed when
  connecting to the JavaScreen (eliminates the need for the current kludgey
  setup to send GData to the JS).

  Revision 1.110  1999/05/14 16:46:51  wenger
  View vertical scroll can now be configured by the user.

  Revision 1.109  1999/05/12 21:01:58  wenger
  Views containing view symbols can now be piled.

  Revision 1.108  1999/05/04 17:17:09  wenger
  Merged js_viewsyms_br thru js_viewsyms_br_1 (code for new JavaScreen
  protocol that deals better with view symbols).

  Revision 1.107  1999/04/26 20:25:09  wenger
  Fixed bug 486.

  Revision 1.106  1999/04/22 19:29:53  wenger
  Separated the configuration of explicit (user-requested) and implicit
  home actions (no GUI for configuring the implicit home); changed the
  Condor user script accordingly; modified JavaScreen support so that this
  all works for the JS.

  Revision 1.105  1999/04/20 14:13:38  wenger
  Improved debug output.

  Revision 1.104  1999/04/16 20:59:24  wenger
  Fixed various bugs related to view symbols, including memory problem
  with MappingInterp dimensionInfo; updated create_condor_session script
  to take advantage of view symbol TData switching capability.

  Revision 1.103  1999/04/16 20:21:35  wenger
  Fixed bug 482 (problem with automatic view home).

  Revision 1.102  1999/04/14 15:30:19  wenger
  Improved 'switch TData': moved the code from Tcl to C++, functionality
  is more flexible -- schemas don't have to match exactly as long as the
  appropriate TData attributes are present; TData can now be specified for
  view symbols in parent view mapping; updated shape help.

  Revision 1.101  1999/04/05 21:09:47  wenger
  Fixed bug 476 ('home' on a visually-linked view now does home on the entire
  link as a unit) (removed the corresponding code from the PileStack class,
  since the pile link now takes care of this automatically).

  Revision 1.100  1999/04/05 16:16:02  wenger
  Record- and set-link follower views with auto filter update enabled have
  'home' done on them after they are updated by a record link or set link.

  Revision 1.99  1999/03/24 17:26:12  wenger
  Non-DTE data source code prevents adding duplicate data source names;
  added "nice axis" feature (sets axis limits to multiples of powers of
  10 if enabled); improved the propagation of DEVise errors back to the
  GUI; fixed bug 474 (problem with view home).

  Revision 1.98  1999/03/16 21:47:58  wenger
  '5' (home) key now works properly on linked piles -- does home on the
  entire pile as a unit.

  Revision 1.97  1999/03/16 17:10:17  wenger
  Improved 'view home' configuration: user can select whether home changes
  X, Y, or both parts of visual filter; added explicit option to force Y
  min to 0 in automatic mode; fixed bug 469 (problems with 'home' in
  record link follower views).

  Revision 1.96  1999/03/12 18:46:05  wenger
  Implemented duplicate symbol elimination.

  Revision 1.95  1999/03/03 18:22:04  wenger
  Fixed bugs 426 and 432 (problems with '5' (home) key); fixed bugs 466
  and 467 (query errors with sorted attributes); minor improvements to
  view symbols.

  Revision 1.94.4.1  1999/04/29 17:46:04  wenger
  Minor improvements to debug output.

  Revision 1.94  1998/12/22 19:39:30  wenger
  User can now change date format for axis labels; fixed bug 041 (axis
  type not being changed between float and date when attribute is changed);
  got dates to work semi-decently as Y axis labels; view highlight is now
  always drawn by outlining the view; fixed some bugs in drawing the highight.

  Revision 1.93  1998/12/01 20:04:33  wenger
  More reductions of memory usage in DEVise -- basically eliminated the
  histogram capability (this really saves a lot, since there are big
  structures in every ViewGraph for this); made creation of TDatas more
  efficient by bypassing command code.

  Revision 1.92  1998/11/11 14:31:03  wenger
  Implemented "highlight views" for record links and set links; improved
  ClassDir::DestroyAllInstances() by having it destroy all links before
  it destroys anything else -- this cuts down on propagation problems as
  views are destroyed; added test code for timing a view's query and draw.

  Revision 1.91  1998/11/09 20:33:27  wenger
  Fixed bug 433 (drill-down problem); improved debug output in various
  related modules.

  Revision 1.90  1998/11/06 17:59:54  wenger
  Multiple string tables fully working -- allows separate tables for the
  axes in a given view.

  Revision 1.89  1998/11/04 20:34:04  wenger
  Multiple string tables partly working -- loading and saving works, one
  table per mapping works; need multiple tables per mapping, API and GUI,
  saving to session, sorting.

  Revision 1.88  1998/11/03 18:27:08  wenger
  Partial fix to bugs 426 and 432.

  Revision 1.87  1998/10/29 21:46:09  wenger
  Added "proof-of-concept" code for filter links; added warning when
  more than one mapping is inserted into a view; updated bug and to-do
  lists.

  Revision 1.86  1998/10/21 17:16:53  wenger
  Fixed bug 101 (problems with the '5' (home) key); added "Set X, Y to
  Show All" (go home) button to Query dialog; fixed bug 421 (crash when
  closing set link sessions); fixed bug 423 (saving session file over
  directory).

  Revision 1.85  1998/10/20 19:46:18  wenger
  Mapping dialog now displays the view's TData name; "Next in Pile" button
  in mapping dialog allows user to edit the mappings of all views in a pile
  without actually flipping them; user has the option to show all view names;
  new GUI to display info about all links and cursors; added API and GUI for
  count mappings.

  Revision 1.84  1998/10/01 17:54:18  wenger
  Implemented the sending of GData to the JavaScreen.

  Revision 1.83  1998/09/08 20:26:19  wenger
  Added option to save which view is selected when saving a session -- for
  JavaScreen client switching support.

  Revision 1.82  1998/07/30 15:31:23  wenger
  Fixed bug 381 (problem with setting master and slave of a link to the same
  view); generally cleaned up some of the master-slave link related code.

  Revision 1.81  1998/05/06 22:05:02  wenger
  Single-attribute set links are now working except where the slave of
  one is the master of another.

  Revision 1.80  1998/05/05 15:15:20  zhenhai
  Implemented 3D Cursor as a rectangular block in the destination view
  showing left, right, top, bottom, front and back cutting planes of the
  source view.

  Revision 1.79  1998/04/30 14:24:23  wenger
  DerivedTables are now owned by master views rather than links;
  views now unlink from master and slave links in destructor.

  Revision 1.78  1998/04/29 17:53:59  wenger
  Created new DerivedTable class in preparation for moving the tables
  from the TAttrLinks to the ViewDatas; found bug 337 (potential big
  problems) while working on this.

  Revision 1.77  1998/04/28 18:03:21  wenger
  Added provision for "logical" and "physical" TDatas to mappings,
  instead of creating new mappings for slave views; other TAttrLink-
  related improvements.

  Revision 1.76  1998/04/10 18:29:33  wenger
  TData attribute links (aka set links) mostly implemented through table
  insertion; a crude GUI for creating them is implemented; fixed some
  bugs in link GUI; changed order in session file for TData attribute links.

  Revision 1.75  1998/04/01 17:11:34  wenger
  4/left arrow, 5 (home), and 6/right arrow keys, and cursor movements
  now get sent to slaves during collaboration.

  Revision 1.74  1998/03/18 08:20:17  zhenhai
  Added visual links between 3D graphics.

  Revision 1.73  1998/03/08 00:01:14  wenger
  Fixed bugs 115 (I think -- can't test), 128, and 311 (multiple-link
  update problems) -- major changes to visual links.

  Revision 1.72  1998/03/05 08:10:56  zhenhai
  Added ability to view 3D graphs from six directions. Use -gl option to run,
  and click key x,y,z and X,Y,Z to select the direction you are viewing.
  Use arrow keys to pan left right up and down.

  Revision 1.71  1998/02/26 22:59:57  wenger
  Added "count mappings" to views, except for API and GUI (waiting for
  Dongbin to finish his mods to ParseAPI); conditionaled out unused parts
  of VisualFilter struct; did some cleanup of MappingInterp class.

  Revision 1.70  1998/02/24 22:55:26  beyer
  Fixed histogram session restore bug.

  Revision 1.69  1998/02/20 06:17:15  beyer
  resurected histograms

  Revision 1.68  1998/02/13 15:51:38  wenger
  Changed ViewData to be based on old ViewScatter class instead of
  TDataViewX; ViewData now returns a list of the records drawn to
  the query processor; removed unused GDataBinX class.

  Revision 1.67  1998/02/10 21:13:15  wenger
  Changed signatures of ReturnGData() in QueryCallback and its subclasses
  to pass back lists of records drawn (not implemented yet); moved
  declaration of ViewGraph_QueryCallback from ViewGraph.h to ViewGraph.c.

  Revision 1.66  1998/01/27 23:04:27  wenger
  Broke the server's view selection dependency on the client (except when
  running in collaboration mode).

  Revision 1.65  1998/01/14 16:39:23  wenger
  Merged cleanup_1_4_7_br_6 thru cleanup_1_4_7_br_7.

  Revision 1.64  1997/12/12 05:50:45  weaver
  *** empty log message ***

  Revision 1.63  1997/11/24 23:15:23  weaver
  Changes for the new ColorManager.

  Revision 1.62  1997/11/24 16:22:29  wenger
  Added GUI for saving GData; turning on GData to socket now forces
  redraw of view; GData to socket params now saved in session files;
  improvement to waitForQueries command.

  Revision 1.61  1997/11/18 23:27:05  wenger
  First version of GData to socket capability; removed some extra include
  dependencies; committed test version of TkControl::OpenDataChannel().

  Revision 1.60.4.1  1998/01/12 20:34:01  wenger
  Fixed duplicate frees in multi that caused core dump on Linux.

  Revision 1.60  1997/08/28 18:21:13  wenger
  Moved duplicate code from ViewScatter, TDataViewX, and ViewLens classes
  up into ViewGraph (parent class).

  Revision 1.59  1997/08/20 22:11:14  wenger
  Merged improve_stop_branch_1 through improve_stop_branch_5 into trunk
  (all mods for interrupted draw and user-friendly stop).

  Revision 1.58.2.1  1997/08/15 23:06:33  wenger
  Interruptible drawing now pretty much working for TDataViewX class,
  too (connector drawing may need work, needs a little more testing).
  (Some debug output still turned on.)

  Revision 1.58  1997/06/18 21:06:30  wenger
  Fixed problems saving to batch scripts.

  Revision 1.57  1997/06/10 19:22:08  wenger
  Removed (some) debug output.

  Revision 1.56  1997/06/05 21:08:41  wenger
  User-configurability of '4', '5', and '6' keys is now completed.

  Revision 1.55  1997/05/30 15:41:21  wenger
  Most of the way to user-configurable '4', '5', and '6' keys -- committing
  this stuff now so it doesn't get mixed up with special stuff for printing
  Mitre demo.

  Revision 1.54.6.1  1997/05/21 20:40:51  weaver
  Changes for new ColorManager

  Revision 1.54  1997/04/30 21:45:40  wenger
  Fixed non-constant strings in complex mappings bug; TDataAsciiInterp
  no longer gives warning message on blank data lines; added makefile
  targets to make a Purify'd version of multi; fixed uninitialized memory
  read in the DList code; fixed bug that caused 1.4 version of multi to
  always crash; better error messages in DTE command parser; version is
  now 1.4.4.

  Revision 1.53  1997/04/21 23:01:29  guangshu
  Considered various cases of statistics, esp date type.

  Revision 1.52  1997/04/11 18:49:16  wenger
  Added dashed line support to the cslib versions of WindowReps; added
  option to not maintain aspect ratio in Tasvir images; re-added shape
  help file that somehow didn't get added in 1.3 merges; removed code
  for displaying GIFs locally (including some of the xv code).

  Revision 1.51  1997/03/28 16:10:28  wenger
  Added headers to all source files that didn't have them; updated
  solaris, solsparc, and hp dependencies.

  Revision 1.50  1997/03/21 23:57:41  guangshu
  Write null records to statBuf when there is no record in the view to
  make the dataSourceBuf valid.

  Revision 1.49  1997/03/20 22:24:55  guangshu
  Enhanced statistics to support user specified number of buckets in histograms,
  group by X and Y, support for date type in group by.

  Revision 1.48  1997/03/19 19:41:52  andyt
  Embedded Tcl/Tk windows are now sized in data units, not screen pixel
  units. The old list of ETk window handles (WindowRep class) has been
  replaced by a list of ETkInfo structs, each with fields for the window
  handle, x-y coordinates, name of the Tcl script, and an "in_use"
  flag. Added an ETk_Cleanup() procedure that gets called inside
  View::ReportQueryDone and destroys all ETk windows that are not marked
  as in_use.

  Revision 1.47  1997/02/26 16:31:49  wenger
  Merged rel_1_3_1 through rel_1_3_3c changes; compiled on Intel/Solaris.

  Revision 1.46  1997/02/03 19:45:37  ssl
  1) RecordLink.[Ch],QueryProcFull.[ch]  : added negative record links
  2) ViewLens.[Ch] : new implementation of piled views
  3) ParseAPI.C : new API for ViewLens, negative record links and layout
     manager

  Revision 1.45.4.1  1997/02/13 18:11:49  ssl
  Added a check to the user interface asking when user links two different
  data sets with a record link

  Revision 1.45  1997/01/24 22:18:40  wenger
  Fixed bug 136.

  Revision 1.44  1996/12/30 23:57:37  andyt
  First version with support for Embedded Tcl/Tk windows. Added new
  ETkWindow symbol shape. Improved the MappingInterp::MapGAttr2TAttr
  function to handle all GData attributes (used to only handle a subset).

  Revision 1.43  1996/11/26 16:51:41  ssl
  Added support for piled viws

  Revision 1.42  1996/11/20 16:51:09  jussi
  Replaced AbortAndReexecute() with AbortQuery() and Refresh().

  Revision 1.41  1996/11/13 16:57:13  wenger
  Color working in direct PostScript output (which is now enabled);
  improved ColorMgr so that it doesn't allocate duplicates of colors
  it already has, also keeps RGB values of the colors it has allocated;
  changed Color to GlobalColor, LocalColor to make the distinction
  explicit between local and global colors (_not_ interchangeable);
  fixed global vs. local color conflict in View class; changed 'dali'
  references in command-line arguments to 'tasvir' (internally, the
  code still mostly refers to Dali).

  Revision 1.40  1996/09/27 21:09:38  wenger
  GDataBin class doesn't allocate space for connectors (no longer used
  anyhow); fixed some more memory leaks and made notes in the code about
  some others that I haven't fixed yet; fixed a few other minor problems
  in the code.

  Revision 1.39  1996/08/29 19:07:29  ssl
  Same bug fix for reclinks slightly modified.

  Revision 1.37  1996/08/07 19:27:11  jussi
  Moved legends in merged view up a bit.

  Revision 1.36  1996/08/05 19:49:02  wenger
  Fixed compile errors caused by some of Kevin's recent changes; changed
  the attrproj stuff to make a .a file instead of a .o; added some more
  TData file writing stuff; misc. cleanup.

  Revision 1.35  1996/08/05 18:41:48  beyer
  - Color stats only print an entry for each color that is in it source view.
  (I.e., if the count for a color is zero, it doesn't have a record in the
  color stats.)
  - Improved safety of stats code.

  Revision 1.34  1996/08/04 21:59:57  beyer
  Added UpdateLinks that allow one view to be told to update by another view.
  Changed TData so that all TData's have a DataSource (for UpdateLinks).
  Changed all of the subclasses of TData to conform.
  A RecFile is now a DataSource.
  Changed the stats buffers in ViewGraph to be DataSources.

  Revision 1.33  1996/08/03 15:20:17  jussi
  Made "out of statistics buffer space" messages appear only when
  DEBUG is defined.

  Revision 1.32  1996/07/30 18:24:56  guangshu
  Commit the change as last one again.

  Revision 1.30  1996/07/25 14:32:55  guangshu
  Added linked list to keep track of the gstat records so it doesnot need to scann the range from xmin to xmax and fixed bugs for histograms

  Revision 1.29  1996/07/23 19:34:50  beyer
  Changed dispatcher so that pipes are not longer used for callback
  requests from other parts of the code.

  Revision 1.28  1996/07/23 17:26:07  jussi
  Added support for piled views.

  Revision 1.27  1996/07/22 23:44:31  guangshu
  Added statistics for gdata. The statistics includes count, ysum, max,
  mean, min.

  Revision 1.26  1996/07/20 17:07:57  guangshu
  Small fixes when wirte to HistBuffer.

  Revision 1.25  1996/07/19 18:00:31  guangshu
  Added support for histograms.

  Revision 1.24  1996/07/13 00:22:21  jussi
  ViewGraph writes only the minimum number of necessary records
  in the color statistics buffer.

  Revision 1.23  1996/07/12 19:40:12  jussi
  View statistics are now printed into a memory buffer.

  Revision 1.22  1996/06/27 15:46:12  jussi
  Moved key '5' functionality to ViewGraph::UpdateAutoScale().

  Revision 1.21  1996/06/24 19:44:30  jussi
  Added a win->Flush() to force statistics on the screen.

  Revision 1.20  1996/06/20 17:10:25  guangshu
  Added support for color statistics.

  Revision 1.19  1996/06/15 13:51:27  jussi
  Added SetMappingLegend() method.

  Revision 1.18  1996/06/13 00:16:32  jussi
  Added support for views that are slaves of more than one record
  link. This allows one to express disjunctive queries.

  Revision 1.17  1996/05/31 15:41:22  jussi
  Added support for record links.

  Revision 1.16  1996/05/07 16:33:23  jussi
  Moved Action member variable from View to ViewGraph. Moved
  implementation of HandleKey, HandlePress and HandlePopup to
  ViewGraph as well. Added IsScatterPlot() method.

  Revision 1.15  1996/04/22 21:38:09  jussi
  Fixed problem with simultaneous view refresh and record query
  activities. Previously, there was a single iterator over the
  mappings of a view, which caused the system to crash when a record
  was queried while the data was still being displayed. Each activity
  now gets its own iterator.

  Revision 1.14  1996/04/20 19:56:59  kmurli
  QueryProcFull now uses the Marker calls of Dispatcher class to call
  itself when needed instead of being continuosly polled by the Dispatcher.

  Revision 1.13  1996/04/16 20:39:54  jussi
  Replaced assert() calls with DOASSERT macro.

  Revision 1.12  1996/04/15 15:13:52  jussi
  A mapping label is now stored as part of the mapping list data
  structure. Added GetMappingLegend() accessor method.

  Revision 1.11  1996/04/10 15:27:22  jussi
  Added RemoveMapping() method.

  Revision 1.10  1996/04/10 02:23:34  jussi
  Added direction parameter to InitMappingIterator(), and added
  SwapMappings() method.

  Revision 1.9  1995/12/28 20:46:38  jussi
  Minor clean up.

  Revision 1.8  1995/12/18 03:15:15  ravim
  Data is never refreshed if the only changes are due to draw/undrawing the
  statistics. XOR logic used to accomplish this optimization.

  Revision 1.7  1995/12/14 22:03:49  jussi
  Added a couple of more checks in _DisplayStats handling.

  Revision 1.6  1995/12/14 20:18:52  jussi
  Fixed initialization of _DisplayStat and added more checking to
  SetViewStatistics (string passed to function can be shorter than
  _DisplayStats).

  Revision 1.5  1995/12/14 15:26:23  jussi
  Added a "return false" statement to ToRemoveStat.

  Revision 1.4  1995/12/07 02:20:51  ravim
  The set of stats to be displayed is given as a binary string. The
  data is refreshed only if necessary.

  Revision 1.3  1995/12/05 17:04:16  jussi
  Moved _stats from View (subclass) so that statistics can be turned
  on and displayed without having to redisplay the data itself.

  Revision 1.2  1995/09/05 22:16:17  jussi
  Added CVS header.
*/

//******************************************************************************

//#define DEBUG
#define DEBUG_LOG
//#define REPORT_QUERY_TIME

#include <assert.h>

#include "ViewGraph.h"
#include "TDataMap.h"
#include "ActionDefault.h"
#include "Init.h"
#include "MasterSlaveLink.h"
#include "RecordLink.h"
#include "VisualLink.h"
#include "TData.h"
#include "Util.h"
#include "AssoArray.h"
#include "TimeStamp.h"
#include "SlaveTable.h"

#include "MappingInterp.h"
#include "CountMapping.h"
#include "DepMgr.h"
#include "DupElim.h"
#include "PileStack.h"
#include "MapInterpClassInfo.h"
#include "Session.h"
#include "DebugLog.h"
#include "ViewSymFilterInfo.h"
#include "ArgList.h"
#include "CmdContainer.h"

#define STEP_SIZE 20

ImplementDList(GStatList, double)
ImplementDList(BStatList, BasicStats *)

//******************************************************************************
// class ViewGraph_QueryCallback
//******************************************************************************

//TEMP -- why isn't ViewGraph just derived from QueryCallback?
class ViewGraph_QueryCallback : public QueryCallback
{
	private:

		ViewGraph*	_parent;
		
	public:

		ViewGraph_QueryCallback(ViewGraph* parent)
			: _parent(parent) {}

		virtual void	QueryInit(void* userData)
		{
			_parent->QueryInit(userData);
		}

		virtual void	QueryDone(int bytes, void* userData,
								  Boolean allDataReturned,
								  TDataMap* map = NULL)
		{
			_parent->QueryDone(bytes, userData, allDataReturned, map);
		}

		virtual void*	GetObj(void)
		{
			return _parent->GetObj();
		}

		virtual MSLinkList*		GetMasterLinkList()
		{
			return _parent->GetMasterLinkList();
		}

		virtual MSLinkList*		GetRecordLinkList()
		{
			return _parent->GetRecordLinkList();
		}

		virtual void	ReturnGData(TDataMap* mapping, RecId id,
									void* gdata, int numGData,
									int& recordsProcessed,
									Boolean needDrawnList, int& recordsDrawn,
									BooleanArray*& drawnList)
		{
			_parent->ReturnGData(mapping, id, gdata, numGData,
								 recordsProcessed, needDrawnList, recordsDrawn,
								 drawnList);
		}

		virtual void	PrintLinkInfo(void)
		{
			_parent->PrintLinkInfo();
		}

		virtual Boolean HasDerivedTable()
		{
			return _parent->HasDerivedTable();
		}

		virtual void InsertValues(TData *tdata, int recCount, void **tdataRecs)
		{
			_parent->InsertValues(tdata, recCount, tdataRecs);
		}
};


//******************************************************************************
// Constructors and Destructors
//******************************************************************************

ViewGraph::ViewGraph(char* name, VisualFilter& initFilter, QueryProc* qp,
					 AxisLabel* xAxis, AxisLabel* yAxis,
					 PColorID fgid, PColorID bgid, Action* action)
	: View(name, initFilter, fgid, bgid, xAxis, yAxis),
	  _updateLink("stats link")
{
    DO_DEBUG(printf("ViewGraph::ViewGraph(0x%p, %s)\n",
		    this, (name != NULL) ? name : "<null>"));

	queryCallback = new ViewGraph_QueryCallback(this);

    _action = action;
    _deleteAction = false;
    if (!_action)
    {
      _action = new ActionDefault("default");
      _deleteAction = true;
    }

    memset(_DisplayStats, '0', STAT_NUM);

    // add terminating null
    _DisplayStats[STAT_NUM] = 0;
    
    // auto scaling is in effect by default
    _autoScale = true;

    _updateLink.SetMasterView(this);

    _statBuffer = new DataSourceFixedBuf(DERIVED_BUF_SIZE, "statBuffer");
    _statBuffer->AddRef();
    _statBuffer->SetControllingView(this);

    _histBuffer = new DataSourceFixedBuf(HIST_BUF_SIZE, "histBuffer");
    _histBuffer->AddRef();
    _histBuffer->SetControllingView(this);
    _histBuffer->Write("0 0\n", 4); // ksb: kludge! doesn't work w/o some data BUG 303

    _gdataStatBufferX = new DataSourceFixedBuf(DERIVED_BUF_SIZE, "gdataStatBufferX");
    _gdataStatBufferY = new DataSourceFixedBuf(DERIVED_BUF_SIZE, "gdataStatBufferY");
    _gdataStatBufferX->AddRef();
    _gdataStatBufferY->AddRef();
    _gdataStatBufferX->SetControllingView(this);
    _gdataStatBufferY->SetControllingView(this);

    _gstatInMem = true;
    histViewName = NULL;

    _horPanInfo.mode = PanModeRelative;
    _horPanInfo.relPan = 0.5;
    _horPanInfo.absPan = 1.0;
    _verPanInfo.mode = PanModeRelative;
    _verPanInfo.relPan = 0.5;
    _verPanInfo.absPan = 1.0;

  _queryProc = qp;
  _map = 0;
  _index = -1;
  _queryFilter = initFilter;

  _gds = NULL;
  _drawToScreen = true;
  _sendToSocket = false;

  _gdsParams.portNum = 0;
  _gdsParams.file = NULL;
  _gdsParams.sendText = true;
  _gdsParams.separator = ' ';
  _gdsParams.rgbColor = true;
  _gdsPostponed = false;

  _jsDrawToScreen = true;
  _jsSendToSocket = false;

  _jsGdsParams.portNum = 0;
  _jsGdsParams.file = NULL;
  _jsGdsParams.sendText = true;
  _jsGdsParams.separator = ' ';
  _jsGdsParams.rgbColor = true;

  _countMapping = NULL;
  _dupElim = NULL;

  _slaveTable = new SlaveTable(this);
  _stringXTableName = NULL;
  _stringYTableName = NULL;
  _stringZTableName = NULL;
  _stringGenTableName = NULL;

  _dataRangesValid = false;

  _niceXAxis = false;
  _niceYAxis = false;
  _homeAfterQueryDone = false;
  _inDerivedStartQuery = false;

  _viewSymFilterInfo = new ViewSymFilterInfo;
  _viewSymParentVal = NULL;

  _objectValid.Set();
}

ViewGraph::~ViewGraph(void)
{
  DOASSERT(_objectValid.IsValid(), "operation on invalid object");
#if defined(DEBUG)
    printf("ViewGraph::~ViewGraph(0x%p, %s)\n",
		this, (GetName() != NULL) ? GetName() : "<null>");
#endif

    // note: viewgraphs report their destruction twice, once during
    // ~ViewGraph() and once during ~View()
    ReportViewDestroyed();

    // disconnect from the stats buffers
    _statBuffer->RemoveControllingView(this);
    if( _statBuffer->DeleteRef() ) {
	delete _statBuffer;
    }
    _histBuffer->RemoveControllingView(this);
    if( _histBuffer->DeleteRef() ) {
	delete _histBuffer;
    }
    _gdataStatBufferX->RemoveControllingView(this);
    _gdataStatBufferY->RemoveControllingView(this);
    if( _gdataStatBufferX->DeleteRef() ) {
	delete _gdataStatBufferX;
    }
    if( _gdataStatBufferY->DeleteRef() ) {
	delete _gdataStatBufferY;
    }

    if (_deleteAction) delete _action;

    // SubClassUnmapped aborts any current query; this _must_ be done
    // before this destructor exits, or members needed to do the abort
    // will no longer be defined.
    SubClassUnmapped();

    int index = _blist.InitIterator();
    while (_blist.More(index)) {
      delete _blist.Next(index);
    }
    _blist.DoneIterator(index);
    _blist.DeleteAll();
    _gstatX.Clear();
    _gstatY.Clear();
    _glistX.DeleteAll();
    _glistY.DeleteAll();

    FreeString(_gdsParams.file);
	FreeString(_jsGdsParams.file);
    delete _gds;
	delete queryCallback;
	delete _countMapping;
	delete _dupElim;

    UnlinkMasterSlave();

	delete _slaveTable;
	_slaveTable = NULL;

	FreeString(_stringXTableName);
	_stringXTableName = NULL;

	FreeString(_stringYTableName);
	_stringYTableName = NULL;

	FreeString(_stringZTableName);
	_stringZTableName = NULL;

	FreeString(_stringGenTableName);
	_stringGenTableName = NULL;

	delete _viewSymFilterInfo;
	_viewSymFilterInfo = NULL;

	if (_viewSymParentVal) FreeString(_viewSymParentVal);
	_viewSymParentVal = NULL;
}

//******************************************************************************
// Utility Functions (Color)
//******************************************************************************

double	ViewGraph::CalcDataColorEntropy(void)
{
  DOASSERT(_objectValid.IsValid(), "operation on invalid object");
	Coloring	coloring;
	IntVector	count;

	for(int i=0; i<gMaxNumColors; i++)
	{
#if !VIEW_MIN_STATS
		int		n = (int)(_stats[i].GetStatVal(STAT_COUNT));
#else
        int n = 0;
#endif

		if (n > 0)
		{
			coloring.AddDataColor(i);
			count.push_back(n);
		}
	}

	return coloring.Entropy(count);
}

//******************************************************************************

void ViewGraph::AddAsMasterView(MasterSlaveLink *link)
{
  DOASSERT(_objectValid.IsValid(), "operation on invalid object");
    // add the link as one of the links whose master this view is
    if (!_masterLink.Find(link)) {
#if defined(DEBUG)
        printf("View %s becomes master of record link %s\n", GetName(),
               link->GetName());
#endif
        _masterLink.Append(link);
    }
    Refresh();
}

void ViewGraph::DropAsMasterView(MasterSlaveLink *link)
{
  DOASSERT(_objectValid.IsValid(), "operation on invalid object");
    if (_masterLink.Find(link)) {
        _masterLink.Delete(link);

		// Refresh the link's followers.
		link->RefreshAll();

#if defined(DEBUG)
        printf("View %s no longer master of record link %s\n", GetName(),
               link->GetName());
#endif
    }
}

void
ViewGraph::UnlinkMasterSlave()
{
  DOASSERT(_objectValid.IsValid(), "operation on invalid object");
	//
	// For all links that this view is the master of, reset the master view.
	//
    int index = _masterLink.InitIterator();
	while (_masterLink.More(index)) {
	  MasterSlaveLink *msLink = _masterLink.Next(index);
	  _masterLink.DeleteCurrent(index);
	  msLink->SetMasterView(NULL);
      if (msLink->GetFlag() == VISUAL_TATTR) TAttrLinkChanged();
	}
	_masterLink.DoneIterator(index);

	//
	// For all links that this view is a slave of, remove this view from the
	// link.
	//
	index = _slaveLink.InitIterator();
	while (_slaveLink.More(index)) {
	  MasterSlaveLink *msLink = _slaveLink.Next(index);
	  _slaveLink.DeleteCurrent(index);
	  msLink->DeleteView(this);
	}
	_slaveLink.DoneIterator(index);

	_updateLink.SetMasterView(NULL);
}

void ViewGraph::AddAsSlaveView(MasterSlaveLink *link)
{
  DOASSERT(_objectValid.IsValid(), "operation on invalid object");
    if (!_slaveLink.Find(link)) {
#if defined(DEBUG)
        printf("View %s becomes slave of record link %s\n", GetName(),
               link->GetName());
#endif
        _slaveLink.Append(link);
    }

    if (link->GetFlag() == VISUAL_TATTR) TAttrLinkChanged();
    Refresh();
}

void ViewGraph::DropAsSlaveView(MasterSlaveLink *link)
{
  DOASSERT(_objectValid.IsValid(), "operation on invalid object");
    if (_slaveLink.Find(link)) {
        _slaveLink.Delete(link);
#if defined(DEBUG)
        printf("View %s no longer slave of record link %s\n", GetName(),
               link->GetName());
#endif
    }

    if (link->GetFlag() == VISUAL_TATTR) TAttrLinkChanged();
    Refresh();
}

void
ViewGraph::AddVisualLink(VisualLink *link)
{
  DOASSERT(_objectValid.IsValid(), "operation on invalid object");
#if defined(DEBUG)
    printf("ViewGraph(%s)::AddVisualLink(%s)\n", GetName(), link->GetName());
#endif

  if (!_visualLinks.Find(link)) {
    _visualLinks.Append(link);
  }
}

void
ViewGraph::DeleteVisualLink(VisualLink *link)
{
  DOASSERT(_objectValid.IsValid(), "operation on invalid object");
#if defined(DEBUG)
    printf("ViewGraph(%s)::DeleteVisualLink(%s)\n", GetName(), link->GetName());
#endif

  int result = _visualLinks.Delete(link);
  DOASSERT(result, "Deleted a link not in list");
}

void ViewGraph::InsertMapping(TDataMap *map, char *label)
{
  DOASSERT(_objectValid.IsValid(), "operation on invalid object");
#if defined(DEBUG)
    printf("ViewGraph(%s)::InsertMapping(%s, %s)\n", GetName(),
      map->GetName(), label);
#endif

    MappingInfo *info = new MappingInfo;
    DOASSERT(info, "Could not create mapping information");
    DOASSERT(map, "null map");

    info->map = map;
    info->label = CopyString(label);

    int index = InitMappingIterator();
    if (!MoreMapping(index)) {
      // this is the first mapping
      if( GetHistogramBuckets() == 0 ) {
        // create default histogram
        const AttrInfo *yAttr = map->MapGAttr2TAttr(MappingCmd_Y);
        if( yAttr && yAttr->hasLoVal && yAttr->hasHiVal ) {
          // y min & max known for the file, so use those to define buckets
          double lo = AttrList::GetVal(&yAttr->loVal, yAttr->type);
          double hi = AttrList::GetVal(&yAttr->hiVal, yAttr->type);
          _allStats.SetHistogram(lo, hi, DEFAULT_HISTOGRAM_BUCKETS);
        } else {
          // global min & max are not known, so use filter hi & lo
          VisualFilter filter;
          GetVisualFilter(filter);
          _allStats.SetHistogram(filter.yLow, filter.yHigh,
                                 DEFAULT_HISTOGRAM_BUCKETS);
        }
      }
    } else {
      printf("Warning: inserting multiple mappings into view {%s}\n",
	GetName());
    }
    DoneMappingIterator(index);

    // determine if this view is dependent upon any other
    TData* tdata = map->GetLogTData();
    DOASSERT(tdata, "null tdata");
    DataSource* ds = tdata->GetDataSource();
    DOASSERT(ds, "null data source");
    ViewGraph* v = ds->ControllingView();
    if( v ) {
#ifdef DEBUG
        printf("View %s is slave of update link for %s\n", GetName(),
               v->GetName());
#endif
	v->GetUpdateLink().InsertView(this);
    }

    _mappings.Append(info);

    UpdateAxisTypes();

//    if(IsXDateType()) {
//    	PrepareStatsBuffer(GetFirstMap());
#if defined(DEBUG)
//	printf("Need to re-prepare stat buffer in Insertmapping()\n");
#endif
//    	PrepareStatsBuffer(map);
//    }
//    GoHome();

    Refresh();
}

void ViewGraph::RemoveMapping(TDataMap *map)
{
  DOASSERT(_objectValid.IsValid(), "operation on invalid object");
#if defined(DEBUG)
    printf("ViewGraph(%s)::RemoveMapping(%s)\n", GetName(), map->GetName());
#endif

    // If there's a query running on this view, we will crash when we
    // actually remove the mapping.
    AbortQuery();

    int index = InitMappingIterator();

    while(MoreMapping(index )) {
        MappingInfo *info = NextMapping(index);
        if (info->map == map) {
            DoneMappingIterator(index);
            _mappings.Delete(info);
            FreeString(info->label);
            delete info;
	    Refresh();
            return;
        }
    }

    DoneMappingIterator(index);
}

char *ViewGraph::GetMappingLegend(TDataMap *map)
{
  DOASSERT(_objectValid.IsValid(), "operation on invalid object");
    int index = InitMappingIterator();

    while(MoreMapping(index)) {
        MappingInfo *info = NextMapping(index);
        if (info->map == map) {
            DoneMappingIterator(index);
            return info->label;
        }
    }

    DoneMappingIterator(index);

    return "";
}

void ViewGraph::SetMappingLegend(TDataMap *map, char *label)
{
  DOASSERT(_objectValid.IsValid(), "operation on invalid object");
    int index = InitMappingIterator();

    while(MoreMapping(index)) {
        MappingInfo *info = NextMapping(index);
        if (info->map == map) {
            FreeString(info->label);
            info->label = CopyString(label);
            break;
        }
    }

    DoneMappingIterator(index);
}

void ViewGraph::DrawLegend()
{
  DOASSERT(_objectValid.IsValid(), "operation on invalid object");
    WindowRep*	win = GetWindowRep();
	
    win->PushTop();
    win->MakeIdentity();
    
    int x, y;
    int w, h;
    GetDataArea(x, y, w, h);

    y += (int)(0.05 * h);
    int yInc = 12;

    int index = InitMappingIterator();

    while(MoreMapping(index)) {
        MappingInfo *info = NextMapping(index);
        char *label = info->label;
		if (!label) continue;
        if (!strlen(label))
          continue;

		TDataMap*	map = info->map;
		PColorID	fgid = GetPColorID(viewGraphLegendColor);
		
		if (map->GetGDataOffset()->_colorOffset < 0)
			fgid = map->GetColoring().GetForeground();

		win->SetForeground(fgid);
		win->AbsoluteText(label, x, y, w - 4, yInc, WindowRep::AlignEast, true);
		y += yInc;
	}

    DoneMappingIterator(index);

    win->PopTransform();
}

void
ViewGraph::GetHome2D(Boolean explicitRequest, VisualFilter &filter)
{
  DOASSERT(_objectValid.IsValid(), "operation on invalid object");
#if defined(DEBUG)
    printf("ViewGraph(%s)::GetHome2D(%d)\n", GetName(), explicitRequest);
#endif

    ViewHomeInfo *homeInfo;
	if (explicitRequest) {
	  homeInfo = &_homeInfo;
	} else {
	  homeInfo = &_implicitHomeInfo;
	}

#if 0
    printf("  homeInfo->homeX = %d\n", homeInfo->homeX);
    printf("  homeInfo->homeY = %d\n", homeInfo->homeY);
    printf("  homeInfo->mode = %d\n", homeInfo->mode);
    printf("  homeInfo->autoYMinZero = %d\n", homeInfo->autoYMinZero);
    printf("  homeInfo->manXLo, manYLo = %g, %g\n", homeInfo->manXLo,
      homeInfo->manYLo);
    printf("  homeInfo->manXHi, manYHi = %g, %g\n", homeInfo->manXHi,
      homeInfo->manYHi);
#endif

    DOASSERT(GetNumDimensions() == 2, "GetHome2D called on non 2D view");

    int index = InitMappingIterator();
    if (!MoreMapping(index)) {
        DoneMappingIterator(index);
        return;
    }
    TDataMap *map = NextMapping(index)->map;
    DoneMappingIterator(index);

    const AttrInfo *xAttr = map->MapGAttr2TAttr(MappingCmd_X);
    const AttrInfo *yAttr = map->MapGAttr2TAttr(MappingCmd_Y);

    Boolean hasFirstRec, hasLastRec;
    RecId firstRec, lastRec;
    if ((xAttr && !strcmp(xAttr->name, REC_ID_NAME)) ||
      (yAttr && !strcmp(yAttr->name, REC_ID_NAME))) {
        TData *tdata = map->GetPhysTData();
        hasFirstRec = tdata->HeadID(firstRec);
        hasLastRec = tdata->LastID(lastRec);
    }

    GetVisualFilter(filter);

    switch (homeInfo->mode) {
    case HomeAuto: {
        if (homeInfo->homeX) {
            Boolean setXLow = false;
            Boolean setXHigh = false;

            // Check data ranges first (fixes bug 469).
            if (_dataRangesValid) {
                if (!setXLow) {
                    filter.xLow = _dataXMin - homeInfo->autoXMargin;
                    setXLow = true;
                }
                if (!setXHigh) {
                    filter.xHigh = _dataXMax + homeInfo->autoXMargin;
                    setXHigh = true;
                }
            }

            if (xAttr) {
                if (!setXLow) {
                    if (xAttr->hasLoVal) {
                        filter.xLow = AttrList::GetVal(&xAttr->loVal,
                          xAttr->type) - homeInfo->autoXMargin;
                        setXLow = true;
                    } else if (!strcmp(xAttr->name, REC_ID_NAME)) {
                        if (hasFirstRec) {
                            filter.xLow = (Coord)firstRec;
                            setXLow = true;
                        }
                    }
                }

                if (!setXHigh) {
                    if (xAttr->hasHiVal) {
                        filter.xHigh = AttrList::GetVal(&xAttr->hiVal,
                          xAttr->type) + homeInfo->autoXMargin;
                        setXHigh = true;
                    } else if (!strcmp(xAttr->name, REC_ID_NAME)) {
                        if (hasLastRec) {
                            filter.xHigh = (Coord)lastRec;
                            setXHigh = true;
                        }
                    }
                }
            }

            if (filter.xHigh == filter.xLow) {
                //filter.xLow -= 1.0;
                filter.xHigh += 1.0;
            }
        }


        if (homeInfo->homeY) {
            Boolean setYLow = false;
            Boolean setYHigh = false;

            // Check data ranges first (fixes bug 469).
            if (_dataRangesValid) {
                if (!setYLow) {
                    filter.yLow = _dataYMin - homeInfo->autoYMargin;
                    setYLow = true;
                }
                if (!setYHigh) {
                    filter.yHigh = _dataYMax + homeInfo->autoYMargin;
                    setYHigh = true;
                }
            }

            if (yAttr) {
                if (!setYLow) {
                    if (yAttr->hasLoVal) {
                        filter.yLow = AttrList::GetVal(&yAttr->loVal,
                          yAttr->type) - homeInfo->autoYMargin;
                        setYLow = true;
                    } else if (!strcmp(yAttr->name, REC_ID_NAME)) {
                        if (hasFirstRec) {
                            filter.yLow = (Coord)firstRec;
                            setYLow = true;
                        }
                    }
                }

                if (!setYHigh) {
                    if (yAttr->hasHiVal) {
                        filter.yHigh = AttrList::GetVal(&yAttr->hiVal,
                          yAttr->type) + homeInfo->autoYMargin;
                        setYHigh = true;
                    } else if (!strcmp(yAttr->name, REC_ID_NAME)) {
                        if (hasLastRec) {
                            filter.yHigh = (Coord)lastRec;
                            setYHigh = true;
                        }
                    }
                }
            }

            if (filter.yHigh == filter.yLow) {
                //filter.yLow -= 1.0;
                filter.yHigh += 1.0;
            }

            if (homeInfo->autoYMinZero) {
                filter.yLow = MIN(filter.yLow, 0.0);
            }
        }
        if (_niceXAxis) NiceAxisRange(filter.xLow, filter.xHigh);
        if (_niceYAxis) NiceAxisRange(filter.yLow, filter.yHigh);
        break;
    }

    case HomeManual: {
        if (homeInfo->homeX) {
            filter.xLow = homeInfo->manXLo;
            filter.xHigh = homeInfo->manXHi;
        }
        if (homeInfo->homeY) {
            filter.yLow = homeInfo->manYLo;
            filter.yHigh = homeInfo->manYHi;
        }
        break;
    }

    default:
        DOASSERT(false, "Bad home mode");
        return;
        break;
    }
#if defined(DEBUG)
    printf("  Returning filter: (%g, %g), (%g, %g)\n", filter.xLow,
	  filter.yLow, filter.xHigh, filter.yHigh);
#endif
}

void ViewGraph::GoHomeCommand()
{
  DOASSERT(_objectValid.IsValid(), "operation on invalid object");
#if defined(DEBUG)
    printf("ViewGraph(%s)::GoHomeCommand()\n", GetName());
#endif

  if (DeviseCommand::GetCmdDepth() > 1) {
    // We don't want this to be sent to collaborating deviseds, etc.
    GoHome(true);

  } else {
    // Propagate to other deviseds if we're in group mode.

    ArgList args(2);
    args.AddArg("viewGoHome");
    args.AddArg(GetName());

    CmdContainer::GenerateCommand(args.GetCount(), args.GetArgs());
  }
}

void ViewGraph::GoHome(Boolean explicitRequest)
{
  DOASSERT(_objectValid.IsValid(), "operation on invalid object");
#if defined(DEBUG)
    printf("ViewGraph(%s)::GoHome(%d)\n", GetName(), explicitRequest);
#endif

    if (GetNumDimensions() == 2) {
	// If this view is in a visual link, we want to do "home" on the
	// entire link as a unit.  Right now, this only works for 2D.
	// RKW 1999-04-05.
        Boolean hasVisLink = false;
        int index = _visualLinks.InitIterator();
        while (_visualLinks.More(index)) {
          VisualLink *link = _visualLinks.Next(index);
          link->GoHome(this, explicitRequest);
          hasVisLink = true;
        }
        _visualLinks.DoneIterator(index);

        if (hasVisLink) {
          return;
        }
    }


    /* show all data records in view i.e. set filter to use the
       actual min/max X values and the actual min/max Y values;
       for 3D graphs, move camera to (0,0,Z) where Z is twice
       the min Z value */

    if (GetNumDimensions() == 2) {
        VisualFilter filter;
	GetHome2D(explicitRequest, filter);

	SetVisualFilter(filter);
    } else {
        Camera c=GetCamera();
#if 0
        c.fx = 0;
        c.fy = 0;
        c.fz = 0;
        double minZ = -4.0;
        if (zAttr && zAttr->hasLoVal)
          minZ = 2 * AttrList::GetVal(&zAttr->loVal, zAttr->type);
        if (minZ > -4.0)
          minZ = -4.0;
        if (!c.spherical_coord) {
            c.x_ = 0;
            c.y_ = 0;
            c.z_ = minZ;
        } else {
            c._theta = 0;
            c._phi = M_PI_2;
            c._rho = fabs(minZ);
        }
#endif
        c.pan_right=0;
        c.pan_up=0;
        SetCamera(c);
    }
}

void ViewGraph::PanLeftOrRight(PanDirection direction)
{
  DOASSERT(_objectValid.IsValid(), "operation on invalid object");
#if defined(DEBUG)
  printf("ViewGraph(0x%p)::PanLeftOrRight(%d)\n", this, (int) direction);
#endif

    int panFactor;

    switch(direction) {
    case PanDirLeft:
      panFactor = -1;
      break;

    case PanDirRight:
      panFactor = 1;
      break;

    default:
      DOASSERT(false, "Illegal pan direction");
      return;
      break;
    }

    if (GetNumDimensions() == 2) {
      VisualFilter filter;
      GetVisualFilter(filter);
      Coord width = filter.xHigh - filter.xLow;

      Coord panDist;
      switch (_horPanInfo.mode) {
      case PanModeRelative:
        panDist = width * _horPanInfo.relPan;
	break;

      case PanModeAbsolute:
	panDist = _horPanInfo.absPan;
	break;

      default:
	DOASSERT(false, "Bad pan mode");
	return;
	break;
      }

      filter.xLow += panFactor * panDist;
      filter.xHigh = filter.xLow + width;

      SetVisualFilterCommand(filter);
    } else { 
      // Note: this will fail for collaboration.  RKW 3/31/98.
      Camera camera = GetCamera();
      camera.pan_right+=0.10*panFactor;
      SetCamera(camera);
    }

    return;
}

void ViewGraph::PanUpOrDown(PanDirection direction)
{
  DOASSERT(_objectValid.IsValid(), "operation on invalid object");
#if defined(DEBUG)
  printf("ViewGraph(0x%p)::PanUpOrDown(%d)\n", this, (int) direction);
#endif

    int panFactor;

    switch(direction) {
    case PanDirUp:
      panFactor = 1;
      break;

    case PanDirDown:
      panFactor = -1;
      break;

    default:
      DOASSERT(false, "Illegal pan direction");
      return;
      break;
    }

    if (GetNumDimensions() == 2) {
      VisualFilter filter;
      GetVisualFilter(filter);
      Coord height = filter.yHigh - filter.yLow;

      Coord panDist;
      switch (_verPanInfo.mode) {
      case PanModeRelative:
        panDist = height * _verPanInfo.relPan;
	break;

      case PanModeAbsolute:
	panDist = _verPanInfo.absPan;
	break;

      default:
	DOASSERT(false, "Bad pan mode");
	return;
	break;
      }

      filter.yLow += panFactor * panDist;
      filter.yHigh = filter.yLow + height;

      SetVisualFilterCommand(filter);
    } else { 
      Camera camera = GetCamera();
      camera.pan_up+=0.10*panFactor;
      SetCamera(camera);
    }
}

void
ViewGraph::CursorHome()
{
  DOASSERT(_objectValid.IsValid(), "operation on invalid object");
#if defined(DEBUG)
  printf("ViewGraph(%s)::CursorHome()\n", GetName());
#endif

  VisualFilter filter;
  GetVisualFilter(filter);
  int index = _cursors->InitIterator();
  while (_cursors->More(index)) {
    DeviseCursor *cursor = _cursors->Next(index);
    if (!cursor->GetFixedSize()) {
#if defined(DEBUG)
      printf("  Doing home on cursor <%s>\n", cursor->GetName());
#endif
      Coord x = (filter.xHigh + filter.xLow) / 2.0;
      Coord y = (filter.yHigh + filter.yLow) / 2.0;
      Coord width = filter.xHigh - filter.xLow;
      Coord height = filter.yHigh - filter.yLow;
      cursor->MoveSource(x, y, width, height, true);
    }
  }
  _cursors->DoneIterator(index);
}

/* Lets view know that something in one of the TAttrLinks it's a slave of
 * has changed (master view changed, for example). */
void
ViewGraph::TAttrLinkChanged()
{
  DOASSERT(_objectValid.IsValid(), "operation on invalid object");
    _slaveTAttrTimestamp = TimeStamp::NextTimeStamp();
    Refresh();
}

/* Update the physical TData (for TAttrLinks). */
void
ViewGraph::UpdatePhysTData()
{
  DOASSERT(_objectValid.IsValid(), "operation on invalid object");
#if defined(DEBUG)
    printf("ViewGraph(%s)::UpdatePhysTData()\n", _name);
#endif

    (void) _slaveTable->UpdateTData();
}

void ViewGraph::WriteMasterLink(RecId start, int num)
{
  DOASSERT(_objectValid.IsValid(), "operation on invalid object");
    // Insert records into record links whose master this view is
    int index = _masterLink.InitIterator();
    while(_masterLink.More(index)) {
        MasterSlaveLink *link = _masterLink.Next(index);
#if defined(DEBUG)
	printf("*********inserting recs (%ld, %ld) into %s\n", 
	       start, start + num - 1, link->GetName());
#endif
        link->InsertRecs(start, num);
    }
    _masterLink.DoneIterator(index);
}

Boolean ViewGraph::IsScatterPlot()
{
  DOASSERT(_objectValid.IsValid(), "operation on invalid object");
    int index = InitMappingIterator();

    if (MoreMapping(index)) {
        TDataMap *map = NextMapping(index)->map;
        int numDimensions;
        VisualFlag *dimensionInfo;
        numDimensions = map->DimensionInfo(dimensionInfo);
        if (!numDimensions) {
            /* a scatter plot has no sorted dimensions */
            DoneMappingIterator(index);
            return true;
        }
    }

    DoneMappingIterator(index);

    return false;
}

void ViewGraph::SetDisplayStats(char *stat)
{
  DOASSERT(_objectValid.IsValid(), "operation on invalid object");
    if (strlen(stat) > STAT_NUM) {
        fprintf(stderr, "Incorrect view statistics length: %d\n",
                (int)strlen(stat));
        return;
    }

    // Never redisplay data as long as the visual filter is the same.
    // Before drawing the lines, need to figure out which of them to draw.
    // If a line existing before needs to be deleted - draw line again.
    // If a line not there before needs to be drawn - draw it.
    // If line not there before, not to be drawn - don't draw it.
    // If line there now, needs to remain - don't draw it.
    // Basically XOR logic.
    
    StatsXOR(_DisplayStats, stat, _DisplayStats);
    _allStats.Report();

    // use memcpy to avoid copying the terminating null in stat
    memcpy(_DisplayStats, stat, strlen(stat));

    // flush new statistics to the screen
    WindowRep *win = GetWindowRep();
    if (win)
      win->Flush();
}


void ViewGraph::MasterRecomputed(ViewGraph* master)
{
  DOASSERT(_objectValid.IsValid(), "operation on invalid object");
#ifdef DEBUG
    printf("ViewGraph::MasterRecomputed [%s]\n", GetName());
#endif
    AbortQuery();
    Refresh();
}

Boolean ViewGraph::ToRemoveStats(char *oldset, char *newset)
{
    DOASSERT(strlen(oldset) == STAT_NUM && strlen(newset) == STAT_NUM,
             "Invalid statistics flags");

    /* Check if a 1 in "old" has become 0 in "new" - means that a stat
       previously being displayed should now be removed. */
    for (int i = 0; i < STAT_NUM; i++)
      if ((oldset[i] == '1') && (newset[i] == '0'))
        return true;
    return false;
}

void ViewGraph::StatsXOR(char *oldstat, char *newstat, char *result)
{
    for (int i = 0; i < STAT_NUM; i++)
      result[i] = ((oldstat[i] - '0')^(newstat[i] - '0')) + '0';
}

/*
AssoArray<int> month(12);

month["Jan"] = 1;
month["Feb"] = 2;
month["Mar"] = 3;
month["Apr"] = 4;
month["May"] = 5;
month["Jun"] = 6;
month["Jul"] = 7;
month["Aug"] = 8;
month["Sep"] = 9;
month["Oct"] = 10;
month["Nov"] = 11;
month["Dec"] = 12;
*/

/* The following two methods areonly temporary solutions, a good solution
   is an associative array which I am working on. */

int findMonth(const char *mon){
	if(!strcmp(mon, "Jan")) return 1;
	if(!strcmp(mon, "Feb")) return 2;
	if(!strcmp(mon, "Mar")) return 3;
	if(!strcmp(mon, "Apr")) return 4;
	if(!strcmp(mon, "May")) return 5;
	if(!strcmp(mon, "Jun")) return 6;
	if(!strcmp(mon, "Jul")) return 7;
	if(!strcmp(mon, "Aug")) return 8;
	if(!strcmp(mon, "Sep")) return 9;
	if(!strcmp(mon, "Oct")) return 10;
	if(!strcmp(mon, "Nov")) return 11;
	if(!strcmp(mon, "Dec")) return 12;
	else printf("***********No such month**********\n");
	return 0;
}

char **ExtractDate(const char *string) {
	char **date = new char*[3];
	date[0] = new char[4];
	date[1] = new char[3];
	date[2] = new char[5];

	const char *p = string;
	strncpy(date[0], p, 4);
	date[0][3] = '\0';
	strncpy(date[1], p+4, 3);
	if(date[1][0]==' ') { date[1]++; date[1][1]='\0'; }
	else date[1][2] = '\0';
	strncpy(date[2], p+7, 5);
	date[2][4] = '\0';

	return date;
}


void ViewGraph::SetHistogram(Coord min, Coord max, int buckets)
{
  DOASSERT(_objectValid.IsValid(), "operation on invalid object");
  AbortQuery();
  _allStats.SetHistogram(min, max, buckets);
  ResetGStatInMem();
  Refresh();
}


void ViewGraph::PrepareStatsBuffer(TDataMap *map)
{
  DOASSERT(_objectValid.IsValid(), "operation on invalid object");
    /* initialize statistics buffer */
    _statBuffer->Clear();
    _histBuffer->Clear();
    _gdataStatBufferX->Clear();
    _gdataStatBufferY->Clear();

#if VIEW_MIN_STATS
    return;
#endif

    VisualFilter filter;
    GetVisualFilter(filter);
    const char *date_string;
    char **date;

    /* put the statistics in the stat buffer */
    char line[1024];
    int i;
    for(i = 0; i < gMaxNumColors; i++) {
//	if( _stats[i].GetStatVal(STAT_COUNT) > 0 ) {
#if !VIEW_MIN_STATS
	    sprintf(line, "%d %d %g %g %g %g %g %g %g %g %g %g\n",
		      i, (int)_stats[i].GetStatVal(STAT_COUNT),	
		      _stats[i].GetStatVal(STAT_YSUM),
		      _stats[i].GetStatVal(STAT_MIN),
		      _stats[i].GetStatVal(STAT_MEAN),
		      _stats[i].GetStatVal(STAT_MAX),
		      _stats[i].GetStatVal(STAT_ZVAL85L),
		      _stats[i].GetStatVal(STAT_ZVAL85H),
		      _stats[i].GetStatVal(STAT_ZVAL90L),
		      _stats[i].GetStatVal(STAT_ZVAL90H),
		      _stats[i].GetStatVal(STAT_ZVAL95L),
		      _stats[i].GetStatVal(STAT_ZVAL95H));
#else
        sprintf(line, "NO STATS!!!\n");
#endif
	    int len = strlen(line);
#if defined(DEBUG) || 0
    printf("Color stat buf line is %s\n", line);
#endif

	    DOASSERT(len < (int) sizeof(line), "too much data in sprintf");
	    if( (int) _statBuffer->Write(line, len) != len ) {
#ifdef DEBUG
		fprintf(stderr, "Out of statistics buffer space\n");
#endif
		break;
	    }
//	  }
      }
    Boolean y_is_date = false;
    if(map) {
        AttrList *gAttrList = map->GDataAttrList();
	if (gAttrList) {
          gAttrList->InitIterator();
          while(gAttrList->More()) {
                AttrInfo *info = gAttrList->Next();
                if(!strcmp(info->name, gdataYName) && info->type==DateAttr) {
                        y_is_date = true;
                }
          }
          gAttrList->DoneIterator();
	}
    }

    double width = _allStats.GetHistWidth();
#if defined(DEBUG) || 0
    printf("histogram width: %g\n", width);
#endif
    if( width == 0 ) {
        printf("Hist width should have been set at this point\n");
/*        _allStats.SetHistWidth(filter.yLow, filter.yHigh);
#if defined(DEBUG) || 0
        printf("Hist width in ViewGraph after set is %g\n", _allStats.GetHistWidth());
#endif
*/    }
	int size = HISTOGRAM_REC_SIZE * _allStats.GetnumBuckets(); // 24 is the max size for each line
	_histBuffer->Resize(size);
	double pos = _allStats.GetHistMin() + width / 2.0;
#if defined(DEBUG) || 0
	printf("Histogram min is %g, max is %g\n", _allStats.GetHistMin(), _allStats.GetHistMax());
#endif
	for(i = 0; i < _allStats.GetnumBuckets(); i++) {
	    if (y_is_date) {
		date_string = DateString(pos);
		date = ExtractDate(date_string);
		sprintf(line, "%d %s %s %d\n", findMonth(date[0]),
                 	date[1], date[2], _allStats.GetHistVal(i));
	    } else sprintf(line, "%.4e %d\n", pos, _allStats.GetHistVal(i));
	    int len = strlen(line);
	    DOASSERT(len < (int) sizeof(line), "too much data in sprintf");
	    if( (int) _histBuffer->Write(line, len) != len ) {
	        fprintf(stderr, "*****Warning: Out of histogram buffer space\n");
	        break;
	    }
#if defined(DEBUG) || 0
	    printf("Hist buf line is %s, pos is %g\n", line, pos);
#endif
	    pos += width;
	}
#if defined(DEBUG) || 0
	printf("Buf has %d lines\n", i);
#endif

    BasicStats *bs;
    Boolean x_is_date = false;
    if(map){
    	AttrList *gAttrList = map->GDataAttrList();
	if (gAttrList) {
    	  gAttrList->InitIterator();
    	  while(gAttrList->More()) {
		AttrInfo *info = gAttrList->Next();
		if(!strcmp(info->name, gdataXName) && info->type==DateAttr) {
			x_is_date = true;
		}
    	  }
          gAttrList->DoneIterator();
	}
    }

    int getXRecs = 0;
    int len;
    int total = 0;

    int index = _glistX.InitIterator();
    while(_glistX.More(index)) {
	double i = _glistX.Next(index); 
	if (_gstatX.Lookup(i, bs)) {
	   DOASSERT(bs,"HashTable lookup error\n");
	   if (x_is_date) {
		date_string = DateString(i);
		date = ExtractDate(date_string);
		sprintf(line, "%d %s %s %d %g %g %g %g\n", findMonth(date[0]), 
			     date[1], date[2], (int)bs->GetStatVal(STAT_COUNT),
			     bs->GetStatVal(STAT_YSUM),
			     bs->GetStatVal(STAT_MIN),
			     bs->GetStatVal(STAT_MEAN),
			     bs->GetStatVal(STAT_MAX));
	   } else sprintf(line, "%g %d %g %g %g %g\n",
			     i, (int)bs->GetStatVal(STAT_COUNT),
			     bs->GetStatVal(STAT_YSUM),
			     bs->GetStatVal(STAT_MIN),
			     bs->GetStatVal(STAT_MEAN),
			     bs->GetStatVal(STAT_MAX));
	   len = strlen(line);
	   total += len;
	   DOASSERT(len < (int) sizeof(line), "too much data in sprintf");
#if defined(DEBUG) || 0
	    	printf("_gstatX buf line is %s\n", line);
#endif
           getXRecs = 1;
	   if( (int) _gdataStatBufferX->Write(line, len) != len ) {
#ifdef DEBUG
	       fprintf(stderr, "******Out of GData Stat Buffer space\n");
#endif
	       break;
	   }
       }
    }
    _glistX.DoneIterator(index);

    if(getXRecs == 0 ) {
        double low = filter.xLow;
	double high = filter.xHigh;

	date_string = DateString(low);
	date = ExtractDate(date_string);
	sprintf(line, "%d %s %s %d %g %g %g %g\n", findMonth(date[0]),
		 date[1], date[2], 0, 0.0, 0.0, 0.0, 0.0);
       	len = strlen(line);
        DOASSERT(len < (int) sizeof(line), "too much data in sprintf");
	if( (int) _gdataStatBufferX->Write(line, len) != len ) {
    		fprintf(stderr, "******Out of GData Stat Buffer space\n");
    }
// 	printf("_gstatX buf line is %s\n", line);

	sprintf(line, "%g %d %g %g %g %g\n", low, 0, 0.0, 0.0, 0.0, 0.0);
        len = strlen(line);
        DOASSERT(len < (int) sizeof(line), "too much data in sprintf");

#if defined(DEBUG) || 0
        printf("_gstatX buf line is %s\n", line);
#endif 
	if( (int) _gdataStatBufferX->Write(line, len) != len ) {
	    fprintf(stderr, "******Out of GData Stat Buffer space\n");
    }
//        printf("_gstatX buf line is %s\n", line);

        date_string = DateString(high);
        date = ExtractDate(date_string); 
        sprintf(line, "%d %s %s %d %g %g %g %g\n", findMonth(date[0]),
                   date[1], date[2], 0, 0.0, 0.0, 0.0, 0.0);
        len = strlen(line);
        DOASSERT(len < (int) sizeof(line), "too much data in sprintf");
        if( (int) _gdataStatBufferX->Write(line, len) != len ) {
        	fprintf(stderr, "******Out of GData Stat Buffer space\n");
        }
//           printf("_gstatX buf line is %s\n", line);

        sprintf(line, "%g %d %g %g %g %g\n", high, 0, 0.0, 0.0, 0.0, 0.0);
        len = strlen(line);
        DOASSERT(len < (int) sizeof(line), "too much data in sprintf");

#if defined(DEBUG) || 0
           printf("_gstatX buf line is %s\n", line);
#endif
        if( (int) _gdataStatBufferX->Write(line, len) != len ) {
            fprintf(stderr, "******Out of GData Stat Buffer space\n");
        }
    }

    int getYRecs = 0;
    index = _glistY.InitIterator();
    while(_glistY.More(index)) {
	double i = _glistY.Next(index); 
	if (_gstatY.Lookup(i, bs)) {
	   DOASSERT(bs,"HashTable lookup error\n");
	   if (y_is_date) {
        	date_string = DateString(i);
        	date = ExtractDate(date_string);
		sprintf(line, "%d %s %s %d %g %g %g %g\n", findMonth(date[0]), 
                             date[1], date[2], (int)bs->GetStatVal(STAT_COUNT),
                             bs->GetStatVal(STAT_YSUM),
                             bs->GetStatVal(STAT_MIN),
                             bs->GetStatVal(STAT_MEAN),
                             bs->GetStatVal(STAT_MAX));
	   } else sprintf(line, "%g %d %g %g %g %g\n",
			     i, (int)bs->GetStatVal(STAT_COUNT),
			     bs->GetStatVal(STAT_YSUM),
			     bs->GetStatVal(STAT_MIN),
			     bs->GetStatVal(STAT_MEAN),
			     bs->GetStatVal(STAT_MAX));
	   len = strlen(line);
	   DOASSERT(len < (int) sizeof(line), "too much data in sprintf");
#if defined(DEBUG) || 0
	    printf("_gstatY buf line is %s\n", line);
#endif
           getYRecs = 1; 
	   if( (int) _gdataStatBufferY->Write(line, len) != len ) {
#ifdef DEBUG
	       fprintf(stderr, "Out of GData Stat Buffer space\n");
#endif
	       break;
	   }
       }
    }	
    _glistY.DoneIterator(index);

    if(getYRecs == 0) {
        double low = filter.yLow;
        double high = filter.yHigh;

	date_string = DateString(low);
        date = ExtractDate(date_string);
        sprintf(line, "%d %s %s %d %g %g %g %g\n", findMonth(date[0]),
                 date[1], date[2], 0, 0.0, 0.0, 0.0, 0.0);
        len = strlen(line);
        DOASSERT(len < (int) sizeof(line), "too much data in sprintf");
        if( (int) _gdataStatBufferY->Write(line, len) != len ) {
                fprintf(stderr, "******Out of GData Stat Buffer space\n");
        }

        sprintf(line, "%g %d %g %g %g %g\n", low, 0, 0.0, 0.0, 0.0, 0.0);
        len = strlen(line);
        DOASSERT(len < (int) sizeof(line), "too much data in sprintf");

#if defined(DEBUG) || 0
        printf("_gstatY buf line is %s\n", line);
#endif
        if( (int) _gdataStatBufferY->Write(line, len) != len ) {
            fprintf(stderr, "******Out of GData Stat Buffer space\n");
        }

        date_string = DateString(high);
        date = ExtractDate(date_string); 
        sprintf(line, "%d %s %s %d %g %g %g %g\n", findMonth(date[0]),
                   date[1], date[2], 0, 0.0, 0.0, 0.0, 0.0);
        len = strlen(line);
        DOASSERT(len < (int) sizeof(line), "too much data in sprintf");
        if( (int) _gdataStatBufferY->Write(line, len) != len ) {
                fprintf(stderr, "******Out of GData Stat Buffer space\n");
        }

        sprintf(line, "%g %d %g %g %g %g\n", high, 0, 0.0, 0.0, 0.0, 0.0);
        len = strlen(line);
        DOASSERT(len < (int) sizeof(line), "too much data in sprintf");

#if defined(DEBUG) || 0
        printf("_gstatY buf line is %s\n", line);
#endif
        if( (int) _gdataStatBufferY->Write(line, len) != len ) {
            fprintf(stderr, "******Out of GData Stat Buffer space\n");
	    }
    }
}

/* Check if the X attr of GData is DateAttr */
Boolean ViewGraph::IsXDateType(){
	AttrList *gAttrList = GetFirstMap()->GDataAttrList();
	if (gAttrList) {
	  gAttrList->InitIterator();
	  while(gAttrList->More()) {
		AttrInfo *info = gAttrList->Next();
		if(!strcmp(info->name, gdataXName) && info->type==DateAttr) 
			return true;
	  }
	  gAttrList->DoneIterator();
	}
	return false;
}

/* Check if the Y attr of GData is DateAttr */
Boolean ViewGraph::IsYDateType(){
	AttrList *gAttrList = GetFirstMap()->GDataAttrList();
	if (gAttrList) {
	  gAttrList->InitIterator();
	  while(gAttrList->More()) {
		AttrInfo *info = gAttrList->Next();
		if(!strcmp(info->name, gdataYName) && info->type==DateAttr) 
			return true;
	  }
	  gAttrList->DoneIterator();
	}
	return false;
}

void ViewGraph::SetAction(Action *action)
{
  DOASSERT(_objectValid.IsValid(), "operation on invalid object");
  if (_deleteAction) delete _action;
  _action = action;
  _deleteAction = false;
}

void ViewGraph::SetHistogramWidthToFilter()
{
  DOASSERT(_objectValid.IsValid(), "operation on invalid object");
    VisualFilter filter;
    GetVisualFilter(filter);
    Coord lo = filter.yLow;
    Coord hi = filter.yHigh;
    bool is_filter = true;
    if( lo == _allStats.GetHistMin() && hi == _allStats.GetHistMax() ) {
	// histogram already at these values so switch to the
	// global file min & max if available
	int index = InitMappingIterator();
	if (MoreMapping(index)) {
	    MappingInfo *info = NextMapping(index);
	    const AttrInfo *yAttr = info->map->MapGAttr2TAttr(MappingCmd_Y);
	    if( yAttr && yAttr->hasLoVal && yAttr->hasHiVal ) {
		lo = AttrList::GetVal(&yAttr->loVal, yAttr->type);
		hi = AttrList::GetVal(&yAttr->hiVal, yAttr->type);
		is_filter = false;
	    }
	}
	DoneMappingIterator(index);
    }
    printf("Histogram for view %s set to (%g, %g) from %s range\n",
	   GetName(), lo, hi, is_filter ? "filter" : "file");
    SetHistogram(lo, hi, GetHistogramBuckets());
}

void ViewGraph::DerivedStartQuery(VisualFilter &filter, int timestamp)
{
  DOASSERT(_objectValid.IsValid(), "operation on invalid object");
#if defined(DEBUG_LOG)
  {
    char logBuf[256];
    sprintf(logBuf, "ViewGraph(%s)::DerivedStartQuery()\n", GetName());
    DebugLog::DefaultLog()->Message(DebugLog::LevelInfo2, logBuf);
  }
#endif

#if defined(DEBUG)
  printf("ViewGraph(%s)::DerivedStartQuery()\n", GetName());
  printf("  Filter x: (%g, %g)\n", filter.xLow, filter.xHigh);
  printf("  Filter y: (%g, %g)\n", filter.yLow, filter.yHigh);
#endif

  if (_inDerivedStartQuery) {
    reportErrNosys("Warning: ViewGraph::DerivedStartQuery() should not be re-entered for a given view");
    return;
  }
  _inDerivedStartQuery = true;

  _queryFilter = filter;
  _timestamp = timestamp;

  // Initialize statistics collection
  _allStats.Init(this);

#if !VIEW_MIN_STATS
  for(int i = 0; i < gMaxNumColors; i++)
    _stats[i].Init(this);
#endif

  int index = _blist.InitIterator();
  while (_blist.More(index)) {
    delete _blist.Next(index);
  }
  _blist.DoneIterator(index);
  _blist.DeleteAll();
  _gstatX.Clear();     /* Clear the hashtable and calculate it again */
  _gstatY.Clear();     /* Clear the hashtable and calculate it again */
  _glistX.DeleteAll(); /* Clear the gdata list */
  _glistY.DeleteAll(); /* Clear the gdata list */

  // Initialize record links whose master this view is
  index = _masterLink.InitIterator();
  while(_masterLink.More(index)) {
    MasterSlaveLink *link = _masterLink.Next(index);
    link->Initialize();
  }
  _masterLink.DoneIterator(index);

  _index = InitMappingIterator(true);   // open iterator backwards
  if (MoreMapping(_index)) {
    _map = NextMapping(_index)->map;
#if defined(DEBUG)
    printf("Submitting query 1 of %d: 0x%p (view <%s>)\n", _mappings.Size(),
	    _map, GetName());
#endif
    _pstorage.Clear();
    _map->ResetMaxSymSize();
    _queryProc->BatchQuery(_map, _queryFilter, queryCallback, 0, _timestamp);
    if (GetParentPileStack()) GetParentPileStack()->QueryStarted(this);

    if (_sendToSocket) {
      if (_gds != NULL) {
	char buf[512];
        sprintf(buf, "Duplicate GDataSock creation in view %s", GetName());
        reportErrNosys(buf);
      } else if ((_gdsParams.file != NULL && strcmp(_gdsParams.file, "")) ||
          (_gdsParams.portNum != 0)) {
	_gdsPostponed = false;
        _gds = new GDataSock(_gdsParams);
        if (!_gds->GetStatus().IsComplete()) {
	  if (_gds->GetStatus().IsError()) {
	    reportErrNosys("Error creating GDataSock object");
	  }
	  delete _gds;
	  _gds = NULL;
        }
      } else {
        _gdsPostponed = true;
      }
    }
  } else {
#ifdef DEBUG
    printf("View has no mappings; reporting query as done\n");
#endif
    ReportQueryDone(0);
    DoneMappingIterator(_index);
    _map = 0;
    _index = -1;
  }

  if (_countMapping) {
	//TEMP -- don't ignore return value??
    (void) _countMapping->Init(this);
  }

  if (_dupElim) {
	//TEMP -- don't ignore return value??
    (void) _dupElim->Init(this);
  }

#if defined(REPORT_QUERY_TIME)
  gettimeofday(&_queryStartTime, NULL);
#endif

#if defined(DEBUG)
  printf("Done with ViewGraph(%s)::DerivedStartQuery()\n", GetName());
#endif

  _inDerivedStartQuery = false;
}

void ViewGraph::DerivedAbortQuery()
{
  DOASSERT(_objectValid.IsValid(), "operation on invalid object");
#if defined(DEBUG_LOG)
  {
    char logBuf[256];
    sprintf(logBuf, "ViewGraph(%s)::DerivedAbortQuery()\n", GetName());
    DebugLog::DefaultLog()->Message(DebugLog::LevelInfo2, logBuf);
  }
#endif

#if defined(DEBUG)
    printf("ViewGraph(%s)::DerivedAbortQuery(), index = %d\n", GetName(),
        _index);
#endif

  if (_map) {
    _queryProc->AbortQuery(_map, queryCallback);
    DOASSERT(_index >= 0, "Invalid iterator index");
    DoneMappingIterator(_index);
    _map = 0;
    _index = -1;
  }
  if (GetParentPileStack()) GetParentPileStack()->QueryDone(this);

  // Abort record links whose master this view is
  int index = _masterLink.InitIterator();
  while(_masterLink.More(index)) {
    MasterSlaveLink *link = _masterLink.Next(index);
    link->Abort();
  }
  _masterLink.DoneIterator(index);

  // We need to get rid of the GDataSock object here because the query has
  // finished, and we'll create a new GDataSock next time if we need one.
  if (_gds != NULL) {
    delete _gds;
    _gds = NULL;
  }
}

void
ViewGraph::SetDrawToScreen(Boolean drawToScreen)
{
  DOASSERT(_objectValid.IsValid(), "operation on invalid object");
#if defined(DEBUG)
  printf("ViewGraph(0x%p)::SetDrawToScreen(%d)\n", this, drawToScreen);
#endif

  if (drawToScreen && !_drawToScreen) {
    // We should make this a non-propagating redraw when that's possible.
    Refresh();
  }

  _drawToScreen = drawToScreen;
}

void
ViewGraph::SetSendToSocket(Boolean sendToSocket)
{
  DOASSERT(_objectValid.IsValid(), "operation on invalid object");
#if defined(DEBUG)
  printf("ViewGraph(0x%p)::SetSendToSocket(%d)\n", this, sendToSocket);
#endif

  if (sendToSocket && !_sendToSocket) {
    // We should make this a non-propagating redraw when that's possible.
    Refresh();
  }

  _sendToSocket = sendToSocket;
}


void
ViewGraph::SetSendParams(const GDataSock::Params &params)
{
  DOASSERT(_objectValid.IsValid(), "operation on invalid object");
#if defined(DEBUG)
  printf("ViewGraph(0x%p)::SetSendParams(%d, %s, %d, '%c')\n", this,
    params.portNum, params.file ? params.file : "NULL", params.sendText,
    params.separator);
#endif

  _gdsParams = params;
  _gdsParams.file = CopyString(_gdsParams.file);
}

DevStatus
ViewGraph::Send(void **gdataArray, TDataMap *map, int recCount)
{
#if defined(DEBUG)
  printf("ViewGraph::Send(%d)\n", recCount);
#endif

  if (_gdsPostponed) {
#if defined(DEBUG)
    printf("GData send postponed\n");
#endif
    return StatusCancel;
  } else {
    if (_gds != NULL) {
      return _gds->Send(this, gdataArray, map, recCount);
    } else {
      reportErrNosys("No GDataSock object");
      return StatusFailed;
    }
  }
}

//******************************************************************************
// Callback Methods (QueryCallback)
//******************************************************************************

void	ViewGraph::QueryDone(int bytes, void* userData,
  Boolean allDataReturned,TDataMap* map)
{
  DOASSERT(_objectValid.IsValid(), "operation on invalid object");
#if defined(REPORT_QUERY_TIME)
	struct timeval queryEndTime;
    gettimeofday(&queryEndTime, NULL);
	double elapsedTime = (queryEndTime.tv_sec - _queryStartTime.tv_sec) +
	  1.0e-6 * (queryEndTime.tv_usec - _queryStartTime.tv_usec);
#endif

#if defined(DEBUG_LOG)
  {
    char logBuf[256];
    sprintf(logBuf, "ViewGraph(%s)::QueryDone(%d)\n", GetName(), bytes);
    DebugLog::DefaultLog()->Message(DebugLog::LevelInfo2, logBuf);
  }
#endif

#if defined(DEBUG)
	printf("ViewGraph(%s)::QueryDone(), index = %d, bytes = %d\n", GetName(),
		_index, bytes);
#endif

#if defined(REPORT_QUERY_TIME)
    printf("  elapsed time: %g sec.\n", elapsedTime);
#endif

	_pstorage.Clear();

	// Note: _index is index of mapping iterator.
	if (_index < 0) {
		return;
    }

	if (MoreMapping(_index))
	{
#if defined(DEBUG)
		printf("Submitting next query 0x%p (view <%s>)\n", _map, GetName());
#endif
		_map = NextMapping(_index)->map;
		_queryProc->BatchQuery(_map, _queryFilter, queryCallback, 0,
							   _timestamp);

		return;
	}
	DoneMappingIterator(_index);

	if (GetParentPileStack()) GetParentPileStack()->QueryDone(this);

	_map = 0;
	_index = -1;

    delete _gds;
    _gds = NULL;

    _allStats.Done();
    _allStats.Report();

#if !VIEW_MIN_STATS
	for(int i=0; i<gMaxNumColors; i++)
		_stats[i].Done();
#endif

#if defined(DEBUG)
    printf("_homeAfterQueryDone = %d\n", _homeAfterQueryDone);
#endif
	if (_homeAfterQueryDone) {
		_homeAfterQueryDone = false;
	    GoHome(false);
		// Make sure view redraws even if filter isn't changed.
		// (Fixes bug 482.)

		// Part of kludgey fix for bug 527.  RKW 1999-10-30.
		//TEMP -- will this really work right for piles?  what we really
		// want to do in a pile is go thru all views in the pile once, then
		// set the visual filters and go thru the pile again
		_pileViewHold = false;//TEMP?
		Refresh(false/*TEMP?*/);
	} else {
	    PrepareStatsBuffer(map);
	    DrawLegend();

	    // Finish record links whose master is this view
	    int		index = _masterLink.InitIterator();

	    while(_masterLink.More(index))
		    _masterLink.Next(index)->Done();

	    _masterLink.DoneIterator(index);

		// We can't call ReportQueryDone() if _homeAfterQueryDone is true,
		// because that goofs up the drawing of piles.  RKW 1999-04-05.
	    ReportQueryDone(bytes);
	}
}

void	ViewGraph::PrintLinkInfo(void) 
{
  DOASSERT(_objectValid.IsValid(), "operation on invalid object");
	printf("View : %s \n", GetName());
	printf("Master of ");

	int		index = _masterLink.InitIterator();

	while(_masterLink.More(index))
	{
		RecordLink*		reclink = (RecordLink*)_masterLink.Next(index);

		if (reclink)
			reclink->Print();
	}

	_masterLink.DoneIterator(index);
	printf("\nSlave of ");

	index = _slaveLink.InitIterator();

	while(_slaveLink.More(index))
	{
		RecordLink*		reclink = (RecordLink*)_slaveLink.Next(index);

		if (reclink) 
			reclink->Print();
	}

	_slaveLink.DoneIterator(index);
	printf("\nUpdate Link");
	_updateLink.Print();
}

//******************************************************************************
// Callback Methods (WindowCallback)
//******************************************************************************

// See PileStack::HandlePress().

// Note: the following actions can take place as a result of receiving
// a button press in a view:
// * Nothing (if view is already selected and not moving a cursor)
// * Select view
// * Zoom (rubberband line) (also selects view)
// * Move cursor (click) (only if view is already selected)
// * Drag cursor (also selects view)

// Zooming or dragging a cursor can take place if the view is not already
// selected; moving a cursor with a click can only take place if the view
// *is* already selected.

// If a view is in a pile, having a view in the pile selected is equivalent
// to having all views in the pile selected.

void	ViewGraph::HandlePress(WindowRep *, int x1, int y1,
							   int x2, int y2, int button, int state)
{
  DOASSERT(_objectValid.IsValid(), "operation on invalid object");
#if defined(DEBUG)
    printf("ViewGraph(0x%p, <%s>)::HandlePress(%d, %d, %d, %d, %d)\n", this,
	  GetName(), x1, y1, x2, y2, button);
#endif

  Boolean allowZoom = true;

  //
  // Note: because a key press may end up doing various things depending
  // on the exact state, we have to propagate it to all relevant views
  // even if some actions are disabled.
  //
  if ((WindowRep::GetCursorHit()._hitType == CursorHit::CursorNone)
      && (x1 != x2 || y1 != y2)) {
	// Don't zoom if rubberband region is too small.
	const int minZoomPix = 5;
	if (ABS(x2 - x1) <= minZoomPix || ABS(y2 - y1) <= minZoomPix) {
	  allowZoom = false;
	}

    if (allowZoom && GetRubberbandDisabled()) {
      printf("Rubberband disabled in view <%s>\n", GetName());
	  allowZoom = false;
    }
  }

  if (IsInPileMode()) {
    GetParentPileStack()->HandlePress(NULL, x1, y1, x2, y2, button, state,
	    allowZoom);
  } else {
    DoHandlePress(NULL, x1, y1, x2, y2, button, state, allowZoom);
  }

  // No reason to do this in batch mode (pixmaps only).
  if (!ControlPanel::Instance()->GetBatchMode()) {
    GetWindowRep()->UpdateCursorHit();
  }
}

void	ViewGraph::DoHandlePress(WindowRep *, int x1, int y1,
							     int x2, int y2, int button, int state,
								 Boolean allowZoom)
{
  DOASSERT(_objectValid.IsValid(), "operation on invalid object");
#if defined(DEBUG)
    printf("ViewGraph(0x%p, <%s>)::DoHandlePress(%d, %d, %d, %d, %d)\n", this,
	  GetName(), x1, y1, x2, y2, button);
#endif

    Boolean outsideDataArea = ForceIntoDataArea(x1, y1) &&
	    ForceIntoDataArea(x2, y2);

    int xlow = MIN(x1, x2);
	int ylow = MIN(y1, y2);
	int xhigh = MAX(x1, x2);
	int yhigh = MAX(y1, y2);

	//
	// If we're piled, this was already done by the PileStack object.
	//
	if (!IsInPileMode()) {
	  if ((xlow == xhigh) && (ylow == yhigh) &&
	      WindowRep::GetCursorHit()._hitType == CursorHit::CursorNone &&
		  !outsideDataArea &&
	      CheckCursorOp(xlow, ylow)) {	// Was a cursor event?
        return;
      }

	  // Note: doing the unhighlight and highlight here breaks the dependency
	  // we had on the client doing this for us.  RKW Jan 27, 1998.
      SelectView();
	}

	//
	// Moved this code to after CheckCursorOp() for two reasons:
	// 1. Dragging a cursor now also selects the view.
	// 2. It matches the code in PileStack::HandlePress() better.
	// RKW 1999-10-06.
	//
    if (WindowRep::GetCursorHit()._hitType != CursorHit::CursorNone) {
	  //
	  // We may be dragging a cursor -- check if it's in this view, and
	  // if so, move it.
	  //
	  DeviseCursor *cursor = WindowRep::GetCursorHit()._cursor;
	  if (cursor->GetDst() == this && (x1 != x2 || y1 != y2)) {
#if defined(DEBUG)
        printf("Moving cursor <%s> in view <%s>\n", cursor->GetName(),
		    GetName());
#endif
	    Coord dataX1, dataY1, dataX2, dataY2;
		cursor->FindNewPosition(x1, y1, x2, y2,
		    WindowRep::GetCursorHit()._hitType, dataX1, dataY1,
			dataX2, dataY2);
	    cursor->MoveSource((dataX1 + dataX2) / 2.0,
		    (dataY1 + dataY2) / 2.0,
			ABS(dataX2 - dataX1), ABS(dataY2 - dataY1), true);
	  }

	  return;
	}

	if (_action && allowZoom && (x1 != x2 || y1 != y2))
	{
	    // Convert from screen to world coordinates
		Coord	worldXLow, worldYLow, worldXHigh, worldYHigh;

		FindWorld(xlow, ylow, xhigh, yhigh,
				  worldXLow, worldYLow, worldXHigh, worldYHigh);

		if (state == 8) { // alt
		    // "Inverse" zoom -- a zoom and then an inverse zoom on the same
		    // pixels gets the visual filter back to where it was.
            Coord oldXSize = _filter.xHigh - _filter.xLow;
		    Coord newXSize = worldXHigh - worldXLow;
		    Coord xRatio = oldXSize / newXSize;
		    worldXLow = _filter.xLow + (_filter.xLow - worldXLow) * xRatio;
		    worldXHigh = _filter.xHigh + (_filter.xHigh - worldXHigh) * xRatio;

		    Coord oldYSize = _filter.yHigh - _filter.yLow;
		    Coord newYSize = worldYHigh - worldYLow;
		    Coord yRatio = oldYSize / newYSize;
		    worldYLow = _filter.yLow + (_filter.yLow - worldYLow) * yRatio;
		    worldYHigh = _filter.yHigh + (_filter.yHigh - worldYHigh) * yRatio;
		}
		
		_action->AreaSelected(this, worldXLow, worldYLow, worldXHigh, 
							  worldYHigh, button);
	}
}

void	ViewGraph::HandleKey(WindowRep *, int key, int x, int y)
{
  DOASSERT(_objectValid.IsValid(), "operation on invalid object");
#if defined(DEBUG)
    printf("ViewGraph(%s)::HandleKey(%d, %d, %d)\n", GetName(), key, x, y);
#endif

  // Kludge warning: f/F ("full" cursor) should be allowed even if things
  // that change this view's visual filter are not.  This is kind of a
  // kludgey way of allowing that.
  if (GetKeysDisabled() && key != 'f' && key != 'F') {
    printf("Key actions disabled in view <%s>\n", GetName());
    return;
  }

  if (IsInPileMode()) {
    GetParentPileStack()->HandleKey(NULL, key, x, y);
  } else {
    DoHandleKey(NULL, key, x, y);
  }
}

void	ViewGraph::DoHandleKey(WindowRep *, int key, int x, int y)
{
  DOASSERT(_objectValid.IsValid(), "operation on invalid object");
#if defined(DEBUG)
    printf("ViewGraph(%s)::DoHandleKey(%d, %d, %d)\n", GetName(), key, x, y);
#endif

	if (!IsInPileMode()) {
	    SelectView();
	}

	if (_action)				// Convert from screen to world coordinates
	{
		Coord	worldXLow, worldYLow, worldXHigh, worldYHigh;

		FindWorld(x, y, x, y, worldXLow, worldYLow, worldXHigh, worldYHigh);
		_action->KeySelected(this, key, worldXLow, worldYLow);
	}
}

Boolean		ViewGraph::HandlePopUp(WindowRep* win, int x, int y, int button,
								   char**& msgs, int& numMsgs)
{
  DOASSERT(_objectValid.IsValid(), "operation on invalid object");
#if defined(DEBUG)
	printf("ViewGraph::HandlePopUp at %d,%d, action = 0x%p\n", x, y, _action);
#endif

	int 			labelX, labelY, labelW, labelH;
	static char*	buf[10];

	SelectView();

	if (GetDrillDownDisabled()) {
      printf("Drill down disabled in view <%s>\n", GetName());
      return false;
	}

	GetLabelArea(labelX, labelY, labelW, labelH);

	if ((x >= labelX) && (x <= labelX + labelW - 1) &&
		(y >= labelY) && (y <= labelY + labelH - 1))
	{
		buf[0] = GetName();
		msgs = buf;
		numMsgs = 1;
		return true;
	}

	if (_action)				// Convert from screen to world coordinates
	{
		Coord	worldXLow, worldYLow, worldXHigh, worldYHigh;

		FindWorld(x, y, x + 1, y - 1,
				  worldXLow, worldYLow, worldXHigh, worldYHigh);
		return _action->PopUp(this, worldXLow, worldYLow, worldXHigh,
							  worldYHigh, button, msgs, numMsgs);
	}

	return false;
}

static char *
CountMapAttrToStr(CountMapping::Attr attr)
{
  char *result;

  switch (attr) {
  case CountMapping::AttrX:
	result = "X";
	break;

  case CountMapping::AttrY:
	result = "Y";
	break;

  default:
	result = "";
	reportErrNosys("Illegal or invalid CountMapping attribute");
	break;
  }

  return result;
}

static CountMapping::Attr
StrToCountMapAttr(char *attr)
{
  CountMapping::Attr result;

  if (!strcmp(attr, "X") || !strcmp(attr, "x")) {
	result = CountMapping::AttrX;
  } else if (!strcmp(attr, "Y") || !strcmp(attr, "y")) {
	result = CountMapping::AttrY;
  } else {
	result = CountMapping::AttrInvalid;
	reportErrNosys("Illegal or invalid CountMapping attribute");
  }

  return result;
}

void
ViewGraph::GetCountMapping(Boolean &enabled, char *&countAttr, char *&putAttr)
{
  DOASSERT(_objectValid.IsValid(), "operation on invalid object");
#if defined(DEBUG)
  printf("ViewGraph(%s)::GetCountMapping()\n", GetName());
#endif

  if (_countMapping != NULL) {
	enabled = true;
	CountMapping::Attr count, put;
	_countMapping->GetAttrs(count, put);
	countAttr = CountMapAttrToStr(count);
	putAttr = CountMapAttrToStr(put);
  } else {
	enabled = false;
	countAttr = "";
	putAttr = "";
  }
}

DevStatus
ViewGraph::SetCountMapping(Boolean enabled, char *countAttr, char *putAttr)
{
  DOASSERT(_objectValid.IsValid(), "operation on invalid object");
#if defined(DEBUG)
  printf("ViewGraph(%s)::SetCountMapping(%d, %s, %s)\n", GetName(), enabled,
	countAttr, putAttr);
#endif

  DevStatus status = StatusOk;

  Boolean deleteOld = false;
  Boolean createNew = false;
  Boolean refresh = false;

  CountMapping::Attr newCount;
  CountMapping::Attr newPut;

  if (enabled) {
    newCount = StrToCountMapAttr(countAttr);
    newPut = StrToCountMapAttr(putAttr);
    if (newCount == CountMapping::AttrInvalid ||
	    newPut == CountMapping::AttrInvalid) {
	  //
	  // Bad value(s) for new count attribute(s).
	  //
	  status = StatusFailed;
    } else {
	  if (_countMapping != NULL) {
	    CountMapping::Attr oldCount, oldPut;
	    _countMapping->GetAttrs(oldCount, oldPut);
	    if (newCount != oldCount || newPut != oldPut) {
		  //
		  // Both enabled, attributes differ.
		  //
		  deleteOld = true;
		  createNew = true;
		  refresh = true;
		}
	  } else {
		//
		// Old disabled, new enabled.
		//
		createNew = true;
		refresh = true;
	  }
	}
  } else {
	if (_countMapping != NULL) {
	  //
	  // Old enabled, new disabled.
	  //
	  deleteOld = true;
	  refresh = true;
	}
  }

  if (deleteOld) {
    delete _countMapping;
    _countMapping = NULL;
  }
  if (createNew) {
    _countMapping = new CountMapping(newCount, newPut);
  }
  if (refresh) {
    Refresh();
  }

  return status;
}

void
ViewGraph::SetStringTable(TDataMap::TableType type, char *name)
{
  DOASSERT(_objectValid.IsValid(), "operation on invalid object");
  char **tableNameP = TableType2NameP(type);

  FreeString(*tableNameP);
  *tableNameP = CopyString(name);

  // Note: this will cause problems if a mapping is shared by multiple
  // views.  RKW 1998-11-03.
  int index = InitMappingIterator();
  while (MoreMapping(index)) {
    MappingInfo *info = NextMapping(index);
    info->map->SetStringTable(type, *tableNameP);
  }
  DoneMappingIterator(index);

  Refresh();
}

char **
ViewGraph::TableType2NameP(TDataMap::TableType type)
{
  DOASSERT(_objectValid.IsValid(), "operation on invalid object");
  char **tableNameP;

  switch (type) {
  case TDataMap::TableX:
    tableNameP = &_stringXTableName;
    break;

  case TDataMap::TableY:
    tableNameP = &_stringYTableName;
    break;

  case TDataMap::TableZ:
    tableNameP = &_stringZTableName;
    break;

  case TDataMap::TableGen:
    tableNameP = &_stringGenTableName;
    break;
  
  default:
    DOASSERT(0, "Invalid TableType");
    break;
  }

  return tableNameP;
}



void
ViewGraph::UpdateAxisTypes()
{
  DOASSERT(_objectValid.IsValid(), "operation on invalid object");

  MappingInfo *mapInfo = _mappings.GetLast();
  if (mapInfo) {
    TDataMap *map = mapInfo->map;
    if (map) {
      AttrList *attrList = map->GDataAttrList();
      if (attrList) {
        AttrInfo *info = attrList->Find(gdataXName);
        if (info && info->type == DateAttr) {
          SetXAxisAttrType(DateAttr);
	} else {
          SetXAxisAttrType(FloatAttr);
	}

        info = attrList->Find(gdataYName);
        if (info && info->type == DateAttr) {
          SetYAxisAttrType(DateAttr);
        } else {
          SetYAxisAttrType(FloatAttr);
	}

        info = attrList->Find(gdataZName);
        if (info && info->type == DateAttr) {
          SetZAxisAttrType(DateAttr);
        } else {
          SetZAxisAttrType(FloatAttr);
	}
      }
    }
  }
}

Boolean
ViewGraph::GetDupElim()
{
  DOASSERT(_objectValid.IsValid(), "operation on invalid object");
  return _dupElim != NULL;
}

void
ViewGraph::SetDupElim(Boolean enable)
{
  DOASSERT(_objectValid.IsValid(), "operation on invalid object");
  if (enable) {
    if (!_dupElim) {
      _dupElim = new DupElim();
      Refresh();
    }
  } else {
    if (_dupElim) {
      delete _dupElim;
      _dupElim = NULL;
      Refresh();
    }
  }
}

void
ViewGraph::GetNiceAxes(Boolean &niceX, Boolean &niceY)
{
  DOASSERT(_objectValid.IsValid(), "operation on invalid object");
#if defined(DEBUG)
  printf("ViewGraph(%s)::GetNiceAxes()\n", GetName());
#endif

  niceX = _niceXAxis;
  niceY = _niceYAxis;
}

void
ViewGraph::SetNiceAxes(Boolean niceX, Boolean niceY)
{
  DOASSERT(_objectValid.IsValid(), "operation on invalid object");
#if defined(DEBUG)
  printf("ViewGraph(%s)::SetNiceAxes(%d, %d)\n", GetName(), niceX, niceY);
#endif

  _niceXAxis = niceX;
  _niceYAxis = niceY;
}

void
ViewGraph::NiceifyAxes(Boolean xAxis, Boolean yAxis)
{
  DOASSERT(_objectValid.IsValid(), "operation on invalid object");
#if defined(DEBUG)
  printf("ViewGraph(%s)::NiceifyAxes(%d, %d)\n", GetName(), xAxis, yAxis);
#endif

  VisualFilter filter;
  GetVisualFilter(filter);

  if (xAxis) NiceAxisRange(filter.xLow, filter.xHigh);
  if (yAxis) NiceAxisRange(filter.yLow, filter.yHigh);

  SetVisualFilter(filter);
}

void
ViewGraph::NiceAxisRange(Coord &low, Coord &high)
{
#if defined(DEBUG)
  printf("ViewGraph::NiceAxisRange(%g, %g)\n", low, high);
#endif

  Coord diff = high - low;

  if (diff > 0.0) {
    // This constant determines the "threshold" for going up or down by a
    // factor of 10 on the multiple.
    const double multFudge = 0.2;

    double logval = floor(log10(diff) - multFudge);
    double multiple = pow(10.0, logval);

#if defined(DEBUG)
    printf("multiple = %g\n", multiple);
#endif

    // This constant should be just less than one so we don't go up to
    // a higher value if the given value is already exactly a multiple
    // of multiple.
    const double roundFudge = 0.99;
    high = floor(high/multiple + roundFudge) * multiple;
    low = floor(low/multiple) * multiple;
  }
  
#if defined(DEBUG)
  printf("  resulting range: (%g, %g)\n", low, high);
#endif
}

void
ViewGraph::RefreshAndHome()
{
  DOASSERT(_objectValid.IsValid(), "operation on invalid object");
#if defined(DEBUG)
  printf("ViewGraph(%s)::RefreshAndHome()\n", GetName());
#endif

  Refresh();
  _homeAfterQueryDone = true;
}

//---------------------------------------------------------------------------
DevStatus
ViewGraph::SwitchTData(const char *tdName)
{
  DOASSERT(_objectValid.IsValid(), "operation on invalid object");
#if defined(DEBUG)
  printf("ViewGraph(%s)::SwitchTData(%s)\n", GetName(), tdName);
#endif

  DevStatus result = StatusOk;
  char errBuf[1024];

  // TEMP -- possibly allow a view to not have a TData assigned

  //
  // Make sure the TData name is (possibly) valid.
  //
  if (!tdName || !strcmp("", tdName)) {
    reportErrNosys("Invalid (null or blank) TData name");
	result = StatusFailed;
  }

  char namebuf[1024];
  if (tdName[0] != '.') {
    // DTE goes nuts if first char of TData name isn't '.'.
    sprintf(namebuf, ".%s", tdName);
	tdName = namebuf;
  }

  //
  // Make sure the view has exactly one mapping.
  //
  if (result.IsComplete()) {
    if (_mappings.Size() == 0) {
      sprintf(errBuf, "View <%s> has no mapping; cannot switch TData",
          GetName());
      reportErrNosys(errBuf);
      result = StatusFailed;
    } else if (_mappings.Size() > 1) {
      sprintf(errBuf, "View <%s> has multiple mappings; cannot switch TData",
          GetName());
      reportErrNosys(errBuf);
      result = StatusFailed;
    }
  }

  if (result.IsComplete()) {
    //
    // Make sure we don't already have the requested TData.
    //
    TDataMap *tdMap = GetFirstMap();
    TData *oldTData = tdMap->GetLogTData();
    if (!strcmp(tdName, oldTData->GetName())) {
#if defined(DEBUG)
      printf("View <%s> already has TData %s\n", GetName(), tdName);
#endif
    } else {

	  //
	  // Try to create a new mapping that's the same as the existing one
	  // except for the TData.  Note that creating the mapping this way
	  // automatically creates the TData if necessary.
	  //
      // Note that we must create the new mapping via the class directory so
	  // that the user will be able to edit it.  RKW 1999-04-13.
	  //
	  // TEMP -- should we check here whether the mapping already exists?
      MappingInterp *oldMapping, *newMapping;

	  ClassDir *classDir = ControlPanel::GetClassDir();
      ClassInfo *oldInfo = classDir->FindClassInfo(tdMap->GetName());

	  // Get creation arguments of old mapping.
      int argc;
	  const char **argv;
	  oldInfo->CreateParams(argc, argv);

	  // Change the TData name and mapping name.
	  argv[0] = tdName;

      char newName[1024];
      sprintf(newName, "%s#%s", tdName, GetName());
      argv[1] = newName;
#if defined(DEBUG)
      printf("Creating new mapping: <%s>\n", newName);
#endif

      // This is kind of a kludgey way to check for errors in the constructor.
      // RKW 1999-04-13.
	  // Note: we're no longer checking DevError::GetLatestError() here
	  // because we may get an 'index file invalid' error, but things
	  // actually work.  RKW 1999-08-17.
	  const char *instName = classDir->CreateWithParams(oldInfo->CategoryName(),
	      oldInfo->ClassName(), argc, argv);

	  Boolean failed = false;
      if (!instName || strcmp(instName, newName)) {
	    failed = true;
	  } else {
        newMapping = (MappingInterp *)classDir->FindInstance(instName);
		if (!newMapping) {
	      failed = true;
		} else if (!newMapping->GetLogTData()) {
	      failed = true;
		} else if (strcmp(tdName, newMapping->GetLogTData()->GetName())) {
	      failed = true;
		}
	  }

	  if (failed) {
		char errBuf[1024];
		sprintf(errBuf, "Unable to create mapping %s; not switching TData "
		    "of view <%s>", newName, GetName());
	    classDir->DestroyInstance(newName);
	    result = StatusFailed;
      } else {
	    oldMapping = (MappingInterp *)oldInfo->GetInstance();

		newMapping->SetStringTable(TDataMap::TableX, _stringXTableName);
		newMapping->SetStringTable(TDataMap::TableY, _stringYTableName);
		newMapping->SetStringTable(TDataMap::TableZ, _stringZTableName);
		newMapping->SetStringTable(TDataMap::TableGen, _stringGenTableName);

		newMapping->SetPixelWidth(oldMapping->GetPixelWidth());

		newMapping->SetParentValue(oldMapping->GetParentValue());
	  }

	  //
	  // If we successfully created the new mapping, switch mappings and refresh
	  // the view; then destroy the old mapping.
	  //
      if (result.IsComplete()) {
        RemoveMapping(oldMapping);
        InsertMapping(newMapping);
        if (AutoUpdateFilter()) {
          RefreshAndHome();
        } else {
          Refresh();
        }

		// This assumes that no other view uses the mapping...
	    classDir->DestroyInstance(oldMapping->GetName());
	    oldMapping = NULL;
      }
    }
  }

#if defined(DEBUG)
      printf("  Returning %s\n", result.Value());
#endif

  return result;
}

//---------------------------------------------------------------------------
DevStatus
ViewGraph::SetParentValue(const char *parentVal)
{
  DOASSERT(_objectValid.IsValid(), "operation on invalid object");
#if defined(DEBUG)
  printf("ViewGraph(%s)::SetParentValue(%s)\n", GetName(), parentVal);
#endif

  DevStatus result = StatusOk;

  // Note: we shouldn't really have to check against map->GetParentValue()
  // here, but I'm doing it for extra safety.  RKW 2000-03-10.
  TDataMap *map = GetFirstMap();
  if (!_viewSymParentVal || strcmp(_viewSymParentVal, parentVal) ||
      strcmp(map->GetParentValue(), parentVal)) {
    //TEMP -- what if > 1 maps?

    map->SetParentValue(parentVal);

    if (_viewSymParentVal) FreeString(_viewSymParentVal);
	_viewSymParentVal = CopyString(parentVal);
  }

  return result;
}

//---------------------------------------------------------------------------
DevStatus
ViewGraph::SaveViewSymFilter()
{
  DOASSERT(_objectValid.IsValid(), "operation on invalid object");
#if defined(DEBUG)
  printf("ViewGraph(%s)::SaveViewSymFilter()\n", GetName());
  VisualFilter *tmpFilter = GetVisualFilter();
  printf("  Visual filter: (%g, %g), (%g, %g)\n", tmpFilter->xLow,
      tmpFilter->yLow, tmpFilter->xHigh, tmpFilter->yHigh);
#endif

  DevStatus result = StatusOk;

  TData *tdata = NULL;
  TDataMap *tdMap = GetFirstMap();
  if (tdMap) tdata = tdMap->GetLogTData();

  const char *parentVal = _viewSymParentVal;
  // In case no parent value is specified.
  if (!parentVal) parentVal = "(null)";

  if (tdata && _viewSymParentVal) {
    _viewSymFilterInfo->Save(tdata->GetName(), _viewSymParentVal,
        GetVisualFilter());
  }

  return result;
}

//---------------------------------------------------------------------------
DevStatus
ViewGraph::RestoreViewSymFilter()
{
  DOASSERT(_objectValid.IsValid(), "operation on invalid object");
#if defined(DEBUG)
  printf("ViewGraph(%s)::RestoreViewSymFilter()\n", GetName());
#endif

  DevStatus result = StatusOk;

  TData *tdata = NULL;
  TDataMap *tdMap = GetFirstMap();
  if (tdMap) tdata = tdMap->GetLogTData();
  if (tdata && _viewSymParentVal) {

    const VisualFilter *filter = _viewSymFilterInfo->Find(tdata->GetName(),
        _viewSymParentVal);
	if (filter) {
      SetVisualFilter(*filter);
	}
  }

  return result;
}

//---------------------------------------------------------------------------
void
ViewGraph::GetJSSendP(Boolean &drawToScreen, Boolean &sendToSocket,
    GDataSock::Params &params)
{
  DOASSERT(_objectValid.IsValid(), "operation on invalid object");
#if defined(DEBUG)
  printf("ViewGraph(%s)::GetJSSendP()\n", GetName());
#endif

  drawToScreen =  _jsDrawToScreen;
  sendToSocket = _jsSendToSocket;
  params = _jsGdsParams;
}

//---------------------------------------------------------------------------
void
ViewGraph::SetJSSendP(Boolean drawToScreen, Boolean sendToSocket,
    const GDataSock::Params &params)
{
  DOASSERT(_objectValid.IsValid(), "operation on invalid object");
#if defined(DEBUG)
  printf("ViewGraph(%s)::SetJSSendP()\n", GetName());
#endif

  _jsDrawToScreen = drawToScreen;
  _jsSendToSocket = sendToSocket;
  _jsGdsParams = params;
  _jsGdsParams.file = CopyString(_jsGdsParams.file);

  if (Session::GetIsJsSession()) {
    SetDrawToScreen(_jsDrawToScreen);
    SetSendToSocket(_jsSendToSocket);
    SetSendParams(_jsGdsParams);
    Refresh();
  }
}

//******************************************************************************

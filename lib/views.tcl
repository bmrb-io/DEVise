#  ========================================================================
#  DEVise Data Visualization Software
#  (c) Copyright 1992-1996
#  By the DEVise Development Group
#  Madison, Wisconsin
#  All Rights Reserved.
#  ========================================================================
#
#  Under no circumstances is this software to be copied, distributed,
#  or altered in any way without prior permission from the DEVise
#  Development Group.

############################################################

#  $Id$

#  $Log$
#  Revision 1.60  1999/11/17 15:08:47  wenger
#  Fixed bug 531 (copying view doesn't copy JavaScreen GData params); also
#  added copying of other missing values.
#
#  Revision 1.59  1999/07/27 15:53:07  wenger
#  Fixed bug 504.
#
#  Revision 1.58  1999/07/14 18:43:04  wenger
#  Added the capability to have axes without ticks and tick labels.
#
#  Revision 1.57  1999/04/07 18:42:18  wenger
#  Fixed bug 481 and other problems in view copying.
#
#  Revision 1.56  1999/02/17 15:10:07  wenger
#  Added "Next in Pile" button to query dialog; more pile fixes; fixed bug
#  in mapping dialog updating when a view is selected.
#
#  Revision 1.55  1998/11/17 14:48:10  wenger
#  Changed master/slave to leader/follower and fixed a few problems in GUI,
#  session description, etc.
#
#  Revision 1.54  1998/10/20 19:46:35  wenger
#  Mapping dialog now displays the view's TData name; "Next in Pile" button
#  in mapping dialog allows user to edit the mappings of all views in a pile
#  without actually flipping them; user has the option to show all view names;
#  new GUI to display info about all links and cursors; added API and GUI for
#  count mappings.
#
#  Revision 1.53  1998/08/03 15:35:25  wenger
#  Added note about disabling menus.
#
#  Revision 1.52  1998/07/17 15:33:57  wenger
#  Improved link creation GUI as per request from Raghu; started on
#  DataReader schema GUI; changed version to 1.5.4.
#
#  Revision 1.51  1998/06/15 19:55:30  wenger
#  Fixed bugs 338 and 363 (problems with special cases of set links).
#
#  Revision 1.50  1998/06/12 19:55:44  wenger
#  Attribute of TAttr/set links can now be changed; GUI has menu of available
#  attributes; attribute is set when master view is set instead of at link
#  creation; misc. debug code added.
#
#  Revision 1.49  1998/04/27 17:30:34  wenger
#  Improvements to TAttrLinks and related code.
#
#  Revision 1.48  1998/04/10 18:29:52  wenger
#  TData attribute links (aka set links) mostly implemented through table
#  insertion; a crude GUI for creating them is implemented; fixed some
#  bugs in link GUI; changed order in session file for TData attribute links.
#
#  Revision 1.47  1998/02/26 19:05:33  beyer
#  updated histogram dialog
#
#  Revision 1.46  1998/02/20 06:17:23  beyer
#  resurected histograms
#
#  Revision 1.45  1998/02/03 23:46:51  wenger
#  Fixed a problem Hongyu had with getting GData on socket; fixed bugs
#  283 and 285 (resulted from problems in color manager merge);
#  conditionaled out some debug output.
#
#  Revision 1.44  1998/01/27 23:04:45  wenger
#  Broke the server's view selection dependency on the client (except when
#  running in collaboration mode).
#
#  Revision 1.43  1998/01/14 22:24:47  wenger
#  Changed 'Bring View Back' to 'Restore View'.
#
#  Revision 1.42  1997/11/24 23:15:48  weaver
#  Changes for the new ColorManager.
#
#  Revision 1.41  1997/08/21 21:06:39  donjerko
#  Added Materialize button, added isDTEType to some code in views.tcl
#
#  Revision 1.40  1997/07/22 18:41:13  wenger
#  Workaround for bug 210: GUI disallows link names starting with 'Pile:'.
#
#  Revision 1.39  1997/07/15 14:59:28  wenger
#  Fixed a bug in the view destroying Tcl code; user can now destroy a window
#  that contains views (DEVise asks for confirmation, destroys the views).
#
#  Revision 1.38  1997/06/10 19:32:20  wenger
#  Copy/Edit GUI doesn't bring up window creation GUI until user clicks 'OK'.
#
#  Revision 1.37  1997/06/09 14:47:16  wenger
#  Added cursor grid; fixed bug 187; a few minor cleanups.
#
#  Revision 1.36  1997/05/30 15:41:39  wenger
#  Most of the way to user-configurable '4', '5', and '6' keys -- committing
#  this stuff now so it doesn't get mixed up with special stuff for printing
#  Mitre demo.
#
#  Revision 1.35  1997/04/21 23:11:57  guangshu
#    1. Make statistics deal with different DATE types.
#    2. Make Aggregates on TData and GData.
#    3. Change Max Avg Min to be HighLow shape
#    4. Improved title.
#    5. etc.
#  Revision 1.35.6.1  1997/05/21 20:41:16  weaver
#  Changes for new ColorManager
#
#  Revision 1.34  1997/03/22 00:00:14  guangshu
#  Monor fix.
#
#  Revision 1.33  1997/03/21 17:49:26  wenger
#  Fixed bug in Tcl code for link creation.
#
#  Revision 1.32  1997/03/21 16:18:00  guangshu
#  Minor change for Date.
#
#  Revision 1.31  1997/03/20 22:38:49  guangshu
#  Added new statistics interface for histogram, groupby and basic stats.
#
#  Revision 1.30  1997/03/20 20:46:09  donjerko
#  DTE Tdata generates unique names by appending sequential numbers to the
#  end of the table name. This way, same table can be opened multiple times.
#
#  Revision 1.29  1997/03/20 01:32:20  wenger
#  Fixed a bug in color allocation; color chooser for data shows old colors
#  (temporarily); background conversion of GData defaults to off.
#
#  Revision 1.28  1997/03/02 00:47:29  donjerko
#  Tdata used to be created before "Define Visualisation" window.
#  For DTE types, it is now being created after this window because the
#  user options can make query more restrictive and therefore cheaper to execute.
#  This screws up the Mapping dialog because only the restricted set of
#  attributes is passed as tdata.
#  This needs to be fixed once we aggree how to go about it.
#
#  Revision 1.27  1997/02/26 16:32:07  wenger
#  Merged rel_1_3_1 through rel_1_3_3c changes; compiled on Intel/Solaris.
#
#  Revision 1.26  1997/02/14 16:48:09  wenger
#  Merged 1.3 branch thru rel_1_3_1 tag back into the main CVS trunk.
#
#  Revision 1.25  1997/02/03 20:02:02  ssl
#  Added interface for negative record links and user defined layout mode
#
#  Revision 1.24  1997/02/03 04:12:42  donjerko
#  Catalog management moved to DTE
#
#  Revision 1.23.4.8  1997/02/21 23:23:27  ssl
#  Made various updates to the UI for links :
#  1) Added info button in various places and in the main menu
#  2) Added loops around various operations to make them more intuitive
#  3) New button in Setlink master now creates a rec link by default
#  4) Info is displayed even if a link is empty
#  5  Removed the double OK buttons from the ResetLinkMaster window
#
#  Revision 1.23.4.7  1997/02/19 23:01:28  wenger
#  A few minor changes in the shape menu stuff to make the GUI more consistent
#  with the rest of Devise; added some more info to the shape help;
#  shape menu doesn't disable shape attributes (extra ones sometimes needed
#  for 3D, etc.); fixed a bug in how Cancel works in the Color Chooser;
#  various bug fixes in shape attribute menus.
#
#  Revision 1.23.4.6  1997/02/13 18:17:08  ssl
#  Added check to UI when user links two different data sources with a record link
#
#  Revision 1.23.4.5  1997/02/11 16:49:17  ssl
#  Fixed minor problem with links
#
#  Revision 1.23.4.4  1997/02/11 01:17:40  ssl
#  Cleaned up the UI for piled views
#  1) Made pile links invisible to user
#  2) Added create/destroy link options to the link menu
#  3) Enhanced the link info window to show all info about a link (type, views,
#     master, link params )
#  4) Pile links get removed when the pile is unpiled
#  5) Set/Reset Master now only shows list of record links
#
#  Revision 1.23.4.3  1997/02/09 20:14:21  wenger
#  Fixed bug 147 (or at least some instances of it) -- found a bug in the
#  query processor that caused it to miss records it should have found;
#  fixed bugs 151 and 153.
#
#  Revision 1.23.4.2  1997/02/08 02:08:42  ssl
#  1)Fixed mapping dialog to update when selected view changes
#  2)Removed  OK buttons and renamed Cancel to Close
#  3)Added a Flip button which allows user to flip views whenever the view
#    is a pile. NOT ENABLED FOR STACKs or other views.
#
#  Revision 1.23.4.1  1997/02/07 15:21:48  wenger
#  Updated Devise version to 1.3.1; fixed bug 148 (GUI now forces unique
#  window names); added axis toggling and color selections to Window menu;
#  other minor fixes to GUI; show command to Tasvir now requests image to
#  be shown all at once.
#
#  Revision 1.23  1997/01/22 23:46:26  jussi
#  Removed references to queryWinOpened, query3dWinOpened,
#  historyWinOpened and stackWinOpened. Replaced them with
#  calls to WindowExists.
#
#  Revision 1.22  1997/01/22 20:13:59  wenger
#  Removed other non-functional user interface components (includes workaround
#  for bug 127); fixed a number of OK/Cancel button positions.
#
#  Revision 1.21  1997/01/14 20:05:56  wenger
#  Fixed some compile warnings; fixed relative positions of OK/Cancel
#  buttons in link GUI; removed some debug code I accidentally left
#  in place.
#
#  Revision 1.20  1996/11/25 22:31:36  beyer
#  1. extended .devise.rc search
#  2. added DestroyView command
#  3. query window updated properly, history window update changed
#  4. filter properly set to (0,100) instead of (100,0) when high,low values
#     are not known.
#
#  Revision 1.19  1996/08/29 22:32:31  guangshu
#  Removed some unnecessary comment.
#
#  Revision 1.18  1996/08/07 15:44:36  guangshu
#  Added updating statistics in query box.
#
#  Revision 1.17  1996/07/23 17:27:00  jussi
#  Fixed bug in ViewUnlink.
#
#  Revision 1.16  1996/07/13 17:30:59  jussi
#  Minor change.
#
#  Revision 1.15  1996/07/13 00:35:23  jussi
#  Added check for return value from OpenAndDefineDataSources.
#
#  Revision 1.14  1996/07/13 00:21:25  jussi
#  Added DoColorStat.
#
#  Revision 1.13  1996/07/01 19:33:31  jussi
#  Minor improvements. Added DoSetRecordCursor procedure, but
#  it is yet to be implemented.
#
#  Revision 1.12  1996/06/20 17:12:53  guangshu
#  Added DoStat procedure.
#
#  Revision 1.11  1996/06/15 14:43:46  jussi
#  Added yuc's changes for 3D support.
#
#  Revision 1.10  1996/06/15 14:26:28  jussi
#  Added DoSetFgColor procedure.
#
#  Revision 1.9  1996/06/13 20:15:26  jussi
#  Fixed bug in DoViewRemove.
#
#  Revision 1.8  1996/05/31 15:48:34  jussi
#  Added DoSetLinkMaster and DoResetLinkMaster procedures.
#
#  Revision 1.7  1996/05/13 18:08:48  jussi
#  Removed ProcessViewCreated and ProcessViewSelected procedures
#  because they were empty and not used.
#
#  Revision 1.6  1996/04/13 22:00:10  jussi
#  When copying a view, the user is no longer asked for the name of
#  the new view. It is automatically generated.
#
#  Revision 1.5  1996/04/11 18:23:03  jussi
#  Major changes in the organization of the user interface.
#
#  Revision 1.4  1996/03/07 16:55:39  jussi
#  Added support for changing number of dimensions in view.
#
#  Revision 1.3  1996/02/02 21:27:03  jussi
#  User is given a palette of colors to choose from rather than
#  a list of names of colors.
#
#  Revision 1.2  1996/01/30 21:09:36  jussi
#  Added DoSetBgColor but made it inactive because view parameters
#  cannot be changed yet. Moved some procedures from control.tk.
#
#  Revision 1.1  1996/01/23 20:50:59  jussi
#  Initial revision.
#

# 0 = not doing anything; 1 = selecting first view for link; 2 = selecting
# second view for link
set linkViewStatus 0

# Name of link we are creating.
set currentLink ""

############################################################

proc RefreshAllViews {} {
    set classes [ DEVise get "view" ]
    foreach class $classes {
	set instances [ DEVise get "view" $class ]
	foreach inst $instances {
	    DEVise refreshView $inst
	}
    }
}

############################################################

proc ProcessViewSelected { view } {
    global curView lastView

    if {$view == $curView} {
        LinkSelectedView
	return
    }

    set lastView $curView

    if {$curView != ""} {
	# unhighlight old view
	# Note: because of changes to ViewGraph, this command is now needed
	# only for collaboration.  RKW Jan. 27, 1998.
	DEVise highlightView $curView 0
    }
 
    set curView $view

    Query_ViewSelected
    Update3DLocation
    Update2DQueryWindow
    Update2DBasicStatWindow
    UpdateHistoryWindow

    # Moved this up to here so the mapping dialog gets cleared if no view
    # is selected (when closing a session, for example).  RKW 1998-10-14.
    UpdateMappingDialog .editMapping $curView

    if {$curView == ""} {
	if {[WindowExists .stack]} {
	    .stack.title.text configure -text "No View Selected"
	}
	.mbar.gdata configure -state disabled
	.mbar.view configure -state disabled
	.panel.query.button2 configure -state disabled

	# there seems to be a problem restoring a new session when
	# the editMapping dialog is killed in the previous session
	# catch { destroy .editMapping }

	return
    }

    if {[WindowExists .stack]} {
	.stack.title.text configure -text "View: $curView"
    }
    .mbar.gdata configure -state normal
    .mbar.view configure -state normal
    .panel.query.button2 configure -state normal

    # highlight new view
    # Note: because of changes to ViewGraph, this command is now needed
    # only for collaboration.  RKW Jan. 27, 1998.
    DEVise highlightView $curView 1

    LinkSelectedView
}

############################################################

# Process View/Create menu
proc DoViewCreate {} {
    set ans [ DoCreateView "Select a view type" ]
}

############################################################

# Copy/edit view
proc DoViewCopy {} {
    global curView tdata window gdata link view

    if {[WindowVisible .copy]} {
	return
    }

    if {![CurrentView]} {
	return
    }

    set view $curView

    set gdataSet [GdataSet]
    set tdataSet [TdataSet]
    set windowSet [WinSet]

    set gdata [lindex [DEVise getViewMappings $view] 0]
    set gdataClass [GetClass mapping $gdata]
    set gdataParam [DEVise getCreateParam mapping $gdataClass $gdata]
    set tdata [lindex [DEVise getCreateParam mapping $gdataClass $gdata] 0]
    set window [DEVise getViewWin $view]

    # create default name for new mapping

    set gdataClass [GetClass mapping $gdata]
    set newGdata [GetMapping $tdata $gdataClass]

    set alwaysNewGdata 1

    if {$alwaysNewGdata || $newGdata == ""} {
	# create a new mapping
	set gdataParam [DEVise getCreateParam mapping $gdataClass $gdata]
	set newGdata [UniqueName "$tdata#$gdataClass"]
    }

    toplevel .copy
    wm title .copy "Copy View"
    wm geometry .copy +50+50

    frame .copy.top
    frame .copy.bot
    pack .copy.top -side top -pady 3m -fill both -expand 1
    pack .copy.bot -side top -pady 5m -fill x

    frame .copy.top.old -relief groove -bd 2
    frame .copy.top.new -relief groove -bd 2
    pack .copy.top.old .copy.top.new -side top -pady 1m -fill both -expand 1

    frame .copy.top.old.tdata
    frame .copy.top.old.gdata
    frame .copy.top.old.window
    pack .copy.top.old.tdata .copy.top.old.gdata .copy.top.old.window \
	    -pady 3m -side top -fill x -expand 1

    frame .copy.top.new.gdata
    pack .copy.top.new.gdata -pady 3m -side top -fill x -expand 1

    label .copy.top.old.tdata.label -text "Source Data:" -width 13
    menubutton .copy.top.old.tdata.menu -relief raised -textvariable tdata \
	    -menu .copy.top.old.tdata.menu.list
    pack .copy.top.old.tdata.label -padx 2m -side left
    pack .copy.top.old.tdata.menu -padx 2m -side left -fill x -expand 1

    label .copy.top.old.gdata.label -text "Mapping:" -width 13
    menubutton .copy.top.old.gdata.menu -relief raised -textvariable gdata \
	    -menu .copy.top.old.gdata.menu.list
    pack .copy.top.old.gdata.label -padx 2m -side left
    pack .copy.top.old.gdata.menu -padx 2m -side left -fill x -expand 1

    label .copy.top.old.window.label -text "Window:" -width 13
    menubutton .copy.top.old.window.menu -relief raised -textvariable window \
	    -menu .copy.top.old.window.menu.list
    pack .copy.top.old.window.label -padx 2m -side left
    pack .copy.top.old.window.menu -padx 2m -side left -fill x -expand 1

    label .copy.top.new.gdata.label -text "Mapping Name:" -width 13
    entry .copy.top.new.gdata.entry -relief sunken
    .copy.top.new.gdata.entry insert 0 $newGdata
    pack .copy.top.new.gdata.label -padx 2m -side left
    pack .copy.top.new.gdata.entry -padx 2m -side left -fill x -expand 1

    menu .copy.top.old.tdata.menu.list -tearoff 0
    foreach t $tdataSet {
	.copy.top.old.tdata.menu.list add command -label $t \
		-command "set tdata {$t}"
    }
    .copy.top.old.tdata.menu.list add separator
    .copy.top.old.tdata.menu.list add command -label "Other..." -command {
	set newtdata [OpenNewDataSource 0]
	if {$newtdata != ""} {
	    set tdata $newtdata
	    .copy.top.old.tdata.menu.list add command -label $tdata \
		    -command "set tdata {$tdata}"
	}
    }

    menu .copy.top.old.gdata.menu.list -tearoff 0
    foreach g $gdataSet {
	.copy.top.old.gdata.menu.list add command -label $g \
		-command "set gdata {$g}"
    }

    menu .copy.top.old.window.menu.list -tearoff 0
    foreach w $windowSet {
	.copy.top.old.window.menu.list add command -label $w \
		-command "set window {$w}"
    }
    .copy.top.old.window.menu.list add separator
    .copy.top.old.window.menu.list add command -label "New..." -command {
	set window "New"
    }

    frame .copy.bot.but
    pack .copy.bot.but -side top

    button .copy.bot.but.ok -text OK -width 10 -command {
	if {$window == "New"} {
	    set newwin [DoCreateWindow "Select window type"]
	    if {$newwin != ""} {
	        set window $newwin
	    } else {
		puts "Unable to create new window"
		return
	    }
	}
	set newGdata [.copy.top.new.gdata.entry get]
	if {$newGdata == ""} {
	    dialog .noViewName "No Mapping Name" \
		    "Please specify mapping name." "" 0 OK
	    return
	}
	DoActualViewCopy $view $tdata $gdata $newGdata $window
	destroy .copy
    }
    button .copy.bot.but.cancel -text Cancel -width 10 -command {
	destroy .copy
    }
    pack .copy.bot.but.ok .copy.bot.but.cancel \
	    -side left -padx 3m
}

############################################################

proc DoActualViewCopy {view tdata gdata newGdata window} {

    # need to add check that tdata(view) is same type as tdata

    if {![DEVise exists $newGdata]} {
	# create a new mapping
	set gdataClass [GetClass mapping $gdata]
	set gdataParam [DEVise getCreateParam mapping $gdataClass $gdata]
	set newGdataParam [linsert [lrange $gdataParam 2 end] 0 $tdata \
		$newGdata]
	set cmd "DEVise create mapping $gdataClass $newGdataParam"
	set result [eval $cmd]
	if {$result == ""} {
	    dialog .copyError "Mapping Error" \
		    "Cannot create mapping." "" 0 OK
	    return
	}
    }

    set class [GetClass view $view]
    set params [DEVise getCreateParam view $class $view]
    set newView [UniqueName $view]
    set newParams [linsert [lrange $params 1 end] 0 $newView]
    eval DEVise create view $class $newParams

    set viewLabelParams [DEVise getLabel $view]
    eval DEVise setLabel {$newView} $viewLabelParams
    set viewStatParams [DEVise getViewStatistics $view]
    eval DEVise setViewStatistics {$newView} $viewStatParams

    eval DEVise setViewDimensions {$newView} [DEVise getViewDimensions $view]
    eval DEVise setViewSolid3D {$newView} [DEVise getViewSolid3D $view]
    eval DEVise setViewXYZoom {$newView} [DEVise getViewXYZoom $view]
    eval DEVise setViewDisplayDataValues {$newView} \
      [DEVise getViewDisplayDataValues $view]

    # View fonts.
    eval DEVise setFont {$newView} {title} [DEVise getFont $view "title"]
    eval DEVise setFont {$newView} {{x axis}} [DEVise getFont $view "x axis"]
    eval DEVise setFont {$newView} {{y axis}} [DEVise getFont $view "y axis"]

    # View home and implicit home.
    eval DEVise viewSetHome {$newView} [DEVise viewGetHome $view]
    eval DEVise viewSetImplicitHome {$newView} [DEVise viewGetImplicitHome \
      $view]

    # Horizontal and vertical panning parameters.
    eval DEVise viewSetHorPan {$newView} [DEVise viewGetHorPan $view]
    eval DEVise viewSetVerPan {$newView} [DEVise viewGetVerPan $view]

    # GData sending parameters.
    eval DEVise setViewGDS {$newView} [DEVise getViewGDS $view]
    eval DEVise viewSetJSSendP {$newView} [DEVise viewGetJSSendP $view]

    eval DEVise setHistogram {$newView} [DEVise getHistogram $view]
    eval DEVise setCountMapping {$newView} [DEVise getCountMapping $view]
    eval DEVise viewSetIsHighlight {$newView} [DEVise viewGetIsHighlight $view]
    eval DEVise setXAxisDateFormat {$newView} \
      {[DEVise getXAxisDateFormat $view]}
    eval DEVise setYAxisDateFormat {$newView} \
      {[DEVise getYAxisDateFormat $view]}

    eval DEVise setViewAutoFilter {$newView} [DEVise getViewAutoFilter $view]

    eval DEVise setDupElim {$newView} [DEVise getDupElim $view]

    eval DEVise setNiceAxes {$newView} [DEVise getNiceAxes $view]

    # Whether axes are shown.
    set stat [DEVise getAxisDisplay $view X]
    eval DEVise setAxisDisplay {$newView} X $stat
    set stat [DEVise getAxisDisplay $view Y]
    eval DEVise setAxisDisplay {$newView} Y $stat

    # Whether axis ticks are shown.
    set stat [DEVise getAxisTicks $view X]
    eval DEVise setAxisTicks {$newView} X $stat
    set stat [DEVise getAxisTicks $view Y]
    eval DEVise setAxisTicks {$newView} Y $stat

    # TEMP -- should links be copied?? RKW 1999-11-17.

    DEVise insertWindow $newView $window
    DEVise insertMapping $newView $newGdata
}

############################################################

# Remove view from window
proc DoViewRemove {} {
    global curView

    if {![CurrentView]} {
	return
    }

    set viewToRemove $curView

    # Unselect current view
    ProcessViewSelected ""

    # Remove view
    set ans [DEVise removeView $viewToRemove]
    if {$ans != "done"} {
	dialog .removeError "View Error" \
		"Cannot Remove View." "" 0 OK
	return
    }
}

############################################################

# Destroy view
proc DoViewDestroy {} {
    global curView dialogListVar

    if {![CurrentView]} {
	return
    }

    set viewName $curView

    set but [ dialog .destroyView "Destroy View" \
	    "Okay to destroy view $viewName?" "" 0 \
	    Cancel OK Another ]
    if {$but == 0} {
	return
    }

    set inlink 0

    foreach link [LinkSet] {
	set views [DEVise getLinkViews $link]
	if {[lsearch $views $viewName] >= 0} {
	    puts "View $viewName has link $link."
	    set inlink 1
	}
    }

    if {$inlink} {
	dialog .hasLinks "View Has Links" \
		"Remove links from view before destroying it." \
		"" -1 OK
	return
    }

    set incursor 0

    foreach cursor [CursorSet] {
	set views [DEVise getCursorViews $cursor]
	if {[lsearch $views $viewName] >= 0} {
	    puts "View $viewName has cursor $cursor."
	    set incursor 1
	}
    }

    if {$incursor} {
	dialog .hasCursors "View Has Cursors" \
		"Remove cursors from view before destroying it." \
		"" -1 OK
	return
    }

    # Unselect current view
    ProcessViewSelected ""

    # Remove view
    set ans [DEVise removeView $viewName]
    if {$ans != ""} {
	dialogList .removeError "Cannot Remove View" $ans "" 0 OK
	return
    }

    DEVise destroy $viewName

    UpdateLinkCursorInfo
}

############################################################

# Move view to another window
proc DoViewMove {} {
    global dialogListVar curView

    if {![CurrentView]} {
	return
    }

    set viewName $curView

    set winName [ DoGetWindow ]
    if {$winName == ""} {
	return
    }
    
    DEVise insertWindow $viewName $winName
    ProcessViewSelected $viewName
}

############################################################

# Bring back view to window
proc DoBringView {} {
    global dialogListVar

    set answer [ dialogList .bringView "Restore View" \
	    "Select view to move to a window" "" 0 { OK Cancel } [ViewSet] ]
    if {$answer == 1 || $dialogListVar(selected) == ""} {
	return
    }
    set viewName $dialogListVar(selected)
     
   set winName [ DoGetWindow ]
    if {$winName == ""} {
	return
    }
    
    DEVise insertWindow $viewName $winName
    ProcessViewSelected $viewName
}

############################################################

# Create a Cursor, return its name
proc DoCursorCreate {} {
    global dialogCkButVar 

    set but [dialogCkBut .createCursor "Create Cursor" \
	    "Enter parameters for creating a new cursor" "" \
	    0 {OK Cancel} name {x y} {3}]
    if { $but != 0 } {
	return ""
    }

    set flag $dialogCkButVar(selected)
    set name $dialogCkButVar(entry)
    if {$name == ""} {
	dialog .noName "No Name Specified" \
		"Error: No name for cursor specified" "" 0 OK
	return 
    }

    if { [DEVise exists $name ] } {
	dialog .linkExists "Cursor Exists" \
		"Error: Cursor name $name already exists" "" 0 OK
	return 
    }

    set result [DEVise create cursor Cursor $name $flag]
    if {$result == ""} {
	set but [ dialog .cursorError "Cursor Error" \
		"Cannot create cursor" "" 0 OK ]
	return
    }

    UpdateLinkCursorInfo

    return $name
}

############################################################

# Create a Cursor, return its name
proc DoCursorDelete {} {
    set cursorSet [CursorSet]
    if {[llength $cursorSet] == 0} {
	return
    }

    set cursor [DoGetCursor "Select cursor to delete"]
    if {$cursor == ""} {
	return
    }
    
    DEVise destroy $cursor

    UpdateLinkCursorInfo
}

proc DoGetCursor {label} {
    global curView dialogListVar

    set cursorSet [CursorSet]
    if { [llength $cursorSet] == 0 } {
	set cursor [DoCursorCreate]
    } else {
	set answer [ dialogList .getCursor "Get Cursor" $label "" 0 \
		{ OK New Cancel } $cursorSet ]
	if { $answer == 1 } {
	    set cursor [ DoCursorCreate ]
	} elseif {$answer == 2 || $dialogListVar(selected) == ""} {
	    set cursor ""
	} else {
	    set cursor $dialogListVar(selected)
	}
    }

    return $cursor
}

############################################################

proc DoSetCursorSrc {} {
    global curView

    if {![CurrentView]} {
	return
    }

    set cursor [DoGetCursor "Select cursor for view\n$curView"]
    if {$cursor == ""} {
	return
    }

    set answer [DEVise setCursorSrc $cursor $curView]

    UpdateLinkCursorInfo
}

############################################################

proc DoSetCursorDst {} {
    global curView

    if {![CurrentView]} {
	return
    }

    set cursor [DoGetCursor "Select cursor for view\n$curView"]
    if {$cursor == ""} {
	return
    }
    
    set answer [DEVise setCursorDst $cursor $curView]

    UpdateLinkCursorInfo
}

############################################################

proc DoSetCursorGrid {} {
    global curView

    if {![CurrentView]} {
	return
    }

    set cursor [DoGetCursor "Select cursor for grid"]
    if {$cursor == ""} {
	return
    }

    GetCursorGrid $cursor
}

############################################################

# Make current view holder of a a record cursor
proc DoSetRecordCursor {} {
    NotImplemented
}

############################################################

# Create a link, and link views to it.

proc CreateAndLinkLink {} {
    global linkViewStatus currentLink

    set currentLink [DoLinkCreate 0]

    if { $currentLink != "" } {
        set linkViewStatus 1
        LinkViewMsg
    }

    UpdateLinkCursorInfo
}

############################################################

# Create a link, return its name
proc DoLinkCreate { isRec } {
    global dialogCkButVar 

    while { 1 } {
	if { $isRec} {
	    # Record link (positive).
	    set flag 4
	} else {
	    # XY visual link.
	    set flag 3
	}
	set but [CreateLink $flag]
	
	if { $but == 2 } {
	    # Cancel.
	    return ""
	} elseif  { $but == 1 } {
	    # Info.
	    DoDisplayLinkInfo
	} else {
	    # OK.
	    set flag $dialogCkButVar(selected)

	    # Convert flag values from what graphics understands to what
	    # C++ code understands.
	    set negative_record 0
	    if { $flag == 4 } {
		# Record link, postive.
		set flag 128
	    } elseif { $flag == 8 } {
		# Record link, negative.
		set flag 128
		set negative_record 1
	    }

	    # Make sure the user specified a link name.
	    set name $dialogCkButVar(entry)
	    if {$name == ""} {
		set but [dialog .noName "No Link Name" \
			"No Link Name Specified" "" 0 OK]
		continue
	    }
	    
	    # Make sure the link name didn't start with "Pile:" (reserved).
	    if {[string range $name 0 4] == "Pile:"} {
		set but [dialog .noName "No Link Name" \
			"Link name cannot start with 'Pile:'" "" 0 OK]
		continue
	    }
	
	    # Make sure an object with the given name doesn't already
	    # exist.
	    if { [DEVise exists $name ] } {
		set but [dialog .linkExists "Link Exists" \
			"Link $name already exists" "" 0 OK]
		continue
	    }
	    
	    # Create the link.
	    set result [DEVise create link Visual_Link $name $flag]
	    if {$negative_record} {
	      DEVise setLinkType $name -1
	    }

	    if {$result == ""} {
		set but [ dialog .linkError "Link Error" \
			"Cannot create link $name" "" 0 OK]
		continue
	    }
	    break
	}
    }

    UpdateLinkCursorInfo

    return $name
}

############################################################
#Display all links
proc DoDisplayLinkInfo {} {
    global dialogListVar

    while { 1 } {
	set linkSet [NonPileLinkSet]
	if { [llength $linkSet ] == 0 } {
	    set but [ dialog .destroyLink "No Links" \
		    "There are no links to give info" "" 0 OK ]
	    break
	}
	set answer [ dialogList .getLink "Select Link" \
		"Select a link" "" 0 \
		{ Info Cancel } $linkSet ]
	if { $answer == 0 } {
	    if {$dialogListVar(selected) != ""} {
		    DisplayLinkInfo $dialogListVar(selected)
	    }
	}
	if { $answer == 1 } {
	    return
	}
    }    
}
############################################################

# Display the views in a link 
proc DisplayLinkInfo { link } {
    set views [DEVise getLinkViews $link]
    set flag [DEVise getLinkFlag $link]
    if { $flag == 128 } {
	# Convert flag value from C++ enum to graphics bitmap.
	set flag 4

	set type "Record"
	set master [DEVise getLinkMaster $link]
	set labels [list  [list Type 10 "$type Link" ] \
		          [list Leader 10 $master] ]
    } elseif { $flag == 1024 } {
	# Convert flag value from C++ enum to graphics bitmap.
	set flag 8

	set type "TData Attribute"
	set master [DEVise getLinkMaster $link]
	set labels [list  [list Type 10 "$type Link" ] \
		          [list Leader 10 $master] ]
    } else {
	set type "Visual"
	set labels [list [list Type 10 "$type Link" ] ]
    }
    dialogInfo .linkInfo "Link Info" "Views linked by $link" "" \
    	    0 OK $views  $labels  \
	    [list $flag 40 2 2 col x y record {tdata attr} ]
    # quick hash - actually should write general purpose widgets to do  
    # selections with multiple kinds of widgets in one window
}

############################################################

# Link current view
proc DoViewLink {} {
    global curView dialogListVar

    if {![CurrentView]} {
	return
    }

    while { 1 } {
	set linkSet [NonPileLinkSet]
	if { [llength $linkSet] == 0 } {
	    set link [DoLinkCreate 0]
            break
	} else {
	    set answer [ dialogList .getLink "Select Link" \
		    "Select a link for view\n$curView" "" 0 \
		    { OK Info New Cancel } $linkSet ]
	    if { $answer == 1 } {
		if {$dialogListVar(selected) != ""} {
		    DisplayLinkInfo $dialogListVar(selected)
		}
	    } elseif { $answer == 2 } {
		set link [ DoLinkCreate  0 ]
	    } elseif {$answer == 3 || $dialogListVar(selected) == ""} {
		set link ""
		break
	    } else {
		set link $dialogListVar(selected)
		break
	    }
	}
    }

    if {$link == ""} {
	return
    }
    set check 1
    if { [DEVise getLinkFlag $link] == 128  } {
	set check [CheckTData $link $curView 0] 
    }
    if { $check } {
	DEVise insertLink $link $curView
    }

    UpdateLinkCursorInfo
}

############################################################

# Unlink current view
proc DoViewUnlink {} {
    global curView dialogListVar

    if {![CurrentView]} {
	return
    }

    while { 1 } {
	set linkSet [NonPileLinkSet]
	set viewLinks ""
	foreach link $linkSet {
	    if { [DEVise viewInLink $link $curView] } {
		lappend viewLinks $link
	    }
	}
	if { [llength $viewLinks ] == 0 } {
	    set but [ dialog .removeLink "No Links" \
		    "View has no established link" "" 0 OK ]
	    break
	} else {
	    set answer [ dialogList .getLink "Select Link" \
		    "Select a link to unlink" "" 0 \
		    { OK Info Cancel } $viewLinks ]
	    if { $answer == 1 } {
		if {$dialogListVar(selected) != ""} {
		    DisplayLinkInfo $dialogListVar(selected)
		}
	    } elseif { $answer == 2 } {
		return
	    } elseif { $dialogListVar(selected) == "" } {
		return
	    } else {
		DEVise unlinkView $dialogListVar(selected) $curView
		break
	    }
	}
    }

    UpdateLinkCursorInfo
}

############################################################
proc DoLinkDestroy {} {
    global dialogListVar

    while { 1 } {
	set linkSet [NonPileLinkSet]
	if { [llength $linkSet ] == 0 } {
	    set but [ dialog .destroyLink "No Links" \
		    "There are no links to destroy" "" 0 OK ]
	    break
	}
	set answer [ dialogList .getLink "Select Link" \
		"Select a link to delete" "" 0 \
		{ OK Info Cancel } $linkSet ]
	if { $answer == 1 } {
	    if {$dialogListVar(selected) != ""} {
		    DisplayLinkInfo $dialogListVar(selected)
	    }
	}
	if { $answer == 2 } {
	    return
	} elseif { $dialogListVar(selected) == "" } {
	    return
	} else {
	    DeleteLink $dialogListVar(selected) 
	}
    }

    UpdateLinkCursorInfo
}

############################################################
proc DoSetLinkType {} {
    global dialogListVar

    set linkSet [RecordLinkSet]
    if { [llength $linkSet] == 0 } {
	dialog .noLinks "No Links" "There are no record links." "" 0 OK
	return
    }
    set answer [ dialogList .getLink "Select Link" \
	    "Select a link" "" 0 \
	    { + - Cancel} $linkSet ]		
    set linkname $dialogListVar(selected)
    if { $answer == 0} {
	# Positive.
	DEVise setLinkType $linkname 1
    } elseif { $answer == 1} {
	# Negative.
	DEVise setLinkType $linkname -1
    } elseif { $answer == 2} {
	return
    }
    set linkMaster [DEVise getLinkMaster $linkname]
    DEVise refreshView $linkMaster
    UpdateLinkCursorInfo
}

############################################################
proc DoSetLinkAttr {} {
    global dialogListVar

    set linkSet [TAttrLinkSet]
    if { [llength $linkSet] == 0 } {
	dialog .noLinks "No Links" "There are no TData attr links." "" 0 OK
	return
    }
    set answer [ dialogList .getLink "Select Link" \
	    "Select a link" "" 0 \
	    {OK Cancel} $linkSet ]		
    if { $answer == 0} {
    	set linkname $dialogListVar(selected)
	SetLinkAttr $linkname
    }

    UpdateLinkCursorInfo
}

############################################################
proc DoLinkDestroy {} {
    global dialogListVar
    set linkSet [NonPileLinkSet]
    set answer [ dialogList .getLink "Select Link" \
	    "Select a link to delete" "" 0 \
	    { OK Cancel } $linkSet ]
    if { $answer == 1 } {
	return
    } elseif { $dialogListVar(selected) == "" } {
	return
    } else {
	DeleteLink $dialogListVar(selected) 
    }
}

############################################################

proc DeleteLink { link } {
    if { $link == "" } {
	return
    }
    set views [DEVise getLinkViews $link]
    if { [llength $views] > 0 } {
	set but [ dialog .deleteLink "Link still active" \
		"The link still has views linked by it \
		- Do you want to unlink views and destroy link ?"\
		"" 0 OK Cancel]
	if { $but == 0 } {
	    foreach view $views {
		DEVise unlinkView $link $view 
	    }
	} else {
	    return
	}
    }
    DEVise destroy $link
    UpdateLinkCursorInfo
}


# Set current view as master
proc DoSetLinkMaster {} {
    global curView dialogListVar

    if {![CurrentView]} {
	return
    }

    set linkSet [RecordLinkSet]
    if { [llength $linkSet] == 0 } {
	set link [DoLinkCreate 1]
	if {$link == ""} {
#	    puts "nnnnnnnn"
	    return
	}
    }
    
    while { 1 } {
	set linkSet [RecordLinkSet]
	set answer [ dialogList .getLink "Select Link" \
		"Select a link for view\n$curView" "" 0 \
		{ OK Info New Cancel } $linkSet ]
	if { $answer == 1 } {
	    if {$dialogListVar(selected) != ""} {
		DisplayLinkInfo $dialogListVar(selected)
	    }
	} elseif { $answer == 2 } {
	    DoLinkCreate 1
	} elseif {$answer == 3 || $dialogListVar(selected) == ""} {
	    return
	} else {
	    set link $dialogListVar(selected)
	    break
	}
    }
    if { [CheckTData $link $curView 1] } {
	DEVise setLinkMaster $link $curView 
	if { [expr 1024 & [DEVise getLinkFlag $link]] } {
	    SetLinkAttr $link
	}
    }
    UpdateLinkCursorInfo
}

############################################################
proc CheckTData { link view isMaster } {
    set check [DEVise checkTDataForRecLink $link $view 1]
    set but 0
    if {!$check } {
	set but [ dialog .tdataCheck "Linking different TData sources" \
		"The tdata for the leader and follower views does not match \n Do you wish to go ahead ? "\
		"" 0 OK Cancel ]
    }
    if { $but == 0 } {
	return 1
    } 
    return 0
}

############################################################

# Set link to be without a master
proc DoResetLinkMaster {} {
    global dialogListVar

    set linkSet [RecordLinkSet]
    if { [llength $linkSet] == 0 } {
	dialog .noLinks "No Links" "There are no record links." "" 0 OK
	return
    }

    while {1} {
	set answer [ dialogList .getLink "Select Link" \
		"Select a link to reset" "" 0 \
		{ OK Info Cancel} $linkSet ]
	if { $answer == 1 } {
	    if {$dialogListVar(selected) != ""} {
		DisplayLinkInfo $dialogListVar(selected)
	    }
	} elseif {$answer == 2 || $dialogListVar(selected) == ""} {
	    return
	} else {
	    set link $dialogListVar(selected)
	    break
	}
    }

    DEVise resetLinkMaster $link
    UpdateLinkCursorInfo
}


############################################################

# Modify parameters of a link
proc DoModifyLink {} {
    global dialogCkButVar dialogListVar
    set linkSet [NonPileLinkSet]
    if { [llength $linkSet ] == 0 } {
	set but [dialog .linkModError "Link Modify Error" \
		"No link has been created yet" "" 0 OK ]
	return
    }
    
    # Select a link
    set answer [ dialogList .getLink "GetLink" \
	    "Select a link to modify param" "" 0 \
	    { Cancel OK } $linkSet ]
    if { $answer == 0 } {
	return
    } elseif { $dialogListVar(selected) == "" } {
	return
    } else {
	set link $dialogListVar(selected)
    }

    # enter parameters for the link
    set flag [DEVise getLinkFlag $link]
    set but [dialogCkBut .modLink "Modify Link" \
	    "Enter parameters for link $link" "" \
	    0 {Cancel OK} "" {x y color size pattern orientation shape} $flag]
    if { $but != 1 } {
	return ""
    }
    set flag $dialogCkButVar(selected)
    DEVise setLinkFlag $link $flag
    UpdateLinkCursorInfo
}

############################################################

proc SetLinkAttr {linkname} {
    global attrName
    global SetLinkAttr_cancel

    # For now, the master and slave attributes are always the same.
    set attrName [DEVise getLinkMasterAttr $linkname]

    #
    # Find the schema name of the master view of this link, so we can
    # create a menu of the available attributes.
    #
    set masterView [DEVise getLinkMaster $linkname]
    if { $masterView == ""} {
        dialog .setAttrError "Link Error" \
	    "Link must have leader view to set attribute." "" 0 OK
	return
    }

    set map [lindex [DEVise getViewMappings $masterView] 0]

    set params [DEVise getInstParam $map]
    if {$params == ""} {
        dialog .editError "No Mapping Parameters" \
                "Mapping has no parameters." "" 0 OK
        return
    }

    set tdata [lindex [lindex $params 0] 1]

    set schemaname [GetSchema $tdata]
    if { $schemaname == "" } {
        return
    }

    #
    # Create the window to allow the user to select an attribute.
    #
    toplevel .setLinkAttr
    wm geometry .setLinkAttr +150+150
    wm title .setLinkAttr "Set Link Attribute"

    frame .setLinkAttr.row1
    frame .setLinkAttr.row2

    # Create the various widgets.
    label .setLinkAttr.label -text "Attribute:"
    menubutton .setLinkAttr.menubut -textvariable attrName \
      -relief raised -menu .setLinkAttr.menubut.menu

    set topgrp __default
    setupAttrRadioMenu .setLinkAttr.menubut.menu attrName "" $schemaname \
      $topgrp $topgrp

    button .setLinkAttr.ok -text "OK" -width 10 \
      -command {global SetLinkAttr_cancel; set SetLinkAttr_cancel 0;
      destroy .setLinkAttr}
    button .setLinkAttr.cancel -text "Cancel" -width 10 \
      -command {global SetLinkAttr_cancel; set SetLinkAttr_cancel 1;
      destroy .setLinkAttr}

    # Pack the widgets into frames.
    pack .setLinkAttr.row1 -side top -pady 4m
    pack .setLinkAttr.row2 -side top -pady 4m

    pack .setLinkAttr.label .setLinkAttr.menubut -in .setLinkAttr.row1 \
      -side left -padx 3m
    pack .setLinkAttr.ok .setLinkAttr.cancel -in .setLinkAttr.row2 \
      -side left -padx 3m

    # Wait for the user to make a selection from this window.
    tkwait visibility .setLinkAttr
    grab set .setLinkAttr
    tkwait window .setLinkAttr

    if { $SetLinkAttr_cancel == 0 && $attrName != "" } {
	# For now, we force the leader and follower attributes to be the same.
	DEVise setLinkMasterAttr $linkname $attrName
	DEVise setLinkSlaveAttr $linkname $attrName
	UpdateLinkCursorInfo
    }
}

############################################################

proc CheckView {} {
    global curView

    if {![CurrentView]} {
	return
    }

    return 1
}

############################################################

# Actually create a new axis Label
proc DoActualCreateAxisLabel { type } {
    global dialogParamVar 
    set paramNames [ DEVise getparam axisLabel $type]
    
    set button [ dialogParam .axisParam "AxisParam" \
	    "Enter parameters to create axis $type"\
	    "" 1 { Cancel OK } $paramNames ]
    if { $button == 0} {
	return
    }

    set cmd "DEVise create axisLabel $type $dialogParamVar(params)"
    set result [ eval $cmd ]
    if { $result == "" } {
	set but [ dialog .axisError "Axis Error" \
		"Cannot create axis" "" 0 OK ]
	return
    }

    return $result
}

############################################################

# Create a new axis label
proc DoCreateAxisLabel {} {
    global dialogListVar
    set axisTypes [ DEVise get axisLabel]

    if {![llength $axisTypes]} {
	dialog .noAxisTypes "No Axis Types Defined" \
		"No axis types have been defined." "" 0 OK
	return ""
    }

    set answer [ dialogList .createAxis "Create Axis"  \
	    "Select type of axis label to create" "" "" \
	    { Cancel Ok } $axisTypes ]
    if { $answer == 0 || $dialogListVar(selected) == "" } {
	return ""
    }

    return [ DoActualCreateAxisLabel $dialogListVar(selected) ]
}

############################################################

# Get an axis label for current view
proc DoGetAxisLabel {} {
    global curView dialogListVar
    set axisSet [AxisSet]
    if { [llength $axisSet] == 0 } {
	set axisLabel [DoCreateAxisLabel]
    } else {
	set answer [ dialogList .getAxisLabel "Select Axis Label" \
		"Select an axis label for current view."\
		"" 0 { New Cancel OK } $axisSet ]
	if { $answer == 0 } {
	    set axisLabel [ DoCreateAxisLabel ]
	} elseif { $answer == 1 || $dialogListVar(selected) == "" } {
	    set axisLabel ""
	} else {
	    set axisLabel $dialogListVar(selected)
	}
    }

    return $axisLabel
}

############################################################

proc DoViewAxis { axis } {
    global curView
    if { ! [CheckView] } {
	return
    }
    set axisLabel [DoGetAxisLabel]
    if {$axisLabel == ""} {
	return
    }
    
    DEVise setAxis $curView $axisLabel $axis
}

############################################################

proc DoActualCreateAction { type } {
    global dialogParamVar 
    set paramNames [ DEVise getparam action $type]
    
    set button [ dialogParam .actionParam "ActionParam" \
	    "Enter parameters to create action $type"\
	    "" 1 { Cancel OK } $paramNames ]
    if { $button == 0} {
	return
    }
    
    set cmd "DEVise create action $type $dialogParamVar(params)"
    set result [eval $cmd ]
    if { $result == "" } {
	set but [ dialog .actionError "Action Error" \
		"Cannot create action" "" 0 OK ]
	return
    }
    return $result
}

############################################################

# Create an action
proc DoCreateAction {} {
    global dialogListVar
    set actionTypes [ DEVise get action]
    set answer [ dialogList .createAction "Create Action"  \
	    "Select type of action to create" "" "" { Cancel Ok } $actionTypes]
    if { $answer == 0 || $dialogListVar(selected) == "" } {
	return
    } else {
	return [ DoActualCreateAction $dialogListVar(selected) ]
    }
}

############################################################

# Get an action
proc DoGetAction {} {
    global curView dialogListVar
    set actionSet [ActionSet ]
    if { [llength $actionSet] == 0 } {
	set action [ DoCreateAction ]
    } else {
	set answer [dialogList .getAction "Select Action" \
		"Select an action for view\n$curView"\
		"" 0 { New Cancel OK } $actionSet]
	if { $answer == 0 } {
	    set action [ DoCreateAction]
	} elseif { $answer == 1 || $dialogListVar(selected) == "" } {
	    set action ""
	} else {
	    set action $dialogListVar(selected)
	}
    }
    return $action
}

############################################################

proc DoViewAction {} {
    global curView
    if { ! [CheckView] } {
	return
    }
    set action [DoGetAction]
    if {$action == ""} {
	return
    }

    DEVise setAction $curView $action
}

############################################################

proc DoSetTitle { {perWindow 0} } {
    global curView newTitle

    if {[WindowVisible .setTitle]} {
	return
    }

    if {![CurrentView]} {
	return
    }

    toplevel .setTitle
    wm title .setTitle "Set View Title"
    wm geometry .setTitle +50+50

    frame .setTitle.top
    frame .setTitle.bot
    pack .setTitle.top -side top -pady 3m -fill both -expand 1
    pack .setTitle.bot -side top -pady 5m -fill x

    frame .setTitle.bot.but
    pack .setTitle.bot.but -side top

    set viewLabelParams [DEVise getLabel $curView]
    set occupyTop [lindex $viewLabelParams 0]
    set newTitle [lindex $viewLabelParams 2]
    if {!$occupyTop && $newTitle == ""} {
	set newTitle $curView
    }

    label .setTitle.top.l1 -text "Title:"
    entry .setTitle.top.e1 -relief sunken -width 30 -textvariable newTitle
    pack .setTitle.top.l1 .setTitle.top.e1 -side left -padx 3m \
	    -fill x -expand 1

    button .setTitle.bot.but.ok -text Show -width 10 -command {
	DEVise setLabel $curView 1 16 $newTitle
	destroy .setTitle
    }
    button .setTitle.bot.but.clear -text Clear -width 10 -command {
	set newTitle ""
    }
    button .setTitle.bot.but.delete -text Hide -width 10 -command {
	DEVise setLabel $curView 0 12 $newTitle
	destroy .setTitle
    }
    button .setTitle.bot.but.cancel -text Cancel -width 10 -command {
	destroy .setTitle
    }
    pack .setTitle.bot.but.ok .setTitle.bot.but.clear \
	    .setTitle.bot.but.delete .setTitle.bot.but.cancel \
	    -side left -padx 3m
}

############################################################

proc DoSetViewDimensions { dim } {
    global curView

    if {![CurrentView]} {
	return
    }

    DEVise setViewDimensions $curView $dim
}

############################################################

proc DoToggleAxis { axis } {
    global curView

    if {![CurrentView]} {
	return
    }

    set stat [DEVise getAxisDisplay $curView $axis]
    set stat [expr !$stat]
    DEVise setAxisDisplay $curView $axis $stat
}

############################################################

proc DoToggleAxisTicks { axis } {
    global curView

    if {![CurrentView]} {
	return
    }

    set enabled [DEVise getAxisTicks $curView $axis]
    set enabled [expr !$enabled]
    DEVise setAxisTicks $curView $axis $enabled
}

############################################################

proc DoToggleAxisAllViews { axis } {
    global curView

    if {![CurrentView]} {
	return
    }

    set stat [DEVise getAxisDisplay $curView $axis]
    set stat [expr !$stat]
    
    set viewClasses [ DEVise get view ]
    foreach viewClass $viewClasses {
	set views [ DEVise get view $viewClass ]
	foreach v $views {
	    DEVise setAxisDisplay $v $axis $stat
	}
    }
}

############################################################

proc DoToggleStatistics {} {
    global curView statmean statmax statmin statline statcurr statcilevel

    # The status is formed by as a binary string xxx where 1 means that the 
    # corr. stat is to be displayed. Further, since "count" which is the 
    # fourth stat cannot currently be displayed meaningfully on the graph
    # always append a 0.

    set statcount 0
    set stat $statmean$statmax$statmin$statline$statcount$statcilevel

    if {$statcurr == 1} {
	if {![CurrentView]} {
	    return
	}
	DEVise setViewStatistics $curView $stat
	return
    }

    set viewClasses [ DEVise get view ]
    foreach viewClass $viewClasses {
	set views [ DEVise get view $viewClass ]
	foreach v $views {
	    DEVise setViewStatistics $v $stat
	}
    }
}

############################################################

proc DoStat {} {
    global statmean statmax statmin statline statcurr statcilevel

    if {[WindowVisible .statWin]} {
        return
    }

    toplevel .statWin
    wm title .statWin "Select Statistics"
    wm geometry .statWin +100+100

    frame .statWin.m -relief groove -bd 2
    frame .statWin.ci -relief groove -bd 2
    frame .statWin.ac
    pack .statWin.m .statWin.ci .statWin.ac -expand 1 -side left -fill x \
	    -padx 3m

    frame .statWin.m.mean
    frame .statWin.m.max
    frame .statWin.m.min
    frame .statWin.m.line
    pack .statWin.m.mean .statWin.m.max .statWin.m.min \
	    .statWin.m.line -expand 1 -side top -fill y
    
    frame .statWin.ci.no
    frame .statWin.ci.ci1
    frame .statWin.ci.ci2
    frame .statWin.ci.ci3
    pack  .statWin.ci.no .statWin.ci.ci1 .statWin.ci.ci2 .statWin.ci.ci3 \
	    -expand 1 -side top -fill y

    frame .statWin.ac.applyAll
    frame .statWin.ac.applyCur
    frame .statWin.ac.cancel
    pack .statWin.ac.applyAll .statWin.ac.applyCur .statWin.ac.cancel \
	    -expand 1 -side top -fill y

    checkbutton .statWin.m.mean.but -text "Mean" -width 10 \
	    -anchor w -variable statmean
    checkbutton .statWin.m.max.but -text "Max" -width 10 \
	    -anchor w -variable statmax
    checkbutton .statWin.m.min.but -text "Min" -width 10 \
	    -anchor w -variable statmin
    checkbutton .statWin.m.line.but -text "Line" -width 10 \
	    -anchor w -variable statline
    pack .statWin.m.mean.but .statWin.m.max.but .statWin.m.min.but .statWin.m.line.but -side top 
    
    radiobutton .statWin.ci.no.but -text "No CIs" -width 10 \
	    -variable statcilevel -value "000"
    radiobutton .statWin.ci.ci1.but -text "85% CI" -width 10 \
	    -variable statcilevel -value "100"
    radiobutton .statWin.ci.ci2.but -text "90% CI" -width 10 \
	    -variable statcilevel -value "010"
    radiobutton .statWin.ci.ci3.but -text "95% CI" \
	    -variable statcilevel -value "001"
    pack .statWin.ci.no.but .statWin.ci.ci1.but .statWin.ci.ci2.but \
	    .statWin.ci.ci3.but -side top -padx 2m -pady 1m

    button .statWin.ac.applyAll.but -text "Apply Current"\
	    -width 15 -command { set statcurr 1; DoToggleStatistics }
    button .statWin.ac.applyCur.but -text "Apply All" \
	    -width 15 -command { set statcurr 0; DoToggleStatistics }
    button .statWin.ac.cancel.but -text Close -width 15 \
	    -command { destroy .statWin }
    pack .statWin.ac.applyAll.but .statWin.ac.applyCur.but \
	    .statWin.ac.cancel.but -side top
}

############################################################
proc flip { digit } {
    if {$digit == 1} { return 0 
    } else { return 1 }
}

############################################################

proc DoBasicStat {} {
    global curView statcount statmean statmax statmin statline statcilevel
    set statmean 0
    set statmax 0
    set statmin 0
    set statline 0
    set statcount 0
    set statcilevel "000"

    if {[WindowVisible .basicStat]} {
	return
    }

    set expand 1
    set fill both

    toplevel	.basicStat
    wm title	.basicStat "Basic Statistics"
    wm geometry .basicStat +100+100

    frame .basicStat.title
    frame .basicStat.mmmc
    frame .basicStat.ci
    frame .basicStat.close

    pack .basicStat.title -side top -pady 1m -expand $expand -fill $fill
    pack .basicStat.mmmc -side top -pady 1m -expand $expand -fill $fill
    pack .basicStat.ci -side top -pady 1m -expand $expand -fill $fill
    pack .basicStat.close -side top -pady 1m -expand $expand -fill $fill

    label .basicStat.title.text -text "No View Selected"
    pack .basicStat.title.text -side top -pady 1m
    if {$curView != ""} {
        .basicStat.title.text configure -text "View: $curView"
    }

    #set up max mean min count buttons and entries

    frame .basicStat.mmmc.upper
    frame .basicStat.mmmc.lower
    pack .basicStat.mmmc.upper .basicStat.mmmc.lower -in .basicStat.mmmc\
                -side top -pady 1m -expand $expand -fill $fill

    button .basicStat.maxButton -text Max -width 10 -command { \
			set statmax [flip $statmax] 
			DoToggleStatistics }
    entry .basicStat.maxEntry -text "" -relief sunken -width 10 -font 8x13
    button .basicStat.minButton -text Min -width 10 -command { \
                        set statmin [flip $statmin] 
                        DoToggleStatistics }
    entry .basicStat.minEntry -text "" -relief sunken -width 10 -font 8x13
    pack .basicStat.maxButton .basicStat.maxEntry .basicStat.minButton \
			.basicStat.minEntry -in .basicStat.mmmc.upper  \
			-side left -expand $expand -fill $fill

    button .basicStat.meanButton -text Mean -width 10 -command { \
                        set statmean [flip $statmean] 
                        DoToggleStatistics }
    entry .basicStat.meanEntry -text "" -relief sunken -width 10 -font 8x13
    button .basicStat.countButton -text Count -width 10 -command { \
                        set statcount [flip $statcount] 
                        DoToggleStatistics }
    entry .basicStat.countEntry -text "" -relief sunken -width 10 -font 8x13
    pack .basicStat.meanButton .basicStat.meanEntry .basicStat.countButton \
                        .basicStat.countEntry -in .basicStat.mmmc.lower  \
                        -side left -expand $expand -fill $fill

    frame .basicStat.ci.left
    frame .basicStat.ci.right

    pack .basicStat.ci.left .basicStat.ci.right -in .basicStat.ci \
		-side left -pady 1m -expand $expand -fill $fill

    button .basicStat.ci.left.noci -text "No CIs" -width 10 -command { \
			if { $statcilevel != "000"} {set statcilevel "000"} 
			DoToggleStatistics }
    button .basicStat.ci.left.line -text "Line" -width 10 -command { \
			set statline [flip $statline] 
                        DoToggleStatistics }
    pack .basicStat.ci.left.noci .basicStat.ci.left.line -in .basicStat.ci.left \
		-side top -pady 1m -expand $expand -fill $fill

    button .basicStat.ci.right.ci1 -text "85% CI" -width 10 -command {\
			set statcilevel "100" 
			DoToggleStatistics }
    button .basicStat.ci.right.ci2 -text "90% CI" -width 10 -command {\
			set statcilevel "010" 
			DoToggleStatistics }
    button .basicStat.ci.right.ci3 -text "95% CI" -width 10 -command {\
			set statcilevel "001" 
			DoToggleStatistics }
    pack .basicStat.ci.right.ci1 .basicStat.ci.right.ci2 .basicStat.ci.right.ci3 \
	-in .basicStat.ci.right -side top -pady 1m -expand $expand -fill $fill

    button .basicStat.closeButton -text Close -width 10 \
			-command { destroy .basicStat }
    pack .basicStat.closeButton -in .basicStat.close -pady 1m \
			-expand $expand 
}

############################################################
proc Update2DBasicStatWindow {} {
    global curView

    if {![WindowExists .basicStat]} {
        return
    }
    foreach i {maxEntry meanEntry minEntry countEntry} {
        .basicStat.$i delete 0 end
    }
    if {$curView == ""} {
        .basicStat.title.text configure -text "No View Selected"
        return
    }

    .basicStat.title.text configure -text "View: $curView"

    set stat [DEVise getAllStats $curView]
    .basicStat.maxEntry insert 0 [lindex $stat 0]
    .basicStat.meanEntry insert 0 [lindex $stat 1]
    .basicStat.minEntry insert 0 [lindex $stat 2]
    .basicStat.countEntry insert 0 [lindex $stat 3]
}

############################################################
proc DoGroupByStat {} {
    global curView derivedSourceList schemadir
    global tdata viewsel linksel windowsel bgcolor
    global titlesel xaxissel yaxissel byAttr ylist newgdata
    global cancel aggregate checked 

    if {[WindowVisible .groupBy]} {
        return
    }

    set sourceName ""
    set sourceName [DEVise getSourceName $curView]
    if {$sourceName == "" } { 
       puts "Error in getting sourceName" 
       dialog .winError "sourceName error" \
       		"Unable to get source name. Please select another one." \
		"" 0 OK
		return 
    }
    set pos [string first : $sourceName]
    if { [string range $sourceName 0 [expr $pos -1]] == "GstatXDTE" } {
       dialog .winError "sourceName error" \
       		"Aggregate on aggregates is not support.  Please select another one." \
		"" 0 OK
		return 
    }

    toplevel    .groupBy
    wm title    .groupBy "Group By Select"
    wm geometry .groupBy +150+150

    frame .groupBy.top -relief groove -bd 1
    frame .groupBy.top.left
    frame .groupBy.top.right
    frame .groupBy.mid -relief groove -bd 1
    frame .groupBy.bot -relief groove -bd 1
    frame .groupBy.close_ok

    pack .groupBy.top -side top -pady 3m -expand 1 -fill both
    pack .groupBy.top.left .groupBy.top.right -side left -pady 3m -expand 1 -fill both
    pack .groupBy.mid -side top -pady 3m -expand 1 -fill both
    pack .groupBy.bot -side top -pady 3m -expand 1 -fill both
    pack .groupBy.close_ok -side top -pady 3m -expand 1 -fill both

    label .groupBy.windowselLabel -text "Window:"
    menubutton .groupBy.windowsel -relief raised \
            -textvariable windowsel -menu .groupBy.windowsel.windowMenu \
            -width 20
    pack .groupBy.windowselLabel .groupBy.windowsel -in .groupBy.top.left \
		-side left -pady 1m -expand 1 -fill both
    set windowsel ""
    if {$curView != ""} {
        set windowsel [DEVise getViewWin $curView]
    } elseif {[DEVise getWinCount] <= 1} {
    	set windowsel New
    } else {
    	set windowsel "None selected yet"
    }
   
    menu .groupBy.windowsel.windowMenu -tearoff 0
    foreach w [WinSet] {
        .groupBy.windowsel.windowMenu add command -label $w \
                -command "set windowsel {$w}"
    }

    .groupBy.windowsel.windowMenu add separator
    .groupBy.windowsel.windowMenu add command -label "New" -command {
    	set windowsel New
    }
#        set newwin [DoCreateWindow "Select window type"]
#        if {$newwin != ""} {
#            set windowsel $newwin
#            .groupBy.windowsel.windowMenu add command -label $windowsel \
#                    -command "set windowsel {$windowsel}"
#        }
   

    set bylist { X Y Color}
    set onlist { X Y Z}
    set category { Max Min Avg Count Sum }

    label .groupBy.byLabel -text "Group By" 
    menubutton .groupBy.xsel -relief raised \
            -textvariable byAttr -menu .groupBy.xsel.menu -width 20

    set t ""
    set result [DEVise checkGstat $curView]

    set xDate "0"
    set xDate [DEVise isXDateType $curView]
    set yDate "0"
    set yDate [DEVise isYDateType $curView]

    menu .groupBy.xsel.menu -tearoff 0
    .groupBy.xsel.menu add cascade -label "GData" -menu .groupBy.xsel.menu.gdata
    .groupBy.xsel.menu add cascade -label "TData" -menu .groupBy.xsel.menu.tdata
    menu .groupBy.xsel.menu.gdata -tearoff 0

        set sourcedef [OpenDataSource $sourceName]
	puts "sourcedef = $sourcedef"
	set tdataType [lindex $sourcedef 2]
	if {$tdataType == "UNIXFILE"} {
		set schemafile [lindex $sourcedef 3]
	} elseif {[isDTEType $tdataType]} {
		set schemafile [lindex $sourcedef 1]
        } else { 
	# GDATASTAT or other stat
		set schemafile [lindex $sourcedef 2]
	}

	puts "schemafile=$schemafile"
        set schemaname [file tail $schemafile]
	if { [isDTEType $tdataType] } {
        	set topgrp [SelectTopGroup $sourceName]
    		set defaultX [setupAttrRadioMenu \
		 .groupBy.xsel.menu.tdata byAttr "tdata." $sourceName $topgrp $topgrp]
        } else {
#		set res [DEVise dteImportFileType $sourceName]
#		puts "res after dteImportFileType = $res"
#        	set topgrp [SelectTopGroup $sourceName]
        	set topgrp [SelectTopGroup $schemaname]
    		set defaultX [setupAttrRadioMenu \
		 .groupBy.xsel.menu.tdata byAttr "tdata." $schemaname $topgrp $topgrp]
        }
	set byAttr $defaultX
        puts "sourceName=$sourceName, schemaname=$schemaname, topgrp=$topgrp"

    	foreach att $bylist {
        	.groupBy.xsel.menu.gdata add radiobutton -label gdata.$att \
			-variable byAttr
    	}
    pack .groupBy.byLabel .groupBy.xsel \
		-in .groupBy.top.right  -side left -padx 1m

    label .groupBy.mid.onLabel -text "Aggregate On: "
    pack .groupBy.mid.onLabel -side left -fill y

     menubutton .groupBy.aggregate -relief raised -textvariable aggregate \
     		-menu .groupBy.aggregate.menu -width 20
     menu .groupBy.aggregate.menu -tearoff 0
     .groupBy.aggregate.menu add cascade -label "GData" \
     		-menu .groupBy.aggregate.menu.gdata
     .groupBy.aggregate.menu add cascade -label "TData" \
     		-menu .groupBy.aggregate.menu.tdata
     menu .groupBy.aggregate.menu.gdata -tearoff 0

     if { [isDTEType $tdataType] } {
#     	set defaultAgg [setupAttrRadioMenu \
#	 .groupBy.aggregate.menu.tdata aggregate "tdata." $schemaname $topgrp $topgrp]
     	set defaultAgg [setupAttrRadioMenu \
	 .groupBy.aggregate.menu.tdata aggregate "tdata." $sourceName $topgrp $topgrp]
     } else {
#     	set defaultAgg [setupAttrRadioMenu \
#	 .groupBy.aggregate.menu.tdata aggregate "tdata." $sourceName $topgrp $topgrp]
     	set defaultAgg [setupAttrRadioMenu \
	 .groupBy.aggregate.menu.tdata aggregate "tdata." $schemaname $topgrp $topgrp]
     }
     set aggregate $defaultAgg
     .groupBy.aggregate.menu.gdata add radiobutton -label gdata.X -variable aggregate
     .groupBy.aggregate.menu.gdata add radiobutton -label gdata.Y -variable aggregate
     pack .groupBy.aggregate -in .groupBy.mid -expand 0 -fill none

    label .groupBy.catLabel -text "Aggregate Category"
    pack .groupBy.catLabel -in .groupBy.bot -side top -fill x -expand 1

    foreach categ $category {
	set checked($categ) 0
	checkbutton .groupBy.cat$categ -text $categ -variable checked($categ) \
		-width 10 -indicatoron 0 -selectcolor #e6ceb1
        pack .groupBy.cat$categ -in .groupBy.bot -side left -fill x -expand 1
    }

    set cancel 0
    button .groupBy.ok -text OK -width 25 -command {
	set attr [string trimleft $byAttr "t."]
	if {$aggregate == $attr} {
	    dialog .winError "Attribute Selection Error" \
		"Group By and Aggregate on attributes must be different. Please select again." \
		"" 0 OK
	    return
	}
	if {$aggregate != "Y" && $attr == "Color"} {
	    dialog .winError "Attribute Selection Error" \
		"Group by Color only supports aggregate on Y at this time. Please select again." \
		"" 0 OK
	    return
	}

	if {$windowsel == "New"} {
	   set newwin [DoCreateWindow "Select window type"]
	   if {$newwin != ""} {
	      set windowsel $newwin
	   }
	}
        if {![DEVise exists $windowsel]} {
            dialog .winError "No Window Specified" \
                    "Please select a window for visualization." \
            "" 0 OK
           return
        }
	set ylist ""
        foreach yname [array names checked] {
            if {$checked($yname) > 0} {
	        lappend ylist $yname
            }
        }
        foreach yname [array names checked] {
            if {$checked($yname) > 0} {
                set cancel 0
                return
            }
        }
        dialog .autoError "Note" \
                "Please select one or more\n\
                Y attributes for automatic\n\
                mapping." "" 0 OK
        return
    }
	
    button .groupBy.cancel -text Cancel -width 25 -command {set cancel 1}
    pack .groupBy.ok .groupBy.cancel -in .groupBy.close_ok -side left -expand 1

    tkwait variable cancel
    destroy .groupBy

    if {$cancel} {
        return
    }

    scanDerivedSources
    set byAttrSource [string range $byAttr 0 4]
    set byAttr [string range $byAttr 6 end]
    set aggAttrSource [string range $aggregate 0 4]
    set aggregate [string range $aggregate 6 end]

    if { $byAttrSource != $aggAttrSource } {
    	dialog .winError "Attribute Selection Error" \
	    "Group By and Aggregate on attributes must be both from tdata or gdata. Please select again." \
	    "" 0 OK
	    return
    }
    set attrSource $byAttrSource
    set tdata ""
    set queryByDTE 0
    set queryInMem 0
    if { $attrSource == "gdata" } {
       set pos [string first "Color" $byAttr]
#       puts "WAHWAHWAHWAHWAH : pos = $pos"
       if {$pos > 0 || $byAttr == "Color"} {
       #GroupBy color is always in memory
    	  set tdata "{Stat: $curView}"
  	  set byAttr [string range $byAttr $pos end]
       } else {
         if {$result != "0" } {
         #GroupBy x or y is in memory
  	   set queryInMem 1
	   set queryByDTE 0
	 } else {
	 #GroupBy x or y is NOT in memory
	   set byTAttr [DEVise mapG2TAttr $curView $byAttr]
	   set onTAttr [DEVise mapG2TAttr $curView $aggregate]
	   if {$byTAttr == "0" || $onTAttr == "0" } {
	   	dialog .winError "Attribute Selection Error" \
		     "Group By cannot be calculated in memory and cannot be converted to TData either. Please define an SQL view to visualize it" \
		     "" 0 OK
		     return
	   }
	   set byAttr $byTAttr
	   set aggregate $onTAttr
	   set queryByDTE 1
	 }
       }
    } elseif {$attrSource == "tdata" } {
        set byGAttr [DEVise mapT2GAttr $curView $byAttr]
	set onGAttr [DEVise mapT2GAttr $curView $aggregate]
	if { $byGAttr == "0" || $byGAttr == "Z" || $onGAttr == "0" || $onGAttr == "Z" || $result == "0" } {
    		set queryByDTE 1
	} else {
		set byAttr $byGAttr
		set aggregate $onGAttr
		set pos [string first "Color" $byAttr]
#		puts "WAHWAHWAHWAHWAH : pos = $pos byAttr=$byAttr aggregate=$aggregate"
		if { $byAttr == "Color"} {
		   set tdata "{Stat: $curView}"
		   set byAttr [string range $byAttr $pos end]
		} else {
		   set queryByDTE 0
		   set queryInMem 1
		}
	}
    } else { 
        puts "Invalid attribute category" 
    	return 
    }

    if {$queryByDTE == 1 } {
    	set baseName "{GstatXDTE: $curView: 1}"
	set subName "GstatXDTE: $curView"
	set uniqTData [UniqueName $baseName]
	puts "uniqTData = $uniqTData"
#    	set tdata "{GstatXDTE: $curView}"
	set tdata $uniqTData
#	puts "Before tdataMMMMM = $tdata"
#	set uniqName [UniqueName $tdata]
#	set tdata $uniqName
	puts "tdataMMMMM = $tdata"
#	set this_sourceName "GstatXDTE: $curView"
	set this_source [string trimleft $tdata "\{"]
	set this_source [string trimright $this_source "\}"]
#	set uniq_this_sourceName [UniqueName $this_sourceName]
	puts "this_source = $this_source"
	set command "$sourceName $byAttr $aggregate $ylist"
#	set temp [lindex $derivedSourceList($this_sourceName) 7]
	set temp [lindex $derivedSourceList($subName) 7]
#	set elem6 [lindex $derivedSourceList($this_sourceName) 6]
	set elem6 [lindex $derivedSourceList($subName) 6]
	puts "Before replace $temp command=$command elem6=$elem6"
	set derivedSourceList($this_source) [lreplace $derivedSourceList($subName) 6 7 $elem6 $command]
	set elem6 [lindex $derivedSourceList($this_source) 6]
	set temp [lindex $derivedSourceList($this_source) 7]
	puts "After replace $temp $elem6"
    }
    if {$queryInMem == 1 } {
          if {$aggregate == "Y"} {
             if { $xDate == "1" } {
                  set tdata "{GstatXDate: $curView}"
                  set this_sourceName "GstatXDate: $curView"
             } else {
                  set tdata "{GstatX: $curView}"
                  set this_sourceName "GstatX: $curView"
             }
          } elseif {$aggregate == "X" } {
             if { $yDate == "1" && $byAttr == "Y" } {
                  set tdata "{GstatYDate: $curView}"
                  set this_sourceName "GstatYDate: $curView"
             } else {
                  set tdata "{GstatY: $curView}"
                  set this_sourceName "GstatY: $curView"
             }
           } 
    }

#    if $attrSource == "gdata" && $byAttr == "X" && $xDate == "1" 
    if {$queryInMem == 1 && $byAttr == "X" && $xDate == "1" } {
    	set byAttr DATE
    }

#    if $attrSource == "gdata" && $byAttr == "Y" && $yDate == "1" 
    if {$queryInMem == 1 && $byAttr == "Y" && $yDate == "1" } {
        set byAttr DATE
    }

    puts "byAttr=$byAttr aggregate=$aggregate ylist=$ylist"

    set viewsel 2  
    # Bar Chart
    set linksel 1 
    # Linkk X
    set bgcolor "AntiqueWhite"
    set titlesel 1
    set xaxissel 1
    set yaxissel 1
    set newgdata 1
    set sname $tdata

    	set name_schema_pair [OpenAndDefineDataSources 1 $tdata]
    	puts "name_schema_pair after OpenAndDefineDataSources = $name_schema_pair"
    	set tdata [lindex $name_schema_pair 0]
    	set is_dte_type [lindex $name_schema_pair 2]
    	if {$name_schema_pair == ""} { return }
    	if {$byAttr == "Y" || [string trimleft $byAttr "t."] == "Y"} { 
		set byAttr "X" 
		set linksel 0 }
    #Y attr will be mapped to X in the new mapping
    puts "result=$result"

    MacroDefAutoActual $tdata $viewsel $linksel $windowsel \
            $titlesel $xaxissel $yaxissel $byAttr $ylist $newgdata $aggregate
}

############################################################
proc DoHistStat {} {
    global curView windowsel

    if {[WindowVisible .histStat]} {
        destroy .histStat
    }
    if {$curView == ""} {
        dialog .viewNotFound "No view selected" "" 0 OK
        return
    }

puts [DEVise getHistViewname $curView]

    set histdata [DEVise getHistogram $curView]
    set min [lindex $histdata 0]
    set max [lindex $histdata 1]
    set buckets [lindex $histdata 2]

    set windowsel [DEVise getViewWin $curView]

    toplevel 	.histStat
    wm title	.histStat "Histogram Stat"
    wm geometry .histStat +150+150

    set labelwidth 10
    set entrywidth 20

    frame .histStat.viewFrame
    label .histStat.viewLabel -text "view" -width $labelwidth
    label .histStat.viewValue -text $curView -width $entrywidth -anchor w
    pack .histStat.viewLabel .histStat.viewValue \
            -in .histStat.viewFrame -side left
    pack .histStat.viewFrame -side top

    frame .histStat.winFrame
    label .histStat.windowLabel -text "window" -width $labelwidth
    menubutton .histStat.windowsel -relief raised \
		-textvariable windowsel -menu .histStat.windowsel.windowMenu \
		-width $entrywidth
    pack .histStat.windowLabel .histStat.windowsel -in .histStat.winFrame \
		-side left -pady 1m
    pack .histStat.winFrame -side top

    menu .histStat.windowsel.windowMenu -tearoff 0
    foreach w [WinSet] {
        .histStat.windowsel.windowMenu add command -label $w \
                -command "set windowsel {$w}"
    }
    .histStat.windowsel.windowMenu add separator
    .histStat.windowsel.windowMenu add command -label "New..." -command {
        set newwin [DoCreateWindow "Select window type"]
        if {$newwin != ""} {
            set windowsel $newwin
            .histStat.windowsel.windowMenu add command -label $windowsel \
                    -command "set windowsel {$windowsel}"
        }
    }

    frame .histStat.minFrame
    label .histStat.minLabel -text "min" -width $labelwidth
    entry .histStat.minEntry -width $entrywidth
    .histStat.minEntry insert 0 $min
    pack .histStat.minLabel .histStat.minEntry -in .histStat.minFrame -side left
    pack .histStat.minFrame -side top

    frame .histStat.maxFrame
    label .histStat.maxLabel -text "max" -width $labelwidth
    entry .histStat.maxEntry -width $entrywidth
    .histStat.maxEntry insert 0 $max
    pack .histStat.maxLabel .histStat.maxEntry -in .histStat.maxFrame -side left
    pack .histStat.maxFrame -side top

    frame .histStat.bucketsFrame
    label .histStat.bucketsLabel -text "buckets" -width $labelwidth
    entry .histStat.bucketsEntry -width $entrywidth
    .histStat.bucketsEntry insert 0 $buckets
    pack .histStat.bucketsLabel .histStat.bucketsEntry \
            -in .histStat.bucketsFrame -side left
    pack .histStat.bucketsFrame -side top

    set bucketWidth [expr (double($max) - $min) / $buckets ]
    frame .histStat.widthFrame
    label .histStat.widthLabel -text "width" -width $labelwidth
    label .histStat.widthValue -text $bucketWidth -width $entrywidth \
            -anchor w
    pack .histStat.widthLabel .histStat.widthValue \
            -in .histStat.widthFrame -side left
    pack .histStat.widthFrame -side top

    frame .histStat.bottom
    button .histStat.apply -text Apply -width 15 -command {
        set min [.histStat.minEntry get]
        set max [.histStat.maxEntry get]
        set buckets [.histStat.bucketsEntry get]
        set bucketWidth [expr (double($max) - $min) / $buckets ]
        .histStat.widthValue configure -text $bucketWidth
        set view [.histStat.viewValue cget -text]
	set tdata "Hist: $view"
        DEVise setHistogram $curView $min $max $buckets
	if {![DEVise exists $tdata]} { 
            set viewsel 2  
            # Bar Chart
            set linksel 0
            # Linkk X
            # set bgcolor "AntiqueWhite"
            set titlesel 1
            set xaxissel 1
            set yaxissel 1
            set newgdata 1
            set ylist Value
            set x Bucket
            
            scanDerivedSources
            updateDerivedSources
            OpenAndDefineDataSources 1 "{$tdata}"
            #set name_schema_pair [OpenAndDefineDataSources 1 $tdata]
            #puts "name_schema_pair + $name_schema_pair"
            
            MacroDefAutoActual "{$tdata}" $viewsel $linksel $windowsel \
                    $titlesel $xaxissel $yaxissel $x $ylist $newgdata 
        }
    }

    button .histStat.close -text Close -width 15 \
            -command { destroy .histStat }
    pack .histStat.apply .histStat.close \
            -in .histStat.bottom -side left -pady 1m 
    pack .histStat.bottom -side top

#    grab set .histStat
#    tkwait window .histStat
}

############################################################

proc DoColorStat {} {
    global derivedSourceList curView

    if {![CurrentView]} {
	return
    }

    scanDerivedSources

    set sname "Stat: $curView"
    set sname [ OpenAndDefineDataSources 1 [list $sname] ]
    if {$sname == ""} {
        return
    }

    set err [catch {set sourcedef $derivedSourceList($sname)}]
    if {$err} {
        dialog .viewNotFound "Derived Data Not Found" \
                "Derived data\n$sname\nnot found." "" 0 OK
        return
    }

    set schemafile [lindex $sourcedef 3]

    set schemaname [file tail $schemafile]
    MacroDefAuto $sname $schemaname
}

############################################################

proc DoSwapView {} {
    global curView lastView dialogListVar

    if {$curView == "" || $lastView == "" || $curView == $lastView} {
	set but [dialog .swapViewError "Swap View Error" \
		"Select two views by clicking in them in order.\n\
		The views must be in the same window." "" 0 OK ]
	return
    }

    # Find all views in a window
    set win1 [DEVise getViewWin $curView]
    set win2 [DEVise getViewWin $lastView]
    if {$win1 != $win2} {
	set but [dialog .swapViewError "Swap View Error" \
		"The views to be swapped must be in the same window." \
		"" 0 OK ]
	return
    }

    DEVise swapView $win1 $curView $lastView
}

############################################################

proc DestroyView {view} {
    global curView
    if {$view == ""} { return 1 }
    DEVise removeView $view
    DEVise destroy $view
    UpdateLinkCursorInfo
    if { $view == $curView } { set curView "" }
    return 0
}

############################################################

proc CurrentView {} {
    global curView

    if {$curView != ""} {
	return 1
    }

    dialog .noCurrentView "No Current View" \
	    "Select a view first by clicking in it." "" 0 OK

    return 0
}

############################################################

# Show message telling user to select a view to link.

proc LinkViewMsg {} {
    global linkViewStatus currentLink

    # Figure out whether the link is a master/slave link.
    set linkFlag [DEVise getLinkFlag $currentLink]
    set isMasterSlaveLink [expr $linkFlag == 128 || $linkFlag == 1024]

    # Figure out the right message to show.
    if { $isMasterSlaveLink } {
	if { $linkViewStatus == 1 } {
	    set message "Click OK, then select leader view."
	} else {
	    set message "Click OK, then select follower view."
	}
    } else {
	if { $linkViewStatus == 1 } {
	    set message "Click OK, then select first view to link."
	} else {
	    set message "Click OK, then select second view to link."
	}
    }

    set answer [dialog .getLinkView "Select View" $message "" 0 OK Cancel]
    if { $answer == 1 } {
	set linkViewStatus 0
	UpdateLinkCursorInfo
    }
    #TEMP -- it would be good to disable all of the other menus, etc., here
}

############################################################

# If we're in the right state (creating a link), link the view
# that was just selected.

proc LinkSelectedView {} {
    global linkViewStatus curView currentLink

    if { $linkViewStatus == 0 } {
	return
    }

    #
    # Figure out whether the link is a master/slave link.
    #
    set linkFlag [DEVise getLinkFlag $currentLink]
    set isMasterSlaveLink [expr $linkFlag == 128 || $linkFlag == 1024]

    if { $linkViewStatus == 1 } {

	#
	# Link the first or master view; if this is a TAttr/set link, also
	# set the link attribute.
	#
        set check 1
        if { $linkFlag == 128  } {
	    set check [CheckTData $currentLink $curView 0] 
        }
        if { $check } {
	    if { $isMasterSlaveLink} {
	        DEVise setLinkMaster $currentLink $curView 
		if { $linkFlag == 1024 } {
	            SetLinkAttr $currentLink
	        }
	    } else {
	        DEVise insertLink $currentLink $curView
	    }
        }

	#
	# Have the user select the second or slave view.
	#
	set linkViewStatus 2
        LinkViewMsg

    } elseif { $linkViewStatus == 2 } {

	#
	# Make sure this view isn't already linked to this link.
	#
        set linkMaster [DEVise getLinkMaster $currentLink]
        set linkViews [DEVise getLinkViews $currentLink]
	if { $linkMaster == $curView || \
	  [lsearch -exact $linkViews $curView] != -1 } {
	    set message "View $curView is already linked by this link.  Please select another view."
	    set answer [dialog .viewAlreadyLinked "View Already Linked" \
	      $message "" 0 OK Cancel]
	    if {$answer == 1} {
		set linkViewStatus 0
	    }
	    UpdateLinkCursorInfo
	    return
	}

	#
	# Link the second or slave view.
	#
        set check 1
        if { $linkFlag == 128  } {
	    set check [CheckTData $currentLink $curView 0] 
        }
        if { $check } {
	    DEVise insertLink $currentLink $curView
        }

	#
	# We're done.
	#
	set linkViewStatus 0
	UpdateLinkCursorInfo
    }
}

############################################################
# Finds and returns the "top" (last drawn) view in the pile containing
# the given view; if the given view isn't in a pile, it just returns the
# given view.

proc FindTopView { view } {

    if {[DEVise getViewPileMode $view]} {
	set win [DEVise getViewWin $view]
	set view [lindex [DEVise getWinViews $win] end]
    }

    return $view
}

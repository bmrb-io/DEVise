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
	return
    }

    set lastView $curView

    if {$curView != ""} {
	# unhighlight old view
	DEVise highlightView $curView 0
    }
 
    set curView $view

    Update3DLocation
    Update2DQueryWindow
    Update2DBasicStatWindow
    UpdateHistoryWindow

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
    UpdateMappingDialog .editMapping $curView

    if {[WindowExists .stack]} {
	.stack.title.text configure -text "View: $curView"
    }
    .mbar.gdata configure -state normal
    .mbar.view configure -state normal
    .panel.query.button2 configure -state normal

    # highlight new view
    DEVise highlightView $curView 1
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
	set newwin [DoCreateWindow "Select window type"]
	if {$newwin != ""} {
	    set window $newwin
	    .copy.top.old.window.menu.list add command -label $window \
		    -command "set window {$window}"
	}
    }

    frame .copy.bot.but
    pack .copy.bot.but -side top

    button .copy.bot.but.ok -text OK -width 10 -command {
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
    set stat [DEVise getAxisDisplay $view X]
    eval DEVise setAxisDisplay {$newView} X $stat
    set stat [DEVise getAxisDisplay $view Y]
    eval DEVise setAxisDisplay {$newView} Y $stat
	
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

    set answer [ dialogList .bringView "Bring View Back" \
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
}

############################################################

# Make current view holder of a a record cursor
proc DoSetRecordCursor {} {
    NotImplemented
}

############################################################

# Create a link, return its name
proc DoLinkCreate { isRec } {
    global dialogCkButVar 

    while { 1 } {
	if { $isRec} {
	    set flag 128
	} else {
	    set flag 3
	}
	set but [dialogCkBut .createLink "Create Link" \
		"Enter parameters for creating a new link" "" \
		0 { OK Info Cancel } name \
		{x y color size pattern orientation shape record} $flag]
	
	if { $but == 2 } {
	    return ""
	} elseif  { $but == 1 } {
	    DoDisplayLinkInfo
	} else {
	    set flag $dialogCkButVar(selected)
	    set name $dialogCkButVar(entry)
	    if {$name == ""} {
		set but [dialog .noName "No Link Name" \
			"No Link Name Specified" "" 0 OK]
		continue
	    }
	
	    if { [DEVise exists $name ] } {
		set but [dialog .linkExists "Link Exists" \
			"Link $name already exists" "" 0 OK]
		continue
	    }
	    
            set type -1
            if {[ expr ($flag & 128) ] }  {
	        set type 0
	        puts " link type + " 
	        set flag 128
            } elseif { [ expr ($flag & 256) ] } {
	        set type 1
	        puts " link type - " 
	        set flag 128
            }
	    set result [DEVise create link Visual_Link $name $flag]
       	    if { $flag == 128  } {
	        DEVise setLinkType $name $type
            }
	    if {$result == ""} {
		set but [ dialog .linkError "Link Error" \
			"Cannot create link $name" "" 0 OK]
		continue
	    }
	    break
	}
    }
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
	set type "Record"
	set master [DEVise getLinkMaster $link]
	set labels [list  [list Type 10 "$type Link" ] \
		          [list Master 10 $master] ]
    } else {
	set type "Visual"
	set labels [list [list Type 10 "$type Link" ] ]
    }
    dialogInfo .linkInfo "Link Info" "Views linked by $link" "" \
    	    0 OK $views  $labels  \
	    [list $flag 20 2 4 col x y color size pattern \
	    orientation shape record ]
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
}

############################################################
proc DoSetLinkType {} {
    global dialogListVar

    set linkSet [RecordLinkSet]
    if { [llength $linkSet] == 0 } {
	dialog .noLinks "No Links" "There are no links." "" 0 OK
	return
    }
    set answer [ dialogList .getLink "Select Link" \
	    "Select a link" "" 0 \
	    { + - Cancel} $linkSet ]		
    set linkname $dialogListVar(selected)
    if { $answer == 0} {
	DEVise setLinkType $linkname 0
    } elseif { $answer == 1} {
	DEVise setLinkType $linkname 1
    } elseif { $answer == 2} {
	return
    }
    set linkMaster [DEVise getLinkMaster $linkname]
    DEVise refreshView $linkMaster
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
	    puts "nnnnnnnn"
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
    }
}

############################################################
proc CheckTData { link view isMaster } {
    set check [DEVise checkTDataForRecLink $link $view 1]
    set but 0
    if {!$check } {
	set but [ dialog .tdataCheck "Linking different TData sources" \
		"The tdata for the master and slave views does not match \n Do you wish to go ahead ? "\
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

proc DoSetFgBgColor { isForeground {perWindow 0} } {
    global curView newColor
    global getColorCanceled

    if {![CurrentView]} {
	return
    }

    if {$isForeground} {
        set paramIndex 5
    } else {
        set paramIndex 6
    }

    getColor newColor
    if {$getColorCanceled} {
	return
    }

    if {$perWindow} {
	set viewList [DEVise getWinViews [DEVise getViewWin $curView]]
    } else {
        set viewList [list $curView]
    }

    foreach viewName $viewList {
        set curViewClass [GetClass view $viewName]
        set param [DEVise getCreateParam view $curViewClass $viewName]
        set oldColor [lindex $param $paramIndex]

        if {$oldColor != $newColor} {
            set param [lreplace $param $paramIndex $paramIndex $newColor]
            set cmd "DEVise changeParam $param"
puts "DIAG $cmd"
            eval $cmd
	}
    }
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

    button .setTitle.bot.but.ok -text OK -width 10 -command {
	DEVise setLabel $curView 1 16 $newTitle
	destroy .setTitle
    }
    button .setTitle.bot.but.clear -text Clear -width 10 -command {
	set newTitle ""
    }
    button .setTitle.bot.but.delete -text Delete -width 10 -command {
	DEVise setLabel $curView 0 12 ""
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

proc DoSetOverrideColor {} {
    global curView DEViseOldColors newColor newColorActive

    if {[WindowVisible .setColor]} {
	return
    }

    if {![CurrentView]} {
	return
    }

    toplevel .setColor
    wm title .setColor "Set View Override Color"
    wm geometry .setColor +50+50

    frame .setColor.top
    frame .setColor.bot
    pack .setColor.top -side top -pady 3m -fill both -expand 1
    pack .setColor.bot -side top -pady 5m -fill x

    frame .setColor.bot.but
    pack .setColor.bot.but -side top

    set viewColorParams [DEVise getViewOverrideColor $curView]
    set hasOverrideColor [lindex $viewColorParams 0]
    set overrideColor [lindex $viewColorParams 1]
    set newColor [findNameValue $DEViseOldColors $overrideColor]
    set newColorActive $hasOverrideColor

    button .setColor.top.b1 -text "Change Color..." -width 20 -bg $newColor \
	    -command {
	getColor newColor true
	.setColor.top.b1 configure -background $newColor
    }
    checkbutton .setColor.top.c1 -text "Enabled" -variable newColorActive
    pack .setColor.top.b1 .setColor.top.c1 \
	    -side left -padx 3m -fill both -expand 1

    button .setColor.bot.but.ok -text OK -width 10 -command {
	set colorValue [findNumericValue $DEViseOldColors $newColor]
	DEVise setViewOverrideColor $curView $newColorActive $colorValue
	destroy .setColor
    }
    button .setColor.bot.but.cancel -text Cancel -width 10 -command {
	destroy .setColor
    }
    pack .setColor.bot.but.ok .setColor.bot.but.cancel \
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
    entry .basicStat.maxEntry -text "" -relief sunken -width 10 \
			-fg black -bg LightGray -font 8x13
    button .basicStat.minButton -text Min -width 10 -command { \
                        set statmin [flip $statmin] 
                        DoToggleStatistics }
    entry .basicStat.minEntry -text "" -relief sunken -width 10 \
                        -fg black -bg LightGray -font 8x13
    pack .basicStat.maxButton .basicStat.maxEntry .basicStat.minButton \
			.basicStat.minEntry -in .basicStat.mmmc.upper  \
			-side left -expand $expand -fill $fill

    button .basicStat.meanButton -text Mean -width 10 -command { \
                        set statmean [flip $statmean] 
                        DoToggleStatistics }
    entry .basicStat.meanEntry -text "" -relief sunken -width 10 \
                        -fg black -bg LightGray -font 8x13
    button .basicStat.countButton -text Count -width 10 -command { \
                        set statcount [flip $statcount] 
                        DoToggleStatistics }
    entry .basicStat.countEntry -text "" -relief sunken -width 10 \
                        -fg black -bg LightGray -font 8x13
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
    }
    if {$windowsel == ""} {
        set windowsel "None selected yet"
    }
   
    menu .groupBy.windowsel.windowMenu -tearoff 0
    foreach w [WinSet] {
        .groupBy.windowsel.windowMenu add command -label $w \
                -command "set windowsel {$w}"
    }

    .groupBy.windowsel.windowMenu add separator
    .groupBy.windowsel.windowMenu add command -label "New..." -command {
        set newwin [DoCreateWindow "Select window type"]
        if {$newwin != ""} {
            set windowsel $newwin
            .groupBy.windowsel.windowMenu add command -label $windowsel \
                    -command "set windowsel {$windowsel}"
        }
    }

    set bylist { Y Color}
    set onlist { X Y Z}
    set category { Max Min Avg Count Sum }

    label .groupBy.byLabel -text "Group By" 
    menubutton .groupBy.xsel -relief raised \
            -textvariable byAttr -menu .groupBy.xsel.xselMenu -width 20
    menu .groupBy.xsel.xselMenu -tearoff 0

    set t ""
    set result [DEVise checkGstat $curView]
    if { $result == "0" } {set t "t."}

    set xDate "0"
    set xDate [DEVise isXDateType $curView]
    if { $xDate == "1" } {
	.groupBy.xsel.xselMenu add radiobutton -label DATE -variable byAttr
	set byAttr "DATE"
    } else {
	.groupBy.xsel.xselMenu add radiobutton -label X -variable byAttr
	set Xattr "X"
	set byAttr $t$Xattr
    }
    foreach att $bylist {
        .groupBy.xsel.xselMenu add radiobutton -label $t$att -variable byAttr
    }
    pack .groupBy.byLabel .groupBy.xsel \
		-in .groupBy.top.right  -side left -padx 1m

#    checkbutton .groupBy.byx -text X -width 10 -anchor w \
#		-variable byx
#    checkbutton .groupBy.byy -text Y -width 10 -anchor w \
#		-variable byy
#    checkbutton .groupBy.bycolor -text Color -width 10 -anchor w \
#		-variable bycolor
#    pack .groupBy.byLabel .groupBy.byx .groupBy.byy .groupBy.bycolor \
#		-in .groupBy.top -side left -expand 1 -fill both

    label .groupBy.mid.onLabel -text "Aggregate On"
    pack .groupBy.mid.onLabel -side left -fill y
#    menubutton .groupBy.onAttr -relief raised \
#            -textvariable onAttr -menu .groupBy.onMenu -width 20
#    menu .groupBy.onMenu -tearoff 0
#    foreach att $onlist {
#        add radiobutton -label $att -variable onAttr
#    }

    frame .groupBy.mid.fx -relief groove -bd 1
    frame .groupBy.mid.fy -relief groove -bd 1
    frame .groupBy.mid.fz -relief groove -bd 1
    pack .groupBy.mid.fx .groupBy.mid.fy .groupBy.mid.fz \
		-side left -expand 1 -fill both
    set aggregate "Y"
    radiobutton .groupBy.mid.fx.onx -text X -width 10 -anchor w \
		-variable aggregate -value "X"
    radiobutton .groupBy.mid.fy.ony -text Y -width 10 -anchor w \
		-variable aggregate -value "Y"
#    radiobutton .groupBy.mid.fz.onz -text Z -width 10 -anchor w \
#		-variable aggregate -value "Z"
#    pack .groupBy.mid.fx.onx .groupBy.mid.fy.ony .groupBy.mid.fz.onz \
#		-side top -padx 2m -pady 1m
    pack .groupBy.mid.fx.onx .groupBy.mid.fy.ony \
		-side top -padx 2m -pady 1m

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

    set tdata ""
    set pos [string first "Color" $byAttr]
    if {$pos > 0 || $byAttr == "Color"} {
	set tdata "{Stat: $curView}"
	set byAttr [string range $byAttr $pos end]
    } else {
	if {$aggregate == "Y"} {
    		set tdata "{GstatX: $curView}"
		set sourceName "GstatX: $curView"
	} elseif {$aggregate == "X" } {
    		set tdata "{GstatY: $curView}"
		set sourceName "GstatY: $curView"
	}
    }
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

    scanDerivedSources
    updateDerivedSources

    if { $xDate == "1" && $byAttr == "DATE"} {
    	set sourceName [string trimleft $sname "{"]
    	set sourceName [string trimright $sourceName "}"]
	set schematype GDATASTAT_DATE
	set schemafile $schemadir/physical/GDATASTAT_DATE
	set derivedSourceList($sourceName) [lreplace $derivedSourceList($sourceName) 2 3 $schematype $schemafile]
    }

    set name_schema_pair [OpenAndDefineDataSources 1 $tdata]
    if {$name_schema_pair == ""} { return }
    if {$byAttr == "Y" || [string trimleft $byAttr "t."] == "Y"} { 
	set byAttr "X" 
	set linksel 0 }
    #Y attr will be mapped to X in the new mapping
    MacroDefAutoActual $tdata $viewsel $linksel $windowsel $bgcolor \
            $titlesel $xaxissel $yaxissel $byAttr $ylist $newgdata 

}

############################################################
proc DoHistStat {} {
    global curView derivedSourceList
    global tdata viewsel linksel windowsel bgcolor
    global titlesel xaxissel yaxissel byAttr ylist newgdata

    if {[WindowVisible .histStat]} {
	return 
    }

    toplevel 	.histStat
    wm title	.histStat "Histogram Stat"
    wm geometry .histStat +150+150

    frame .histStat.top
    frame .histStat.upper
    frame .histStat.bottom
    pack .histStat.top -side top -pady 1m -expand 1 -fill both
    pack .histStat.upper -side top -pady 1m -expand 1 -fill both
    pack .histStat.bottom -side top -pady 1m -expand 1 -fill both

    label .histStat.windowLabel -text "Window: "
    menubutton .histStat.windowsel -relief raised \
		-textvariable windowsel -menu .histStat.windowsel.windowMenu \
		-width 20
    pack .histStat.windowLabel .histStat.windowsel -in .histStat.top \
		-side left -pady 1m

    set windowsel ""
    if {$curView != ""} {
        set windowsel [DEVise getViewWin $curView]
    }
    if {$windowsel == ""} {
        set windowsel "None selected yet"
    }
   
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

    label .histStat.histLabel -text "Number of buckets"
    entry .histStat.buckEntry -text "" -relief  sunken -width 15 \
		-fg black -bg LightGray -font 8x13
    .histStat.buckEntry insert 0 50

    pack .histStat.histLabel .histStat.buckEntry -in .histStat.upper \
		-side left -pady 1m 

    button .histStat.ok -text OK -width 15 -command {
	set numBucks [.histStat.buckEntry get]
	DEVise setBuckRefresh $curView $numBucks 
	set tdata ""
	set tdata "{Hist: $curView}"
	set tdata_1 "Hist: $curView"
	set exist [DEVise exists $tdata_1]
	if {![DEVise exists $tdata_1]} { 
	 	set viewsel 2  
		# Bar Chart
		set linksel 0
        	# Linkk X
    		set bgcolor "AntiqueWhite"
   		set titlesel 1
	        set xaxissel 1
       		set yaxissel 1
       		set newgdata 1
		set sname "{Hist: $curView}"
       		set ylist Value
        	set x Bucket

		scanDerivedSources
    		updateDerivedSources
		set name_schema_pair [OpenAndDefineDataSources 1 $tdata]
		puts "name_schema_pair + $name_schema_pair"

		MacroDefAutoActual $tdata $viewsel $linksel $windowsel $bgcolor \
            	   $titlesel $xaxissel $yaxissel $x $ylist $newgdata 
		}
		return
    }
    button .histStat.cancel -text Cancel -width 15 -command {
		destroy .histStat }
    pack .histStat.ok .histStat.cancel -in .histStat.bottom -side left -pady 1m 
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
    if {$curView == ""} { return 1 }
    DEVise removeView $view
    DEVise destroy $view
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


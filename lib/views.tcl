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
#  Revision 1.25  1997/02/03 20:02:02  ssl
#  Added interface for negative record links and user defined layout mode
#
#  Revision 1.24  1997/02/03 04:12:42  donjerko
#  Catalog management moved to DTE
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
proc DoLinkCreate {} {
    global dialogCkButVar 

    set but [dialogCkBut .createLink "Create Link" \
	    "Enter parameters for creating a new link" "" \
	     0 { OK Cancel } name \
	    {x y color size pattern orientation shape record_positive record_negative } {3}]
    if { $but != 0 } {
	return ""
    }

    set flag $dialogCkButVar(selected)
    set name $dialogCkButVar(entry)
    if {$name == ""} {
	set but [dialog .noName "No Link Name" \
		"No Link Name Specified" "" 0 OK]
	return 
    }
    
    if { [DEVise exists $name ] } {
	set but [dialog .linkExists "Link Exists" \
		"Link $name already exists" "" 0 OK]
	return 
    }
    set type -1
    if {[ expr ($flag & 128) ] }  {
	set type 0
	puts " link type + " 
	set flag 128
    } else if { [ expr ($flag & 256) } {
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
	return
    }
    return $name
}
############################################################

# Display the views in a link 
proc DisplayLinkInfo { link } {
    set views [DEVise getLinkViews $link]
    if { [llength $views] == 0 } {
	dialog .linkInfo "No Views in Link" \
		"Link $link currently does not link any view" "" 0 OK
	return
    }
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
	    set link [DoLinkCreate]
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
		set link [ DoLinkCreate ]
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

    DEVise insertLink $link $curView
}

############################################################

# Unlink current view
proc DoViewUnlink {} {
    global curView dialogListVar

    if {![CurrentView]} {
	return
    }

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
    } else {
	set answer [ dialogList .getLink "Select Link" \
		"Select a link to unlink" "" 0 \
		{ OK Cancel } $viewLinks ]
	if { $answer == 1 } {
	    return
	} elseif { $dialogListVar(selected) == "" } {
	    return
	} else {
	    DEVise unlinkView $dialogListVar(selected) $curView
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
		"Cannot delete link, unlink views first" "" 0 OK ]
	return
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
	set link [DoLinkCreate]
	if {$link == ""} {
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
	    DoLinkCreate
	} elseif {$answer == 3 || $dialogListVar(selected) == ""} {
	    return
	} else {
	    set link $dialogListVar(selected)
	    break
	}
    }

    DEVise setLinkMaster $link $curView
}


############################################################

# Set link to be without a master
proc DoResetLinkMaster {} {
    global dialogListVar

    set linkSet [RecordLinkSet]
    if { [llength $linkSet] == 0 } {
	dialog .noLinks "No Links" "There are no links." "" 0 OK
	return
    }

    while {1} {
	set answer [ dialogList .getLink "Select Link" \
		"Select a link to reset" "" 0 \
		{ OK Info Cancel OK } $linkSet ]
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

    if {![CurrentView]} {
	return
    }

    if {$isForeground} {
        set paramIndex 5
    } else {
        set paramIndex 6
    }

    getColor newColor
    if {$newColor == "cancel"} {
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
    global curView DEViseColors newColor newColorActive

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
    set newColor [findNameValue $DEViseColors $overrideColor]
    set newColorActive $hasOverrideColor

    button .setColor.top.b1 -text "Change Color..." -width 20 -bg $newColor \
	    -command {
	getColor newColor
	.setColor.top.b1 configure -background $newColor
    }
    checkbutton .setColor.top.c1 -text "Enabled" -variable newColorActive
    pack .setColor.top.b1 .setColor.top.c1 \
	    -side left -padx 3m -fill both -expand 1

    button .setColor.bot.but.ok -text OK -width 10 -command {
	set colorValue [findNumericValue $DEViseColors $newColor]
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


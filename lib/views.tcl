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

proc ProcessViewCreated { view } {
}

############################################################

proc ProcessViewDestroyed { view } {
}

############################################################

proc ProcessViewSelected { view } {
    global curView lastView historyWinOpened

    if {$view == $curView} {
	return
    } else {
	set lastView $curView
    }

    if {$curView != ""} {
	# unhighlight old view
	DEVise highlightView $curView 0
    }

    set curView $view
    .viewFrame.viewName configure -text $curView

    ClearHistory

    if {$curView == ""} {
	return
    }

    # highlight new view
    DEVise highlightView $curView 1

    set filters [DEVise getVisualFilters $curView]
    # puts "setting filter $curView $filters"
    foreach filter $filters { 
	ProcessViewFilterChange $curView -1 \
		[lindex $filter 0] [lindex $filter 1] \
		[lindex $filter 2] [lindex $filter 3] [lindex $filter 4]
    }
}

############################################################

proc ProcessViewFilterChange { view flushed xLow yLow xHigh yHigh marked} {
    global curView historyWinOpened

    if {$view != $curView} {
	return
    }

    # Change the control panel's entry box
    foreach i { xlow ylow xhigh yhigh} {
	.history.$i delete 0 end
    }
    .history.xlow insert 0 $xLow
    .history.ylow insert 0 $yLow
    .history.xhigh insert 0 $xHigh
    .history.yhigh insert 0 $yHigh

    if {$historyWinOpened} {
	if {$flushed >= 0} {
	    # Remove last in the list 
	    foreach i { listMark listXlow listXhigh listYlow listYhigh } {
		set size [.historyWin.$i size]
		.historyWin.$i delete [expr $size-$flushed-1]
	    }
	}

	# Insert new filter 
	.historyWin.listXlow insert 0 $xLow
	.historyWin.listXhigh insert 0 $xHigh
	.historyWin.listYlow insert 0 $yLow
	.historyWin.listYhigh insert 0 $yHigh
	if {$marked} {
	    .historyWin.listMark insert 0 "*"
	} else {
	    .historyWin.listMark insert 0 " "
	}
    }
}

############################################################

proc ProcessUseButton {} {
    global curView

    if {$curView == ""} {
	return
    }

    set xlow [DEVise parseDateFloat [.history.xlow get]]
    set ylow [DEVise parseDateFloat [.history.ylow get]]
    set xhigh [DEVise parseDateFloat [.history.xhigh get]]
    set yhigh [DEVise parseDateFloat [.history.yhigh get]]

    if {$xlow >= $xhigh} {
	dialog .useError "Invalid Visual Filter" \
		"X low $xlow >= X high $xhigh." \
		"" 0 Cancel
	return
    }

    if {$ylow >= $yhigh} {
	dialog .useError "Invalid Visual Filter" \
		"Y low $ylow >= Y high $yhigh." \
		"" 0 Cancel
	return
    }

    DEVise setFilter $curView $xlow $ylow $xhigh $yhigh
}

############################################################

proc ProcessLeftButton {} {
    global curView left_amount
    if {$curView == ""} {
	return
    }
    set filter [DEVise getCurVisualFilter $curView ]

    if { $left_amount <= 0.0 } {
	set but [dialog .leftError "Scroll Left Error" \
		"Cannot scroll left $left_amount screen." \
		"" 0 {Cancel}  ]
	return
    }
    set xlow [lindex $filter 0 ]
    set ylow [lindex $filter 1 ]
    set xhigh [lindex $filter 2 ]
    set yhigh [lindex $filter 3 ]

    set width [expr $xhigh-$xlow]
    set scrollAmount [expr $width*$left_amount]
    set xlow [expr $xlow+$scrollAmount]
    set xhigh [expr $xhigh+$scrollAmount]
    
    DEVise setFilter $curView $xlow $ylow $xhigh $yhigh
}

############################################################

proc ProcessRightButton {} { 
    global curView right_amount
    if {$curView == ""} {
	return
    }
    set filter [DEVise getCurVisualFilter $curView ]

    if { $right_amount <= 0.0 } {
	set but [dialog .rightError "Scroll Right Error" \
		"Cannot scroll right $right_amount screen." \
		"" 0 {Cancel}  ]
	return
    }
    set xlow [lindex $filter 0 ]
    set ylow [lindex $filter 1 ]
    set xhigh [lindex $filter 2 ]
    set yhigh [lindex $filter 3 ]
    
    set width [expr $xhigh-$xlow]
    set scrollAmount [expr $width*$right_amount]
    set xlow [expr $xlow-$scrollAmount]
    set xhigh [expr $xhigh-$scrollAmount]
    
    DEVise setFilter $curView $xlow $ylow $xhigh $yhigh
}

############################################################

proc ProcessUpButton {} { 
    global curView up_amount
    if {$curView == ""} {
	return
    }
    set filter [DEVise getCurVisualFilter $curView ]
    
    if { $up_amount <= 0.0 } {
	set but [dialog .upError "Scroll Up Error" \
		"Cannot scroll up $up_amount screen." \
		"" 0 {Cancel}  ]
	return
    }
    set xlow [lindex $filter 0 ]
    set ylow [lindex $filter 1 ]
    set xhigh [lindex $filter 2 ]
    set yhigh [lindex $filter 3 ]
    
    set height [expr $yhigh-$ylow]
    set scrollAmount [expr $height*$up_amount]
    set ylow [expr $ylow-$scrollAmount]
    set yhigh [expr $yhigh-$scrollAmount]
    
    DEVise setFilter $curView $xlow $ylow $xhigh $yhigh
}

############################################################

proc ProcessDownButton {} {
    global curView down_amount
    if {$curView == ""} {
	return
    }
    set filter [DEVise getCurVisualFilter $curView ]
    
    if { $down_amount <= 0.0 } {
	set but [dialog .downError "Scroll Down Error" \
		"Cannot scroll down $up_amount screen." \
		"" 0 {Cancel}  ]
	return
    }
    set xlow [lindex $filter 0 ]
    set ylow [lindex $filter 1 ]
    set xhigh [lindex $filter 2 ]
    set yhigh [lindex $filter 3 ]
    
    set height [expr $yhigh-$ylow]
    set scrollAmount [expr $height*$down_amount]
    set ylow [expr $ylow+$scrollAmount]
    set yhigh [expr $yhigh+$scrollAmount]
    
    DEVise setFilter $curView $xlow $ylow $xhigh $yhigh
}

############################################################

proc ProcessInButton {} {
    global curView in_amount
    if {$curView == ""} {
	return
    }
    set filter [DEVise getCurVisualFilter $curView ]
    
    if { $in_amount <= 0.0 || $in_amount >= 1.0 } {
	set but [dialog .inError "Zoom In Error" \
		"Cannot zoom in $in_amount screen." \
		"" 0 {Cancel}  ]
	return
    }
    set xlow [lindex $filter 0 ]
    set ylow [lindex $filter 1 ]
    set xhigh [lindex $filter 2 ]
    set yhigh [lindex $filter 3 ]
    
    set width [expr $xhigh-$xlow]
    set scrollAmount [expr $width*$in_amount]
    set mid [ expr ($xhigh+$xlow)/2.0]
    set xlow [expr $mid - ($scrollAmount/2.0) ]
    set xhigh [expr $xlow+$scrollAmount]
    
    DEVise setFilter $curView $xlow $ylow $xhigh $yhigh
}

############################################################

proc ProcessYInButton {} {
    global curView in_amount
    if {$curView == ""} {
	return
    }
    set filter [DEVise getCurVisualFilter $curView ]
    
    if { $in_amount <= 0.0 || $in_amount >= 1.0 } {
	set but [dialog .inError "Zoom In Error" \
		"Cannot zoom in $in_amount screen." \
		"" 0 {Cancel}  ]
	return
    }
    set xlow [lindex $filter 0 ]
    set ylow [lindex $filter 1 ]
    set xhigh [lindex $filter 2 ]
    set yhigh [lindex $filter 3 ]
    
    set width [expr $yhigh-$ylow]
    set scrollAmount [expr $width*$in_amount]
    set mid [ expr ($yhigh+$ylow)/2.0]
    set ylow [expr $mid - ($scrollAmount/2.0) ]
    set yhigh [expr $ylow+$scrollAmount]
    
    DEVise setFilter $curView $xlow $ylow $xhigh $yhigh
}

############################################################

proc ProcessOutButton {} { 
    global curView out_amount
    if {$curView == ""} {
	return
    }
    set filter [DEVise getCurVisualFilter $curView ]
    
    if { $out_amount <= 1.0  } {
	set but [dialog .outError "Zoom Out Error" \
		"Cannot zoom out $out_amount screen." \
		"" 0 {Cancel}  ]
	return
    }
    set xlow [lindex $filter 0 ]
    set ylow [lindex $filter 1 ]
    set xhigh [lindex $filter 2 ]
    set yhigh [lindex $filter 3 ]
    
    set width [expr $xhigh-$xlow]
    set scrollAmount [expr $width*$out_amount]
    set mid [ expr ($xhigh+$xlow)/2.0]
    set xlow [expr $mid - ($scrollAmount/2.0) ]
    set xhigh [expr $xlow+$scrollAmount]
    
    DEVise setFilter $curView $xlow $ylow $xhigh $yhigh
}

############################################################

proc ProcessYOutButton {} { 
    global curView out_amount
    if {$curView == ""} {
	return
    }
    set filter [DEVise getCurVisualFilter $curView ]
    
    if { $out_amount <= 1.0 } {
	set but [dialog .outError "Zoom Out Error" \
		"Cannot zoom out $out_amount screen." \
		"" 0 {Cancel}  ]
	return
    }
    set xlow [lindex $filter 0 ]
    set ylow [lindex $filter 1 ]
    set xhigh [lindex $filter 2 ]
    set yhigh [lindex $filter 3 ]
    
    set width [expr $yhigh-$ylow]
    set scrollAmount [expr $width*$out_amount]
    set mid [ expr ($yhigh+$ylow)/2.0]
    set ylow [expr $mid - ($scrollAmount/2.0) ]
    set yhigh [expr $ylow+$scrollAmount]
    
    DEVise setFilter $curView $xlow $ylow $xhigh $yhigh
}

############################################################

# Process View/Create menu
proc DoViewCreate {} {
    set ans [ DoCreateView "Select a view type" ]
}

############################################################

# Remove view from window
proc DoViewRemove {} {
    global curView
    if {$curView == ""} {
	dialog .removeView "No Current View" \
		"Select a view first by clicking in it." "" 0 OK
	return
    }

    set viewToRemove $curView

    # Unselect current view
    ProcessViewSelected ""

    # Remove view
    set ans [DEVise removeView $viewToRemove]
    if {$ans != ""} {
	dialogList .removeError "Cannot Remove View" $ans "" 0 OK
	return
    }
}

############################################################

# Destroy view
proc DoViewDestroy {} {
    global curView dialogListVar

    set viewName $curView

    if {$viewName != ""} {
	set but [ dialog .removeView "Destroy View" \
		"Okay to destroy view $viewName?" "" 0 \
		Cancel OK Another ]
	if {$but == 0} {
	    return
	}
	if {$but == 2} {
	    set viewName ""
	}
    }

    if {$viewName == ""} {
	set answer [ dialogList .removeView "Destroy View" \
		"Select view to move to a window" "" 0 \
		{ Cancel OK } [ViewSet] ]
	if {$answer == 0 || $dialogListVar(selected) == ""} {
	    return
	}
	set viewName $dialogListVar(selected)
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

    foreach gdata [GdataSet] {
#	puts $gdata
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

    if {$curView == ""} {
	set answer [ dialogList .removeView "Move View" \
		"Select view to move." "" 0 { Cancel OK } \
		[ViewSet] ]
	if {$answer == 0} {
	    return
	}
	set viewName $dialogListVar(selected)
    } else {
	set viewName $curView
    }    

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

    set answer [ dialogList .removeView "Move View" \
	    "Select view to move to a window" "" 0 { Cancel OK } [ViewSet] ]
    if {$answer == 0 || $dialogListVar(selected) == ""} {
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
	    0 {Cancel OK} name {x y} {3}]
    if { $but != 1 } {
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
	set but [ dialog .cursorError "CursorError" \
		"Can't create cursor" "" 0 OK ]
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
	set answer [ dialogList .getCursor "GetCursor" $label "" 0 \
		{ New Cancel OK } $cursorSet ]
	if { $answer == 0 } {
	    set cursor [ DoCursorCreate ]
	} elseif {$answer == 1 || $dialogListVar(selected) == ""} {
	    set cursor ""
	} else {
	    set cursor $dialogListVar(selected)
	}
    }

    return $cursor
}

############################################################

proc DoSetCursorSrc {} {
    global curView dialogListVar

    if {$curView == ""} {
	set but [dialog .cursorError "No Current View" \
		"Select a view first by clicking in it." "" 0 OK ]
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
    global curView dialogListVar

    if {$curView == ""} {
	set but [dialog .cursorError "No Current View " \
		"Select a view first by clicking in it." "" 0 OK ]
	return
    }
    
    set cursor [DoGetCursor "Select cursor for view\n$curView"]
    if {$cursor == ""} {
	return
    }
    
    set answer [DEVise setCursorDst $cursor $curView]
}

############################################################

# Create a link, return its name
proc DoLinkCreate {} {
    global dialogCkButVar 

    set but [dialogCkBut .createLink "Create Link" \
	    "Enter parameters for creating a new link" "" \
	    0 {Cancel OK} name {x y color size pattern orientation shape} {3}]
    if { $but != 1 } {
	return ""
    }

    set flag $dialogCkButVar(selected)
    set name $dialogCkButVar(entry)
    if {$name == ""} {
	set but [dialog .noName "NoName" \
		"Error: No name for link specified" "" 0 {Continue }]
	return 
    }
    
    if { [DEVise exists $name ] } {
	set but [dialog .linkExists "LinkExists" \
		"Error: Link name $name already exists" "" 0 {Continue }]
	return 
    }

    set result [DEVise create link Visual_Link $name $flag]
    if {$result == ""} {
	set but [ dialog .linkError "LinkError" \
		"Can't create link" "" 0 OK ]
	return
    }

    return $name
}

############################################################

# Display the views in a link 
proc DisplayLinkInfo { link } {
    set views [DEVise getLinkViews $link]
    if { [llength $views] == 0 } {
	dialog .linkInfo "LinkInfo" \
		"Link $link currently does not link any view" "" 0 OK ]
	return
    }
    dialogList .linkInfo "LinkInfo" "Views linked by $link" "" 0 OK $views
}

############################################################

# Link current view
proc DoViewLink {} {
    global curView dialogListVar

    if {$curView == ""} {
	set but [dialog .linkError "No Current View" \
		"Select a view first by clicking in it." "" 0 OK ]
	return
    }

    while { 1 } {
	set linkSet [LinkSet]
	if { [llength $linkSet] == 0 } {
	    set link [DoLinkCreate]
	    if {$link == ""} {
		break
	    }
	} else {
	    set answer [ dialogList .getLink "GetLink" "Select a link for\
		    view\n$curView" "" 0 \
		    { Info New Cancel OK } $linkSet ]
	    if { $answer == 0 } {
		if {$dialogListVar(selected) != ""} {
		    DisplayLinkInfo $dialogListVar(selected)
		}
	    } elseif { $answer == 1 } {
		set link [ DoLinkCreate ]
	    } elseif {$answer == 2 || $dialogListVar(selected) == ""} {
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

    set anwer [DEVise insertLink $link $curView]
    # if {$anwer != 0 } {
	# set but [ dialog .linkError "LinkError" \
		# "Can't insert view $curView into link $link" "" 0 OK ]
    #}
}

############################################################

# Unlink current view
proc DoViewUnlink {} {
    global curView dialogListVar
    if {$curView == ""} {
	set but [dialog .unlinkError "No Current View" \
		"Select a view first by clicking in it." "" 0 OK ]
	return
    }

    set linkSet [LinkSet]
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
		{ Cancel OK } $linkSet ]
	if { $answer == 0 } {
	    return
	} elseif { $dialogListVar(selected) == "" } {
	    return
	} else {
	    DEVise unlinkView $dialogListVar(selected) $curView
	}
    }
}

############################################################

# Modify parameters of a link
proc DoModifyLink {} {
    global dialogCkButVar dialogListVar
    set linkSet [LinkSet]
    if { [llength $linkSet ] == 0 } {
	set but [dialog .linkModError "LinkModError" \
		"No link has been created yet" "" 0 OK ]
	return
    }
    
    # Select a link
    set answer [ dialogList .getLink "GetLink" \
	    "Select a link to modify param" "" 0 \
	    { Cancel OK } $linkSet ]
    if { $answer == 0 } {
	return
    } elseif { [ string compare $dialogListVar(selected) "" ] == 0 } {
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
    if { [ string compare $curView "" ] ==  0 } {
	set but [dialog .viewError "No Current View" \
		"Select a view first by clicking in it." "" 0 OK ]
	return 0
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
	set but [ dialog .axisError "AxisError" \
		"Can't create axis" "" 0 OK ]
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

    set answer [ dialogList .createAxis "CreateAxis"  \
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
	} elseif { $answer == 1 || \
		[ string compare $dialogListVar(selected) "" ] == 0 } {
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
    if { [string compare $result "" ] == 0 } {
	set but [ dialog .actionError "ActionError" \
		"Can't create action" "" 0 OK ]
	return
    }
    return $result
}

############################################################

# Create an action
proc DoCreateAction {} {
    global dialogListVar
    set actionTypes [ DEVise get action]
    set answer [ dialogList .createAction "CreateAction"  \
	    "Select type of action to create" "" "" { Cancel Ok } $actionTypes]
    if { $answer == 0 || \
	    [ string compare $dialogListVar(selected) "" ] == 0 } {
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
	set action [ DoCreateAction]
    } else {
	set answer [dialogList .getAction "GetAction" "Select an \
		action for view\n$curView"\
		"" 0 { New Cancel OK } $actionSet]
	if { $answer == 0 } {
	    set action [ DoCreateAction]
	} elseif { $answer == 1 || \
		[ string compare $dialogListVar(selected) "" ] == 0 } {
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

proc DoSetBgColor {} {
    global curView viewColor

    set but [dialog .setTitleWinError "Not Supported Yet" \
	    "Changing view background color not supported yet." "" 0 OK ]
    return

    if {$curView == ""} {
	set but [dialog .setTitleWinError "No Current View" \
		"Select a view first by clicking in it." "" 0 OK ]
	return
    }
    
    set curViewClass [GetClass view $curView]
    set param [DEVise getCreateParam view $curViewClass $curView]
    set viewColor [lindex $param 5]
    set oldViewColor $viewColor

    getColor viewColor
    if {$oldViewColor == $viewColor} {
	return
    }

    set param [linsert [lrange $param 0 4] end $viewColor]
    set cmd "DEVise changeParam $param"
    eval $cmd
}

############################################################

proc DoSetTitle {} {
    global curView newTitle

    if {$curView == ""} {
	set but [dialog .setTitleWinError "No Current View" \
		"Select a view first by clicking in it." "" 0 OK ]
	return
    }
    
    # see if .setTitle window already exists; if so, just return
    set err [catch {set exists [wm state .setTitle]}]
    if {!$err} { wm deiconify .setTitle; return }

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

proc DoToggleAxis { axis } {
    global curView
    if {$curView == ""} {
	set but [dialog .toggleWinError "No Current View" \
		"Select a view first by clicking in it." "" 0 OK ]
	return
    }
    
    set stat [DEVise getAxisDisplay $curView $axis]
    set stat [expr !$stat]
    DEVise setAxisDisplay $curView $axis $stat
}

############################################################

proc DoToggleAxisAllViews { axis } {
    global curView
    if {$curView == ""} {
	set but [dialog .toggleWinError "No Current View" \
		"Select a view first by clicking in it." "" 0 OK ]
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
    global curView statmean statmax statmin statcurr statcilevel

    # The status is formed by as a binary string xxx where 1 means that the 
    # corr. stat is to be displayed. Further, since "count" which is the 
    # fourth stat cannot currently be displayed meaningfully on the graph
    # always append a 0.
    set statcount 0
    set stat $statmean$statmax$statmin$statcount$statcilevel

    if {$statcurr == 1} {
	if {$curView == ""} {
	    set but [dialog .toggleWinError "No Current View" \
		    "Select a view first by clicking in it." "" 0 OK ]
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

proc DoSwapView {} {
    global curView lastView dialogListVar
    if { [ string compare $curView "" ] ==  0  ||
    [string compare $lastView "" ] == 0 ||
    [string compare $curView $lastView] == 0 } {
	set but [dialog .swapViewError "SwapViewError" \
		"Select two views by clicking in them in order.\n\
		The views must be in the same window." "" 0 OK ]
	return
    }

    # Find all views in a window
    set win1 [DEVise getViewWin $curView]
    set win2 [DEVise getViewWin $lastView]
    if { [string  compare $win1 $win2] != 0 } {
	set but [dialog .swapViewError "SwapViewError" \
		"Select two views by clicking in them in order.\n\
		The views must be in the same window." "" 0 OK ]
	return
    }

    DEVise swapView $win1 $curView $lastView
}

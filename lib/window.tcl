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
#  Revision 1.4  1996/04/23 20:36:46  jussi
#  Replaced Unknown view legend with X.
#
#  Revision 1.3  1996/04/15 15:05:39  jussi
#  Rewrote WindowMerge and WindowSplit code.
#
#  Revision 1.2  1996/04/14 00:07:08  jussi
#  DupWindow now copies all links of the source view into the new view.
#
#  Revision 1.1  1996/04/11 18:13:07  jussi
#  Initial revision.
#

############################################################

proc RemoveWindow {win} {
    global dialogListVar

    if {$win == ""} {
	set winSet [ WinSet ]
	set answer [ dialogList .window "Select Window" \
		"Select window to remove" "" "" \
		{ Cancel Ok } $winSet ]
	if {$answer == 0  || $dialogListVar(selected) == ""} {
	    return 
	}
	set win $dialogListVar(selected)
    }

    set views [DEVise getWinViews $win]
    if {$views != ""} {
	dialog .viewsInWindow "Views Still In Window" \
		"Window contains views. Remove all views first." \
		"" 0 OK
	return
    }
    
    DEVise destroy $win
}

############################################################

proc SetWindowLayout {win} {
    global curView layoutWin layoutOption verRequested horRequested

    if {$win == ""} {
	if {![CurrentView]} {
	    return
	}
	set win [DEVise getViewWin $curView]
    }

    if {[WindowVisible .setLayout]} {
	return
    }

    set layoutWin $win

    toplevel .setLayout
    wm title .setLayout "Set Window Layout"
    wm geometry .setLayout +50+50

    frame .setLayout.top
    frame .setLayout.bot
    pack .setLayout.top -side top -pady 3m -fill both -expand 1
    pack .setLayout.bot -side top -pady 5m -fill x

    frame .setLayout.row1
    frame .setLayout.row2
    frame .setLayout.row3
    frame .setLayout.row4
    pack .setLayout.row1 .setLayout.row2 .setLayout.row3 .setLayout.row4 \
	    -in .setLayout.top -side top -fill x

    frame .setLayout.bot.but
    pack .setLayout.bot.but -side top

    radiobutton .setLayout.row1.b -text Automatic -variable layoutOption \
	    -value auto -width 12 -anchor w -command {
	.setLayout.row2.e configure -state disabled
	.setLayout.row3.e configure -state disabled
    }
    pack .setLayout.row1.b -side left

    radiobutton .setLayout.row2.b -text Vertical \
	    -variable layoutOption -value fixver -width 12 -anchor w \
	    -command {
	.setLayout.row2.e configure -state normal
	.setLayout.row3.e configure -state disabled
    }
    label .setLayout.row2.l -text "Width:" -width 8
    entry .setLayout.row2.e -relief sunken -textvariable horRequested \
	    -width 5
    pack .setLayout.row2.b .setLayout.row2.l .setLayout.row2.e -side left

    radiobutton .setLayout.row3.b -text Horizontal \
	    -variable layoutOption -value fixhor -width 12 -anchor w \
	    -command {
	.setLayout.row3.e configure -state disabled
	.setLayout.row3.e configure -state normal
    }
    label .setLayout.row3.l -text "Height:" -width 8
    entry .setLayout.row3.e -relief sunken -textvariable verRequested \
	    -width 5
    pack .setLayout.row3.b .setLayout.row3.l .setLayout.row3.e -side left

    radiobutton .setLayout.row4.b -text Stacked -variable layoutOption \
	    -value stacked -width 12 -anchor w -command {
	.setLayout.row2.e configure -state disabled
	.setLayout.row3.e configure -state disabled
    }
    pack .setLayout.row4.b -side left

    set verRequested 1
    set horRequested 1
    set layoutOption auto
    .setLayout.row2.e configure -state disabled
    .setLayout.row3.e configure -state disabled

    set layout [DEVise getWindowLayout $layoutWin]
    if {[lindex $layout 0] >= 1} {
	set verRequested [lindex $layout 0]
    } elseif {[lindex $layout 1] >= 1} {
	set horRequested [lindex $layout 1]
    }
    if {[lindex $layout 2]} {
	set layoutOption stacked
    } elseif {[lindex $layout 0] >= 1} {
	set layoutOption fixhor
	.setLayout.row3.e configure -state normal
    } elseif {[lindex $layout 1] >= 1} {
	set layoutOption fixver
	.setLayout.row2.e configure -state normal
    }
	
    button .setLayout.bot.but.ok -text OK -width 10 -command {
	if {$layoutOption == "auto"} {
	    DEVise setWindowLayout $layoutWin -1 -1 0
	} elseif {$layoutOption == "fixver"} {
	    if {$horRequested < 1} { set horRequested 1 }
	    DEVise setWindowLayout $layoutWin -1 $horRequested 0
	} elseif {$layoutOption == "fixhor"} {
	    if {$verRequested < 1} { set verRequested 1 }
	    DEVise setWindowLayout $layoutWin $verRequested -1 0
	} else {
	    DEVise setWindowLayout $layoutWin -1 -1 1
	}
	destroy .setLayout
    }
    button .setLayout.bot.but.cancel -text Cancel -width 10 -command {
	destroy .setLayout
    }
    pack .setLayout.bot.but.ok .setLayout.bot.but.cancel -side left -padx 3m
}

############################################################

proc DupWindow {} {
    global curView

    if {![CurrentView]} {
	return
    }

    set win [DEVise getViewWin $curView]

    # Create new window
    set class [GetClass window $win]
    set params [DEVise getCreateParam window $class $win]
    set newWin [UniqueName $win]
    set newParam [linsert [lrange $params 1 end] 0 $newWin]
    eval DEVise create window $class $newParam

    set layout [DEVise getWindowLayout $win]
    eval DEVise setWindowLayout {$newWin} $layout

    set views [DEVise getWinViews $win]
    
    # Create mappings for views
    foreach view $views {
	# Crete a new view
	set class [GetClass view $view]
	set params [DEVise getCreateParam view $class $view]
	set viewName [lindex $params 0]
	set newView [UniqueName $viewName]

	# replace name of view by name of new view
	set newParam [linsert [lrange $params 1 end] 0 $newView]
	eval DEVise create view $class $newParam

	# insert links of $view into $newView
	foreach link [LinkSet] {
	    if {[DEVise viewInLink $link $view]} {
		DEVise insertLink $link $newView
	    }
	}

	set viewLabelParams [DEVise getLabel $view]
	eval DEVise setLabel {$newView} $viewLabelParams
	set viewStatParams [DEVise getViewStatistics $view]
	eval DEVise setViewStatistics {$newView} $viewStatParams
	set stat [DEVise getAxisDisplay $view X]
	eval DEVise setAxisDisplay {$newView} X $stat
	set stat [DEVise getAxisDisplay $view Y]
	eval DEVise setAxisDisplay {$newView} Y $stat
	
	DEVise insertWindow $newView $newWin

	set maps [DEVise getViewMappings $view]
	foreach map $maps {
	    DEVise insertMapping $newView $map
	}
    }
}

############################################################

proc DoSavePixmap {} {
    global curView

    if {![CurrentView]} {
	return
    }
    
    set file [DEVise open pixmap.tmp wb]
    DEVise savePixmap $curView $file
    DEVise close $file
}

############################################################

proc DoLoadPixmap {} {
    global curView

    if {![CurrentView]} {
	return
    }
    
    set file [DEVise open pixmap.tmp rb]
    DEVise loadPixmap $curView $file
    DEVise close $file
}

############################################################

# Create a new window
proc DoCreateWindow { message } {
    global dialogListVar

    set winTypes [ DEVise get window ]

    if { [llength $winTypes] == 1 } {
	return [ DoActualCreateWindow [lindex $winTypes 0] ]
    }

    set answer [ dialogList .createWin "Create Window"  \
	    $message "" "" { Cancel Ok } $winTypes ]
    if {$answer == 0 || $dialogListVar(selected) == ""} {
	return ""
    } else {
	return [ DoActualCreateWindow $dialogListVar(selected) ]
    }
}

############################################################

proc DoActualCreateWindow { winType } {
    global dialogParamVar windowName

    set windowName [UniqueName DEViseWn0]
    set paramNames [ DEVise getparam window $winType ]

    set button [ dialogParam .windowParam "Create Window" \
	    "Enter window parameters"\
	    "" 1 { Cancel OK } $paramNames ]

    if { $button == 0} {
	return
    }

    set cmd "DEVise create window $winType $dialogParamVar(params)"
    set result [eval $cmd]
    if {$result == ""} {
	set but [ dialog .winError "Window Error" \
		"Can't create window using $winType" "" 0 OK ]
	return
    }
    return $result
}

############################################################

# Get a window 
proc DoGetWindow {} {
    global dialogListVar

    set winSet [ WinSet ]

    if { [ llength $winSet ] == 0 } {
	return [ DoCreateWindow "Select window type" ]
    }

    set answer [ dialogList .window "Select Window" \
	    "Select a window to display view in" "" "" \
	    { Cancel New Ok } $winSet ]
    if { $answer == 1 } {
	return [ DoCreateWindow "Select window type" ]
    }

    if {$answer == 0  || $dialogListVar(selected) == ""} {
	return 
    }

    return $dialogListVar(selected)
}

############################################################

proc DoWindowMerge {} {
    global curView

    if {![CurrentView]} {
	return
    }

    # no need to merge if there are no views other than curView
    if {[llength [DEVise getWinViews [DEVise getViewWin $curView]]] < 2} {
	return
    }

    # first create a duplicate of current view

    set class [GetClass view $curView]
    set params [DEVise getCreateParam view $class $curView]
    set win [DEVise getViewWin $curView]

    set newView [UniqueName $curView]
    set newParams [linsert [lrange $params 1 end] 0 $newView]
    eval DEVise create view $class $newParams
    DEVise insertWindow $newView $win

    set labelParams [DEVise getLabel $curView]
    set newLabelParams [linsert [lrange $labelParams 0 1] 2 "Composite View"]
    eval DEVise setLabel {$newView} $newLabelParams
    set viewStatParams [DEVise getViewStatistics $curView]
    eval DEVise setViewStatistics {$newView} $viewStatParams
    set stat [DEVise getAxisDisplay $curView X]
    eval DEVise setAxisDisplay {$newView} X $stat
    set stat [DEVise getAxisDisplay $curView Y]
    eval DEVise setAxisDisplay {$newView} Y $stat
    
    # insert links of $curView into $newView
    foreach link [LinkSet] {
	if {[DEVise viewInLink $link $curView]} {
	    DEVise insertLink $link $newView
	}
    }

    ProcessViewSelected $newView

    # now move mappings from all other views to this view

    foreach v [DEVise getWinViews $win] {
	if {$v == $newView} {
	    continue
	}

	set viewLabelParams [DEVise getLabel $v]
	set label [lindex $viewLabelParams 2]

	foreach m [DEVise getViewMappings $v] {
	    set legend [DEVise getMappingLegend $v $m]
	    if {$legend == ""} {
		set legend $label
	    }
	    # insert mapping $m to view $newView, with legend $legend
	    DEVise insertMapping $newView $m $legend
	}

	# remove all links in view $v
	foreach link [LinkSet] {
	    if {[DEVise viewInLink $link $v]} {
		DEVise unlinkView $link $v
	    }
	}

	# remove all cursors in view $v
	foreach cursor [CursorSet] {
	    set views [DEVise getCursorViews $cursor]
	    if {[lindex $views 0] == $v} {
		# replace view $v with $newView as the the source
		# in cursor $cursor
		DEVise setCursorSrc $cursor $newView
	    }
	    if {[lindex $views 1] == $v} {
		# replace view $v with $newView as the the destination
		# in cursor $cursor
		DEVise setCursorDst $cursor $newView
	    }
	}

	# remove view $v from its window
	set ans [DEVise removeView $v]
	if {$ans != ""} {
	    dialogList .removeError "Cannot Remove View" $ans "" 0 OK
	}

	# destroy view $v
	DEVise destroy $v
    }

    DEVise refreshView $newView
}

############################################################

proc DoWindowSplit {} {
    global curView

    if {![CurrentView]} {
	return
    }

    set class [GetClass view $curView]
    set params [DEVise getCreateParam view $class $curView]
    set labelParams [DEVise getLabel $curView]
    set win [DEVise getViewWin $curView]

    # find out which links the current view is part of
    set viewLinks ""
    foreach link [LinkSet] {
	if {[DEVise viewInLink $link $curView]} {
	    lappend viewLinks $link
	}
    }

    set lastView ""

    foreach m [DEVise getViewMappings $curView] {
	set newView [UniqueName $curView]
	set newParams [linsert [lrange $params 1 end] 0 $newView]
	# create view $newView
	eval DEVise create view $class $newParams

	set lastView $newView

	set legend [DEVise getMappingLegend $curView $m]
	if {$legend == ""} {
	    set legend "New View"
	}

	set newLabelParams [linsert [lrange $labelParams 0 1] 2 $legend]
	eval DEVise setLabel {$newView} $newLabelParams
	set viewStatParams [DEVise getViewStatistics $curView]
	eval DEVise setViewStatistics {$newView} $viewStatParams
	set stat [DEVise getAxisDisplay $curView X]
	eval DEVise setAxisDisplay {$newView} X $stat
	set stat [DEVise getAxisDisplay $curView Y]
	eval DEVise setAxisDisplay {$newView} Y $stat
	
	# move mapping $m from view $curView to $newView
	DEVise insertMapping $newView $m
	DEVise removeMapping $curView $m

	# insert links of $curView into $newView
	foreach l $viewLinks {
	    DEVise insertLink $l $newView
	}

	# insert view $newView to window $win
	DEVise insertWindow $newView $win
	DEVise refreshView $newView
    }

    if {$lastView != ""} {
	set v $curView
	ProcessViewSelected $lastView
	# remove all links in view $v
	foreach link [LinkSet] {
	    if {[DEVise viewInLink $link $v]} {
		DEVise unlinkView $link $v
	    }
	}
	set ans [DEVise removeView $v]
	if {$ans != ""} {
	    dialogList .removeError "Cannot Remove View" $ans "" 0 OK
	}
	DEVise destroy $v
    }
}

############################################################

proc DoWindowStackControl {} {
    global curView stackWinOpened

    if {[WindowVisible .stack]} {
	return
    }

    toplevel    .stack
    wm title    .stack "Stack Control"
    wm geometry .stack +100+100

    # build view name widget

    frame .stack.title
    pack .stack.title -side top -pady 1m -expand 1 -fill x

    label .stack.title.text -text "No View Selected"
    pack .stack.title.text -side top -pady 1m

    # build button bar widget

    frame .stack.bot
    frame .stack.bot.row1
    frame .stack.bot.row2
    frame .stack.bot.row1.but
    frame .stack.bot.row2.but

    button .stack.bot.row1.but.merge -text "Merge" -width 10 \
	    -command { DoWindowMerge }
    button .stack.bot.row1.but.split -text "Split" -width 10 \
	    -command { DoWindowSplit }
    button .stack.bot.row1.but.stack -text "Stack" -width 10 \
	    -command { DoWindowStack }
    button .stack.bot.row1.but.unstack -text "Unstack" -width 10 \
	    -command { DoWindowUnstack }

    pack .stack.bot.row1.but.merge .stack.bot.row1.but.split \
	    .stack.bot.row1.but.stack .stack.bot.row1.but.unstack \
	    -side left -expand 1 -fill x -padx 3m

    button .stack.bot.row2.but.rotate -text "Rotate" -width 10 \
	    -command { RotateMergedView }
    button .stack.bot.row2.but.flip -text "Flip" -width 10 \
	    -command { FlipStackedView }
    button .stack.bot.row2.but.edit -text "Edit..." -width 10 \
	    -command { EditMergedView }
    button .stack.bot.row2.but.close -text Close -width 10 \
	    -command "global stackyWinOpened; \
	              set stackWinOpened false; \
	              destroy .stack"

    pack .stack.bot.row2.but.rotate .stack.bot.row2.but.flip \
	    .stack.bot.row2.but.edit .stack.bot.row2.but.close \
	    -side left -expand 1 -fill x -padx 3m

    pack .stack.bot.row1.but -side top
    pack .stack.bot.row2.but -side top
    pack .stack.bot.row1 .stack.bot.row2 -side top -pady 1m
    pack .stack.bot -side top -pady 5m

    set stackWinOpened true

    if {$curView != ""} {
	.stack.title.text configure -text "View: $curView"
    }
}

############################################################

proc RotateMergedView {} {
    global curView

    if {![CurrentView]} {
	return
    }

    # no change needed if only one mapping in view
    set mappings [DEVise getViewMappings $curView]
    if {[llength $mappings] < 2} {
	return
    }

    # move first mapping to end of mapping list in view
    set map [lindex $mappings 0]
    set legend [DEVise getMappingLegend $curView $map]
    if {$legend == ""} {
	set legend "X"
    }
    DEVise removeMapping $curView $map
    DEVise insertMapping $curView $map $legend
    
    DEVise refreshView $curView
}

############################################################

proc EditMergedView {} {
}

############################################################

proc FlipStackedView {} {
    global curView

    if {![CurrentView]} {
	return
    }

    set win [DEVise getViewWin $curView]
    set views [DEVise getWinViews $win]

    set prevView ""
    foreach view $views {
	if {$prevView == ""} {
	    set prevView $view
	    continue
	}
	DEVise swapView $win $prevView $view
    }

    set views [DEVise getWinViews $win]
    DEVise raiseView [lindex $views 0]
}

############################################################

proc DoWindowStack {} {
    global curView

    if {![CurrentView]} {
	return
    }

    set win [DEVise getViewWin $curView]
    DEVise setWindowLayout $win -1 -1 1
}

############################################################

proc DoWindowUnstack {} {
    global curView

    if {![CurrentView]} {
	return
    }

    set win [DEVise getViewWin $curView]
    set layout [DEVise getWindowLayout $win]
    set newLayout [list [lindex $layout 0] [lindex $layout 1] 0]
    eval DEVise setWindowLayout {$win} $newLayout
}

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
#  Revision 1.25  1997/01/27 20:15:25  wenger
#  Workaround to bug 137: disables Stack Control dialog buttons while drawing.
#
#  Revision 1.24  1997/01/23 21:47:45  wenger
#  Fixed a couple more OK/Cancel button pairs.
#
#  Revision 1.23  1997/01/23 19:28:27  jussi
#  Removed debugging messages. Fixed bug #126.
#
#  Revision 1.22  1997/01/23 16:02:24  jussi
#  Removed debugging message.
#
#  Revision 1.21  1997/01/23 16:02:12  jussi
#  Added code that raises the top view in a stack when stack
#  order is rotated.
#
#  Revision 1.20  1997/01/23 00:08:12  jussi
#  Removed references to stackWinOpened. Removed Rotate button.
#
#  Revision 1.19  1997/01/22 20:14:00  wenger
#  Removed other non-functional user interface components (includes workaround
#  for bug 127); fixed a number of OK/Cancel button positions.
#
#  Revision 1.18  1997/01/22 14:40:34  wenger
#  Added devise.etk to release scripts; removed merged view buttons from
#  Stack Control dialog.
#
#  Revision 1.17  1997/01/17 20:32:32  wenger
#  Fixed bugs 088, 121, 122; put workaround in place for bug 123; added
#  simulation of XOR drawing in PSWindowRep; removed diagnostic output
#  from Tcl/Tk code; removed (at least for now) the ETk interface from
#  the cslib versions of WindowRep classes so that the cslib will link
#  okay; cslib server now tests XOR drawing.
#
#  Revision 1.16  1997/01/08 18:56:51  wenger
#  Added 'View List' selection to window menu (gives a list of the views
#  in the window containing the selected view, especially useful for
#  piled views); fixed OK/Cancel in Remove Window dialog.
#
#  Revision 1.15  1996/11/26 17:25:22  ssl
#  added support for stacked views
#
#  Revision 1.14  1996/08/03 15:51:52  jussi
#  3D view parameters are preserved when a merged view is split.
#
#  Revision 1.13  1996/08/03 15:34:39  jussi
#  When views are merged, the 3D parameters of the currently selected
#  view will determine the 3D parameters of the composite view.
#
#  Revision 1.12  1996/07/23 17:27:18  jussi
#  Added support for piled views.
#
#  Revision 1.11  1996/07/21 14:59:45  jussi
#  Code no longer inserts an "X" when rotating legend-less mappings
#  in a merged view.
#
#  Revision 1.10  1996/07/08 17:08:38  jussi
#  The mapping legend (label) is now copied in DupWindow.
#
#  Revision 1.9  1996/07/01 19:32:28  jussi
#  The 3D camera location is now duplicated in Window/Duplicate.
#
#  Revision 1.8  1996/05/31 19:14:30  jussi
#  Fixed calls to DEVise removeView; there is no return value from
#  the call.
#
#  Revision 1.7  1996/05/11 03:01:13  jussi
#  Minor fix in DoActualCreateWindow.
#
#  Revision 1.6  1996/05/09 18:15:21  kmurli
#  No changes, just inserted some debugging print statements and removed them
#
#  Revision 1.5  1996/05/07 16:48:12  jussi
#  Rotation of mappings not done if there is just one mappings
#  in a view.
#
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
		{ OK Cancel } $winSet ]
	if {$answer == 1  || $dialogListVar(selected) == ""} {
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

        # set view statistics and axes
	set viewLabelParams [DEVise getLabel $view]
	eval DEVise setLabel {$newView} $viewLabelParams
	set viewStatParams [DEVise getViewStatistics $view]
	eval DEVise setViewStatistics {$newView} $viewStatParams
	set stat [DEVise getAxisDisplay $view X]
	eval DEVise setAxisDisplay {$newView} X $stat
	set stat [DEVise getAxisDisplay $view Y]
	eval DEVise setAxisDisplay {$newView} Y $stat
	
        # set 2D/3D and camera location
        set numDim [DEVise getViewDimensions $view]
        eval DEVise setViewDimensions {$newView} $numDim
	set camera [DEVise get3DLocation $view]
        eval DEVise set3DLocation {$newView} [lrange $camera 1 6]

	DEVise insertWindow $newView $newWin

	set maps [DEVise getViewMappings $view]
	foreach map $maps {
            set legend [DEVise getMappingLegend $view $map]
	    DEVise insertMapping $newView $map $legend
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
	    $message "" "" { OK Cancel } $winTypes ]
    if {$answer == 1 || $dialogListVar(selected) == ""} {
	return ""
    } else {
	return [ DoActualCreateWindow $dialogListVar(selected) ]
    }
}

############################################################

proc DoActualCreateWindow { winType } {
    global dialogParamVar windowName

    set paramNames [ DEVise getparam window $winType ]
    set windowName [UniqueName DEViseWn0]
    set nameParm [lindex $paramNames 0]
    set nameParm [lreplace $nameParm 1 1 $windowName]
    set paramNames [lreplace $paramNames 0 0 $nameParm]

    set button [ dialogParam .windowParam "Create Window" \
	    "Enter window parameters"\
	    "" 0 { OK Cancel } $paramNames ]

    if { $button == 1} {
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
	    { OK New Cancel } $winSet ]
    if { $answer == 1 } {
	return [ DoCreateWindow "Select window type" ]
    }

    if {$answer == 2  || $dialogListVar(selected) == ""} {
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

    set viewDimensions [DEVise getViewDimensions $curView]
    eval DEVise setViewDimensions {$newView} $viewDimensions
    set viewSolid3D [DEVise getViewSolid3D $curView]
    eval DEVise setViewSolid3D {$newView} $viewSolid3D
    set viewXYZoom [DEVise getViewXYZoom $curView]
    eval DEVise setViewXYZoom {$newView} $viewXYZoom
    set camera [DEVise get3DLocation $curView]
    set x [lindex $camera 1]
    set y [lindex $camera 2]
    set z [lindex $camera 3]
    set fx [lindex $camera 4]
    set fy [lindex $camera 5]
    set fz [lindex $camera 6]
    eval DEVise set3DLocation {$newView} $x $y $z $fx $fy $fz
    
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

	# remove view $v from its window and then destroy the view
	DEVise removeView $v
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
	
        set viewDimensions [DEVise getViewDimensions $curView]
        eval DEVise setViewDimensions {$newView} $viewDimensions
        set viewSolid3D [DEVise getViewSolid3D $curView]
        eval DEVise setViewSolid3D {$newView} $viewSolid3D
        set viewXYZoom [DEVise getViewXYZoom $curView]
        eval DEVise setViewXYZoom {$newView} $viewXYZoom
        set camera [DEVise get3DLocation $curView]
        set x [lindex $camera 1]
        set y [lindex $camera 2]
        set z [lindex $camera 3]
        set fx [lindex $camera 4]
        set fy [lindex $camera 5]
        set fz [lindex $camera 6]
        eval DEVise set3DLocation {$newView} $x $y $z $fx $fy $fz
    
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
	# remove view $v from its window and then destroy the view
	DEVise removeView $v
	DEVise destroy $v
    }
}

############################################################

proc DoWindowStackControl {} {
    global curView

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

    button .stack.bot.row1.but.pile -text "Pile" -width 10 \
	    -command { DoWindowPile }
    button .stack.bot.row2.but.unpile -text "Unpile" -width 10 \
	    -command { DoWindowUnpile }
    button .stack.bot.row1.but.stack -text "Stack" -width 10 \
	    -command { DoWindowStack }
    button .stack.bot.row2.but.unstack -text "Unstack" -width 10 \
	    -command { DoWindowUnstack }
    button .stack.bot.row1.but.flip -text "Flip" -width 10 \
	    -command { FlipStackedView }

#    button .stack.bot.row1.but.edit -text "Edit..." -width 10 \
#	    -command { EditMergedView }

    button .stack.bot.row2.but.close -text Close -width 10 \
	    -command { destroy .stack }

    pack .stack.bot.row1.but.pile \
            .stack.bot.row1.but.stack \
            .stack.bot.row1.but.flip \
	    -side left -padx 3m
#    pack .stack.bot.row1.but.pile \
#            .stack.bot.row1.but.stack \
#            .stack.bot.row1.but.edit \
#	    -side left -expand 1 -fill x -padx 3m

    pack .stack.bot.row2.but.unpile \
	    .stack.bot.row2.but.unstack \
            .stack.bot.row2.but.close \
	    -side left -expand 1 -fill x -padx 3m

    pack .stack.bot.row1.but -side top
    pack .stack.bot.row2.but -side top
    pack .stack.bot.row1 .stack.bot.row2 -side top -pady 1m
    pack .stack.bot -side top -pady 5m

    if {$curView != ""} {
	.stack.title.text configure -text "View: $curView"
    }
}

############################################################

proc EnableStackControl {} {
    if {![WindowExists .stack]} {
	return
    }

    .stack.bot.row1.but.pile config -state normal
    .stack.bot.row2.but.unpile config -state normal
    .stack.bot.row1.but.stack config -state normal
    .stack.bot.row2.but.unstack config -state normal
    .stack.bot.row1.but.flip config -state normal
}

############################################################

proc DisableStackControl {} {
    if {![WindowExists .stack]} {
	return
    }

    .stack.bot.row1.but.pile config -state disabled
    .stack.bot.row2.but.unpile config -state disabled
    .stack.bot.row1.but.stack config -state disabled
    .stack.bot.row2.but.unstack config -state disabled
    .stack.bot.row1.but.flip config -state disabled
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
    DEVise removeMapping $curView $map
    if {$legend != ""} {
        DEVise insertMapping $curView $map $legend
    } else {
        DEVise insertMapping $curView $map
    }
    
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

    # if window is stacked, raise top view

    set layout [DEVise getWindowLayout $win]
    if {[lindex $layout 2]} {
        DEVise raiseView [lindex [DEVise getWinViews $win] 0]
    }    
}

############################################################

proc DoWindowPile {} {
    global curView

    if {![CurrentView]} {
	return
    }

    # no need to pile if there are no views other than curView
    if {[llength [DEVise getWinViews [DEVise getViewWin $curView]]] < 2} {
	return
    }

    # no need to pile if current view already in pile mode
    if {[DEVise getViewPileMode $curView]} {
        return
    }

    # create new XY link for piled views
    set link [UniqueName "Pile: Link"]
    set result [DEVise create link Visual_Link $link 3]
    if {$result == ""} {
        dialog .linkError "Link Error" \
                "Error creating link $link" "" 0 OK
        return
    }

    # insert pile link into current view first so that its filter
    # will progagate to other views in pile
    DEVise insertLink $link $curView

    # set pileMode flag 'on' in all views in current window and
    # insert link to all views

    set win [DEVise getViewWin $curView]
    foreach v [DEVise getWinViews $win] {
        if {$v != $curView} {
            DEVise insertLink $link $v
	    DEVise refreshView $v
        }
        DEVise setViewPileMode $v 1
    }

    # set window to stacked mode
    DEVise setWindowLayout $win -1 -1 1
}

############################################################

proc DoWindowUnpile {} {
    global curView

    if {![CurrentView]} {
	return
    }

    # no need to unpile if there are no views other than curView
    if {[llength [DEVise getWinViews [DEVise getViewWin $curView]]] < 2} {
	return
    }

    # no need to unpile if current view not in pile mode
    if {![DEVise getViewPileMode $curView]} {
        return
    }

    # find name(s) of pile links in current view
    set pileLinks ""
    foreach link [LinkSet] {
	if {[DEVise viewInLink $link $curView]} {
            if {[string range $link 0 4] == "Pile:"} {
                lappend pileLinks $link
            }
	}
    }

    # set pileMode flag 'off' in all views in current window and
    # remove pile links from all views

    set win [DEVise getViewWin $curView]
    foreach v [DEVise getWinViews $win] {
        DEVise setViewPileMode $v 0
        foreach link $pileLinks {
	    if {[DEVise viewInLink $link $v]} {
		DEVise unlinkView $link $v
	    }
        }
    }

    # restore window layout
    set layout [DEVise getWindowLayout $win]
    set newLayout [list [lindex $layout 0] [lindex $layout 1] 0]
    eval DEVise setWindowLayout {$win} $newLayout
    DEVise refreshView $curView
}

############################################################

proc DoWindowStack {} {
    global curView

    if {![CurrentView]} {
	return
    }

    set win [DEVise getViewWin $curView]
    DEVise setWindowLayout $win -1 -1 1
    DEVise refreshView $curView
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
    foreach v [DEVise getWinViews $win] {
        DEVise refreshView $v
    }
}

############################################################

# Show a list of the views contained in the window containing the currently-
# selected view.

proc WinViewList {} {
    global curView

    if {![CurrentView]} {
	return
    }

    set win [DEVise getViewWin $curView]

    set views [DEVise getWinViews $win]
    if {$views != ""} {
	dialogList .winViewList "View List" "Window $win Views" "" 0 {OK} $views
    }
}

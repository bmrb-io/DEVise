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
#  Revision 1.2  1996/06/15 14:43:16  jussi
#  Added yuc's 3D procedures.
#
#  Revision 1.1  1996/04/11 18:13:34  jussi
#  Initial revision.
#

############################################################

set queryColors "red blue green white black yellow orange brown"

proc SetQuery {} { 
    global curView queryWinOpened queryColors

    set expand 1
    set fill both

    if {[WindowVisible .query]} {
	return
    }

    toplevel    .query
    wm title    .query "Query Tool"
    wm geometry .query +200+200

    # build view name widget

    frame .query.title
    pack .query.title -side top -pady 1m -expand $expand -fill $fill

    label .query.title.text -text "No View Selected"
    pack .query.title.text -side top -pady 1m

    # build xyRange widgets

    frame .query.xyRange
    pack .query.xyRange -side top -pady 1m -expand $expand -fill $fill

    frame .query.xyRange.top
    frame .query.xyRange.bottom 

    frame .query.xyRange.top.yEntry
    frame .query.xyRange.top.yDummy -width 280 -relief groove -bd 3
    frame .query.xyRange.top.yEntry.yhigh
    frame .query.xyRange.top.yEntry.ylow

    frame .query.xyRange.bottom.xDummy -width 280
    frame .query.xyRange.bottom.xEntry
    frame .query.xyRange.bottom.xEntry.xlow
    frame .query.xyRange.bottom.xEntry.xhigh

    # set up Y labels and entries

    label .query.xyRange.top.yEntry.yhigh.yHighHeading -text "Y high" \
	    -width 6 -font 8x13
    entry .query.yhigh -text "" -relief sunken -width 20 \
	    -fg black -bg LightGray -font 8x13

    pack .query.xyRange.top.yEntry.yhigh.yHighHeading .query.yhigh \
	    -in .query.xyRange.top.yEntry.yhigh \
	    -side left -expand $expand -fill $fill

    label .query.xyRange.top.yEntry.ylow.yLowHeading -text "Y low" \
	    -width 6 -font 8x13
    entry .query.ylow -text "" -relief sunken -width 20 \
	    -fg black -bg white -font 8x13

    pack .query.xyRange.top.yEntry.ylow.yLowHeading .query.ylow \
	    -in .query.xyRange.top.yEntry.ylow \
	    -side left -expand $expand -fill $fill

    pack .query.xyRange.top.yEntry.yhigh \
	    .query.xyRange.top.yEntry.ylow \
	    -side top -expand $expand -fill $fill

    pack .query.xyRange.top.yEntry \
	    .query.xyRange.top.yDummy \
	    -side left -expand $expand -fill $fill

    # set up X labels and entries

    entry .query.xlow -text "" -relief sunken -width 20 \
	    -fg black -bg white -font 8x13
    label .query.xyRange.bottom.xEntry.xlow.xLowHeading -text "X low"

    pack .query.xlow .query.xyRange.bottom.xEntry.xlow.xLowHeading \
	    -in .query.xyRange.bottom.xEntry.xlow \
	    -side top -expand $expand -fill $fill

    entry .query.xhigh -text "" -relief sunken -width 20 \
	    -fg black -bg LightGray -font 8x13
    label .query.xyRange.bottom.xEntry.xhigh.xHighHeading -text "X high"

    pack .query.xhigh .query.xyRange.bottom.xEntry.xhigh.xHighHeading \
	    -in .query.xyRange.bottom.xEntry.xhigh \
	    -side top -expand $expand -fill $fill

    pack .query.xyRange.bottom.xEntry.xlow \
	    .query.xyRange.bottom.xEntry.xhigh \
	    -side left -expand $expand -fill $fill

    pack .query.xyRange.bottom.xDummy \
	    .query.xyRange.bottom.xEntry \
	    -side left -expand $expand -fill $fill

    pack .query.xyRange.top .query.xyRange.bottom -side top \
	    -expand $expand -fill $fill

#    button .query.xyRange.top.yDummy.stat -text "Toggle Statistics" \
#	    -command DoStat
#    pack .query.xyRange.top.yDummy.stat -side left -expand $expand

    # set up selection area

    frame .query.sel
    pack .query.sel -in .query.xyRange.bottom.xDummy \
	    -side top -pady 1m -expand $expand -fill $fill

    button .query.sel.attr -text "Attributes..." \
	    -command DoAttributeSelect
    button .query.sel.color -text "Colors..." \
	    -command DoColorSelect

    pack .query.sel.attr .query.sel.color -side left -padx 3m

    if {0} {
	frame .query.sel.sample -relief groove -bd 2
	pack .query.sel.sample -side left -padx 3m \
		-expand $expand -fill $fill

	# set up color selection sample

	set i 0
	foreach color $queryColors {
	    frame .query.sel.sample.color$i -bg $color
	    pack .query.sel.sample.color$i -side left -expand 1 -fill both
	    incr i
	}
    }

    # set up buttons

    frame .query.bot
    frame .query.bot.but

    button .query.bot.but.exec -text "Execute" -width 10 \
	    -command { ExecuteQuery }
    button .query.bot.but.undo -text "Undo Edit" -width 10 \
	    -command { DoUndoEdit }
    button .query.bot.but.back -text "Back One" -width 10 \
	    -command { DoGoBackOne; ExecuteQuery }
    button .query.bot.but.history -text "History" -width 10 \
	    -command { DoHistoryToggle }
    button .query.bot.but.close -text Close -width 10 \
	    -command { global queryWinOpened; \
	               set queryWinOpened 0; \
	               destroy .query }

    pack .query.bot.but.exec .query.bot.but.undo .query.bot.but.back \
	    .query.bot.but.history .query.bot.but.close \
	    -side left -expand 1 -fill x -padx 3m

    pack .query.bot.but -side top
    pack .query.bot -side top -pady 5m

    set queryWinOpened true

    if {$curView != ""} {
	.query.title.text configure -text "View: $curView"
	DoUndoEdit
    }
}

############################################################

proc ExecuteQuery {} {
    global curView

    if {$curView == ""} {
	return
    }

    set xlow [DEVise parseDateFloat [.query.xlow get]]
    set ylow [DEVise parseDateFloat [.query.ylow get]]
    set xhigh [DEVise parseDateFloat [.query.xhigh get]]
    set yhigh [DEVise parseDateFloat [.query.yhigh get]]

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

proc DoUndoEdit {} {
    global curView
    
    if { $curView == "" } {
	return
    }
    
    set filters [DEVise getVisualFilters $curView] 
    set filter [lindex $filters [expr [llength $filters]-1]]
    
    foreach i { xlow ylow xhigh yhigh} {
	.query.$i delete 0 end
    }
    .query.xlow insert 0 [lindex $filter 0]
    .query.ylow insert 0 [lindex $filter 1]
    .query.xhigh insert 0 [lindex $filter 2]
    .query.yhigh insert 0 [lindex $filter 3]
}

############################################################

proc DoGoBackOne {} {
    global curView
    
    if { $curView == "" } {
	return
    }
    
    set filters [DEVise getVisualFilters $curView] 
    set len [llength $filters]
    if { $len <= 1 } {
	return
    }
    set filter [lindex $filters [expr $len-2]]
    
    foreach i {xlow ylow xhigh yhigh} {
	.query.$i delete 0 end
    }
    .query.xlow insert 0 [lindex $filter 0]
    .query.ylow insert 0 [lindex $filter 1]
    .query.xhigh insert 0 [lindex $filter 2]
    .query.yhigh insert 0 [lindex $filter 3]
}

############################################################

proc DoAttributeSelect {} {
}

############################################################

proc DoColorSelect {} {
}

############################################################

proc AboutDevise {} {
    if {[WindowVisible .about]} {
	return
    }

    toplevel    .about
    wm title    .about "About DEVise"
    wm geometry .about +100+100

    message .about.msg -justify center -width 10c \
	    -text "Message about DEVise."
    button .about.ok -text OK -width 10 -command "destroy .about"
    pack .about.msg .about.ok -side top -padx 5m -pady 3m
}

proc Do3DQuery {} { 
    global curView query3DWinOpened

    if {[WindowVisible .query3d]} {
	return
    }

    toplevel    .query3d
    wm title    .query3d "Query Tool"
    wm geometry .query3d +200+200

    # build view name widget

    frame .query3d.title
    pack .query3d.title -side top -pady 1m -expand 1 -fill both

    label .query3d.title.text -text "No View Selected"
    pack .query3d.title.text -side top -pady 1m

    # build xyzLoc widgets

    frame .query3d.xyzLoc
    pack .query3d.xyzLoc -side top -pady 1m -padx 3m -expand 1 -fill both

    frame .query3d.xyzLoc.xEntry
    frame .query3d.xyzLoc.yEntry
    frame .query3d.xyzLoc.zEntry
    pack .query3d.xyzLoc.xEntry .query3d.xyzLoc.yEntry .query3d.xyzLoc.zEntry \
	    -side top -pady 2m -padx 2m

    # set up X label and entry

    label .query3d.xyzLoc.xEntry.heading -text "X Coordinate"
    entry .query3d.xyzLoc.xEntry.x -text "" -relief sunken -width 20
    pack .query3d.xyzLoc.xEntry.heading .query3d.xyzLoc.xEntry.x \
	    -side left -expand 1 -fill both

    # set up Y label and entry

    label .query3d.xyzLoc.yEntry.heading -text "Y Coordinate"
    entry .query3d.xyzLoc.yEntry.y -text "" -relief sunken -width 20
    pack .query3d.xyzLoc.yEntry.heading .query3d.xyzLoc.yEntry.y \
	    -side left -expand 1 -fill both

    # set up Z label and entry

    label .query3d.xyzLoc.zEntry.heading -text "Z Coordinate"
    entry .query3d.xyzLoc.zEntry.z -text "" -relief sunken -width 20
    pack .query3d.xyzLoc.zEntry.heading .query3d.xyzLoc.zEntry.z \
	    -side left -expand 1 -fill both

    # set up buttons

    frame .query3d.bot
    frame .query3d.bot.but

    button .query3d.bot.but.exec -text "Execute" -width 10 \
	    -command {
	if {$curView == ""} {
	    return
	}
	set x [.query3d.xyzLoc.xEntry.x get]
	set y [.query3d.xyzLoc.yEntry.y get]
	set z [.query3d.xyzLoc.zEntry.z get]
	puts "DEVise set3DLocation $curView $x $y $z"
	DEVise set3DLocation $curView $x $y $z
    }
    button .query3d.bot.but.close -text Close -width 10 \
	    -command "global query3DWinOpened; \
	              set query3DWinOpened 0; \
	              destroy .query3d"

    pack .query3d.bot.but.exec .query3d.bot.but.close \
	    -side left -expand 1 -fill x -padx 3m

    pack .query3d.bot.but -side top
    pack .query3d.bot -side top -pady 5m

    set query3DWinOpened 1

    if {$curView != ""} {
	Update3DLocation
    }
}

proc Update3DLocation {} {
    global curView query3DWinOpened

    if {!$query3DWinOpened} {
	return
    }

    .query3d.xyzLoc.xEntry.x delete 0 end
    .query3d.xyzLoc.yEntry.y delete 0 end
    .query3d.xyzLoc.zEntry.z delete 0 end
    .query3d.title.text configure -text "No View Selected"

    if {$curView == ""} {
	return
    }

    set xyzloc [DEVise get3DLocation $curView]
    .query3d.xyzLoc.xEntry.x insert 0 [lindex $xyzloc 0]
    .query3d.xyzLoc.yEntry.y insert 0 [lindex $xyzloc 1]
    .query3d.xyzLoc.zEntry.z insert 0 [lindex $xyzloc 2]
    .query3d.title.text configure -text "View: $curView"
}

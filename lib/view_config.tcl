# ========================================================================
# DEVise Data Visualization Software
# (c) Copyright 1992-1999
# By the DEVise Development Group
# Madison, Wisconsin
# All Rights Reserved.
# ========================================================================

# Under no circumstances is this software to be copied, distributed,
# or altered in any way without prior permission from the DEVise
# Development Group.

############################################################

# This module contains the GUI components related to view configuration
# (setting the home and panning parameters).

############################################################

# $Id$

# $Log$
# Revision 1.5  1999/03/16 17:10:26  wenger
# Improved 'view home' configuration: user can select whether home changes
# X, Y, or both parts of visual filter; added explicit option to force Y
# min to 0 in automatic mode; fixed bug 469 (problems with 'home' in
# record link follower views).
#
# Revision 1.4  1997/06/09 14:47:14  wenger
# Added cursor grid; fixed bug 187; a few minor cleanups.
#
# Revision 1.3  1997/06/05 21:08:56  wenger
# User-configurability of '4', '5', and '6' keys is now completed.
#
# Revision 1.2  1997/06/05 18:11:40  wenger
# Pretty much working now.  I need to clean up diagnostic code and get
# the blanking out of sections according to mode working.
#
# Revision 1.1  1997/05/30 15:41:38  wenger
# Most of the way to user-configurable '4', '5', and '6' keys -- committing
# this stuff now so it doesn't get mixed up with special stuff for printing
# Mitre demo.
#

############################################################

proc GetViewHome {} {
    global curView

    # If this window already exists, raise it to the top and return.
    if {[WindowVisible .getViewHome]} {
	return
    }

    # Create the top level widget and the frames we'll later use for
    # positioning.
    toplevel .getViewHome
    wm title .getViewHome "Set View Home"

    frame .getViewHome.row0
    frame .getViewHome.row1
    frame .getViewHome.row2
    frame .getViewHome.row3
    frame .getViewHome.row4
    frame .getViewHome.row5
    frame .getViewHome.row6
    frame .getViewHome.row7
    frame .getViewHome.row8

    # Create the various widgets.
    button .getViewHome.ok -text "OK" -width 10 \
      -command {SetViewHome; destroy .getViewHome}
    button .getViewHome.cancel -text "Cancel" -width 10 \
      -command "destroy .getViewHome"

    global viewHomeDoX viewHomeDoY
    checkbutton .getViewHome.doXHome -text {Home on X} -variable viewHomeDoX
    checkbutton .getViewHome.doYHome -text {Home on Y} -variable viewHomeDoY

    # See ViewGraph.h for mode values...
    global viewHomeMode
    radiobutton .getViewHome.auto -text "Automatic" -variable viewHomeMode \
      -value 1 -command "SwitchHomeMode"
    radiobutton .getViewHome.man -text "Fixed" -variable viewHomeMode \
      -value 2 -command "SwitchHomeMode"

    global viewHomeYZero
    checkbutton .getViewHome.doYZero -text {Y Min Zero} -variable viewHomeYZero

    global viewHomeXMar viewHomeYMar
    label .getViewHome.xMarLab -text "X Margin:"
    entry .getViewHome.xMarEnt -width 20 -relief sunken \
      -textvariable viewHomeXMar
    label .getViewHome.yMarLab -text "Y Margin:"
    entry .getViewHome.yMarEnt -width 20 -relief sunken \
      -textvariable viewHomeYMar

    global viewHomeXLow viewHomeXHigh
    label .getViewHome.xLowLab -text "X Low:"
    entry .getViewHome.xLowEnt -width 20 -relief sunken \
      -textvariable viewHomeXLow
    label .getViewHome.xHighLab -text "X High:"
    entry .getViewHome.xHighEnt -width 20 -relief sunken \
      -textvariable viewHomeXHigh

    global viewHomeYLow viewHomeYHigh
    label .getViewHome.yLowLab -text "Y Low:"
    entry .getViewHome.yLowEnt -width 20 -relief sunken \
      -textvariable viewHomeYLow
    label .getViewHome.yHighLab -text "Y High:"
    entry .getViewHome.yHighEnt -width 20 -relief sunken \
      -textvariable viewHomeYHigh

    button .getViewHome.setCurrent -text "Set to Current Query" \
      -command "SetHomeToCurrent"

    # These frames are for spacing only.
    frame .getViewHome.row0a -width 10m -height 6m
    frame .getViewHome.row2a -width 130m -height 6m
    frame .getViewHome.row4a -width 10m -height 6m
    frame .getViewHome.row7a -width 10m -height 4m

    # Pack some widgets into the frames.  Other widges will be packed in
    # the SwitchHomeMode procedure.
    pack .getViewHome.row0 -side top
    pack .getViewHome.row0a -side top
    pack .getViewHome.row1 -side bottom -pady 4m
    pack .getViewHome.row2 -side top
    pack .getViewHome.row2a -side top

    pack .getViewHome.ok .getViewHome.cancel -in .getViewHome.row1 \
      -side left -padx 3m
    pack .getViewHome.doXHome .getViewHome.doYHome -in .getViewHome.row0 \
      -side left -pady 1m
    pack .getViewHome.auto .getViewHome.man -in .getViewHome.row2 \
      -side left -pady 1m
    pack .getViewHome.doYZero -in .getViewHome.row8 -side left
    pack .getViewHome.xMarLab .getViewHome.xMarEnt -in .getViewHome.row3 \
      -side left
    pack .getViewHome.yMarLab .getViewHome.yMarEnt -in .getViewHome.row4 \
      -side left
    pack .getViewHome.xLowLab .getViewHome.xLowEnt .getViewHome.xHighLab \
      .getViewHome.xHighEnt -in .getViewHome.row5 -side left
    pack .getViewHome.yLowLab .getViewHome.yLowEnt .getViewHome.yHighLab \
      .getViewHome.yHighEnt -in .getViewHome.row6 -side left
    pack .getViewHome.setCurrent -in .getViewHome.row7 -side left

    # Get the current values from the view and set the GUI accordingly.
    # Returns: <mode> <autoXMargin> <autoYMargin> <manXLo> <manYLo>
    # <manXHi> <manYHi>
    set home [DEVise viewGetHome $curView]
    set viewHomeDoX [lindex $home 0]
    set viewHomeDoY [lindex $home 1]
    set viewHomeMode [lindex $home 2]
    set viewHomeYZero [lindex $home 3]
    set viewHomeXMar [lindex $home 4]
    set viewHomeYMar [lindex $home 5]
    set viewHomeXLow [lindex $home 6]
    set viewHomeYLow [lindex $home 7]
    set viewHomeXHigh [lindex $home 8]
    set viewHomeYHigh [lindex $home 9]
    SwitchHomeMode

    # Wait for the user to make a selection from this window.
    tkwait visibility .getViewHome
    grab set .getViewHome
    tkwait window .getViewHome
}

##########################################################################

proc SwitchHomeMode {} {
  global viewHomeMode

  if {$viewHomeMode == 1} {
    pack .getViewHome.row8 -after .getViewHome.row2a -pady 1m
    pack .getViewHome.row3 -after .getViewHome.row8 -pady 1m
    pack .getViewHome.row4 -after .getViewHome.row3 -pady 1m
    pack .getViewHome.row4a -after .getViewHome.row4 -pady 1m

    pack forget .getViewHome.row5
    pack forget .getViewHome.row6
    pack forget .getViewHome.row7
    pack forget .getViewHome.row7a
  } else {
    pack forget .getViewHome.row8
    pack forget .getViewHome.row3
    pack forget .getViewHome.row4
    pack forget .getViewHome.row4a

    pack .getViewHome.row5 -after .getViewHome.row2a -pady 1m
    pack .getViewHome.row6 -after .getViewHome.row5 -pady 1m
    pack .getViewHome.row7 -after .getViewHome.row6 -pady 1m
    pack .getViewHome.row7a -after .getViewHome.row7
  }
}

##########################################################################

proc SetHomeToCurrent {} {
    global curView
    global viewHomeXLow viewHomeXHigh
    global viewHomeYLow viewHomeYHigh

    set filters [DEVise getVisualFilters $curView]
    # Filter is <x low> <y low> <x high> <y high> <something>
    set filter [lindex $filters [expr [llength $filters]-1]]

    set viewHomeXLow [lindex $filter 0]
    set viewHomeYLow [lindex $filter 1]
    set viewHomeXHigh [lindex $filter 2]
    set viewHomeYHigh [lindex $filter 3]
}

##########################################################################

proc SetViewHome {} {
    global curView
    global viewHomeMode
    global viewHomeXMar viewHomeYMar
    global viewHomeXLow viewHomeXHigh
    global viewHomeYLow viewHomeYHigh
    global viewHomeDoX viewHomeDoY
    global viewHomeYZero

    # Arguments: <viewName> <mode> <autoXMargin> <autoYMargin> <manXLo>
    # <manYLo> <manXHi> <manYHi>
    DEVise viewSetHome $curView $viewHomeDoX $viewHomeDoY $viewHomeMode \
      $viewHomeYZero \
      $viewHomeXMar $viewHomeYMar $viewHomeXLow $viewHomeYLow $viewHomeXHigh \
      $viewHomeYHigh
}

##########################################################################

proc GetViewHorPan {} {
    global verticalPan
    set verticalPan 0
    EditViewPan
}

##########################################################################

proc GetViewVerPan {} {
    global verticalPan
    set verticalPan 1
    EditViewPan
}

##########################################################################

proc EditViewPan {} {
    global curView
    global verticalPan

    # If this window already exists, raise it to the top and return.
    if {[WindowVisible .getViewPan]} {
	return
    }

    # Create the top level widget and the frames we'll later use for
    # positioning.
    toplevel .getViewPan
    if {$verticalPan} {
        wm title .getViewPan "Set View Vertical Scroll"
    } else {
        wm title .getViewPan "Set View Horizontal Scroll"
    }

    frame .getViewPan.row1
    frame .getViewPan.row2
    frame .getViewPan.row3
    frame .getViewPan.row4

    # Create the various widgets.
    button .getViewPan.ok -text "OK" -width 10 \
      -command {SetViewPan; destroy .getViewPan}
    button .getViewPan.cancel -text "Cancel" -width 10 \
      -command "destroy .getViewPan"

    # See ViewGraph.h for mode values...
    global viewPanMode
    radiobutton .getViewPan.rel -text "Relative" -variable viewPanMode \
      -value 1 -command "SwitchPanMode"
    radiobutton .getViewPan.abs -text "Absolute" -variable viewPanMode \
      -value 2 -command "SwitchPanMode"

    global viewPanRel viewPanAbs
    label .getViewPan.relLab -text "Relative pan:"
    entry .getViewPan.relEnt -width 20 -relief sunken \
      -textvariable viewPanRel
    label .getViewPan.relLab2 -text "%"
    label .getViewPan.absLab -text "Absolute pan:"
    entry .getViewPan.absEnt -width 20 -relief sunken \
      -textvariable viewPanAbs

    # These frames are for spacing only.
    frame .getViewPan.row2a -width 90m -height 6m
    frame .getViewPan.row4a -width 10m -height 4m

    # Pack some widgets into the frames.  Other widges will be packed in
    # the SwitchHomeMode procedure.
    pack .getViewPan.row1 -side bottom -pady 4m
    pack .getViewPan.row2 -side top
    pack .getViewPan.row2a -side top
    pack .getViewPan.row4a -side top -pady 1m

    pack .getViewPan.ok .getViewPan.cancel -in .getViewPan.row1 \
      -side left -padx 3m
    pack .getViewPan.rel .getViewPan.abs -in .getViewPan.row2 \
      -side top -pady 1m
    pack .getViewPan.relLab .getViewPan.relEnt .getViewPan.relLab2 \
      -in .getViewPan.row3 -side left
    pack .getViewPan.absLab .getViewPan.absEnt -in .getViewPan.row4 \
      -side left

    # Get the current values from the view and set the GUI accordingly.
    # Returns: <mode> <relativePan> <absolutePan>
    if {$verticalPan} {
        set panInfo [DEVise viewGetVerPan $curView]
    } else {
        set panInfo [DEVise viewGetHorPan $curView]
    }
    set viewPanMode [lindex $panInfo 0]
    # Convert to percent.
    set viewPanRel [expr [lindex $panInfo 1] * 100.0]
    set viewPanAbs [lindex $panInfo 2]
    SwitchPanMode

    # Wait for the user to make a selection from this window.
    tkwait visibility .getViewPan
    grab set .getViewPan
    tkwait window .getViewPan
}

##########################################################################

proc SwitchPanMode {} {
    global viewPanMode

    if {$viewPanMode == 1} {
      pack .getViewPan.row3 -after .getViewPan.row2a -pady 1m

      pack forget .getViewPan.row4
    } else {
      pack forget .getViewPan.row3

      pack .getViewPan.row4 -after .getViewPan.row2a -pady 1m
    }
}

##########################################################################

proc SetViewPan {} {
    global curView
    global verticalPan
    global viewPanMode
    global viewPanRel viewPanAbs

    # Arguments: <viewName> <mode> <relativePan> <absolutePan>
    # Convert relativePan from percent to fraction.
    if {$verticalPan} {
        DEVise viewSetVerPan $curView $viewPanMode \
          [expr $viewPanRel / 100.0] $viewPanAbs
    } else {
        DEVise viewSetHorPan $curView $viewPanMode \
          [expr $viewPanRel / 100.0] $viewPanAbs
    }
}

#============================================================================

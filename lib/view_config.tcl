# ========================================================================
# DEVise Data Visualization Software
# (c) Copyright 1992-1996
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

    frame .getViewHome.row1
    frame .getViewHome.row2
    frame .getViewHome.row3
    frame .getViewHome.row4
    frame .getViewHome.row5
    frame .getViewHome.row6
    frame .getViewHome.row7

    # Create the various widgets.
    button .getViewHome.ok -text "OK" -width 10 \
      -command {SetViewHome; destroy .getViewHome}
    button .getViewHome.cancel -text "Cancel" -width 10 \
      -command "destroy .getViewHome"

    # See ViewGraph.h for mode values...
    global viewHomeMode
    radiobutton .getViewHome.auto -text "Automatic" -variable viewHomeMode \
      -value 1 -command "SwitchHomeMode"
    radiobutton .getViewHome.man -text "Fixed" -variable viewHomeMode \
      -value 2 -command "SwitchHomeMode"

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
    frame .getViewHome.row2a -width 130m -height 6m
    frame .getViewHome.row4a -width 10m -height 6m
    frame .getViewHome.row7a -width 10m -height 4m

    # Pack some widgets into the frames.  Other widges will be packed in
    # the SwitchHomeMode procedure.
    pack .getViewHome.row1 -side bottom -pady 4m
    pack .getViewHome.row2 -side top
    pack .getViewHome.row2a -side top

    pack .getViewHome.ok .getViewHome.cancel -in .getViewHome.row1 \
      -side left -padx 3m
    pack .getViewHome.auto .getViewHome.man -in .getViewHome.row2 \
      -side top -pady 1m
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
    set viewHomeMode [lindex $home 0]
    set viewHomeXMar [lindex $home 1]
    set viewHomeYMar [lindex $home 2]
    set viewHomeXLow [lindex $home 3]
    set viewHomeYLow [lindex $home 4]
    set viewHomeXHigh [lindex $home 5]
    set viewHomeYHigh [lindex $home 6]
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
    pack .getViewHome.row3 -after .getViewHome.row2a -pady 1m
    pack .getViewHome.row4 -after .getViewHome.row3 -pady 1m
    pack .getViewHome.row4a -after .getViewHome.row4 -pady 1m

    pack forget .getViewHome.row5
    pack forget .getViewHome.row6
    pack forget .getViewHome.row7
    pack forget .getViewHome.row7a
  } else {
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

    # Arguments: <viewName> <mode> <autoXMargin> <autoYMargin> <manXLo>
    # <manYLo> <manXHi> <manYHi>
    DEVise viewSetHome $curView $viewHomeMode $viewHomeXMar $viewHomeYMar \
      $viewHomeXLow $viewHomeYLow $viewHomeXHigh $viewHomeYHigh
}

##########################################################################

proc GetViewHorPan {} {
    global curView

    # If this window already exists, raise it to the top and return.
    if {[WindowVisible .getViewHorPan]} {
	return
    }

    # Create the top level widget and the frames we'll later use for
    # positioning.
    toplevel .getViewHorPan
    wm title .getViewHorPan "Set View Horizontal Scroll"

    frame .getViewHorPan.row1
    frame .getViewHorPan.row2
    frame .getViewHorPan.row3
    frame .getViewHorPan.row4

    # Create the various widgets.
    button .getViewHorPan.ok -text "OK" -width 10 \
      -command {SetViewHorPan; destroy .getViewHorPan}
    button .getViewHorPan.cancel -text "Cancel" -width 10 \
      -command "destroy .getViewHorPan"

    # See ViewGraph.h for mode values...
    global viewHorPanMode
    radiobutton .getViewHorPan.rel -text "Relative" -variable viewHorPanMode \
      -value 1 -command "SwitchHorPanMode"
    radiobutton .getViewHorPan.abs -text "Absolute" -variable viewHorPanMode \
      -value 2 -command "SwitchHorPanMode"

    global viewHorPanRel viewHorPanAbs
    label .getViewHorPan.relLab -text "Relative pan:"
    entry .getViewHorPan.relEnt -width 20 -relief sunken \
      -textvariable viewHorPanRel
    label .getViewHorPan.relLab2 -text "%"
    label .getViewHorPan.absLab -text "Absolute pan:"
    entry .getViewHorPan.absEnt -width 20 -relief sunken \
      -textvariable viewHorPanAbs

    # These frames are for spacing only.
    frame .getViewHorPan.row2a -width 90m -height 6m
    frame .getViewHorPan.row4a -width 10m -height 4m

    # Pack some widgets into the frames.  Other widges will be packed in
    # the SwitchHomeMode procedure.
    pack .getViewHorPan.row1 -side bottom -pady 4m
    pack .getViewHorPan.row2 -side top
    pack .getViewHorPan.row2a -side top
    pack .getViewHorPan.row4a -side top -pady 1m

    pack .getViewHorPan.ok .getViewHorPan.cancel -in .getViewHorPan.row1 \
      -side left -padx 3m
    pack .getViewHorPan.rel .getViewHorPan.abs -in .getViewHorPan.row2 \
      -side top -pady 1m
    pack .getViewHorPan.relLab .getViewHorPan.relEnt .getViewHorPan.relLab2 \
      -in .getViewHorPan.row3 -side left
    pack .getViewHorPan.absLab .getViewHorPan.absEnt -in .getViewHorPan.row4 \
      -side left

    # Get the current values from the view and set the GUI accordingly.
    # Returns: <mode> <relativePan> <absolutePan>
    set horPan [DEVise viewGetHorPan $curView]
    set viewHorPanMode [lindex $horPan 0]
    # Convert to percent.
    set viewHorPanRel [expr [lindex $horPan 1] * 100.0]
    set viewHorPanAbs [lindex $horPan 2]
    SwitchHorPanMode

    # Wait for the user to make a selection from this window.
    tkwait visibility .getViewHorPan
    grab set .getViewHorPan
    tkwait window .getViewHorPan
}

##########################################################################

proc SwitchHorPanMode {} {
    global viewHorPanMode

    if {$viewHorPanMode == 1} {
      pack .getViewHorPan.row3 -after .getViewHorPan.row2a -pady 1m

      pack forget .getViewHorPan.row4
    } else {
      pack forget .getViewHorPan.row3

      pack .getViewHorPan.row4 -after .getViewHorPan.row2a -pady 1m
    }
}

##########################################################################

proc SetViewHorPan {} {
    global curView
    global viewHorPanMode
    global viewHorPanRel viewHorPanAbs

    # Arguments: <viewName> <mode> <relativePan> <absolutePan>
    # Convert relativePan from percent to fraction.
    DEVise viewSetHorPan $curView $viewHorPanMode \
      [expr $viewHorPanRel / 100.0] $viewHorPanAbs
}

#============================================================================

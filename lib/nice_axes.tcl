# ========================================================================
# DEVise Data Visualization Software
# (c) Copyright 1999
# By the DEVise Development Group
# Madison, Wisconsin
# All Rights Reserved.
# ========================================================================

# Under no circumstances is this software to be copied, distributed,
# or altered in any way without prior permission from the DEVise
# Development Group.

############################################################

# This module contains the GUI components related to nice axes.

############################################################

# $Id$

# $Log$

############################################################

proc EditNiceAxes {} {
    global curView

    # If this window already exists, raise it to the top and return.
    if {[WindowVisible .editNiceAxes]} {
	return
    }

    # Create the top level widget and the frames we'll later use for
    # positioning.
    toplevel .editNiceAxes
    wm title .editNiceAxes "Nice Axes ($curView)"

    frame .editNiceAxes.row1
    frame .editNiceAxes.row2

    # Create the various widgets.
    button .editNiceAxes.ok -text "OK" -width 10 \
      -command {SetNiceAxes; destroy .editNiceAxes}
    button .editNiceAxes.cancel -text "Cancel" -width 10 \
      -command "destroy .editNiceAxes"

    # Check buttons to turn on and off automatic nice axes.
    global niceXAxis niceYAxis
    checkbutton .editNiceAxes.niceX -text "Auto Nice X" -variable niceXAxis
    checkbutton .editNiceAxes.niceY -text "Auto Nice Y" -variable niceYAxis

    # Pack the widgets into the frames.
    pack .editNiceAxes.row1 -side bottom -pady 4m
    pack .editNiceAxes.row2 -side top

    pack .editNiceAxes.ok .editNiceAxes.cancel -in .editNiceAxes.row1 \
      -side left -padx 3m
    pack .editNiceAxes.niceX .editNiceAxes.niceY -in .editNiceAxes.row2 \
      -side top -pady 1m

    # Get the current values from the view and set the GUI accordingly.
    # Returns: <enabled>
    set niceAxes [DEVise getNiceAxes $curView]
    set niceXAxis [lindex $niceAxes 0]
    set niceYAxis [lindex $niceAxes 1]

    # Wait for the user to make a selection from this window.
    tkwait visibility .editNiceAxes
    grab set .editNiceAxes
    tkwait window .editNiceAxes
}

##########################################################################

proc SetNiceAxes {} {
    global curView
    global niceXAxis niceYAxis

    # Arguments: <viewName> <niceX> <niceY>
    DEVise setNiceAxes $curView $niceXAxis $niceYAxis
}

#============================================================================

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

# This module contains the GUI components related to enabling and
# disabling automatic visual filter updates.

############################################################

# $Id$

# $Log$

############################################################

proc EditAutoFilter {} {
    global curView

    # If this window already exists, raise it to the top and return.
    if {[WindowVisible .editAutoFilter]} {
	return
    }

    # Create the top level widget and the frames we'll later use for
    # positioning.
    toplevel .editAutoFilter
    wm title .editAutoFilter "Auto Filter ($curView)"

    frame .editAutoFilter.row1
    frame .editAutoFilter.row2
    frame .editAutoFilter.row3
    frame .editAutoFilter.row4

    # Create the various widgets.
    button .editAutoFilter.ok -text "OK" -width 10 \
      -command {SetAutoFilter; destroy .editAutoFilter}
    button .editAutoFilter.cancel -text "Cancel" -width 10 \
      -command "destroy .editAutoFilter"

    global autoFilterEnabled

    # Get the current value from the view and set the GUI accordingly.
    set autoFilterEnabled [DEVise getViewAutoFilter $curView]

    # Radio buttons to enable/disable automatic filter updates.
    radiobutton .editAutoFilter.yes -text "Enable" \
      -variable autoFilterEnabled -value 1
    radiobutton .editAutoFilter.no -text "Disable" \
      -variable autoFilterEnabled -value 0

    # These frames are for spacing only.
    frame .editAutoFilter.row2a -width 80m -height 6m
    frame .editAutoFilter.row4a -width 10m -height 4m

    # Pack the widgets into the frames.
    pack .editAutoFilter.row1 -side bottom -pady 4m
    pack .editAutoFilter.row2 -side top
    pack .editAutoFilter.row2a -side top

    pack .editAutoFilter.ok .editAutoFilter.cancel -in .editAutoFilter.row1 \
      -side left -padx 3m
    pack .editAutoFilter.no .editAutoFilter.yes -in .editAutoFilter.row2 \
      -side top -pady 1m

    # Wait for the user to make a selection from this window.
    tkwait visibility .editAutoFilter
    grab set .editAutoFilter
    tkwait window .editAutoFilter
}

##########################################################################

proc SetAutoFilter {} {
    global curView
    global autoFilterEnabled

    DEVise setViewAutoFilter $curView $autoFilterEnabled
}

#============================================================================

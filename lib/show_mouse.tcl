# ========================================================================
# DEVise Data Visualization Software
# (c) Copyright 2000-2001
# By the DEVise Development Group
# Madison, Wisconsin
# All Rights Reserved.
# ========================================================================

# Under no circumstances is this software to be copied, distributed,
# or altered in any way without prior permission from the DEVise
# Development Group.

############################################################

# This module contains the GUI components related to enabling and
# disabling showing the mouse location in a view.

############################################################

# $Id$

# $Log$
# Revision 1.1  2000/06/20 16:57:52  wenger
# Added commands and GUI to enable/disable the display of mouse location
# in various views, and globally.
#

############################################################

proc EditShowMouse {} {
    global curView

    # If this window already exists, raise it to the top and return.
    if {[WindowVisible .editShowMouse]} {
	return
    }

    # Create the top level widget and the frames we'll later use for
    # positioning.
    toplevel .editShowMouse
    wm title .editShowMouse "Show Mouse Location ($curView)"

    frame .editShowMouse.row1
    frame .editShowMouse.row2
    frame .editShowMouse.row3
    frame .editShowMouse.row4

    # Create the various widgets.
    button .editShowMouse.ok -text "OK" -width 10 \
      -command {SetShowMouse; destroy .editShowMouse}
    button .editShowMouse.cancel -text "Cancel" -width 10 \
      -command "destroy .editShowMouse"

    # Get the current value from the view and set the GUI accordingly.
    # Show value meanings: 0 = none; 1 = xy; 2 = x; 3 = y;
    # values are this way for backwards compatibility.
    set showMouseLoc [DEVise getShowMouseLocation $curView]

    global showMouseX showMouseY
    set showMouseX [expr $showMouseLoc == 1 || $showMouseLoc == 2]
    set showMouseY [expr $showMouseLoc == 1 || $showMouseLoc == 3]

    # Radio buttons to enable/disable showing of mouse location.
    checkbutton .editShowMouse.yes -text "X" \
      -variable showMouseX
    checkbutton .editShowMouse.no -text "Y" \
      -variable showMouseY

    # These frames are for spacing only.
    frame .editShowMouse.row2a -width 80m -height 6m
    frame .editShowMouse.row4a -width 10m -height 4m

    # Pack the widgets into the frames.
    pack .editShowMouse.row1 -side bottom -pady 4m
    pack .editShowMouse.row2 -side top
    pack .editShowMouse.row2a -side top

    pack .editShowMouse.ok .editShowMouse.cancel -in .editShowMouse.row1 \
      -side left -padx 3m
    pack .editShowMouse.yes .editShowMouse.no -in .editShowMouse.row2 \
      -side top -pady 1m

    # Wait for the user to make a selection from this window.
    tkwait visibility .editShowMouse
    grab set .editShowMouse
    tkwait window .editShowMouse
}

##########################################################################

proc SetShowMouse {} {
    global curView
    global showMouseX showMouseY

    # Show value meanings: 0 = none; 1 = xy; 2 = x; 3 = y;
    # values are this way for backwards compatibility.
    if {$showMouseX} {
        if {$showMouseY} {
	    set showMouseLoc 1
	} else {
	    set showMouseLoc 2
	}
    } else {
        if {$showMouseY} {
	    set showMouseLoc 3
	} else {
	    set showMouseLoc 0
	}
    }

    DEVise setShowMouseLocation $curView $showMouseLoc
}

#============================================================================

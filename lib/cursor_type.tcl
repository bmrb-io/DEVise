# ========================================================================
# DEVise Data Visualization Software
# (c) Copyright 2000
# By the DEVise Development Group
# Madison, Wisconsin
# All Rights Reserved.
# ========================================================================

# Under no circumstances is this software to be copied, distributed,
# or altered in any way without prior permission from the DEVise
# Development Group.

############################################################

# This module contains GUI for editing a cursor's type (X, Y, or XY).

############################################################

# $Id$

# $Log$

############################################################

proc EditCursorType {} {

    global ectName
    set ectName [DoGetCursor "Select cursor to edit"]
    if {$ectName == ""} {
	# User selected "Cancel".
        return
    }

    # If this window already exists, raise it to the top and return.
    if {[WindowVisible .editCursorType]} {
	return
    }

    # Create the top level widget and the frames we'll later use for
    # positioning.
    toplevel .editCursorType
    wm title .editCursorType "Cursor Type ($ectName)"

    frame .editCursorType.row1
    frame .editCursorType.row2

    # Create the various widgets.
    button .editCursorType.ok -text "OK" -width 10 \
      -command {SetCursorType; destroy .editCursorType}
    button .editCursorType.cancel -text "Cancel" -width 10 \
      -command "destroy .editCursorType"

    # Check buttons to control the various constraints.
    global ectX ectY
    checkbutton .editCursorType.x -text "X" \
      -variable ectX
    checkbutton .editCursorType.y -text "Y" \
      -variable ectY

    # Pack the widgets into the frames.
    pack .editCursorType.row1 -side bottom -pady 4m
    pack .editCursorType.row2 -side top

    pack .editCursorType.ok .editCursorType.cancel \
      -in .editCursorType.row1 -side left -padx 3m
    pack .editCursorType.x .editCursorType.y \
      -in .editCursorType.row2 -side top -pady 1m

    # Get the current values from the view and set the GUI accordingly.
    set flag [DEVise getCursorFlag $ectName]
    set ectX [expr ($flag & 1) != 0]
    set ectY [expr ($flag & 2) != 0]

    # Wait for the user to make a selection from this window.
    tkwait visibility .editCursorType
    grab set .editCursorType
    tkwait window .editCursorType
}

##########################################################################

proc SetCursorType {} {
    global ectName
    global ectX ectY

    set flag [expr $ectX + ($ectY << 1)]

    DEVise setCursorFlag $ectName $flag
}

#============================================================================

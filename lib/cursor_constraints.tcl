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

# This module contains GUI for enabling and disabling various
# cursor constraints.

############################################################

# $Id$

# $Log$
# Revision 1.2  2000/08/30 20:16:20  wenger
# Oops!  Left a couple of diagnostics prints in.
#
# Revision 1.1  2000/08/30 20:09:12  wenger
# Added the option of forcing a cursor to be entirely within its destination
# view; added control for whether a cursor must be at least partially within
# its destination view; generally improved implementation of cursor
# constraints.
#

############################################################

proc EditCursorConstraints {} {

    global eccName
    set eccName [DoGetCursor "Select cursor for constraints"]
    if {$eccName == ""} {
	# User selected "Cancel".
        return
    }

    # If this window already exists, raise it to the top and return.
    if {[WindowVisible .editCursorConstraints]} {
	return
    }

    # Create the top level widget and the frames we'll later use for
    # positioning.
    toplevel .editCursorConstraints
    wm title .editCursorConstraints "Cursor Constraints ($eccName)"

    frame .editCursorConstraints.row1
    frame .editCursorConstraints.row2

    # Create the various widgets.
    button .editCursorConstraints.ok -text "OK" -width 10 \
      -command {SetCursorConstraints; destroy .editCursorConstraints}
    button .editCursorConstraints.cancel -text "Cancel" -width 10 \
      -command "destroy .editCursorConstraints"

    # Check buttons to control the various constraints.
    global eccFixed eccPartInDest eccAllInDest
    checkbutton .editCursorConstraints.fixed -text "Fixed size" \
      -variable eccFixed
    checkbutton .editCursorConstraints.part -text "Part in dest view" \
      -variable eccPartInDest
    checkbutton .editCursorConstraints.all -text "All in dest view" \
      -variable eccAllInDest

    # Pack the widgets into the frames.
    pack .editCursorConstraints.row1 -side bottom -pady 4m
    pack .editCursorConstraints.row2 -side top

    pack .editCursorConstraints.ok .editCursorConstraints.cancel \
      -in .editCursorConstraints.row1 -side left -padx 3m
    pack .editCursorConstraints.fixed .editCursorConstraints.part \
      .editCursorConstraints.all \
      -in .editCursorConstraints.row2 -side top -pady 1m

    # Get the current values from the view and set the GUI accordingly.
    set constraints [DEVise getCursorConstraints $eccName]
    set eccFixed [lindex $constraints 0]
    set eccPartInDest [lindex $constraints 1]
    set eccAllInDest [lindex $constraints 2]

    # Wait for the user to make a selection from this window.
    tkwait visibility .editCursorConstraints
    grab set .editCursorConstraints
    tkwait window .editCursorConstraints
}

##########################################################################

proc SetCursorConstraints {} {
    global eccName
    global eccFixed eccPartInDest eccAllInDest

    DEVise setCursorConstraints $eccName $eccFixed $eccPartInDest $eccAllInDest
}

#============================================================================

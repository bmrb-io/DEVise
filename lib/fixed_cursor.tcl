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

# This module contains GUI for turning fixed cursor size on and off.

############################################################

# $Id$

# $Log$

############################################################

proc EditFixedCursor {} {

    global efcName
    set efcName [DoGetCursor "Select cursor for fixed/variable size"]

    # If this window already exists, raise it to the top and return.
    if {[WindowVisible .editFixedCursor]} {
	return
    }

    # Create the top level widget and the frames we'll later use for
    # positioning.
    toplevel .editFixedCursor
    wm title .editFixedCursor "Fixed Cursor Size ($efcName)"

    frame .editFixedCursor.row1
    frame .editFixedCursor.row2

    # Create the various widgets.
    button .editFixedCursor.ok -text "OK" -width 10 \
      -command {SetFixedCursor; destroy .editFixedCursor}
    button .editFixedCursor.cancel -text "Cancel" -width 10 \
      -command "destroy .editFixedCursor"

    # Radio buttons to enable/disable duplicate elimination
    global efcFixed
    radiobutton .editFixedCursor.yes -text "Enable" \
      -variable efcFixed -value 1
    radiobutton .editFixedCursor.no -text "Disable" \
      -variable efcFixed -value 0

    # Pack the widgets into the frames.
    pack .editFixedCursor.row1 -side bottom -pady 4m
    pack .editFixedCursor.row2 -side top

    pack .editFixedCursor.ok .editFixedCursor.cancel -in .editFixedCursor.row1 \
      -side left -padx 3m
    pack .editFixedCursor.no .editFixedCursor.yes -in .editFixedCursor.row2 \
      -side top -pady 1m

    # Get the current values from the view and set the GUI accordingly.
    # Returns: <enabled>
    set efcFixed [DEVise getCursorFixedSize $efcName]

    # Wait for the user to make a selection from this window.
    tkwait visibility .editFixedCursor
    grab set .editFixedCursor
    tkwait window .editFixedCursor
}

##########################################################################

proc SetFixedCursor {} {
    global efcName
    global efcFixed

    # Arguments: <cursor name> <fixed>
    DEVise setCursorFixedSize $efcName $efcFixed
}

#============================================================================

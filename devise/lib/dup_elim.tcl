# ========================================================================
# DEVise Data Visualization Software
# (c) Copyright 1992-1998
# By the DEVise Development Group
# Madison, Wisconsin
# All Rights Reserved.
# ========================================================================

# Under no circumstances is this software to be copied, distributed,
# or altered in any way without prior permission from the DEVise
# Development Group.

############################################################

# This module contains the GUI components related to duplicate elimination.

############################################################

# $Id$

# $Log$

############################################################

proc EditDupElim {} {
    global curView

    # If this window already exists, raise it to the top and return.
    if {[WindowVisible .editDupElim]} {
	return
    }

    # Create the top level widget and the frames we'll later use for
    # positioning.
    toplevel .editDupElim
    wm title .editDupElim "Duplicate Elimination ($curView)"

    frame .editDupElim.row1
    frame .editDupElim.row2

    # Create the various widgets.
    button .editDupElim.ok -text "OK" -width 10 \
      -command {SetDupElim; destroy .editDupElim}
    button .editDupElim.cancel -text "Cancel" -width 10 \
      -command "destroy .editDupElim"

    # Radio buttons to enable/disable duplicate elimination
    global dupElimEnabled
    radiobutton .editDupElim.yes -text "Enable" \
      -variable dupElimEnabled -value 1
    radiobutton .editDupElim.no -text "Disable" \
      -variable dupElimEnabled -value 0

    # Pack the widgets into the frames.
    pack .editDupElim.row1 -side bottom -pady 4m
    pack .editDupElim.row2 -side top

    pack .editDupElim.ok .editDupElim.cancel -in .editDupElim.row1 \
      -side left -padx 3m
    pack .editDupElim.no .editDupElim.yes -in .editDupElim.row2 \
      -side top -pady 1m

    # Get the current values from the view and set the GUI accordingly.
    # Returns: <enabled>
    set dupElimEnabled [DEVise getDupElim $curView]

    # Wait for the user to make a selection from this window.
    tkwait visibility .editDupElim
    grab set .editDupElim
    tkwait window .editDupElim
}

##########################################################################

proc SetDupElim {} {
    global curView
    global dupElimEnabled

    # Arguments: <viewName> <enabled>
    DEVise setDupElim $curView $dupElimEnabled
}

#============================================================================

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
# disabling actions in views.

############################################################

# $Id$

# $Log$

############################################################

proc EditActionDisable {} {
    global curView

    # If this window already exists, raise it to the top and return.
    if {[WindowVisible .editActionDisable]} {
	return
    }

    # Create the top level widget and the frames we'll later use for
    # positioning.
    toplevel .editActionDisable
    wm title .editActionDisable "Auto Filter ($curView)"

    frame .editActionDisable.row1
    frame .editActionDisable.row2
    frame .editActionDisable.row3
    frame .editActionDisable.row4

    # Create the various widgets.
    button .editActionDisable.ok -text "OK" -width 10 \
      -command {SetActionDisable; destroy .editActionDisable}
    button .editActionDisable.cancel -text "Cancel" -width 10 \
      -command "destroy .editActionDisable"

    global rubberbandDisabled
    global cursorMoveDisabled
    global drillDownDisabled
    global keysDisabled

    # Get the current value from the view and set the GUI accordingly.
    set actionsDisabled [DEVise viewGetDisabledActions $curView]
    set rubberbandDisabled [lindex $actionsDisabled 0]
    set cursorMoveDisabled [lindex $actionsDisabled 1]
    set drillDownDisabled [lindex $actionsDisabled 2]
    set keysDisabled [lindex $actionsDisabled 3]

    # Check buttons to enable/disable automatic filter updates.
    checkbutton .editActionDisable.rubberband -text "Rubberband disabled" \
      -variable rubberbandDisabled
    checkbutton .editActionDisable.cursorMove -text "Cursor move disabled" \
      -variable cursorMoveDisabled
    checkbutton .editActionDisable.drillDown -text "Drill down disabled" \
      -variable drillDownDisabled
    checkbutton .editActionDisable.keys -text "Keys disabled" \
      -variable keysDisabled

    # These frames are for spacing only.
    frame .editActionDisable.row2a -width 80m -height 6m
    frame .editActionDisable.row4a -width 10m -height 4m

    # Pack the widgets into the frames.
    pack .editActionDisable.row1 -side bottom -pady 4m
    pack .editActionDisable.row2 -side top
    pack .editActionDisable.row2a -side top

    pack .editActionDisable.ok .editActionDisable.cancel \
      -in .editActionDisable.row1 -side left -padx 3m
    pack .editActionDisable.rubberband .editActionDisable.cursorMove \
      .editActionDisable.drillDown .editActionDisable.keys \
      -in .editActionDisable.row2 -side top -pady 1m

    # Wait for the user to make a selection from this window.
    tkwait visibility .editActionDisable
    grab set .editActionDisable
    tkwait window .editActionDisable
}

##########################################################################

proc SetActionDisable {} {
    global curView
    global rubberbandDisabled
    global cursorMoveDisabled
    global drillDownDisabled
    global keysDisabled

    DEVise viewSetDisabledActions $curView $rubberbandDisabled \
      $cursorMoveDisabled $drillDownDisabled $keysDisabled
}

#============================================================================

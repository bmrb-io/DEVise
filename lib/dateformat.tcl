# ========================================================================
# DEVise Data Visualization Software
# (c) Copyright 1998
# By the DEVise Development Group
# Madison, Wisconsin
# All Rights Reserved.
# ========================================================================

# Under no circumstances is this software to be copied, distributed,
# or altered in any way without prior permission from the DEVise
# Development Group.

############################################################

# This module contains the GUI components related to selecting
# an axis date format.

############################################################

# $Id$

# $Log$

############################################################

# which is X or Y
proc EditDateFormat { which } {
    global curView
    global dateFormat


    # If this window already exists, raise it to the top and return.
    if {[WindowVisible .getDateFormat]} {
	return
    }


    # Create the top level widget and the frames we'll later use for
    # positioning.
    toplevel .getDateFormat
    wm title .getDateFormat "Choose Axis Date Format"

    frame .getDateFormat.row1
    frame .getDateFormat.row2
    frame .getDateFormat.row3
    frame .getDateFormat.row4


    if {$which == "X"} {
        set dateFormat [DEVise getXAxisDateFormat $curView]
    } else {
        set dateFormat [DEVise getYAxisDateFormat $curView]
    }


    # Create the various objects that will be shown in the window.
    label .getDateFormat.label -text "'$curView' $which axis date format"

    label .getDateFormat.note -text "Enter cftime() format string: "
    entry .getDateFormat.entry -width 20 -relief sunken \
      -textvariable dateFormat

    menubutton .getDateFormat.option -relief raised -text "Options" \
      -menu .getDateFormat.option.menu
    menu .getDateFormat.option.menu
    .getDateFormat.option.menu add command -label "%b %d %Y %T (default)" \
      -command { set dateFormat "%b %d %Y %T" }
    .getDateFormat.option.menu add command -label "%Y-%m-%d %T (ISO)" \
      -command { set dateFormat "%Y-%m-%d %T" }
    .getDateFormat.option.menu add command -label "%b %d %Y (date only)" \
      -command { set dateFormat "%b %d %Y" }
    .getDateFormat.option.menu add command -label "%T (time only)" \
      -command { set dateFormat "%T" }
    .getDateFormat.option.menu add command -label "%a (day of week)" \
      -command { set dateFormat "%a" }

    button .getDateFormat.ok -text "OK" -width 10 \
      -command "SetDateFormat {$which}; destroy .getDateFormat"
    button .getDateFormat.cancel -text "Cancel" -width 10 \
      -command "destroy .getDateFormat"


    # Pack everything into the frames.
    pack .getDateFormat.row1 -side top -pady 4m
    pack .getDateFormat.row2 -side top -padx 5m -pady 1m
    pack .getDateFormat.row3 -side top -pady 1m
    pack .getDateFormat.row4 -side top -pady 4m

    pack .getDateFormat.label -in .getDateFormat.row1 -side top
    pack .getDateFormat.note .getDateFormat.entry -in .getDateFormat.row2 \
      -side left
    pack .getDateFormat.option -in .getDateFormat.row3 -side top
    pack .getDateFormat.ok .getDateFormat.cancel -in .getDateFormat.row4 \
      -side left -padx 3m


    # Wait for the user to make a selection from this window.
    tkwait visibility .getDateFormat
    grab set .getDateFormat
    tkwait window .getDateFormat
}

##########################################################################

proc SetDateFormat { which } {
    global curView
    global dateFormat

    if {$which == "X"} {
	DEVise setXAxisDateFormat $curView $dateFormat
    } else {
	DEVise setYAxisDateFormat $curView $dateFormat
    }
}


#============================================================================

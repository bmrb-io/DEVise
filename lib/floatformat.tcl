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

# This module contains the GUI components related to selecting
# an axis floating-point format.

############################################################

# $Id$

# $Log$
# Revision 1.1  2000/06/20 22:17:14  wenger
# Added floating-point format for axes and mouse location display.
#

############################################################

# which is X or Y
proc EditFloatFormat { which } {
    global curView
    global floatFormat


    # If this window already exists, raise it to the top and return.
    if {[WindowVisible .editFloatFormat]} {
	return
    }


    # Create the top level widget and the frames we'll later use for
    # positioning.
    toplevel .editFloatFormat
    wm title .editFloatFormat "Choose Axis Floating-Point Format"

    frame .editFloatFormat.row1
    frame .editFloatFormat.row2
    frame .editFloatFormat.row3
    frame .editFloatFormat.row4


    if {$which == "X"} {
        set floatFormat [DEVise getXAxisFloatFormat $curView]
    } else {
        set floatFormat [DEVise getYAxisFloatFormat $curView]
    }


    # Create the various objects that will be shown in the window.
    label .editFloatFormat.label -text "'$curView' $which axis float format"

    label .editFloatFormat.note -text "Enter printf() format string: "
    entry .editFloatFormat.entry -width 20 -relief sunken \
      -textvariable floatFormat

    menubutton .editFloatFormat.option -relief raised -text "Options" \
      -menu .editFloatFormat.option.menu
    menu .editFloatFormat.option.menu
    .editFloatFormat.option.menu add command -label "%.4g (default)" \
      -command { set floatFormat "%.4g" }
    .editFloatFormat.option.menu add command -label "%.0f (integer)" \
      -command { set floatFormat "%.0f" }
    .editFloatFormat.option.menu add command -label "%.6g (high-precision)" \
      -command { set floatFormat "%.6g" }
    .editFloatFormat.option.menu add command -label "- (blank)" \
      -command { set floatFormat "-" }

    button .editFloatFormat.ok -text "OK" -width 10 \
      -command "SetFloatFormat {$which}; destroy .editFloatFormat"
    button .editFloatFormat.cancel -text "Cancel" -width 10 \
      -command "destroy .editFloatFormat"


    # Pack everything into the frames.
    pack .editFloatFormat.row1 -side top -pady 4m
    pack .editFloatFormat.row2 -side top -padx 5m -pady 1m
    pack .editFloatFormat.row3 -side top -pady 1m
    pack .editFloatFormat.row4 -side top -pady 4m

    pack .editFloatFormat.label -in .editFloatFormat.row1 -side top
    pack .editFloatFormat.note .editFloatFormat.entry -in .editFloatFormat.row2 \
      -side left
    pack .editFloatFormat.option -in .editFloatFormat.row3 -side top
    pack .editFloatFormat.ok .editFloatFormat.cancel -in .editFloatFormat.row4 \
      -side left -padx 3m


    # Wait for the user to make a selection from this window.
    tkwait visibility .editFloatFormat
    grab set .editFloatFormat
    tkwait window .editFloatFormat
}

##########################################################################

proc SetFloatFormat { which } {
    global curView
    global floatFormat

    if {$which == "X"} {
	DEVise setXAxisFloatFormat $curView $floatFormat
    } else {
	DEVise setYAxisFloatFormat $curView $floatFormat
    }
}


#============================================================================

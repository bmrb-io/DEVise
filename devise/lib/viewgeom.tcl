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

proc EditViewGeom {} {
    global curView
    global viewX viewY viewWidth viewHeight

    #TEMPTEMP
    set viewX 0.0
    set viewY 0.0
    set viewWidth 0.5
    set viewHeight 0.5

    #TEMPTEMP
    #DEVise setViewGeometry $curView 0.2 0.1 0.7 0.3
    #return


    # If this window already exists, raise it to the top and return.
    if {[WindowVisible .getViewGeom]} {
	return
    }


    # Create the top level widget and the frames we'll later use for
    # positioning.
    toplevel .getViewGeom
    wm title .getViewGeom "Set View Geometry"

    frame .getViewGeom.row1
    frame .getViewGeom.row2
    frame .getViewGeom.row3
    frame .getViewGeom.row4
    frame .getViewGeom.row5


    # Create the various objects that will be shown in the window.
    label .getViewGeom.xLabel -text "X:"
    entry .getViewGeom.xEntry -width 10 -relief sunken -textvariable viewX
    label .getViewGeom.yLabel -text "Y:"
    entry .getViewGeom.yEntry -width 10 -relief sunken -textvariable viewY
    label .getViewGeom.widthLabel -text "Width:"
    entry .getViewGeom.widthEntry -width 10 -relief sunken \
      -textvariable viewWidth
    label .getViewGeom.heightLabel -text "Height:"
    entry .getViewGeom.heightEntry -width 10 -relief sunken \
      -textvariable viewHeight

    button .getViewGeom.ok -text "OK" -width 10 \
      -command "SetViewGeom; destroy .getViewGeom"
    button .getViewGeom.cancel -text "Cancel" -width 10 \
      -command "destroy .getViewGeom"


    # Pack everything into the frames.
    pack .getViewGeom.row1 -side top -pady 4m
    pack .getViewGeom.row2 -side top -pady 1m
    pack .getViewGeom.row3 -side top -pady 1m
    pack .getViewGeom.row4 -side top -pady 4m
    pack .getViewGeom.row5 -side top -pady 4m

    pack .getViewGeom.xLabel .getViewGeom.xEntry -in .getViewGeom.row1 \
      -side left
    pack .getViewGeom.yLabel .getViewGeom.yEntry -in .getViewGeom.row2 \
      -side left
    pack .getViewGeom.widthLabel .getViewGeom.widthEntry \
      -in .getViewGeom.row3 -side left
    pack .getViewGeom.heightLabel .getViewGeom.heightEntry \
      -in .getViewGeom.row4 -side left
    pack .getViewGeom.ok .getViewGeom.cancel -in .getViewGeom.row5 \
      -side left -padx 3m


    # Wait for the user to make a selection from this window.
    tkwait visibility .getViewGeom
    grab set .getViewGeom
    tkwait window .getViewGeom
}

##########################################################################

proc SetViewGeom {} {
    global curView
    global viewX viewY viewWidth viewHeight

    DEVise setViewGeometry $curView $viewX $viewY $viewWidth $viewHeight
}

#============================================================================

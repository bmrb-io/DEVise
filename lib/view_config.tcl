# ========================================================================
# DEVise Data Visualization Software
# (c) Copyright 1992-1996
# By the DEVise Development Group
# Madison, Wisconsin
# All Rights Reserved.
# ========================================================================

# Under no circumstances is this software to be copied, distributed,
# or altered in any way without prior permission from the DEVise
# Development Group.

############################################################

# This module contains the GUI components related to view configuration
# (setting the home and panning parameters).

############################################################

# $Id$

# $Log$

############################################################

proc GetViewHome {} {
    global curView


    # If this window already exists, raise it to the top and return.
    if {[WindowVisible .getViewHome]} {
	return
    }


    # Create the top level widget and the frames we'll later use for
    # positioning.
    toplevel .getViewHome
    wm title .getViewHome "Set View Home"


    frame .getViewHome.row1


    button .getViewHome.ok -text "OK" -width 10 \
      -command "destroy .getViewHome"
    button .getViewHome.cancel -text "Cancel" -width 10 \
      -command "destroy .getViewHome"


    # Pack everything into the frames.
    pack .getViewHome.row1 -side top -pady 4m

    pack .getViewHome.ok .getViewHome.cancel -in .getViewHome.row1 \
      -side left -padx 3m



    # Wait for the user to make a selection from this window.
    tkwait visibility .getViewHome
    grab set .getViewHome
    tkwait window .getViewHome
}

##########################################################################

proc GetViewHorPan {} {
    global curView


    # If this window already exists, raise it to the top and return.
    if {[WindowVisible .getViewHorPan]} {
	return
    }


    # Create the top level widget and the frames we'll later use for
    # positioning.
    toplevel .getViewHorPan
    wm title .getViewHorPan "Set View Horizontal Pan"


    frame .getViewHorPan.row1


    button .getViewHorPan.ok -text "OK" -width 10 \
      -command "destroy .getViewHorPan"
    button .getViewHorPan.cancel -text "Cancel" -width 10 \
      -command "destroy .getViewHorPan"


    # Pack everything into the frames.
    pack .getViewHorPan.row1 -side top -pady 4m

    pack .getViewHorPan.ok .getViewHorPan.cancel -in .getViewHorPan.row1 \
      -side left -padx 3m



    # Wait for the user to make a selection from this window.
    tkwait visibility .getViewHorPan
    grab set .getViewHorPan
    tkwait window .getViewHorPan
}

##########################################################################



#============================================================================

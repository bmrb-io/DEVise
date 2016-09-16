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

# This module contains the GUI components related to view print
# configuration (whether to print a window, and whether to
# print it as a bitmap).

############################################################

# $Id$

# $Log$
# Revision 1.2  1997/05/30 21:18:30  wenger
# Accidentally added this damn thing on a branch the first time.
#
# Revision 1.1.2.1  1997/05/30 20:46:50  wenger
# Added GUI to allow user to specify windows to exclude from display
# print and/or print from pixmaps (for EmbeddedTk).  Exclusion is
# implemented but pixmap printing is not.
#

############################################################

proc GetWinPrintConfig {} {
    global curView
    global printCfgWin doExclude doPixmap

    if {![CurrentView]} {
        return
    }

    # If this window already exists, raise it to the top and return.
    if {[WindowVisible .getWinPrintCfg]} {
	return
    }

    set printCfgWin [DEVise getViewWin $curView]

    # Create the top level widget and the frames we'll later use for
    # positioning.
    toplevel .getWinPrintCfg
    wm title .getWinPrintCfg "Set Window Print Configuration"

    frame .getWinPrintCfg.row1
    frame .getWinPrintCfg.row2

    # Get the current print configuration of the window.
    set printConfig [DEVise winGetPrint $printCfgWin]
    set doExclude [lindex $printConfig 0]
    set doPixmap [lindex $printConfig 1]

    checkbutton .getWinPrintCfg.exclude -variable doExclude -relief raised \
      -bd 2 -text "Exclude" -padx 2m -pady 1m
    checkbutton .getWinPrintCfg.bitmap -variable doPixmap -relief raised \
      -bd 2 -text "Pixmap" -padx 2m -pady 1m

    button .getWinPrintCfg.ok -text "OK" -width 10 \
      -command "SetWinPrintConfig; destroy .getWinPrintCfg"
    button .getWinPrintCfg.cancel -text "Cancel" -width 10 \
      -command "destroy .getWinPrintCfg"


    # Pack everything into the frames.
    pack .getWinPrintCfg.row1 -side bottom -pady 4m
    pack .getWinPrintCfg.row2 -side top -pady 4m

    pack .getWinPrintCfg.ok .getWinPrintCfg.cancel -in .getWinPrintCfg.row1 \
      -side left -padx 3m
    pack .getWinPrintCfg.exclude .getWinPrintCfg.bitmap \
      -in .getWinPrintCfg.row2 -side left -padx 3m

    # Wait for the user to make a selection from this window.
    tkwait visibility .getWinPrintCfg
    grab set .getWinPrintCfg
    tkwait window .getWinPrintCfg
}

############################################################

proc SetWinPrintConfig {} {
    global printCfgWin doExclude doPixmap

    DEVise winSetPrint $printCfgWin $doExclude $doPixmap
}

#============================================================================

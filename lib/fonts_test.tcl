#!/s/std/bin/wish -f
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

# This module contains Tcl/Tk code that allows testing the font selection
# GUI without running the entire DEVise application.

############################################################

# $Id$

# $Log$

############################################################

source util.tcl
source fonts.tcl

set curView "View 1"
global curView

wm geometry . 200x50

#TEMPTEMP set color
set backGround DarkOliveGreen
set foreGround white

tk_setPalette background $backGround foreground $foreGround \
    selectBackground $foreGround selectForeground $backGround
#. configure -bg $backGround


frame .mbar -relief raised -bd 2
pack .mbar -side top -fill x

menubutton .mbar.file -text File -menu .mbar.file.menu
menubutton .mbar.view -text View -menu .mbar.view.menu
pack .mbar.file .mbar.view -side left

menu .mbar.file.menu -tearoff false
.mbar.file.menu add command -label "Quit" -command { destroy . }

menu .mbar.view.menu -tearoff false
.mbar.view.menu add cascade -label "Fonts" -menu .mbar.view.menu.fontsmenu

menu .mbar.view.menu.fontsmenu -tearoff false
.mbar.view.menu.fontsmenu add command -label "Title" \
    -command { GetFont "title" }
.mbar.view.menu.fontsmenu add command -label "X Axis" \
    -command { GetFont "x axis" }
.mbar.view.menu.fontsmenu add  command -label "Y Axis"\
    -command { GetFont "y axis" }
# TEMPTEMP? use this to bring up mapping window
.mbar.view.menu.fontsmenu add  command -label "Data Labels"

#============================================================================

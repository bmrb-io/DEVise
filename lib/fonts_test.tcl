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
# Revision 1.1  1996/12/15 20:23:53  wenger
# Added view name to mapping editor window; added first version of font
# selection GUI (currently disabled).
#

############################################################

source util.tcl
source fonts.tcl

set curView "View 1"
global curView

set "testFonts(title)" {0 12 0 0}
set "testFonts(x axis)" {0 12 0 0}
set "testFonts(y axis)" {0 12 0 0}
global testFonts

wm geometry . 200x50

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

##########################################################################

proc DEVise { command args } {
  global testFonts

  puts -nonewline "DEVise $command"
  foreach arg $args {
    puts -nonewline " $arg"
  }
  puts ""

  if { $command == "setFont" } {
    set view [lindex $args 0]
    set which [lindex $args 1]

    set family [lindex $args 2]
    set size [lindex $args 3]
    set bold [lindex $args 4]
    set italic [lindex $args 5]

    set "testFonts($which)" [list $family $size $bold $italic]
  }

  if { $command == "getFont" } {
    set view [lindex $args 0]
    set which [lindex $args 1]

    return $testFonts($which)
  }
}

#============================================================================

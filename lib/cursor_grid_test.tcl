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

# This module contains Tcl/Tk code that allows testing the cursor grid
# GUI without running the entire DEVise application.

############################################################

# $Id$

# $Log$

############################################################

source util.tcl
source cursor_grid.tcl

set useGrid 0
global useGrid

set gridX 1.0
global gridX

set gridY 2.0
global gridY

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
.mbar.view.menu add cascade -label "Cursors"  \
    -menu .mbar.view.menu.cursormenu

menu .mbar.view.menu.cursormenu -tearoff false
.mbar.view.menu.cursormenu add command -label "SetGrid" \
    -command { GetCursorGrid EddyMerckx }

##########################################################################

proc DEVise { command args } {
  global useGrid
  global gridX
  global gridY

  puts -nonewline "DEVise $command"
  foreach arg $args {
    puts -nonewline " $arg"
  }
  puts ""

  if { $command == "getCursorGrid" } {
    return [list $useGrid $gridX $gridY]
  }

  if { $command == "setCursorGrid" } {
    set useGrid [lindex $args 1]
    set gridX [lindex $args 2]
    set gridY [lindex $args 3]
    return
  }
}

#============================================================================

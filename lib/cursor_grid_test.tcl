#!/s/std/bin/wish -f
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

# This module contains Tcl/Tk code that allows testing the cursor grid
# GUI without running the entire DEVise application.

############################################################

# $Id$

# $Log$
# Revision 1.1  1997/06/09 14:47:09  wenger
# Added cursor grid; fixed bug 187; a few minor cleanups.
#

############################################################

source util.tcl
source cursor_grid.tcl

set useGrid 0
global useGrid

set gridX 1.0
global gridX

set gridY 2.0
global gridY

set edgeGrid 0
global edgeGrid

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
  global edgeGrid

  puts -nonewline "DEVise $command"
  foreach arg $args {
    puts -nonewline " $arg"
  }
  puts ""

  if { $command == "getCursorGrid" } {
    return [list $useGrid $gridX $gridY $edgeGrid]
  }

  if { $command == "setCursorGrid" } {
    set useGrid [lindex $args 1]
    set gridX [lindex $args 2]
    set gridY [lindex $args 3]
    set edgeGrid [lindex $args 4]
    return
  }
}

#============================================================================

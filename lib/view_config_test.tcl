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

# This module contains Tcl/Tk code that allows testing the view configuration
# GUI without running the entire DEVise application.

############################################################

# $Id$

# $Log$
# Revision 1.1  1997/05/30 15:41:38  wenger
# Most of the way to user-configurable '4', '5', and '6' keys -- committing
# this stuff now so it doesn't get mixed up with special stuff for printing
# Mitre demo.
#

############################################################

source util.tcl
source view_config.tcl

set curView "View 1"
global curView

global viewHome
set viewHome {1 0.0 5.0 -1.0 -2.0 100.0 10.0}

global viewHorPan
set viewHorPan {1 0.5 2}

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
.mbar.view.menu add cascade -label "Configuration"  \
    -menu .mbar.view.menu.configmenu

menu .mbar.view.menu.configmenu -tearoff false
.mbar.view.menu.configmenu add command -label "Home" \
    -command { GetViewHome }
.mbar.view.menu.configmenu add command -label "Horiz. Pan" \
    -command { GetViewHorPan }

##########################################################################

proc DEVise { command args } {
  global viewHome
  global viewHorPan

  puts -nonewline "DEVise $command"
  foreach arg $args {
    puts -nonewline " $arg"
  }
  puts ""

  if { $command == "viewSetHome" } {
    set viewHome [lrange $args 1 7]
    puts "DIAG viewHome = $viewHome"
    return
  }

  if { $command == "viewSetHorPan" } {
    set viewHorPan [lrange $args 1 3]
    puts "DIAG viewHorPan = $viewHorPan"
    return
  }

  if { $command == "viewGetHome" } {
    puts "DIAG viewHome = $viewHome"
    return $viewHome
  }

  if { $command == "viewGetHorPan" } {
    puts "DIAG viewHorPan = $viewHorPan"
    return $viewHorPan
  }

  if { $command == "getVisualFilters" } {
    return {{{9.00} {5.00} {20.00} {11.00} 0}}
  }
}

#============================================================================

# ========================================================================
# DEVise Data Visualization Software
# (c) Copyright 1992-1998
# By the DEVise Development Group
# Madison, Wisconsin
# All Rights Reserved.
# ========================================================================

# Under no circumstances is this software to be copied, distributed,
# or altered in any way without prior permission from the DEVise
# Development Group.

############################################################

# This module contains the GUI components related to
# symbol alignments.

############################################################

# $Id$

# $Log$
# Revision 1.2  1998/03/12 22:57:53  wenger
# Minor user interface improvements.
#
# Revision 1.1  1998/02/05 23:46:16  wenger
# Added view-level specification of symbol alignment, API commands, simple
# GUI for Sanjay.
#

############################################################

# Allow user to edit the symbol alignment for the selected
# view.
# No return value.
proc EditSymbolAlignment {} {
  global curView
  global align

  set align [DEVise viewGetAlign $curView]
  set align [ChooseSymbolAlignment $align]
}

#-----------------------------------------------------------

# GUI for user to choose alignment.
# Returns new alignment value.
proc ChooseSymbolAlignment {align} {
  global curView

  toplevel .chooseAlign
  wm title .chooseAlign "Choose Symbol Alignment"
  wm geometry .chooseAlign +200+200

  frame .chooseAlign.row1
  frame .chooseAlign.row2

  label .chooseAlign.lab -text "Alignment:"
  menubutton .chooseAlign.align -relief raised -bd 2 -text $align \
    -menu .chooseAlign.align.menu

  menu .chooseAlign.align.menu -tearoff false
  .chooseAlign.align.menu add command -label "0 (NorthWest)" \
    -command { .chooseAlign.align configure -text "0" }
  .chooseAlign.align.menu add command -label "1 (West)" \
    -command { .chooseAlign.align configure -text "1" }
  .chooseAlign.align.menu add command -label "2 (SouthWest)" \
    -command { .chooseAlign.align configure -text "2" }
  .chooseAlign.align.menu add command -label "3 (North)" \
    -command { .chooseAlign.align configure -text "3" }
  .chooseAlign.align.menu add command -label "4 (Center)" \
    -command { .chooseAlign.align configure -text "4" }
  .chooseAlign.align.menu add command -label "5 (South)" \
    -command { .chooseAlign.align configure -text "5" }
  .chooseAlign.align.menu add command -label "6 (NorthEast)" \
    -command { .chooseAlign.align configure -text "6" }
  .chooseAlign.align.menu add command -label "7 (East)" \
    -command { .chooseAlign.align configure -text "7" }
  .chooseAlign.align.menu add command -label "8 (SouthEast)" \
    -command { .chooseAlign.align configure -text "8" }

  button .chooseAlign.ok -text "OK" -width 10 -command {
    DEVise viewSetAlign $curView [lindex [.chooseAlign.align configure -text] 4]
    destroy .chooseAlign
  }
  button .chooseAlign.cancel -text "Cancel" -width 10 -command {
    destroy .chooseAlign
  }

  pack .chooseAlign.row1 -side top -padx 30 -pady 10
  pack .chooseAlign.row2 -side top -padx 30 -pady 10

  pack .chooseAlign.lab -in .chooseAlign.row1 -side left
  pack .chooseAlign.align -in .chooseAlign.row1 -side left
  pack .chooseAlign.ok -in .chooseAlign.row2 -side left
  pack .chooseAlign.cancel -in .chooseAlign.row2 -side left

  tkwait visibility .chooseAlign
  grab set .chooseAlign
  tkwait window .chooseAlign
}

#============================================================================

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
# string tables.

############################################################

# $Id$

# $Log$

############################################################

# GUI for user to edit string table assignments for selected view.
proc EditStringTables {} {
  global curView
  global viewStr_xTable viewStr_yTable viewStr_zTable viewStr_genTable

  GetStringTables

  toplevel .assignStrings
  wm title .assignStrings "Assign String Tables"
  wm geometry .assignStrings +200+200

  frame .assignStrings.row1
  frame .assignStrings.row2
  frame .assignStrings.row3
  frame .assignStrings.row4
  frame .assignStrings.row5

  label .assignStrings.xLabel -text "X Axis:"
  entry .assignStrings.xEntry -width 20 -relief sunken -bd 2 \
    -textvariable viewStr_xTable
  label .assignStrings.yLabel -text "Y Axis:"
  entry .assignStrings.yEntry -width 20 -relief sunken -bd 2 \
    -textvariable viewStr_yTable
  label .assignStrings.zLabel -text "Z Axis:"
  entry .assignStrings.zEntry -width 20 -relief sunken -bd 2 \
    -textvariable viewStr_zTable
  label .assignStrings.genLabel -text "Other:"
  entry .assignStrings.genEntry -width 20 -relief sunken -bd 2 \
    -textvariable viewStr_genTable

  button .assignStrings.ok -text "OK" -width 10 -command {
    SetStringTables
    destroy .assignStrings
  }
  button .assignStrings.cancel -text "Cancel" -width 10 -command {
    destroy .assignStrings
  }

  pack .assignStrings.row1 -side top -padx 30 -pady 10
  pack .assignStrings.row2 -side top -padx 30 -pady 10
  pack .assignStrings.row3 -side top -padx 30 -pady 10
  pack .assignStrings.row4 -side top -padx 30 -pady 10
  pack .assignStrings.row5 -side top -padx 30 -pady 10

  pack .assignStrings.xLabel .assignStrings.xEntry -in .assignStrings.row1 \
    -side left
  pack .assignStrings.yLabel .assignStrings.yEntry -in .assignStrings.row2 \
    -side left
  pack .assignStrings.zLabel .assignStrings.zEntry -in .assignStrings.row3 \
    -side left
  pack .assignStrings.genLabel .assignStrings.genEntry \
    -in .assignStrings.row4 -side left
  pack .assignStrings.ok -in .assignStrings.row5 -side left
  pack .assignStrings.cancel -in .assignStrings.row5 -side left -padx 5

  tkwait visibility .assignStrings
  grab set .assignStrings
  tkwait window .assignStrings
}

#-----------------------------------------------------------

proc GetStringTables {} {
  global curView
  global viewStr_xTable viewStr_yTable viewStr_zTable view_Str_genTable

  set viewStr_xTable [DEVise viewGetStringTable $curView "x"]
  set viewStr_yTable [DEVise viewGetStringTable $curView "y"]
  set viewStr_zTable [DEVise viewGetStringTable $curView "z"]
  set viewStr_genTable [DEVise viewGetStringTable $curView "gen"]
}

#-----------------------------------------------------------

proc SetStringTables {} {
  global curView
  global viewStr_xTable viewStr_yTable viewStr_zTable viewStr_genTable

  DEVise viewSetStringTable $curView "x" $viewStr_xTable
  DEVise viewSetStringTable $curView "y" $viewStr_yTable
  DEVise viewSetStringTable $curView "z" $viewStr_zTable
  DEVise viewSetStringTable $curView "gen" $viewStr_genTable
}

#============================================================================

#========================================================================
#  DEVise Data Visualization Software
#  (c) Copyright 1992-1997
#  By the DEVise Development Group
#  Madison, Wisconsin
#  All Rights Reserved.
#  ========================================================================
#
#  Under no circumstances is this software to be copied, distributed,
#  or altered in any way without prior permission from the DEVise
#  Development Group.

############################################################

#  GUI for 'GData to socket'.

############################################################

#	$Id$	

#	$Log$

############################################################

proc SaveGData {} {
  global curView
  global gdsFile gdsSeparator

  # If this window already exists, raise it to the top and return.
  if {[WindowVisible .saveGData]} {
    return
  }

  set viewGDS [DEVise getViewGDS $curView]
  set gdsFile [lindex $viewGDS 3]
  set gdsSeparator [lindex $viewGDS 5]

  # Create the top level widget and the frames we'll later use for
  # positioning.
  toplevel .saveGData
  wm title .saveGData "Save View GData"

  frame .saveGData.row1
  frame .saveGData.row2
  frame .saveGData.row3

  # Create the various widgets.
  button .saveGData.ok -text "OK" -width 10 \
    -command {DoSaveGData; destroy .saveGData}
  button .saveGData.cancel -text "Cancel" -width 10 \
    -command "destroy .saveGData"

  label .saveGData.fileLab -text "File name:"
  entry .saveGData.fileEnt -width 20 -relief sunken -textvariable gdsFile
  button .saveGData.fileBut -text "Select..." -command {
    global fsBox datadir
    set fsBox(path) $datadir
    set fsBox(pattern) *
    set gdsFile [FSBox "Select GData file"]
  }

  label .saveGData.sepLab -text "Separator char.:"
  entry .saveGData.sepEnt -width 5 -relief sunken -textvariable gdsSeparator
  # Allow tab for separator character.
  bind .saveGData.sepEnt <Tab> { set gdsSeparator "\	" }

  # Pack the widgets into the frames.
  pack .saveGData.row1 -side bottom -pady 4m
  pack .saveGData.row2 -side top -pady 3m
  pack .saveGData.row3 -side top -pady 3m

  pack .saveGData.ok .saveGData.cancel -in .saveGData.row1 \
    -side left -padx 3m
  pack .saveGData.fileLab .saveGData.fileEnt .saveGData.fileBut \
    -in .saveGData.row2 -side left -padx 3m
  pack .saveGData.sepLab .saveGData.sepEnt -in .saveGData.row3 -side left \
    -padx 3m

  # Wait for the user to make a selection from this window.
  tkwait visibility .saveGData
  grab set .saveGData
  tkwait window .saveGData
}

proc DoSaveGData {} {
  global curView
  global gdsFile gdsSeparator

  DEVise setViewGDS $curView 1 1 0 $gdsFile 1 $gdsSeparator 
  DEVise waitForQueries
  DEVise setViewGDS $curView 1 0 0 $gdsFile 1 $gdsSeparator 
}

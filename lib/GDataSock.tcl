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
#	Revision 1.2  1999/05/17 18:38:07  wenger
#	Views now have GData sending configuration that is only employed when
#	connecting to the JavaScreen (eliminates the need for the current kludgey
#	setup to send GData to the JS).
#	
#	Revision 1.1  1997/11/24 16:22:37  wenger
#	Added GUI for saving GData; turning on GData to socket now forces
#	redraw of view; GData to socket params now saved in session files;
#	improvement to waitForQueries command.
#

############################################################

proc SaveGData {} {
  global curView
  global gdsFile gdsSeparator gdsRgb

  # If this window already exists, raise it to the top and return.
  if {[WindowVisible .saveGData]} {
    return
  }

  set viewGDS [DEVise getViewGDS $curView]
  set gdsFile [lindex $viewGDS 3]
  set gdsSeparator [lindex $viewGDS 5]
  set gdsRgb [lindex $viewGDS 6]

  # Create the top level widget and the frames we'll later use for
  # positioning.
  toplevel .saveGData
  wm title .saveGData "Save View GData"

  frame .saveGData.row1
  frame .saveGData.row2
  frame .saveGData.row3
  frame .saveGData.row4

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

  checkbutton .saveGData.rgb -variable gdsRgb -relief raised -bd 2 \
    -text "Convert colors to RGB" -padx 2m -pady 1m

  # Pack the widgets into the frames.
  pack .saveGData.row1 -side bottom -pady 4m
  pack .saveGData.row2 -side top -pady 3m
  pack .saveGData.row3 -side top -pady 3m
  pack .saveGData.row4 -side top -pady 3m

  pack .saveGData.ok .saveGData.cancel -in .saveGData.row1 \
    -side left -padx 3m
  pack .saveGData.fileLab .saveGData.fileEnt .saveGData.fileBut \
    -in .saveGData.row2 -side left -padx 3m
  pack .saveGData.sepLab .saveGData.sepEnt -in .saveGData.row3 -side left \
    -padx 3m
  pack .saveGData.rgb -in .saveGData.row4 -side left -padx 3m

  # Wait for the user to make a selection from this window.
  tkwait visibility .saveGData
  grab set .saveGData
  tkwait window .saveGData
}

############################################################

proc DoSaveGData {} {
  global curView
  global gdsFile gdsSeparator gdsRgb

  DEVise setViewGDS $curView 1 1 0 $gdsFile 1 $gdsSeparator $gdsRgb
  DEVise waitForQueries
  DEVise setViewGDS $curView 1 0 0 $gdsFile 1 $gdsSeparator $gdsRgb
}

############################################################

proc EditJSGData {} {
  global curView
  global jsGdsDraw jsGdsSend
  global jsGdsFile jsGdsSeparator jsGdsRgb

  # If this window already exists, raise it to the top and return.
  if {[WindowVisible .jsGData]} {
    return
  }

  set viewGDS [DEVise viewGetJSSendP $curView]
  set jsGdsDraw [lindex $viewGDS 0]
  set jsGdsSend [lindex $viewGDS 1]
  set jsGdsFile [lindex $viewGDS 3]
  set jsGdsSeparator [lindex $viewGDS 5]
  set jsGdsRgb [lindex $viewGDS 6]

  # Create the top level widget and the frames we'll later use for
  # positioning.
  toplevel .jsGData
  wm title .jsGData "JavaScreen GData Configuration"

  frame .jsGData.row1
  frame .jsGData.row2
  frame .jsGData.row3
  frame .jsGData.row4
  frame .jsGData.row5

  # Create the various widgets.
  button .jsGData.ok -text "OK" -width 10 \
    -command {SetJSGData; destroy .jsGData}
  button .jsGData.cancel -text "Cancel" -width 10 \
    -command "destroy .jsGData"

  checkbutton .jsGData.draw -variable jsGdsDraw -relief raised -bd 2 \
    -text "Draw to Screen" -padx 2m -pady 1m
  checkbutton .jsGData.send -variable jsGdsSend -relief raised -bd 2 \
    -text "Send to Socket/File" -padx 2m -pady 1m

  label .jsGData.fileLab -text "File name:"
  entry .jsGData.fileEnt -width 20 -relief sunken -textvariable jsGdsFile
  button .jsGData.fileBut -text "Select..." -command {
    global fsBox datadir
    set fsBox(path) $datadir
    set fsBox(pattern) *
    set jsGdsFile [FSBox "Select GData file"]
  }

  label .jsGData.sepLab -text "Separator char.:"
  entry .jsGData.sepEnt -width 5 -relief sunken -textvariable jsGdsSeparator
  # Allow tab for separator character.
  bind .jsGData.sepEnt <Tab> { set jsGdsSeparator "\	" }

  checkbutton .jsGData.rgb -variable jsGdsRgb -relief raised -bd 2 \
    -text "Convert colors to RGB" -padx 2m -pady 1m

  # Pack the widgets into the frames.
  pack .jsGData.row1 -side bottom -pady 4m
  pack .jsGData.row4 -side top -pady 3m
  pack .jsGData.row2 -side top -pady 3m
  pack .jsGData.row3 -side top -pady 3m
  pack .jsGData.row5 -side top -pady 3m

  pack .jsGData.ok .jsGData.cancel -in .jsGData.row1 \
    -side left -padx 3m
  pack .jsGData.draw .jsGData.send -in .jsGData.row4 -side left -padx 3m
  pack .jsGData.fileLab .jsGData.fileEnt .jsGData.fileBut \
    -in .jsGData.row2 -side left -padx 3m
  pack .jsGData.sepLab .jsGData.sepEnt -in .jsGData.row3 -side left \
    -padx 3m
  pack .jsGData.rgb -in .jsGData.row5 -side left -padx 3m

  # Wait for the user to make a selection from this window.
  tkwait visibility .jsGData
  grab set .jsGData
  tkwait window .jsGData
}

############################################################

proc SetJSGData {} {
  global curView
  global jsGdsDraw jsGdsSend
  global jsGdsFile jsGdsSeparator jsGdsRgb

  DEVise viewSetJSSendP $curView $jsGdsDraw $jsGdsSend 0 $jsGdsFile 1 \
    $jsGdsSeparator $jsGdsRgb
}

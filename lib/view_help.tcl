# ========================================================================
# DEVise Data Visualization Software
# (c) Copyright 2000
# By the DEVise Development Group
# Madison, Wisconsin
# All Rights Reserved.
# ========================================================================

# Under no circumstances is this software to be copied, distributed,
# or altered in any way without prior permission from the DEVise
# Development Group.

############################################################

# This module contains the code for reading, editing, and saving
# view help text.

############################################################

# $Id$

# $Log$

############################################################

set viewHelpStr ""

############################################################
# Allow the user to view and edit the view help.

proc EditViewHelp {} {
  global viewHelpStr
  global curView

  set viewHelpStr [DEVise getViewHelp $curView]

  if {![WindowExists .viewHelpEdit]} {
    # Window for view help text.
    toplevel .viewHelpEdit
    wm title .viewHelpEdit "View Help"
    wm geometry .viewHelpEdit 400x200

    # Text widget and scrollbar for view help text.
    text .viewHelpEdit.text -relief raised -bd 2 \
      -yscrollcommand ".viewHelpEdit.scroll set"
    scrollbar .viewHelpEdit.scroll -command ".viewHelpEdit.text yview"

    # Apply, Clear, and Close buttons
    frame .viewHelpEdit.butFrame
    button .viewHelpEdit.apply -text "OK" -command {ApplyViewHelp;
      CloseHelpView}
    button .viewHelpEdit.clear -text "Clear" -command {ClearViewHelp}
    button .viewHelpEdit.close -text "Cancel" -command {CloseHelpView}

    # Put everything into place.
    pack .viewHelpEdit.butFrame -side bottom
    pack .viewHelpEdit.apply -in .viewHelpEdit.butFrame -side left -padx 5m
    pack .viewHelpEdit.clear -in .viewHelpEdit.butFrame -side left -padx 5m
    pack .viewHelpEdit.close -in .viewHelpEdit.butFrame -side right -padx 5m
    pack .viewHelpEdit.scroll -side right -fill y
    pack .viewHelpEdit.text -side left
  }

  UpdateHelpView

  tkwait visibility .viewHelpEdit
  grab set .viewHelpEdit
  tkwait window .viewHelpEdit
}

############################################################
# Update the display of the view help.

proc UpdateHelpView {} {
  global viewHelpStr

  if {[WindowExists .viewHelpEdit]} {
    # Avoid unnecessary changes to the window.
    set tmpHelp [.viewHelpEdit.text get 1.0 end]
    if {$tmpHelp != $viewHelpStr} {
      .viewHelpEdit.text delete 1.0 end
      .viewHelpEdit.text insert end $viewHelpStr
    }
  }
}

############################################################
# Clear the view help.

proc ClearViewHelp {} {
  global viewHelpStr

  set viewHelpStr ""

  UpdateHelpView
}

############################################################
# Apply the current view help text to the current view.

proc ApplyViewHelp {} {
  global viewHelpStr
  global curView

  set viewHelpStr [.viewHelpEdit.text get 1.0 end]

  # Stupid text widget always adds a newline -- get rid of it.
  set viewHelpStr [string trimright $viewHelpStr]

  DEVise setViewHelp $curView $viewHelpStr
}

############################################################
# Close the view help dialog.

proc CloseHelpView {} {
  destroy .viewHelpEdit
}

#============================================================================

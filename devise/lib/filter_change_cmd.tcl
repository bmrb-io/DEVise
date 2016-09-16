#TEMP -- get rid of all Desc...
# ========================================================================
# DEVise Data Visualization Software
# (c) Copyright 2009
# By the DEVise Development Group
# Madison, Wisconsin
# All Rights Reserved.
# ========================================================================

# Under no circumstances is this software to be copied, distributed,
# or altered in any way without prior permission from the DEVise
# Development Group.

############################################################

# This module contains the code for reading, editing, and saving
# view filter change commands.

############################################################

# $Id$

# $Log$

############################################################
# Clear the session description.

proc ClearFilterChangeCmd {} {
  UpdateFilterChangeCmdView ""
}

############################################################
# Allow the user to view and edit the filter change commands
#TEMP -- add something about enclosing individual commands in {}

proc EditFilterChangeCmd {} {
  if {![WindowExists .filterChangeCmds]} {
    # Window for session description.
    toplevel .filterChangeCmds
    wm title .filterChangeCmds "Filter Change Commands"
    wm geometry .filterChangeCmds 400x200

    # Reminder to TEMPTEMP
    label .filterChangeCmds.note -text "Enclose each command in {}"

    # Text widget and scrollbar for session description.
    text .filterChangeCmds.text -relief raised -bd 2 \
      -yscrollcommand ".filterChangeCmds.scroll set"
    scrollbar .filterChangeCmds.scroll -command ".filterChangeCmds.text yview"

    # OK, Clear, and Close buttons
    frame .filterChangeCmds.butFrame
    button .filterChangeCmds.apply -text "OK" -command {ApplyFilterChangeCmdView; CloseFilterChangeCmdView}
    button .filterChangeCmds.clear -text "Clear" -command {ClearFilterChangeCmd}
    button .filterChangeCmds.close -text "Close" -command {CloseFilterChangeCmdView}

    # Put everything into place.
    pack .filterChangeCmds.butFrame -side bottom
    pack .filterChangeCmds.apply -in .filterChangeCmds.butFrame -side left -padx 5m
    pack .filterChangeCmds.clear -in .filterChangeCmds.butFrame -side left -padx 5m
    pack .filterChangeCmds.close -in .filterChangeCmds.butFrame -side right -padx 5m
    pack .filterChangeCmds.note -in .filterChangeCmds.butFrame -side bottom -pady 5m
    pack .filterChangeCmds.scroll -side right -fill y
    pack .filterChangeCmds.text -side left
  }

  UpdateFilterChangeCmdView [DEVise getSessionDesc]

  tkwait visibility .filterChangeCmds
  grab set .filterChangeCmds
  tkwait window .filterChangeCmds
}

############################################################
# Update the view of the session description.

proc UpdateFilterChangeCmdView {description} {
  if {[WindowExists .filterChangeCmds]} {
    # Avoid unnecessary changes to the window.
    set tmpDesc [.filterChangeCmds.text get 1.0 end]
    if {$tmpDesc != $description} {
      .filterChangeCmds.text delete 1.0 end
      .filterChangeCmds.text insert end $description
    }
  }
}

############################################################
# Apply the current description in the window to the session.

proc ApplyFilterChangeCmdView {} {
  set description [.filterChangeCmds.text get 1.0 end]

  # Stupid text widget always adds a newline -- get rid of it.
  set description [string trimright $description]

  DEVise setSessionDesc $description
}

############################################################
# Close the view description window.

proc CloseFilterChangeCmdView {} {
  destroy .filterChangeCmds
}

#============================================================================

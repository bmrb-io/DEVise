# ========================================================================
# DEVise Data Visualization Software
# (c) Copyright 1992-1997
# By the DEVise Development Group
# Madison, Wisconsin
# All Rights Reserved.
# ========================================================================

# Under no circumstances is this software to be copied, distributed,
# or altered in any way without prior permission from the DEVise
# Development Group.

############################################################

# This module contains the code for reading, editing, and saving
# session descriptions.

############################################################

# $Id$

# $Log$
# Revision 1.1  1997/04/30 18:27:22  wenger
# Added session text description capability.
#

############################################################

set sessionDescription ""

############################################################
# Clear the session description.

proc ClearDescription {} {
  global sessionDescription

  set sessionDescription ""

  UpdateDescView
}

############################################################
# Set the session description to the given string.

proc SetDescription {description} {
  global sessionDescription

  set sessionDescription $description

  UpdateDescView
}

############################################################
# Allow the user to view and edit the session description.

proc ViewDescription {} {
  global sessionDescription

  if {![WindowExists .sessDesc]} {
    # Window for session description.
    toplevel .sessDesc
    wm title .sessDesc "Session Description"
    wm geometry .sessDesc 400x200

    # Text widget and scrollbar for session description.
    text .sessDesc.text -relief raised -bd 2 \
      -yscrollcommand ".sessDesc.scroll set"
    scrollbar .sessDesc.scroll -command ".sessDesc.text yview"

    # Apply, Clear, and Close buttons
    frame .sessDesc.butFrame
    button .sessDesc.apply -text "Apply" -command {ApplyDescView}
    button .sessDesc.clear -text "Clear" -command {ClearDescription}
    button .sessDesc.close -text "Close" -command {CloseDescView}

    # Put everything into place.
    pack .sessDesc.butFrame -side bottom
    pack .sessDesc.apply -in .sessDesc.butFrame -side left -padx 5m
    pack .sessDesc.clear -in .sessDesc.butFrame -side left -padx 5m
    pack .sessDesc.close -in .sessDesc.butFrame -side right -padx 5m
    pack .sessDesc.scroll -side right -fill y
    pack .sessDesc.text -side left
  }

  UpdateDescView
}

############################################################
# Save the session description to the given fileId.
proc SaveDescription {fileId asBatchScript} {
  global sessionDescription

  if {$asBatchScript} {
    return
  }

  puts $fileId ""
  puts $fileId "# Set session description"
  puts $fileId "SetDescription \"$sessionDescription\""
}

############################################################
# Update the view of the session description.

proc UpdateDescView {} {
  global sessionDescription

  if {[WindowExists .sessDesc]} {
    # Avoid unnecessary changes to the window.
    set tmpDesc [.sessDesc.text get 1.0 end]
    if {$tmpDesc != $sessionDescription} {
      .sessDesc.text delete 1.0 end
      .sessDesc.text insert end $sessionDescription
    }
  }
}

############################################################
# Apply the current description in the window to the session.

proc ApplyDescView {} {
  global sessionDescription

  set sessionDescription [.sessDesc.text get 1.0 end]
}

############################################################
# Close the view description window.

proc CloseDescView {} {
  destroy .sessDesc
}

#============================================================================

# ========================================================================
# DEVise Data Visualization Software
# (c) Copyright 1992-2000
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
# Revision 1.2  1997/06/18 21:06:40  wenger
# Fixed problems saving to batch scripts.
#
# Revision 1.1  1997/04/30 18:27:22  wenger
# Added session text description capability.
#

############################################################
# Clear the session description.

proc ClearDescription {} {
  UpdateDescView ""
}

############################################################
# Allow the user to view and edit the session description.

proc EditDescription {} {
  if {![WindowExists .sessDesc]} {
    # Window for session description.
    toplevel .sessDesc
    wm title .sessDesc "Session Description"
    wm geometry .sessDesc 400x200

    # Text widget and scrollbar for session description.
    text .sessDesc.text -relief raised -bd 2 \
      -yscrollcommand ".sessDesc.scroll set"
    scrollbar .sessDesc.scroll -command ".sessDesc.text yview"

    # OK, Clear, and Close buttons
    frame .sessDesc.butFrame
    button .sessDesc.apply -text "OK" -command {ApplyDescView; CloseDescView}
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

  UpdateDescView [DEVise getSessionDesc]

  tkwait visibility .sessDesc
  grab set .sessDesc
  tkwait window .sessDesc
}

############################################################
# Update the view of the session description.

proc UpdateDescView {description} {
  if {[WindowExists .sessDesc]} {
    # Avoid unnecessary changes to the window.
    set tmpDesc [.sessDesc.text get 1.0 end]
    if {$tmpDesc != $description} {
      .sessDesc.text delete 1.0 end
      .sessDesc.text insert end $description
    }
  }
}

############################################################
# Apply the current description in the window to the session.

proc ApplyDescView {} {
  set description [.sessDesc.text get 1.0 end]

  # Stupid text widget always adds a newline -- get rid of it.
  set description [string trimright $description]

  DEVise setSessionDesc $description
}

############################################################
# Close the view description window.

proc CloseDescView {} {
  destroy .sessDesc
}

#============================================================================

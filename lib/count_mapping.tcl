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

# This module contains the GUI components related to count mappings.

############################################################

# $Id$

# $Log$

############################################################

proc EditCountMapping {} {
    global curView

    # If this window already exists, raise it to the top and return.
    if {[WindowVisible .editCountMapping]} {
	return
    }

    # Create the top level widget and the frames we'll later use for
    # positioning.
    toplevel .editCountMapping
    wm title .editCountMapping "Count Mapping ($curView)"

    frame .editCountMapping.row1
    frame .editCountMapping.row2
    frame .editCountMapping.row3
    frame .editCountMapping.row4

    # Create the various widgets.
    button .editCountMapping.ok -text "OK" -width 10 \
      -command {SetCountMapping; destroy .editCountMapping}
    button .editCountMapping.cancel -text "Cancel" -width 10 \
      -command "destroy .editCountMapping"

    # Radio buttons to enable/disable count mappings.
    global countMapEnabled
    radiobutton .editCountMapping.yes -text "Enable" \
      -variable countMapEnabled -value 1 -command "ToggleCountMapGUI"
    radiobutton .editCountMapping.no -text "Disable" \
      -variable countMapEnabled -value 0 -command "ToggleCountMapGUI"

    # Button and menu for count attribute.
    label .editCountMapping.countLab -text "Attribute to count:"
    menubutton .editCountMapping.countAttr -relief raised -bd 2 \
      -menu .editCountMapping.countAttr.menu

    menu .editCountMapping.countAttr.menu -tearoff false
    .editCountMapping.countAttr.menu add command -label "X" \
      -command { .editCountMapping.countAttr configure -text "X"; \
      .editCountMapping.putAttr configure -text "Y" }
    .editCountMapping.countAttr.menu add command -label "Y" \
      -command { .editCountMapping.countAttr configure -text "Y"; \
      .editCountMapping.putAttr configure -text "X" }

    # Button and menu for put attribute.
    label .editCountMapping.putLab -text "Attribute to get count:"
    menubutton .editCountMapping.putAttr -relief raised -bd 2 \
      -menu .editCountMapping.putAttr.menu

    menu .editCountMapping.putAttr.menu -tearoff false
    .editCountMapping.putAttr.menu add command -label "X" \
      -command { .editCountMapping.putAttr configure -text "X";
      .editCountMapping.countAttr configure -text "Y" }
    .editCountMapping.putAttr.menu add command -label "Y" \
      -command { .editCountMapping.putAttr configure -text "Y";
      .editCountMapping.countAttr configure -text "X" }

    # These frames are for spacing only.
    frame .editCountMapping.row2a -width 80m -height 6m
    frame .editCountMapping.row4a -width 10m -height 4m

    # Pack some widgets into the frames.  Other widges will be packed in
    # the ToggleCountMapGUI procedure.
    pack .editCountMapping.row1 -side bottom -pady 4m
    pack .editCountMapping.row2 -side top
    pack .editCountMapping.row2a -side top

    pack .editCountMapping.ok .editCountMapping.cancel -in .editCountMapping.row1 \
      -side left -padx 3m
    pack .editCountMapping.no .editCountMapping.yes -in .editCountMapping.row2 \
      -side top -pady 1m
    pack .editCountMapping.countLab .editCountMapping.countAttr \
      -in .editCountMapping.row3 -side left
    pack .editCountMapping.putLab .editCountMapping.putAttr -in .editCountMapping.row4 \
      -side left

    # Get the current values from the cursor and set the GUI accordingly.
    # Returns: <enabled> <countAttr> <putAttr>
    set countMapping [DEVise getCountMapping $curView]
    set countMapEnabled [lindex $countMapping 0]
    .editCountMapping.countAttr configure -text [lindex $countMapping 1]
    .editCountMapping.putAttr configure -text [lindex $countMapping 2]
    ToggleCountMapGUI

    # Wait for the user to make a selection from this window.
    tkwait visibility .editCountMapping
    grab set .editCountMapping
    tkwait window .editCountMapping
}

##########################################################################

proc ToggleCountMapGUI {} {
    global countMapEnabled

    if {$countMapEnabled} {
      pack .editCountMapping.row3 -after .editCountMapping.row2a -pady 1m
      pack .editCountMapping.row4 -after .editCountMapping.row3 -pady 1m
      pack .editCountMapping.row4a -after .editCountMapping.row4 -pady 1m
    } else {
      pack forget .editCountMapping.row3
      pack forget .editCountMapping.row4
      pack forget .editCountMapping.row4a
    }
}

##########################################################################

proc SetCountMapping {} {
    global curView
    global countMapEnabled

    set countMapCountAttr [lindex [.editCountMapping.countAttr \
      configure -text] 4]
    set countMapPutAttr [lindex [.editCountMapping.putAttr \
      configure -text] 4]

    # Arguments: <viewName> <enabled> <countAttr> <putAttr>
    DEVise setCountMapping $curView $countMapEnabled $countMapCountAttr \
      $countMapPutAttr
}

#============================================================================

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

# This module contains the GUI components related to selecting a font.

############################################################

# $Id$

# $Log$

############################################################

proc GetFont { which } {
puts "DIAG GetFont $which"
    global curView


    # If this window already exists, raise it to the top and return.
    if {[WindowVisible .getFont]} {
	return
    }


    # Create the top level widget and the frames we'll later use for
    # positioning.
    toplevel .getFont
    wm title .getFont "Choose Font"

    frame .getFont.row1
    frame .getFont.row2
    frame .getFont.row3
    frame .getFont.row4

    frame .getFont.row2.1
    frame .getFont.row2.2


    # Create the various objects that will be shown in the window.
    label .getFont.label -text "View '$curView' $which font"

    label .getFont.famlabel -text "Family:"
    menubutton .getFont.family -relief raised -bd 2 -text "0" \
      -menu .getFont.family.menu

    label .getFont.sizelabel -text "Size:"
    menubutton .getFont.size -relief raised -bd 2 -text "8" \
      -menu .getFont.size.menu

    checkbutton .getFont.bold -relief raised -bd 2 -text "Bold" \
      -padx 2m -pady 1m
    checkbutton .getFont.italic -relief raised -bd 2 -text "Italic" \
      -padx 2m -pady 1m

    button .getFont.apply -text "Apply" -command SetFont
    button .getFont.ok -text "OK" -command { SetFont; destroy .getFont }
    button .getFont.cancel -text "Cancel" -command "destroy .getFont"


    # Pack everything into the frames.
    pack .getFont.row1 -side top -pady 3m
    pack .getFont.row2 -side top -pady 3m
    pack .getFont.row2.1 .getFont.row2.2 -side left -padx 3m
    pack .getFont.row3 -side top -pady 3m
    pack .getFont.row4 -side top -pady 3m

    pack .getFont.label -in .getFont.row1 -side top
    pack .getFont.famlabel .getFont.family -in .getFont.row2.1 -side left
    pack .getFont.sizelabel .getFont.size -in .getFont.row2.2 -side left
    pack .getFont.bold .getFont.italic -in .getFont.row3 -side left -padx 3m
    pack .getFont.apply .getFont.ok .getFont.cancel -in .getFont.row4 \
      -side left -padx 3m


    # Create the menus.
#TEMPTEMP -- need to get number of fonts from Devise server
    menu .getFont.family.menu -tearoff false
    .getFont.family.menu add command -label "0" \
      -command { .getFont.family configure -text "0" }
    .getFont.family.menu add command -label "1" \
      -command { .getFont.family configure -text "1" }
    .getFont.family.menu add command -label "2" \
      -command { .getFont.family configure -text "2" }
    .getFont.family.menu add command -label "3" \
      -command { .getFont.family configure -text "3" }
    .getFont.family.menu add command -label "4" \
      -command { .getFont.family configure -text "4" }

#TEMPTEMP -- need to get available sizes from Devise server
    menu .getFont.size.menu -tearoff false
    .getFont.size.menu add command -label "8" \
      -command { .getFont.size configure -text "8" }
    .getFont.size.menu add command -label "10" \
      -command { .getFont.size configure -text "10" }
    .getFont.size.menu add command -label "12" \
      -command { .getFont.size configure -text "12" }
    .getFont.size.menu add command -label "15" \
      -command { .getFont.size configure -text "15" }


    # Wait for the user to make a selection from this window.
    tkwait visibility .getFont
    grab set .getFont
    tkwait window .getFont
}

##########################################################################

proc SetFont {} {
#TEMPTEMP -- read the values of the different buttons, etc., and send
#a DEVise command
    puts "DIAG SetFont"
}


#============================================================================

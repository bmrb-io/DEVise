# ========================================================================
# DEVise Data Visualization Software
# (c) Copyright 1992-1999
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
# Revision 1.5  1999/05/14 14:00:57  wenger
# User can now control data font family, weight, and slant, on a per-view
# basis.
#
# Revision 1.4  1999/04/21 20:35:50  wenger
# Improved interface for changing fonts, titles, etc.:
# * Fonts can now be set on a window-wide basis.
# * Setting fonts, title, or axis date format in a piled view automatically
# changes all views in the pile accordingly.
#
# Revision 1.3  1997/04/09 18:24:45  wenger
# Fixed various makefiles (removed extra -gstabs flags, etc.);
# setup script now links client.tcl into all cslib directories;
# added 24 point font to available sizes in font selection GUI.
#
# Revision 1.2  1996/12/20 16:50:36  wenger
# Fonts for view label, x axis, and y axis can now be changed.
#
# Revision 1.1  1996/12/15 20:23:53  wenger
# Added view name to mapping editor window; added first version of font
# selection GUI (currently disabled).
#

############################################################

proc GetFont { which {isWindow 0} } {
    global curView
    global isBold isItalic


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


    set currentFont [DEVise getFont $curView $which]
    set family [lindex $currentFont 0]
    set size [lindex $currentFont 1]
    set isBold [lindex $currentFont 2]
    set isItalic [lindex $currentFont 3]


    # Create the various objects that will be shown in the window.
    label .getFont.label -text "View '$curView' $which font"

    label .getFont.famlabel -text "Family:"
    menubutton .getFont.family -relief raised -bd 2 -text $family \
      -menu .getFont.family.menu

    label .getFont.sizelabel -text "Size:"
    menubutton .getFont.size -relief raised -bd 2 -text $size \
      -menu .getFont.size.menu
    if {$which == "data"} {
        .getFont.size configure -state disabled
    }

    checkbutton .getFont.bold -variable isBold -relief raised -bd 2 \
      -text "Bold" -padx 2m -pady 1m
    checkbutton .getFont.italic -variable isItalic -relief raised -bd 2 \
      -text "Italic" -padx 2m -pady 1m

# Removed Apply button because view doesn't get repainted while dialog
# is visible anyhow (because of grab).  RKW 12/20/96.
#    button .getFont.apply -text "Apply" -width 10 \
#      -command "SetFont {$which} $isWindow"
    button .getFont.ok -text "OK" -width 10 \
      -command "SetFont {$which}  $isWindow; destroy .getFont"
    button .getFont.cancel -text "Cancel" -width 10 \
      -command "destroy .getFont"


    # Pack everything into the frames.
    pack .getFont.row1 -side top -pady 4m
    pack .getFont.row2 -side top -pady 1m
    pack .getFont.row2.1 .getFont.row2.2 -side left -padx 10m
    pack .getFont.row3 -side top -pady 1m
    pack .getFont.row4 -side top -pady 4m

    pack .getFont.label -in .getFont.row1 -side top
    pack .getFont.famlabel .getFont.family -in .getFont.row2.1 -side left
    pack .getFont.sizelabel .getFont.size -in .getFont.row2.2 -side left
    pack .getFont.bold .getFont.italic -in .getFont.row3 -side left -padx 10m
    pack .getFont.ok .getFont.cancel -in .getFont.row4 \
      -side left -padx 3m
#    pack .getFont.apply .getFont.ok .getFont.cancel -in .getFont.row4 \
#      -side left -padx 3m


    # Create the menus.
    # Note: we should eventually get the available font families from the
    # server.
    menu .getFont.family.menu -tearoff false
    .getFont.family.menu add command -label "0 (Courier)" \
      -command { .getFont.family configure -text "0" }
    .getFont.family.menu add command -label "1 (Times)" \
      -command { .getFont.family configure -text "1" }
    .getFont.family.menu add command -label "2 (Helvetica)" \
      -command { .getFont.family configure -text "2" }
    .getFont.family.menu add command -label "3 (Lucida)" \
      -command { .getFont.family configure -text "3" }
    .getFont.family.menu add command -label "4 (New Century Schoolbook)" \
      -command { .getFont.family configure -text "4" }

    # Note: we should eventually get information about the available font
    # sizes from the server.
    menu .getFont.size.menu -tearoff false
    .getFont.size.menu add command -label "8" \
      -command { .getFont.size configure -text "8" }
    .getFont.size.menu add command -label "10" \
      -command { .getFont.size configure -text "10" }
    .getFont.size.menu add command -label "12" \
      -command { .getFont.size configure -text "12" }
    .getFont.size.menu add command -label "14" \
      -command { .getFont.size configure -text "14" }
    .getFont.size.menu add command -label "18" \
      -command { .getFont.size configure -text "18" }
    .getFont.size.menu add command -label "24" \
      -command { .getFont.size configure -text "24" }


    # Wait for the user to make a selection from this window.
    tkwait visibility .getFont
    grab set .getFont
    tkwait window .getFont
}

##########################################################################

proc SetFont { which isWindow } {
    global curView
    global isBold isItalic

    set family [lindex [.getFont.family configure -text] 4]
    set size [lindex [.getFont.size configure -text] 4]

    if {$isWindow} {
        DEVise setFont [DEVise getViewWin $curView] $which $family $size \
	  $isBold $isItalic
    } else {
        DEVise setFont $curView $which $family $size $isBold $isItalic
    }
}

#============================================================================

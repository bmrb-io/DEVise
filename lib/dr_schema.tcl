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

# This module contains the DataReader schema GUI.

############################################################

# $Id$

# $Log$

############################################################

proc DrNewSchema {} {
    puts "DIAG DrNewSchema"
    DrSchemaWin

    #TEMPTEMP
    return "/p/devise/foo"
}

proc DrOpenSchema {schemaFile} {
    puts "DIAG DrOpenSchema"
    puts "DIAG schemaFile = $schemaFile"
    DrSchemaWin
}

proc DrSchemaWin {} {

    # Create the top level widget and the frames we'll later use for
    # positioning.
    toplevel .drSchemaWin
    wm title .drSchemaWin "DataReader Schema Description"

    frame .drSchemaWin.row1

    # Create the various widgets.
    button .drSchemaWin.save -text "Save" -width 10 \
      -command { DrWriteSchema; destroy .drSchemaWin }
    button .drSchemaWin.cancel -text "Cancel" -width 10 \
      -command "destroy .drSchemaWin"

    # Pack the widgets into the frames.
    pack .drSchemaWin.row1 -side bottom -pady 4m

    pack .drSchemaWin.save .drSchemaWin.cancel -in .drSchemaWin.row1 \
      -side left -padx 3m

    # Wait for the user to make a selection from this window.
    tkwait visibility .drSchemaWin
    grab set .drSchemaWin
    tkwait window .drSchemaWin
}

proc DrReadSchema {} {
    puts "DIAG DrReadSchema"
}

proc DrWriteSchema {} {
    puts "DIAG DrWriteSchema"
}





proc GetCursorGrid {cursorName} {
    global curView
    global gridCursorName
    set gridCursorName $cursorName

    # If this window already exists, raise it to the top and return.
    if {[WindowVisible .getCursorGrid]} {
	return
    }

    # Create the top level widget and the frames we'll later use for
    # positioning.
    toplevel .getCursorGrid
    wm title .getCursorGrid "Set Cursor Grid ($cursorName)"

    frame .getCursorGrid.row1
    frame .getCursorGrid.row2
    frame .getCursorGrid.row3
    frame .getCursorGrid.row4

    # Create the various widgets.
    button .getCursorGrid.ok -text "OK" -width 10 \
      -command {SetCursorGrid; destroy .getCursorGrid}
    button .getCursorGrid.cancel -text "Cancel" -width 10 \
      -command "destroy .getCursorGrid"

    global cursorUseGrid
    radiobutton .getCursorGrid.yes -text "Use Grid" -variable cursorUseGrid \
      -value 1 -command "SwitchGridMode"
    radiobutton .getCursorGrid.no -text "No Grid" -variable cursorUseGrid \
      -value 0 -command "SwitchGridMode"

    global cursorGridX cursorGridY
    label .getCursorGrid.xLab -text "Grid X:"
    entry .getCursorGrid.xEnt -width 20 -relief sunken \
      -textvariable cursorGridX
    label .getCursorGrid.yLab -text "Grid Y:"
    entry .getCursorGrid.yEnt -width 20 -relief sunken \
      -textvariable cursorGridY

    # These frames are for spacing only.
    frame .getCursorGrid.row2a -width 80m -height 6m
    frame .getCursorGrid.row4a -width 10m -height 4m

    # Pack some widgets into the frames.  Other widges will be packed in
    # the SwitchGridMode procedure.
    pack .getCursorGrid.row1 -side bottom -pady 4m
    pack .getCursorGrid.row2 -side top
    pack .getCursorGrid.row2a -side top

    pack .getCursorGrid.ok .getCursorGrid.cancel -in .getCursorGrid.row1 \
      -side left -padx 3m
    pack .getCursorGrid.no .getCursorGrid.yes -in .getCursorGrid.row2 \
      -side top -pady 1m
    pack .getCursorGrid.xLab .getCursorGrid.xEnt \
      -in .getCursorGrid.row3 -side left
    pack .getCursorGrid.yLab .getCursorGrid.yEnt -in .getCursorGrid.row4 \
      -side left

    # Get the current values from the cursor and set the GUI accordingly.
    # Returns: <useGrid> <gridX> <gridY>
    set grid [DEVise getCursorGrid $cursorName]
    set cursorUseGrid [lindex $grid 0]
    set cursorGridX [lindex $grid 1]
    set cursorGridY [lindex $grid 2]
    SwitchGridMode

    # Wait for the user to make a selection from this window.
    tkwait visibility .getCursorGrid
    grab set .getCursorGrid
    tkwait window .getCursorGrid
}

##########################################################################

proc SwitchGridMode {} {
    global cursorUseGrid

    if {$cursorUseGrid} {
      pack .getCursorGrid.row3 -after .getCursorGrid.row2a -pady 1m
      pack .getCursorGrid.row4 -after .getCursorGrid.row3 -pady 1m
      pack .getCursorGrid.row4a -after .getCursorGrid.row4 -pady 1m
    } else {
      pack forget .getCursorGrid.row3
      pack forget .getCursorGrid.row4
      pack forget .getCursorGrid.row4a
    }
}

##########################################################################

proc SetCursorGrid {} {
    global gridCursorName
    global cursorUseGrid
    global cursorGridX cursorGridY

    # Arguments: <cursorName> <useGrid> <gridX> <gridY>
    DEVise setCursorGrid $gridCursorName $cursorUseGrid $cursorGridX \
      $cursorGridY
}

#============================================================================

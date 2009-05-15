# ========================================================================
# DEVise Data Visualization Software
# (c) Copyright 1992-2009
# By the DEVise Development Group
# Madison, Wisconsin
# All Rights Reserved.
# ========================================================================

# Under no circumstances is this software to be copied, distributed,
# or altered in any way without prior permission from the DEVise
# Development Group.

############################################################

# GUI to control misc. view properties that we don't control anywhere
# else.

############################################################

# $Id$

# $Log$
# Revision 1.1  2006/07/12 20:54:51  wenger
# Added "Misc. View Properties" GUI to control things we haven't
# had a GUI for previously.
#

############################################################

# Note: I'm not doing anything with setViewSolid3D because the non-JavaScreen
#   3D code doesn't really do anything anymore.  wenger 2006-07-12.
# Note: I'm not doing anything with setViewDisplayDataValues because it
#   seems to be a duplicate of highlightView.  wenger 2006-07-12.
# Note: I'm not doing anything with setViewXYZoom because it doesn't
#   seem to actually have any effect in the View code anymore.  wenger
#   2006-07-12.

proc EditViewMisc {} {
    global curView

    # If this window already exists, raise it to the top and return.
    if {[WindowVisible .editViewMisc]} {
	return
    }


    # Get the current values from the view and set the GUI accordingly.
    global viewMiscXLinkMult
    global viewMiscYLinkMult
    global viewMiscHomeVisLink
    global viewMiscHomeVisLinkInvisible
    global viewMiscUseJmol
    global viewMiscGAttrLinkMode
    global viewMiscExcludeDD

    set viewMiscXLinkMult [DEVise getLinkMultFact $curView X]
    set viewMiscYLinkMult [DEVise getLinkMultFact $curView Y]
    set viewMiscHomeVisLink [DEVise getDoHomeOnVisLink $curView]
    set viewMiscHomeVisLinkInvisible [DEVise \
      getDoHomeOnVisLinkIfInvisible $curView]
    set viewMiscUseJmol [DEVise viewGetUseJmol $curView]
    set viewMiscGAttrLinkMode [DEVise getGAttrLinkMode $curView]
    set viewMiscExcludeDD [DEVise getExcludeFromDrillDown $curView]


    # Create the top level widget and the frames we'll later use for
    # positioning.
    toplevel .editViewMisc
    wm title .editViewMisc "Misc. View Properties ($curView)"

    frame .editViewMisc.row1
    frame .editViewMisc.row2
    frame .editViewMisc.row3
    frame .editViewMisc.row4
    frame .editViewMisc.row5
    frame .editViewMisc.row6
    frame .editViewMisc.row7

    # Create the various widgets.
    button .editViewMisc.ok -text "OK" -width 10 \
      -command {SetViewMisc; destroy .editViewMisc}
    button .editViewMisc.cancel -text "Cancel" -width 10 \
      -command "destroy .editViewMisc"

    label .editViewMisc.xMultLabel -text "Link X Mult Factor:" 
    entry .editViewMisc.xMultEntry -width 20 -relief sunken \
      -textvariable viewMiscXLinkMult

    label .editViewMisc.yMultLabel -text "Link Y Mult Factor:"
    entry .editViewMisc.yMultEntry -width 20 -relief sunken \
      -textvariable viewMiscYLinkMult

    checkbutton .editViewMisc.linkHome -text "Do Home on Visual Link" \
      -variable viewMiscHomeVisLink
    checkbutton .editViewMisc.linkHomeInvisible \
      -text "Do Home on Visual Link if Invisible" \
      -variable viewMiscHomeVisLinkInvisible

    checkbutton .editViewMisc.useJmol -text "Use Jmol" \
      -variable viewMiscUseJmol

    checkbutton .editViewMisc.excludeDD -text "Exclude from drill-down" \
      -variable viewMiscExcludeDD

    label .editViewMisc.gAttrModeLabel -text "GData Attr Link Mode:"
    radiobutton .editViewMisc.gAttrModeInter -text "Intersection" \
      -variable viewMiscGAttrLinkMode -value 0
    radiobutton .editViewMisc.gAttrModeUnion -text "Union" \
      -variable viewMiscGAttrLinkMode -value 1

    # These frames are for spacing only.
    frame .editViewMisc.row3a -width 10m -height 4m
    frame .editViewMisc.row4a -width 10m -height 4m
    frame .editViewMisc.row5a -width 10m -height 4m
    frame .editViewMisc.row6a -width 10m -height 4m

    # Pack the widgets into the frames.
    pack .editViewMisc.row1 -side bottom -pady 4m
    pack .editViewMisc.row2 -side top -pady 1m
    pack .editViewMisc.row3 -side top -pady 1m
    pack .editViewMisc.row3a -side top
    pack .editViewMisc.row4 -side top
    pack .editViewMisc.row4a -side top
    pack .editViewMisc.row5 -side top
    pack .editViewMisc.row5a -side top
    pack .editViewMisc.row6 -side top
    pack .editViewMisc.row6a -side top
    pack .editViewMisc.row7 -side top

    pack .editViewMisc.ok .editViewMisc.cancel -in .editViewMisc.row1 \
      -side left -padx 3m

    pack .editViewMisc.xMultLabel .editViewMisc.xMultEntry \
      -in .editViewMisc.row2 -side left -padx 5m
    pack .editViewMisc.yMultLabel .editViewMisc.yMultEntry \
      -in .editViewMisc.row3 -side left -padx 5m

    pack .editViewMisc.linkHome .editViewMisc.linkHomeInvisible \
      -in .editViewMisc.row4 -side left -padx 5m

    pack .editViewMisc.gAttrModeLabel .editViewMisc.gAttrModeInter \
      .editViewMisc.gAttrModeUnion -in .editViewMisc.row5 \
      -side left -padx 2m

    pack .editViewMisc.useJmol -in .editViewMisc.row6 -side left -padx 5m

    pack .editViewMisc.excludeDD -in .editViewMisc.row7 -side left -padx 5m

    # Wait for the user to make a selection from this window.
    tkwait visibility .editViewMisc
    grab set .editViewMisc
    tkwait window .editViewMisc
}

##########################################################################

proc SetViewMisc {} {
    global curView
    global viewMiscXLinkMult
    global viewMiscYLinkMult
    global viewMiscHomeVisLink
    global viewMiscHomeVisLinkInvisible
    global viewMiscUseJmol
    global viewMiscGAttrLinkMode
    global viewMiscExcludeDD

    DEVise setLinkMultFact $curView X $viewMiscXLinkMult
    DEVise setLinkMultFact $curView Y $viewMiscYLinkMult
    DEVise setDoHomeOnVisLink $curView $viewMiscHomeVisLink
    DEVise setDoHomeOnVisLinkIfInvisible $curView $viewMiscHomeVisLinkInvisible
    DEVise viewSetUseJmol $curView $viewMiscUseJmol
    DEVise setGAttrLinkMode $curView $viewMiscGAttrLinkMode
    DEVise setExcludeFromDrillDown $curView $viewMiscExcludeDD
}

#============================================================================

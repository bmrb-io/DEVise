#  ========================================================================
#  DEVise Data Visualization Software
#  (c) Copyright 1992-1996
#  By the DEVise Development Group
#  Madison, Wisconsin
#  All Rights Reserved.
#  ========================================================================
#
#  Under no circumstances is this software to be copied, distributed,
#  or altered in any way without prior permission from the DEVise
#  Development Group.

############################################################

#  $Id$

#  $Log$

############################################################

set backGround darkOliveGreen
set foreGround white

tk_setPalette background $backGround foreground $foreGround \
	selectBackground $foreGround selectForeground $backGround
. configure -bg $backGround

wm geometry . +100+100

frame .mbar -relief raised -bd 4
frame .panel -relief groove

pack .mbar -side top -expand 0 -fill x
pack .panel -side top -expand 1 -fill both

frame .panel.left -relief groove -bd 2 -bg tan -width 3c -height 10c
frame .panel.right -relief groove -bd 2 -bg white -width 17c -height 10c
pack .panel.left .panel.right -side left -padx 2m -expand 1 -fill both

button .panel.left.select -text Select -width 8 -command {Server select}
button .panel.left.move -text Move -width 8 -command {Server move}
pack .panel.left.select .panel.left.move -side top -pady 1m

menubutton .mbar.empty -text "" -width 8 -menu .mbar.empty.menu \
	-state disabled
menubutton .mbar.file -text File -width 8 -menu .mbar.file.menu
menubutton .mbar.edit -text Edit -width 8 -menu .mbar.edit.menu
menubutton .mbar.misc -text "Misc." -width 8 -menu .mbar.misc.menu
menubutton .mbar.model -text Model -width 8 -menu .mbar.model.menu

tk_menuBar .mbar .mbar.file .mbar.edit .mbar.misc .mbar.model
focus .mbar

pack .mbar.empty .mbar.file .mbar.edit .mbar.misc .mbar.model \
	-expand 0 -side left -fill x

menu .mbar.file.menu -tearoff 0
.mbar.file.menu add command -label "New" -command {Server file new}
.mbar.file.menu add command -label "Open" -command {Server file open}
.mbar.file.menu add command -label "Save" -command {Server file save}
.mbar.file.menu add command -label "Save As" -command {Server file saveas}
.mbar.file.menu add command -label "Save View As" \
	-command {Server file saveviewas}
.mbar.file.menu add separator
.mbar.file.menu add command -label "Print" -command {Server file print}
.mbar.file.menu add command -label "Print Selected" \
	-command {Server file printselected}
.mbar.file.menu add separator
.mbar.file.menu add command -label "Quit" -command {Server exit; destroy .}

menu .mbar.edit.menu -tearoff 0
.mbar.edit.menu add command -label "Undo" -command {Server edit undo}
.mbar.edit.menu add command -label "Redo" -command {Server edit redo}
.mbar.edit.menu add separator
.mbar.edit.menu add command -label "Cut" -command {Server edit cut}
.mbar.edit.menu add command -label "Copy" -command {Server edit copy}
.mbar.edit.menu add command -label "Paste" -command {Server edit paste}
.mbar.edit.menu add command -label "Delete" -command {Server edit delete}
.mbar.edit.menu add separator
.mbar.edit.menu add command -label "Select all" \
	-command {Server edit selectall}
.mbar.edit.menu add separator
.mbar.edit.menu add command -label "Bring To Front" \
	-command {Server edit bringtofront}
.mbar.edit.menu add command -label "Send To Back" \
	-command {Server edit sendtoback}
.mbar.edit.menu add separator
.mbar.edit.menu add command -label "Select All" \
	-command {Server edit selectAll}
.mbar.edit.menu add separator
.mbar.edit.menu add cascade -label "Hide All..." \
	-menu .mbar.edit.menu.hideall
.mbar.edit.menu add command -label "Show All..." \
	-command {Server edit showall}
.mbar.edit.menu add separator
.mbar.edit.menu add command -label "Alignment..." \
	-command {Server edit alignment}
.mbar.edit.menu add separator
.mbar.edit.menu add command -label "Zooming" -command {Server edit zooming}
.mbar.edit.menu add command -label "Alignment" -command {Server edit panning}
.mbar.edit.menu add separator
.mbar.edit.menu add command -label "Grouping" -command {Server edit grouping}

menu .mbar.edit.menu.hideall -tearoff 0
.mbar.edit.menu.hideall add command -label "XYZ" \
	-command {Server edit hideall XYZ}

menu .mbar.misc.menu -tearoff 0
.mbar.misc.menu add command -label "Show Footprint" \
	-command {Server misc showfootprint}
.mbar.misc.menu add command -label "Export Schema" \
	-command {Server misc exportschema}
.mbar.misc.menu add command -label "Import Schema" \
	-command {Server misc importschema}
.mbar.misc.menu add command -label "Send Schema" \
	-command {Server misc sendschema}
.mbar.misc.menu add command -label "Get Schema" \
	-command {Server misc getschema}
.mbar.misc.menu add separator
.mbar.misc.menu add command -label "Write Out Personal Model" \
	-command {Server misc writeoutpersonalmodel}
.mbar.misc.menu add command -label "Start Profile" \
	-command {Server misc startprofile}
.mbar.misc.menu add command -label "Stop Profile" \
	-command {Server misc stopprofile}
.mbar.misc.menu add command -label "Execute Script" \
	-command {Server misc executescript}
.mbar.misc.menu add command -label "Get Info About" \
	-command {Server misc getinfoabout}

menu .mbar.model.menu -tearoff 0
.mbar.model.menu add command -label "Load Metaphore From File" \
	-command {Server model loadmetaphorefromfile}
.mbar.model.menu add separator
.mbar.model.menu add command -label "Output Current Metaphore as Description" \
	-command {Server model outputcurrentmetaphoreasdescription}
.mbar.model.menu add command -label "Parse Metaphore Description in File" \
	-command {Server model parsemetaphoredescriptioninfile}

#  ========================================================================
#  DEVise Data Visualization Software
#  (c) Copyright 1992-1998
#  By the DEVise Development Group
#  Madison, Wisconsin
#  All Rights Reserved.
#  ========================================================================
#
#  Under no circumstances is this software to be copied, distributed,
#  or altered in any way without prior permission from the DEVise
#  Development Group.

############################################################

#	$Id$

#	$Log$

############################################################

proc CreateLink { flag } {
	global button dialogCkButVar dialogCkButInternalVar

	# 1. Create the top-level window and divide it into top
	# and bottom parts.

	toplevel .createLinkGUI -class Dialog
	wm geometry .createLinkGUI +100+100
	wm title .createLinkGUI "Create Link"
	wm iconname .createLinkGUI Dialog
	frame .createLinkGUI.top -relief raised -bd 1
	frame .createLinkGUI.listFrame
	pack .createLinkGUI.top .createLinkGUI.listFrame -side top -fill both
	frame .createLinkGUI.bot -relief raised -bd 1
	pack .createLinkGUI.bot -side bottom -fill both

	# 2. Fill the top part with the message.

	set text "Enter parameters for creating a new link"
    set txtWidth [LineWidth $text]
	set txtHeight [LineHeight $text]
	text .createLinkGUI.top.msg -width $txtWidth -height $txtHeight \
		-font -*-*-medium-r-normal-*-140-*-*-m-*
	.createLinkGUI.top.msg insert end $text
	.createLinkGUI.top.msg configure -state disabled
	pack .createLinkGUI.top.msg -side right -expand 1 -fill both\
			-padx 3m -pady 3m

	# Create entry.
	frame .createLinkGUI.entries
	label .createLinkGUI.entries.label -text "name"
	entry .createLinkGUI.entries.entry -relief sunken -width 60
	pack .createLinkGUI.entries.label .createLinkGUI.entries.entry -side left

	# Create a checkbutton for each of the items
	set items {x y record {tdata attribute}}
	set butNames ""
	set varNames ""
	set cnt 0

	foreach i $items {
		set butName .createLinkGUI.but_$i
		set varName dialogCkButInternalVar(var_$i)
		if { [expr (1 << $cnt) & $flag] == 0 } {
			set $varName 0
		} else {
			set $varName 1 
		}
		lappend butNames $butName
		lappend varNames $varName
		set cnt [incr cnt]

		checkbutton $butName -text $i -variable $varName -anchor w
	}

	# This little section is the point of this whole business -- it
	# disallows illegal combinations.
	.createLinkGUI.but_x configure -command { \
	  set dialogCkButInternalVar(var_record) 0; \
	  set {dialogCkButInternalVar(var_tdata attribute)} 0 \
	}
	.createLinkGUI.but_y configure -command { \
	  set dialogCkButInternalVar(var_record) 0; \
	  set {dialogCkButInternalVar(var_tdata attribute)} 0 \
	}
	.createLinkGUI.but_record configure -command {
	  set dialogCkButInternalVar(var_x) 0; \
	  set dialogCkButInternalVar(var_y) 0; \
	  set {dialogCkButInternalVar(var_tdata attribute)} 0 \
	}
	{.createLinkGUI.but_tdata attribute} configure -command {
	  set dialogCkButInternalVar(var_x) 0; \
	  set dialogCkButInternalVar(var_y) 0; \
	  set dialogCkButInternalVar(var_record) 0; \
	}

	eval pack .createLinkGUI.entries $butNames -in .createLinkGUI.listFrame -side top \
		-fill x -expand 1

	# 3. Create a row of buttons at the bottom of the dialog.

	set i 0
	set args { OK Info Cancel }
	set default 0
	foreach but $args {
		button .createLinkGUI.bot.button$i -text $but -command\
				"set button $i"
		if {$i == $default} {
			frame .createLinkGUI.bot.default -relief sunken -bd 1
			raise .createLinkGUI.bot.button$i
			pack .createLinkGUI.bot.default -side left -expand 1 -fill x
#					-padx 3m -pady 2m
			pack .createLinkGUI.bot.button$i -in .createLinkGUI.bot.default\
					-side left -expand 1 -fill x
#					-padx 2m -pady 2m -ipadx 2m -ipady 1m
		} else {
			pack .createLinkGUI.bot.button$i -side left -expand 1 -fill x
#					-padx 3m -pady 3m -ipadx 2m -ipady 1m
		}
		incr i
	}

	# 4. Set up a binding for <Return>, if there`s a default,
	# set a grab, and claim the focus too.

	if {$default >= 0} {
		bind .createLinkGUI <Return> ".createLinkGUI.bot.button$default flash; \
			set button $default"
	}
	set oldFocus [focus]
	grab set .createLinkGUI
	focus .createLinkGUI

	# 5. Wait for the user to respond, then restore the focus
	# and return the index of the selected button.

	tkwait variable button

	set i 0
	set retVal 0
	foreach varName $varNames {
		eval set val $$varName
		if { $val > 0 } {
			set retVal [expr $retVal|(1<<$i)]
		}
		set i [incr i]
	}
	set dialogCkButVar(selected) $retVal

	set dialogCkButVar(entry) [ .createLinkGUI.entries.entry get ]

	destroy .createLinkGUI
	focus $oldFocus
	return $button
}

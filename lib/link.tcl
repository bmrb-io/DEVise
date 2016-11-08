#  ========================================================================
#  DEVise Data Visualization Software
#  (c) Copyright 1992-2002
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
#	Revision 1.5.10.1  2002/09/17 18:50:55  wenger
#	Added GUI for GAttr links.
#	
#	Revision 1.5  2001/04/27 17:09:56  wenger
#	Made various cleanups to external process dynamic data generation and
#	added most GUI (still need special GUI for creating the data source);
#	cleanups included finding and fixing bug 668.
#	
#	Revision 1.4  2001/01/08 20:33:04  wenger
#	Merged all changes thru mgd_thru_dup_gds_fix on the js_cgi_br branch
#	back onto the trunk.
#	
#	Revision 1.2.2.1  2000/10/18 20:35:22  wenger
#	Merged changes from fixed_bug_616 through link_gui_improvements onto
#	the branch.
#	
#	Revision 1.3  2000/10/16 16:11:54  wenger
#	Link creation GUI now gives a choice of positive or negative record
#	links; fixed bug 622 (record link type not saved in session files);
#	fixed bug 623 (record link update problem); other link-related cleanups.
#	
#	Revision 1.2  1999/11/15 23:08:08  wenger
#	Removed set link GUI, since this is no longer an option.
#	
#	Revision 1.1  1998/07/17 15:33:50  wenger
#	Improved link creation GUI as per request from Raghu; started on
#	DataReader schema GUI; changed version to 1.5.4.
#

############################################################

# Flag is a bitwise or of the following:
#   1: X link
#   2: Y link
#   4: Record link, positive
#   8: Record link, negative
#   16: External data link
#   32: GData attr link
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

	# Create entry for link name.
	frame .createLinkGUI.entries
	label .createLinkGUI.entries.label -text "name"
	entry .createLinkGUI.entries.entry -relief sunken -width 60
	pack .createLinkGUI.entries.label .createLinkGUI.entries.entry -side left

	# Create a checkbutton for each of the items
	# First element of each is name, second is text to show in GUI.
	set items {{x x} {y y} {record_positive "record (positive)"} \
	  {record_negative "record (negative)"} {ext_data "external data"} \
	  {gattr "GData attr"}}
	set butNames ""
	set varNames ""
	set cnt 0

	foreach i $items {
		set itemName [lindex $i 0]
		set itemText [lindex $i 1]
		set butName .createLinkGUI.but_$itemName
		set varName dialogCkButInternalVar(var_$itemName)

		# Turn on the selection(s) indicated by the given flag value.
		if { [expr (1 << $cnt) & $flag] == 0 } {
			set $varName 0
		} else {
			set $varName 1 
		}

		lappend butNames $butName
		lappend varNames $varName
		set cnt [incr cnt]

		checkbutton $butName -text $itemText -variable $varName -anchor w
	}

	# This little section is the point of this whole business -- it
	# disallows illegal combinations.
	.createLinkGUI.but_x configure -command { \
	  set dialogCkButInternalVar(var_record_positive) 0; \
	  set dialogCkButInternalVar(var_record_negative) 0; \
	  set dialogCkButInternalVar(var_ext_data) 0; \
	}
	.createLinkGUI.but_y configure -command { \
	  set dialogCkButInternalVar(var_record_positive) 0; \
	  set dialogCkButInternalVar(var_record_negative) 0; \
	  set dialogCkButInternalVar(var_ext_data) 0; \
	  set dialogCkButInternalVar(var_gattr) 0; \
	}
	.createLinkGUI.but_record_positive configure -command {
	  set dialogCkButInternalVar(var_x) 0; \
	  set dialogCkButInternalVar(var_y) 0; \
	  set dialogCkButInternalVar(var_record_negative) 0; \
	  set dialogCkButInternalVar(var_ext_data) 0; \
	  set dialogCkButInternalVar(var_gattr) 0; \
	}
	.createLinkGUI.but_record_negative configure -command {
	  set dialogCkButInternalVar(var_x) 0; \
	  set dialogCkButInternalVar(var_y) 0; \
	  set dialogCkButInternalVar(var_record_positive) 0; \
	  set dialogCkButInternalVar(var_ext_data) 0; \
	  set dialogCkButInternalVar(var_gattr) 0; \
	}
	.createLinkGUI.but_ext_data configure -command {
	  set dialogCkButInternalVar(var_x) 0; \
	  set dialogCkButInternalVar(var_y) 0; \
	  set dialogCkButInternalVar(var_record_positive) 0; \
	  set dialogCkButInternalVar(var_record_negative) 0; \
	  set dialogCkButInternalVar(var_gattr) 0; \
	}
	.createLinkGUI.but_gattr configure -command {
	  set dialogCkButInternalVar(var_x) 0; \
	  set dialogCkButInternalVar(var_y) 0; \
	  set dialogCkButInternalVar(var_record_positive) 0; \
	  set dialogCkButInternalVar(var_record_negative) 0; \
	  set dialogCkButInternalVar(var_ext_data) 0; \
	}

	eval pack .createLinkGUI.entries $butNames -in .createLinkGUI.listFrame \
	    -side top -fill x -expand 1

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

############################################################

# Allow the user to select a GAttr link (if there are any)
# for which to set the attributes.

proc DoSetGAttrLinkAttrs {} {
    global dialogListVar

    set linkSet [GAttrLinkSet]
    if { [llength $linkSet] == 0 } {
        dialog .noLinks "No Links" "There are no GAttr links." "" 0 OK
        return
    }

	set answer [dialogList .getGAttrLink "Select Link" \
	  "Select a link" "" 0 { OK Cancel } $linkSet]

    if {$answer == 0} {
	    SetGAttrLinkAttrs $dialogListVar(selected)
	}
}

############################################################

# Show the dialog to allow the user to set the attributes for
# the given GData attribute link.

proc SetGAttrLinkAttrs {linkName} {

	set gdAttrs {"x" "y" "z" "color" "size" "pattern" "orientation" "shape" \
	  "shapeAttr_0" "shapeAttr_1" "shapeAttr_2" "shapeAttr_3" "shapeAttr_4" \
	  "shapeAttr_5" "shapeAttr_6" "shapeAttr_7" "shapeAttr_8" "shapeAttr_9" \
	  "shapeAttr_10" "shapeAttr_11" "shapeAttr_12" "shapeAttr_13" \
	  "shapeAttr_14" }

    set leaderAttr [DEVise getLinkMasterAttr $linkName]
    set followerAttr [DEVise getLinkSlaveAttr $linkName]

    # If this window already exists, raise it to the top and return.
	if {[WindowVisible .setLinkGAttrs]} {
	   return
	}

    # Create the top level widget and the frames we'll later use for
	# positioning.
	toplevel .setLinkGAttrs
	wm title .setLinkGAttrs "Set Link Attributes ($linkName)"

	frame .setLinkGAttrs.leaderRow
	frame .setLinkGAttrs.followerRow
	frame .setLinkGAttrs.buttonRow

    # Create the various objects that will be shown in the window.
	label .setLinkGAttrs.leaderLabel -text "Leader:"
	label .setLinkGAttrs.followerLabel -text "Follower:"

	menubutton .setLinkGAttrs.leader -relief raised -bd 2 -text $leaderAttr \
	  -menu .setLinkGAttrs.leader.menu
	menu .setLinkGAttrs.leader.menu -tearoff false
	foreach attr $gdAttrs {
	    .setLinkGAttrs.leader.menu add command -label $attr \
		  -command ".setLinkGAttrs.leader configure -text $attr"
	}

	menubutton .setLinkGAttrs.follower -relief raised -bd 2 \
	  -text $followerAttr -menu .setLinkGAttrs.follower.menu
	menu .setLinkGAttrs.follower.menu -tearoff false
	foreach attr $gdAttrs {
	    .setLinkGAttrs.follower.menu add command -label $attr \
		  -command ".setLinkGAttrs.follower configure -text $attr"
	}

    button .setLinkGAttrs.ok -text "OK" -width 10 \
	  -command "SetGAttrLinkAttrsOK $linkName; destroy .setLinkGAttrs"
    button .setLinkGAttrs.cancel -text "Cancel" -width 10 \
	  -command "destroy .setLinkGAttrs"

    # Pack everything into the frames.
	pack .setLinkGAttrs.leaderRow -side top -pady 2m
	pack .setLinkGAttrs.followerRow -side top -pady 2m
	pack .setLinkGAttrs.buttonRow -side top -pady 2m

	pack .setLinkGAttrs.leaderLabel .setLinkGAttrs.leader \
	  -in .setLinkGAttrs.leaderRow -side left -padx 4m
	pack .setLinkGAttrs.followerLabel .setLinkGAttrs.follower \
	  -in .setLinkGAttrs.followerRow -side left -padx 4m
	pack .setLinkGAttrs.ok .setLinkGAttrs.cancel \
	  -in .setLinkGAttrs.buttonRow -side left -padx 4m

    # Wait for the user to make a selection from this window.
    tkwait visibility .setLinkGAttrs
	grab set .setLinkGAttrs
    tkwait window .setLinkGAttrs
}

############################################################

# User has clicked 'OK' button in the GAttr link attribute
# dialog.

proc SetGAttrLinkAttrsOK {linkName} {
    set leaderAttr [lindex [.setLinkGAttrs.leader configure -text] 4]
    set followerAttr [lindex [.setLinkGAttrs.follower configure -text] 4]

    DEVise setLinkMasterAttr $linkName $leaderAttr
	DEVise setLinkSlaveAttr $linkName $followerAttr
}

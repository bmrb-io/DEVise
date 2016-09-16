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
#  Revision 1.3  1996/11/25 22:31:32  beyer
#  1. extended .devise.rc search
#  2. added DestroyView command
#  3. query window updated properly, history window update changed
#  4. filter properly set to (0,100) instead of (100,0) when high,low values
#     are not known.
#
#  Revision 1.2  1996/04/11 18:23:00  jussi
#  Major changes in the organization of the user interface.
#
#  Revision 1.1  1996/01/23 20:51:00  jussi
#  Initial revision.
#

############################################################

proc ProcessMarkSelected { sel } {
    global curView
    set mark [.history.listMark get $sel]
    if {$mark == "*"} {
	set newMark " "
	set newVal 0
    } else {
	set newMark "*"
	set newVal 1
    }
    set index [expr [.history.listMark size]-$sel-1]
    DEVise markViewFilter $curView $index $newVal
    .history.listMark insert [expr $sel+1] $newMark
    .history.listMark delete $sel
}

############################################################

proc ProcessHistorySelected { sel } {
    if {[WindowExists .query]} {
	foreach i { xlow ylow xhigh yhigh } {
	    .query.$i delete 0 end
	}
	
	.query.xlow  insert 0 [.history.listXlow get $sel]
	.query.ylow  insert 0 [.history.listYlow get $sel]
	.query.xhigh insert 0 [.history.listXhigh get $sel]
	.query.yhigh insert 0 [.history.listYhigh get $sel]
    }
}

############################################################

proc DoHistoryToggle {} {
    if {[WindowExists .history]} {
	destroy .history
    } else {
	OpenHistory
    }
}

############################################################

proc OpenHistory {} {
    global curView LightBlue
    
    set w .history

    if {[WindowVisible $w]} {
	return
    }
    
    toplevel $w 
    wm title $w "DEVise history"
    wm geometry $w +100+100
    wm iconname $w "history"

    frame $w.heading -background LightBlue
    label $w.markHeading -text mark
    label $w.xlowHeading -text xlow
    label $w.xhighHeading -text xhigh
    label $w.ylowHeading -text ylow
    label $w.yhighHeading -text yhigh
    
    pack $w.markHeading -side left -in $w.heading 
    pack $w.xlowHeading $w.xhighHeading $w.ylowHeading \
	    $w.yhighHeading -side left -in $w.heading -expand 1 -fill x
    
    frame $w.lists -background LightBlue
    listbox $w.listMark -yscrollcommand "$w.scroll set" \
	    -relief raised -selectmode single -width 4
    
    foreach i { listXlow listYlow listXhigh listYhigh } {
	listbox $w.$i -yscrollcommand "$w.scroll set" \
		-relief raised -selectmode single
    }
    bind $w.listMark <Button-1> {
	set sel [.history.listMark nearest %y]
	if {$sel >= 0} {
	    ProcessMarkSelected $sel
	}
    }
    
    bind $w.listXlow <Button-1> {
	set sel [.history.listXlow nearest %y]
	if {$sel >= 0} {
	    ProcessHistorySelected $sel
	}
    }
    bind $w.listXhigh <Button-1> {
	set sel [.history.listXhigh nearest %y]
	if {$sel >= 0} {
	    ProcessHistorySelected $sel
	}
    }
    bind $w.listYlow <Button-1> {
	set sel [.history.listYlow nearest %y]
	if {$sel >= 0} {
	    ProcessHistorySelected $sel
	}
    }
    bind $w.listYhigh <Button-1> {
	set sel [.history.listYhigh nearest %y]
	if {$sel >= 0} {
	    ProcessHistorySelected $sel
	}
    }
    scrollbar $w.scroll -command "DoScrollBar"
    
    button $w.close -text Close -width 10 -command { destroy .history }
    
    pack $w.close -side bottom
    
    pack $w.scroll -in $w.lists -side right -fill both
    pack  $w.listYhigh $w.listYlow $w.listXhigh $w.listXlow \
	    -in $w.lists -side right 
    pack $w.listMark -in $w.lists -side left 
    
    pack $w.heading $w.lists -side top -fill x -expand 1
    
    UpdateHistoryWindow
}

############################################################

proc UpdateHistoryWindow {} {
    global curView

    if {![WindowExists .history]} {
        return
    }

    foreach i {listMark listXlow listXhigh listYlow listYhigh} {
        selection clear .history.$i
        .history.$i delete 0 end
    }
    if { $curView != "" } {
	# Init history with history of current view
	set filters [DEVise getVisualFilters $curView]
	foreach filter $filters {
	    .history.listXlow insert 0 [lindex $filter 0]
	    .history.listXhigh insert 0 [lindex $filter 2]
	    .history.listYlow insert 0 [lindex $filter 1]
	    .history.listYhigh insert 0 [lindex $filter 3]
	    if { [lindex $filter 4] == 0 } {
		set mark " "
	    } else {
		set mark "*"
	    }
	    .history.listMark insert 0 $mark
	}
    }
}
 
############################################################

# note: DoScrollBar takes variable number of arguments
proc DoScrollBar args {
    foreach i {listMark listXlow listXhigh listYlow listYhigh} {
	eval .history.$i yview $args
    }
}

############################################################

proc ClearHistory {} {
    if {![WindowExists .history]} {
        return
    }

    foreach i {listMark listXlow listXhigh listYlow listYhigh} {
        selection clear .history.$i
        .history.$i delete 0 end
    }
}

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
#  Revision 1.2  1996/04/11 18:23:00  jussi
#  Major changes in the organization of the user interface.
#
#  Revision 1.1  1996/01/23 20:51:00  jussi
#  Initial revision.
#

############################################################

proc ProcessMarkSelected { sel } {
    global curView
    set mark [.historyWin.listMark get $sel]
    if {$mark == "*"} {
	set newMark " "
	set newVal 0
    } else {
	set newMark "*"
	set newVal 1
    }
    set index [expr [.historyWin.listMark size]-$sel-1]
    DEVise markViewFilter $curView $index $newVal
    .historyWin.listMark insert [expr $sel+1] $newMark
    .historyWin.listMark delete $sel
}

############################################################

proc ProcessHistorySelected { sel } {
    global queryWinOpened

    if {$queryWinOpened} {
	foreach i { xlow ylow xhigh yhigh } {
	    .query.$i delete 0 end
	}
	
	.query.xlow  insert 0 [.historyWin.listXlow get $sel]
	.query.ylow  insert 0 [.historyWin.listYlow get $sel]
	.query.xhigh insert 0 [.historyWin.listXhigh get $sel]
	.query.yhigh insert 0 [.historyWin.listYhigh get $sel]
    }
}

############################################################

proc CloseHistory {} {
    global historyWinOpened
    destroy .historyWin
    set historyWinOpened 0
}

############################################################

proc DoHistoryToggle {} {
    global historyWinOpened
    if { $historyWinOpened} {
	CloseHistory
    } else {
	OpenHistory
    }
}

############################################################

proc OpenHistory {} {
    global curView historyWinOpened LightBlue
    
    if { $historyWinOpened } {
	return
    }
    
    set w .historyWin
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
	set sel [.historyWin.listMark nearest %y]
	if {$sel >= 0} {
	    ProcessMarkSelected $sel
	}
    }
    
    bind $w.listXlow <Button-1> {
	set sel [.historyWin.listXlow nearest %y]
	if {$sel >= 0} {
	    ProcessHistorySelected $sel
	}
    }
    bind $w.listXhigh <Button-1> {
	set sel [.historyWin.listXhigh nearest %y]
	if {$sel >= 0} {
	    ProcessHistorySelected $sel
	}
    }
    bind $w.listYlow <Button-1> {
	set sel [.historyWin.listYlow nearest %y]
	if {$sel >= 0} {
	    ProcessHistorySelected $sel
	}
    }
    bind $w.listYhigh <Button-1> {
	set sel [.historyWin.listYhigh nearest %y]
	if {$sel >= 0} {
	    ProcessHistorySelected $sel
	}
    }
    scrollbar $w.scroll -command "DoScrollBar"
    
    button $w.close -text Close -command CloseHistory -width 10
    
    pack $w.close -side bottom
    
    pack $w.scroll -in $w.lists -side right -fill both
    pack  $w.listYhigh $w.listYlow $w.listXhigh $w.listXlow \
	    -in $w.lists -side right 
    pack $w.listMark -in $w.lists -side left 
    
    pack $w.heading $w.lists -side top -fill x -expand 1
    
    set historyWinOpened 1
    UpdateHistoryWindow
}

############################################################

proc UpdateHistoryWindow {} {
    global curView historyWinOpened
    if { !$historyWinOpened } {
        return
    }
    foreach i {listMark listXlow listXhigh listYlow listYhigh} {
        selection clear .historyWin.$i
        .historyWin.$i delete 0 end
    }
    if { $curView != "" } {
	# Init history with history of current view
	set filters [DEVise getVisualFilters $curView]
	foreach filter $filters {
	    .historyWin.listXlow insert 0 [lindex $filter 0]
	    .historyWin.listXhigh insert 0 [lindex $filter 2]
	    .historyWin.listYlow insert 0 [lindex $filter 1]
	    .historyWin.listYhigh insert 0 [lindex $filter 3]
	    if { [lindex $filter 4] == 0 } {
		set mark " "
	    } else {
		set mark "*"
	    }
	    .historyWin.listMark insert 0 $mark
	}
    }
}
 
############################################################

# note: DoScrollBar takes variable number of arguments
proc DoScrollBar args {
    foreach i {listMark listXlow listXhigh listYlow listYhigh} {
	eval .historyWin.$i yview $args
    }
}

############################################################

proc ClearHistory {} {
    global historyWinOpened

    if {$historyWinOpened} {
	foreach i {listMark listXlow listXhigh listYlow listYhigh} {
	    selection clear .historyWin.$i
	    .historyWin.$i delete 0 end
	}
    }
}

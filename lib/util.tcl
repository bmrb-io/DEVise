#  ========================================================================
#  DEVise Data Visualization Software
#  (c) Copyright 1992-1995
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

proc PrintView {} {
    global printcmd allviews

    # see if .printdef window already exists; if so, just return
    set err [catch {wm state .printdef}]
    if {!$err} { wm deiconify .printdef; return }

    toplevel .printdef
    wm title .printdef "Print View"
    wm minsize .printdef 200 100
    wm maxsize .printdef 800 600
    wm geometry .printdef =500x150+150+150
    selection clear .printdef
    
    set printcmd "lpr "
    set allviews 0

    frame .printdef.top -relief groove -borderwidth 2
    frame .printdef.bot
    pack .printdef.top -side top -fill both -expand 1
    pack .printdef.bot -side top -fill x -pady 5m

    frame .printdef.top.row1
    frame .printdef.top.row2
    pack .printdef.top.row1 .printdef.top.row2 -side top -pady 3m

    label .printdef.top.row1.l1 -text "Print Command:"
    entry .printdef.top.row1.e1 -relief sunken -textvariable printcmd \
	    -width 30
    pack .printdef.top.row1.l1 .printdef.top.row1.e1 -side left -padx 2m \
	    -fill x -expand 1

    label .printdef.top.row2.l1 -text "Print Views:"
    radiobutton .printdef.top.row2.r1 -text "All Views" \
	    -variable allviews -value 1
    radiobutton .printdef.top.row2.r2 -text "Selected View" \
	    -variable allviews -value 0
    pack .printdef.top.row2.l1 .printdef.top.row2.r1 .printdef.top.row2.r2 \
	    -side left -padx 2m -fill x -expand 1

    frame .printdef.bot.but
    pack .printdef.bot.but -side top

    button .printdef.bot.but.ok -text OK -width 10 \
	    -command {	PrintActual $printcmd $allviews; \
	                destroy .printdef }
    button .printdef.bot.but.cancel -text Cancel -width 10 \
	    -command { destroy .printdef }
    pack .printdef.bot.but.ok .printdef.bot.but.cancel -side left -padx 7m

    tkwait visibility .printdef
}

proc PrintActual {printcmd allviews} {
    global curView

    set windowlist ""

    if {!$allviews} {
	if {$curView == ""} {
	    dialog .printView "Print Selected View" \
		    "Please select a view by clicking in it first." "" 0 OK
	    return
	}
	set windowlist [DEVise getViewWin $curView]

    } else {

	set viewClasses [ DEVise get view ]
	foreach viewClass $viewClasses {
	    set views [ DEVise get view $viewClass ]
	    foreach v $views {
		set win [DEVise getViewWin $v]
		if {[lsearch $windowlist $win] < 0} {
		    lappend windowlist $win
		}
	    }
	}
    }

    set filename "/tmp/devise-dump%d.ps"

    puts "Print windows: $windowlist"
    set err [ eval DEVise printWindows $filename $windowlist ]
    if {$err > 0} {
	dialog .printError "Print Error" \
		"An error occurred while printing views." "" 0 OK
	return
    }
}

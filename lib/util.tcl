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
#  Revision 1.4  1995/12/01 19:46:30  jussi
#  Fixed case where a view that is not mapped to the screen caused
#  an error.
#
#  Revision 1.3  1995/11/29 15:55:11  jussi
#  Removed constant window size definitions because they will produce
#  unexpected results on some window managers.
#
#  Revision 1.2  1995/11/28 16:58:25  jussi
#  Added capability to store window images in a user specified file
#  instead of printing it.
#
# Revision 1.1  1995/11/28  00:02:52  jussi
# Initial revision.
#

############################################################

proc PrintView {} {
    global toprinter printcmd filename allviews

    # see if .printdef window already exists; if so, just return
    set err [catch {wm state .printdef}]
    if {!$err} { wm deiconify .printdef; return }

    toplevel .printdef
    wm title .printdef "Print View"
    wm geometry .printdef +150+150
    selection clear .printdef
    
    set toprinter 1
    set printcmd "lpr "
    set filename "/tmp/devise_dump.ps"
    set allviews 0

    frame .printdef.top -relief groove -borderwidth 2
    frame .printdef.bot
    pack .printdef.top -side top -fill both -expand 1
    pack .printdef.bot -side top -fill x -pady 5m

    frame .printdef.top.row1
    frame .printdef.top.row2
    frame .printdef.top.row3 -relief groove -borderwidth 2
    pack .printdef.top.row1 .printdef.top.row2 .printdef.top.row3 \
	    -side top -pady 3m

    radiobutton .printdef.top.row1.r1 -text "To Printer" -width 12 \
	    -variable toprinter -value 1 -command {
	.printdef.top.row2.e1 configure -state disabled
	.printdef.top.row1.e1 configure -state normal
    } -anchor e
    label .printdef.top.row1.l1 -text "Print Command:" -width 14
    entry .printdef.top.row1.e1 -relief sunken -textvariable printcmd \
	    -width 30
    pack .printdef.top.row1.r1 .printdef.top.row1.l1 .printdef.top.row1.e1 \
	    -side left -padx 2m -fill x -expand 1

    radiobutton .printdef.top.row2.r1 -text "To File" -width 12 \
	    -variable toprinter -value 0 -command {
	.printdef.top.row1.e1 configure -state disabled
	.printdef.top.row2.e1 configure -state normal
    } -anchor e
    label .printdef.top.row2.l1 -text "File name:" -width 14
    entry .printdef.top.row2.e1 -relief sunken -textvariable filename \
	    -width 30 -state disabled
    pack .printdef.top.row2.r1 .printdef.top.row2.l1 .printdef.top.row2.e1 \
	    -side left -padx 2m -fill x -expand 1

    label .printdef.top.row3.l1 -text "Print Views:"
    radiobutton .printdef.top.row3.r1 -text "All Views" \
	    -variable allviews -value 1
    radiobutton .printdef.top.row3.r2 -text "Selected View" \
	    -variable allviews -value 0
    pack .printdef.top.row3.l1 .printdef.top.row3.r1 .printdef.top.row3.r2 \
	    -side left -padx 2m -fill x -expand 1

    frame .printdef.bot.but
    pack .printdef.bot.but -side top

    button .printdef.bot.but.ok -text OK -width 10 \
	    -command {	PrintActual $toprinter $printcmd $filename $allviews; \
	                destroy .printdef }
    button .printdef.bot.but.cancel -text Cancel -width 10 \
	    -command { destroy .printdef }
    pack .printdef.bot.but.ok .printdef.bot.but.cancel -side left -padx 7m

    tkwait visibility .printdef
}

proc PrintActual {toprinter printcmd filename allviews} {
    global curView

    set windowlist ""

    if {!$allviews} {
	if {$curView == ""} {
	    dialog .printView "Print Selected View" \
		    "Please select a view by clicking in it first." "" 0 OK
	    return
	}
	set windowlist [list [DEVise getViewWin $curView]]

    } else {

	set viewClasses [ DEVise get view ]
	foreach viewClass $viewClasses {
	    set views [ DEVise get view $viewClass ]
	    foreach v $views {
		set win [DEVise getViewWin $v]
		if {$win != "" && [lsearch $windowlist $win] < 0} {
		    lappend windowlist $win
		}
	    }
	}
    }

    if {$toprinter} {
	set template [format "/tmp/devise-%s-%%d.ps" [pid]]
    } else {
	set template [format "%s-%%d" $filename]
    }

    set i 0
    foreach win $windowlist {
	set file [format $template $i]
	puts "Save window $win to file $file"
	set err [ DEVise saveWindowImage $win $file ]
	if {$err > 0} {
	    dialog .printError "Window Image Save Error" \
		    "An error occurred while saving window images to files." \
		    "" 0 OK
	    return
	}
	if {$toprinter} {
	    puts "File $file not actually printed:"
	    puts "  $printcmd $file"
	    puts "  rm $file"
	}
	incr i
    }
}

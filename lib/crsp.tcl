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

#	$Id$

#	$Log$
#	Revision 1.7  1995/11/29 16:02:49  jussi
#	Removed constant window size definitions because they will produce
#	unexpected results on some window managers.
#
#	Revision 1.6  1995/11/20 22:40:16  jussi
#	Base tape offset now received from calling program.
#
#	Revision 1.5  1995/11/15 19:16:15  ravim
#	Changed old references to cstat* to crsp*.
#
#	Revision 1.4  1995/11/14 22:57:21  jussi
#	Listbox listing CRSP securities now shows CUSIP number; previously
#	the PERMNO was displayed which I had mistaken for the CUSIP number.
#
#	Revision 1.3  1995/11/10 22:01:25  jussi
#	Another small change.
#
#	Revision 1.2  1995/11/10 22:00:44  jussi
#	Minor fix.
#
#	Revision 1.1  1995/11/10 21:45:07  jussi
#	Initial revision.

############################################################

set crsp_seclst ""
set crsp_numsec 0
set crsp_selection ""

############################################################

proc crsp_unique_name {symbol} {
    return [string trim $symbol]
}

############################################################

proc crsp_extract_data {tapedrive filenum offset blocksize key file} {
    global sourceTypes crsp_status

    set indexFile [lindex $sourceTypes(CRSP) 2]
    set crsp_status "Extracting CRSP data..."
    crsp_setupStatus .crspstatus
    crsp_extract $tapedrive $filenum $offset $blocksize $indexFile $key $file
    catch {destroy .crspstatus}
}

############################################################

proc crsp_setupButtons {buttons} {
    global crsp_selection

    frame $buttons
    frame $buttons.row1
    pack $buttons.row1 -side top -pady 3m

    button $buttons.row1.select -text Select -width 10 -command {
	set crsp_selection [get_crsp_selection]
    }
    button $buttons.row1.cancel -text Cancel -width 10 -command {
	set crsp_selection ""
    }
    pack $buttons.row1.select $buttons.row1.cancel -side left -padx 3m
}

############################################################

proc get_crsp_selection {} {
    set err [catch { set owner [selection own] }]
    if {$err > 0} { return "" }
    if {$owner != ".crsp.seclst.list"} { return "" }
    set err [catch { set select [selection get] }]
    if {$err > 0} { return "" }
    set sel [list [string trim [string range $select 34 39]] \
	          [string trim [string range $select 0 32]]]
    return $sel
}

############################################################

proc crsp_setupFirmList {flist} {
    global crsp_selection

    frame $flist -relief groove -borderwidth 2
    listbox $flist.list -relief raised -borderwidth 2 \
	    -yscrollcommand "$flist.scroll set" -font 9x15 \
	    -selectmode single -width 53 -height 30
    scrollbar $flist.scroll -command "$flist.list yview"
    label $flist.msg -text "Database has 0 securities."
    pack $flist.msg -side top -pady 3m
    pack $flist.list -expand 1 -side left -fill both
    pack $flist.scroll -side right -fill y
    
    bind $flist.list <Double-Button-1> {
	set crsp_selection [get_crsp_selection]
    }
}

############################################################

proc crsp_scanFirmList {listb msg} {
    global sourceTypes
    global crsp_seclst crsp_numsec crsp_status

    $listb delete 0 end
    set crsp_seclst ""
    set crsp_numsec 0
    set indexFile [lindex $sourceTypes(CRSP) 2]
    set err [catch { set firms [open $indexFile] }]
    if {$err > 0} {
	dialog .noFile "No Index File" \
		"Cannot find index file $indexFile." \
		"" 0 OK
	return -1
    }

    set crsp_status "Reading tape index..."
    crsp_setupStatus .crspstatus

    while {[gets $firms line] >= 0} {
	lappend crsp_seclst [split $line ,]
	incr crsp_numsec
	if {$crsp_numsec % 500 == 0} {
	    $msg configure -text "Database has $crsp_numsec securities."
	    update
	}
    }
    close $firms

    $msg configure -text "Database has $crsp_numsec securities."
    update
    crsp_updateFirmListBox $listb

    return 1
}

############################################################

proc crsp_updateFirmListBox {listb} {
    global crsp_seclst crsp_status

    set crsp_status "Building securities list..."
    update

    $listb delete 0 end
    set n 0
    foreach sel $crsp_seclst {
	set name [string trim [lindex $sel 6] "\""]
	set cusip [lindex $sel 3]
	set begindate [lindex $sel 7]
	set enddate [lindex $sel 8]
	set listitem [format "%-32.32s  %-6.6s  %-4.4s  %-4.4s" \
		$name $cusip $begindate $enddate]
	$listb insert end $listitem
	if {$n % 500 == 0} { update }
	incr n
    }
    catch {destroy .crspstatus}
}

############################################################

proc crsp_setupStatus {stat} {
    global crsp_status
    toplevel $stat
    wm title $stat "Status"
    wm geometry $stat +150+400
    label $stat.msg -textvariable crsp_status
    pack $stat.msg -side top -padx 8m -pady 4m
    grab set $stat
    update
    # for some reason raising the window takes a long time,
    # on the order of one half or even one second
    # raise $stat
}

############################################################

proc crspMain {} {
    global crsp_seclst crsp_numsec crsp_selection

    if {[WindowVisible .crsp]} {
	return
    }

    set crsp_selection ""

    toplevel .crsp
    wm title .crsp "Select Security from CRSP Database"
    wm geometry .crsp +50+50
    selection clear .crsp

    frame .crsp.firms -relief groove -borderwidth 2
    crsp_setupFirmList .crsp.seclst
    pack .crsp.seclst -in .crsp.firms -side left -expand 1 -fill both
    pack .crsp.firms -side top -expand 1 -fill both -pady 1m

    crsp_setupButtons .crsp.buttons
    pack .crsp.buttons -side top -pady 3m
    update

    # scan firm index only if the firm list doesn't exist yet
    if {!$crsp_numsec} {
	if {[crsp_scanFirmList .crsp.seclst.list .crsp.seclst.msg] < 0} {
	    return "Error"
	}
    } else {
	.crsp.seclst.msg configure \
		-text "Database has $crsp_numsec securities."
	update
	crsp_updateFirmListBox .crsp.seclst.list
    }

    tkwait variable crsp_selection
    catch {destroy .crsp}

    return $crsp_selection
}

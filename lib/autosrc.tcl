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
#	Revision 1.1  1995/11/15 22:12:09  jussi
#	Initial revision.
#

############################################################

proc autoSourceAdd {source} {
    global name cusip other

    if {$source != "CRSP"} {
	NotImplemented
	return
    }

    # see if .autosrc window already exists; if so, just return
    set err [catch {wm state .autosrc}]
    if {!$err} { wm deiconify .autosrc; return }

    toplevel .autosrc
    wm title .autosrc "Add Data Streams"
    wm geometry .autosrc +150+150
    selection clear .autosrc

    frame .autosrc.top -relief groove -borderwidth 2
    frame .autosrc.bot
    pack .autosrc.top -side top -pady 5m -fill both -expand 1
    pack .autosrc.bot -side top -pady 5m -fill x
    frame .autosrc.bot.but
    pack .autosrc.bot.but -side top

    label .autosrc.top.label -text "Enter search criteria"
    pack .autosrc.top.label -side top -pady 5m

    label .autosrc.top.l1 -text "Name:"
    entry .autosrc.top.e1 -relief sunken -textvariable name -width 16
    label .autosrc.top.l2 -text "CUSIP:"
    entry .autosrc.top.e2 -relief sunken -textvariable cusip -width 10
    label .autosrc.top.l3 -text "Other:"
    entry .autosrc.top.e3 -relief sunken -textvariable other -width 10
    pack .autosrc.top.l1 .autosrc.top.e1 .autosrc.top.l2 .autosrc.top.e2 \
	     .autosrc.top.l3 .autosrc.top.e3 -side left -padx 2m \
	     -pady 5m -fill x -expand 1

    button .autosrc.bot.but.add -text "Find and Add" -width 15 -command {
	autoSourceAddCRSP $name $cusip $other
	destroy .autosrc
    }
    button .autosrc.bot.but.cancel -text Cancel -width 15 -command {
	destroy .autosrc
    }
    pack .autosrc.bot.but.add .autosrc.bot.but.cancel -side left -padx 5m

    tkwait window .autosrc
}

############################################################

proc autoSourceAddCRSP {name cusip other} {
    global sourceList sourceTypes

    if {$name == "" && $cusip == "" && $other == ""} {
	dialog .noSearchParams "No Parameters" \
		"Cannot add all streams at once." \
		"" 0 OK
	return
    }

    set indexFile [lindex $sourceTypes(CRSP) 2]
    set err [catch { set firms [open $indexFile] }]
    if {$err > 0} {
	dialog .noFile "No Index File" \
		"Cannot find index file $indexFile." \
		"" 0 OK
	return
    }

    while {[gets $firms line] >= 0} {
	set linelst [split $line ,]
	set namefield [string trim [lindex $linelst 6] "\""]
	set cusipfield [string range [lindex $linelst 3] 0 5]
	if {$name != "" && ![string match $name $namefield]} { continue }
	if {$cusip != "" && ![string match $cusip $cusipfield]} { continue }

	set dispname $namefield
	set source "CRSP"
	set key $cusipfield
	set schematype "CRSP-Schema"
	set schemafile [lindex $sourceTypes($source) 1]
	set cachefile [getCacheName $source $key]
	set evaluation 75
	set priority 50
	set command ""

	set err [catch {set exists $sourceList($dispname)}]
	if {!$err} {
	    puts "Already exists: \"$dispname\" ($source:$key)"
	    continue
	}

	set sourcedef [list $source $key $schematype $schemafile \
		$cachefile $evaluation $priority $command]

	set conflict [getSourceByCache $cachefile]
	if {$source != "UNIXFILE" && $conflict != ""} {
	    dialog .existsAlready "Data Stream Exists" \
		    "This definition conflicts\n\
		    with \"$conflict\"." "" 0 OK
	    return
	}

	puts "Adding definition for \"$dispname\" ($source:$key)"
	set "sourceList($dispname)" $sourcedef
    }

    saveSources
    updateSources

    close $firms
}

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
#	Revision 1.12  1995/12/12 01:46:46  ravim
#	Physical/logical schema support.
#
#	Revision 1.11  1995/12/11 18:02:56  ravim
#	Physical/logical schema distinction handled.
#
#	Revision 1.10  1995/11/29 15:53:29  jussi
#	Made inclusion of sourcedef.tcl conditional. Provided default values for
#	sourceTypes. Removed constant window size definitions because they
#	produce unexpected results on some window managers.
#
#	Revision 1.9  1995/11/24 21:39:42  jussi
#	Changed width of schema filename to 40. Added code to extract the
#	command (pathname actually) for UNIXFILE source type from a text
#	widget.
#
#	Revision 1.8  1995/11/24  07:44:49  ravim
#	Mapping between different types of data sources incorporated using
#	mapping tables.
#
#	Revision 1.7  1995/11/20 22:37:16  jussi
#	Reworked caching interface to include record ranges. Schema files
#	are now scanned (not imported) to find the schema type when a
#	data stream is defined. Interface to some extraction routines changed.
#
#	Revision 1.6  1995/11/15 22:12:47  jussi
#	Added link to auto-source-definition (autosrc.tcl).
#
#	Revision 1.5  1995/11/15 19:41:33  ravim
#	Removed call to cacheData; data is now cached when stream is
#	selected for visualization.
#
#	Revision 1.4  1995/11/14 23:16:41  jussi
#	Changed default values of source type and schema file when
#	defining a new data stream. Check for missing information
#	earlier than before when defining new data stream.
#
#	Revision 1.3  1995/11/14 22:51:11  jussi
#	Interfaced stream selection dialog into DefSource of macrodef.tk.
#	Minor other changes.
#
#	Revision 1.2  1995/11/10 22:02:14  jussi
#	Minor change.
#
#	Revision 1.1  1995/11/10 21:45:10  jussi
#	Initial revision.

############################################################

# Default values for data source types, redefined in sourcedef.tcl.

set sourceTypes(COMMAND) {{Unix Command Output} $schemadir/logical/COMMAND}
set sourceTypes(COMPUSTAT) {{Annual and Quarterly Company Financial Data} $schemadir/logical/COMPUSTAT compustat.idx}
set sourceTypes(CRSP) {{Security Data} $schemadir/logical/CRSP crsp_dsm94.idx}
set sourceTypes(ISSM) {{Historical Stock Data (Trades and Quotes} $schemadir/logical/ISSM-T issm.idx}
set sourceTypes(NETWORK) {{Network Server Output} $schemadir/logical/NETWORK}
set sourceTypes(SEQ) {{SEQ Query Output} $schemadir/logical/SEQ}
set sourceTypes(SQL) {{SQL Query Output} $schemadir/logical/SQL}
set sourceTypes(UNIXFILE) {{Unix File} $schemadir/logical/UNIXFILE}
set sourceTypes(WWW) {{World Wide Web} $schemadir/logical/WWW}

set sourceFile $datadir/sourcedef.tcl
if {[file exists $sourceFile]} {
    puts "Using source definition file $sourceFile"
    source $sourceFile
}

source $libdir/mapdef.tcl
source $libdir/autosrc.tcl
source $libdir/issm.tk
source $libdir/cstat.tk
source $libdir/crsp.tcl

############################################################

# format of items in sourceList: 
#   sourceList(dispname) = {source key schematype schemafile \
#                           cachefile evaluation priority command}
#
# dispname is the name of the source as given by the user
# source is the type of the data source (SEQ, SQL, CRSP, etc.)
# key is used to index the source; e.g. company name for stock databases
# schematype is the name of the schema (e.g. ISSM-Trade)
# schemafile is the name of the schemafile
# cachefile is the name of the file where Devise caches the data
# evaluation is number 0 to 100 indicating the urgency of materializing
#    this data; 100 means immediately evaluation, 0 means evaluation
#    only when data needs to be displayed
# priority is number 0 to 100 indicating the cost of re-materializing this
#    data; 100 means infinite cost (cache file must not be kicked out from
#    the disk), 0 means data is inexpensive to regenerate (cache file can
#    be kicked out from the disk without consequences)
# command defines source-specific information:
#    for SEQ and SQL, this field contains the query
#    for UNIX, this field contains the Unix command
#    for WWW, this field contains the URL of the Web resource
#    for UNIXFILE, this field contains the root directory while the key
#      contains the filename appended to the root directory

############################################################

proc saveSources {} {
    global sourceFile sourceList sourceTypes

    set f [open $sourceFile w]
    puts $f "# Data source types"
    puts $f ""
    foreach type [lsort [array names sourceTypes]] {
	puts $f "set \"sourceTypes($type)\" \{$sourceTypes($type)\}"
    }
    puts $f ""
    puts $f "# Data sources"
    puts $f ""
    foreach dispName [lsort [array names sourceList]] {
	puts $f "set \"sourceList($dispName)\" \{$sourceList($dispName)\}"
    }
    close $f
}

############################################################

proc defineStream {base edit} {
    global sourceList sourceTypes editonly oldDispName oldCachefile
    global dispname source key schemafile evaluation priority command

    # see if .srcdef window already exists; if so, just return
    set err [catch {wm state .srcdef}]
    if {!$err} { wm deiconify .srcdef; return }

    toplevel .srcdef
    if {$edit} {
	wm title .srcdef "Edit Data Stream Definition"
    } elseif {$base != ""} {
	wm title .srcdef "Copy Data Stream Definition"
    } else {
	wm title .srcdef "Define Data Stream"
    }
    wm geometry .srcdef +150+150
    selection clear .srcdef

    set editonly $edit
    if {$editonly} { set editstate disabled } else { set editstate normal }

    frame .srcdef.top -relief groove -borderwidth 2
    frame .srcdef.but
    pack .srcdef.top -side top -pady 3m -fill both -expand 1
    pack .srcdef.but -side top -pady 3m -fill x
    frame .srcdef.but.row1
    pack .srcdef.but.row1 -side top

    # setup default values; use definition given as parameter if possible
    set dispname ""
    set source "UNIXFILE"
    set key ""
    set schemafile [lindex $sourceTypes($source) 1]
    set cachefile ""
    set evaluation 100
    set priority 50
    set command ""

    set err [catch {set sourcedef $sourceList($base)}]
    if {$base != "" && $err == 0} {
	set dispname $base
	set source [lindex $sourcedef 0]
	set key [lindex $sourcedef 1]
	set schemafile [lindex $sourcedef 3]
	set cachefile [lindex $sourcedef 4]
	set evaluation [lindex $sourcedef 5]
	set priority [lindex $sourcedef 6]
	set command [lindex $sourcedef 7]
    }
	
    set oldDispName $base
    set oldCachefile $cachefile

    frame .srcdef.top.row1
    frame .srcdef.top.row2
    frame .srcdef.top.row3
    frame .srcdef.top.row4 -relief groove -borderwidth 2
    frame .srcdef.top.row5
    pack .srcdef.top.row1 .srcdef.top.row2 .srcdef.top.row3 \
	     .srcdef.top.row4 .srcdef.top.row5 -side top -pady 1m

    label .srcdef.top.row1.l1 -text "Display Name:"
    entry .srcdef.top.row1.e1 -relief sunken -textvariable dispname \
	    -width 40
    pack .srcdef.top.row1.l1 .srcdef.top.row1.e1 -side left -padx 3m \
	    -fill x -expand 1

    label .srcdef.top.row2.l1 -text "Data Source:"
    menubutton .srcdef.top.row2.e1 -relief raised -textvariable source \
	    -width 12 -menu .srcdef.top.row2.e1.menu
    label .srcdef.top.row2.l2 -text "Key:"
    entry .srcdef.top.row2.e2 -relief sunken -textvariable key \
	    -width 16
    button .srcdef.top.row2.b1 -text "Select..." -width 10 -command {
	if {$source == ""} {
	    dialog .note "Select Source" \
		    "Select data source first." "" 0 OK
	    return
	}
	set sel [selectSourceKey $source]
	if {$sel != ""} {
	    set key [lindex $sel 0]
	    set dispname [lindex $sel 1]
	}
    }
    pack .srcdef.top.row2.l1 .srcdef.top.row2.e1 \
	    .srcdef.top.row2.l2 .srcdef.top.row2.e2 .srcdef.top.row2.b1 \
	    -side left -padx 2m -fill x -expand 1

    # Create menu of valid source types
    menu .srcdef.top.row2.e1.menu -tearoff 0
    foreach sourcetype [lsort [array names sourceTypes]] {
	.srcdef.top.row2.e1.menu add radiobutton -label $sourcetype \
		-variable source -value $sourcetype -command {
	    set schemafile [lindex $sourceTypes($source) 1]
	}
    }
    tk_menuBar .srcdef .srcdef.sourcemenu

    label .srcdef.top.row3.l1 -text "Schema File:"
    entry .srcdef.top.row3.e1 -relief sunken -textvariable schemafile \
	    -width 40
    button .srcdef.top.row3.b1 -text "Select..." -width 10 -command {
	global schemadir fsBox
	set fsBox(path) $schemadir/logical
	set fsBox(pattern) *
	set file [FSBox "Select schema file"]
	if {$file != ""} { set schemafile $file }
    }
    pack .srcdef.top.row3.l1 .srcdef.top.row3.e1 .srcdef.top.row3.b1 \
	    -side left -padx 2m -fill x -expand 1

    scale .srcdef.top.row4.s1 -label "Evaluation Factor" -from 0 -to 100 \
	    -length 10c -orient horizontal -variable evaluation \
	    -tickinterval 20 -resolution 5
    scale .srcdef.top.row4.s2 -label "Cache Priority" -from 0 -to 100 \
	    -length 10c -orient horizontal -variable priority \
	    -tickinterval 20 -resolution 5
    pack .srcdef.top.row4.s1 .srcdef.top.row4.s2 -side left -padx 2m \
	    -fill x -expand 1

    label .srcdef.top.row5.l1 -text "Info/Command:"
    text .srcdef.top.row5.e1 -relief sunken -borderwidth 2 \
	    -width 54 -height 3
    .srcdef.top.row5.e1 insert end $command
    pack .srcdef.top.row5.l1 .srcdef.top.row5.e1 -side left -padx 3m \
	    -fill x -expand 1

    button .srcdef.but.ok -text OK -width 10 -command {
	if {$dispname == ""} {
	    dialog .noName "Missing Information" \
		    "Please enter all requested information." \
		    "" 0 OK
	    return
	}
	set cachefile [getCacheName $source $key]
	set command [string trim [.srcdef.top.row5.e1 get 1.0 end]]
	set newschematype [scanSchema $schemafile]
	if {$newschematype == ""} { return }
	set schematype $newschematype

	set sourcedef [list $source $key $schematype $schemafile \
		$cachefile $evaluation $priority $command]
	
	set conflict [getSourceByCache $cachefile]
	if {!$editonly && $source != "UNIXFILE" && $conflict != ""} {
	    dialog .existsAlready "Data Stream Exists" \
		    "This definition conflicts\n\
		    with \"$conflict\"." "" 0 OK
	    return
	}
	if {$editonly} {
	    if {[file exists $oldCachefile] \
		    && $cachefile != $oldCachefile} {
		set but [dialog .uncacheSource "Removing Old Cache" \
			"Source and/or key changed.\n\
			Okay to remove old cache file?" \
			"" 1 OK Cancel]
		if {$but > 0} {
		    return
		}
		exec rm $oldCachefile
	    }
	    unset sourceList($oldDispName)
	}
	set err [catch {set exists $sourceList($dispname)}]
	if {$err == 0} {
	    dialog .sourceExists "Data Stream Exists" \
		    "Data Stream \"$dispname\" exists already." \
		    "" 0 OK
	    return
	}
	set "sourceList($dispname)" $sourcedef
	saveSources
	updateSources
	destroy .srcdef
    }
    button .srcdef.but.cancel -text Cancel -width 10 \
	    -command { destroy .srcdef }
    pack .srcdef.but.ok .srcdef.but.cancel -in .srcdef.but.row1 \
	    -side left -padx 7m

    tkwait visibility .srcdef
}

############################################################

proc scanSchema {schemafile} {
    global schemadir
    if {[catch { set f [open $schemafile "r"] }] > 0} {
	dialog .noFile "No Schema File" \
		"Cannot open schema file $schemafile." \
		"" 0 OK
	return ""
    }

    set type ""

    while {[gets $f line] >= 0} {
	if {[lindex $line 0] == "type"} {
	    set type [lindex $line 1]
	    break
	} elseif {[lindex $line 0] == "physical"} {
	    return [scanSchema [lindex $line 1]]
	}
    }

    close $f

    puts "Schema file $schemafile has type $type"

    return $type
}

############################################################

proc getSourceByCache {cachefile} {
    global sourceList
    foreach dispName [lsort [array names sourceList]] {
	set cachefile2 [lindex $sourceList($dispName) 4]
	if {$cachefile == $cachefile2} {
	    return $dispName
	}
    }
    return ""
}

############################################################

proc isCached {dispname startrec endrec} {
    global sourceList

    set sourcedef $sourceList($dispname)

    set source [lindex $sourcedef 0]
    set key [lindex $sourcedef 1]
    set cachefile [lindex $sourcedef 4]
    set command [lindex $sourcedef 7]
    
    if {$source == "UNIXFILE"} {
	# first line of command specifies directory
	set dir [lindex [split $command \n] 0]
	if {$dir == ""} {
	    set dir "."
	}
	return $dir/$key
    }

    # see if whole data stream is cached
    if {[file exists $cachefile]} {
	return $cachefile
    }

    # see if record range is cached
    set pattern [format "%s*" $cachefile]
    foreach cachefile [glob -nocomplain $pattern] {
	if {[scan $cachefile "*.dat.%d.%d" start end] == 2} {
	    if {$start <= $startrec && $end >= $endrec} {
		return $cachefile
	    }
	}
    }

    return ""
}

############################################################

proc cacheData {dispname startrec endrec} {
    global sourceList sourceConfig errorInfo

    set cached [isCached $dispname $startrec $endrec]
    if {$cached != ""} {
	return $cached
    }

    set sourcedef $sourceList($dispname)

    set source [lindex $sourcedef 0]
    set key [lindex $sourcedef 1]
    set cachefile [lindex $sourcedef 4]
    set priority [lindex $sourcedef 6]
    set command [lindex $sourcedef 7]
    
    set cmd ""

    if {$source == "COMMAND"} {
	set cmd "exec $command > $cachefile"
    } elseif {$source == "WWW"} {
	set prog [lindex $sourceConfig($source) 0]
	set cmd "exec $prog $command > $cachefile"
    } elseif {$source == "SEQ"} {
	set prog [lindex $sourceConfig($source) 0]
	set port [lindex $sourceConfig($source) 1]
	set cmd "exec $prog $port $command > $cachefile"
    } elseif {$source == "SQL"} {
	set prog [lindex $sourceConfig($source) 0]
	set port [lindex $sourceConfig($source) 1]
	set cmd "exec $prog $port $command > $cachefile"
    } elseif {$source == "NETWORK"} {
	set prog [lindex $sourceConfig($source) 0]
	set port [lindex $sourceConfig($source) 1]
	set cmd "exec $prog $port > $cachefile"
    } elseif {$source == "ISSM"} {
	set tapedrive [lindex $sourceConfig($source) 0]
	set filenum [lindex $sourceConfig($source) 1]
	set offset [lindex $sourceConfig($source) 2]
	set blocksize [lindex $sourceConfig($source) 3]
	set cmd "issm_extract_data $tapedrive $filenum \
		$offset $blocksize $key $cachefile"
    } elseif {$source == "COMPUSTAT"} {
	set tapedrive [lindex $sourceConfig($source) 0]
	set filenum [lindex $sourceConfig($source) 1]
	set offset [lindex $sourceConfig($source) 2]
	set blocksize [lindex $sourceConfig($source) 3]
	set cmd "cstat_extract_data $tapedrive $filenum \
		$offset $blocksize $key $cachefile"
    } elseif {$source == "CRSP"} {
	set tapedrive [lindex $sourceConfig($source) 0]
	set filenum [lindex $sourceConfig($source) 1]
	set offset [lindex $sourceConfig($source) 2]
	set blocksize [lindex $sourceConfig($source) 3]
	set cmd "crsp_extract_data $tapedrive $filenum \
		$offset $blocksize $key $cachefile"
    }

    if {$cmd == ""} {
	dialog .noCmd "Cannot Extract" \
		"No extraction command defined for this data source." \
		"" 0 OK
	return ""
    } else {
	update
	if {[catch { eval $cmd }] > 0} {
	    puts "Cannot execute command:"
	    puts "  $cmd"
	    dialog .error "Error Occurred" \
		    "An error occurred in extracting data\n\
		    from the data source. See text window for\n\
		    the error message." \
		    "" 0 OK
	    uncacheData $dispname
	    return ""
	}
    }

    return $cachefile
}

############################################################

proc uncacheData {dispname} {
    global sourceList sourceConfig

    set sourcedef $sourceList($dispname)

    set cachefile [lindex $sourcedef 4]

    set pattern [format "%s*" $cachefile]
    foreach cachefile [glob -nocomplain $pattern] {
	exec rm $cachefile
    }
}

############################################################

proc getCacheName {source key} {
    global cachedir

    if {$source == "UNIXFILE"} {
	return /dev/null
    }
    if {$source == "COMMAND" || $source == "SEQ" || $source == "SQL" \
	|| $source == "CRSP" || $source == "WWW"} {
	return [format "%s/%s.%s.dat" $cachedir [string trim $source] \
		[string trim $key]]
    }
    if {$source == "COMPUSTAT"} {
	return [format "%s/%s.%s.dat" $cachedir [string trim $source] \
		[cstat_unique_name $key]]
    }
    if {$source == "ISSM"} {
	return [format "%s/%s.%s.dat" $cachedir [string trim $source] \
		[issm_unique_name $key]]
    }

    dialog .error "Incorrect Data Source" \
	    "An invalid data source has been selected." \
	    "" 0 OK
}

############################################################

proc selectSourceKey {source} {
    if {$source == "UNIXFILE"} {
	return [selectUnixFile]
    }
    if {$source == "COMMAND" || $source == "SEQ" || $source == "SQL" \
	|| $source == "WWW"} {
	return "default Default"
    }
    if {$source == "COMPUSTAT"} {
	return [cstatMain]
    }
    if {$source == "ISSM"} {
	return [issmMain]
    }
    if {$source == "CRSP"} {
	return [crspMain]
    }

    dialog .error "Incorrect Data Source" \
	    "An invalid data source has been selected." \
	    "" 0 OK
    return ""
}

############################################################

proc selectUnixFile {} {
    global schemafile fsBox datadir schemadir

    # Get file name
    set fsBox(path) $datadir
    set fsBox(pattern) *
    set file [FSBox "Select data file"]

    # No file selected?
    if {$file == ""} {
	return ""
    }

    set fsBox(path) $schemadir/logical
    set fsBox(pattern) *
    set file2 [FSBox "Select schema file"]
    
    if {$file2 == ""} {
	return ""
    }

    set schemafile $file2
    # command is a text widget so we must insert the text
    # into the widget; setting the variable not enough
    .srcdef.top.row5.e1 delete 1.0 end
    .srcdef.top.row5.e1 insert end [file dirname $file]
    set file [file tail $file]

    return "$file $file"
}

############################################################

proc selectStream {} {
    global streamSelected sourceTypes MapTable

    # see if .srcsel window already exists; if so, just return
    set err [catch {wm state .srcsel}]
    if {!$err} { wm deiconify .srcsel; return }

    toplevel .srcsel
    wm title .srcsel "Data Streams"
    wm geometry .srcsel +100+100
    selection clear .srcsel

    frame .srcsel.mbar -relief raised -borderwidth 2
    frame .srcsel.top
    frame .srcsel.bot
    pack .srcsel.mbar -side top -fill x
    pack .srcsel.top -side top -fill both -expand 1
    pack .srcsel.bot -side top -fill x -pady 5m

    frame .srcsel.bot.but
    pack .srcsel.bot.but -side top

    menubutton .srcsel.mbar.define -text Define -menu .srcsel.mbar.define.menu
    menubutton .srcsel.mbar.stream -text Stream -menu .srcsel.mbar.stream.menu
    menubutton .srcsel.mbar.display -text Display -menu .srcsel.mbar.display.menu
    menubutton .srcsel.mbar.follow -text "Follow to" -menu .srcsel.mbar.follow.menu
    menubutton .srcsel.mbar.help -text Help -menu .srcsel.mbar.help.menu
    pack .srcsel.mbar.define .srcsel.mbar.stream .srcsel.mbar.display \
	    .srcsel.mbar.follow .srcsel.mbar.help -side left

    menu .srcsel.mbar.define.menu -tearoff 0
    .srcsel.mbar.define.menu add command -label "New..." \
	    -command { defineStream "" 0 }
    .srcsel.mbar.define.menu add cascade -label "Auto Add" \
	    -menu .srcsel.mbar.define.menu.auto

    menu .srcsel.mbar.define.menu.auto -tearoff 0
    foreach sourcetype [lsort [array names sourceTypes]] {
	.srcsel.mbar.define.menu.auto add command -label $sourcetype \
		-command "autoSourceAdd $sourcetype"
    }

    menu .srcsel.mbar.follow.menu -tearoff 0
    foreach mtype [lsort [array names MapTable]] {
	.srcsel.mbar.follow.menu add command -label $mtype \
		-command "mapFollow $mtype"
    }

    menu .srcsel.mbar.stream.menu -tearoff 0
    .srcsel.mbar.stream.menu add command -label "Edit..." -command {
	set dispname [getSelectedSource]
	if {$dispname == ""} {
	    dialog .note "Note" "Select stream to be edited first." "" 0 OK
	    return
	}
	defineStream $dispname 1
    }
    .srcsel.mbar.stream.menu add command -label "Copy..." -command {
	set dispname [getSelectedSource]
	if {$dispname == ""} {
	    dialog .note "Note" "Select stream to copy first." "" 0 OK
	    return
	}
	defineStream $dispname 0
    }
    .srcsel.mbar.stream.menu add command -label Delete -command {
	set dispname [getSelectedSource]
	if {$dispname == ""} {
	    dialog .note "Note" "Select stream to delete first." "" 0 OK
	    return
	}
	set but [dialog .confirm "Confirm Stream Deletion" \
		"Delete stream \"$dispname\"?" "" 1 Yes No]
	if {$but > 0} {
	    return
	}
	if {[catch {unset "sourceList($dispname)"}] == 0} {
	    saveSources
	    updateSources
	}
    }

    menu .srcsel.mbar.display.menu -tearoff 0
    .srcsel.mbar.display.menu add command -label "Show All" -command {}
    .srcsel.mbar.display.menu add command -label "Limit" -command {}
    .srcsel.mbar.display.menu add separator
    .srcsel.mbar.display.menu add command -label "Sort" -command {}
    .srcsel.mbar.display.menu add separator
    .srcsel.mbar.display.menu add command -label "Verbose" -command {}

    menu .srcsel.mbar.help.menu -tearoff 0
    .srcsel.mbar.help.menu add command -label Help -command {
	dialog .help "Help" \
		"This dialog lets you choose one of the defined data streams\
		for visualization. A stream is selected by either\
		double-clicking on it, or by clicking on it once and\
		pressing the Select button. Press the Cancel button\
		to return without selecting a data stream.\n\n\
		Choose Stream/New to define a new data stream using a blank\
		template.\n\n\
		Stream/Edit lets you edit an existing data stream. Select\
		data stream first with the mouse.\n\n\
		You can copy an existing data stream and use its definition\
		as a template for a new data stream with Stream/Copy. Select\
		template with the mouse first.\n\n\
		Stream/Delete lets you delete the definition of a selected\
		data stream. The program asks for confirmation before\
		actually removing the definition." \
		"" 0 OK
    }

    tk_menuBar .srcsel.mbar .srcsel.mbar.define .srcsel.mbar.stream \
	    .srcsel.mbar.display .srcsel.mbar.follow .srcsel.mbar.help

    listbox .srcsel.top.list -relief raised -borderwidth 2 \
	    -yscrollcommand ".srcsel.top.scroll set" -font 9x15 \
	    -selectmode single -width 61 -height 22
    scrollbar .srcsel.top.scroll -command ".srcsel.top.list yview"
    pack .srcsel.top.list -side left -fill both -expand 1
    pack .srcsel.top.scroll -side right -fill y
    bind .srcsel.top.list <Double-Button-1> {
	set streamSelected [getSelectedSource]
    }

    updateSources

    button .srcsel.bot.but.select -text Select -width 10 -command {
	set streamSelected [getSelectedSource]
    }
    button .srcsel.bot.but.cancel -text Cancel -width 10 -command {
	set streamSelected ""
    }
    pack .srcsel.bot.but.select .srcsel.bot.but.cancel -side left -padx 3m

    set streamSelected ""
    tkwait variable streamSelected
    catch {destroy .srcsel}

    return $streamSelected
}

############################################################

proc updateSources {} {
    global sourceList

    if {[catch {wm state .srcsel}] > 0} {
	return
    }

    .srcsel.top.list delete 0 end
    foreach dispName [lsort [array names sourceList]] {
	set source [lindex $sourceList($dispName) 0]
	set cachefile [lindex $sourceList($dispName) 4]
	set cached [file exists $cachefile]
	if {$cached} { set cached "Cached" } else { set cached "" }
	set item [format "%-40.40s  %-10.10s  %-6.6s" \
		$dispName $source $cached]
	.srcsel.top.list insert end $item
    }
}

############################################################

proc getSelectedSource {} {
    set err [catch { set owner [selection own] }]
    if {$err > 0} { return "" }
    if {$owner != ".srcsel.top.list"} { return "" }
    set err [catch { set select [selection get] }]
    if {$err > 0} { return "" }
    set dispName [string range $select 0 40]
    set dispName [string trimright $dispName]
    return $dispName
}

############################################################

proc selectSelectedSource {} {
    global sourceList
    set dispName [getSelectedSource]
    set err [catch {set sourceDef $sourceList($dispName)}]
    if {$err > 0} { return }
    puts "Selected: $dispName"
    puts "  $sourceDef"
}

############################################################

proc mapFollow {newtype} {
    global button evaluation priority MapTable sourceList sourceTypes

    set curr [getSelectedSource]
    if {$curr == ""} {
	dialog .note "Select Source" \
		"Select data source first." "" 0 OK
	return
    } 
    set oldtype [lindex $sourceList($curr) 0]
    set oldkey [lindex $sourceList($curr) 1]
    if {$oldtype == $newtype} {
	dialog .note "Error" "$oldtype stream for the current selection \
		already exists in the list."  "" 0 OK
	return
    }

    # Check if a mapping table exists between oldtype and newtype
    if {[info exists MapTable($oldtype)] == 0} {
	dialog .note "Error" "The mapping table between $oldtype and \
		$newtype does not exist." "" 0 OK
	return
    }

    set num 0
    set len [llength $MapTable($oldtype)]
    while {$num < $len} {
	set elem [lindex $MapTable($oldtype) $num]
	set typ [lindex $elem 0]
	if {$typ == $newtype} {
	    break
	}
	incr num
    }

    if {$num == $len} {
	dialog .note "Error" "The mapping table between $oldtype and \
		$newtype does not exist." "" 0 OK
	return
    }

    # Use the table to find the index for the new type
    set mapfile [lindex $elem 1]
    set oldidx [lindex $elem 2]
    set newidx [lindex $elem 3]

    set newkeylist ""
    set f [open $mapfile r]
    while {[gets $f line] != -1} {
	set coldkey [lindex $line $oldidx]
	if {$coldkey == $oldkey} {
	    set cnewkey [lindex $line $newidx]
	    set newkeylist [lappend newkeylist $cnewkey]
	}
    }
    close $f

    # How many matches were found?
    set len [llength $newkeylist]
    if {$len == 0} {
	dialog .note "Result of Mapping" "The mapping table currently \
		does not contain any match for the selected item." "" 0 OK
	return
    }

    dialog .note "Result of Mapping" "The mapping table contains $len \
	    entry(ies) corresponding to the selected item. Do you wish \
	    to add them to the list now ?" "" 0 YES NO

    if {$button == 1} {return}

    set num 0
    foreach itm $newkeylist {
	set dname "$newtype$num $curr"
	set sfile [lindex $sourceTypes($newtype) 1]
	set stype [scanSchema $sfile]
	set cname [getCacheName $newtype $itm]
	set sdef [list $newtype $itm $stype $sfile $cname 100 50 {}]
	set "sourceList($dname)" $sdef

	incr num
    }
    saveSources
    updateSources
    dialog .note "Note" "The names on the list have been automatically \
	    generated. You may change them to more meaningful names by \
	    clicking on Edit." "" 0 OK

}

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

set sourceFile $libdir/sourcedef.tcl
if {![file exists $sourceFile]} {
    puts "Cannot read definition file $sourceFile"
    exit 1
}
source $sourceFile

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
    global dispname source key schematype schemafile
    global evaluation priority command

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
    wm minsize .srcdef 630 320
    wm maxsize .srcdef 630 320
    wm geometry .srcdef =630x320+150+150
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
    set schematype "n/a"
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
	set schematype [lindex $sourcedef 2]
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
		-variable source -value $sourcetype \
		-command updateSchemaSelection
    }
    tk_menuBar .srcdef .srcdef.sourcemenu

    label .srcdef.top.row3.l1 -text "Schema File:"
    entry .srcdef.top.row3.e1 -relief sunken -textvariable schemafile \
	    -width 32
    label .srcdef.top.row3.l2 -text "Schema:"
    label .srcdef.top.row3.e2 -textvariable schematype -width 20
    pack .srcdef.top.row3.l1 .srcdef.top.row3.e1 \
	    .srcdef.top.row3.l2 .srcdef.top.row3.e2 -side left -padx 2m \
	    -fill x -expand 1

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
	#set newschematype [readSchema $schemafile]
	set newschematype ""
	if {$newschematype != ""} {
	    set schematype $newschematype
	}
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
	cacheData $dispname
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

proc updateSchemaSelection {} {
    global sourceTypes source schemafile schematype
    set schemafile [lindex $sourceTypes($source) 1]
    #set newschematype [readSchema $schemafile]
    set newschematype ""
    if {$newschematype != ""} {
	set schematype $newschematype
    }
}

############################################################

proc readSchema {schemafile} {
    if {![file exists $schemafile]} {
	dialog .noFile "No Schema File" \
		"Schema file $schemafile does not exist." \
		"" 0 OK
	return ""
    }

    set result [DEVise importFileType $schemafile]
    if {$result == ""} {
	dialog .schemaFileError "Schema File Error" \
		"Error reading schema file $schemafile."
	return ""
    }

    return $result
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

proc cacheData {dispname} {
    global sourceList sourceConfig errorInfo

    set err [catch {set sourcedef $sourceList($dispname)}]
    if {$err > 0} {
	puts "Error -- source being cached nonexistent."
	return ""
    }

    set source [lindex $sourcedef 0]
    set key [lindex $sourcedef 1]
    set schematype [lindex $sourcedef 2]
    set schemafile [lindex $sourcedef 3]
    set cachefile [lindex $sourcedef 4]
    set evaluation [lindex $sourcedef 5]
    set priority [lindex $sourcedef 6]
    set command [lindex $sourcedef 7]
    
    if {[file exists $cachefile]} {
	puts "Data stream $source:$key already cached."
	if {$source == "UNIXFILE"} {
	    # first line of command specifies directory
	    set dir [lindex [split $command \n] 0]
	    if {$dir == ""} {
		set dir "."
	    }
	    return $dir/$key
	}
	return $cachefile
    }

    set but [dialog .cacheData "Caching Data" \
	    "Cache source $source:$key to disk now?" \
	    "" 1 Yes No]
    if {$but == 1} {
	puts "Caching not performed."
	return ""
    }

    update

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
	set blocksize [lindex $sourceConfig($source) 2]
	set cmd "issm_extract_data $tapedrive $filenum \
		$blocksize $key $cachefile"
    } elseif {$source == "COMPUSTAT"} {
	set tapedrive [lindex $sourceConfig($source) 0]
	set filenum [lindex $sourceConfig($source) 1]
	set blocksize [lindex $sourceConfig($source) 2]
	set cmd "cstat_extract_data $tapedrive $filenum \
		$blocksize $key $cachefile"
    } elseif {$source == "CRSP"} {
	set tapedrive [lindex $sourceConfig($source) 0]
	set filenum [lindex $sourceConfig($source) 1]
	set blocksize [lindex $sourceConfig($source) 2]
	set cmd "crsp_extract_data $tapedrive $filenum \
		$blocksize $key $cachefile"
    }

    if {$cmd == ""} {
	dialog .noCmd "Cannot Extract" \
		"No extraction command defined for this data source." \
		"" 0 OK
	return ""
    } else {
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

    set err [catch {set sourcedef $sourceList($dispname)}]
    if {$err > 0} {
	puts "Error -- source being uncached nonexistent."
	return
    }

    set cachefile [lindex $sourcedef 4]
    
    if {![file exists $cachefile]} {
	return
    }

    exec rm $cachefile
}

############################################################

proc getCacheName {source key} {
    global datadir

    if {$source == "UNIXFILE"} {
	return /dev/null
    }
    if {$source == "COMMAND" || $source == "SEQ" || $source == "SQL" \
	|| $source == "CRSP" || $source == "WWW"} {
	return [format "%s/%s.%s.dat" $datadir [string trim $source] \
		[string trim $key]]
    }
    if {$source == "COMPUSTAT"} {
	return [format "%s/%s.%s.dat" $datadir [string trim $source] \
		[cstat_unique_name $key]]
    }
    if {$source == "ISSM"} {
	return [format "%s/%s.%s.dat" $datadir [string trim $source] \
		[issm_unique_name $key]]
    }

    dialog .error "Incorrect Data Source" \
	    "An invalid data source has been selected." \
	    "" 0 OK
}

############################################################

proc selectSourceKey {source} {
    if {$source == "UNIXFILE"} {
	return "default.dat Default"
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

proc selectStream {} {
    global streamSelected

    # see if .srcsel window already exists; if so, just return
    set err [catch {wm state .srcsel}]
    if {!$err} { wm deiconify .srcsel; return }

    toplevel .srcsel
    wm title .srcsel "Data Streams"
    wm minsize .srcsel 575 300
    wm maxsize .srcsel 575 800
    wm geometry .srcsel =575x500+100+100
    selection clear .srcsel

    frame .srcsel.mbar -relief raised -borderwidth 2
    frame .srcsel.top
    frame .srcsel.bot
    pack .srcsel.mbar -side top -fill x
    pack .srcsel.top -side top -fill both -expand 1
    pack .srcsel.bot -side top -fill x -pady 5m

    frame .srcsel.bot.but
    pack .srcsel.bot.but -side top

    menubutton .srcsel.mbar.stream -text Stream -menu .srcsel.mbar.stream.menu
    menubutton .srcsel.mbar.display -text Display -menu .srcsel.mbar.display.menu
    menubutton .srcsel.mbar.help -text Help -menu .srcsel.mbar.help.menu
    pack .srcsel.mbar.stream .srcsel.mbar.display .srcsel.mbar.help -side left

    menu .srcsel.mbar.stream.menu -tearoff 0
    .srcsel.mbar.stream.menu add command -label "New..." \
	    -command { defineStream "" 0 }
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

    tk_menuBar .srcsel.mbar .srcsel.mbar.stream \
	    .srcsel.mbar.display .srcsel.mbar.help

    listbox .srcsel.top.list -relief raised -borderwidth 2 \
	    -yscrollcommand ".srcsel.top.scroll set" -font 9x15 \
	    -selectmode single
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

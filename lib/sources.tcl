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

#	$Id$

#	$Log$
#	Revision 1.35  1996/06/20 17:20:15  guangshu
#	Fixed small bug.
#
#	Revision 1.34  1996/06/20 17:13:58  guangshu
#	Added support for derived TData sets (statistics for now).
#
#	Revision 1.33  1996/06/04 14:22:08  wenger
#	Ascii data can now be read from session files (or other files
#	where the data is only part of the file); added some assertions
#	to check for pointer alignment in functions that rely on this;
#	Makefile changes to make compiling with debugging easier.
#
#	Revision 1.32  1996/05/16 21:39:07  wenger
#	implemented saving schemas ans session file for importing tdata
#
#	Revision 1.31  1996/05/11 03:02:06  jussi
#	Added CRSP_NSDQ data source. Changes made towards making
#	caching of multiple data streams in one step possible.
#	Improved help screen.
#
#	Revision 1.30  1996/04/10 03:06:59  jussi
#	Checking of whether a top-level window already exists is now
#	made via WindowVisible (util.tcl).
#
#	Revision 1.29  1996/02/25 23:03:21  jussi
#	When data stream is uncache, index files and gdata files are
#	removed as well.
#
#	Revision 1.28  1996/02/05 19:55:40  jussi
#	Location of dialog boxes changed a little.
#
#	Revision 1.27  1996/01/26 18:45:53  jussi
#	Minor fix.
#
#	Revision 1.26  1996/01/25 17:45:53  jussi
#	Refined 'query-changed' condition when editing a stream
#	definition.
#
#	Revision 1.25  1996/01/19 18:33:40  jussi
#	Function selectStream now takes an optional title parameter
#	which is displayed at the top of the data stream list if
#	it is non-empty.
#
#	Revision 1.24  1996/01/18 17:22:28  jussi
#	Minor fix.
#
#	Revision 1.23  1996/01/17 18:04:41  jussi
#	In administrator mode (UserMode == 0), physical schemas are
#	displayed by default.
#
#	Revision 1.22  1996/01/15 18:06:42  jussi
#	Minor fix.
#
#	Revision 1.21  1996/01/13 23:54:53  jussi
#	Recent changes had caused Unix files to show up as uncached
#	in the stream selection list. Fixed this.
#
#	Revision 1.20  1996/01/13 20:57:27  jussi
#	Fixed minor bugs. Added CachePrune function.
#
#	Revision 1.19  1996/01/13 03:18:31  jussi
#	Refined the support for WWW. Cache files are invalidated when
#	query (URL) is updated.
#
#	Revision 1.18  1996/01/12 16:20:26  jussi
#	Old command value was extracted incorrectly -- fixed.
#
#	Revision 1.17  1996/01/11 21:00:59  jussi
#	No major changes -- improvements in coding style and organization.
#
#	Revision 1.16  1996/01/11 16:04:08  jussi
#	Added button for uncaching data.
#
#	Revision 1.15  1996/01/10 00:41:44  jussi
#	Added support for automatically generated SEQ schemas when a SEQ
#	query is edited.
#
#	Revision 1.14  1996/01/09 16:37:30  jussi
#	Changed call to seq_extract.
#
#	Revision 1.13  1995/12/29 17:26:35  jussi
#	Replaced absolute path names to schema dir with $schemadir.
#
#	Revision 1.12  1995/12/12 01:46:46  ravim
#	Physical/logical schema support.
#
#	Revision 1.11  1995/12/11 18:02:56  ravim
#	Physical/logical schema distinction handled.
#
#	Revision 1.10  1995/11/29 15:53:29  jussi
#	Made inclusion of sourcedef.tcl conditional. Provided default values
#	for sourceTypes. Removed constant window size definitions because
#	they produce unexpected results on some window managers.
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

set sourceTypes(COMMAND) "{Unix Command Output} $schemadir/logical/COMMAND"
set sourceTypes(COMPUSTAT) "{Annual and Quarterly Company Financial Data} $schemadir/logical/COMPUSTAT compustat.idx"
set sourceTypes(CRSP) "{Security Data} $schemadir/logical/CRSP crsp_dsm94.idx"
set sourceTypes(CRSP_NSDQ) "{Nasdaq Security Data} $schemadir/logical/CRSP crsp_ndsm94.idx"
set sourceTypes(ISSM) "{Historical Stock Data (Trades and Quotes} $schemadir/logical/ISSM-T issm.idx"
set sourceTypes(NETWORK) "{Network Server Output} $schemadir/logical/NETWORK"
set sourceTypes(SEQ) "{SEQ Query Output} $schemadir/logical/SEQ"
set sourceTypes(SQL) "{SQL Query Output} $schemadir/logical/SQL"
set sourceTypes(UNIXFILE) "{Unix File} $schemadir/logical/UNIXFILE"
set sourceTypes(WWW) "{World Wide Web} $schemadir/logical/WWW"

set sourceFile $datadir/sourcedef.tcl
if {[file exists $sourceFile]} {
    puts "Using data stream catalog $sourceFile"
    source $sourceFile
}

source $libdir/mapdef.tcl
source $libdir/autosrc.tcl
source $libdir/issm.tk
source $libdir/cstat.tk
source $libdir/crsp.tcl

set cacheSize [expr 100 * 1024 * 1024]

############################################################

# format of items in sourceList: 
#   sourceList(dispname) = {source key schematype schemafile \
#                           cachefile evaluation priority command \
#                           dataSegment}
#
# dispname is the name of the source as given by the user
# source is the type of the data source (SEQ, SQL, CRSP, etc.)
# key is used to index the source; e.g. company name for stock databases
# schematype is the name of the schema (e.g. ISSM-Trade)
# schemafile is the name of the schemafile
# cachefile is the name of the file where Devise caches the data;
#    this field is NOT used for UNIXFILE and WWW data streams
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
# dataSegment defines whether the given data set occupies only a segment
#    of a file (or whatever the source of data is).  If dataSegment exists,
#    it is a list consisting of the offset and the length of the data.  If
#    dataSegment does not exists, the data is assumed to occupy the entire
#    file, etc.

############################################################

proc saveSources {} {
    global sourceFile sourceList sourceTypes

    catch { exec mv $sourceFile $sourceFile.bak }

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

proc updateStreamDef {} {
    global schemadir
    global sourceList sourceTypes editonly oldDispName
    global dispname source key schemafile evaluation priority command

    if {$source == "SEQ"} {
	set schemafile [format "%s/%s.%s.schema" $schemadir/physical \
		[string trim $source] [string trim $key]]
	set schematype [format "%s.%s" [string trim $source] \
		[string trim $key]]
    }

    if {$dispname == "" || $key == "" || $schemafile == ""} {
	dialog .noName "Missing Information" \
		"Please enter all requested information." \
		"" 0 OK
	return
    }

    set cachefile [getCacheName $source $key]
    set command [string trim [.srcdef.top.row5.e1 get 1.0 end]]

    if {$editonly && $source == "SEQ"} {
	set oldCommand [lindex $sourceList($oldDispName) 7]
	if {$oldCommand != $command} {
	    uncacheData $oldDispName "Query changed."
	}
    }
    if {$source != "SEQ"} {
	set newschematype [scanSchema $schemafile]
	if {$newschematype == ""} { return }
	set schematype $newschematype
    }

    set sourcedef [list $source $key $schematype $schemafile \
	    $cachefile $evaluation $priority $command]
	
    set conflict [getSourceByCache $cachefile]
    if {!$editonly && $conflict != ""} {
	dialog .existsAlready "Data Stream Exists" \
		"This definition conflicts\n\
		with \"$conflict\"." "" 0 OK
	return
    }

    if {$editonly} {
	set oldCachefile $sourceList($oldDispName)
	if {[file exists $oldCachefile] \
		&& $cachefile != $oldCachefile} {
	    uncacheData $oldDispName "Source and/or key changed."
	}
	unset sourceList($oldDispName)
    }

    set err [catch {set exists $sourceList($dispname)}]
    if {$err == 0} {
	dialog .sourceExists "Data Stream Exists" \
		"Data stream \"$dispname\" exists already." \
		"" 0 OK
	return
    }

    set "sourceList($dispname)" $sourcedef
    saveSources
    updateSources
    destroy .srcdef
}

############################################################

proc defineStream {base edit} {
    global sourceList sourceTypes editonly oldDispName 
    global dispname source key schemafile evaluation priority command

    if {[WindowVisible .srcdef]} {
	return
    }

    toplevel .srcdef
    if {$edit} {
	wm title .srcdef "Edit Data Stream Definition"
    } elseif {$base != ""} {
	wm title .srcdef "Copy Data Stream Definition"
    } else {
	wm title .srcdef "Define Data Stream"
    }
    wm geometry .srcdef +100+100
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
	global schemadir fsBox UserMode
	set fsBox(path) $schemadir/logical
	if {!$UserMode} {
	    set fsBox(path) $schemadir/physical
	}
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

    button .srcdef.but.ok -text OK -width 10 -command updateStreamDef
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

proc getSEQSchema {command schemafile schematype} {
    global sourceConfig

    set host [lindex $sourceConfig(SEQ) 0]
    set port [lindex $sourceConfig(SEQ) 1]
    # add a disjoint from-to clause which causes the SEQ server
    # to return an empty data set; just the schema is returned
    set command "$command from \"1996-01-01 00:00:00\" \
	    to \"1995-01-01 00:00:00\""
    set cmd "seq_extract $host $port \{$command;\} /dev/null \
	    $schemafile $schematype"

    update
    if {[catch { eval $cmd }] > 0} {
	puts "Cannot execute command:"
	puts "  $cmd"
	dialog .error "Error Occurred" \
		"An error occurred while extracting the schema\n\
		from the SEQ server. See text window for\n\
		the error message." \
		"" 0 OK
	catch { exec rm $schemafile }
	return -1
    }

    return 0
}

############################################################

proc getSourceByCache {cachefile} {
    global sourceList

    if {$cachefile == ""} {
	return ""
    }

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
    global sourceList derivedSourceList

    set err [catch {set sourcedef $derivedSourceList($dispname)}]
    if {$err} {
	set sourcedef $sourceList($dispname)
    }

    set source [lindex $sourcedef 0]
    set key [lindex $sourcedef 1]
    set cachefile [lindex $sourcedef 4]
    set command [lindex $sourcedef 7]
    
    if {$source == "WWW"} {
        return $command
    }

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
    set schematype [lindex $sourcedef 2]
    set schemafile [lindex $sourcedef 3]
    set cachefile [lindex $sourcedef 4]
    set priority [lindex $sourcedef 6]
    set command [lindex $sourcedef 7]
    
    set cmd ""

    if {$source == "COMMAND"} {
	set cmd "exec $command > $cachefile"
    } elseif {$source == "SEQ"} {
	set host [lindex $sourceConfig($source) 0]
	set port [lindex $sourceConfig($source) 1]
	set cmd "seq_extract $host $port \{$command\} $cachefile \
		$schemafile $schematype"
    } elseif {$source == "SQL"} {
	set prog [lindex $sourceConfig($source) 0]
	set port [lindex $sourceConfig($source) 1]
	set cmd "exec $prog $port \{$command\} > $cachefile"
    } elseif {$source == "NETWORK"} {
	set prog [lindex $sourceConfig($source) 0]
	set port [lindex $sourceConfig($source) 1]
	set cmd "exec $prog $port > $cachefile"
    } elseif {$source == "ISSM"} {
	set tapedrive [lindex $sourceConfig($source) 0]
	set filenum [lindex $sourceConfig($source) 1]
	set offset [lindex $sourceConfig($source) 2]
	set blocksize [lindex $sourceConfig($source) 3]
	set cmd "issm_extract_data $source $tapedrive $filenum \
		$offset $blocksize $key $cachefile"
    } elseif {$source == "COMPUSTAT"} {
	set tapedrive [lindex $sourceConfig($source) 0]
	set filenum [lindex $sourceConfig($source) 1]
	set offset [lindex $sourceConfig($source) 2]
	set blocksize [lindex $sourceConfig($source) 3]
	set cmd "cstat_extract_data $source $tapedrive $filenum \
		$offset $blocksize $key $cachefile"
    } elseif {$source == "CRSP" || $source == "CRSP_NSDQ"} {
	set tapedrive [lindex $sourceConfig($source) 0]
	set filenum [lindex $sourceConfig($source) 1]
	set offset [lindex $sourceConfig($source) 2]
	set blocksize [lindex $sourceConfig($source) 3]
	set cmd "crsp_extract_data $source $tapedrive $filenum \
		$offset $blocksize $key $cachefile"
    }

    if {$cmd == ""} {
	dialog .noCmd "Cannot Extract" \
		"No extraction command defined for this data source." \
		"" 0 OK
	return ""
    }

    statusWindow .info "Status" \
	    "Extracting \"$dispname\".\n\nThis may take a while."
    update

    if {[catch { eval $cmd }] > 0} {
	catch { destroy .info }
	puts "Cannot execute command:"
	puts "  $cmd"
	dialog .error "Error Occurred" \
		"An error occurred while extracting data\n\
		from the data source. See text window for\n\
		the error message." \
		"" 0 OK
	uncacheData $dispname ""
	return ""
    }

    catch { destroy .info }

    cachePrune [list $dispname]

    return $cachefile
}

############################################################

proc cachePrune {avoid} {
    global sourceList cacheSize cachedir

    # see if we're out of cache space

    set total 0
    foreach cachefile [glob -nocomplain [format "%s/*" $cachedir]] {
	incr total [file size $cachefile]
    }

    if {$total < $cacheSize} {
	return
    }

    # build up an array of data sources with existing cache files,
    # indexed by the cache priority; cachep(priority) contains a list
    # of data sources with cache files having that priority

    foreach dispname [array names sourceList] {
	set cachefile [lindex $sourceList($dispname) 4]
	if {![file exists $cachefile] || [lsearch $avoid $dispname] >= 0} {
	    continue
	}
	set priority [lindex $sourceList($dispname) 6]
	set priority [format "%03d" $priority]
	set err [catch {set exists $cachep($priority)}]
	if {$err} {
	    set cachep($priority) ""
	}
	lappend cachep($priority) $dispname
    }

    # remove cache files, starting from the lowest-priority
    # cache files

    puts "Pruning cache, total size $total, limit $cacheSize"

    foreach p [lsort [array names cachep]] {
	puts "$p: $cachep($p)"
	foreach dispname $cachep($p) {
	    set cachefile [lindex $sourceList($dispname) 4]
	    set size [file size $cachefile]
	    puts "$dispname, size $size, total $total"
	    if {[uncacheData $dispname "Cache full."]} {
		incr total [expr -$size]
	    }
	    if {$total <= $cacheSize} { break }
	}
	if {$total <= $cacheSize} { break }
    }

    updateSources

    set fillfactor [expr 1.0 * $total / $cacheSize]
    if {$fillfactor <= 0.8} { return }

    set percentage [format "%.2f%%" [expr $fillfactor * 100]]
    if {$fillfactor <= 1.0} {
	dialog .cacheFilling "Cache Nearly Full" \
		"Disk cache is $percentage full." "" 0 OK
    } else {
	dialog .cacheFull "Cache Full" \
		"Disk cache is $percentage full." "" 0 OK
    }
}

############################################################

proc uncacheData {dispname reason} {
    global sourceList sourceConfig tmpdir env

    # see if data stream open
    if {[lsearch [TdataSet] $dispname] >= 0} {
	dialog .cantUncache "Cannot uncache open stream" \
		"Stream \"$dispname\" is open. Cannot remove\
		cache and other files. Close session and then retry." \
		"" 0 OK
	return 0
    }

    set workdir "work"
    if { [info exists env(DEVISE_WORK)] } {
	set workdir $env(DEVISE_WORK)
    }

    set realCacheFile [isCached $dispname -1 -1]

    # remove index and gdata files if they exist
    set indexfile [format "$workdir/%s.cache" [file tail $realCacheFile]]
    if {[file exists $indexfile]} {
	# puts "Removing index file $indexfile..."
	exec rm $indexfile
    }

    set gdatapattern [format "$tmpdir/DEVise_%ld/$dispname#*" [pid]]
    foreach gdatafile [glob -nocomplain $gdatapattern] {
	# puts "Removing gdata file $gdatafile..."
	exec rm $gdatafile
    }

    set sourcedef $sourceList($dispname)
    set cachefile [lindex $sourcedef 4]

    # can't remove non-existent cache file (UNIXFILE sets cache
    # file to an empty string)
    if {$cachefile == "" || $cachefile == "/dev/null"} {
	return 0
    }

    if {$reason != ""} {
	set but [dialog .uncacheOkay "Remove Cache File" \
		"$reason\n\
		Okay to remove cache file?" \
		"" 1 Yes No]
	if {$but > 0} {
	    return 0
	}
    }

    set pattern [format "%s*" $cachefile]
    foreach cachefile [glob -nocomplain $pattern] {
	puts "Removing cache file $cachefile"
	exec rm $cachefile
    }

    return 1
}

############################################################

proc getCacheName {source key} {
    global cachedir

    if {$source == "UNIXFILE" || $source == "WWW"} {
	return ""
    }

    if {$source == "COMPUSTAT"} {
	return [format "%s/%s.%s.dat" $cachedir [string trim $source] \
		[cstat_unique_name $key]]
    }

    if {$source == "ISSM"} {
	return [format "%s/%s.%s.dat" $cachedir [string trim $source] \
		[issm_unique_name $key]]
    }

    return [format "%s/%s.%s.dat" $cachedir [string trim $source] \
	    [string trim $key]]
}

############################################################

proc selectSourceKey {source} {
    if {$source == "UNIXFILE"} {
	return [selectUnixFile]
    }
    if {$source == "COMMAND" || $source == "SEQ" || $source == "SQL" \
	|| $source == "WWW"} {
	return [list "default" "$source Default"]
    }
    if {$source == "COMPUSTAT"} {
	return [cstatMain $source]
    }
    if {$source == "ISSM"} {
	return [issmMain $source]
    }
    if {$source == "CRSP" || $source == "CRSP_NSDQ"} {
	return [crspMain $source]
    }

    dialog .error "Incorrect Data Source" \
	    "An invalid data source has been selected." \
	    "" 0 OK
    return ""
}

############################################################

proc selectUnixFile {} {
    global schemafile fsBox datadir schemadir UserMode

    # Get file name
    set fsBox(path) $datadir
    set fsBox(pattern) *
    set file [FSBox "Select data file"]

    # No file selected?
    if {$file == ""} {
	return ""
    }

    set fsBox(path) $schemadir/logical
    if {!$UserMode} {
	set fsBox(path) $schemadir/physical
    }
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

proc selectStream {{title ""}} {
    global streamsSelected sourceTypes MapTable

    if {[WindowVisible .srcsel]} {
	return
    }

    toplevel .srcsel
    wm title .srcsel "Data Streams"
    wm geometry .srcsel +50+50
    selection clear .srcsel

    frame .srcsel.mbar -relief raised -borderwidth 2
    frame .srcsel.top
    frame .srcsel.bot
    pack .srcsel.mbar -side top -fill x
    if {$title != ""} {
	label .srcsel.title -text $title
	pack .srcsel.title -side top -fill x -expand 1 -pady 3m
    }
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
	if {[llength $dispname] != 1} {
	    dialog .note "Note" "Select one stream to edit." "" 0 OK
	    return
	}
	defineStream [lindex $dispname 0] 1
    }
    .srcsel.mbar.stream.menu add command -label "Copy..." -command {
	set dispname [getSelectedSource]
	if {[llength $dispname] != 1} {
	    dialog .note "Note" "Select one stream to copy." "" 0 OK
	    return
	}
	defineStream [lindex $dispname 0] 0
    }
    .srcsel.mbar.stream.menu add command -label Delete -command {
	set dispnames [getSelectedSource]
	if {$dispnames == ""} {
	    dialog .note "Note" "Select streams to delete first." "" 0 OK
	    return
	}
	foreach d $dispnames {
	    set but [dialog .confirm "Confirm Stream Deletion" \
		    "Delete stream \"$d\"?" "" 1 Yes No Cancel]
	    if {$but == 2} {
		return
	    }
	    if {$but == 1} {
		continue
	    }
	    uncacheData $d ""
	    if {[catch {unset "sourceList($d)"}] == 0} {
		saveSources
		updateSources
	    }
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
		"This dialog lets you choose one or more data streams\
		for visualization. You can select a single data stream\
		by double-clicking on it, or you can select multiple\
		data streams with control-click and then\
		pressing the Select button. Press the Cancel button\
		to return without selecting anything.\n\n\
		Choose Stream/New to define a new data stream using a blank\
		template.\n\n\
		Stream/Edit lets you edit an existing data stream. Select\
		data stream first with the mouse.\n\n\
		You can copy an existing data stream and use its definition\
		as a template for a new data stream with Stream/Copy. Select\
		template with the mouse first.\n\n\
		Stream/Delete lets you delete the definition of the selected\
		data streams. The program asks for confirmation before\
		actually removing the entries." \
		"" 0 OK
    }

    tk_menuBar .srcsel.mbar .srcsel.mbar.define .srcsel.mbar.stream \
	    .srcsel.mbar.display .srcsel.mbar.follow .srcsel.mbar.help

    listbox .srcsel.top.list -relief raised -borderwidth 2 \
	    -yscrollcommand ".srcsel.top.scroll set" -font 9x15 \
	    -selectmode extended -width 61 -height 22
    scrollbar .srcsel.top.scroll -command ".srcsel.top.list yview"
    pack .srcsel.top.list -side left -fill both -expand 1
    pack .srcsel.top.scroll -side right -fill y
    bind .srcsel.top.list <Double-Button-1> {
	set streamsSelected [getSelectedSource]
    }

    updateSources

    button .srcsel.bot.but.select -text Select -width 10 -command {
	set streamsSelected [getSelectedSource]
    }
    button .srcsel.bot.but.uncache -text Uncache -width 10 -command {
	set dispnames [getSelectedSource]
	foreach d $dispnames {
	    uncacheData $d "Uncache requested."
	    updateSources
	}
    }
    button .srcsel.bot.but.cancel -text Cancel -width 10 -command {
	set streamsSelected ""
    }
    pack .srcsel.bot.but.select .srcsel.bot.but.uncache \
	    .srcsel.bot.but.cancel -side left -padx 3m

    set streamsSelected ""
    tkwait variable streamsSelected
    catch {destroy .srcsel}

    return $streamsSelected
}

############################################################

proc selectDerivedStream {{title ""}} {
    global streamsSelected

    if {[WindowVisible .srcsel]} {
	return
    }

    toplevel .srcsel
    wm title .srcsel "Derived Data Streams"
    wm geometry .srcsel +50+50
    selection clear .srcsel

    frame .srcsel.mbar -relief raised -borderwidth 2
    frame .srcsel.top
    frame .srcsel.bot
    pack .srcsel.mbar -side top -fill x
    if {$title != ""} {
	label .srcsel.title -text $title
	pack .srcsel.title -side top -fill x -expand 1 -pady 3m
    }
    pack .srcsel.top -side top -fill both -expand 1
    pack .srcsel.bot -side top -fill x -pady 5m

    frame .srcsel.bot.but
    pack .srcsel.bot.but -side top

    listbox .srcsel.top.list -relief raised -borderwidth 2 \
	    -yscrollcommand ".srcsel.top.scroll set" -font 9x15 \
	    -selectmode single -width 61 -height 22
    scrollbar .srcsel.top.scroll -command ".srcsel.top.list yview"
    pack .srcsel.top.list -side left -fill both -expand 1
    pack .srcsel.top.scroll -side right -fill y
    bind .srcsel.top.list <Double-Button-1> {
	set streamsSelected [getSelectedSource]
    }

    scanDerivedSources
    updateDerivedSources

    button .srcsel.bot.but.select -text Select -width 10 -command {
	set streamsSelected [getSelectedSource]
    }
    button .srcsel.bot.but.cancel -text Cancel -width 10 -command {
	set streamsSelected ""
    }
    pack .srcsel.bot.but.select .srcsel.bot.but.cancel -side left -padx 3m

    set streamSelected ""
    tkwait variable streamsSelected
    catch {destroy .srcsel}

    return $streamsSelected
}

############################################################
# This procedure adds a new source to the data source list,
# forces the Data Sources window to be updated accordingly.

proc addDataSource {dispName source} {
    global sourceList

    set sourceList($dispName) $source
    updateSources

    return
}

############################################################

proc updateSources {} {
    global sourceList

    if {[catch {wm state .srcsel}] > 0} {
	return
    }

    .srcsel.top.list delete 0 end
    foreach sname [lsort [array names sourceList]] {
	set source [lindex $sourceList($sname) 0]
	set cachefile [lindex $sourceList($sname) 4]
	set cached ""
	if {$source == "WWW"} {
            set cached "Server"
        } elseif {[isCached $sname -1 -1] != ""} {
	    set cached "Cached"
	}
	set item [format "%-40.40s  %-10.10s  %-6.6s" \
		$sname $source $cached]
	.srcsel.top.list insert end $item
    }
}

############################################################

proc scanDerivedSources {} {
    global derivedSourceList schemadir env

    catch { unset derivedSourceList }

    set workdir "work"
    if { [info exists env(DEVISE_WORK)] } {
	set workdir $env(DEVISE_WORK)
    }

    foreach cachefile [glob -nocomplain [format "%s/*.stat" $workdir]] {
	set sname [file tail $cachefile]
	set source "STAT"
	set key $sname
	set schematype COLORSTAT
	set schemafile $schemadir/physical/COLORSTAT
	set evaluation 100
	set priority 50
	set command ""

	if {![file readable $schemafile]} {
	    puts "Schema file $schemafile does not exist."
	    puts "Cannot handle derived data streams."
	    return
	}

	set sourcedef [list $source $key $schematype $schemafile \
		$cachefile $evaluation $priority $command]
	set "derivedSourceList($sname)" $sourcedef
    }
}

############################################################

proc updateDerivedSources {} {
    global derivedSourceList

    if {[catch {wm state .srcsel}] > 0} {
	return
    }

    .srcsel.top.list delete 0 end

    foreach sname [lsort [array names derivedSourceList]] {
	set source [lindex $derivedSourceList($sname) 0]
	set cachefile [lindex $derivedSourceList($sname) 4]
	set cached "Cached"
	set item [format "%-40.40s  %-10.10s  %-6.6s" \
		$sname $source $cached]
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

    set select [split $select \n]
    set retval ""
    foreach s $select {
	set dispName [string range $s 0 40]
	set dispName [string trimright $dispName]
	lappend retval $dispName
    }
    return $retval
}

############################################################

proc mapFollow {newtype} {
    global button evaluation priority MapTable sourceList sourceTypes

    set curr [getSelectedSource]
    if {[llength $curr] != 1} {
	dialog .note "Note" "Select one data source." "" 0 OK
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

scanDerivedSources

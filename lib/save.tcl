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
#  Revision 1.9  1996/06/19 19:56:56  wenger
#  Improved UtilAtof() to increase speed; updated code for testing it.
#
#  Revision 1.8  1996/06/15 14:26:44  jussi
#  Added saving of mapping legends.
#
#  Revision 1.7  1996/06/12 14:57:58  wenger
#  Added GUI and some code for saving data to templates; added preliminary
#  graphical display of TDatas; you now have the option of closing a session
#  in template mode without merging the template into the main data catalog;
#  removed some unnecessary interdependencies among include files; updated
#  the dependencies for Sun, Solaris, and HP; removed never-accessed code in
#  ParseAPI.C.
#
#  Revision 1.6  1996/05/31 16:36:12  jussi
#  Minor fix in DoActualSave.
#
#  Revision 1.5  1996/05/31 15:49:34  jussi
#  Fixed problem with tdata dictionary. Added saving of link master.
#
#  Revision 1.4  1996/05/16 21:39:06  wenger
#  implemented saving schemas ans session file for importing tdata
#
#  Revision 1.3  1996/04/14 00:27:00  jussi
#  Changed format of session file to reflect the new mapping class
#  interface (namely, createInterp has been replaced with
#  createMappingClass).
#
#  Revision 1.2  1996/03/07 17:00:23  jussi
#  Added support for number of dimensions.
#
#  Revision 1.1  1996/01/23 20:50:58  jussi
#  Initial revision.
#

############################################################

# Save a info about creating a class instance into file f
proc SaveInstance { f category class instance } {
    set params [ DEVise getCreateParam $category $class $instance ]
    puts $f "DEVise create $category $class $params"
    flush $f
}

############################################################

# Save all instances of a given class into file f
proc SaveClass { f category class } {
    set instances [ DEVise get $category $class]
    foreach i $instances {
	SaveInstance $f $category $class $i 
    }
}

############################################################

# Save all instances of a given category into file f
proc SaveCategory { f category } {
    set classes [ DEVise get $category ]
    foreach i $classes {
	SaveClass $f $category $i 
    }
}

############################################################

# Save info about mappings to all views into file f
proc SaveAllViewMappings { f mappingDict viewDict } {
    # Get all mapping classes
    set viewClasses [ DEVise get "view" ]
    foreach vClass $viewClasses {
	# Get all views for this class
	set views [ DEVise get "view" $vClass ]
	foreach v $views {
	    set viewVar [DictLookup $viewDict $v]
	    # Get all mappings for this view
	    set mappings [ DEVise getViewMappings $v ]
	    foreach map $mappings {
		set mapVar [DictLookup $mappingDict $map]
		puts $f "DEVise insertMapping \$$viewVar \$$mapVar"
		set label [DEVise getMappingLegend $v $map]
		if {$label != ""} {
		    puts $f "DEVise setMappingLegend \$$viewVar \$$mapVar $label"
		}
	    }
	}
    }
}

############################################################

# Save info about axis labels to all views 
proc SaveAllViewAxisLabel { f viewDict } {
    set viewClasses [ DEVise get "view" ]
    foreach vClass $viewClasses {
	# Get all views for this class
	set views [ DEVise get "view" $vClass ]
	foreach v $views {
	    set viewVar [DictLookup $viewDict $v]
	    set xAxisLabel [ DEVise getAxis $v x]
	    if {$xAxisLabel != ""} {
		puts $f "DEVise setAxis \$$viewVar $xAxisLabel x"
	    }
	    
	    set yAxisLabel [ DEVise getAxis $v y]
	    if {$yAxisLabel != ""} {
		puts $f "DEVise setAxis \$$viewVar $yAxisLabel y"
	    }
	    
	    # Set axis display on/off.
	    set stat [DEVise getAxisDisplay $v X]
	    puts $f "DEVise setAxisDisplay \$$viewVar X $stat"
	    
	    set stat [DEVise getAxisDisplay $v Y]
	    puts $f "DEVise setAxisDisplay \$$viewVar Y $stat"
	}
    }
}

############################################################

# Save info about action
proc SaveAllViewAction { f dict} {
    set viewClasses [ DEVise get "view" ]
    foreach vClass $viewClasses {
	# Get all views for this class
	set views [ DEVise get "view" $vClass ]
	foreach v $views {
	    set viewVar [DictLookup $dict $v]
	    set action [DEVise getAction $v]
	    if {$action != ""} {
		puts $f "DEVise setAction \$$viewVar $action"
	    }
	}
    }
}

############################################################

# Save info about views inserted into windows into file f
proc SaveAllWindowViews { f viewDict} {
    # Get all window classes
    set windowClasses [ DEVise get "window" ]
    foreach winClass $windowClasses {
	# Get all windows for this class
	set windows [ DEVise get "window" $winClass ]
	foreach win $windows {
	    # Get all views inserted into this window
	    set views [DEVise getWinViews $win ]
	    foreach v $views {
		set viewVar [DictLookup $viewDict $v]
		puts $f "DEVise insertWindow \$$viewVar \{$win\}"
	    }
	}
    }
}

############################################################

# Save name of views in links
proc SaveLinkViews { file dict} {
    set linkSet [LinkSet]
    foreach link $linkSet {
	set views [DEVise getLinkViews $link]
	foreach view $views {
	    set viewVar [DictLookup $dict $view]
	    puts $file "DEVise insertLink \{$link\} \$$viewVar"
	}
	set view [DEVise getLinkMaster $link]
	if {$view != ""} {
	    set viewVar [DictLookup $dict $view]
	    puts $file "DEVise setLinkMaster \{$link\} \$$viewVar"
	}
    }
}

############################################################

# Save name of views in cursors
proc SaveCursorViews { file dict} {
    set cursorSet [CursorSet]
    foreach cursor $cursorSet {
	set views [DEVise getCursorViews $cursor]
	set src [DictLookup $dict [lindex $views 0]]
	set dst [DictLookup $dict [lindex $views 1]]
	if {$src != ""} {
	    puts $file "DEVise setCursorSrc \{$cursor\} \$$src"
	}
	if {$dst != ""} {
	    puts $file "DEVise setCursorDst \{$cursor\} \$$dst"
	}
    }
}

############################################################

# Save session
proc DoActualSave { infile asTemplate asExport withData } {
    global mode curView sourceFile sourceList sourceTypes
    global templateMode schemadir datadir

    # you can't save an imported file until it has been merged
    if {$templateMode} { 
	dialog .open "Merge Data" \
	    "You must merge an imported template before saving it" "" 0 OK
	return
    }

    if {[file exists $infile]} {
	if { ![file writable $infile] } {
	    dialog .saveError "Cannot Save Session" \
		    "File $infile not writable" "" 0 OK
	    return
	}
    }

    ChangeStatus 1

    # rename existing file to a backup file
    catch { exec mv $infile $infile.bak }

    # checkpoint tdata
    set classes [ DEVise get "tdata" ]
    foreach class $classes {
	set instances [ DEVise get "tdata" $class ]
	foreach inst $instances {
	    DEVise tcheckpoint $inst
	}
    }

    set f [open $infile w ]

    if {$asExport} {
	puts $f "global templateMode sourceList datadir schemadir"
	puts $f "global physSchema logSchema"
	puts $f "set templateMode 1"
	# The sourceList will have to be reset in the imported file
	puts $f "unset sourceList"
	puts $f ""
    }

    puts $f "# Import schemas"
    set catFile [DEVise catFiles]
    foreach file $catFile {
	if {$asExport} {
	    # read contents of schema
	    set fileP [open $file r]
	    gets $fileP schemaTest
	    if {[lindex $schemaTest 0] == "physical"} {
		set lschema $schemaTest
		set lschema [concat $lschema [read $fileP]]
		close $fileP
		set fileP [open [lindex $schemaTest end] "r"]
	    } else {
		close $fileP	
		set fileP [open $file "r"]
		set lschema ""
	    }
	    set pschema [read $fileP]
	    close $fileP
	    set sname [file tail $file]
	    puts $f "set physSchema($sname) \"$pschema\""
	    puts $f "set logSchema($sname) \"$lschema\""
	    puts $f "DEVise parseSchema $sname \$physSchema($sname) \$logSchema($sname)"
	} else {
	    puts $f "DEVise importFileType $file"
	}
    }
    puts $f ""

    puts $f "# Layout mode"
    puts $f "DEVise changeMode 0"
    puts $f ""

    puts $f "# Import file (Create TData)"
    puts $f ""
    if {!$asExport && !$asTemplate} {
	puts $f "set loadPixmap 1"
	puts $f ""
    } 

    set fileDict ""
    set fileNum 1
    set totalTData [llength [DEVise get tdata]]

    # If we're saving data in the session file, we refer to the session file
    # via the 'file' variable so things will work if the session file is
    # renamed.
    if {$withData} {
	puts $f "set filename \[file tail \$file\]"
	puts $f "set directory \[file dirname \$file\]"
    }

    if {$asExport} {
	foreach class [DEVise get tdata] {
	    foreach inst [DEVise get tdata $class] {
		set params [DEVise getCreateParam tdata $class $inst]
		set fileAlias [lindex $params 1]
		set sourcedef $sourceList($fileAlias)
		# in sourcedef, replace path name of entry #3 with \$schemadir
		# if entry #0 == UNIXFILE, replace entry #7 with \$datadir
		puts $f "\tset \"sourceList($fileAlias)\" \{$sourcedef\}"
		set instances [DEVise get tdata $class]
	    }
	}
    }

    foreach class [DEVise get tdata] {
	foreach inst [DEVise get tdata $class] {
	    set params [DEVise getCreateParam tdata $class $inst]
	    set filePath [lindex $params 0]
	    set fileAlias [lindex $params 1]
	    set fileDict [DictInsert $fileDict $fileAlias tdata_$fileNum]
	    set tdataVar tdata_$fileNum
	    if {$asTemplate} {
		puts $f "set $tdataVar \[ GetTDataTemplate $class $totalTData $fileNum \]"
		puts $f "if \{\$$tdataVar == \"\"\} \{"
		puts $f "\treturn 0"
		puts $f "\}"
	    } else {
		puts $f "if \{ \$template \} \{"
		puts $f "\tset loadPixmap 0"
		puts $f "\tset $tdataVar \[ GetTDataTemplate $class $totalTData $fileNum \]"

		puts $f "\tif \{\$$tdataVar == \"\"\} \{"
		puts $f "\t\treturn 0"
		puts $f "\t\}"
		puts $f "\} else \{"
		puts $f "\tset $tdataVar \{$fileAlias\}"
		puts $f "\tset source \[OpenDataSource $$tdataVar]"
		puts $f "\tif \{\$source == \"\"\} \{"
		puts $f "\t\treturn 0"
		puts $f "\t\}"
		puts $f "\tset dispname \[lindex \$source 0\]"
		puts $f "\tset filePath \[lindex \$source 1\]"
		puts $f "\tDEVise create tdata $class \$filePath \$dispname"
		puts $f "\tif \{\$dispname != \$$tdataVar\} \{"
		puts $f "\t\tset loadPixmap 0"
		puts $f "\t\tset $tdataVar \$dispname"
		puts $f "\t\}"
		puts $f "\}"
	    }
	    set fileNum [expr $fileNum+1]
	}
    }
    puts $f  ""

    puts $f "# Create interpreted mapping classes "
    foreach mclass [ DEVise get mapping ] {
	# puts "mclass $mclass"
	puts $f "DEVise createMappingClass $mclass"
    }
    puts $f ""

    puts $f "# Create mappings instance (GData)"
    set mapDict ""
    set mapNum 1
    foreach mapClass [ DEVise get mapping ] {
	foreach inst [DEVise get mapping $mapClass] {
	    set params [DEVise getCreateParam mapping $mapClass $inst]
	    set fileAlias [lindex $params 0]
	    set fileVar [DictLookup $fileDict $fileAlias]
	    set gdataName [lindex $params 1]
	    regsub $fileAlias $gdataName \%s gdataExpr
	    puts $f "set map_$mapNum \[ format \"$gdataExpr\" \$$fileVar \]"
	    puts $f "DEVise create mapping \{$mapClass\} \$$fileVar \$map_$mapNum [lrange $params 2 end]"
	    set mapDict [DictInsert $mapDict $inst map_$mapNum]
	    incr mapNum
	}
    }
    puts $f ""

    puts $f "# Save pixel width for mappings"
    foreach map [ GdataSet ] {
	set width [DEVise getPixelWidth $map]
	set gdataVar [DictLookup $mapDict $map]
	puts $f "DEVise setPixelWidth \$$gdataVar $width"
    }
    puts $f  ""

    puts $f "# Create views"
    set viewDict ""
    set viewNum 1
    foreach viewClass [ DEVise get view ] {
	foreach inst [ DEVise get view $viewClass ] {
	    set params [DEVise getCreateParam view $viewClass $inst]
	    set viewMap [lindex [DEVise getViewMappings $inst] 0]
	    set viewVar view_$viewNum
	    if {$viewMap == ""} { 
		puts $f "set $viewVar $inst"
	    } else {
		set viewTData [DEVise getMappingTData $viewMap]
		set tdataVar [DictLookup $fileDict $viewTData]
		regsub $viewTData $inst \%s viewExpr
		puts $f "set $viewVar \[ format \"$viewExpr\" \$$tdataVar \]"
	    }
	    puts $f "DEVise create view $viewClass \$$viewVar [lrange $params 1 end]"

	    set viewLabelParams [DEVise getLabel $inst]
	    puts $f "DEVise setLabel \$$viewVar $viewLabelParams"
	    set viewStatParams [DEVise getViewStatistics $inst]
	    puts $f "DEVise setViewStatistics \$$viewVar $viewStatParams"
	    set viewDimensions [DEVise getViewDimensions $inst]
	    puts $f "DEVise setViewDimensions \$$viewVar $viewDimensions"

	    set viewDict [DictInsert $viewDict $inst $viewVar]
	    set viewNum [expr $viewNum+1]
	}
    }
    puts $f ""

    puts $f "# Create windows"
    SaveCategory $f "window" 
    puts $f ""

    puts $f "# Setup window layouts"
    foreach win [WinSet] {
	set layout [DEVise getWindowLayout $win]
	puts $f "DEVise setWindowLayout \{$win\} $layout"
    }
    puts $f ""
    
    puts $f "# Create Links"
    SaveCategory $f "link" 
    puts $f ""

    puts $f "# Create Cursors"
    SaveCategory $f "cursor"
    puts $f ""

    puts $f "# Create axislabel"
    SaveCategory $f "axisLabel"
    puts $f ""

    puts $f "# Create Actions"
    SaveCategory $f "action"
    puts $f ""

    puts $f "# Put labels into views"
    SaveAllViewAxisLabel $f $viewDict
    puts $f ""

    puts $f "# Put action into view"
    SaveAllViewAction $f $viewDict
    puts $f ""

    puts $f "# Link views"
    SaveLinkViews $f $viewDict
    puts $f ""

    puts $f "# Put views in cursors"
    SaveCursorViews $f $viewDict
    puts $f ""

    puts $f "# Put axis label into views"
    puts $f ""

    puts $f "# Insert mappings into views"
    SaveAllViewMappings $f $mapDict $viewDict 
    puts $f ""

    puts $f "# Insert views into windows"
    SaveAllWindowViews $f $viewDict
    puts $f ""

    puts $f "# Init history of view"
    foreach view [ViewSet] {
	set viewVar [DictLookup $viewDict $view]
	puts $f "DEVise clearViewHistory \$$viewVar"
	if {$asTemplate || $asExport} {
	    continue
	}
	foreach hist [ DEVise getVisualFilters $view ] {
	    puts $f "DEVise insertViewHistory \$$viewVar {[lindex $hist 0]} {[lindex $hist 1]} {[lindex $hist 2]} {[lindex $hist 3]} {[lindex $hist 4]}"
	}
    }

    puts $f ""
    puts $f "# Set camera location for each view"
    foreach view [ViewSet] {
	set camera [DEVise get3DLocation $view]
	set x [lindex $camera 1]
	set y [lindex $camera 2]
	set z [lindex $camera 3]
	puts $f "DEVise set3DLocation \$$viewVar $x $y $z"
    }

    if { $mode == 1 } {
        puts $f ""
	puts $f "# Display mode"
	puts $f "DEVise changeMode 1"
    }

    if {$asTemplate || $asExport} {
        puts $f ""
	puts $f "return 1"
    }

# save data here
    if {$withData} {
	SaveAllData $f
    }

    if {$asTemplate || $asExport} {
	ChangeStatus 0
	close $f
	return
    }

    # Save pixmaps 
    set bitmapFile $infile.pixmap
    if { [ file exists $bitmapFile ] } {
	if { ! [ file writable $bitmapFile ] } {
	    dialog .saveError "Cannot Save Pixmap" \
		    "File $bitmapFile not writable" "" 0 OK
	    ChangeStatus 0
	    return
	}
    }

    set bitF [DEVise open $bitmapFile wb]
    set date [DEVise date]
    DEVise writeLine $date $bitF
    set savedCurView $curView
    ProcessViewSelected ""

    puts $f ""
    puts $f "# Load pixmaps from views"
    puts $f "set pixmapName $bitmapFile"
    puts $f "if \{ \$loadPixmap && \[ file exists \$pixmapName \] \} \{"
    puts $f "	set pixmapFile \[ DEVise open \$pixmapName rb \]"
    puts $f "	set fileDate \[ DEVise readLine \$pixmapFile \]"
    puts $f "	set sessionDate \{$date\}"
    puts $f "	if \{\$fileDate == \$sessionDate\} \{ "
    foreach view [ViewSet] {
	DEVise savePixmap $view $bitF
	puts $f "		DEVise loadPixmap \{$view\} \$pixmapFile"
    }
    puts $f "	\}"
    puts $f "	DEVise close \$pixmapFile"
    puts $f "\}"
    DEVise close $bitF

    ProcessViewSelected $savedCurView

    puts $f ""
    puts $f "return 1"

    ChangeStatus 0
    close $f
}

############################################################

proc DoSave {} {
    global sessionName templateMode

    if {$sessionName == "session.tk"} {
	DoSaveAs 0 0 0
	return
    }

    DoActualSave $sessionName 0 0 0
}

############################################################

proc DoSaveAs { asTemplate asExport withData } {
    global sessionName fsBox sessiondir templateMode

    # Get file name
    set fsBox(path) $sessiondir
    set fsBox(pattern) *.tk
    set file [ FSBox ]
    
    if {$file == ""} { return }

    set button [ dialog .saveSession "Save Session" \
	    "Save session to file\n$file?"  "" 1  {Cancel} OK ]
    if {$button != 1} { return }

    DoActualSave $file $asTemplate $asExport $withData
    if {!$asTemplate} {
	set sessionName $file
    }
}


####################################################################

# This procedure merges the schema and sourceList from an imported file,
# with the local schemas and sourceList.

proc DoTemplateMerge {} {
    global sourceFile sourceList sourceTypes templateMode schemadir
    global physSchema logSchema datadir

    if {!$templateMode} { 
	dialog .open "Merge Data" \
		"You must be in template mode in order to merge" "" 1 Ok
	return
    }

    # write schemas to schema files in $schemadir
    # make sure schema files don't exist before overwriting them

    # save logical schemas
    foreach schema [array names logSchema] {
	if {[file exists [concat $schemadir/logical/$schema]]} {
	    puts "Conflict -- could not add logical schema $schema"
	} else {
	    set fileP [open $schemadir/logical/$schema "w"]
	    puts $fileP $logSchema($schema)
	    close $fileP
	}
	#DEVise importFileType $schemadir/logical/$schema
    }

    # save physical schemas
    foreach schema [array names physSchema] {
	if {[file exists [concat $schemadir/physical/$schema]]} {
	    puts "Conflict -- could not add physical schema $schema"
	} else {
	    set fileP [open $schemadir/physical/$schema "w"]
	    puts $fileP $physSchema($schema)
	    close $fileP
	}
	DEVise importFileType $schemadir/physical/$schema
    }
	        
    unset physSchema
    unset logSchema

    foreach entry [array names sourceList] {
	set tempSourceList($entry) $sourceList($entry)
    }

    unset sourceList 
    set sourceFile $datadir/sourcedef.tcl
    if {[file exists $sourceFile]} {
	puts "Reading data stream catalog $sourceFile"
	source $sourceFile
    }

    # add entries to sourceList
    foreach entry [array names tempSourceList] {
	set defn $tempSourceList($entry)
	set err [catch {set exists $sourceList($entry)}]
	if {$err > 0} {
	    puts "Adding entry $entry to global catalog"
	    set sourceList($entry) $defn
	} else {
	    puts "Conflict -- could not add entry $entry"
	}
    }
    
    saveSources

    set templateMode 0

    puts "Done with merging template catalog with global catalog"
}

############################################################

# Save all TDatas to the given file.
proc SaveAllData { fileId } {

    # Keep from making duplicates by keeping a list of what we have saved.
    set dataSaved ""

    set classes [ DEVise get tdata ]
    foreach class $classes {
        set instances [ DEVise get tdata $class ]
        foreach inst $instances {
	    if {[lsearch $dataSaved $inst] == -1} {
		SaveOneData $fileId $inst
	        lappend dataSaved $inst
	    }
        }
    }

    return
}

############################################################

# Save the given TData to the given file.
proc SaveOneData { fileId tdata } {
    global sourceList

    #TEMPTEMP -- probably need a catch here?
    set source $sourceList($tdata)

    # Make sure this TData is a UNIXFILE, otherwise we can't save it yet.
    set type [lindex $source 0]
puts "DIAG type = $type"
    if {$type != "UNIXFILE"} {
	puts "TData $tdata is not a UNIXFILE -- can't write to template"
	return
    }

    set filename [lindex $source 1]
    set dirname [lindex $source 7]
    set tdFile [open $dirname/$filename r]

    puts $fileId ""
    puts $fileId "### $tdata"

#TEMPTEMP -- need to deal with tdatas that are only part of a file
    set data [read $tdFile]
    puts $fileId $data
    unset fileId

    close $tdFile

    return
}

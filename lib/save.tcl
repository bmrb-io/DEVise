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
proc DoActualSave { infile asTemplate } {
    global mode curView

    ChangeStatus 1
    # copy existing file to a backup file
    if { [ file exists $infile ] } {
	set src [open $infile]
	set dst [open $infile.bak w]
	while { [gets $src line] >= 0 } {
	    puts $dst $line
	}
	close $src
	close $dst

	if { ! [ file writable $infile ] } {
	    set but [ dialog .saveError "File $infile not writable" \
		    "" 0 OK ]
	    ChangeStatus 0
	    return
	}
    }

    # checkpoint tdata
    set classes [ DEVise get "tdata" ]
    foreach class $classes {
	set instances [ DEVise get "tdata" $class ]
	foreach inst $instances {
	    DEVise tcheckpoint $inst
	}
    }

    set f [open $infile w ]

    puts $f "# Import file types"
    set catFiles [DEVise catFiles]
    foreach file $catFiles {
	puts $f "DEVise importFileType $file"
    }
    puts $f ""

    puts $f "# Layout mode"
    puts $f "DEVise changeMode 0"
    puts $f ""

    puts $f "# Import file (Create TData)"
    puts $f ""
    if {!$asTemplate} {
	puts $f "set loadPixmap 1"
	puts $f ""
    } 

    set fileDict ""
    set fileNum 1
    set classes [DEVise get tdata]
    set totalTData [llength [TdataSet]]
    foreach class $classes {
	set instances [DEVise get tdata $class]
	foreach inst $instances {
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
		puts $f "\tset source \[OpenDataSource \$$tdataVar]"
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
    set mapClasses [ DEVise interpMapClassInfo ]
    foreach mclass $mapClasses {
	# puts "mclass $mclass"
	set mapTData [DictLookup $fileDict [lindex $mclass 0]]
	puts $f "DEVise createInterp \$$mapTData [lrange $mclass 1 end]"
    }
    puts $f ""

    puts $f "# Create mappings instances (and GData)"
    set mapClasses [DEVise get mapping]
    set mapDict ""
    set mapNum 1
    foreach mapClass $mapClasses {
	set instances [DEVise get mapping $mapClass]
	foreach inst $instances {
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
    foreach map [GdataSet ] {
	set width [DEVise getPixelWidth $map]
	set gdataVar [DictLookup $mapDict $map]
	puts $f "DEVise setPixelWidth \$$gdataVar $width"
    }
    puts $f  ""

    puts $f "# Create views"
    set viewDict ""
    set viewNum 1
    set viewClasses [DEVise get view]
    foreach viewClass $viewClasses {
	set instances [DEVise get view $viewClass]
	foreach inst $instances {
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
    set viewSet [ViewSet]
    foreach view $viewSet {
	set viewVar [DictLookup $viewDict $view]
	puts $f "DEVise clearViewHistory \$$viewVar"
	if ($asTemplate) {
	    continue
	}
	set historyList [DEVise getVisualFilters $view]
	foreach hist $historyList {
	    puts $f "DEVise insertViewHistory \$$viewVar {[lindex $hist 0]} {[lindex $hist 1]} {[lindex $hist 2]} {[lindex $hist 3]} {[lindex $hist 4]}"
	}
    }

    if { $mode == 1 } {
        puts $f ""
	puts $f "# Display mode"
	puts $f "DEVise changeMode 1"
    }

    if {$asTemplate} {
        puts $f ""
	puts $f "return 1"
	ChangeStatus 0
	close $f
	return
    }

    # Save pixmaps 
    set bitmapFile $infile.pixmap
    if { [ file exists $bitmapFile ] } {
	if { ! [ file writable $bitmapFile ] } {
	    set but [ dialog .saveError "File $bitmapFile not writable" \
		    "" 0 OK ]
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
    set viewSet [ViewSet]
    foreach view $viewSet {
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
    global sessionName

    if {$sessionName == "session.tk"} {
	DoSaveAs 0
	return
    }

    DoActualSave $sessionName 0
}

############################################################

proc DoSaveAs { asTemplate } {
    global sessionName fsBox sessiondir

    # Get file name
    set fsBox(path) $sessiondir
    set fsBox(pattern) *.tk
    set file [ FSBox ]
    
    if {$file == ""} { return }

    set button [ dialog .saveSession "Save Session" \
	    "Save session to file\n$file?"  "" 1  {Cancel} OK ]
    if {$button != 1} { return }

    DoActualSave $file $asTemplate
    if {!$asTemplate} {
	set sessionName $file
    }
}

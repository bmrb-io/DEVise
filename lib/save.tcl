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
#  Revision 1.31  1997/02/03 04:12:39  donjerko
#  Catalog management moved to DTE
#
#  Revision 1.30  1997/01/23 18:13:33  jussi
#  Disabled saving of pixmaps due to bug #135 and #069.
#
#  Revision 1.29  1997/01/22 23:24:08  jussi
#  Fixed computation of total number of TData streams saved in
#  session.
#
#  Revision 1.28  1997/01/06 19:12:05  wenger
#  Fixed the relative positions of 'OK' and 'Cancel' buttons on a number of
#  windows; view names now start with 'View 1' instead of 'View'; removed
#  some diagnostic output; added code to test the UniqueName procedure.
#
#  Revision 1.27  1996/12/20 16:50:37  wenger
#  Fonts for view label, x axis, and y axis can now be changed.
#
#  Revision 1.26  1996/11/20 16:48:22  jussi
#  Display/layout mode no longer saved in the session. When session
#  is restored, the user's currently selected mode is in control.
#
#  Revision 1.25  1996/07/23 17:26:44  jussi
#  Added saving of pileMode, overrideColor, and displayDataValues flags.
#
#  Revision 1.24  1996/07/21 02:22:45  jussi
#  Added saving of flags indicating solid/wireframe 3D objects and
#  XY vs. X/Y zoom mode.
#
#  Revision 1.23  1996/07/16 23:47:37  jussi
#  Added support for saving a session as a batch script (no Tcl variable
#  names used or if statements or anything, just a sequential list
#  of DEVise API calls).
#
#  Revision 1.22  1996/07/12 22:24:37  jussi
#  Views are now saved before tdata and mappings in a session file.
#  Exporting template with data does not add "addDataSource"
#  statements for BASICSTAT data sources.
#
#  Revision 1.21  1996/07/12 18:25:07  wenger
#  Fixed bugs with handling file headers in schemas; added DataSourceBuf
#  to TDataAscii.
#
#  Revision 1.20  1996/07/12 00:26:19  jussi
#  Added protective braces around TData names in addDataSource calls.
#  Fixed source definitions of non-UNIXFILE data sources in exported
#  templates.
#
#  Revision 1.19  1996/07/11 17:26:08  wenger
#  Devise now writes headers to some of the files it writes;
#  DataSourceSegment class allows non-fixed data length with non-zero
#  offset; GUI for editing schema files can deal with comment lines;
#  added targets to top-level makefiles to allow more flexibility.
#
#  Revision 1.18  1996/07/08 17:15:44  jussi
#  Added protective braces around mapping label.
#
#  Revision 1.17  1996/07/05 17:05:00  jussi
#  Disabled debugging statements which I forgot to do before
#  last check-in.
#
#  Revision 1.16  1996/07/05 15:25:52  jussi
#  The names of compiled mapping classes are no longer saved in session
#  files. This behavior confused Devise and when the session was
#  restored and re-saved, twice as many class names were saved etc.
#
#  Revision 1.15  1996/07/01 19:36:46  jussi
#  Made changes to reflect the new TData constructor interface.
#
#  Revision 1.14  1996/06/27 17:21:13  jussi
#  Fixed minor bug in saving 3D location.
#
#  Revision 1.13  1996/06/27 00:00:35  jussi
#  Coordinates of 3D focal point are now saved and displayed.
#
#  Revision 1.12  1996/06/26 17:22:06  wenger
#  Finished code for saving data to templates (includes quite a bit of
#  cleanup of the DoActualSave procedure).
#
#  Revision 1.11  1996/06/24 15:52:02  wenger
#  First version of Tcl code to save data to templates.
#
#  Revision 1.10  1996/06/21 20:11:03  jussi
#  Location of camera is now saved in the session file.
#
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
    puts $f "DEVise create \{$category\} \{$class\} $params"
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
proc SaveAllViewMappings { f mappingDict viewDict asBatchScript } {
    # Get all mapping classes
    set viewClasses [ DEVise get "view" ]
    foreach vClass $viewClasses {
	# Get all views for this class
	set views [ DEVise get "view" $vClass ]
	foreach v $views {
            set viewName "\{$v\}"
            if {!$asBatchScript} {
	        set viewVar [DictLookup $viewDict $v]
	        set viewName "\$$viewVar"
            }
	    # Get all mappings for this view
	    set mappings [ DEVise getViewMappings $v ]
	    foreach map $mappings {
                set mapName "\{$map\}"
                if {!$asBatchScript} {
                    set mapVar [DictLookup $mappingDict $map]
                    set mapName "\$$mapVar"
                }
                puts $f "DEVise insertMapping $viewName $mapName"
		set label [DEVise getMappingLegend $v $map]
		if {$label != ""} {
                    puts $f "DEVise setMappingLegend $viewName $mapName \{$label\}"
                }
	    }
	}
    }
}

############################################################

# Save info about axis labels to all views 
proc SaveAllViewAxisLabel { f viewDict asBatchScript } {
    set viewClasses [ DEVise get "view" ]
    foreach vClass $viewClasses {
	# Get all views for this class
	set views [ DEVise get "view" $vClass ]
	foreach v $views {
            set viewName "\{$v\}"
            if {!$asBatchScript} {
	        set viewVar [DictLookup $viewDict $v]
	        set viewName "\$$viewVar"
            }
	    set xAxisLabel [ DEVise getAxis $v x]
	    if {$xAxisLabel != ""} {
		puts $f "DEVise setAxis $viewName \{$xAxisLabel\} x"
	    }
	    
	    set yAxisLabel [ DEVise getAxis $v y]
	    if {$yAxisLabel != ""} {
		puts $f "DEVise setAxis $viewName \{$yAxisLabel\} y"
	    }
	    
	    # Set axis display on/off.
	    set stat [DEVise getAxisDisplay $v X]
	    puts $f "DEVise setAxisDisplay $viewName X $stat"
	    
	    set stat [DEVise getAxisDisplay $v Y]
	    puts $f "DEVise setAxisDisplay $viewName Y $stat"
	}
    }
}

############################################################

# Save info about action
proc SaveAllViewAction { f dict asBatchScript } {
    set viewClasses [ DEVise get "view" ]
    foreach vClass $viewClasses {
	# Get all views for this class
	set views [ DEVise get "view" $vClass ]
	foreach v $views {
            set viewName "\{$v\}"
            if {!$asBatchScript} {
	        set viewVar [DictLookup $dict $v]
	        set viewName "\$$viewVar"
            }
	    set action [DEVise getAction $v]
	    if {$action != ""} {
		puts $f "DEVise setAction $viewName \{$action\}"
	    }
	}
    }
}

############################################################

# Save info about views inserted into windows into file f
proc SaveAllWindowViews { f viewDict asBatchScript } {
    # Get all window classes
    set windowClasses [ DEVise get "window" ]
    foreach winClass $windowClasses {
	# Get all windows for this class
	set windows [ DEVise get "window" $winClass ]
	foreach win $windows {
	    # Get all views inserted into this window
	    set views [DEVise getWinViews $win ]
	    foreach v $views {
                set viewName "\{$v\}"
                if {!$asBatchScript} {
                    set viewVar [DictLookup $viewDict $v]
                    set viewName "\$$viewVar"
                }
		puts $f "DEVise insertWindow $viewName \{$win\}"
	    }
	}
    }
}

############################################################

# Save name of views in links
proc SaveLinkViews { file dict asBatchScript } {
    set linkSet [LinkSet]
    foreach link $linkSet {
	set views [DEVise getLinkViews $link]
	foreach v $views {
            set viewName "\{$v\}"
            if {!$asBatchScript} {
	        set viewVar [DictLookup $dict $v]
	        set viewName "\$$viewVar"
            }
	    puts $file "DEVise insertLink \{$link\} $viewName"
	}
	set v [DEVise getLinkMaster $link]
	if {$v != ""} {
            set viewName "\{$v\}"
            if {!$asBatchScript} {
	        set viewVar [DictLookup $dict $v]
	        set viewName "\$$viewVar"
            }
	    puts $file "DEVise setLinkMaster \{$link\} $viewName"
	}
    }
}

############################################################

# Save name of views in cursors
proc SaveCursorViews { file dict asBatchScript } {
    set cursorSet [CursorSet]
    foreach cursor $cursorSet {
	set views [DEVise getCursorViews $cursor]
	set src [lindex $views 0]
	set dst [lindex $views 1]
	if {$src != ""} {
            set viewName ""
            if {!$asBatchScript} {
                set viewVar [DictLookup $dict $src]
                set viewName "\$$viewVar"
            }
	    puts $file "DEVise setCursorSrc \{$cursor\} $viewName"
	}
	if {$dst != ""} {
            set viewName ""
            if {!$asBatchScript} {
                set viewVar [DictLookup $dict $dst]
                set viewName "\$$viewVar"
            }
	    puts $file "DEVise setCursorDst \{$cursor\} $viewName"
	}
    }
}

############################################################

# Save session
proc DoActualSave { infile asTemplate asExport withData asBatchScript } {
    global templateMode schemadir datadir

    # you can't save an imported file until it has been merged
    if {$templateMode} { 
	dialog .open "Merge Data" \
	    "You must merge an imported template before saving it" "" 0 OK
	return
    }

    if {$asBatchScript} {
        if {$asTemplate || $asExport || $withData} {
            dialog .open "Incompatible Selection" \
                    "Cannot save batch script as a template\
                    or as an exported template or with data" "" 0 OK
            return
        }
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

    set header [DEVise getFileHeader session]
    puts $f $header
    unset header

    if {$asExport} {
	if {$withData} {
	    puts $f "# WARNING: do not edit this file manually.  Proper"
	    puts $f "# functioning depends on the exact offsets of the"
	    puts $f "# data in the file."
	    puts $f ""
	}
	puts $f "global templateMode sourceList datadir schemadir"
	puts $f "global physSchema logSchema"
	puts $f "set templateMode 1"
	# The sourceList will have to be reset in the imported file
#	puts $f "unset sourceList"
	puts $f ""
    }

    # Save the schemas or the references to the schema files.
    SaveAllSchemas $f $asExport

    # Save the commands needed to create the views.
    set viewDict ""
    SaveViews $f viewDict $asBatchScript

    if {!$asBatchScript} {
        # Save the commands needed to create the data sources.
        set positionOffsets(dummy) ""
        set lengthOffsets(dummy) ""
        SaveDataSources $f $asExport $asTemplate $withData \
                positionOffsets lengthOffsets
    }

    # Save the commands needed to create the TDatas.
    set fileDict ""
    SaveCreateTDatas $f $asTemplate fileDict $asBatchScript

    # Save the commands needed to create the mappings.
    set mapDict ""
    SaveMappings $f $fileDict mapDict $asBatchScript

    SaveMisc $f $asTemplate $asExport $viewDict $mapDict $asBatchScript

    if {$asTemplate || $asExport} {
        puts $f ""
	puts $f "return 1"
    }

    # Save the actual data if required.
    if {$withData} {
	SaveAllData $f positionOffsets lengthOffsets
    }

    if {$asTemplate || $asExport || $asBatchScript} {
	ChangeStatus 0
	close $f
	return
    }

    # Save the pixmaps.
    set savedCurView ""
    SavePixmaps $f $infile savedCurView

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
	DoSaveAs 0 0 0 0
	return
    }

    DoActualSave $sessionName 0 0 0 0
}

############################################################

proc DoSaveAs { asTemplate asExport withData asBatchScript } {
    global sessionName fsBox sessiondir templateMode

    # Get file name
    set fsBox(path) $sessiondir
    set fsBox(pattern) *.tk
    set file [ FSBox ]
    
    if {$file == ""} { return }

    set button [ dialog .saveSession "Save Session" \
	    "Save session to file\n$file?"  "" 0  OK {Cancel} ]
    if {$button != 0} { return }

    DoActualSave $file $asTemplate $asExport $withData $asBatchScript
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
	    set header [DEVise getFileHeader logSchema]
	    puts $fileP $header
	    unset header
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
	    set header [DEVise getFileHeader physSchema]
	    puts $fileP $header
	    unset header
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

# Save all schemas to the given file.
proc SaveAllSchemas { fileId asExport } {
    puts $fileId "# Import schemas"
    set catFile [ DEVise catFiles ]
    foreach file $catFile {
    	puts "current catFile: $file"
	SaveOneSchema $fileId $asExport $file
    }
    puts $fileId ""
}

############################################################

# Save one schema to the given file.
proc SaveOneSchema { fileId asExport schemaFile } {
    if {$asExport} {
	# Read the first nonblank, non-comment line of this schema to
	# determine whether it's a physical or logical schema.
	set fileP [open $schemaFile r]
	readSchemaLine $fileP schemaTest
	close $fileP

	if {[lindex $schemaTest 0] == "physical"} {
	    # schemaFile is a logical schema -- read in the logical schema and
	    # set the physical schema name.
            set lschema ""
	    ReadSchemaFile $schemaFile lschema
	    set pschemaFile [lindex $schemaTest end]
	} else {
	    # schemaFile is a physical schema.
	    set pschemaFile $schemaFile
	    set lschema ""
	}

	# Now read the physical schema.
	set pschema ""
	ReadSchemaFile $pschemaFile pschema

	set sname [file tail $schemaFile]
	puts $fileId "set physSchema($sname) \"$pschema\""
	puts $fileId "set logSchema($sname) \"$lschema\""
	puts $fileId "DEVise parseSchema $sname \$physSchema($sname) \$logSchema($sname)"
    } elseif {[string index $schemaFile 0] != "."} {

    # this is a hack to find out if dte has registered this schema
    # dte schemas are just the table names and therefore start with .

 	puts $fileId "DEVise importFileType $schemaFile"
    } else {
 	puts $fileId "DEVise dteImportFileType $schemaFile"
    }
}

############################################################

# Read the contents of a schema file into the given variable, stripping
# out all blank and comment lines.
proc ReadSchemaFile { schemaFile schemaRef } {
    upvar $schemaRef schema

    set fileP [open $schemaFile r]

    set line ""
    set len [readSchemaLine $fileP line]
    while {$len >= 0} {
        set schema "$schema$line\n"
        set len [ readSchemaLine $fileP line ]
    }

    close $fileP
}

############################################################

# Save the appropriate commands to create the data sources.
proc SaveDataSources { fileId asExport asTemplate withData
                       posOffRef lenOffRef } {
    upvar $posOffRef positionOffsets
    upvar $lenOffRef lengthOffsets
    global sourceList derivedSourceList

    puts $fileId "# Import file (Create TData)"
    puts $fileId ""
    if {!$asExport && !$asTemplate} {
	puts $fileId "set loadPixmap 1"
	puts $fileId ""
    } 

    # If we're saving data in the session file, we refer to the session file
    # via the 'file' variable so things will work if the session file is
    # renamed.
    if {$withData} {
	puts $fileId "set filename \[file tail \$file\]"
	puts $fileId "set directory \[file dirname \$file\]"
        puts $fileId ""
    }

    if {$asExport} {
        # Placeholder for data offset and length.  This will be written to the
    	# template now, and then overwritten later with the real offset and
    	# length when we know them.
    	set placeholder "          "

	foreach class [DEVise get tdata] {
	    foreach inst [DEVise get tdata $class] {
		set params [DEVise getCreateParam tdata $class $inst]
		set fileAlias [lindex $params 0]
                set err [catch {set sourcedef $derivedSourceList($fileAlias)}]
                if {$err} {
                    # set sourcedef $sourceList($fileAlias)
				set sourcedef [DEVise dteShowCatalogEntry $fileAlias]
				set sourcedef [lindex $sourcedef 0]
				set sourcedef [lrange $sourcedef 1 end]
                }
                set type [lindex $sourcedef 0]

                # For non-UNIXFILE data sources, we cannot store $filename
                # and $directory in the source definition as we would
                # be overwriting some other stuff (like the URL);
                # however we need to store some values in positionOffsets
                # and lengthOffsets so that SaveAllData can reference
                # the values.

		if {$type == "UNIXFILE" && $withData} {
		    # Replace the filename with '$filename' and the directory
		    # with '$directory'.  We use format instead of lreplace
		    # because lreplace puts braces around $filename and
		    # $directory, and that goofs things up when you try
		    # to load the template.
		    set sourcedef [format \
			"%s \$filename %s %s \{%s\} %s %s \$directory" \
	    		[lindex $sourcedef 0] [lindex $sourcedef 2] \
			[lindex $sourcedef 3] [lindex $sourcedef 4] \
			[lindex $sourcedef 5] [lindex $sourcedef 6]]

		    puts -nonewline $fileId \
                        "addDataSource \{$fileAlias\} \[list $sourcedef \{"
		    set "positionOffsets($fileAlias)" [tell $fileId]
		    puts -nonewline $fileId "$placeholder "
		    set "lengthOffsets($fileAlias)" [tell $fileId]
		    puts $fileId "$placeholder\}\]"
		} else {
		    set "positionOffsets($fileAlias)" 0
		    set "lengthOffsets($fileAlias)" 0
                    if {$type != "BASICSTAT"} {
                        puts $fileId "addDataSource \{$fileAlias\} \[list $sourcedef\]"
                    }
		}
	    }
	}
        puts $fileId ""
    }
}

############################################################

# Save the commands to create the TDatas to the given file.
proc SaveCreateTDatas { fileId asTemplate fileDictRef asBatchScript } {
    upvar $fileDictRef fileDict

    set totalTData 0
    foreach class [DEVise get tdata] {
	incr totalTData [llength [DEVise get tdata $class]]
    }

    set fileNum 1
    foreach class [DEVise get tdata] {
	foreach inst [DEVise get tdata $class] {
	    set params [DEVise getCreateParam tdata $class $inst]
	    set sname [lindex $params 0]
	    set stype [lindex $params 1]
	    set param [lindex $params 2]

#	    set fileDict [DictInsert $fileDict $sname tdata_$fileNum]
# modified by DD

 	    set fileDict [DictInsert $fileDict $inst tdata_$fileNum]
	    set tdataVar tdata_$fileNum
            if {$asBatchScript} {
		puts $fileId "DEVise dataSegment \{$sname\} \{$param\} 0 0"
		puts $fileId "DEVise create tdata \{$class\} \{$sname\} \{$stype\} \{$param\}"
            } elseif {$asTemplate} {
		puts $fileId "set $tdataVar \[ GetTDataTemplate $class $totalTData $fileNum \]"
		puts $fileId "if \{\$$tdataVar == \"\"\} \{"
		puts $fileId "\treturn 0"
		puts $fileId "\}"
	    } else {
		puts $fileId "if \{ \$template \} \{"
		puts $fileId "\tset loadPixmap 0"
		puts $fileId "\tset $tdataVar \[ GetTDataTemplate $class $totalTData $fileNum \]"

		puts $fileId "\tif \{\$$tdataVar == \"\"\} \{"
		puts $fileId "\t\treturn 0"
		puts $fileId "\t\}"
		puts $fileId "\} else \{"
		if {[string index $class 0] == "."} {

			# this is a hack to find out if dte has registered this schema
			# dte schemas are just the table names and therefore start with .

			puts $fileId "\tset $tdataVar \[DEVise create tdata \{$class\} $params \]"
		} else {
			puts $fileId "\tset $tdataVar \{$sname\}"
			puts $fileId "\tset source \[OpenDataSource $$tdataVar]"
			puts $fileId "\tif \{\$source == \"\"\} \{"
			puts $fileId "\t\treturn 0"
			puts $fileId "\t\}"
			puts $fileId "\tset sname \[lindex \$source 0\]"
			puts $fileId "\tset param \[lindex \$source 1\]"
			puts $fileId "\tset stype \[lindex \$source 2\]"
			puts $fileId "\tDEVise create tdata \{$class\} \$sname \$stype \$param"
			puts $fileId "\tif \{\$sname != \$$tdataVar\} \{"
			puts $fileId "\t\tset loadPixmap 0"
			puts $fileId "\t\tset $tdataVar \$sname"
			puts $fileId "\t\}"
		}
		puts $fileId "\}"
	    }

	    incr fileNum
	}
    }
    puts $fileId  ""
}

############################################################

# Save mappings to the given file.
proc SaveMappings { fileId fileDict mapDictRef asBatchScript } {
    upvar $mapDictRef mapDict

    # build up list of class names of interpreted mappings
    set interpretedGDataClasses ""
    foreach m [ InterpretedGData ] {
        set class [ GetClass mapping $m ]
        if {[lsearch $interpretedGDataClasses $class] < 0} {
            # puts "Adding class $class"
            lappend interpretedGDataClasses $class
        }
    }

    puts $fileId "# Create interpreted mapping classes"
    foreach mclass $interpretedGDataClasses {
	puts $fileId "DEVise createMappingClass \{$mclass\}"
    }
    puts $fileId ""

    puts $fileId "# Create mapping instances (GData)"
    set mapDict ""
    set mapNum 1
    foreach mapClass [DEVise get mapping] {
	foreach map [DEVise get mapping $mapClass] {
	    set params [DEVise getCreateParam mapping $mapClass $map]
	    set fileAlias [lindex $params 0]
	    set gdataName [lindex $params 1]
            if {$map != $gdataName} {
                puts "Warning: mapping $map is named $gdataName"
            }
            set width [ DEVise getPixelWidth $map ]
            if {$asBatchScript} {
                puts $fileId "DEVise create mapping \{$mapClass\} \{$fileAlias\} \{$map\} [lrange $params 2 end]"
                puts $fileId "DEVise setPixelWidth \{$map\} \{$width\}"
            }
            set fileVar [ DictLookup $fileDict $fileAlias ]
            regsub $fileAlias $map \%s gdataExpr
            if {!$asBatchScript} {
                puts $fileId "set map_$mapNum \[ format \"$gdataExpr\" \$$fileVar \]"
                puts $fileId "DEVise create mapping \{$mapClass\} \$$fileVar \$map_$mapNum [lrange $params 2 end]"
                puts $fileId "DEVise setPixelWidth \$map_$mapNum \{$width\}"
            }
	    set mapDict [ DictInsert $mapDict $map map_$mapNum ]
	    incr mapNum
            puts $fileId ""
	}
    }
}

############################################################

# Save views to the given file.
proc SaveViews { fileId viewDictRef asBatchScript } {
    upvar $viewDictRef viewDict

    puts $fileId "# Create views"
    set viewDict ""
    set viewNum 1
    foreach viewClass [ DEVise get view ] {
	foreach inst [ DEVise get view $viewClass ] {
	    set params [DEVise getCreateParam view $viewClass $inst]
	    set viewVar view_$viewNum
            set viewName "\{$inst\}"
            if {!$asBatchScript} {
                puts $fileId "set $viewVar \{$inst\}"
	        set viewName "\$$viewVar"
            }
	    puts $fileId "DEVise create view \{$viewClass\} $viewName [lrange $params 1 end]"
	    set viewLabelParams [DEVise getLabel $inst]
	    puts $fileId "DEVise setLabel $viewName $viewLabelParams"
	    set viewStatParams [DEVise getViewStatistics $inst]
	    puts $fileId "DEVise setViewStatistics $viewName $viewStatParams"
	    set viewDimensions [DEVise getViewDimensions $inst]
	    puts $fileId "DEVise setViewDimensions $viewName $viewDimensions"
	    set viewSolid3D [DEVise getViewSolid3D $inst]
	    puts $fileId "DEVise setViewSolid3D $viewName $viewSolid3D"
	    set viewXYZoom [DEVise getViewXYZoom $inst]
	    puts $fileId "DEVise setViewXYZoom $viewName $viewXYZoom"
	    set viewDataDisp [DEVise getViewDisplayDataValues $inst]
	    puts $fileId "DEVise setViewDisplayDataValues $viewName $viewDataDisp"
	    set viewPileMode [DEVise getViewPileMode $inst]
	    puts $fileId "DEVise setViewPileMode $viewName $viewPileMode"
	    set viewOverride [DEVise getViewOverrideColor $inst]
	    puts $fileId "DEVise setViewOverrideColor $viewName $viewOverride"

	    set font [DEVise getFont $inst title]
	    puts $fileId "DEVise setFont $viewName title $font"
	    set font [DEVise getFont $inst "x axis"]
	    puts $fileId "DEVise setFont $viewName {x axis} $font"
	    set font [DEVise getFont $inst "y axis"]
	    puts $fileId "DEVise setFont $viewName {y axis} $font"

	    set viewDict [DictInsert $viewDict $inst $viewVar]
	    incr viewNum
	}
    }
    puts $fileId ""

    if {!$asBatchScript} {
        puts $fileId "scanDerivedSources"
        puts $fileId ""
    }
}

############################################################

# Save miscellaneous stuff to the given file.
proc SaveMisc { fileId asTemplate asExport viewDict mapDict asBatchScript } {
    puts $fileId "# Create windows"
    SaveCategory $fileId "window" 
    puts $fileId ""

    puts $fileId "# Setup window layouts"
    foreach win [ WinSet ] {
	set layout [ DEVise getWindowLayout $win ]
	puts $fileId "DEVise setWindowLayout \{$win\} $layout"
    }
    puts $fileId ""
    
    puts $fileId "# Create Links"
    SaveCategory $fileId "link" 
    puts $fileId ""

    puts $fileId "# Create Cursors"
    SaveCategory $fileId "cursor"
    puts $fileId ""

    puts $fileId "# Create axislabel"
    SaveCategory $fileId "axisLabel"
    puts $fileId ""

    puts $fileId "# Create actions"
    SaveCategory $fileId "action"
    puts $fileId ""

    puts $fileId "# Put labels into views"
    SaveAllViewAxisLabel $fileId $viewDict $asBatchScript
    puts $fileId ""

    puts $fileId "# Put action into view"
    SaveAllViewAction $fileId $viewDict $asBatchScript
    puts $fileId ""

    puts $fileId "# Link views"
    SaveLinkViews $fileId $viewDict $asBatchScript
    puts $fileId ""

    puts $fileId "# Put views in cursors"
    SaveCursorViews $fileId $viewDict $asBatchScript
    puts $fileId ""

    puts $fileId "# Put axis label into views"
    puts $fileId ""

    puts $fileId "# Insert mappings into views"
    SaveAllViewMappings $fileId $mapDict $viewDict $asBatchScript
    puts $fileId ""

    puts $fileId "# Insert views into windows"
    SaveAllWindowViews $fileId $viewDict $asBatchScript
    puts $fileId ""

    puts $fileId "# Init history of view"
    foreach view [ViewSet] {
        set viewName "\{$view\}"
        if {!$asBatchScript} {
            set viewVar [DictLookup $viewDict $view]
            set viewName "\$$viewVar"
        }
	puts $fileId "DEVise clearViewHistory $viewName"
	if {$asTemplate || $asExport || $asBatchScript} {
	    continue
	}
        foreach hist [ DEVise getVisualFilters $view ] {
            puts $fileId "DEVise insertViewHistory \$$viewVar {[lindex $hist 0]} {[lindex $hist 1]} {[lindex $hist 2]} {[lindex $hist 3]} {[lindex $hist 4]}"
        }
    }

    puts $fileId ""
    puts $fileId "# Set camera location for each view"
    foreach view [ViewSet] {
        set viewName "\{$view\}"
        if {!$asBatchScript} {
            set viewVar [DictLookup $viewDict $view]
            set viewName "\$$viewVar"
        }
	set camera [DEVise get3DLocation $view]
	set x [lindex $camera 1]
	set y [lindex $camera 2]
	set z [lindex $camera 3]
	set fx [lindex $camera 4]
	set fy [lindex $camera 5]
	set fz [lindex $camera 6]
	puts $fileId "DEVise set3DLocation $viewName $x $y $z $fx $fy $fz"
    }
}

############################################################

# Save all TDatas (the actual data) to the given file.
proc SaveAllData { fileId positionOffsets lengthOffsets } {
    upvar $positionOffsets posOff
    upvar $lengthOffsets lenOff

    # Keep from making duplicates by keeping a list of what we have saved.
    set dataSaved ""

    set classes [ DEVise get tdata ]
    foreach class $classes {
        set instances [ DEVise get tdata $class ]
        foreach inst $instances {
	    if {[lsearch $dataSaved $inst] == -1} {
		SaveOneData $fileId $inst $posOff($inst) $lenOff($inst)
	        lappend dataSaved $inst
	    }
        }
    }

    return
}

############################################################

# Save the given TData (the actual data) to the given output (template)
# file.
#   fileId is the file descriptor of the output file.
#   tdata is the name of the TData to save.
#   positionOffset is the offset in the output file at which we
#     must write the position of the data in the output file.
#   lengthOffset is the offset in the output file at which we
#     must write the length of the data in the output file.

proc SaveOneData { fileId tdata positionOffset lengthOffset } {
    global sourceList derivedSourceList

    # Get the source information for this TData.
    set err [catch {set source $derivedSourceList($tdata)}]
    if {$err} {
        # set source $sourceList($tdata)
		set source [DEVise dteShowCatalogEntry $tdata]
		set source [lindex $source 0]
		set source [lrange $source 1 end]
    }

    # Make sure this TData is a UNIXFILE, otherwise we can't save it yet.
    set type [lindex $source 0]
    if {$type != "UNIXFILE"} {
	return
    }

    set filename [lindex $source 1]
    set dirname [lindex $source 7]

    # Open data file here so we don't output the other stuff if open fails.
    set tdFile [open $dirname/$filename r]

    # Put a comment into the output file telling what TData this is.
    puts $fileId ""
    puts $fileId "### $tdata"

    # Now read the TData from its file (determining whether the TData occupies
    # the entire file or just a segment of the file).
    set segment [lindex $source 8]
    if {$segment == ""} {
	# Read the whole file.
    	set data [read $tdFile]
    } else {
	# Read just the specified part of the file.
	seek $tdFile [lindex $segment 0] start
    	set data [read $tdFile [lindex $segment 1]]
    }

    close $tdFile

    # Dump the TData into the output file, saving its offset and length.
    set tdataPosition [tell $fileId]
    puts $fileId $data
    unset data
    set tdataLength [expr [tell $fileId] - $tdataPosition]

    # Now go back and fill in the data offset and length in the places
    # we reserved for them in the addDataSource command.  Note that after
    # doing so, we need to seek back to the end of the output file so that
    # subsequent writes will write at the right place.
    seek $fileId $positionOffset
    puts -nonewline $fileId $tdataPosition
    seek $fileId $lengthOffset
    puts -nonewline $fileId $tdataLength
    seek $fileId 0 end

    return
}

############################################################

# Save pixmaps.
proc SavePixmaps { fileId infile savedCurViewRef } {
    upvar $savedCurViewRef savedCurView
    global curView

    # Work-around for bug #135 (pixmaps not restored properly,
    # so they are disabled temporarily)
    return

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

    puts $fileId ""
    puts $fileId "# Load pixmaps from views"
    puts $fileId "set pixmapName $bitmapFile"
    puts $fileId "if \{ \$loadPixmap && \[ file exists \$pixmapName \] \} \{"
    puts $fileId "	set pixmapFile \[ DEVise open \$pixmapName rb \]"
    puts $fileId "	set fileDate \[ DEVise readLine \$pixmapFile \]"
    puts $fileId "	set sessionDate \{$date\}"
    puts $fileId "	if \{\$fileDate == \$sessionDate\} \{ "
    foreach view [ViewSet] {
	DEVise savePixmap $view $bitF
	puts $fileId "		DEVise loadPixmap \{$view\} \$pixmapFile"
    }
    puts $fileId "	\}"
    puts $fileId "	DEVise close \$pixmapFile"
    puts $fileId "\}"
    DEVise close $bitF
}

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
#	Revision 1.71  1997/12/01 21:22:07  wenger
#	Merged the cleanup_1_4_7_br branch through the cleanup_1_4_7_br3 tag
#	into the trunk.
#
#	Revision 1.70  1997/11/12 23:26:53  donjerko
#	Added some dialog windows for error reporting.
#
#	Revision 1.69.4.2  1997/12/01 18:35:03  wenger
#	Fixed bug 246 (GUI no longer allows data source names containing dots
#	or spaces).
#
#	Revision 1.69.4.1  1997/11/26 23:43:44  wenger
#	Added the option to create a new schema in the data source editing
#	GUIs for UNIXFILES and Tables; fixed a few bugs in the data source
#	and schema GUIs.
#
#	Revision 1.69  1997/09/09 23:25:20  donjerko
#	*** empty log message ***
#
#	Revision 1.68  1997/05/28 16:57:35  wenger
#	Changed back to the query processor version that sometimes returns
#	too many records to hopefully avoid returning too few; other minor
#	cleanups, etc.
#
#	Revision 1.67  1997/05/28 15:39:55  wenger
#	Merged Shilpa's layout manager code through the layout_mgr_branch_2 tag.
#
#	Revision 1.66.4.1  1997/05/20 16:11:33  ssl
#	Added layout manager to DEVise
#
#	Revision 1.66  1997/04/22 15:25:54  wenger
#	Conditionaled out lots of debug code; fixed data source visualization
#	window so the window for the data again defaults to 'New' if there are
#	no windows.
#
#	Revision 1.65  1997/04/21 23:08:27  guangshu
#	Make statistics work with DTE.
#
#	Revision 1.64  1997/03/23 23:46:28  donjerko
#	*** empty log message ***
#
#	Revision 1.63  1997/03/20 22:37:42  guangshu
#	Added GDATASTAT_X AND GDATASTAT_Y.
#
#	Revision 1.62  1997/03/18 20:42:42  donjerko
#	Changes for GUI.
#
#	Revision 1.61  1997/03/14 18:37:10  donjerko
#	*** empty log message ***
#
#	Revision 1.60  1997/03/06 02:37:45  donjerko
#	Added window to create StandardTable
#
#	Revision 1.59  1997/02/25 22:16:56  donjerko
#	Fixed some bugs in tcl - dte communication
#
#	Revision 1.58  1997/02/18 18:07:52  donjerko
#	Added index deletions.
#
#	Revision 1.57  1997/02/14 16:48:08  wenger
#	Merged 1.3 branch thru rel_1_3_1 tag back into the main CVS trunk.
#
#	Revision 1.56  1997/02/03 04:12:42  donjerko
#	Catalog management moved to DTE
#
#	Revision 1.55.4.2  1997/02/09 16:14:47  wenger
#	Cleaned up the data source GUI a little more (as per suggestions from
#	Miron).
#
#	Revision 1.55.4.1  1997/02/07 15:21:45  wenger
#	Updated Devise version to 1.3.1; fixed bug 148 (GUI now forces unique
#	window names); added axis toggling and color selections to Window menu;
#	other minor fixes to GUI; show command to Tasvir now requests image to
#	be shown all at once.
#
#	Revision 1.55  1997/01/30 17:25:42  wenger
#	Removed all data source types except UNIXFILE and WWW.
#
#	Revision 1.54  1997/01/30 02:12:36  beyer
#	sources now listed in alphabetical order, regardless of case.
#
#	Revision 1.53  1997/01/22 20:13:58  wenger
#	Removed other non-functional user interface components (includes workaround
#	for bug 127); fixed a number of OK/Cancel button positions.
#
#	Revision 1.52  1996/12/27 14:59:51  donjerko
#	Import button is used only for importing new streams.
#	Added a button "Display" in the import window that immediately goes to
#	visualization.
#
#	Revision 1.51  1996/12/23 22:20:41  donjerko
#	  Commented out a bunch of non-working Tk/Tcl code.  Changed OK/Cancel
#	  pairs to have OK always on the left, Cancel on the right.  Renamed
#	  and moved around various menu items; to avoid confusion.
#
#	  More changes will follow later, after you're had time to adjust.
#
#	  Shaun/Donko
#
#	Revision 1.50  1996/12/15 07:01:39  donjerko
#	Added the window to create an index on streams.
#
#	Revision 1.49  1996/11/15 10:06:26  kmurli
#	Changed importFile parameters and ParseCat parameters to take in the file type
#	and data file name so that a whole query can be formed if necessary for calling
#	DQL type. (In case of a query schema)
#
#	Revision 1.48  1996/11/03 02:41:53  kmurli
#	Modified to include the query schema level. Also modified to include DQL
#	processing
#
#	Revision 1.47  1996/11/01 19:29:02  kmurli
#	Changed to include the DQL type. Also calls importFileDQL command in
#	ParseAPI.c to facilitate the creation of the DQL classes.
#
#	Revision 1.46  1996/08/29 22:29:31  guangshu
#	Changed puts to proc Puts in case the client wants to be quiet.
#
#	Revision 1.45  1996/07/22 23:50:57  guangshu
#	Added statistics for gdata. The statistics includes count, ysum, max, mean, min.
#
#	Revision 1.44  1996/07/19 18:29:45  guangshu
#	Added support for histograms.
#
#	Revision 1.43  1996/07/16 19:10:00  jussi
#	Had to undo previous change.
#
#	Revision 1.42  1996/07/16 19:05:57  jussi
#	Changed source type of BASICSTAT sources to DERIVED.
#
#	Revision 1.41  1996/07/13 17:29:35  jussi
#	Schema name for view statistics is now BASICSTAT.
#
#	Revision 1.40  1996/07/12 21:54:22  jussi
#	Added BASICSTAT derived data source type.
#
#	Revision 1.39  1996/07/11 17:26:17  wenger
#	Devise now writes headers to some of the files it writes;
#	DataSourceSegment class allows non-fixed data length with non-zero
#	offset; GUI for editing schema files can deal with comment lines;
#	added targets to top-level makefiles to allow more flexibility.
#
#	Revision 1.38  1996/07/08 17:50:54  jussi
#	Further change to fix derived data source.
#
#	Revision 1.37  1996/07/08 17:24:41  jussi
#	Replaced STAT source type with UNIXFILE.
#
#	Revision 1.36  1996/07/01 19:37:43  jussi
#	Made changes to reflect new TData constructor interface. Web
#	data caching is now handled in the server, not in the client.
#
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

#set sourceTypes(COMMAND) "{Unix Command Output} $schemadir/logical/COMMAND"
#set sourceTypes(COMPUSTAT) "{Annual and Quarterly Company Financial Data} $schemadir/logical/COMPUSTAT compustat.idx"
#set sourceTypes(CRSP) "{Security Data} $schemadir/logical/CRSP crsp_dsm94.idx"
#set sourceTypes(CRSP_NSDQ) "{Nasdaq Security Data} $schemadir/logical/CRSP crsp_ndsm94.idx"
#set sourceTypes(ISSM) "{Historical Stock Data (Trades and Quotes} $schemadir/logical/ISSM-T issm.idx"
#set sourceTypes(NETWORK) "{Network Server Output} $schemadir/logical/NETWORK"
#set sourceTypes(SEQ) "{SEQ Query Output} $schemadir/logical/SEQ"
#set sourceTypes(SQL) "{SQL Query Output} $schemadir/logical/SQL"
set "sourceTypes(SQLView)" "{SQL View} $schemadir/logical/SQL"
set "sourceTypes(Table)" "{Table} $schemadir/logical/SQL"
set "sourceTypes(StandardTable)" "{StandardTable} $schemadir/logical/SQL"
set "sourceTypes(Directory)" "{Directory} $schemadir/logical/SQL"
set sourceTypes(UNIXFILE) "{Unix File} $schemadir/logical/UNIXFILE"
set sourceTypes(DEVise) "{Devise} $schemadir/logical/UNIXFILE"
#set sourceTypes(WWW) "{World Wide Web} $schemadir/logical/WWW"

set sourceFile $datadir/sourcedef.tcl
#set sourceFile /local.doc/oldstuff/coral/newrun/northeast/devise/sourcedef.tcl
if {[file exists $sourceFile]} {
#TEMPTEMP -- this is still old-style data source code!!!
    Puts "Using data stream catalog $sourceFile"
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

proc addQuotes {s} {

	# This procedure returns the input string enclosed in quotes, escaping
	# all the internal quotes and backslashes.
	# It is used on some strings to be passed to DTE

     set retVal "\""
     set len [string length $s]
     for {set i 0} {$i < $len} {incr i} {
          set ch [string index $s $i]
          if {$ch == "\\"} {
               append retVal "\\\\"
          } elseif {$ch == "\""} {
               append retVal "\\\""
          } else {
               append retVal $ch
          }
     }
     append retVal "\""
     return $retVal
}

proc addSQLQuotes {string quote} {

	# This procedure is not finished yet

     set retVal $guote
     set len [string length $s]
     for {set i 0} {$i < $len} {incr i} {
          set ch [string index $s $i]
          if {$ch == "\\"} {
               append retVal "\\\\"
          } elseif {$ch == "\""} {
               append retVal "\\\""
          } else {
               append retVal $ch
          }
     }
     append retVal "\""
     return $retVal
}

proc newViewFile {{schemaFile ""} {dataFile ""}} {
	global schemadir
	if {$schemaFile != ""} {
		set logicalAttrlist [DEVise dteShowAttrNames $schemaFile $dataFile]
	} else {
		set logicalAttrlist ""
	}
	set result [qbrowse 1 ""]
	if {$result != ""} {
		set viewName [lindex $result 0]
		set select [lindex $result 2]
		set as [lindex $result 3]
		set where [lindex $result 4]
		set fileName "$schemadir/query/$viewName"

	#	DEVise dteSaveSingleView $fileName $select $as $where

		saveViewFile $fileName $select $as $where
#		puts "$viewName $select $as $where"
		return $fileName
	} else {
		return ""
	}
}

proc saveSources {} {
    global sourceFile sourceList sourceTypes

    catch { exec mv $sourceFile $sourceFile.bak }

    set f [open $sourceFile w]

    set header [DEVise getFileHeader dataCat]
    puts $f $header
    unset header

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

proc updateStreamDef_and_display {dispname} {

	updateStreamDef
	set tdata_schema_pair [OpenAndDefineDataSources 0 $dispname]
	if {$tdata_schema_pair == ""} {
		puts "tdata = NULL"
		return
	}
	set tdata [lindex $tdata_schema_pair 0]
	set schemaname [lindex $tdata_schema_pair 1]
	MacroDefAuto $tdata $schemaname
}

proc updateStreamDef {} {
    global schemadir
    global sourceList sourceTypes editonly oldDispName
    global dispname source key schemafile evaluation priority command

    # Don't allow names with dots or spaces in them -- they will cause
    # the DTE problems.
    if {[string match {*[. ]*} $dispname]} {
      dialog .winError "Name error" \
	"Data source name cannot contain <dot> or <space>.  Please change data source name." \
	"" 0 OK
      return
    }

#	if {$source == "SEQ"} {
#	set schemafile [format "%s/%s.%s.schema" $schemadir/physical \
#		[string trim $source] [string trim $key]]
#	set schematype [format "%s.%s" [string trim $source] \
#		[string trim $key]]
#    }
#	if {$source == "DQL"} {
#	  set key [format "%s.%s" [string trim $source] [string trim $dispname]]
#	  set schemafile $key
#      #set schemafile [format "%s.%s" [string trim $source] [string trim $key]]
#      set schematype $schemafile 
#    }

    if {$dispname == "" || $key == "" || $schemafile == ""} {
	dialog .noName "Missing Information" \
		"Please enter all requested information." \
		"" 0 OK
	return
    }

#   set cachefile [getCacheName $source $key]

    set cachefile ""
    set command [string trim [.srcdef.top.row5.e1 get 1.0 end]]

#	if {$editonly && $source == "SEQ"} {
#		set oldCommand [lindex $sourceList($oldDispName) 7]
#		if {$oldCommand != $command} {
#	    		uncacheData $oldDispName "Query changed."
#		}
#	}
#	if {$editonly && $source == "DQL"} {
#		set oldCommand [lindex $sourceList($oldDispName) 7]
#		if {$oldCommand != $command} {
#		    uncacheData $oldDispName "Query changed."
#		}
#	}
#
#    if {$source != "SEQ" && $source != "DQL" } {
#	set newschematype [scanSchema $schemafile]
#	if {$newschematype == ""} { return }
#	set schematype $newschematype
#    }

	set newschematype [scanSchema $schemafile]
	if {$newschematype == ""} { return }
	set schematype $newschematype

    set sourcedef "[addQuotes $dispname] $source $key $schematype \
		$schemafile [addQuotes $cachefile] $evaluation $priority \
		[addQuotes $command] \"\""
		
#   set conflict [getSourceByCache $cachefile]
#   if {!$editonly && $conflict != ""} {
#	dialog .existsAlready "Data Stream Exists" \
#		"This definition conflicts\n\
#		with \"$conflict\"." "" 0 OK
#	return
#   }

    if {$editonly} {

# commented out temporarily (dd)
#
#	set oldCachefile $sourceList($oldDispName)
#
#	# this is WRONG !!!!!!!!!!!!!!!!!!!!!!!!!!!!!! 
#
#	if {[file exists $oldCachefile] \
#		&& $cachefile != $oldCachefile} {
#	    uncacheData $oldDispName "Source and/or key changed."
#	}
	
	# delete the old entry

	DEVise dteDeleteCatalogEntry [fullPathName $oldDispName]

    } else {
    		
		# this is an insertion, make sure that table does not exist already

		set errCode [catch {DEVise dteShowCatalogEntry [fullPathName $dispname]} oldEntry]
		if {$errCode != 0} {
			dialog .noName "DTE Error" $oldEntry "" 0 OK
			return
		}
		if {[llength $oldEntry] > 0} {
			dialog .sourceExists "Data Stream Exists" \
			"Data stream \"$dispname\" exists already." \
			"" 0 OK
			return
		}
    }
	
#   set "sourceList($dispname)" $sourcedef

    DEVise dteInsertCatalogEntry [CWD] $sourcedef

#   saveSources
    updateSources
    destroy .srcdef
}

############################################################
proc indexUpdate {logicalAttrlist} {

# input {list of available attrs}
# returns {indexName {list of key attrs} {list of add attrs} standAloneFlag}
	
	global indexName
	global keyAttrs
	global dataAttrs
	global standAlone

	set standAlone No 
	# Create the new index window and update the indices..
    global schemafile tgrps glist attrlist dispname
	
    toplevel .gbrowse
    wm title .gbrowse "Edit Index"
    wm geometry .gbrowse +50+50
    wm iconname .gbrowse "Groups"

    # Toplevel rows
    frame .gbrowse.attrs -relief raised
    frame .gbrowse.list -relief raised
    frame .gbrowse.ops -relief raised
    pack .gbrowse.attrs .gbrowse.list .gbrowse.ops \
	    -side top -fill x -padx 3m -pady 1m

    #----------------------------------------------------------
    # Create a listbox with all the attrs in the logical schema
	# This reads the attributes (items) in the logical schema 
	# and puts it in the logicalAttrlist list..
	
    label .gbrowse.attrs.label -text "List of Available Attributes"
    pack .gbrowse.attrs.label -side top -fill x
    listbox .gbrowse.attrs.list -relief raised -bd 1 \
	    -yscrollcommand ".gbrowse.attrs.scroll set" \
	    -font 9x15 -selectmode single
    scrollbar .gbrowse.attrs.scroll -command ".gbrowse.attrs.list yview"
    pack .gbrowse.attrs.scroll -side right -fill y
    pack .gbrowse.attrs.list -fill x

    # Populate the list with all the attrs 
    foreach attr $logicalAttrlist {
	  .gbrowse.attrs.list insert end $attr 
    }

    # Bind the mouse click on this list
    bind .gbrowse.attrs.list <Double-1> {
	  if {$keyAttrs == ""} {
	  	set keyAttrs  "$keyAttrs [.gbrowse.attrs.list get active] "
	} else {
	  	set keyAttrs  "$keyAttrs, [.gbrowse.attrs.list get active] "
	}
    }

    frame .gbrowse.ops.index -relief raised -bd 1
    frame .gbrowse.ops.name -relief raised -bd 1
    frame .gbrowse.ops.data -relief raised -bd 1
    frame .gbrowse.ops.standAlone -relief raised -bd 1
    frame .gbrowse.ops.op -relief raised

    pack .gbrowse.ops.index .gbrowse.ops.name \
    	.gbrowse.ops.data .gbrowse.ops.standAlone .gbrowse.ops.op \
    	-side top -fill x \
	    -padx 3m -pady 1m
    
    label .gbrowse.ops.index.indexLabel -text "Index Name "
    entry .gbrowse.ops.index.indexName -width 40 -relief sunken -bd 2 \
	    -textvariable indexName 
    pack .gbrowse.ops.index.indexLabel .gbrowse.ops.index.indexName -side left \
	   -padx 1m -pady 1m
    
    label .gbrowse.ops.name.label -text "Key attributes "
    entry .gbrowse.ops.name.entry -width 80 -relief sunken -bd 2 \
	    -textvariable keyAttrs 
    pack .gbrowse.ops.name.label .gbrowse.ops.name.entry -side left \
	   -fill x -padx 1m -pady 1m -expand 1

    label .gbrowse.ops.data.label -text "Data attributes "
    entry .gbrowse.ops.data.entry -width 80 -relief sunken -bd 2 \
	    -textvariable dataAttrs
    pack .gbrowse.ops.data.label .gbrowse.ops.data.entry -side left \
	   -fill x -padx 1m -pady 1m -expand 1

    label .gbrowse.ops.standAlone.label -text "Stand Alone "
    radiobutton .gbrowse.ops.standAlone.yes -text Yes -variable standAlone \
         -value Yes -anchor nw
    radiobutton .gbrowse.ops.standAlone.no -text No -variable standAlone \
         -value No -anchor nw
    pack .gbrowse.ops.standAlone.label .gbrowse.ops.standAlone.yes \
	.gbrowse.ops.standAlone.no \
      -side left -fill x -padx 1m

    button .gbrowse.ops.op.create -text "OK" -command {
	if {$indexName == "" || $keyAttrs == ""} {
		dialog .noName "Missing Information" \
			"Please enter all requested information." \
			"" 0 OK
	} else {
		destroy .gbrowse
	}
    }
    
	button .gbrowse.ops.op.quit -text "Cancel" -command {
		set indexName ""
		set keyAttrs ""
		set dataAttrs ""
		set standAlone No
	  destroy .gbrowse
    }
    
    pack .gbrowse.ops.op.create .gbrowse.ops.op.quit \
	    -side left -padx 3m -pady 1m -expand 1

	tkwait visibility .gbrowse
	grab set .gbrowse
	tkwait window .gbrowse
	
}

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

# modified by dd

#	puts "editonly = $editonly\n"
	set sourcedef ""
	if {$base != ""} {
		set errCode [catch {DEVise dteShowCatalogEntry [fullPathName $base]} sourcedef]
		if {$errCode != 0} {
			dialog .noName "DTE Error" $sourcedef "" 0 OK
			return
		}
	}

	if {[llength $sourcedef] == 0} {
		set err 1
	} else {
		set err 0
	}

    if {$base != "" && $err == 0} {

     # This is an update and an original table was found

     set dispname $base
     set source [lindex $sourcedef 1]
     set key [lindex $sourcedef 2]
     set schemafile [lindex $sourcedef 4]
puts "schemafile = $schemafile"
     set cachefile [lindex $sourcedef 5]
     set evaluation [lindex $sourcedef 6]
     set priority [lindex $sourcedef 7]
     set command [lindex $sourcedef 8]
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
    .srcdef.top.row2.e1.menu add radiobutton -label "UNIXFILE"
    tk_menuBar .srcdef .srcdef.sourcemenu

    label .srcdef.top.row3.l1 -text "Schema:"
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
    button .srcdef.top.row3.new -text "New..." -width 10 \
	    -command {
		global is_unidata
		set is_unidata 0
	        set schemafile [newschema]
	    }
    pack .srcdef.top.row3.l1 .srcdef.top.row3.e1 .srcdef.top.row3.b1 \
	    .srcdef.top.row3.new \
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

#   button .srcdef.but.index -text Index -width 10 -command indexUpdate 

    button .srcdef.but.ok -text OK -width 10 -command updateStreamDef
    button .srcdef.but.display -text Display -width 10 \
    		-command {updateStreamDef_and_display $dispname}
    button .srcdef.but.cancel -text Cancel -width 10 \
	    -command { destroy .srcdef }
    pack .srcdef.but.display .srcdef.but.ok .srcdef.but.cancel -in .srcdef.but.row1 \
	    -side left -padx 7m

    tkwait visibility .srcdef
}

###########################################################

set _cwd [list] 

proc CWD {} {
	global _cwd
#	puts "CWD returns .[join $_cwd .]"
	return ".[join $_cwd .]"
}

proc CD {path} {
	global _cwd
#	puts "in CD\{$path\}"
#	puts "dir before = [CWD]"
	lappend _cwd $path
#	puts "dir after = [CWD]"
	return [CWD]
}

proc CDup {} {
	global _cwd
	set len [llength $_cwd]
	set _cwd [lrange $_cwd 0 [expr $len - 2]]
	return [CWD]
}

proc notRootDir {} {
	global _cwd
	return [llength $_cwd]
}

proc fullPathName {tableName {currentDir ""}} {
	global _cwd
	if {$currentDir == ""} {
		if {[llength $_cwd]} {
			return ".[join $_cwd .].$tableName"
		} else {
			return ".$tableName"
		}
	} else {
		if {$currentDir == "."} {
			return ".$tableName"
		} else {
			return "$currentDir.$tableName"
		}
	}
}

proc isDirectory {typeName} {
	if {$typeName == "Directory" || $typeName == "DEVise" || $typeName == "DBServer"} {
		return 1
	} else {
		return 0
	}
}

proc defineANY {sourcetype} {
	global displayImmediately
	global _cwd

	set retVal [define$sourcetype ""]
 	puts "retVal = $retVal"
	set table [lindex $retVal 0]
	if {$retVal != ""} {
		set directory [selectStream "CD to parent directory"]
		if {$directory == ""} {
			return
		}
		set currentDir [CWD]

		DEVise dteInsertCatalogEntry $currentDir $retVal
          if {$displayImmediately} {
               displayTable [fullPathName $table $currentDir]
          }
	}
}

proc defineStandardTable {content} {

# these variable are used within the widgets of this procedure only

	global retVal tableName sourceType schemaFile url viewFile
	set sourcetype "StandardTable"
	set sourceType $sourcetype

# cannot display remote site
	global displayImmediately
	set displayImmediately 0

    toplevel .srcdef
 	wm title .srcdef "$sourcetype Definition"
    wm geometry .srcdef +100+100
    selection clear .srcdef

    frame .srcdef.top -relief groove -borderwidth 2
    frame .srcdef.but
    pack .srcdef.top -side top -pady 3m -fill both -expand 1
    pack .srcdef.but -side top -pady 3m -fill x
    frame .srcdef.but.row1
    pack .srcdef.but.row1 -side top

     set tableName [lindex $content 0]
	set url [lindex $content 2]
	
    frame .srcdef.top.row1
    frame .srcdef.top.row2
    frame .srcdef.top.row3
    frame .srcdef.top.row4 -relief groove -borderwidth 2
    frame .srcdef.top.row5
    pack .srcdef.top.row2 .srcdef.top.row3 .srcdef.top.row1 \
	     .srcdef.top.row4 .srcdef.top.row5 -side top -pady 1m

    label .srcdef.top.row1.l1 -text "Table Name:"
    entry .srcdef.top.row1.e1 -relief sunken -textvariable tableName \
	    -width 40
    pack .srcdef.top.row1.l1 .srcdef.top.row1.e1 -side left -padx 3m \
	    -fill x -expand 1

    label .srcdef.top.row2.l1 -text "URL"
    entry .srcdef.top.row2.e1 -relief sunken -textvariable url \
	    -width 40 
    button .srcdef.top.row2.b1 -text "Select..." -width 10 -command {
     global dialogListVar
    	if {$sourceType == "Directory"} {
		global schemadir fsBox
		set fsBox(path) $schemadir
		set fsBox(pattern) *.dte
		set file [FSBox "Select $sourceType file" newDirectory]
		if {$file != ""} { set url $file }
	} else {
		set answer [dialogList .selectTData "Select DEVise Script"  \
		              "Select DEVise Script" \
				   "" 0 { OK New Cancel } \
				   {http://fontina.cs.wisc.edu/cgi-fontina/donko/script} ]
		if {$answer == 0} {
			set url $dialogListVar(selected)
		} elseif {$answer == 1} {
			set url [lineEntryBox "New DEVise address" "HTTP address"]
		} else {
			set url ""
		}
	}
	set tableName ""
    }

    pack .srcdef.top.row2.l1 .srcdef.top.row2.e1 .srcdef.top.row2.b1 \
	    -side left -padx 2m -fill x -expand 1

	button .srcdef.but.ok -text OK -width 10 -command {
		if {$tableName == "" || $url == "" } {
			dialog .noName "Missing Information" \
				"Please enter all requested information." "" 0 OK
		} else {
                        # Don't allow names with dots or spaces in them -- they will cause
                        # the DTE problems.
                        if {[string match {*[. ]*} $tableName]} {
                          dialog .winError "Name error" \
	                    "Data source name cannot contain <dot> or <space>.  Please change data source name." \
	                    "" 0 OK
                          return
                        }
			set retVal "[addQuotes $tableName] $sourceType $url"
			destroy .srcdef
		}
	}

	button .srcdef.but.cancel -text Cancel -width 10  -command {
			set retVal ""
			destroy .srcdef
	}

	pack .srcdef.but.ok \
		.srcdef.but.cancel -in .srcdef.but.row1 -side left -padx 7m

	tkwait visibility .srcdef
	grab set .srcdef
	tkwait window .srcdef
	return $retVal
}

proc defineDEVise {content} {
	defineDEViseOrDir DEVise "HTTP address" $content
}

proc defineDirectory {content} {
	defineDEViseOrDir Directory "File" $content
}

proc defineSQLView {content} {
	# argument 0 stands for "no single table view"
	global viewName from select as where seqby
	set viewName [lindex $content 0]
	set type "SQLView"
	set as [lindex $content 2]
	set select [lindex $content 3]
	set from [lindex $content 4]
	set where [lindex $content 5]
	set groupBy [lindex $content 6]
	set seqby [lindex $content 7]
	set orderBy [lindex $content 8]

	set retVal [qbrowse 0 $viewName $from $select $as $where \
		$groupBy $seqby $orderBy]
	if {$retVal != ""} {
 		return $retVal
	} else {
		return
	}
}

proc defineUNIXFILE {content} {
	global displayImmediatelly
	set displayImmediatelly 0
	set tableName [lindex $content 0]
	if {$content == ""} {
		# new table
		defineStream $tableName 0
	} else {
		# editing existing table
		defineStream $tableName 1
	}
}

proc defineTable {content} {

# var displayImmediately has a boolean value and is used by a caller 

	global displayImmediately

# these variable are used within the widgets of this procedure only

	global retVal tableName sourceType schemaFile dataFile viewFile

    toplevel .srcdef
	wm title .srcdef "Edit Table"
    wm geometry .srcdef +100+100
    selection clear .srcdef

    frame .srcdef.top -relief groove -borderwidth 2
    frame .srcdef.but
    pack .srcdef.top -side top -pady 3m -fill both -expand 1
    pack .srcdef.but -side top -pady 3m -fill x
    frame .srcdef.but.row1
    pack .srcdef.but.row1 -side top

     set tableName [lindex $content 0]
	set sourceType "Table"
	set schemaFile [lindex $content 2]
	set dataFile [lindex $content 3]
	set viewFile [lindex $content 4]
	
    frame .srcdef.top.row1
    frame .srcdef.top.row2
    frame .srcdef.top.row3
    frame .srcdef.top.row4 -relief groove -borderwidth 2
    frame .srcdef.top.row5
    pack .srcdef.top.row2 .srcdef.top.row3 .srcdef.top.row1 \
	     .srcdef.top.row4 .srcdef.top.row5 -side top -pady 1m

    label .srcdef.top.row1.l1 -text "Table Name:"
    entry .srcdef.top.row1.e1 -relief sunken -textvariable tableName \
	    -width 40
    pack .srcdef.top.row1.l1 .srcdef.top.row1.e1 -side left -padx 3m \
	    -fill x -expand 1

    label .srcdef.top.row2.l1 -text "Data:"
    entry .srcdef.top.row2.e1 -relief sunken -textvariable dataFile \
	    -width 40 
    button .srcdef.top.row2.b1 -text "Select..." -width 10 -command {
	global datadir fsBox
	set fsBox(path) $datadir
	set fsBox(pattern) *
	set file [FSBox "Select data file"]
	if {$file != ""} { set dataFile $file }
    }
    pack .srcdef.top.row2.l1 .srcdef.top.row2.e1 .srcdef.top.row2.b1 \
	    -side left -padx 2m -fill x -expand 1

    label .srcdef.top.row3.l1 -text "Schema:"
    entry .srcdef.top.row3.e1 -relief sunken -textvariable schemaFile \
	    -width 40
    button .srcdef.top.row3.b1 -text "Select..." -width 10 -command {
	global schemadir fsBox
	set fsBox(path) $schemadir/physical
	set fsBox(pattern) *
	set file [FSBox "Select schema file" "newschema"]
	if {$file != ""} { set schemaFile $file }
    }
    button .srcdef.top.row3.new -text "New..." -width 10 \
	    -command {
		global is_unidata
		set is_unidata 1
	        set schemaFile [unidata_newschema]
	    }
    pack .srcdef.top.row3.l1 .srcdef.top.row3.e1 .srcdef.top.row3.b1 \
	    .srcdef.top.row3.new \
	    -side left -padx 2m -fill x -expand 1

    label .srcdef.top.row4.l1 -text "View:"
    entry .srcdef.top.row4.e1 -relief sunken -textvariable viewFile \
	    -width 40 
    button .srcdef.top.row4.b1 -text "Select..." -width 10 -command {
	if {$schemaFile == "" || $dataFile == ""} {
		dialog .noName "Missing Information" \
			"Please enter schema and data file names first." "" 0 OK
		return
	}
	set fsBox(path) $schemadir/query
	set fsBox(pattern) *
	set file [FSBox "Select view file" "newViewFile $schemaFile $dataFile"]
	if {$file != ""} { set viewFile $file }
    }
    pack .srcdef.top.row4.l1 .srcdef.top.row4.e1 .srcdef.top.row4.b1 \
	    -side left -padx 2m -fill x -expand 1

    button .srcdef.but.display -text Display -width 10 -command {
    		set displayImmediately 1
		defineTableOKAction
	}

	button .srcdef.but.ok -text OK -width 10 -command {
    		set displayImmediately 0
		defineTableOKAction
	}

	button .srcdef.but.cancel -text Cancel -width 10  -command {
			set retVal ""
			destroy .srcdef
	}

	pack .srcdef.but.display .srcdef.but.ok \
		.srcdef.but.cancel -in .srcdef.but.row1 -side left -padx 7m

	tkwait visibility .srcdef
	grab set .srcdef
	tkwait window .srcdef
	return $retVal
}

proc defineTableOKAction {} {

	global retVal tableName sourceType schemaFile dataFile viewFile

        # Don't allow names with dots or spaces in them -- they will cause
        # the DTE problems.
        if {[string match {*[. ]*} $tableName]} {
          dialog .winError "Name error" \
	    "Data source name cannot contain <dot> or <space>.  Please change data source name." \
	    "" 0 OK
          return
        }

	if {$tableName == "" || $schemaFile == "" || $dataFile == ""} {
		dialog .noName "Missing Information" \
			"Please enter all requested information." \
			"" 0 OK
	} else {
		set retVal "[addQuotes $tableName] $sourceType $schemaFile \
			$dataFile [addQuotes $viewFile]"
		destroy .srcdef
	}
}

##################################################################

proc defineDEViseOrDir {sourcetype label content} {

# these variable are used within the widgets of this procedure only

	global retVal tableName sourceType schemaFile url viewFile
	set sourceType $sourcetype

# cannot display remote site
	global displayImmediately
	set displayImmediately 0

    toplevel .srcdef
 	wm title .srcdef "$sourcetype Definition"
    wm geometry .srcdef +100+100
    selection clear .srcdef

    frame .srcdef.top -relief groove -borderwidth 2
    frame .srcdef.but
    pack .srcdef.top -side top -pady 3m -fill both -expand 1
    pack .srcdef.but -side top -pady 3m -fill x
    frame .srcdef.but.row1
    pack .srcdef.but.row1 -side top

     set tableName [lindex $content 0]
	set url [lindex $content 2]
	
    frame .srcdef.top.row1
    frame .srcdef.top.row2
    frame .srcdef.top.row3
    frame .srcdef.top.row4 -relief groove -borderwidth 2
    frame .srcdef.top.row5
    pack .srcdef.top.row2 .srcdef.top.row3 .srcdef.top.row1 \
	     .srcdef.top.row4 .srcdef.top.row5 -side top -pady 1m

    label .srcdef.top.row1.l1 -text "Table Name:"
    entry .srcdef.top.row1.e1 -relief sunken -textvariable tableName \
	    -width 40
    pack .srcdef.top.row1.l1 .srcdef.top.row1.e1 -side left -padx 3m \
	    -fill x -expand 1

    label .srcdef.top.row2.l1 -text $label
    entry .srcdef.top.row2.e1 -relief sunken -textvariable url \
	    -width 40 
    button .srcdef.top.row2.b1 -text "Select..." -width 10 -command {
     global dialogListVar
    	if {$sourceType == "Directory"} {
		global schemadir fsBox
		set fsBox(path) $schemadir
		set fsBox(pattern) *.dte
		set file [FSBox "Select $sourceType file" newDirectory]
		if {$file != ""} { set url $file }
	} else {
		set answer [dialogList .selectTData "Select DEVise Script"  \
		              "Select DEVise Script" \
				   "" 0 { OK New Cancel } \
				   {http://fontina.cs.wisc.edu/cgi-fontina/donko/script} ]
		if {$answer == 0} {
			set url $dialogListVar(selected)
		} elseif {$answer == 1} {
			set url [lineEntryBox "New DEVise address" "HTTP address"]
		} else {
			set url ""
		}
	}
	set tableName ""
    }

    pack .srcdef.top.row2.l1 .srcdef.top.row2.e1 .srcdef.top.row2.b1 \
	    -side left -padx 2m -fill x -expand 1

	button .srcdef.but.ok -text OK -width 10 -command {
		if {$tableName == "" || $url == "" } {
			dialog .noName "Missing Information" \
				"Please enter all requested information." "" 0 OK
		} else {
    # Don't allow names with dots or spaces in them -- they will cause
    # the DTE problems.
    if {[string match {*[. ]*} $tableName]} {
      dialog .winError "Name error" \
	"Data source name cannot contain <dot> or <space>.  Please change data source name." \
	"" 0 OK
      return
    }
			set retVal "[addQuotes $tableName] $sourceType $url"
			destroy .srcdef
		}
	}

	button .srcdef.but.cancel -text Cancel -width 10  -command {
			set retVal ""
			destroy .srcdef
	}

	pack .srcdef.but.ok \
		.srcdef.but.cancel -in .srcdef.but.row1 -side left -padx 7m

	grab set .srcdef
	tkwait window .srcdef
	return $retVal
}

proc newDirectory {} {

# these variable are used within the widgets of this procedure only

	global retVal
	global displayImmediately

#	cannot display a directoy

	set displayImmediately 0

	global schemadir
	set retVal ""
	set tableName [lineEntryBox "Create Directory" "File name"]
	if {$tableName != ""} {
		set fullPath "$schemadir/$tableName.dte"
		set f [open $fullPath w]
#		puts $f "1 catentry entry ;"
		close $f
		set retVal $fullPath
	}

	return $retVal
}

proc lineEntryBox {winTitle labelTitle} {

	global retVal

    toplevel .newdir
 	wm title .newdir $winTitle
    wm geometry .newdir +100+100
    selection clear .newdir

    frame .newdir.top -relief groove -borderwidth 2
    frame .newdir.but
    pack .newdir.top -side top -pady 3m -fill both -expand 1
    pack .newdir.but -side top -pady 3m -fill x

    frame .newdir.top.row1
    pack .newdir.top.row1 -side top -pady 1m

    label .newdir.top.row1.l1 -text $labelTitle
    entry .newdir.top.row1.e1 -relief sunken -textvariable tableName \
	    -width 40
    pack .newdir.top.row1.l1 .newdir.top.row1.e1 -side left -padx 3m \
	    -fill x -expand 1

	button .newdir.but.ok -text OK -width 10 -command {
		global retVal
		set retVal ""
		if {$tableName == ""} {
			dialog .noName "Missing Information" \
				"Please enter all requested information." "" 0 OK
		} else {
			set retVal $tableName
			destroy .newdir
		}
	}

	button .newdir.but.cancel -text Cancel -width 10  -command {
			global retVal
			set retVal ""
			destroy .newdir
	}

	pack .newdir.but.ok \
		.newdir.but.cancel -in .newdir.but -side left -expand 1 -padx 7m

	grab set .newdir
	tkwait window .newdir
	return $retVal
}

############################################################

proc scanSchema {schemafile} {
    global schemadir
    if {[catch { set f [open $schemafile "r"] }] > 0} {
	dialog .noFile "No Schema" \
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
	} elseif {[lindex $line 0] == "logical"} {
	    return [scanSchema [lindex $line 1]]
	}
    }

    close $f

#    puts "Schema file $schemafile has type $type"

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

#    puts "isCached dispname=$dispname, command=$command"
    
    if {$source == "DQL" || $source == "WWW" || $source == "BASICSTAT" \
	||$source=="HISTOGRAM"||$source=="GDATASTAT_X"||$source=="GDATASTAT_X_DATE" \
	|| $source=="HISTOGRAM_DATE"||$source =="GDATASTAT_Y" \
	||$source =="GDATASTAT_Y_DATE" ||$source =="GDATASTAT_X_DTE" } {
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
puts "schemafile = $schemafile"
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
#	puts "$p: $cachep($p)"
	foreach dispname $cachep($p) {
	    set cachefile [lindex $sourceList($dispname) 4]
	    set size [file size $cachefile]
#	    puts "$dispname, size $size, total $total"
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
#	puts "Removing cache file $cachefile"
	exec rm $cachefile
    }

    return 1
}

############################################################

proc getCacheName {source key} {
    global cachedir

    if {$source == "UNIXFILE" || $source == "WWW" || $source == "BASICSTAT" \
		|| $source == "HISTOGRAM" || || $source == "HISTOGRAM_DATE" \
		|| $source == "GDATASTAT_X" || $source == "GDATASTAT_X_DATE" \
		|| $source == "GDATASTAT_Y" || $source == "GDATASTAT_Y_DATE"} {
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
	|| $source == "WWW" || $source == "DQL" } {
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
    
    # Note: code was removed here that prevented the data file being
    # set if you didn't also select a schema.  RKW Nov. 26, 1997.

    set schemafile $file2
    # command is a text widget so we must insert the text
    # into the widget; setting the variable not enough
    .srcdef.top.row5.e1 delete 1.0 end
    .srcdef.top.row5.e1 insert end [file dirname $file]
    set file [file tail $file]

    return "$file $file"
}

############################################################

proc selectStream {{title "Select Table"}} {
    global streamsSelected sourceTypes MapTable

    if {[WindowVisible .srcsel]} {
	return
    }

	set errCode [catch {DEVise dteListCatalog [CWD]} listing]
     if {$errCode != 0} {
          dialog .noName "DTE Error" $listing "" 0 OK
          return
     }

    toplevel .srcsel
    wm title .srcsel $title
    wm geometry .srcsel +50+50
    selection clear .srcsel

    frame .srcsel.mbar -relief raised -borderwidth 2
    frame .srcsel.top
    frame .srcsel.bot

    listbox .srcsel.top.list -relief raised -borderwidth 2 \
	    -yscrollcommand ".srcsel.top.scroll set" -font 9x15 \
	    -selectmode extended -width 61 -height 22

# This part is taken from procedure updateSources
# That procedure is probably does not do anything usefull DD

	.srcsel.top.list delete 0 end
	if {[notRootDir]} {
		set sname ".."
		set source "Directory"
		set cached ""
		set item [format "%-40.40s  %-10.10s  %-6.6s" \
			$sname $source $cached]
		.srcsel.top.list insert end $item
	}
	foreach pair [lsort $listing] {
		set sname [lindex $pair 0]
		set source [lindex $pair 1]
		set cached ""

		set item [format "%-40.40s  %-10.10s  %-6.6s" \
			$sname $source $cached]
		.srcsel.top.list insert end $item
	}

# End of block from updateSources

    pack .srcsel.mbar -side top -fill x
    if {$title != ""} {
 	label .srcsel.title -text $title
 	pack .srcsel.title -side top -fill x -expand 1 -pady 3m
    }
    pack .srcsel.top -side top -fill both -expand 1
    pack .srcsel.bot -side top -fill x -pady 5m

    frame .srcsel.bot.but
    pack .srcsel.bot.but -side top

    tk_menuBar .srcsel.mbar .srcsel.mbar.define .srcsel.mbar.stream \
	    .srcsel.mbar.display .srcsel.mbar.follow .srcsel.mbar.help

    scrollbar .srcsel.top.scroll -command ".srcsel.top.list yview"

    pack .srcsel.top.list -side left -fill both -expand 1
    pack .srcsel.top.scroll -side right -fill y

    bind .srcsel.top.list <Double-Button-1> {

	set tmp [getSelectedLine]

 	puts "streamsSelected = $tmp"

	# check if the selected source is directory (need to change retVal)
	# if so, update CWD, and list sources

     if {[llength $tmp] != 1} {
		# this should never happen, but just in case
         dialog .note "Note" "Please select only one table." "" 0 OK
         return
     }

 	set table_type_pair [lindex $tmp 0]
	set tableName [lindex $table_type_pair 0]
	set typeName [lindex $table_type_pair 1]
 	puts "tableName = $tableName; typeName = $typeName"
	if { [isDirectory $typeName] } {
		if {$tableName == ".."} {
			CDup
		} else {
			CD $tableName
		}
 		updateSources
	} else {
#		puts "continuing with streamsSelected = $tableName"
		set streamsSelected [fullPathName $tableName]
	}
    }

#   updateSources

    button .srcsel.bot.but.select -text OK -width 10 -command {

	# This button is selecting any table (including directories) except ..

	set tmpStreamsSelected {}
	set tmp [getSelectedLine]

#	puts "streamsSelected = $tmp"

#    if {[llength $tmp] != 1} {
#        dialog .note "Note" "Please select only one table." "" 0 OK
#        return
#    }

	if {$tmp == ""} {
		set tmpStreamsSelected [CWD]
	} else {
		foreach table_type_pair $tmp {
			set tableName [lindex $table_type_pair 0]
			set typeName [lindex $table_type_pair 1]
#			puts "tableName = $tableName; typeName = $typeName"
			if {$tableName == ".."} {
				dialog .note "Note" \
					"Discarding selection of \"..\"." "" 0 OK
				return
			} else {
				lappend tmpStreamsSelected [fullPathName $tableName]
			}
		}
	}
	set streamsSelected $tmpStreamsSelected
    }

#   button .srcsel.bot.but.uncache -text Uncache -width 10 -command {
#	set dispnames [getSelectedSource]
#	foreach d $dispnames {
#	    uncacheData $d "Uncache requested."
#	    updateSources
#	}
#   }

    button .srcsel.bot.but.cancel -text Cancel -width 10 -command {
	set streamsSelected ""
    }

    pack .srcsel.bot.but.select \
 	    .srcsel.bot.but.cancel -side left -padx 3m

    set streamsSelected ""

    tkwait visibility .srcsel
    grab set .srcsel
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

# This procedure should be called when the select table window must be
# changed in place, for example when we change the directories.

    if {[catch {wm state .srcsel}] > 0} {
	return
    }
    .srcsel.top.list delete 0 end
    	
	# this call is useless
	set errCode [catch {DEVise dteListCatalog [CWD]} listing]

     if {$errCode != 0} {
          dialog .noName "DTE Error" $listing "" 0 OK
          return
     }
	if {[notRootDir]} {
		set sname ".."
		set source "Directory"
		set cached ""
		set item [format "%-40.40s  %-10.10s  %-6.6s" \
			$sname $source $cached]
		.srcsel.top.list insert end $item
	}
	foreach pair [lsort $listing] {
		set sname [lindex $pair 0]
		set source [lindex $pair 1]
		set cached ""

#	set cachefile [lindex $sourceList($sname) 4]
#	if {$source == "WWW"} {
#           set cached "Server"
#       } elseif {[isCached $sname -1 -1] != ""} {
#	    set cached "Cached"
#	}

	set item [format "%-40.40s  %-10.10s  %-6.6s" \
		$sname $source $cached]
	.srcsel.top.list insert end $item
    }
}

############################################################

proc scanDerivedSources {} {
    global derivedSourceList schemadir

    
#    catch { unset derivedSourceList }

    foreach view [ViewSet] {
	set sname "Stat: $view"
	set source "BASICSTAT"
	set key $view
	set schematype BASICSTAT
	set schemafile $schemadir/physical/BASICSTAT
        set cachefile ""
	set evaluation 100
	set priority 50
	set command "View:$view"

	set sourcedef [list $source $key $schematype $schemafile \
		$cachefile $evaluation $priority $command]
	set "derivedSourceList($sname)" $sourcedef

	set sname "Hist: $view"
	set source "HISTOGRAM"
	set schematype HISTOGRAM
	set schemafile $schemadir/physical/HISTOGRAM
	set command "Hist: $view"

	set sourcedef [list $source $key $schematype $schemafile \
		$cachefile $evaluation $priority $command]
	set "derivedSourceList($sname)" $sourcedef

	set sname "HistDate: $view"
	set source "HISTOGRAM_DATE"
	set schematype HISTOGRAM_DATE
	set schemafile $schemadir/physical/HISTOGRAM_DATE
	set command "HistDate: $view"

	set sourcedef [list $source $key $schematype $schemafile \
		$cachefile $evaluation $priority $command]
	set "derivedSourceList($sname)" $sourcedef

	set sname "GstatX: $view"
	set source "GDATASTAT_X"
	set schematype GDATASTAT
	set schemafile $schemadir/physical/GDATASTAT
	set command "GstatX: $view"
	set sourcedef [list $source $key $schematype $schemafile \
		$cachefile $evaluation $priority $command]
	set "derivedSourceList($sname)" $sourcedef

	set sname "GstatXDTE: $view"
	set source "GDATASTAT_X_DTE"
	set schematype GDATASTAT_X_DTE
	set schemafile $schemadir/physical/GDATASTAT_X_DTE
	set command "GstatXDTE: $view"
	set sourcedef [list $source $key $schematype $schemafile \
		$cachefile $evaluation $priority $command]
	set err [catch {set exists $derivedSourceList($sname)}]
	if { $err } {
	        set "derivedSourceList($sname)" $sourcedef
	}

	set sname "GstatXDate: $view"
	set source "GDATASTAT_X_DATE"
	set schematype GDATASTAT_X_DATE
	set schemafile $schemadir/physical/GDATASTAT_X_DATE
	set command "GstatXDate: $view"
	set sourcedef [list $source $key $schematype $schemafile \
		$cachefile $evaluation $priority $command]
	set "derivedSourceList($sname)" $sourcedef

	set sname "GstatY: $view"
	set source "GDATASTAT_Y"
	set schematype GDATASTAT
	set schemafile $schemadir/physical/GDATASTAT
	set command "GstatY: $view"
	set sourcedef [list $source $key $schematype $schemafile \
		$cachefile $evaluation $priority $command]
	set "derivedSourceList($sname)" $sourcedef

	set sname "GstatYDate: $view"
	set source "GDATASTAT_Y_DATE"
	set schematype GDATASTAT_Y_DATE
	set schemafile $schemadir/physical/GDATASTAT_Y_DATE
	set command "GstatYDate: $view"
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

proc getSelectedLine {} {

	# this proc is same as getSelectedSource except that it returns the
	# whole selected line

    set err [catch { set owner [selection own] }]
    if {$err > 0} { return "" }
    if {$owner != ".srcsel.top.list"} { return "" }
    set err [catch { set select [selection get] }]
    if {$err > 0} { return "" }

    set select [split $select \n]
    set retval ""
    foreach s $select {
	set dispName [lindex $s 0]
	set typeName [lindex $s 1]
	lappend retval [list $dispName $typeName]
    }
#   return [lindex $retval 0]
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

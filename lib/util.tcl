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
#  Revision 1.8  1996/01/23 20:51:42  jussi
#  Moved general utility functions from control.tk.
#
#  Revision 1.7  1996/01/17 21:02:16  jussi
#  Added appropriate suffixes to file names.
#
#  Revision 1.6  1996/01/17 20:53:45  jussi
#  Added support for additional image export formats.
#
#  Revision 1.5  1996/01/17 19:47:53  jussi
#  Minor bug fix.
#
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
#  Revision 1.1  1995/11/28  00:02:52  jussi
#  Initial revision.
#

############################################################

proc ExecuteScript {script} {
    puts "Executing script $script..."
    source $script
}

############################################################

proc UniqueName {name} {
    while { [DEVise exists $name] } {
	set len [string length $name]
	set lastChar [string index $name [expr $len - 1]]
	if { [scan $lastChar "%d" temp] == 1 } {
	    # numeric for the last character
	    set name [format "%s%d" [string range $name 0 [expr $len - 2]] \
		    [expr $lastChar + 1]]
	} else {
	    set name "$name 2"
	}
    }
    return $name
}

############################################################

# Return line width of string. New lines are accounted for.
# For example, "abc\ndefg" has a line width of 4
proc LineWidth { txt } {
    set strLen [string length $txt]
    set width 0
    set curWidth 0
    for { set i 0} { $i < $strLen } {set i [expr $i+1] } {
	if { [string compare [string index $txt $i] \n] == 0 } {
	    if {$curWidth > $width} {
		set width $curWidth
	    }
	    set curWidth 0
	} else {
	    set curWidth [expr $curWidth+1]
	}
    }
    if {$curWidth > $width} {
	set width $curWidth
    }
    return $width
}

############################################################

# Return # of lines in string 
# For example, "abc\ndefg" has 2 lines
proc LineHeight { txt } {
    set strLen [string length $txt]
    set height 0
    for { set i 0} { $i < $strLen } {set i [expr $i+1] } {
	if { [string compare [string index $txt $i] \n] == 0 } {
	    set height [expr $height+1]
	}
    }
    set height [expr $height+1]
    return $height
}

############################################################

# eliminate string attributes from schema
proc ElimStringAttr { attrs } {
    set newAttrs ""
    foreach attr $attrs {
	set attrType [lindex $attr 1]
	if {$attrType != "string"} {
	    lappend newAttrs $attr
	}
    }
    return $newAttrs
}

############################################################

# drop attribute types and sorted flags
proc DropAttrTypes { attrs } {
    set newAttrs ""
    foreach attr $attrs {
	lappend newAttrs [list [lindex $attr 0]]
    }
    return $newAttrs
}

############################################################

proc findNumericValue {list value} {
    foreach item $list {
	if {[lindex $item 1] == $value} {
	    return [lindex $item 0]
	}
    }
    return $value
}

############################################################

proc findNameValue {list key} {
    foreach item $list {
	if {[lindex $item 0] == $key} {
	    return [lindex $item 1]
	}
    }
    return $key
}

############################################################

# abstract data type for dictionary
# Insert name and val into dictionary. Return new dictionary.
proc DictInsert { dict name val }  {
    set element [list $name $val]
    return [lappend dict $element]
}

############################################################

# lookup val based on name
proc DictLookup { dict name } {
    foreach element $dict {
	if {$name == [lindex $element 0]} {
	    return [lindex $element 1]
	}
    }
    return ""
}

############################################################

# Get all instances for a category
proc CategoryInstances {category } {
    set inst {}
    set classes [ DEVise get $category ]
    foreach c $classes {
	set inst [ concat $inst [ DEVise get $category $c ] ]
    }
    return $inst
}

############################################################

# Get the class of an instance belonging to category
proc GetClass { category instance } {
    set classes [DEVise get $category ]
    foreach c $classes {
	set insts [DEVise get $category $c]
	foreach inst $insts {
	    # puts "comparing $category $c $inst"
	    if {$inst == $instance} {
		return $c
	    }
	}
    }
}

############################################################

# All instances of tdata that we have
# Form: {tdatafilebasename1 tdatafilebasename2 ...}
proc TdataSet {} {
    return [CategoryInstances "tdata" ]
}

############################################################

# All instances of GData
# Form: {gdataname1 gdataname2 ...}
proc GdataSet {} {
    return [ CategoryInstances "mapping" ]
}

############################################################

# All interpreted GData
proc InterpretedGData {} {
    set gSet [ GdataSet ]
    set result ""
    foreach gdata  $gSet {
	if { [DEVise isInterpretedGData $gdata ] } {
	    lappend result $gdata
	}
    }
    return $result
}

############################################################

# list of all views
proc ViewSet {} {
    return [ CategoryInstances "view" ]
}

############################################################

#list of all cursors
proc CursorSet {} {
    return [ CategoryInstances "cursor" ]
}

############################################################

# list of all links
proc LinkSet {} { 
    return [ CategoryInstances "link" ]
}

############################################################

# list of all windows
proc WinSet {} {
    return [ CategoryInstances "window" ]
}

############################################################

# list of all axis 
proc AxisSet {} {
    return [ CategoryInstances "axisLabel" ]
}

############################################################

# list of all actions
proc ActionSet {} {
    return [ CategoryInstances "action" ]
}

############################################################

# Return name of schema for a given tdata
proc GetSchemaName {name} {
    foreach schema [DEVise get tdata] {
	foreach tdata [DEVise get tdata $schema] {
	    set params [DEVise getCreateParam tdata $schema $tdata]
	    set fileAlias [lindex $params 1]
	    if {$fileAlias == $name} {
		return $schema
	    }
	}
    }

    puts "Error: Schema of $tdata not found."
    exit 1
}

############################################################

# Print the name of all instances in category
proc PrintCategory { category } {
    set classes [ DEVise get $category ]
    puts "classes $classes"
    foreach c  $classes {
	set instances [ DEVise get $category $c]
	foreach i $instances {
	    puts -nonewline "$i "
	}
    }
    puts ""
}

############################################################

proc DoExit {} {
    set answer [ dialog .quit "Quit" \
	    "Are you sure you want to quit?" "" 1 \
	    { Cancel} { Ok } ]
    if { $answer == 1 } {
	DEVise exit
    }
}

############################################################

proc PrintView {} {
    global toprinter printcmd filename allviews formatsel

    # see if .printdef window already exists; if so, just return
    set err [catch {wm state .printdef}]
    if {!$err} { wm deiconify .printdef; return }

    toplevel .printdef
    wm title .printdef "Print View"
    wm geometry .printdef +150+150
    selection clear .printdef
    
    set toprinter 1
    set printcmd "lpr "
    set filename "/tmp/devise"
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
	    -variable toprinter -value 1 -anchor e
    label .printdef.top.row1.l1 -text "Print Command:" -width 14
    entry .printdef.top.row1.e1 -relief sunken -textvariable printcmd \
	    -width 30
    pack .printdef.top.row1.r1 .printdef.top.row1.l1 .printdef.top.row1.e1 \
	    -side left -padx 2m -fill x -expand 1

    radiobutton .printdef.top.row2.r1 -text "To File" -width 12 \
	    -variable toprinter -value 0 -anchor e
    menubutton .printdef.top.row2.m1 -relief raised \
	    -textvariable formatsel -menu .printdef.top.row2.m1.menu \
	    -width 10
    label .printdef.top.row2.l1 -text "File Name:" -width 14
    entry .printdef.top.row2.e1 -relief sunken -textvariable filename \
	    -width 30
    pack .printdef.top.row2.r1 .printdef.top.row2.m1 \
	    .printdef.top.row2.l1 .printdef.top.row2.e1 \
	    -side left -padx 2m -fill x -expand 1

    menu .printdef.top.row2.m1.menu -tearoff 0
    .printdef.top.row2.m1.menu add radiobutton -label Postscript \
	    -variable formatsel -value Postscript
    .printdef.top.row2.m1.menu add radiobutton -label EPS \
	    -variable formatsel -value EPS
    .printdef.top.row2.m1.menu add radiobutton -label GIF \
	    -variable formatsel -value GIF
    set formatsel Postscript

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
	    -command {
	PrintActual $toprinter $printcmd $filename $allviews $formatsel; \
	destroy .printdef
    }
    button .printdef.bot.but.cancel -text Cancel -width 10 \
	    -command { destroy .printdef }
    pack .printdef.bot.but.ok .printdef.bot.but.cancel -side left -padx 7m

    tkwait visibility .printdef
}

############################################################

proc PrintActual {toprinter printcmd filename allviews format} {
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

    set format [string tolower $format]

    set suffix ".ps"
    if {$format != "postscript"} {
	set suffix ".$format"
    }

    if {$toprinter} {
	set format postscript
	set template [format "/tmp/devise.%s.%%d$suffix" [pid]]
    } else {
	set template [format "%s.%%d$suffix" $filename]
    }

    set i 0
    foreach win $windowlist {
	set file [format $template $i]
	puts "Save window $win to file $file"
	set err [ DEVise saveWindowImage $format $win $file ]
	if {$err > 0} {
	    dialog .printError "Window Image Save Error" \
		    "An error occurred while saving window images to files." \
		    "" 0 OK
	    return
	}
	if {$toprinter} {
	    puts "File $file not actually printed. To print file, do:"
	    puts "  $printcmd $file"
	    puts "  rm $file"
	}
	incr i
    }
}

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
#  Revision 1.17  1996/05/09 18:15:19  kmurli
#  No changes, just inserted some debugging print statements and removed them
#
#  Revision 1.16  1996/04/23 20:36:24  jussi
#  Corrected name of abort window.
#
#  Revision 1.15  1996/04/16 19:29:29  jussi
#  Added AbortProgram function.
#
#  Revision 1.14  1996/04/10 02:58:31  jussi
#  Added WindowVisible and made getColor and PrintView use it.
#
#  Revision 1.13  1996/03/26 21:27:08  jussi
#  Commented out extraneous debugging statement.
#
#  Revision 1.12  1996/03/26 21:16:40  jussi
#  Added ScaleUpper and ScaleLower.
#
#  Revision 1.11  1996/03/06 00:00:02  jussi
#  Index number of color displayed in colored button in getColor.
#
#  Revision 1.10  1996/02/02 21:24:46  jussi
#  Added getColor.
#
#  Revision 1.9  1996/01/26 23:22:46  jussi
#  Added ExecuteScript procedure.
#
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
    foreach gdata $gSet {
	set isInterpreted [DEVise isInterpretedGData $gdata]
	if {$isInterpreted} {
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

# list of all cursors

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
	destroy .
    }
}

############################################################

proc getColor {varname} {
    global $varname DEViseColors

    if {[WindowVisible .getColor]} {
	return
    }

    toplevel .getColor
    wm title .getColor "Choose Color"
    wm geometry .getColor +100+100

    frame .getColor.top -relief groove -borderwidth 2
    frame .getColor.bot
    pack .getColor.top -side top -fill both -expand 1
    pack .getColor.bot -side top -pady 5m -fill x
    frame .getColor.bot.but
    pack .getColor.bot.but -side top

    set row -1
    set col -1
    set maxcol 8
    set size 5

    foreach val $DEViseColors {
	set cindex [lindex $val 0]
	set color [lindex $val 1]
	if {$col < 0} {
	    incr row
	    set col 0
	    frame .getColor.top.row$row
	    pack .getColor.top.row$row -side top -fill x -expand 1
	}
	button .getColor.top.row$row.col$col -background $color \
		-activebackground $color -width $size -text $cindex \
		-command "set $varname $color; destroy .getColor"
	pack .getColor.top.row$row.col$col -side left -fill x -expand 1
	incr col
	if {$col >= $maxcol} {
	    set col -1
	}
    }
    
    # Add empty frames at end of last row
    for {} {$row >= 0 && $col >= 0 && $col < $maxcol} {incr col} {
	button .getColor.top.row$row.col$col -relief flat -width $size \
		-state disabled
	pack .getColor.top.row$row.col$col -side left -fill x -expand 1
    }

    button .getColor.bot.but.cancel -text Cancel -width 10 \
	    -command "destroy .getColor"
    pack .getColor.bot.but.cancel -side left

    tkwait visibility .getColor
    grab set .getColor
    tkwait window .getColor
}

############################################################

proc PrintView {} {
    global toprinter printcmd filename allviews formatsel

    if {[WindowVisible .printdef]} {
	return
    }

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

proc ScaleUpper {val} {
    set scale {0 0.001 0.01 0.1 1 2 5 10 20 30 40 50 100 200 300 400 500
               1000 2000 3000 4000 5000 10000 50000 100000}
    foreach s $scale {
	if {$val < $s} {
	    # puts "$val, upper $s"
	    return $s
	}
    }
    return $val
}

proc ScaleLower {val} {
    set scale {0 0.001 0.01 0.1 1 2 5 10 20 30 40 50 100 200 300 400 500
               1000 2000 3000 4000 5000 10000 50000 100000}
    set prevs $val
    foreach s $scale {
	if {$val < $s} {
	    # puts "$val, lower $prevs"
	    return $prevs
	}
	set prevs $s
    }
    return $prevs
}

proc WindowVisible {w} {
    # see if $w already exists
    set err [catch {set state [wm state $w]}]
	
    if {$err} {
	return 0
    }

    if {$state == "iconic"} {
	wm deiconify $w
    } else {
	wm iconify $w
    }

    return 1
}

############################################################

proc AbortProgram {reason} {
    if {[WindowVisible .abort]} {
	return
    }

    toplevel .abort
    wm title .abort "Internal Error"
    wm geometry .abort +100+100
    message .abort.msg -justify center -width 10c \
	    -text "An internal error has occurred. The error message is\
	           printed in the text window.\n\n\
		   If the error is reproducible, please send the error\
		   message along with a description of how the error\
		   can be produced to jussi@cs.wisc.edu.\n\n\
		   The program will now exit."
    button .abort.exit -text Exit -width 10 -command "destroy .abort"
    pack .abort.msg .abort.exit -side top -padx 5m -pady 3m

    tkwait visibility .abort
    grab set .abort
    tkwait window .abort
}

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
#  Revision 1.62  1998/11/11 14:31:15  wenger
#  Implemented "highlight views" for record links and set links; improved
#  ClassDir::DestroyAllInstances() by having it destroy all links before
#  it destroys anything else -- this cuts down on propagation problems as
#  views are destroyed; added test code for timing a view's query and draw.
#
#  Revision 1.61  1998/11/02 19:22:53  wenger
#  Added "range/MQL" session description capability.
#
#  Revision 1.60  1998/10/20 19:46:33  wenger
#  Mapping dialog now displays the view's TData name; "Next in Pile" button
#  in mapping dialog allows user to edit the mappings of all views in a pile
#  without actually flipping them; user has the option to show all view names;
#  new GUI to display info about all links and cursors; added API and GUI for
#  count mappings.
#
#  Revision 1.59  1998/08/18 15:22:31  wenger
#  Found and fixed bug 384 (devisec not quitting).
#
#  Revision 1.58  1998/06/12 19:55:43  wenger
#  Attribute of TAttr/set links can now be changed; GUI has menu of available
#  attributes; attribute is set when master view is set instead of at link
#  creation; misc. debug code added.
#
#  Revision 1.57  1998/04/10 18:29:50  wenger
#  TData attribute links (aka set links) mostly implemented through table
#  insertion; a crude GUI for creating them is implemented; fixed some
#  bugs in link GUI; changed order in session file for TData attribute links.
#
#  Revision 1.56  1998/03/27 15:09:18  wenger
#  Added dumping of logical session description, added GUI for dumping
#  logical or physical description; cleaned up some of the command code
#  a little.
#
#  Revision 1.55  1998/02/26 20:50:40  taodb
#  Removed redundancy between groupcontrol.tk and control.tk
#
#  Revision 1.54  1998/02/12 17:17:35  wenger
#  Merged through collab_br_2; updated version number to 1.5.1.
#
#  Revision 1.53  1998/02/02 18:26:35  wenger
#  Strings file can now be loaded manually; name of strings file is now
#  stored in session file; added 'serverExit' command and kill_devised
#  script to cleanly kill devised; fixed bug 249; more info is now
#  printed for unrecognized commands.
#
#  Revision 1.52.2.2  1998/02/12 05:10:16  taodb
#  Seperated GUI interface for group and normal version of DEVise
#
#  Revision 1.52.2.1  1998/02/04 00:23:42  taodb
#  Make "R" blinking after a request is received
#
#  Revision 1.52  1997/11/24 23:15:48  weaver
#  Changes for the new ColorManager.
#
#  Revision 1.51  1997/11/12 15:47:48  wenger
#  Merged the cleanup_1_4_7_br branch through the cleanup_1_4_7_br_2 tag
#  into the trunk.
#
#  Revision 1.50.6.1  1997/11/10 23:37:14  wenger
#  Fixed bug 239.
#
#  Revision 1.50  1997/07/22 15:36:48  wenger
#  Added capability to dump human-readable information about all links
#  and cursors.
#
#  Revision 1.49  1997/07/17 18:44:17  wenger
#  Added menu selections to report number of strings and save string space.
#
#  Revision 1.48  1997/06/25 17:05:17  wenger
#  Fixed bug 192 (fixed problem in the PSWindowRep::FillPixelRect() member
#  function, disabled updating of record links during print, print dialog
#  grabs input.
#
#  Revision 1.47  1997/04/29 14:34:52  wenger
#  User interface improvments: Quit and Close don't ask for confirmation
#  if there is no session open; the file selection box gives better info
#  as to what you are doing; Save and Save as aren't allowed if there is
#  no session open; changed default print filename from
#  /u/g/u/guangshu/public/html/pictures/map/ back to /tmp/devise again.
#
#  Revision 1.46  1997/04/21 23:11:02  guangshu
#  Improved code of UniqueName.
#
#  Revision 1.45  1997/03/25 17:59:42  wenger
#  Merged rel_1_3_3c through rel_1_3_4b changes into the main trunk.
#
#  Revision 1.44  1997/03/20 01:32:19  wenger
#  Fixed a bug in color allocation; color chooser for data shows old colors
#  (temporarily); background conversion of GData defaults to off.
#
#  Revision 1.43  1997/02/26 16:32:06  wenger
#  Merged rel_1_3_1 through rel_1_3_3c changes; compiled on Intel/Solaris.
#
#  Revision 1.42  1997/02/14 16:48:09  wenger
#  Merged 1.3 branch thru rel_1_3_1 tag back into the main CVS trunk.
#
#  Revision 1.41  1997/02/11 00:48:06  ssl
#  Fixed minor bug with Record Link list
#
#  Revision 1.40  1997/02/03 20:02:04  ssl
#  Added interface for negative record links and user defined layout mode
#
#  Revision 1.39.4.5  1997/03/15 00:31:42  wenger
#  PostScript printing of entire DEVise display now works; PostScript output
#  is now centered on page; other cleanups of the PostScript printing along
#  the way.
#
#  Revision 1.39.4.4  1997/02/19 23:01:27  wenger
#  A few minor changes in the shape menu stuff to make the GUI more consistent
#  with the rest of Devise; added some more info to the shape help;
#  shape menu doesn't disable shape attributes (extra ones sometimes needed
#  for 3D, etc.); fixed a bug in how Cancel works in the Color Chooser;
#  various bug fixes in shape attribute menus.
#
#  Revision 1.39.4.3  1997/02/13 18:17:07  ssl
#  Added check to UI when user links two different data sources with a record link
#
#  Revision 1.39.4.2  1997/02/11 01:17:41  ssl
#  Cleaned up the UI for piled views
#  1) Made pile links invisible to user
#  2) Added create/destroy link options to the link menu
#  3) Enhanced the link info window to show all info about a link (type, views,
#     master, link params )
#  4) Pile links get removed when the pile is unpiled
#  5) Set/Reset Master now only shows list of record links
#
#  Revision 1.39.4.1  1997/02/07 15:21:47  wenger
#  Updated Devise version to 1.3.1; fixed bug 148 (GUI now forces unique
#  window names); added axis toggling and color selections to Window menu;
#  other minor fixes to GUI; show command to Tasvir now requests image to
#  be shown all at once.
#
#  Revision 1.39  1997/01/30 02:12:02  beyer
#  added stringCaseCmp for case insensitive string comparisons and
#  lsortCase for case insensitive list sorting.
#
#  Revision 1.38  1997/01/23 00:07:14  jussi
#  Added WindowExists procedure.
#
#  Revision 1.37  1997/01/17 20:32:31  wenger
#  Fixed bugs 088, 121, 122; put workaround in place for bug 123; added
#  simulation of XOR drawing in PSWindowRep; removed diagnostic output
#  from Tcl/Tk code; removed (at least for now) the ETk interface from
#  the cslib versions of WindowRep classes so that the cslib will link
#  okay; cslib server now tests XOR drawing.
#
#  Revision 1.36  1997/01/13 18:08:16  wenger
#  Fixed bugs 043, 083, 084, 091, 114.
#
#  Revision 1.35  1996/12/23 22:20:42  donjerko
#    Commented out a bunch of non-working Tk/Tcl code.  Changed OK/Cancel
#    pairs to have OK always on the left, Cancel on the right.  Renamed
#    and moved around various menu items; to avoid confusion.
#
#    More changes will follow later, after you're had time to adjust.
#
#    Shaun/Donko
#
#  Revision 1.34  1996/12/17 05:23:40  beyer
#  Added missing findNumericCaseValue.
#
#  Revision 1.33  1996/11/26 09:45:55  beyer
#  WindowVisible always puts a window on top, rather than iconify
#
#  Revision 1.32  1996/10/07 22:54:08  wenger
#  Added more error checking and better error messages in response to
#  some of the problems uncovered by CS 737 students.
#
#  Revision 1.31  1996/09/18 20:08:22  guangshu
#  Added option when save gif files. It can save every view inside a window.
#  Before it can only save the whole window.
#
#  Revision 1.30  1996/09/17 19:31:23  wenger
#  Changed default print filename from /u/g/u/guangshu/public/html/pictures/map/
#  back to /tmp/devise.
#
#  Revision 1.29  1996/09/13 23:09:33  guangshu
#  Added support to save maps when saving the display.
#
#  Revision 1.28  1996/09/06 14:23:30  jussi
#  Added missing parameter to dialog procedure call.
#
#  Revision 1.27  1996/08/29 22:31:28  guangshu
#  Added option for client to get the gif files from the server and changed
#  puts to proc Puts.
#
#  Revision 1.26  1996/07/18 02:20:31  jussi
#  Added Print Display option.
#
#  Revision 1.25  1996/07/13 17:31:17  jussi
#  Moved statusWindow procedure from macrodef.tk to util.tcl.
#
#  Revision 1.24  1996/07/05 14:41:33  jussi
#  Added error message to GetClass procedure.
#
#  Revision 1.23  1996/06/27 22:54:09  jussi
#  Added support for XOR color value.
#
#  Revision 1.22  1996/05/15 16:37:05  jussi
#  Removed ExecuteScript procedure.
#
#  Revision 1.21  1996/05/14 15:08:59  jussi
#  Removed extract puts statement.
#
#  Revision 1.20  1996/05/14 15:06:09  jussi
#  Added catch statement to DEVise saveWindowImage call.
#
#  Revision 1.19  1996/05/11 17:24:57  jussi
#  Added warning output to DictLookup.
#
#  Revision 1.18  1996/05/11 03:00:22  jussi
#  Small changes.
#
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

############################################################

proc UniqueName {name} {
    set trimedleft 0
    set trimedright 0
    set len [string length $name]
    set lastOne [string index $name [expr $len - 1]]
    set firstOne [string index $name 0]
    if { $lastOne == "\}" } {
    	set name [string trimright $name "\}"]
	puts "name in UniqueName $name"
	set trimedleft 1
    }
    if { $firstOne == "\{" } {
    	set name [string trimleft $name "\{"]
	puts "name in UniqueName $name"
	set trimedright 1
    }
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
    if { $trimedright == 1 } {
        set name "$name\}"
    } 
    if { $trimedleft == 1 } {
        set name "\{$name"
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

# case insensitive version of FindNumericValue
proc findNumericCaseValue {list value} {
    foreach item $list {
        if {[string tolower [lindex $item 1]] == [string tolower $value]} {
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
    puts "Warning -- could not find $name in $dict"
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
    set classes [DEVise get $category]
    foreach c $classes {
	set insts [DEVise get $category $c]
	foreach inst $insts {
	    # puts "comparing $category $c $inst"
	    if {$inst == $instance} {
		return $c
	    }
	}
    }
    puts "Warning -- could not find class of $instance"
    return ""
}

############################################################

# All instances of tdata that we have
# Form: {tdatafilebasename1 tdatafilebasename2 ...}

proc TdataSet {} {
    return [CategoryInstances "tdata"]
}

############################################################

# All instances of GData
# Form: {gdataname1 gdataname2 ...}

proc GdataSet {} {
    return [CategoryInstances "mapping"]
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

# list of all lenses 

proc ViewLensSet {} {
    return [ DEVise get "view" "ViewLens" ]
}
############################################################

# list of all views other than lenses 

proc ViewExceptLensSet {} {
    set tmp [concat [ DEVise get "view" "Scatter" ] \
		[ DEVise get "view" "SortedX"] ]
    puts "ViewExceptLensSet = $tmp"
    return $tmp
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
#list of all record and TData attribute links
proc RecordLinkSet {} {
    set linkSet [ CategoryInstances "link"]
    set recLinkSet {}
    foreach link $linkSet {
        set flag [DEVise getLinkFlag $link]
        if { [expr $flag & 128] || [expr $flag & 1024] } {
            lappend recLinkSet $link
        }
    }
    return $recLinkSet
}


############################################################
#list of all TData attribute links
proc TAttrLinkSet {} {
    set linkSet [ CategoryInstances "link"]
    set tAttrLinkSet {}
    foreach link $linkSet {
        set flag [DEVise getLinkFlag $link]
        if { [expr $flag & 1024] } {
            lappend tAttrLinkSet $link
        }
    }
    return $tAttrLinkSet
}

############################################################

#list of non pile links
proc NonPileLinkSet {} {
    set nonPileLinks {}
    foreach link [LinkSet] {
	if {[string range $link 0 4] != "Pile:"} {
	    lappend nonPileLinks $link
	}
    }
    return $nonPileLinks
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
    foreach c  $classes {
	set instances [ DEVise get $category $c]
	foreach i $instances {
	    puts -nonewline "$i "
	}
    }
    puts ""
}

############################################################

proc DoExit { mode } {
	if { $mode == "csgroup" } {
		DoGroupExit
		return
	}
    if {[SessionIsOpen]} {
      set answer [ dialog .quit "Quit" \
	      "Are you sure you want to quit?" "" 0 \
	      { OK } { Cancel} ]
    } else {
      set answer 0
    }
    if { $answer == 0 } {
		DEVise exit
		destroy .
    }
}

############################################################
proc DoGroupExit {} {
    if {[SessionIsOpen]} {
      set answer [ dialog .quit "Quit" \
	      "Are you sure you want to quit?" "" 0 \
	      { OK } { Cancel} ]
    } else {
      set answer 0
    }
    if { $answer == 0 } {
		DEVise GroupCmd ExitGroupMode
		DEVise exit
		destroy .
    }
}

############################################################

proc PrintViewSetUp {} {
    global toprinter printcmd filename printsrc formatsel 

    toplevel .printdef
    wm title .printdef "Print View"
    wm geometry .printdef +150+150
    selection clear .printdef
    
    set toprinter 1
    set printcmd "lpr "
    set filename "/tmp/devise"
    set printsrc 0 

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
    radiobutton .printdef.top.row3.r1 -text "Display" \
	    -variable printsrc -value 2
    radiobutton .printdef.top.row3.r2 -text "All Windows" \
	    -variable printsrc -value 1
    radiobutton .printdef.top.row3.r3 -text "Selected Window" \
	    -variable printsrc -value 0
    radiobutton .printdef.top.row3.r4 -text "All Views" \
	    -variable printsrc -value 3
    pack .printdef.top.row3.l1 .printdef.top.row3.r1 .printdef.top.row3.r4 \
	    .printdef.top.row3.r2 .printdef.top.row3.r3 -side left -padx 2m \
	    -fill x -expand 1
}

############################################################################
proc PrintView {} {
    global toprinter printcmd filename  printsrc formatsel mapfile url defaultUrl
    if {[WindowVisible .printdef]} {
        return
    }

    PrintViewSetUp

    frame .printdef.bot.but
    pack .printdef.bot.but -side top

    button .printdef.bot.but.ok -text OK -width 10 \
	    -command {
	PrintActual $toprinter $printcmd $filename $printsrc $formatsel 0 0 0 0; \
	destroy .printdef
        }
    button .printdef.bot.but.cancel -text Cancel -width 10 \
	    -command { destroy .printdef }
    pack .printdef.bot.but.ok .printdef.bot.but.cancel -side left -padx 7m

    tkwait visibility .printdef
    # Grab here so user can't do something like resize windows during the
    # printing process.
    grab set .printdef
}

##########################################################################

proc PrintWithMap {} {
     global toprinter printcmd filename printsrc formatsel mapfile url defaultUrl
     if {[WindowVisible .printdef]} {
        return
     }
     set mapfile ""
     set url ""
     set defaultUrl ""

     PrintViewSetUp
     
     frame .printdef.bot.ent
     pack .printdef.bot.ent -side top

     label .printdef.bot.ent.l1 -text "Map File" -width 10 
     entry .printdef.bot.ent.t1 -relief sunken -textvariable mapfile \
		-width 20
     label .printdef.bot.ent.l2 -text "URL:" -width 10
     entry .printdef.bot.ent.t2 -relief sunken -textvariable url \
		-width 20
     label .printdef.bot.ent.l3 -text " Default URL:" -width 10
     entry .printdef.bot.ent.t3 -relief sunken -textvariable defaultUrl \
		-width 20
     pack .printdef.bot.ent.l1 .printdef.bot.ent.t1 \
		.printdef.bot.ent.l2 .printdef.bot.ent.t2 \
		.printdef.bot.ent.l3 .printdef.bot.ent.t3 \
		-side left -fill x -expand 1

     frame .printdef.bot.but
     pack .printdef.bot.but -side top

     button .printdef.bot.but.ok -text OK -width 10 \
		-command {
		PrintActual $toprinter $printcmd $filename $printsrc \
			      $formatsel 1 $mapfile $url $defaultUrl; \
		destroy .printdef
               }
     button .printdef.bot.but.cancel -text Cancel -width 10 \
		-command { destroy .printdef }
     pack .printdef.bot.but.ok .printdef.bot.but.cancel -side left -padx 7m

     tkwait visibility .printdef
}

##########################################################################

proc PrintActual {toprinter printcmd filename printsrc fmt map mapfile \
			url defaultUrl} {
    global curView

    set format [string tolower $fmt]

    set suffix ".ps"
    if {$format != "postscript"} {
	set suffix ".$format"
    }

    if {$toprinter} {
	set format postscript
	set template [format "/tmp/devise.%s.%%d.ps" [pid]]
    } else {
	set template "$filename.%d$suffix"
    }

    if {$printsrc == 2} {
	# Printing entire display...
	set file "$filename$suffix"
	
	if {$map == 1} {
	   puts "Saving entire display to file $file"
	   set err [ catch { DEVise saveDisplayImageAndMap $format $file \
				 $mapfile $url $defaultUrl } ]
	   if {$err > 0} {
		dialog .printError "Display Image Save Error" \
			"An error occurred while saving display image to file." \
			"" 0 OK
	        return
	   }
	   return
	}
	puts "Saving entire display to file $file"
	set err [ catch { DEVise saveDisplayImage $format $file } ]
	if {$err > 0} {
	    dialog .printError "Display Image Save Error" \
		    "An error occurred while saving display image to file." \
		    "" 0 OK
	    return
	}
	return
    } elseif {$printsrc == 3} {
	# Printing all views...
	set err [catch {DEVise saveDisplayView $format $filename} ]
	if {$err > 0} {
	     dialog .printError "View Image Save Error" \
		     "An error occurred while saving display image to file." \
		     "" 0 OK
	     return
	}
	return
    }


    set windowlist ""

    if {!$printsrc} {
	# Printing selected window...
	if {$curView == ""} {
	    dialog .printView "Print Selected Window" \
		    "Please select a window by clicking in it first." "" 0 OK
	    return
	}
	set windowlist [list [DEVise getViewWin $curView]]

    } else {
	# Printing all windows...
	set viewClasses [ DEVise get view ]
	foreach viewClass $viewClasses {
	    set views [ DEVise get view $viewClass ]
	    foreach v $views {
		set win [DEVise getViewWin $v]
		if {$win != "" && [lsearch $windowlist $win] < 0} {
		    # Check whether this window should be excluded from
		    # printing.
		    set printConfig [DEVise winGetPrint $win]
		    if {![lindex $printConfig 0]} {
		        lappend windowlist $win
		    }
		}
	    }
	}
    }

    set i 0
    foreach win $windowlist {
	set file [format $template $i]
	if {$filename == "stderr" || $filename == "stdout"} {
	     set err [ catch { SetGetImage $format $win $filename } ]
	     if {$err > 0} {
		 dialog .printError "Window Image transfer Error" \
			 "An error occurred while saving window images to files." \
			 "" 0 OK
		return
	     }
	} else {
	     Puts "Saving window $win to file $file"
	     set err [ catch { DEVise saveWindowImage $format $win $file } ]
	     if {$err > 0} {
	         dialog .printError "Window Image Save Error" \
		         "An error occurred while saving window images to files." \
		         "" 0 OK
	         return
	     }
	     if {$toprinter} {
	         Puts "Printing file $file to printer"
                 set pcmd [ lindex $printcmd 0 ]
                 set parg [ lrange $printcmd 1 end ]
	         exec $pcmd $parg $file
		 exec rm $file
	     }
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
	wm withdraw $w
	wm deiconify $w
    }

    return 1
}

proc WindowExists {w} {
    # see if $w already exists
    set err [catch {set state [wm state $w]}]
	
    if {$err} {
	return 0
    }

    return 1
}


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
		   can be produced to devise-sup@cs.wisc.edu.\n\n\
		   The program will now exit."
    button .abort.exit -text Exit -width 10 -command "destroy .abort"
    pack .abort.msg .abort.exit -side top -padx 5m -pady 3m

    tkwait visibility .abort
    grab set .abort
    tkwait window .abort
}

############################################################

proc statusWindow {w title msg} {
    toplevel $w
    wm title $w $title
    wm geometry $w +200+200
    message $w.msg -justify center -width 8c -text $msg
    pack $w.msg -side top -padx 8m -pady 4m
    update
}

############################################################

proc stringCaseCmp {s1 s2} {
    return [string compare [string tolower $s1] [string tolower $s2]];
}

############################################################

proc lsortCase {l} {
    return [lsort -command stringCaseCmp $l];
}

############################################################

# Returns 1 if a session if open, 0 otherwise.
# Note: there may be some small possibility that DEVise could have
# some session objects and this procedure could still return 0.
# However, I think that chance is pretty small.  RKW 4/29/97.

proc SessionIsOpen {} {

    # Check whether we have any windows other than the main window.
    if {[DEVise getWinCount] > 1} {
      return 1
    } else {
      # Check whether we have any TDatas.
      set tDataCount 0
      foreach class [DEVise get tdata] {
        incr tDataCount [llength [DEVise get tdata $class]]
      }

      if {$tDataCount > 0} {
	return 1
      } else {
	return 0
      }
    }
}

############################################################
# Get and display the number of strings in the DEVise  string space.

proc GetStringCount {} {
    set stringCount [DEVise getStringCount]
    dialog .stringInfo "String Count" \
      "String table has $stringCount strings" "" 0 OK
}

############################################################
# Load the DEVise string space from a file.

proc LoadStringSpace {} {
  global fsBox sessiondir

  set fsBox(path) $sessiondir
  set fsBox(pattern) *.str
  set filename [ FSBox "Select file for loading strings" ]

  if {$filename != ""} {
    DEVise loadStringSpace $filename
  }
}

############################################################
# Save the DEVise string space to a file.

proc SaveStringSpace {} {
  global fsBox sessiondir

  set fsBox(path) $sessiondir
  set fsBox(pattern) *.str
  set filename [ FSBox "Select file for saving strings" ]

  if {$filename != ""} {
    DEVise saveStringSpace $filename
  }
}

############################################################
# Dump info about links and cursors to a file.

proc DumpLinksCursors {} {
  global fsBox

  set fsBox(path) [CWD]
  set fsBox(pattern) *
  set filename [ FSBox "Select file for link/cursor info" ]

  if {$filename != ""} {
    DEVise dumpLinkCursor $filename
  }
}

############################################################
# Save (physical or logical) session description to a file.

proc SaveSessionDesc {physical} {
  global fsBox

  set fsBox(path) [CWD]
  set fsBox(pattern) *
  set filename [ FSBox "Select file for session description" ]

  if {$filename != ""} {
    DEVise writeDesc $filename $physical
  }
}

############################################################
# Save range/MQL session description to a file.

proc SaveRangeDesc {} {
  global fsBox

  set fsBox(path) [CWD]
  set fsBox(pattern) *
  set filename [ FSBox "Select file for session description" ]

  if {$filename != ""} {
    DEVise writeRangeDesc $filename
  }
}



############################################################
# Show info about links and cursors.

proc ShowLinkCursorInfo {} {

  # If this window already exists, raise it to the top and return.
  if {[WindowVisible .lcInfo]} {
    return
  }

  # Create the top level widget and the frames we'll later use for
  # positioning.
  toplevel .lcInfo
  wm title .lcInfo "Link/Cursor Info"

  frame .lcInfo.row1
  frame .lcInfo.row2
  frame .lcInfo.row3
  frame .lcInfo.listframe -relief raised -borderwidth 2

  # Create the various widgets.
  set listWidth 100
  button .lcInfo.ok -text "OK" -width 10 \
    -command {destroy .lcInfo}

  set listHeight 10

  set listWidth 12
  listbox .lcInfo.lchead -relief flat -width $listWidth -height 1
  .lcInfo.lchead insert end "Link/cursor"
  listbox .lcInfo.lclist -relief flat -width $listWidth -height $listHeight \
    -yscrollcommand {.lcInfo.scroll set}

  set listWidth 20
  listbox .lcInfo.namehead -relief flat -width $listWidth -height 1
  .lcInfo.namehead insert end "Name"
  listbox .lcInfo.namelist -relief flat -width $listWidth \
    -height $listHeight -yscrollcommand {.lcInfo.scroll set}

  set listWidth 15
  listbox .lcInfo.typehead -relief flat -width $listWidth -height 1
  .lcInfo.typehead insert end "Type"
  listbox .lcInfo.typelist -relief flat -width $listWidth \
    -height $listHeight -yscrollcommand {.lcInfo.scroll set}

  set listWidth 20
  listbox .lcInfo.vnamehead -relief flat -width $listWidth -height 1
  .lcInfo.vnamehead insert end "View name"
  listbox .lcInfo.vnamelist -relief flat -width $listWidth \
    -height $listHeight -yscrollcommand {.lcInfo.scroll set}

  set listWidth 20
  listbox .lcInfo.vtitlehead -relief flat -width $listWidth -height 1
  .lcInfo.vtitlehead insert end "View title"
  listbox .lcInfo.vtitlelist -relief flat -width $listWidth \
    -height $listHeight -yscrollcommand {.lcInfo.scroll set}

  set listWidth 15
  listbox .lcInfo.mshead -relief flat -width $listWidth -height 1
  .lcInfo.mshead insert end "Leader/follower"
  listbox .lcInfo.mslist -relief flat -width $listWidth \
    -height $listHeight -yscrollcommand {.lcInfo.scroll set}

  #TEMP Dragging one listbox doesn't move the others.  It looks like there's
  # a way to make that happen, but I don't want to go to the trouble right
  # now.  RKW 1998-10-20.
  scrollbar .lcInfo.scroll -command [list BindYview [list \
    .lcInfo.lclist .lcInfo.typelist .lcInfo.namelist \
    .lcInfo.vnamelist .lcInfo.vtitlelist .lcInfo.mslist ]]

  UpdateLinkCursorInfo

  # Pack everything.
  pack .lcInfo.row1 -side bottom -pady 4m
  pack .lcInfo.row2 -side bottom
  pack .lcInfo.row3 -side left

  pack .lcInfo.listframe -in .lcInfo.row2 -side left

  pack .lcInfo.ok -in .lcInfo.row1 -side left -padx 3m
  pack .lcInfo.lclist .lcInfo.namelist .lcInfo.typelist .lcInfo.vnamelist \
    .lcInfo.vtitlelist .lcInfo.mslist -in .lcInfo.listframe -side left -padx 0
  pack .lcInfo.scroll -in .lcInfo.row2 -side right -fill y
  pack .lcInfo.lchead .lcInfo.namehead .lcInfo.typehead .lcInfo.vnamehead \
    .lcInfo.vtitlehead .lcInfo.mshead -in .lcInfo.row3 -side left

  # *Don't* grab input focus here, because we want the user to be able
  # to do other stuff while having the link/cursor info showing.
}

############################################################
# Update the link/cursor info, if it's currently shown.

proc UpdateLinkCursorInfo {} {

  if {![winfo exists .lcInfo]} {
    return
  }

  # Clear out the listbox.
  .lcInfo.lclist delete 0 end
  .lcInfo.namelist delete 0 end
  .lcInfo.typelist delete 0 end
  .lcInfo.vnamelist delete 0 end
  .lcInfo.vtitlelist delete 0 end
  .lcInfo.mslist delete 0 end

  set links [NonPileLinkSet]
  set links [lsort $links]

  foreach link $links {
    set flag [DEVise getLinkFlag $link]
    set views [DEVise getLinkViews $link]

    set masterView ""
    set slaveStr ""

    if {$flag == 1} {
      set type "VisualX"
    } elseif {$flag == 2} {
      set type "VisualY"
    } elseif {$flag == 3} {
      set type "VisualXY"
    } elseif {$flag == 128} {
      if {[DEVise getLinkType $link] == 1} {
        set type "Record+"
      } else {
        set type "Record-"
      }
      set masterView [DEVise getLinkMaster $link]
      set slaveStr "follower"
    } elseif {$flag == 1024} {
      set masterAttr [DEVise getLinkMasterAttr $link]
      set slaveAttr [DEVise getLinkSlaveAttr $link]
      set type "Set ($masterAttr/$slaveAttr)"
      set masterView [DEVise getLinkMaster $link]
      set slaveStr "follower"
    } else {
      set type "unknown"
    } 

    if {$masterView == "" && [llength $views] == 0} {
      # Link has no views.
      .lcInfo.lclist insert end "link"
      .lcInfo.namelist insert end $link
      .lcInfo.typelist insert end $type
      .lcInfo.vnamelist insert end ""
      .lcInfo.vtitlelist insert end ""
      .lcInfo.mslist insert end ""
    } else {
      # Link has views.
      if {$masterView != ""} {
        set labelParams [DEVise getLabel $masterView]
        set viewTitle [lindex $labelParams 2]

        .lcInfo.lclist insert end "link"
        .lcInfo.namelist insert end $link
        .lcInfo.typelist insert end $type
        .lcInfo.vnamelist insert end $masterView
        .lcInfo.vtitlelist insert end $viewTitle
        .lcInfo.mslist insert end "leader"
      }

      foreach view $views {
        set labelParams [DEVise getLabel $view]
        set viewTitle [lindex $labelParams 2]

        .lcInfo.lclist insert end "link"
        .lcInfo.namelist insert end $link
        .lcInfo.typelist insert end $type
        .lcInfo.vnamelist insert end $view
        .lcInfo.vtitlelist insert end $viewTitle
        .lcInfo.mslist insert end $slaveStr
      }
    }
  }

  set cursors [CategoryInstances "cursor"]
  set cursors [lsort $cursors]

  foreach cursor $cursors {
    set views [DEVise getCursorViews $cursor]
    set type [DEVise getCursorType $cursor]

    if {[lindex $views 0] == {} && [lindex $views 1] == {}} {
      # Cursor has no views.
      .lcInfo.lclist insert end "cursor"
      .lcInfo.namelist insert end $cursor
      .lcInfo.typelist insert end $type
      .lcInfo.vnamelist insert end ""
      .lcInfo.vtitlelist insert end ""
      .lcInfo.mslist insert end ""
    } else {
      # Cursor has views.
      # First view is source.
      set srcdest "source"
      foreach view $views {
	if {$view != {}} {
          set labelParams [DEVise getLabel $view]
          set viewTitle [lindex $labelParams 2]

          .lcInfo.lclist insert end "cursor"
          .lcInfo.namelist insert end $cursor
          .lcInfo.typelist insert end $type
          .lcInfo.vnamelist insert end $view
          .lcInfo.vtitlelist insert end $viewTitle
          .lcInfo.mslist insert end $srcdest

	  # Second view is destination.
	  set srcdest "destination"
	}
      }
    }
  }
}

############################################################
# Control all of the damn listboxes with one scrollbar.
#
# Note: this procedure is taken from the examples in _Practical Programming
# in Tcl and Tk_ by Brent Welch.

proc BindYview { lists args } {
  foreach l $lists {
    eval {$l yview} $args
  }
}

############################################################
# Toggle whether the selected view is a highlight view.

proc ToggleViewHighlight {} {
    global curView

    set highlight [DEVise viewGetIsHighlight $curView]
    set highlight [expr !$highlight]
    DEVise viewSetIsHighlight $curView $highlight
}

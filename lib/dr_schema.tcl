# ========================================================================
# DEVise Data Visualization Software
# (c) Copyright 1992-1999
# By the DEVise Development Group
# Madison, Wisconsin
# All Rights Reserved.
# ========================================================================

# Under no circumstances is this software to be copied, distributed,
# or altered in any way without prior permission from the DEVise
# Development Group.

############################################################

# This module contains the DataReader schema GUI.

# Note: as of 1999-03-10, this doesn't deal correctly with "skip"
# attributes.  RKW.

############################################################

# $Id$

# $Log$
# Revision 1.1  1998/07/17 18:38:09  wenger
# Oops!  Forgot this.
#

############################################################

proc DrNewSchema {} {

    DrSetDefaults
    return [DrSchemaWin]
}

#----------------------------------------------------------------------------
proc DrOpenSchema {schemaFile} {

    global attrlist sname

    DrSetDefaults

    set sname $schemaFile

    if {[DrReadSchema $schemaFile]} {
        DrSchemaWin
    }
}

#----------------------------------------------------------------------------
proc DrSchemaWin {} {

    global sname comm separ attrlist aname
    global atype astrlen attrnum glist Logical
    global schemadir
    global adatef

    # Count number of attributes
    set attrnum 0

    toplevel .drSchemaWin
    wm title .drSchemaWin "DataReader Schema Description"
    wm geometry .drSchemaWin +50+50
    
    # Top level rows in the window
    frame .drSchemaWin.firstrow -relief raised
    frame .drSchemaWin.commseprow -relief raised
    frame .drSchemaWin.opt -relief raised
    frame .drSchemaWin.attr -relief raised
    pack .drSchemaWin.firstrow  .drSchemaWin.commseprow .drSchemaWin.attr \
	    .drSchemaWin.opt 	-side top -fill x -padx 1m -pady 1m
    
    #-----------------------------------------------------------
    # Within first row - schema name
    frame .drSchemaWin.fname -relief raised -bd 1
    pack .drSchemaWin.fname -in .drSchemaWin.firstrow \
	    -side left -fill x -padx 3m -pady 1m
    
    label .drSchemaWin.fname.label -text "Schema Name "
    entry .drSchemaWin.fname.entry -width 25 -relief sunken -bd 2 \
	    -textvariable sname
    pack .drSchemaWin.fname.label .drSchemaWin.fname.entry -side left -padx 1m \
	    -pady 1m
    
    #-----------------------------------------------------------
    # Comment and separator characters
    frame .drSchemaWin.comm -relief raised
    frame .drSchemaWin.separ -relief raised
    pack .drSchemaWin.comm -side left  -in .drSchemaWin.commseprow \
	    -padx 3m -pady 1m 
    pack .drSchemaWin.separ -side right  -in .drSchemaWin.commseprow \
	    -padx 3m -pady 1m
    
    label .drSchemaWin.comm.label -text "Comment characters"
    entry .drSchemaWin.comm.entry -width 5 -relief sunken -bd 2 \
	    -textvariable comm
    pack .drSchemaWin.comm.label .drSchemaWin.comm.entry -side left \
	    -padx 1m -pady 1m
    
    label .drSchemaWin.separ.label -text "Field separator"
    entry .drSchemaWin.separ.entry -width 15 -relief sunken -bd 2 \
	    -textvariable separ
    bind .drSchemaWin.separ.entry <Tab> { set separ "$separ\	'" }

    pack .drSchemaWin.separ.label .drSchemaWin.separ.entry -side left \
	    -padx 1m -pady 1m 
    
    #-----------------------------------------------------------
    # Attributes
    frame .drSchemaWin.attrlist -relief raised
    frame .drSchemaWin.attradd -relief raised
    pack .drSchemaWin.attrlist -in .drSchemaWin.attr \
	    -side top -padx 3m -pady 1m -fill x
    pack .drSchemaWin.attradd -in .drSchemaWin.attr \
	    -side bottom -padx 3m -pady 1m -fill x
    
    label .drSchemaWin.attrlist.label -text "List of Attributes"
    label .drSchemaWin.attrlist.spec \
	    -text "Type   Name           Maxlen  Date_format" \
	    -font 9x15
    pack .drSchemaWin.attrlist.label -side top -fill x
    pack .drSchemaWin.attrlist.spec -side top -anchor nw
    listbox .drSchemaWin.attrlist.list -relief raised -bd 1 \
	    -yscrollcommand ".drSchemaWin.attrlist.scroll set" \
	    -xscrollcommand ".drSchemaWin.attrlist.xscroll set" \
	    -font 9x15 -selectmode extended
    scrollbar .drSchemaWin.attrlist.scroll \
	    -command ".drSchemaWin.attrlist.list yview"
    scrollbar .drSchemaWin.attrlist.xscroll -orient horizontal \
	    -command ".drSchemaWin.attrlist.list xview"
    pack .drSchemaWin.attrlist.scroll -side right -fill y
    pack .drSchemaWin.attrlist.xscroll -side bottom -fill x
    pack .drSchemaWin.attrlist.list -fill x 
    
    # By default insert all elements of attrlist into the list
    foreach attr $attrlist {
	# Format before inserting
	set ttype [lindex $attr 0]
	set tname [lindex $attr 1]
	set tstrlen [lindex $attr 2]
	set tdatef [lindex $attr 3]
	set fstr [DrFormatEntry $ttype $tname $tstrlen $tdatef]
	.drSchemaWin.attrlist.list insert end $fstr

	set attrnum [expr $attrnum+1]
    }
    
    frame .drSchemaWin.attrspec -relief raised
    frame .drSchemaWin.attrop -relief raised
    pack .drSchemaWin.attrspec .drSchemaWin.attrop \
	    -in .drSchemaWin.attradd -side top -fill x -padx 3m -pady 1m
    
    frame .drSchemaWin.attrname -relief raised -bd 1
    frame .drSchemaWin.atype -relief raised -bd 1
    frame .drSchemaWin.astrlen -relief raised -bd 1
    frame .drSchemaWin.adatef -relief raised -bd 1
    pack .drSchemaWin.attrname .drSchemaWin.atype .drSchemaWin.astrlen \
	    .drSchemaWin.adatef \
	    -in .drSchemaWin.attrspec -side top -fill x -padx 3m -pady 1m
    
    # Attribute name
    label .drSchemaWin.attrname.label -text "Attribute Name "
    entry .drSchemaWin.attrname.entry -width 25 -relief sunken -bd 2 \
	    -textvariable aname
    pack .drSchemaWin.attrname.label .drSchemaWin.attrname.entry -side left \
	    -padx 3m -pady 1m
    
    # Attribute type
    label .drSchemaWin.atype.label -text "Type   "
    radiobutton .drSchemaWin.atype.double -text double -variable atype \
	    -value "double" -anchor nw
    radiobutton .drSchemaWin.atype.int -text int -variable atype \
	    -value "int" -anchor nw
    radiobutton .drSchemaWin.atype.string -text string -variable atype \
	    -value "string" -anchor nw
    radiobutton .drSchemaWin.atype.date -text date -variable atype \
	    -value "date" -anchor nw
    pack .drSchemaWin.atype.label .drSchemaWin.atype.double \
    	    .drSchemaWin.atype.int \
	    .drSchemaWin.atype.string \
	    .drSchemaWin.atype.date  -side left -fill x -padx 1m 
    trace variable atype w DrChgType

    # Max string length
    label .drSchemaWin.astrlen.label -text "Max string length"
    entry .drSchemaWin.astrlen.entry -width 25 -relief sunken -bd 2 \
         -textvariable astrlen -state disabled
    set astrlen ""
    pack .drSchemaWin.astrlen.label .drSchemaWin.astrlen.entry -side left \
	    -padx 3m -pady 1m

    # Date format
    label .drSchemaWin.adatef.label -text "Date format"
    entry .drSchemaWin.adatef.entry -width 25 -relief sunken -bd 2 \
         -textvariable adatef -state disabled
    set adatef ""
    pack .drSchemaWin.adatef.label .drSchemaWin.adatef.entry -side left \
	    -padx 3m -pady 1m

    button .drSchemaWin.attrop.add -text "Add item" -command DrAddItem

    button .drSchemaWin.attrop.delete -text "Delete item" -command {
	.drSchemaWin.attrlist.list delete active
	# Decrement count
	set attrnum [expr $attrnum-1]
    }
    # Set up key binding for modify list
    bind .drSchemaWin.attrlist.list <Double-1> {
	set attrl [.drSchemaWin.attrlist.list get active] 
	set atype [lindex $attrl 0]
	set aname [lindex $attrl 1]
	set astrlen [lindex $attrl 2]
	set adatef [lindex $attrl 3]
    }
    
    button .drSchemaWin.attrop.modify -text "Modify item" \
      -command { DrModifyItem }

    pack .drSchemaWin.attrop.add .drSchemaWin.attrop.delete \
	    .drSchemaWin.attrop.modify -side left -padx 3m -pady 1m -expand 1
    
    #-----------------------------------------------------------
    # Bottom portion of window
    button .drSchemaWin.opt.save -text Save -command DrSaveSchema

    global schemaCancelled
    set schemaCancelled 0
    button .drSchemaWin.opt.quit -text Cancel -command {
            trace vdelete atype w DrChgType
	    set schemaCancelled 1
            destroy .drSchemaWin
    }

    pack .drSchemaWin.opt.save .drSchemaWin.opt.quit  -side left \
	    -padx 3m -pady 1m -expand 1
    
    # modified by DD so that it works with FSBox

    tkwait visibility .drSchemaWin
    grab .drSchemaWin

    tkwait window .drSchemaWin

    if { $schemaCancelled } {
        return ""
    } else {
        return $schemadir/physical/$sname
    }
}

#----------------------------------------------------------------------------
proc DrChgType {name element op} {
    upvar $name type

    global astrlen adatef

    if {$type == "string"} {
        .drSchemaWin.astrlen.entry configure -state normal
    } else {
        .drSchemaWin.astrlen.entry configure -state disabled
	set astrlen ""
    }

    if {$type == "date"} {
        .drSchemaWin.adatef.entry configure -state normal
    } else {
        .drSchemaWin.adatef.entry configure -state disabled
	set adatef ""
    }
}

#----------------------------------------------------------------------------
proc DrAddItem {} {

    global aname attrnum atype astrlen adatef

    set aname [lindex [string trim $aname] 0]
    if {$aname == ""} {
        showmesg "Attribute name has not been specified."
        return
    }

    if {$atype == "string"} {
        if {[string trim $astrlen] == ""} {
            showmesg "Attribute max string length has not been specified."
            return
	}
    }

    if {$atype == "date"} {
        if {[string trim $adatef] == ""} {
            showmesg "Attribute date format has not been specified."
            return
	}
    }

    # Increment count
    set attrnum [expr $attrnum+1]
    # Insert into list and clear selection
    set fstr [DrFormatEntry $atype $aname $astrlen $adatef]

    .drSchemaWin.attrlist.list insert end $fstr

    set aname ""
    set atype "double"
    set astrlen ""
    set adatef ""
}

#----------------------------------------------------------------------------
# returns 1 if okay, 0 if error
proc DrReadSchema {schemaFile} {

    global schemadir attrlist
    global comm separ

    if {[catch {set schemaList \
      [DEVise parseDRSchema $schemadir/physical/$schemaFile]}]} {
        return 0
    }

    # Ignore record separator for now -- defaults to newline.

    set separ [lindex $schemaList 1]
    set comm [lindex $schemaList 2]

    set attrlist [lrange $schemaList 3 end]

    return 1
}

#----------------------------------------------------------------------------
proc DrSaveSchema {} {
    global sname attrnum comm separ atype

    if {$sname == ""} {
        showmesg "No schema name specified."
    } else {
        set attrlist {}
        while {$attrnum > 0} {
	    set attr [.drSchemaWin.attrlist.list get [expr $attrnum-1]]
	    set attrnum [expr $attrnum-1]

	    # Append to attrlist
	    set attrlist [linsert $attrlist 0 $attr]
        }
	    
        # Write into a file 
	DrWriteSchema $sname $comm $separ $attrlist

        trace vdelete atype w DrChgType
        destroy .drSchemaWin
    }
}

#----------------------------------------------------------------------------
proc DrWriteSchema {name comment fieldSep attrlist} {

    global schemadir

    set f [open $schemadir/physical/$name w]

    set header [DEVise getFileHeader physSchema]
    puts $f $header
    unset header

    puts $f "datareader 2.0;"
    puts $f ""

    puts $f "record_separator = \[\\n\]+;"
    if {$fieldSep != ""} {
        puts $f "field_separator = $fieldSep;"
    }
    if {$comment != ""} {
        puts $f "comment = \"$comment\";"
    }
    puts $f ""

    foreach attr $attrlist {
	set type [lindex $attr 0]
	set name [lindex $attr 1]
	set maxlen [lindex $attr 2]
	set datef [lindex $attr 3]

	puts -nonewline $f "$type $name"
	if {$maxlen != ""} {
	    puts -nonewline $f " maxlen = $maxlen"
	}
	if {$datef != ""} {
	    puts -nonewline $f " date_format = \"$datef\""
	}
	puts $f ";"
    }

    close $f
}

#----------------------------------------------------------------------------
proc DrSetDefaults {} {
    global sname comm separ attrlist aname
    global atype astrlen

    # First set up some global variables
    set sname ""
    set comm "//"
    set separ "\[\\t\]+"
    set attrlist ""
    set aname ""
    set atype "double"
    set astrlen 0
    set adatef ""
}

#----------------------------------------------------------------------------
proc DrModifyItem {} {

    global atype aname astrlen adatef

    set aname [lindex [string trim $aname] 0]
    if {$aname == ""} {
        showmesg "Double-click on an item to select it."
        return
    }

    if {$atype == "string"} {
        if {[string trim $astrlen] == ""} {
            showmesg "Attribute max string length has not been specified."
            return
	}
    }

    if {$atype == "date"} {
        if {[string trim $adatef] == ""} {
            showmesg "Attribute date format has not been specified."
            return
	}
    }

    set fstr [DrFormatEntry $atype $aname $astrlen $adatef]

    .drSchemaWin.attrlist.list insert active $fstr
    .drSchemaWin.attrlist.list delete active

    # Clear selection
    set aname ""
    set atype "double"
    set astrlen ""
}

#----------------------------------------------------------------------------
proc DrFormatEntry {type name maxlen datef} {

    if {$name == ""} {
	# "skip" attribute
        set name "{}"
    }

    if {$maxlen == ""} {
        set maxlen "{}"
    }

    set attrstr [format "%-6.6s %-14.14s %-6.6s  {%s}" \
      $type $name $maxlen $datef]
    return $attrstr
}

#============================================================================

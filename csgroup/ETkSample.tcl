#!/bin/sh
#\
exec wish $0 $*

button .b -text "Embedded Tk Window\n(click me)" -command ShowArgs
pack .b -fill both -expand true

proc ShowArgs {} {

    global argc argv
    catch {destroy .args}
    set w [toplevel .args]
    wm title $w "ETk Arguments"
    
    set l [label $w.head -text \
	    "The Tcl script received these\ncommand-line arguments:"]
    pack $l -side top -anchor w -padx 5 -pady 5
    
    set l [label $w.argc -text "argc:\t$argc"]
    pack $l -side top -anchor w -padx 5
    
    set b [button $w.ok -text OK -command "destroy $w"]
    pack $b -side bottom -pady 3
    
    set f [frame $w.border -height 2 -bg black]
    pack $f -fill x -expand true -side bottom
    
    set i 0
    foreach a $argv {
	set l [label $w.argv$i -text "arg $i:\t[lindex $argv $i]"]
	pack $l -side top -anchor w -padx 5
	incr i
    }

}



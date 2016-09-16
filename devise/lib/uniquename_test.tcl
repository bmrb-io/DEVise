#!/s/std/bin/wish -f
# ========================================================================
# DEVise Data Visualization Software
# (c) Copyright 1992-1996
# By the DEVise Development Group
# Madison, Wisconsin
# All Rights Reserved.
# ========================================================================

# Under no circumstances is this software to be copied, distributed,
# or altered in any way without prior permission from the DEVise
# Development Group.

############################################################

# This module contains Tcl code for testing the UniqueName procedure.

############################################################

# $Id$

# $Log$

##########################################################################

set nameList 0

##########################################################################

proc DEVise { command args } {
    global nameList

    if {$command == "exists"} {
#	puts "DIAG nameList = $nameList"
	set name [lindex $args 0]
#	puts "  DIAG name = $name"
	if {[lsearch $nameList $name] == -1} {
#	    puts "  DIAG doesn't exist"
	    return 0
	} else {
#	    puts "  DIAG exists"
	    return 1
	}
    }

    return 0
}

##########################################################################

source util.tcl

puts "Starting with 'View'"

for {set count 0} {$count <= 20} {set count [expr $count + 1]} {
    set name [UniqueName View]
    lappend nameList $name
    puts $name
}

puts ""
puts "Starting with 'View 1'"
set nameList 0

for {set count 0} {$count <= 20} {set count [expr $count + 1]} {
    set name [UniqueName {View 1}]
    lappend nameList $name
    puts $name
}

#============================================================================

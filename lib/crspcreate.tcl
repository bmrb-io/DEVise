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
#  Revision 1.1  1995/11/24 07:48:22  ravim
#  Initial version.
#

############################################################

# Read the crsp index file and output the coral program for loading
# the relation.

proc crspcreate {ifname rel ofname} {
    global datadir
    set infile [open $ifname r]
    set outfile [open $ofname w]

    # Output initial portion of program
    puts $outfile "set(insert_mode)."
    puts $outfile "t_begin."
    
    # Read through each line of the index file and insert quotes around strings
    while {[gets $infile line] >= 0} {
	puts -nonewline $outfile "$rel\("
	set line [split $line ,]
	puts -nonewline $outfile [lindex $line 0]
	puts -nonewline $outfile ","
	puts -nonewline $outfile [lindex $line 1]
	puts -nonewline $outfile ","
	puts -nonewline $outfile [lindex $line 2]
	puts -nonewline $outfile ",\""
	puts -nonewline $outfile [lindex $line 3]
	puts -nonewline $outfile "\","
	puts -nonewline $outfile [lindex $line 4]
	puts -nonewline $outfile ","
	puts -nonewline $outfile [lindex $line 5]
	puts -nonewline $outfile ","
	puts -nonewline $outfile [lindex $line 6]
	puts -nonewline $outfile ","
	puts -nonewline $outfile [lindex $line 7]
	puts -nonewline $outfile ","
	puts -nonewline $outfile [lindex $line 8]
	puts $outfile "\)."
    }
    
    # Output final portion of program
    puts $outfile "t_commit."
    puts $outfile "clear(insert_mode)."
    
    # Close files
    close $infile
    close $outfile
}




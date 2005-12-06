# Test DEVise 'home' functionality.
# $Id$

# Note: as this script exists now, it is very incomplete, even for testing
# the "home" bugs.  I'm setting it up mainly as a template for future
# expansion.  wenger 2003-06-06.

# Ideally, all test sessions, data, etc., should be put into CVS, but
# I'm leaving that for the future.  wenger 2003-06-06.

set okay 1

#
# Test bug 874.
#
DEVise openSession $env(DEVISE_SESSION)/Test/Bug874.ds
DEVise keyToView {Y vs. X} 53 0 0
DEVise waitForQueries
set filters [DEVise getVisualFilters {Y vs. X}]
set filter [lindex $filters [expr [llength $filters]-1]]
set ymin [lindex $filter 1]
if {$ymin != "0"} {
    puts "ERROR: home.tcl bug 874 test fails!"
    set okay 0
}




#
# Final summary.
#
if {$okay} {
    puts "home.tcl test OK!"
    DEVise testExit 0
} else {
    puts "home.tcl test FAILED!"
    DEVise testExit 1
}

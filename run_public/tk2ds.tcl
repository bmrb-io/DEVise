#!/s/std/bin/tclsh
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

# Script for converting an old-style (Tcl) session file to a
# new-style (batch) session file.

############################################################

# $Id$

# $Log$
# Revision 1.1  1997/10/02 18:46:38  wenger
# Opening and saving batch-style sessions in back end now fully working;
# added tk2ds.tcl script for conversion.
#

############################################################

global newFd

#-----------------------------------------------------------

proc scanDerivedSources {} {
  # No op.
}

#-----------------------------------------------------------

proc SetDescription {description} {
  # No op.
}

#-----------------------------------------------------------

proc OpenDataSource {tdataName} {

  # Turn old-style (pre-DTE) name into new-style (DTE) name.
  if {[string index $tdataName 0] != "."} {
    set newname ""
    regsub -all {\.| |,} $tdataName _ newname
    set tdataName ".$newname"
  }

  return $tdataName
}

#-----------------------------------------------------------

proc DEVise args {
  global newFd

  # Just echo to the output file unless the command is "DEVise create
  # tdata...", "DEVise importFileType", "DEVise dteImportFileType", or
  # "DEVise dataSegment".
  if {[string match "create" [lindex $args 0]] && \
      [string match "tdata" [lindex $args 1]]} {
    # No op.
  } elseif {[string match "importFileType" [lindex $args 0]]} {
    # No op.
  } elseif {[string match "dteImportFileType" [lindex $args 0]]} {
    # No op.
  } elseif {[string match "dataSegment" [lindex $args 0]]} {
    # No op.
  } else {
    puts $newFd "DEVise $args"
  }
}

#-----------------------------------------------------------

# Check for correct arguments.
if {$argc != 1 && $argc != 2} {
  puts "Usage: $argv0 <old session file> \[new session file\]"
  exit
}

# Get file names.
set oldFileName [lindex $argv 0]

if {$argc > 1} {
  set newFileName [lindex $argv 1]
} else {
  regsub {\.tk} $oldFileName {.ds} newFileName
}

if {$oldFileName == $newFileName} {
  puts "old session file and new session file must have different names"
  exit
}

puts "Converting $oldFileName to $newFileName"

# Open/create files.
set oldFd [open $oldFileName r]
set newFd [open $newFileName w]

# Read and convert the old session file.
set template 0

source $oldFileName

# Close files.
close $newFd
close $oldFd

#============================================================================

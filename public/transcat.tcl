#!/s/std/bin/tclsh

source sourcedef.tcl

proc addQuotes {s} {
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

puts "1 catentry entry ;\n"

foreach entry [array names sourceList] {
	# 0th, 4th, 7th and 8th entries are quoted 
	set lst $sourceList($entry)
	set type [lindex $lst 0]
	if {$type != "UNIXFILE"} {
		continue
	}
	regsub -all {\.| |,} $entry _ entry
	puts -nonewline "[addQuotes $entry] "
	set i 0
	while {$i < 4} {
		puts -nonewline "[lindex $lst $i] "
		incr i
	}
	puts -nonewline "[addQuotes [lindex $lst $i]] "
	incr i
	while {$i < 7} {
		puts -nonewline "[lindex $lst $i] "
		incr i
	}
	puts -nonewline "[addQuotes [lindex $lst $i]] "
	incr i
	puts -nonewline "[addQuotes [lindex $lst $i]] "
	puts ";"
}

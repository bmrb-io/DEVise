#!/s/std/bin/wish

#
# ShowWord.tcl
# args: filename word <line number where word first appears>
#

set backGround gray85
set foreGround black

tk_setPalette background $backGround foreground $foreGround \
	selectBackground $foreGround selectForeground $backGround
. configure -bg $backGround

proc Next { text word index } {
    set exp "(\[^A-Za-z0-9_\]|^)($word)(\[^A-Za-z0-9_\]|\$)"
    set index [$text index "$index + 1 chars"]
    set next [$text search -forwards -nocase -regexp \
	    $exp $index end]
    if {$next == {}} {
	bell
	return {}
    }
    set next [$text search -forwards -nocase $word $next]
    #puts "Next: index $index, next $next"
    $text see $next
    HighlightWord $text $next
    return $next
}

proc Prev { text word index } {
    set exp "(\[^A-Za-z0-9_\]|^)($word)(\[^A-Za-z0-9_\]|\$)"
    set index [$text index "$index - 1 chars"]
    set next [$text search -backwards -nocase -regexp \
	    $exp $index 1.0]
    if {$next == {}} {
	bell
	return {}
    }
    set next [$text search -forwards -nocase $word $next]
    #puts "Prev: index $index, next $next"
    $text see $next
    HighlightWord $text $next
    return $next
}

proc HighlightWord { text index } {
    global Index Line
    $text tag delete TheWord
    $text tag add TheWord $index "$index wordend"
    $text tag config TheWord -foreground black -background yellow
    set Index $index
    set Line "Line [lindex [split $index .] 0]"
}

proc Build {} {

    foreach w [winfo children .] {
	catch {destroy $w}
    }
    
    . config -relief flat -bd 4 -highlightthickness 3 \
	    -highlightbackground black
    
    frame .top
    set f [frame .top.file]
    label $f.label -anchor w -text "File:"
    label $f.value -anchor w -textvariable FileTail -relief sunken -bd 2
    pack $f.label -side left
    pack $f.value -side left -fill x -expand true
    pack $f -side left -fill x -expand true
    
    set f [frame .top.string]
    label $f.label -anchor w -text "Search string:"
    label $f.value -anchor w -textvariable Word -relief sunken -bd 2
    pack $f.label -side left
    pack $f.value -side left -fill x -expand true
    pack $f -side right -fill x -expand true
        
    set f [frame .text]
    set t [text $f.t -setgrid true -wrap none -state normal -font fixed \
	    -exportselection false -highlightthickness 0 \
	    -yscrollcommand "$f.sy set" -xscrollcommand "$f.sx set"]
    scrollbar $f.sy -orient vertical -width 10 -command "$f.t yview" \
	    -highlightthickness 0
    scrollbar $f.sx -orient horizontal -width 10 -command "$f.t xview" \
	    -highlightthickness 0    
    pack $f.sy -side right -fill y
    pack $f.sx -side bottom -fill x
    pack $f.t -side left -anchor nw -fill both -expand true    

    frame .bottom    
    set f [frame .bottom.lineno]
    label $f.lineno -relief sunken -bd 2 -textvariable Line
    pack $f.lineno -fill x -expand true
    pack $f -side left  -fill x -padx 10 -expand true
    set f [frame .bottom.buttons]
    button $f.first -text "<<\nFirst" -font fixed\
	    -command "Next $t \$Word 0.0"
    button $f.prev -text "  <  \nPrev" -font fixed \
	    -command "Prev $t \$Word \$Index"
    button $f.next -text "  >  \nNext" -font fixed \
	    -command "Next $t \$Word \$Index"
    button $f.last -text "  >> \nLast" -font fixed \
	    -command "Prev $t \$Word end"
    pack $f.first $f.prev $f.next $f.last -side left
    pack $f -side right -fill x

    pack .top -side top -fill x -anchor nw -pady 2 -padx 2
    pack .bottom -side bottom -fill x
    pack .text -side top -fill both -expand true -pady 2

}

proc ShowFile { text filename } {
    global FileName
    $text delete 1.0 end
    set FileName $filename
    if {$filename == {}} {
	return
    }
    $text config -state normal
    $text insert end [exec cat $filename]
    $text config -state disabled
}

proc Main {} {
    
    global argc argv FileName FileTail Index Word Text
    
    set FileName [lindex $argv 0]
    set FileTail [file tail $FileName]
    set Word [lindex $argv 1]
    set Index 1.0
    set Text .text.t
    
    Build
    
    ShowFile $Text $FileName

    if {$Word != {}} {
	Next $Text $Word 1.0
    }
    
}

proc ETk_Reset { argc argv } {
    tk_dialog .dialog Title "argc $argc, argv $argv" {} {} OK
}

#
# Begin executing...
#

if {![info exists tcl_interactive] || !$tcl_interactive} {
    Main
} else {
    puts "You can begin execution by submitting the Main command"
}


# Creates a button with 'label' as the text.
# When the button is pushed, it contacts an open browser and displays 'url'

global argc argv
set url [lindex $argv 0]
set label [lindex $argv 1]
button .b -text $label -command {BrowserOpenURL $url}
pack .b -fill both -expand true

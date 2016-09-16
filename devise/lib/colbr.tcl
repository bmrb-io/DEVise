proc Init {} {

	text .console -yscrollcommand ".sb set" -setgrid true
	scrollbar .sb -command ".console yview"
	pack .sb -side right -fill both
	pack .console -fill both -expand 1 -side left

	wm protocol . WM_DELETE_WINDOW { wm withdraw . }
	wm title . "Collaborator"

	.console tag configure hdg -foreground red
	.console tag configure bdg -foreground blue
	
	focus .console
}

proc outputHdg { buf } {

	.console insert insert "$buf" hdg
	.console yview -pickplace insert
}

proc outputBdg { buf } {

	.console insert insert "$buf" bdg
	.console yview -pickplace insert
}

Init
outputHdg "Ashish\n"
outputBdg "Thusoo\n"

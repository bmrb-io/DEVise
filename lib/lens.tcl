#========================================================================
#  DEVise Data Visualization Software
#  (c) Copyright 1992-1996
#  By the DEVise Development Group
#  Madison, Wisconsin
#  All Rights Reserved.
#  ========================================================================
#
#  Under no circumstances is this software to be copied, distributed,
#  or altered in any way without prior permission from the DEVise
#  Development Group.

############################################################

#       $Id$

#       $Log$
#       Revision 1.2.8.1  1997/05/20 16:11:27  ssl
#       Added layout manager to DEVise
#
#       Revision 1.2  1996/12/02 16:54:38  wenger
#       Fixed compile warning; added standard headers to some files;
#       conditionaled out debug code.
#

############################################################

proc DoWindowLensControl {} {
    global curView lensWinOpened

    if { [WindowVisible .lens] } {
	return
    }

    toplevel .lens
    wm title .lens "Lens Control"
    wm geometry .lens +100+100
    
    #build view name widget

    frame .lens.title
    pack .lens.title -side top -pady 1m -expand 1 -fill x
    
    
    label .lens.title.text -text "No View Selected"
    pack .lens.title.text -side top -pady 1m

    # build button bar widget

    frame .lens.bot
    frame .lens.bot.row1
    frame .lens.bot.row2
    frame .lens.bot.row1.but
    frame .lens.bot.row2.but

    button .lens.bot.row1.but.create -text "Create" -width 10 \
	    -command { DoLensCreate }
    button .lens.bot.row1.but.split -text "Split" -width 10 \
	    -command { DoLensSplit }
    button .lens.bot.row1.but.destroy -text "Destroy" -width 10\
	    -command { DoLensDestroy}
    button .lens.bot.row1.but.edit -text "Edit..." -width 10 \
	    -command { EditLens }
    button .lens.bot.row2.but.close -text Close -width 10 \
	    -command "global lensyWinOpened; \
	              set lensWinOpened false; \
	              destroy .lens"

    pack .lens.bot.row1.but.create .lens.bot.row1.but.split \
            .lens.bot.row1.but.destroy .lens.bot.row1.but.edit \
	    -side left -expand 1 -fill x -padx 3m
    
    pack   .lens.bot.row2.but.close \
	    -side left -expand 1 -fill x -padx 3m
    
    pack .lens.bot.row1.but -side top
    pack .lens.bot.row2.but -side top
    pack .lens.bot.row1 .lens.bot.row2 -side top -pady 1m
    pack .lens.bot -side top -pady 5m

    set lensWinOpened true

    if {$curView != ""} {
	.lens.title.text configure -text "View: $curView"
    }
}

proc DoLensCreate {} {
    global dialogCkButVar
    
    global curView
    # Create lens window 
    if {![CurrentView]} {
	return
    }
    

    set win [DEVise getViewWin $curView]
    set class [GetClass window $win]
    set newWin [UniqueName $win]
    set params [DEVise getCreateParam window $class $win]
    set newParam [linsert [lrange $params 1 end] 0 $newWin]
    eval DEVise create window $class $newParam
    puts "$win,$class, $newWin, $params, $newParam" 
    
    set views [ViewSet]
    
    set but [dialogCkBut .createLens "Create Lens" \
	    "Enter parameters for creation of Lens" ""\
	    0 {Cancel OK} name \
	    $views {3}]
    # Show list of views to create lens out of 
    set flag $dialogCkButVar(selected)
    set name $dialogCkButVar(entry)
    
    set selectedViews ""
    set length [expr [llength $views] - 1]
    puts "len = $length"
    if { $name == ""} {
	set but [dialog .noName "Name missing" \
		"No name given to View Lens" \
		"" 0 OK]
	return
    }
    for {set i $length } { $i >= 0 } {set i [expr $i - 1]} {
	puts "[expr 1 << $i]"
	if {[expr $flag & [expr 1 << $i]] != 0} {
	    puts "view [lindex $views $i]"
	    set selectedViews [lappend selectedViews [lindex $views $i]]
	}
    }

    puts "selected views = $selectedViews"
    puts "name = $name"
    puts "flag = $flag"
    
    set filter [ DEVise getCurVisualFilter $curView ]
#    set bgcolor [ DEVise getBgColor $curView]
    set bgcolor AntiqueWhite

    if {[DEVise create view ViewLens $name \
	    [lindex $filter 0] [lindex $filter 1] \
	    [lindex $filter 2] [lindex $filter 2] \
	    $bgcolor] == ""} {
	puts "error in creating viewlens"
	return
    }
    # ask DEVise to insert $viewinstance into $window
    set result [DEVise insertWindow $name $newWin]


    # Insert the selected views into the lens
    foreach view $selectedViews {
	if { [DEVise insertViewInLens $name $view] != "done"} {
	    puts "error in inserting View $view in lens $name"
	    return
	}
    }
    
    # Create a visual link between the selected views and the lens
    set link [UniqueName "Lens:Link"]
    puts "Creating $link for lens"
    set result [DEVise create link Visual_Link $link 3]
    if {$result == "" } {
	dialog .linkError "Link Error" \
		"Error in creating link $link" "" 0 OK
	return
    }
    
    #insert link into the selected views and the lens
    foreach view $selectedViews {
	puts "Inserting $link into $view"
	DEVise insertLink $link $view
	#refresh view
    }
    DEVise insertLink $link $name 
    DEVise setWindowLayout $newWin -1 -1 1
    DEVise refreshView $name
}

proc DoLensDestroy {} {
}

proc DoLensSplit  {} {
}

proc DoLensEdit {} {
}


#  ========================================================================
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

#  $Id$

#  $Log$
#  Revision 1.2  1996/11/23 00:24:01  wenger
#  Incorporated all of the PostScript-related stuff into the client/server
#  library; added printing to PostScript to the example client and server;
#  made some fixes to PSDisplay, PSWindowRep, and XWindowRep classes as
#  a result of testing with client/server stuff.
#
#  Revision 1.1  1996/10/17 20:42:18  jussi
#  Initial revision.
#

############################################################

set backGround black
set foreGround white

tk_setPalette background $backGround foreground $foreGround \
	selectBackground $foreGround selectForeground $backGround
. configure -bg $backGround

wm geometry . +100+100

frame .mbar -relief raised -bd 4
frame .panel -relief groove -width 8c -height 5c

pack .mbar -side top -expand 0 -fill x
pack .panel -side top -expand 1 -fill both

menubutton .mbar.file -text File -underline 0 \
	-menu .mbar.file.menu
menubutton .mbar.edit -text Edit -underline 0 \
	-menu .mbar.edit.menu

pack .mbar.file .mbar.edit -expand 1 -side left -fill x

menu .mbar.file.menu -tearoff 0
.mbar.file.menu add command -label "New" -command {Server file new}
.mbar.file.menu add command -label "Open..." -command {Server file open}
.mbar.file.menu add command -label "Close" -command {Server file close}
.mbar.file.menu add separator
.mbar.file.menu add command -label "Print" -command {Server print}
.mbar.file.menu add separator
.mbar.file.menu add command -label "Exit" -command {Server exit; destroy .}

menu .mbar.edit.menu -tearoff 0
.mbar.edit.menu add command -label "Cut" -command {Server edit cut}
.mbar.edit.menu add command -label "Copy" -command {Server edit copy}
.mbar.edit.menu add command -label "Paste" -command {Server edit paste}

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

############################################################

set DEViseColors \
	{{0 black} {1 white} {2 red} {3 blue} {4 orange} \
	{5 tan} {6 green} {7 purple} {8 aquamarine} {9 peru} {10 chocolate} \
	{11 tomato} {12 pink} {13 plum} {14 azure1} {15 cyan1} {16 SeaGreen1} \
	{17 khaki1} {18 goldenrod1} {19 yellow} {20 sienna1} \
	{21 LightCoral} {22 AntiqueWhite} {23 LemonChiffon} {24 LightGray}\
	{25 Lavender} {26 LavenderBlush} {27 MistyRose} {28 NavyBlue} \
	{29 SlateBlue} {30 MediumBlue} {31 DeepSkyBlue} {32 SkyBlue} \
	{33 red1} {34 red2} {35 green1} {36 green2} {37 blue1} \
	{38 blue2} {39 blue3} {40 blue4} {41 gold1} {42 gold2}}

set DEViseShapes \
	{{0 Rect} {1 RectX} {2 Bar} {3 Polygon} {4 Oval} {5 Vector} \
	{6 Block} {7 3DVector} {8 HorLine}}

set libdir "."
if { [info exists env(DEVISE_LIB)] } {
    set libdir $env(DEVISE_LIB)
}
set datadir "."
if { [info exists env(DEVISE_DAT)] } {
    set datadir $env(DEVISE_DAT)
}
set schemadir "."
if { [info exists env(DEVISE_SCHEMA)] } {
    set schemadir $env(DEVISE_SCHEMA)
}
set tmpdir "tmp"
if { [info exists env(DEVISE_TMP)] } {
    set tmpdir $env(DEVISE_TMP)
}

set hostCfg $libdir/config.tcl
if {![file exists $hostCfg]} {
    puts "Cannot read configuration file $hostCfg"
} else {
    puts "Reading configuration file $hostCfg"
    source $hostCfg
}

if {![catch {set hostCfg $libdir/config.$env(HOSTNAME)}]} {
    if {[file exists $hostCfg]} {
	puts "Reading configuration file $hostCfg"
	source $hostCfg
    }
}

source $libdir/dialog.tk
source $libdir/macrodef.tk
source $libdir/util.tcl
source $libdir/sources.tcl

set mode 0
DEVise changeMode $mode

set backGround DarkOliveGreen
set foreGround white

set progName [ file tail $argv0 ]
set envName [ format "%s_RC" [string toupper $progName] ]
set rcFile1 [ format ".%s_rc" $progName ]
set rcFile2 [ format "%s.rc" $progName ]
set rcFile3 "devise.rc"

if { [ info exists env($envName)] && [file exists $env($envName) ] } {
    set rcFile $env($envName)
} elseif { [ file exists $rcFile1 ] } {
    set rcFile $rcFile1
} elseif { [ file exists $rcFile2 ] } {
    set rcFile $rcFile2
} elseif { [ file exists $rcFile3 ] } {
    set rcFile $rcFile3
} else {
    set rcFile ""
}

if {$rcFile != ""} {
    puts "Reading initialization file $rcFile"
    source $rcFile
}

wm geometry . +0+0

tk_setPalette background $backGround foreground $foreGround \
	selectBackground $foreGround selectForeground $backGround
. configure -bg $backGround

message .msg -justify center -width 10c -text "Executing batch file..."
pack .msg -side top -padx 5m -pady 5m

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
#  Revision 1.42  1997/10/03 16:01:48  wenger
#  Enabled session opening and saving from back end; incremented version; a
#  few more minor fixes to session-related code.
#
#  Revision 1.41  1997/09/15 18:44:58  wenger
#  Kludgey fix for DTE thinking that schema file names starting with ".."
#  belong to it.
#
#  Revision 1.40  1997/07/17 19:50:39  wenger
#  Added menu selections to report number of strings and save string space.
#
#  Revision 1.39  1997/07/15 19:15:36  wenger
#  All session files now saved as batch scripts instead of Tcl scripts.
#
#  Revision 1.38  1997/06/18 21:06:40  wenger
#  Fixed problems saving to batch scripts.
#
#  Revision 1.37  1997/05/30 15:41:37  wenger
#  Most of the way to user-configurable '4', '5', and '6' keys -- committing
#  this stuff now so it doesn't get mixed up with special stuff for printing
#  Mitre demo.
#
#  Revision 1.36.6.1  1997/05/21 20:41:14  weaver
#  Changes for new ColorManager
#
#  Revision 1.36  1997/04/30 18:27:24  wenger
#  Added session text description capability.
#
#  Revision 1.35  1997/04/29 14:34:50  wenger
#  User interface improvments: Quit and Close don't ask for confirmation
#  if there is no session open; the file selection box gives better info
#  as to what you are doing; Save and Save as aren't allowed if there is
#  no session open; changed default print filename from
#  /u/g/u/guangshu/public/html/pictures/map/ back to /tmp/devise again.
#
#  Revision 1.34  1997/04/21 23:07:57  guangshu
#  Save statistics (In memory and by DTE) with session.
#
#  Revision 1.33  1997/04/14 20:46:03  donjerko
#  DTE files are recognized as one begining with "." but not with "./".
#
#  Revision 1.32  1997/03/20 20:46:08  donjerko
#  DTE Tdata generates unique names by appending sequential numbers to the
#  end of the table name. This way, same table can be opened multiple times.
#
#  Revision 1.31  1997/02/03 04:12:39  donjerko
#  Catalog management moved to DTE
#
#  Revision 1.30  1997/01/23 18:13:33  jussi
#  Disabled saving of pixmaps due to bug #135 and #069.
#
#  Revision 1.29  1997/01/22 23:24:08  jussi
#  Fixed computation of total number of TData streams saved in
#  session.
#
#  Revision 1.28  1997/01/06 19:12:05  wenger
#  Fixed the relative positions of 'OK' and 'Cancel' buttons on a number of
#  windows; view names now start with 'View 1' instead of 'View'; removed
#  some diagnostic output; added code to test the UniqueName procedure.
#
#  Revision 1.27  1996/12/20 16:50:37  wenger
#  Fonts for view label, x axis, and y axis can now be changed.
#
#  Revision 1.26  1996/11/20 16:48:22  jussi
#  Display/layout mode no longer saved in the session. When session
#  is restored, the user's currently selected mode is in control.
#
#  Revision 1.25  1996/07/23 17:26:44  jussi
#  Added saving of pileMode, overrideColor, and displayDataValues flags.
#
#  Revision 1.24  1996/07/21 02:22:45  jussi
#  Added saving of flags indicating solid/wireframe 3D objects and
#  XY vs. X/Y zoom mode.
#
#  Revision 1.23  1996/07/16 23:47:37  jussi
#  Added support for saving a session as a batch script (no Tcl variable
#  names used or if statements or anything, just a sequential list
#  of DEVise API calls).
#
#  Revision 1.22  1996/07/12 22:24:37  jussi
#  Views are now saved before tdata and mappings in a session file.
#  Exporting template with data does not add "addDataSource"
#  statements for BASICSTAT data sources.
#
#  Revision 1.21  1996/07/12 18:25:07  wenger
#  Fixed bugs with handling file headers in schemas; added DataSourceBuf
#  to TDataAscii.
#
#  Revision 1.20  1996/07/12 00:26:19  jussi
#  Added protective braces around TData names in addDataSource calls.
#  Fixed source definitions of non-UNIXFILE data sources in exported
#  templates.
#
#  Revision 1.19  1996/07/11 17:26:08  wenger
#  Devise now writes headers to some of the files it writes;
#  DataSourceSegment class allows non-fixed data length with non-zero
#  offset; GUI for editing schema files can deal with comment lines;
#  added targets to top-level makefiles to allow more flexibility.
#
#  Revision 1.18  1996/07/08 17:15:44  jussi
#  Added protective braces around mapping label.
#
#  Revision 1.17  1996/07/05 17:05:00  jussi
#  Disabled debugging statements which I forgot to do before
#  last check-in.
#
#  Revision 1.16  1996/07/05 15:25:52  jussi
#  The names of compiled mapping classes are no longer saved in session
#  files. This behavior confused Devise and when the session was
#  restored and re-saved, twice as many class names were saved etc.
#
#  Revision 1.15  1996/07/01 19:36:46  jussi
#  Made changes to reflect the new TData constructor interface.
#
#  Revision 1.14  1996/06/27 17:21:13  jussi
#  Fixed minor bug in saving 3D location.
#
#  Revision 1.13  1996/06/27 00:00:35  jussi
#  Coordinates of 3D focal point are now saved and displayed.
#
#  Revision 1.12  1996/06/26 17:22:06  wenger
#  Finished code for saving data to templates (includes quite a bit of
#  cleanup of the DoActualSave procedure).
#
#  Revision 1.11  1996/06/24 15:52:02  wenger
#  First version of Tcl code to save data to templates.
#
#  Revision 1.10  1996/06/21 20:11:03  jussi
#  Location of camera is now saved in the session file.
#
#  Revision 1.9  1996/06/19 19:56:56  wenger
#  Improved UtilAtof() to increase speed; updated code for testing it.
#
#  Revision 1.8  1996/06/15 14:26:44  jussi
#  Added saving of mapping legends.
#
#  Revision 1.7  1996/06/12 14:57:58  wenger
#  Added GUI and some code for saving data to templates; added preliminary
#  graphical display of TDatas; you now have the option of closing a session
#  in template mode without merging the template into the main data catalog;
#  removed some unnecessary interdependencies among include files; updated
#  the dependencies for Sun, Solaris, and HP; removed never-accessed code in
#  ParseAPI.C.
#
#  Revision 1.6  1996/05/31 16:36:12  jussi
#  Minor fix in DoActualSave.
#
#  Revision 1.5  1996/05/31 15:49:34  jussi
#  Fixed problem with tdata dictionary. Added saving of link master.
#
#  Revision 1.4  1996/05/16 21:39:06  wenger
#  implemented saving schemas ans session file for importing tdata
#
#  Revision 1.3  1996/04/14 00:27:00  jussi
#  Changed format of session file to reflect the new mapping class
#  interface (namely, createInterp has been replaced with
#  createMappingClass).
#
#  Revision 1.2  1996/03/07 17:00:23  jussi
#  Added support for number of dimensions.
#
#  Revision 1.1  1996/01/23 20:50:58  jussi
#  Initial revision.
#

############################################################

# Save session
proc DoActualSave { infile asTemplate asExport withData asBatchScript } {
    global templateMode schemadir datadir

    # Change for saving sessions as batch scripts only.
    set asBatchScript 1

    # you can't save an imported file until it has been merged
    if {$templateMode} { 
	dialog .open "Merge Data" \
	    "You must merge an imported template before saving it" "" 0 OK
	return
    }

    if {$asBatchScript} {
        if {$asTemplate || $asExport || $withData} {
            dialog .open "Incompatible Selection" \
                    "Cannot save batch script as a template\
                    or as an exported template or with data" "" 0 OK
            return
        }
    }

    if {[file exists $infile]} {
	if { ![file writable $infile] } {
	    dialog .saveError "Cannot Save Session" \
		    "File $infile not writable" "" 0 OK
	    return
	}
    }

    ChangeStatus 1

    # rename existing file to a backup file
    catch { exec mv $infile $infile.bak }

    # checkpoint tdata
    set classes [ DEVise get "tdata" ]
    foreach class $classes {
	set instances [ DEVise get "tdata" $class ]
	foreach inst $instances {
	    DEVise tcheckpoint $inst
	}
    }

    DEVise saveSession $infile $asTemplate $asExport $withData
}

############################################################

proc DoSave {} {
    global sessionName templateMode

    if {$sessionName == "session.tk"} {
	DoSaveAs 0 0 0 0
	return
    }

    DoActualSave $sessionName 0 0 0 0
}

############################################################

proc DoSaveAs { asTemplate asExport withData asBatchScript } {
    global sessionName fsBox sessiondir templateMode

    if {![SessionIsOpen]} {
      puts "No session open -- save not allowed"
      return
    }

    # Get file name
    set fsBox(path) $sessiondir
    #TEMP?set fsBox(pattern) *.tk
    set fsBox(pattern) *.ds
    set file [ FSBox "Select file to save session as" ]
    
    if {$file == ""} { return }

    # <<< Changes for saving sessions as batch scripts only...
    # If the user selected a .tk file, change extension to .ds.
    regsub {.tk} $file {.ds} file

    # If there is no extension, append .ds.
    set slash [string last "/" $file]
    set fileonly [string range $file [expr $slash + 1] end]
    if {![regexp {\.} $fileonly]} {
      set file $file.ds
    }
    # ... >>>

    set button [ dialog .saveSession "Save Session" \
	    "Save session to file\n$file?"  "" 0  OK {Cancel} ]
    if {$button != 0} { return }

    DoActualSave $file $asTemplate $asExport $withData $asBatchScript
    if {!$asTemplate} {
	set sessionName $file
    }
}

####################################################################

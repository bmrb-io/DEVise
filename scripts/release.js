#! /bin/csh -f

#  ========================================================================
#  DEVise Data Visualization Software
#  (c) Copyright 2001-2015
#  By the DEVise Development Group
#  Madison, Wisconsin
#  All Rights Reserved.
#  ========================================================================
#
#  Under no circumstances is this software to be copied, distributed,
#  or altered in any way without prior permission from the DEVise
#  Development Group.

#  Release the JavaScreen.

############################################################

#  $Id$

#  $Log$
#  Revision 1.18  2015/05/13 16:44:54  wenger
#  Fixed release.js script to release stuff in the cgi-bin directory,
#  which also fixed bug 1047.
#
#  Revision 1.17  2014/05/30 21:09:25  wenger
#  Fixed bug 1033:  added path to js_is_signed.pl invocations.
#
#  Revision 1.16  2013/12/17 17:51:20  wenger
#  Fixed JS release script for single jar file.
#
#  Revision 1.15  2013/12/13 21:51:31  wenger
#  Merged js_todo14_002_br_0 thru js_todo14_002_br_1 to trunk.
#
#  Revision 1.14.18.5  2013/12/12 21:49:28  wenger
#  JS signing:  Minor cleanups.
#
#  Revision 1.14.18.4  2013/11/27 23:36:13  wenger
#  JS signing:  Added js_is_signed.pl to the JavaScreen release
#  script.
#
#  Revision 1.14.18.3  2013/11/27 20:17:14  wenger
#  JS signing:  Changed JS version to 5.14.3x_sign1; temporarily changed
#  release.js to release things to JavaScreen.signed for testing,
#  distribution gets stuff from there, too.
#
#  Revision 1.14.18.2  2013/11/26 19:34:22  wenger
#  JS signing:  Release and install scripts now warn if jar file isn't
#  signed.
#
#  Revision 1.14.18.1  2013/11/05 22:07:49  wenger
#  JS signing:  Changed the JS to use a single jar file that includes
#  the Jmol classes.
#
#  Revision 1.14  2008/03/06 18:37:19  wenger
#  Added jsj to JavaScreen release.
#
#  Revision 1.13  2008/02/21 23:08:35  wenger
#  Added new JavaScreen/html directory to JavaScreen release script.
#
#  Revision 1.12  2007/05/14 18:26:32  wenger
#  Fixed JavaScreen release script for new UI directory.
#
#  Revision 1.11  2007/02/20 00:00:37  wenger
#  Changed JavaScreen distribution tarfile to have JavaScreen version
#  instead of DEVise version; minor cleanup to distribution scripts.
#
#  Revision 1.10  2005/12/06 20:08:44  wenger
#  Merged V1_7b0_br_4 thru V1_7b0_br_5 to trunk.  (This should
#  be the end of the V1_7b0_br branch.)
#
#  Revision 1.9  2002/07/19 17:07:54  wenger
#  Merged V1_7b0_br_2 thru V1_7b0_br_3 to trunk.
#
#  Revision 1.8  2002/06/17 19:41:46  wenger
#  Merged V1_7b0_br_1 thru V1_7b0_br_2 to trunk.
#
#  Revision 1.7.4.5  2005/06/10 21:22:45  wenger
#  More configuration and install doc updates.
#
#  Revision 1.7.4.4  2003/09/23 22:31:24  wenger
#  Modification dates of class files are now preserved.
#
#  Revision 1.7.4.3  2003/06/20 18:06:55  wenger
#  Fixed this to release the new Xvfb scripts.
#
#  Revision 1.7.4.2  2002/06/26 17:29:42  wenger
#  Improved various error messages and client debug log messages; very
#  minor bug fixes; js_log script is now part of installation.
#
#  Revision 1.7.4.1  2002/05/08 16:09:26  wenger
#  Added collaboration follower page; changed html targets from "new" to
#  "_blank" so we get the desired behavior; added link from collab help
#  page to collab follower page.
#
#  Revision 1.7  2001/12/03 18:20:43  wenger
#  Added JavaScreen collaboration instructions; added standard DEVise
#  background to web pages.
#
#  Revision 1.6  2001/09/25 14:38:55  wenger
#  Modified all JS-related scripts, etc., to work without '.' in path;
#  fixed some problems in the JSS related to starting and killing deviseds.
#
#  Revision 1.5  2001/09/05 19:33:26  wenger
#  Added bin2 and dyn_lib directories in JavaScreen release and install,
#  and made corresponding changes to scripts; other distribution-related
#  changes.
#
#  Revision 1.4  2001/08/30 16:47:11  wenger
#  Various improvements to release and install scripts.
#
#  Revision 1.3  2001/08/29 17:39:44  wenger
#  Major changes to DEVise and JavaScreen release scripts.
#

############################################################

#-----------------------------------------------------------
# Get and check command-line arguments.

if ($#argv < 1) then
  echo "Usage: release.js <destination>"
  exit 1
endif

set src = Java/JavaScreen

set desttop = $1/release
set dest = $desttop/JavaScreen

#-----------------------------------------------------------
# Make sure we have the files we need to do the release.

set files = ($src/devise_js.jar $src/JavaScreen/jspop.class \
    $src/JavaScreen/jss.class $src/bin2/java $src/js_version)
foreach file ($files)
  if (! -f $file) then
    echo "File $file missing."
    echo "Cannot proceed with release procedure."
    exit 1
  endif
end

set signed = `cd Java/JavaScreen; ./js_is_signed.pl`
if (! $signed) then
  echo ""
  echo "Jar file is not signed"
  echo -n "Are you sure you want to continue? [n] "

  set answer = $<
  if ($answer != y && $answer != Y) then
    echo Aborted.
    exit 1
  endif
endif

#-----------------------------------------------------------
# Confirm that we want to go ahead.

echo ""
echo "You are about to release YOUR copy of DEVise JavaScreen to the"
echo "general public. The destination directory is:"
echo "  $dest"
echo ""
echo -n "Are you absolutely sure you want to do this? [N] "

set answer = $<
if ($answer != y && $answer != Y) then
  echo Aborted.
  exit 1
endif

echo ""

#-----------------------------------------------------------
# Create the top-level directories we need.

echo "Creating top-level directories"

if (! -d $desttop) then
  mkdir $desttop
endif

if (! -d $dest) then
  mkdir $dest
endif

#-----------------------------------------------------------
# Release the class files.

echo ""
echo "Releasing class files"
set classdir = $dest/JavaScreen
if (! -d $classdir) then
  mkdir $classdir
endif
if (! -d $classdir/UI) then
  mkdir $classdir/UI
endif
cp -p $src/JavaScreen/*.class $classdir
cp -p $src/JavaScreen/UI/*.class $classdir/UI

#-----------------------------------------------------------
# Release the jar files.

echo ""
echo "Releasing jar files"
cp $src/devise_js.jar $dest

#-----------------------------------------------------------
# Set up the bin directory.

echo ""
echo "Setting up bin directory"
pushd $dest >& /dev/null
if (! -e bin) then
  ln -s ../bin .
endif
popd >& /dev/null

#-----------------------------------------------------------
# Set up the bin2 directory.

pushd $src >& /dev/null
tar c --exclude=CVS bin2 | tar x --directory=$dest
popd >& /dev/null

#-----------------------------------------------------------
# Set up the dyn_lib directory.

pushd $src >& /dev/null
tar c --exclude=CVS dyn_lib | tar x --directory=$dest
popd >& /dev/null

#-----------------------------------------------------------
# Set up the lib directory.

echo ""
echo "Setting up lib directory"
pushd $dest >& /dev/null
if (! -e lib) then
  ln -s ../lib .
endif
popd >& /dev/null

#-----------------------------------------------------------
# Release various script files, etc.

echo ""
echo "Releasing misc. files"

pushd $src > /dev/null

set files = (check_jss restart_jss jss DEVise.kill jss.kill ports+files \
    DEVise.jspop DEVise.jspop_soil get_timestamp jss_savepid \
    check_connect check_jsall check_jspop jspop \
    js jsj js_cgi restart_jspop jspop.kill kill_jsall ports+files \
    get_timestamp run_top run_check jspop_savepid users.cfg install_js \
    Tasvir js_version js_is_signed.pl)
foreach file ($files)
  cp -p $file $dest
  chmod 755 $dest/$file
end

set files = (jsa.html jsb.html keyboard.html collab.html collabf.html)
foreach file ($files)
  cp -p $file $dest
  chmod 644 $dest/$file
end

popd >& /dev/null


if (! -e $dest/scripts) then
  mkdir $dest/scripts
endif
cp -p $src/scripts/[a-z]* $dest/scripts
cp -p $src/scripts/X* $dest/scripts
chmod 755 $dest/scripts/*


cp -p $src/Makefile.config.* $dest
cp -p $src/Makefile.setup $dest
chmod 644 $dest/Makefile*
cp -p $src/CONFIG.txt $dest

#-----------------------------------------------------------
# Release the CGI-related files.

set cgidest = $dest/cgi-bin

if (! -e $cgidest) then
  mkdir $cgidest
endif

set files = (js.cgi js_log)
foreach file ($files)
  cp -p $src/cgi-bin/$file $cgidest
  chmod 755 $cgidest/$file
end

set files = (.htaccess example_httpd_config)
foreach file ($files)
  cp -p $src/cgi-bin/$file $cgidest
  chmod 644 $cgidest/$file
end

#-----------------------------------------------------------
# Release the GIFs.

echo ""
echo "Releasing GIFs"
cp -p $src/*.gif $dest
chmod 644 $dest/*.gif

#-----------------------------------------------------------
# Release the html directory.
echo ""
echo "Releasing html directory"
mkdir -p $dest/html
chmod 755 $dest/html
cp -p $src/html/* $dest/html
chmod 644 $dest/html/*
mkdir -p $dest/html/feedback
cp -p $src/html/feedback/*[a-z] $dest/html/feedback
chmod 644 $dest/html/feedback/*


#-----------------------------------------------------------
# Release the cron entry files.

echo ""
echo "Releasing cron entry files"
cp -p $src/cron_entry.* $dest
chmod 644 $dest/cron_entry.*

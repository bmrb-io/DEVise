#! /bin/csh -f

#  ========================================================================
#  DEVise Data Visualization Software
#  (c) Copyright 2001
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

set files = ($src/jsa1.jar $src/JavaScreen/jspop.class \
    $src/JavaScreen/jss.class)
foreach file ($files)
  if (! -f $file) then
    echo "File $file missing."
    echo "Cannot proceed with release procedure."
    exit 1
  endif
end

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
cp $src/JavaScreen/*.class $classdir

#-----------------------------------------------------------
# Release the jar files.

echo ""
echo "Releasing jar files"
cp $src/*.jar $dest

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
    DEVise_savepid js.cgi check_connect check_jsall check_jspop jspop \
    js js_cgi restart_jspop jspop.kill kill_jsall ports+files get_stats \
    get_timestamp run_top run_check jspop_savepid users.cfg install_js \
    Tasvir)
foreach file ($files)
  cp -p $file $dest
  chmod 755 $dest/$file
end

set files = (jsa.html jsb.html)
foreach file ($files)
  cp -p $file $dest
  chmod 644 $dest/$file
end

popd >& /dev/null

#-----------------------------------------------------------
# Release the GIFs.

echo ""
echo "Releasing GIFs"
cp -p $src/*.gif $dest
chmod 644 $dest/*.gif

#-----------------------------------------------------------
# Release the cron entry files.

echo ""
echo "Releasing cron entry files"
cp -p $src/cron_entry.* $dest
chmod 644 $dest/cron_entry.*

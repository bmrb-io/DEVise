#! /bin/csh -f

#  ========================================================================
#  DEVise Data Visualization Software
#  (c) Copyright 1992-1998
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

############################################################

set files = "Java/JavaScreen/js.class"

set target = /p/devise/public

if ($#argv > 0) then
  set target = $1
endif

foreach file ($files)
  if ($file == "") then
    continue
  endif
  if (! -f $file) then
    echo File $file missing. Cannot proceed with release procedure.
    exit 1
  endif
end

echo You are about to release YOUR copy of the JavaScreen to the
echo general public. The target directory is $target.
echo ""
echo -n "Are you absolutely sure you want to do this? [N] "

set answer = $<
if ($answer != y && $answer != Y) then
  echo Aborted.
  exit 1
endif

echo ""
echo Executing release procedures...
sleep 1

if (! -d $target) then
  mkdir $target
endif
if (! -d $target/Java) then
  mkdir $target/Java
endif
if (! -d $target/Java/JavaScreen) then
  mkdir $target/Java/JavaScreen
endif

cp -p Java/JavaScreen/*.class $target/Java/JavaScreen


#TEMPTEMP -- release runjs script to /p/devise/run


echo ""
echo Done!

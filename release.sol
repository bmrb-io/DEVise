#! /bin/csh -f

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

# Release Intel/Solaris and SPARC/Solaris versions only.

############################################################

#  $Id$

#  $Log$
#  Revision 1.1  1997/03/19 18:44:06  wenger
#  Disabled RTree indices in DTE, got that to compile; fixed some compile
#  warnings; undid accidental(?) changes to devise run script; devise.etk
#  uses 'localhost' instead of '$HOSTNAME'; added Solaris-only release
#  script.
#

############################################################

set files = "solaris/generic/generic solsparc/generic/generic \
        solaris/multi/disp solsparc/multi/disp \
	lib/control.tk doc/userman.ps doc/sysman.ps \
	doc/Agreement doc/Copyright doc/Disclaimer"
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

echo You are about to release YOUR copy of Devise to the
echo general public. The target directory is $target.
echo ""
echo The release includes Solaris executables,
echo Tcl/Tk code, and user and other documentation.
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


echo ""
echo Releasing new copy of generic Devise executables...
if (! -d $target) then
  mkdir $target
endif

if (! -d $target/sunx86_54) then
  mkdir $target/sunx86_54
endif
if (! -f $target/sunx86_55) then
  ln -s sunx86_54 $target/sunx86_55
endif
ls -l solaris/generic/generic $target/sunx86_54/devise
cp -p solaris/generic/generic $target/sunx86_54/devise

if (! -d $target/sun4m_54) then
  mkdir $target/sun4m_54
endif
if (! -f $target/sun4m_55) then
  ln -s sun4m_54 $target/sun4m_55
endif
if (! -f $target/sun4x_55) then
  ln -s sun4m_55 $target/sun4x_55
endif
ls -l solsparc/generic/generic $target/sun4m_54/devise
cp -p solsparc/generic/generic $target/sun4m_54/devise

#ls -l aix/generic/generic $target/rs_aix32/devise
#cp -p aix/generic/generic $target/rs_aix32/devise


echo ""
echo Releasing new copy of PVM Devise executables...
ls -l solaris/multi/disp $target/sunx86_54/multi
cp -p solaris/multi/disp $target/sunx86_54/multi
ls -l solsparc/multi/disp $target/sun4m_54/multi
cp -p solsparc/multi/disp $target/sun4m_54/multi
#ls -l aix/multi/disp $target/rs_aix32/multi
#cp -p aix/multi/disp $target/rs_aix32/multi


echo ""
echo Releasing new Tcl/Tk code...
if (! -d $target/lib) then
  mkdir $target/lib
endif
cp -p lib/[a-z]* $target/lib
cp -p lib/[A-Z]*.* $target/lib


echo ""
echo Releasing run files...

set run_target = $target/../run

set rundirs = "$run_target/ $run_target/tmp $run_target/work"

foreach dir ($rundirs)
  if (! -d $dir) then
    mkdir $dir
  endif
end

set runfiles = "run_public/devise \
	run_public/deviseweb run_public/dumpgif.script \
	run_public/multi"

foreach file ($runfiles)
  if ($file == "") then
    continue
  endif
  if (! -f $file) then
    echo File $file missing. Cannot proceed with release procedure.
    exit 1
  else
    cp -p $file $run_target
  endif
end

set file = compustat.idx
if (-f $run_target/$file) then
  rm $run_target/$file
endif
ln -s /p/devise/dat/$file $run_target/$file

set file = crsp_dsm94.idx
if (-f $run_target/$file) then
  rm $run_target/$file
endif
ln -s /p/devise/dat/$file $run_target/$file

set file = devise.rc
if (-f $run_target/$file) then
  rm $run_target/$file
endif
ln -s $target/$file $run_target/$file

set file = dsi94.dat
if (-f $run_target/$file) then
  rm $run_target/$file
endif
ln -s /p/devise/dat/$file $run_target/$file

set file = issm.idx
if (-f $run_target/$file) then
  rm $run_target/$file
endif
ln -s /p/devise/dat/$file $run_target/$file


echo ""
echo Releasing documentation...
if (! -d $target/doc) then
  mkdir $target/doc
endif
cp -p doc/*.ps doc/*.txt $target/doc
cp -p doc/Agreement $target/doc
cp -p doc/Copyright $target/doc
cp -p doc/Disclaimer $target/doc


echo ""
echo Setting up other files and directories...
if (! -f $target/bin) then
  ln -s @sys $target/bin
endif

tar cv --exclude=CVS public | tar xv --directory=$target/..

cp run_public/devise $target/devisepub


echo ""
echo Done!

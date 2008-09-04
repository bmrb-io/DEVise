#! /bin/csh -f
############################################################

# Run the standard DEVise with the Peptide-CGI test data,
# sessions, etc. in this directory.

# $Id$

############################################################

limit coredumpsize 0
limit descriptors 256

set devpath = `which devise`
set devise_run = `dirname $devpath`
echo "DIAG devise_run: $devise_run"

setenv DEVISE_PROG	$devise_run/..
echo "DIAG DEVISE_PROG: $DEVISE_PROG"

source $devise_run/dev_prog.gen
#TEMPTEMP
setenv DEVISE_MONO_EXEC    /p/condor/workspaces/shared/wenger/devise/devise_release/linux_rhel5/generic/generic
setenv DEVISE_LIB /p/condor/workspaces/shared/wenger/devise/devise_release/lib
source ./dev_vis.pep
echo "DIAG DEVISE_MONO_EXEC: $DEVISE_MONO_EXEC"

source $devise_run/start_mono

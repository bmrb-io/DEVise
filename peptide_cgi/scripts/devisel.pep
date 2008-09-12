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

setenv DEVISE_PROG	$devise_run/..

source $devise_run/dev_prog.gen
setenv DEVISE_MONO_EXEC ../../linux_rhel5/generic/generic
setenv DEVISE_LIB ../../lib

source ./dev_vis.pep

source $devise_run/start_mono

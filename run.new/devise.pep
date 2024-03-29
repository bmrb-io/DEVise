#! /bin/csh -f
############################################################

# Run the standard DEVise with the Peptide-CGI test data,
# sessions, etc.

# $Id$

############################################################

limit coredumpsize 0
limit descriptors 256

set devise_run = `dirname $0`

source $devise_run/dev_prog.std
source $devise_run/dev_vis.pep

source $devise_run/start_mono

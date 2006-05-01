############################################################

# Define the environment variables needed to run DEVise with
# the Peptide-CGI test data, sessions, etc.

# $Id$

############################################################

setenv DEVISE_VIS    /scratch/wenger/peptide_cgi_test/peptide_cgi/dev_test

source $DEVISE_PROG/run/dev_vis.gen

# Special cases.
setenv DEVISE_SCHEMA	$DEVISE_VIS/schema
setenv DEVISE_HOME_TABLE	$DEVISE_SCHEMA/catalog.dte
setenv DEVISE_CACHE	$DEVISE_PROG/cache
setenv DEVISE_PALETTE	$DEVISE_PROG/public/palette

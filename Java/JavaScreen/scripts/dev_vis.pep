############################################################

# Define the environment variables needed to run DEVise with
# the Peptide-CGI dev_test setup.

# $Id$

############################################################

pushd ../../..
set tmpdir = `pwd`
popd
setenv DEVISE_VIS    $tmpdir/peptide_cgi/dev_test

source scripts/dev_vis.gen

# Special cases.
setenv DEVISE_SCHEMA  $DEVISE_VIS/schema
setenv DEVISE_HOME_TABLE        $DEVISE_SCHEMA/catalog.dte

setenv DEVISE_CACHE   $DEVISE_VIS/cache
setenv DEVISE_PALETTE   $DEVISE_VIS/palette


#! /usr/bin/env perl
#  ========================================================================
#  DEVise Data Visualization Software
#  (c) Copyright 2014
#  By the DEVise Development Group
#  Madison, Wisconsin
#  All Rights Reserved.
#  ========================================================================
#
#  Under no circumstances is this software to be copied, distributed,
#  or altered in any way without prior permission from the DEVise
#  Development Group.

# One-step visualization script.

############################################################
#
#  $Id$
#
############################################################

use strict;

my $version = "0.9.0";

if ($ARGV[0] eq "-usage") {
	print "Usage:\n";
	print "    bmrb_vis.pl <nmr-star file>\n";
	print "    bmrb_vis.pl -usage\n";
	print "    bmrb_vis.pl -v\n";
	exit(0);
} elsif ($ARGV[0] eq "-v") {
	print "$version\n";
	exit(0);
}

my $starfile = $ARGV[0];

if (! -e $starfile) {
	die "Specified NMR-STAR file $starfile does not exist!\n";
}

print "Visualizing $starfile\n";

#TEMP -- do http post; print url; pass url to browser



# TEMP: Info about http post in Perl
# http://www.perl.com/pub/2002/08/20/perlandlwp.html
# http://stackoverflow.com/questions/11264470/how-to-post-content-with-an-http-request-perl

# http://manatee.bmrb.wisc.edu/vis_serv/srv.shtml
#
# Kent, the files are in /website/htdocs/vis_serv: srv.shtml and srv.php.
#
# You need to POST the file, input name is "fileupl", that's all.

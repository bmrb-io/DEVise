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

#TEMPTEMP -- test sticking html into browser
#TEMPTEMP -- test what happens w/ bad data

use strict;

my $version = "0.9.1";

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

my $url = "http://manatee.bmrb.wisc.edu/vis_serv/srv.php";

my $command = "curl --form fileupl=@" . $starfile . " " . $url;
my $response = `$command`;

print "DIAG response: $response\n";

# TEMP: okay, content here is html; if we want a browser to actually
# call this script, as opposed to using the script as a test, I guess we
# send back the URL as opposed to the HTML.

# TEMP: Info about http post in Perl
# http://www.perl.com/pub/2002/08/20/perlandlwp.html
# http://stackoverflow.com/questions/11264470/how-to-post-content-with-an-http-request-perl

# Opening a browser in Perl
# http://stackoverflow.com/questions/8867262/how-do-i-launch-the-default-web-browser-in-perl-on-any-operating-system

# http://manatee.bmrb.wisc.edu/vis_serv/srv.shtml
#
# Kent, the files are in /website/htdocs/vis_serv: srv.shtml and srv.php.
#
# You need to POST the file, input name is "fileupl", that's all.

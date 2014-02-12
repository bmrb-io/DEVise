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

my $version = "0.9.2";
my $starfile;
my $browser = "firefox"; # default

my $arg = shift @ARGV;

if (!defined $arg || $arg eq "-usage") {
	print "Usage:\n";
	print "    bmrb_vis.pl [-b <browser>] <nmr-star file>\n";
	print "    bmrb_vis.pl -usage\n";
	print "    bmrb_vis.pl -v\n";
	exit(0);
} elsif ($arg eq "-v") {
	print "$version\n";
	exit(0);
} elsif ($arg eq "-b") {
	$arg = shift @ARGV;
	if (!defined $arg) {
		print "-b flag needs value!\n";
		exit(1);
	}
	$browser = $arg;
	$starfile = shift @ARGV;
} else {
	$starfile = $arg;
}

if (!defined $starfile) {
	printf "No NMR-STAR file specified!\n";
	exit(1);
}

if (! -e $starfile) {
	die "Specified NMR-STAR file $starfile does not exist!\n";
}

print "Visualizing $starfile\n";

my $url = "http://manatee.bmrb.wisc.edu/vis_serv/srv.php";
my $command = "curl --form fileupl=@" . $starfile . " " . $url;
my $response = `$command`;

my $page;
if ($response =~ /.*Location:\s*(\S*).*/) {
	$page = $1;
} else {
	die "Didn't find Location in response: $response\n";
}

print "Visualization page: <$page>; paste this URL into your browser if the page is not shown automatically\n";

#TEMPTEMP -- do I need "&"?
# Need to create command this way instead of as a single string to
# bypass the shell (which gets goofed up by special characters in the
# URL).
my @command = ($browser, $page, "&");
my $result = system(@command);

if ($result != 0) {
	print "Error sending web page to browser $browser\n";
}

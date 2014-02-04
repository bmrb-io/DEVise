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
use LWP;

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

my $file_contents;
{
	local $/=undef;
	open FILE, "$starfile" or die "Couldn't open file: $!";
	$file_contents = <FILE>;
	close FILE;
}

# print "DIAG file_contents: $file_contents";#TEMP

#TEMP -- do http post; print url; pass url to browser


my $browser = LWP::UserAgent->new;
# This is for testing.
my $url = "http://manatee.bmrb.wisc.edu/vis_serv/srv.shtml";

my $response = $browser->post($url,
	[
        'fileupl' => $file_contents,
	]
);

if (! $response->is_success) {
	print "POST failed\n";
	my $foo = $response->status_line;
	print "status_line: $foo\n";
	die "ERROR";
}
my $bar = $response->content_type;
print "DIAG content_type: $bar\n";
$bar = $response->message;
print "DIAG message: $bar\n";
$bar = $response->content;
print "DIAG content: $bar\n";
$bar = $response->headers;
print "DIAG headers: $bar\n";

# TEMP: okay, content here is html; if we want a browser to actually
# call this script, as opposed to using the script as a test, I guess we
# send back the URL as opposed to the HTML.

# TEMP: Info about http post in Perl
# http://www.perl.com/pub/2002/08/20/perlandlwp.html
# http://stackoverflow.com/questions/11264470/how-to-post-content-with-an-http-request-perl

# http://manatee.bmrb.wisc.edu/vis_serv/srv.shtml
#
# Kent, the files are in /website/htdocs/vis_serv: srv.shtml and srv.php.
#
# You need to POST the file, input name is "fileupl", that's all.

#! /usr/bin/env perl

#  ========================================================================
#  DEVise Data Visualization Software
#  (c) Copyright 2011
#  By the DEVise Development Group
#  Madison, Wisconsin
#  All Rights Reserved.
#  ========================================================================
#
#  Under no circumstances is this software to be copied, distributed,
#  or altered in any way without prior permission from the DEVise
#  Development Group.

#  Script for testing the "external process" capability -- concatenates
#  two arguments.

############################################################

#  $Id$

############################################################

my $result = shift(@ARGV) . shift(@ARGV);
print "$result\n";

#! /s/perl/bin/perl

#  ========================================================================
#  DEVise Data Visualization Software
#  (c) Copyright 2000
#  By the DEVise Development Group
#  Madison, Wisconsin
#  All Rights Reserved.
#  ========================================================================
#
#  Under no circumstances is this software to be copied, distributed,
#  or altered in any way without prior permission from the DEVise
#  Development Group.

#  This script takes Condor data, already split by split_user_data,
#  and creates a DEVise session file that displays
#  each user's data as a view symbol.

#  Add all known variables to a bond table.

############################################################

#  $Id$

############################################################

die "usage: add_vars.pl <variable file> <bond file> <bond+var file>\n"
  if ($#ARGV != 2);

$debug = 0;

$var_file = shift(@ARGV);
$bond_file = shift(@ARGV);
$bondvar_file = shift(@ARGV);

#
# Read in the variable value info.
#
open(IN, $var_file) or die "Couldn't open file $var_file: $!\n";

# List of variable values by residue number.
@vararray = ();

while ($inline = <IN>) {
  chomp($inline);
  print "inline = $inline\n" if ($debug);

  # Check for comment line.
  # Note: what if # isn't the first char on the line?
  $firstchar = substr($inline, 0, 1);
  if ($firstchar ne "#") {
    
    # Split into separate arguments.
    my @tmpargs = split(" ", $inline);

    $vararray[$tmpargs[2]] = $inline;
  }
}
close(IN);


# Read in bonds and add appropriate variables.
open(IN, $bond_file) or die "Couldn't open file $bond_file: $!\n";
open(OUT, ">$bondvar_file") or die "Couldn't open file $bondvar_file: $!\n";

while ($inline = <IN>) {
  chomp($inline);

  # Check for blank and comment lines.
  # Note: what if # isn't the first char on the line?
  $firstchar = substr($inline, 0, 1);
  if ($inline ne "" and $firstchar ne "#") {
    print "inline = $inline\n" if ($debug);

    my @bondvals = split("	", $inline);
    $resnum = $bondvals[2];
    my @varvals = split(" ", $vararray[$resnum]);

    print OUT "@bondvals[0..8] @bondvals[16..19] @varvals[12..37]\n";


  }
}
close(OUT);
close(IN);

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

# Build a full bond table (backbone and side chains) for a protein.

############################################################

#  $Id$

############################################################

die "usage: build_bonds.pl <connection file> <atom file> <bond file>\n"
  if ($#ARGV != 2);

$debug = 0;

$conn_file = shift(@ARGV);
$atom_file = shift(@ARGV);
$bond_file = shift(@ARGV);

#
# Read in the connection info.
#
open(IN, $conn_file) or die "Couldn't open file $conn_file: $!\n";

# List of connections by amino acid abbreviation ({acid}[bondnum][atomnum]).
%connhash = ();

# List of bonds by amino acid number ([acidnum][bondnum][atomnum]).
@bondarray = ();

$lastacid = "";
$acidnum = -1;
$bondnum = 0;
while ($inline = <IN>) {
  chomp($inline);
  print "inline = $inline\n" if ($debug);

  # Check for comment line.
  # Note: what if # isn't the first char on the line?
  $firstchar = substr($inline, 0, 1);
  if ($firstchar ne "#") {
    
    # Split into separate arguments.
    my @tmpargs = split(",", $inline);
    $lastarg = $#tmpargs;
    if ($lastarg < 3) {
      print "Error: too few arguments\n";
    } else {
      $thisacid = $tmpargs[1];
      if ($thisacid ne $lastacid) {
        EndOfAcidConn();
      }

      for ($atom = 3; $atom <= $lastarg; $atom++) {
        $bondarray[$acidnum][$bondnum][0] = $tmpargs[2];
        $bondarray[$acidnum][$bondnum][1] = $tmpargs[$atom];
	$bondnum++;
      }
    }
  }
}
EndOfAcidConn();
close(OUT);
close(IN);


#
# Read in the atom list.
#
open(IN, $atom_file) or die "Couldn't open file $atom_file: $!\n";
open(OUT, ">$bond_file") or die "Couldn't open file $bond_file: $!\n";

%atoms = ();
$lastresnum = 0;
$lastacid = "";
$lastC = "";
while ($inline = <IN>) {
  chomp($inline);
  print "inline = $inline\n" if ($debug);

  my @tmpargs = split(" +",$inline);
  $thisresnum = $tmpargs[2];
  $thisacid = $tmpargs[3];
  if ($lastresnum ne $thisresnum) {
    EndOfAcidAtoms();
  }
  $atomname = $tmpargs[4];
  $atoms{$atomname} = $inline;
}
close(IN);

EndOfAcidAtoms();




sub EndOfAcidConn {
  if ($lastacid ne "") {
    print "End of amino acid $lastacid ($acidnum)\n" if ($debug);
    $connhash{$lastacid} = \@{$bondarray[$acidnum]};
  }
  $bondnum = 0;
  $lastacid = $thisacid;
  $acidnum++;
}

sub EndOfAcidAtoms {
  if ($lastacid ne "") {
    print "\nEnd of amino acid $lastacid\n" if ($debug);
    OutputBonds();
  }
  $lastacid = $thisacid;
  $lastresnum = $thisresnum;
  $lastC = $atoms{C};
  %atoms = ();
}

sub OutputBonds {
  print OUT "\n# $lastacid (residue $lastresnum)\n";

  #TEMPTEMP -- get this to work with a reference?
  #@conns = \@{$connhash{$lastacid}};
  @conns = @{$connhash{$lastacid}};
  #$conns = \@{$connhash{$lastacid}};

  $lastbond = $#conns;
  for ($bondnum = 0; $bondnum <= $lastbond; $bondnum++) {
    $atom0name = $conns[$bondnum][0];
    $atom1name = $conns[$bondnum][1];

    if (!$atoms{$atom0name}) {
      print STDERR "Atom $atom0name is missing in residue $lastresnum\n";
    } elsif (!$atoms{$atom1name}) {
      print STDERR "Atom $atom1name is missing in residue $lastresnum\n";
    } else {
      $atom0line = $atoms{$atom0name};
      $atom1line = $atoms{$atom1name};
      ConnectAtoms();
    }
  }

  # Connect to the previous residue.
  if ($lastC) {
    print OUT "\n# Connect to previous residue.\n";

    $atom0line = $lastC;
    $atom1line = $atoms{N};
    ConnectAtoms();

    $atom0line = $atoms{N};
    $atom1line = $lastC;
    ConnectAtoms();
  }
}

sub ConnectAtoms {
  @info0 = split(" +", $atom0line);
  $x0 = $info0[6];
  $y0 = $info0[7];
  $z0 = $info0[8];

  @info1 = split(" +", $atom1line);
  $x1 = $info1[6];
  $y1 = $info1[7];
  $z1 = $info1[8];

  $xdiff = ($x1-$x0)/2.0;
  $ydiff = ($y1-$y0)/2.0;
  $zdiff = ($z1-$z0)/2.0;

  $totaldiff = sqrt($xdiff * $xdiff + $ydiff * $ydiff + $zdiff * $zdiff);
  if ($totaldiff > 5.0) {
    print STDERR "Bond it too long ($totaldiff) at $info0[4],$info1[4] in res $lastresnum\n";
  } else {
    # This is so both halves of a given bond have the same structure
    # type.
    $struct_type = "backbone";
    if ($info0[16] eq "side_proton" or $info1[16] eq "side_proton") {
      $struct_type = "side_proton";
    } elsif ($info0[16] eq "side_heavy" or $info1[16] eq "side_heavy") {
      $struct_type = "side_heavy";
    }

    for ($index = 0; $index < 16; $index++) {
      print OUT "$info0[$index]	";
    }
    print OUT "$struct_type	$xdiff  $ydiff  $zdiff\n";
  }
}

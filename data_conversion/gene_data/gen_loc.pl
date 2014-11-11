#! /usr/bin/env perl

# Add location and direction to Data_for_scatter.txt.

print "# Gene_name A1 M1 P1 A2 M2 P2 Direction Location\n";

# Consume header line
$line = <STDIN>;
chomp($line);

# "Normal" lines
$direction = 1;
$location = 1;
while (<STDIN>) {
	chomp();
	$line = $_;
	print "$line\t$direction\t$location\n";
	if ($direction gt 1) {
		$direction = 1;
		$location++;
	} else {
		$direction++;
	}
}

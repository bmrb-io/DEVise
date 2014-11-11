#! /usr/bin/env perl

print "# Gene 3 views\n";
print "# Gene A1 A2 B1 B2 C1 C2\n";

for ($gene = 1; $gene <= 250; $gene++) {
	$a1 = rand(100);
	$a2 = rand(100);
	$b1 = rand(1);
	$b2 = rand(1);
	$c1 = rand(10000);
	$c2 = rand(10000);
	print "G$gene $a1 $a2 $b1 $b2 $c1 $c2\n";
}

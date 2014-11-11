#! /usr/bin/env perl

# Not totally random, but more random than version 2.

print "# Gene Sample Expression Transcription\n";

# $prev = rand(20)-10;

for ($gene = 1; $gene <= 50; $gene++) {
	$prev = rand(20)-10;
	for ($sample = 1; $sample <= 50; $sample++) {
		$exp = $prev + rand(8) - 4;
		if ($exp > 9.99) { $exp = 9.99; }
		if ($exp < -9.99) { $exp = -9.99; }
		$prev = $exp;
		$trans = rand(100);
		print "$gene\t$sample\t$exp\t$trans\n";
	}
}

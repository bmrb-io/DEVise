#! /usr/bin/env perl

print "# Gene Sample Expression Transcription\n";

for ($gene = 1; $gene <= 50; $gene++) {
	for ($sample = 1; $sample <= 50; $sample++) {
		$exp = rand(20)-10;
		$trans = rand(100);
		print "$gene\t$sample\t$exp\t$trans\n";
	}
}

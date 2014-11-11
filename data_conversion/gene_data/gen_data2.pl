#! /usr/bin/env perl

# Make things not random -- maybe more interesting?

print "# Gene Sample Expression Transcription\n";

for ($gene = 1; $gene <= 50; $gene++) {
	for ($sample = 1; $sample <= 50; $sample++) {
		$exp = sin($sample/2)*9 + rand(2)-1;
		$trans = rand(100);
		print "$gene\t$sample\t$exp\t$trans\n";
	}
}

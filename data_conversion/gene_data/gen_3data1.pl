#! /usr/bin/env perl

print "# Line Column Gene_description Gene_ID Transcript_ID Field Value\n";

# Header line
$line = <STDIN>;
chomp($line);
@hdr_fields = split /\t/, $line;

# "Normal" lines
$lineno = 1;
while (<STDIN>) {
	chomp();
	$line = $_;
	@fields = split /\t/, $line;
	$gene_desc = $fields[0];
	$gene_id = $fields[1];
	$trans_id = $fields[2];

	$column = 1;
	$index = 3;
	for ($index = 3; $index <= $#fields; $index++) {
		print "$lineno	$column	$gene_desc	$gene_id	$trans_id	$hdr_fields[$index]	$fields[$index]\n";
		$column++;
	}
	$lineno++;
}

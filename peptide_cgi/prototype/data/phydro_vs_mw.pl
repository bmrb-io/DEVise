#! /usr/bin/perl

$outfile = "phydro_vs_mw.dat";
open (DAT, ">$outfile") or die "Can't open $outfile: $!";

print DAT "# Acc_num; mol_wt; hydrophobicity; spect_freq\n";

@freq_table = ( "500 MHz", "600 MHz", "700 MHz", "900 MHz" );

for ($an = 1; $an <= 16000; $an++) {
	$hydro = rand() * 4.0 - 2.0;
	if ($hydro > 0.0) {
		$hydro = $hydro * $hydro;
	} else {
		$hydro = -$hydro * $hydro;
	}
	$mol_wt = 200.0 + rand() * 10000.0;
	$freq = $freq_table[rand() * 4];
	print DAT "$an	$mol_wt	$hydro	$freq\n";
}

close(DAT);

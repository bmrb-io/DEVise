#!/s/std/bin/perl 

if (@ARGV == 0) {
	print "Usage : convert.pl <filename> <wildchar> ....\n\n";
	exit(1);
}
for ($i = 0 ; $i < @ARGV; $i++) {
	$temfile = $ARGV[$i] ;
	if ($temfile =~ /\*/) {
		@files = glob($temfile);
	} else {
		@files = $temfile;
	}

	foreach $unifile (@files) {
		@tmpf = split(/\./,$unifile);
		if ($tmpf[@tmpf-1] eq "uni") {
			$tmpf[@tmpf-1] = "ddr";
		} else {
			$tmpf[@tmpf] = "ddr";
		}
		$ddrfile = join("\.",@tmpf);
		$res = parse($unifile,$ddrfile);
		if ($res <0) {
			print "Parse Error : $unifile CAN'T BE CONVERTED TO $ddrfile \n\n";
			unlink($ddrfile);
		} else {
			print "***** $unifile CONVERTED TO $ddrfile *****\n\n";
		}
	}
}

sub parse {
	my($ufile,$dfile) = @_;

	open(INP,$ufile);
	open(OUT,">" . $dfile);
	$retval = 1;
	$begin = "";
	$quote = 0;
	$length = 0;
	$maxlen = 0;
	$attr = 0;
	$separator = 0;

	while ($line = <INP>) {
		$line =~ s/[ ]+$//g;
		$line =~ s/^[ ]+//g;
		$line =~ s/^[\t]+//g;
		if ((substr($line,0,1) eq "#") || (substr($line,0,1) eq "\\\\")) {
			print OUT $begin,$line;
			next;
		}
		if ($line eq "\n") {
			print OUT $begin,$line;
			next;
		}
		if (substr($line,0,2) eq "\\*") {
			print OUT $begin,$line;
			while ($line = <INP>) {
				print OUT $begin,$line;
				if ($line =~ /\*\\/) {
					last;
				}
			}
		}

		if ($line =~ /schema/) {
			print OUT $begin,$line;
			$begin .= "\t";
			next;
		}

		if ($line =~ /attr/) {
			print OUT $begin,$line;
			$begin .= "\t";
			$attr = 1;
			next;
		}
		
		if ($line =~ /}/) {
			chop($begin);
			print OUT $begin,$line;
			$quote = 0;
			$length = 0;
			$maxlen = 0;
			$attr = 0;
			$separator = 0;
			next;
		}

		if ($line =~ /type/) {
			if ($line =~ /binary/) {
				print "Binary Type files are not supported in DataReader\n";
				$retval = -1;
				last;
			}

			if ($line =~ /text/) {
				print "Type is not supported in DataReader, ignoring type\n";
				next;
			}

			if ($line =~ /ascii/) {
				print "Type is not supported in DataReader, ignoring type\n";
				next;
			}

			if ($line =~ /float/) {
				$line =~ s/float/double/g ;
				print OUT $begin,$line;
				next;
			}
			if ($line =~ /unixtime/) {
				print "Unix Time is not supported in DataReader\n";
				$retval = -1;
				last;
			}

			print OUT $begin,$line;
			next;
		}

		if ($line =~/char_set/) {
			print "char_set is not needed in DataReader, ignoring recsize\n";
			next;
		}

		if ($line =~ /binary_order/) {
			print "Binary Type files are not supported in DataReader\n";
			$retval = -1;
			last;
		}

		if ($line =~ /recsize/) {
			print "Record size is not needed in DataReader, ignoring recsize\n";
			next;
		}

		if ($line =~ /delimiter/) {
			$i1 = index($line, "\'");
			$i2 = rindex($line, "\'");
			$delimiter = "delimiter = [" . substr($line,$i1+1,$i2-$i1-1) . "]+ ;\n";
			print OUT $begin,$delimiter;
			next;
		}

		if ($line =~ /comment/) {
			print "comment is not supported in DataReader, ignoring comment\n";
			next;
		}

		if ($line =~ /converter/) {
			print "converter is not supported in DataReader\n";
			$retval = -1;
			last;
		}

		if ($line =~ /separator/) {
			print "separator is not supported in UniData, ignoring Separator\n";
			next;
		}

		if ($line =~ /whitespace/) {
			if (($length == 1) && ($attr == 1)) {
				print "Attribute can't have length & separator\n";
				$retval = -1;
				last;
			}
			$separator = 1;
			$i1 = index($line, "\'");
			$i2 = rindex($line, "\'");
			$separator = "separator = [" . substr($line,$i1+1,$i2-$i1-1) . "]+ ;\n";
			print OUT $begin,$separator;
			next;
		}

		if ($line =~ /maxlen/) {
			if ($length == 1) {
				print "Attribute can't have length & maxlen\n";
				$retval = -1;
				last;
			}
			$maxlen = 1;
			print OUT $begin,$line;
			next;
		}

		if ($line =~ /quote/) {
			if (($length == 1) && ($attr == 1)) {
				print "Attribute can't have length & quote\n";
				$retval = -1;
				last;
			}

			$quote = 1;
			$i1 = index($line, "\'");
			$i2 = rindex($line, "\'");
			$quote = "quote = [" . substr($line,$i1+1,$i2-$i1-1) . "] ;\n";
			print OUT $begin,$quote;
			next;
		}

		if ($line =~ /date_format/) {
			$i1 = index($line, "\'");
			$i2 = rindex($line, "\'");
			$tmps = substr($line,$i1+1,$i2-$i1-1) ;
			$tmpr = "";
			$isok = 1;
			for ($j = 0; $j < length($tmps); $j++) {
				$c1 = substr($tmps,$j,1);
				$c2 = substr($tmps,$j+1,1);

				if ($c1 ne "%") {
					$tmpr .= $c1;
					next;
				} else {
					$j++;
					if ($c2 eq "a") {
						print "Week names are not supported in DataReader\n";
						$isok = -1;
						last;
					} elsif ($c2 eq "A") {
						print "Week names are not supported in DataReader\n";
						$isok = -1;
						last;
					} elsif ($c2 eq "b") {
						$tmpr .= "MON";
					} elsif ($c2 eq "B") {
						$tmpr .= "MONTH";
					} elsif ($c2 eq "d") {
						$tmpr .= "DD";
					} elsif ($c2 eq "H") {
						$tmpr .= "hh";
					} elsif ($c2 eq "I") {
						$tmpr .= "hhP";
					} elsif ($c2 eq "j") {
						print "Day of the year is not supported in DataReader\n";
						$isok = -1;
						last;
					} elsif ($c2 eq "m") {
						$tmpr .= "MM";
					} elsif ($c2 eq "M") {
						$tmpr .= "mm";
					} elsif ($c2 eq "P") {
						$tmpr .= "P";
					} elsif ($c2 eq "S") {
						$tmpr .= "ss";
					} elsif ($c2 eq "U") {
						print "Week Number of the year is not supported in DataReader\n";
						$isok = -1;
						last;
					} elsif ($c2 eq "w") {
						print "Week Number of the year is not supported in DataReader\n";
						$isok = -1;
						last;
					} elsif ($c2 eq "W") {
						print "Week Number of the year is not supported in DataReader\n";
						$isok = -1;
						last;
					} elsif ($c2 eq "y") {
						$tmpr .= "YY";
					} elsif ($c2 eq "Y") {
						$tmpr .= "YYYY";
					} elsif ($c2 eq "z") {
						print "Time Zone Abr. is not supported in DataReader\n";
						$isok = -1;
						last;
					} elsif ($c2 eq "%") {
						$tmpr .= "%";
					} else {
						print "$c2 ??? This date format is not supported in UniData\n";
						$isok = -1;
						last;
					}
				}
			}
			if ($isok < 0) {
				$retval = -1;
				last;
			}
			print OUT $begin,"date_format = \"" . $tmpr . "\";\n";
			next;
		}

		if ($line =~ /position/) {
			$i1 = index($line, "[");
			$i2 = rindex($line, "]");
			$tmps = substr($line,$i1+1,$i2-$i1-1) ;
			@temar1 = split(/\.\./,$tmps);
			if (($temar1[0] eq "") || ($temar1[1] eq "")) {
				print "Position is not supported in DataReader, for converting position to length, both righ & left position must be specified\n";
				$retval = -1;
				last;

			}
			if (($maxlen== 1) || ($quote == 1) || ($separator == 1)){
				print "Attribute can't have length (position) & [quote|separator|maxlen]\n";
				$retval = -1;
				last;
			}

			$length = 1;
			print "Positions may cause some problems...\n";
			print OUT $begin,"length = " . ($temar1[1] - $temar1[0] + 1) . ";\n";
			next;
		}

		if ($line =~ /length/) {
			if ($line =~ /\,/) {
				print "Multi-Field length specification is not supported in DataReader\n";
				$retval = -1;
				last;
			}
			if (($maxlen== 1) || ($quote == 1) || ($separator == 1)){
				print "Attribute can't have length & [quote|separator|maxlen]\n";
				$retval = -1;
				last;
			}

			print OUT $begin,$line;
			next;
		}

		if ($line =~ /min/) {
			print "Min is not supported in DataReader\n";
			$retval = -1;
			last;
		}

		if ($line =~ /max/) {
			print "Max is not supported in DataReader\n";
			$retval = -1;
			last;
		}

		if ($line =~ /sorted/) {
			print "sorted is not supported in DataReader\n";
			$retval = -1;
			last;
		}

		if ($line =~ /unique/) {
			print "unique is not supported in DataReader\n";
			$retval = -1;
			last;
		}

		if ($line =~ /consecutive/) {
			print "consecutive is not supported in DataReader\n";
			$retval = -1;
			last;
		}

		if ($line =~ /null_allowed/) {
			print "null_allowed is not supported in DataReader\n";
			$retval = -1;
			last;
		}

		if ($line =~ /format/) {
			print "format is not supported in DataReader\n";
			$retval = -1;
			last;
		}

		if ($line =~ /value/) {
			print "value is not supported in DataReader\n";
			$retval = -1;
			last;
		}

		if ($line =~ /filter/) {
			print "filter is not supported in DataReader\n";
			$retval = -1;
			last;
		}

		if ($line =~ /reader/) {
			print "reader is not supported in DataReader\n";
			$retval = -1;
			last;
		}

		if ($line =~ /global/) {
			print "global is not supported in DataReader\n";
			$retval = -1;
			last;
		}

		if ($line =~ /function_of/) {
			print "function_of is not supported in DataReader\n";
			$retval = -1;
			last;
		}

		if ($line =~ /list_of/) {
			print "list_of is not supported in DataReader\n";
			$retval = -1;
			last;
		}

		if ($line =~ /typedef/) {
			print "typdef is not supported in DataReader\n";
			$retval = -1;
			last;
		}

		if ($line =~ /enum/) {
			print "enum is not supported in DataReader\n";
			$retval = -1;
			last;
		}

		chop($line);
		print "$line is not supported in UniData & DataReader\n";
		$retval = -1;
		last;

	}
	close (INP);
	close (OUT);
	return $retval;
}

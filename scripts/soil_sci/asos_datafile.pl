#!/s/std/bin/perl

$asosdir = "/p/devise/ftp/soil_data";
$asosdat = "/p/devise/twilson2/devise/dat/asos_data/zp/stn_data";
$homedir = "/p/devise/twilson2/devise/dat/asos_data/perl_program";

$year = 2000000;
$yr = $year;

# Current DOY
$day_of_year = (localtime(time()))[7];
#
open(ASOSLIST, "$homedir/asos.list");
while ($line = <ASOSLIST>) {
  chop ($line);
  ($stn, $stnlat, $stnlon) = split (/\s+/, $line, 3);
  $minstn[$i] = $stn;
  $lat[$i] = $stnlat;
  $lon[$i] = $stnlon;
  $i++;
  $nstn = $i - 1;
  $n[$i] = 0;
}
#
for ($i=0; $i<=$nstn; $i++) {
#     print("$i  $lat[$i]  $lon[$i] $minstn[$i]\n");
}

chdir ($asosdir);
#for ($doy=$day_of_year; $doy<=$day_of_year; $doy++) {
for ($doy=$day_of_year) {
  $yyddd = $yr + $doy;
  $file = "WI_HR.$yyddd";
  open (INFILE, "$file");
  $line = <INFILE>;
  $line = <INFILE>;
  while ($line = <INFILE>) {
    $yearddd  = substr($line,0,9);
    $hour   = substr($line,10,9);
    $hhmmss = substr($line,20,9);
    $xstn   = substr($line,30,4);
    $xlat   = substr($line,35,9);
    $xlon   = substr($line,45,9);
    $td     = substr($line,55,9);
    $ta     = substr($line,65,9);
    $wind   = substr($line,75,9);
    $pres   = substr($line,85,9);
    $prec   = substr($line,95,9);
    $wdir   = substr($line,105,9);
    for ($i=0; $i<=$nstn; $i++) {
      if ($xstn eq $minstn[$i]) {
        $n[$i]++;
        $timloc = $hhmmss/10000.;
        $doy = $yearddd - $year;
        &AWSTime;
#
#        $dectime{$i,$n[$i]} = $yearddd + $hhmmss/240000.;
        if ($ta!=0 && $td!=0) {
          $ea   = 6.108*10.0**(7.5*($td-273.15)/(237.15+$td-273.15));
          $esat = 6.108*10.0**(7.5*($ta-273.15)/(237.15+$ta-273.15));
          $rh{$i,$n[$i]} = $ea/$esat*100;
        } else {
          $rh{$i,$n[$i]} = -9999;
        }
	$eao{$i,$n[$i]} = $ea;
	$preco{$i,$n[$i]} = $prec;
        $tao{$i,$n[$i]} = $ta - 273.15;
        $tdo{$i,$n[$i]} = $td - 273.15;
        $windo{$i,$n[$i]} = $wind;
        $wdiro{$i,$n[$i]} = $wdir;
      }
    }
  }
  close (INFILE);
}

for ($i=0; $i<=$nstn; $i++) {
 open (STNFILE, ">>$asosdat/$minstn[$i]") || die ("$asosdat/$minstn[$i]");
  for ($j=1; $j<=$n[$i]; $j++) {  
    printf STNFILE ("%9s %9.0f %9.3f  %9.3f  %9.3f  %9.3f  %9.3f  %9.3f  %9.3f\n",
              $yyddd{$i,$j},$hhmm{$i,$j},$rh{$i,$j},$eao{$i,$j},$tao{$i,$j},
	      $tdo{$i,$j},$preco{$i,$j},$windo{$i,$j},$wdiro{$i,$j});
  }
  close (STNFILE)
}

#---------------------------------------------------------------------------
#							            AWS_TIME
sub AWSTime {

  $mm = $timloc % 1.0;
  $hh = $timloc - $mm;
  $hhmm{$i,$n[$i]} = $hh*100. + $mm*60.;
  $hhmm{$i,$n[$i]} = $hhmm{$i,$n[$i]} % 2400;
  $ddd = $doy;
  if ($hhmm{$i,$n[$i]} == 0.0) {
    $hhmm{$i,$n[$i]} = 2400;
    $ddd--;
  }
  $yyddd{$i,$n[$i]} = $yr + $ddd;

  if ($hhmm{$i,$n[$i]} > 600.0) {
    if($hhmm{$i,$n[$i]} >= 2340.0) {
      if($hhmm{$i,$n[$i]} < 2400.0) {
        if($yyddd{$i,$n[$i]} > $yyddd{$i-1,$n[$i-1]}) {
          $yyddd{$i,$n[$i]} = $yyddd{$i,$n[$i]} - 1.0;
	  if($yyddd{$i,$n[$i]} == 2000000) {
	    $yyddd{$i,$n[$i]} = 1999365;
          }
        }
      }
    }
    $hhmm{$i,$n[$i]} = $hhmm{$i,$n[$i]} - 600.0;
  } else { 
    $hhmm{$i,$n[$i]} = $hhmm{$i,$n[$i]} + 600.0 + 1200.0;
    $yyddd{$i,$n[$i]} = $yyddd{$i,$n[$i]} - 1.0;
    if($yyddd{$i,$n[$i]} == 2000000) {
       $yyddd{$i,$n[$i]} = 1999365;
    }
  }
  $dy = $yyddd{$i,$n[$i]} - $yr;
  if($yyddd{$i,$n[$i]} >= $yr) {
    $yy = "00";
  }
#
  if($dy < 10) {
    $dy = "00$dy";
  } elsif($dy >= 10 && $dy < 100) {
    $dy = "0$dy";
  } elsif($dy >= 100) {
    $dy = $dy;
  }
  $yyddd{$i,$n[$i]} = $yy.$dy;
#  print("$yyddd{$i,$n[$i]}  $hhmm{$i,$n[$i]}\n");
}

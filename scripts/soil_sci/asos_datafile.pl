#!/s/std/bin/perl

############################################################

# Perl script to update ASOS data files.

# $Id$

############################################################

die "usage: asos_datafile.pl [year day_of_year]\n" if ($#ARGV != -1 and $#ARGV != 1);

my ($year, $day_of_year);

if ($#ARGV == 1) {
  $year = shift(@ARGV);
  $day_of_year = shift(@ARGV);
} else {
  $day_of_year = -1; # invalid
}


$asosdir = "/p/devise/ftp/soil_data";
$asosdat = "/local.pumori/devise/dat/Asos";
$homedir = "/local.pumori/devise/dat/Asos/scripts";


#
# Get the info we need to figure out the filename we should open for the
# most recent data.
#

if ($day_of_year == -1) {
  # Seconds in a day.
  my $day = 60 * 60 * 24;

  # The file we get today has yesterday's data in it, so go back one day.
  @timeinfo = localtime(time() - $day);

  $year = $timeinfo[5] + 1900;

  # Days in filename go from 1 - 366; localtime returns 0 - 365.
  $day_of_year = $timeinfo[7] + 1;
}

my $yr = $year * 1000;


#
# Get a list of all stations.
#
open(ASOSLIST, "$homedir/asos.list") or
    die "Can't open $homedir/asos.list $!\n";

$stnNum = 0;
while ($line = <ASOSLIST>) {
  chop ($line);
  ($stn, $stnlat, $stnlon) = split (/\s+/, $line, 3);
  $minstn[$stnNum] = $stn; #TEMP -- why "minstn"???
  $lat[$stnNum] = $stnlat;
  $lon[$stnNum] = $stnlon;
  $stnNum++;
  $recCt[$stnNum] = 0; # Number of data records for this station.
}
$stnCt = $stnNum - 1; # Number of stations.

#
for ($stnNum=0; $stnNum<=$stnCt; $stnNum++) {
#     print("$stnNum  $lat[$stnNum]  $lon[$stnNum] $minstn[$stnNum]\n");
}


chdir ($asosdir);
#for ($doy=$day_of_year; $doy<=$day_of_year; $doy++) {
for ($doy=$day_of_year) {
  my $datestr = $yr + $doy;
  $file = "WI_HR.$datestr";

  open (INFILE, "$file") or die "Can't open $file: $!\n";
  $line = <INFILE>;
  $line = <INFILE>;
  while ($line = <INFILE>) {
    $yearddd  = substr($line,0,9);
    # Not used. $hour   = substr($line,10,9);
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
    for ($stnNum=0; $stnNum<=$stnCt; $stnNum++) {
      if ($xstn eq $minstn[$stnNum]) {
        $recCt[$stnNum]++;
	#TEMP -- we shouldn't be using yr here, because that comes from
	# the current date, not the date in the input file
        $doy = $yearddd - $yr;
        &AWSTime;
#
#        $dectime{$stnNum,$recCt[$stnNum]} = $yearddd + $hhmmss/240000.;
        if ($ta!=0 && $td!=0) {
          $ea   = 6.108*10.0**(7.5*($td-273.15)/(237.15+$td-273.15));
          $esat = 6.108*10.0**(7.5*($ta-273.15)/(237.15+$ta-273.15));
          $rh{$stnNum,$recCt[$stnNum]} = $ea/$esat*100;
        } else {
          $rh{$stnNum,$recCt[$stnNum]} = -9999;
        }
	$eao{$stnNum,$recCt[$stnNum]} = $ea;
	$preco{$stnNum,$recCt[$stnNum]} = $prec;
        $tao{$stnNum,$recCt[$stnNum]} = $ta - 273.15;
        $tdo{$stnNum,$recCt[$stnNum]} = $td - 273.15;
        $windo{$stnNum,$recCt[$stnNum]} = $wind;
        $wdiro{$stnNum,$recCt[$stnNum]} = $wdir;
      }
    }
  }
  close (INFILE);
}

for ($stnNum=0; $stnNum<=$stnCt; $stnNum++) {
  open (STNFILE, ">>$asosdat/$minstn[$stnNum]") or
     die "Can't open $asosdat/$minstn[$stnNum]: $!";
  for ($j=1; $j<=$recCt[$stnNum]; $j++) {  
    printf STNFILE ("%9s %9.0f %9.3f  %9.3f  %9.3f  %9.3f  %9.3f  %9.3f  %9.3f\n",
              $yyddd{$stnNum,$j},$hhmm{$stnNum,$j},$rh{$stnNum,$j},$eao{$stnNum,$j},$tao{$stnNum,$j},
	      $tdo{$stnNum,$j},$preco{$stnNum,$j},$windo{$stnNum,$j},$wdiro{$stnNum,$j});
  }
  close (STNFILE)
}

#---------------------------------------------------------------------------
#							            AWS_TIME
sub AWSTime {

  $hhmm{$stnNum,$recCt[$stnNum]} = $hhmmss / 100;
  $hhmm{$stnNum,$recCt[$stnNum]} = $hhmm{$stnNum,$recCt[$stnNum]} % 2400;
  $ddd = $doy;
  if ($hhmm{$stnNum,$recCt[$stnNum]} == 0.0) {
    $hhmm{$stnNum,$recCt[$stnNum]} = 2400;
    $ddd--;
  }
  $yyddd{$stnNum,$recCt[$stnNum]} = $yr + $ddd;

  #TEMP -- what the hell is this?  some kind of attempt to convert from 
  # GMT to local time???
  if ($hhmm{$stnNum,$recCt[$stnNum]} > 600.0) {
    if($hhmm{$stnNum,$recCt[$stnNum]} >= 2340.0) {
      if($hhmm{$stnNum,$recCt[$stnNum]} < 2400.0) {
        if($yyddd{$stnNum,$recCt[$stnNum]} > $yyddd{$stnNum-1,$recCt[$stnNum-1]}) {
          $yyddd{$stnNum,$recCt[$stnNum]} = $yyddd{$stnNum,$recCt[$stnNum]} - 1.0;
	  #TEMP -- this is not general!!!
	  if($yyddd{$stnNum,$recCt[$stnNum]} == 2000000) {
	    $yyddd{$stnNum,$recCt[$stnNum]} = 1999365;
          } elsif($yyddd{$stnNum,$recCt[$stnNum]} == 2001000) {
	    $yyddd{$stnNum,$recCt[$stnNum]} = 2000366;
	  }
        }
      }
    }
    $hhmm{$stnNum,$recCt[$stnNum]} = $hhmm{$stnNum,$recCt[$stnNum]} - 600.0;
  } else { 
    $hhmm{$stnNum,$recCt[$stnNum]} = $hhmm{$stnNum,$recCt[$stnNum]} + 600.0 + 1200.0;
    $yyddd{$stnNum,$recCt[$stnNum]} = $yyddd{$stnNum,$recCt[$stnNum]} - 1.0;
    #TEMP -- this is not general!!!
    if($yyddd{$stnNum,$recCt[$stnNum]} == 2000000) {
       $yyddd{$stnNum,$recCt[$stnNum]} = 1999365;
    } elsif($yyddd{$stnNum,$recCt[$stnNum]} == 2001000) {
       $yyddd{$stnNum,$recCt[$stnNum]} = 2000366;
    }
  }

  # Change $yyddd{$stnNum,$recCt[$stnNum] from int to string, cut off
  # the 1st. 2 chars (e.g., 2001002 becomes "01002").
  my $tmpstr = sprintf("%7d", $yyddd{$stnNum,$recCt[$stnNum]});
  $yyddd{$stnNum,$recCt[$stnNum]} = substr($tmpstr, 2);

#  print("$yyddd{$stnNum,$recCt[$stnNum]}  $hhmm{$stnNum,$recCt[$stnNum]}\n");
}

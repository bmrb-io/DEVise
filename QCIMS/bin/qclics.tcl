#!/bin/sh
# next line restarts using tcl \
exec tclsh "$0" "$@"

set compextn ".jpg"

set tempdir "/tmp"
set qfilename "$tempdir/rdopt.Q" 

set bppscale 5000
set numpoints 50 
set threshspan 25
set method lagrangian 
set doavgsize 1
set docorrect 0
set cbppdef 1.0

# objective function
set bppgiven 1
set selbpp 1.0
set totsizegiven 0
set totsize 10000
set selqual 35.0

# constraints 
set bppmin 0
set bppmax 16
set psnrmin 0
set psnrmax 100

proc ExitProc {} {
  global numimages numpoints qfilename ImName
  for {set n 0} {$n < $numimages} {incr n 1} {
    for {set i 0} {$i < $numpoints} {incr i 1} {
      exec /bin/rm $qfilename.$ImName($n).$i
    }
  }
  exit
}

set paramFileName [lindex $argv 0]

if {[string compare $paramFileName ""] == 0} {
  puts "No parameter file given"
  exit
} 

set paramFile [open $paramFileName r]

set srcdir ""
set targetdir ""

while {[gets $paramFile paramLine] >= 0} {
  if {[string compare $paramLine ""]} {
    scan $paramLine "%s%s" paramName paramVal
    if {[string compare $paramName "SRC_DIR"] == 0} {
      set srcdir $paramVal
    } elseif {[string compare $paramName "TARGET_DIR"] == 0} {
      set targetdir $paramVal
    } elseif {[string compare $paramName "CORRECT"] == 0} {
      set docorrect 1
    } elseif {[string compare $paramName "BPP_SCALE"] == 0} {
      scan  $paramVal "%d" bppscale
    } elseif {[string compare $paramName "BPP_MAX"] == 0} {
      scan  $paramVal "%lf" bppmax
    } elseif {[string compare $paramName "BPP_MIN"] == 0} {
      scan  $paramVal "%lf" bppmin 
    } elseif {[string compare $paramName "PSNR_MIN"] == 0} {
      scan  $paramVal "%lf" psnrmin 
    } elseif {[string compare $paramName "PSNR_MAX"] == 0} {
      scan  $paramVal "%lf" psnrmax 
    } elseif {[string compare $paramName "THRESH_SPAN"] == 0} {
      scan  $paramVal "%d" threshspan
    } elseif {[string compare $paramName "AVG_SIZE"] == 0} {
      set doavgsize 1
    } elseif {[string compare $paramName "AVG_BPP"] == 0} {
      set doavgsize 0
    } elseif {[string compare $paramName "SEL_PSNR"] == 0} {
      scan  $paramVal "%lf" selqual
      set bppgiven 0
    } elseif {[string compare $paramName "SEL_BPP"] == 0} {
      scan  $paramVal "%lf" selbpp
      set totsizegiven 0
      set bppgiven 1
    } elseif {[string compare $paramName "TOTAL_SIZE"] == 0} {
      scan  $paramVal "%d" totsize
      set totsizegiven 1
      set bppgiven 1
    } elseif {[string compare $paramName "NUM_POINTS"] == 0} {
      scan  $paramVal "%d" numpoints
    } elseif {[string compare $paramName "METHOD"] == 0} {
      if {[string match "d*" $paramVal]} {
	set method dynamic
      } else {
	set method lagrangian
      } 
    }
  }
}
close $paramFile

if {([string compare srcdir ""]==0) || ([string compare targetdir ""]==0)} {
  puts "Parameter file must have SRC_DIR and TARGET_DIR"
  exit
}



proc RdOptImage {imnum} { 
  global bppscale bppmax threshspan bppgiven totsizegiven \
	 bppmin psnrmin psnrmax selqual selbpp \
	 totsize numpoints srcdir targetdir \
	 qfilename compextn \
	 ImPixels ImWidth ImHeight ImPlanes TotalNumPixels \
	 ImName ImBpp ImPsnr method \
	 Stats docorrect cbpp




  if {$ImPlanes($imnum) == 1} {
    if {$bppmax > 8.0} {
      set tbppmax 8.0
    } else {
      set tbppmax $bppmax
    }
  } else { 
    if {$bppmax > 16.0} {
      set tbppmax 16.0
    } else {
      set tbppmax $bppmax
    }
  } 



  set cmd "|/usr/bin/time rdopt -bppscale $bppscale -bppmax $tbppmax \
    -method $method \
    -numtables $ImPlanes($imnum) -bppplane 0 \
    -errfile - -thresh $threshspan"
  
  if {$docorrect == 1} {
    set cmd "$cmd -correct $cbpp"
  }

  set cmd "$cmd -im $srcdir/$ImName($imnum) -rgbto2ycc"


  puts "    Rdopting $ImName($imnum)"

  set ifp [open $cmd r+]

  gets $ifp line;# image line


  gets $ifp line
  scan $line "%s%s%s%lf%s%s%lf%s" foo1 foo2 foo3 minbpp \
	      minpsnrs foo4 maxbpp maxpsnrs
  scan $minpsnrs "(%lf)" minpsnr
  scan $maxpsnrs "(%lf)" maxpsnr
  
  if {$minpsnr > $psnrmin} {
    set tminp $minpsnr
  } else {
    set tminp $psnrmin
  }
  if {$minbpp > $bppmin} {
    set tminb $minbpp 
  } else {
    set tminb $bppmin 
  } 

  if {$maxpsnr < $psnrmax} {
    set tmaxp $maxpsnr
  } else {
    set tmaxp $psnrmax
  }
  if {$maxbpp < $tbppmax} {
    set tmaxb $maxbpp 
  } else {
    set tmaxb $tbppmax 
  } 


  gets $ifp line
  gets $ifp line
  gets $ifp line

  while {[read $ifp 1] != { }} {};




  # get 0th table

  puts $ifp "qfile $qfilename.$ImName($imnum).0"
  flush $ifp
  while {[read $ifp 1] != { }} {};
  puts $ifp "bpp $tminb"
  flush $ifp
  while {[read $ifp 1] != { }} {};
  set qf [open $qfilename.$ImName($imnum).0 r]
  while {[gets $qf line] >= 0} {
    if {[string match "*Bpp =*" $line]} {
      scan $line "%s%s%lf" foo1 foo2 ImBpp($imnum,0)
    }
    if {[string match "*PSNR =*" $line]} {
      scan $line "%s%s%lf" foo1 foo2 ImPsnr($imnum,0)
    }
  }
  close $qf 
  
  puts $ifp "qfile $qfilename.$ImName($imnum).0.temp"
  flush $ifp
  while {[read $ifp 1] != { }} {};
  puts $ifp "psnr $tminp"
  flush $ifp
  while {[read $ifp 1] != { }} {};
  set qf [open $qfilename.$ImName($imnum).0.temp r]
  while {[gets $qf line] >= 0} {
    if {[string match "*Bpp =*" $line]} {
      scan $line "%s%s%lf" foo1 foo2 tempb
    }
    if {[string match "*PSNR =*" $line]} {
      scan $line "%s%s%lf" foo1 foo2 tempp
    }
  }
  close $qf 

  if {$tempb > $ImBpp($imnum,0)} {
    exec /bin/mv $qfilename.$ImName($imnum).0.temp $qfilename.$ImName($imnum).0
    set ImBpp($imnum,0) $tempb
    set ImPsnr($imnum,0) $tempp
  } else {
    exec /bin/rm $qfilename.$ImName($imnum).0.temp
  } 

  # get last table 
  set n [expr $numpoints - 1]

  puts $ifp "qfile $qfilename.$ImName($imnum).$n"
  flush $ifp
  while {[read $ifp 1] != { }} {};
  puts $ifp "bpp $tmaxb"
  flush $ifp
  while {[read $ifp 1] != { }} {};
  set qf [open $qfilename.$ImName($imnum).$n r]
  while {[gets $qf line] >= 0} {
    if {[string match "*Bpp =*" $line]} {
      scan $line "%s%s%lf" foo1 foo2 ImBpp($imnum,$n)
    }
    if {[string match "*PSNR =*" $line]} {
      scan $line "%s%s%lf" foo1 foo2 ImPsnr($imnum,$n)
    }
  }
  close $qf 

  puts $ifp "qfile $qfilename.$ImName($imnum).$n.temp"
  flush $ifp
  while {[read $ifp 1] != { }} {};
  puts $ifp "psnr $tmaxp"
  flush $ifp
  while {[read $ifp 1] != { }} {};
  set qf [open $qfilename.$ImName($imnum).$n.temp r]
  while {[gets $qf line] >= 0} {
    if {[string match "*Bpp =*" $line]} {
      scan $line "%s%s%lf" foo1 foo2 tempb
    }
    if {[string match "*PSNR =*" $line]} {
      scan $line "%s%s%lf" foo1 foo2 tempp
    }
  } 
  close $qf 

  if {$tempb < $ImBpp($imnum,$n)} {
    exec /bin/mv $qfilename.$ImName($imnum).$n.temp $qfilename.$ImName($imnum).$n
    set ImBpp($imnum,$n) $tempb
    set ImPsnr($imnum,$n) $tempp
  } else {
    exec /bin/rm $qfilename.$ImName($imnum).$n.temp
  } 

  # get remaining R-D points
  if {$numpoints > 2} {
    set bincr [expr (0.0 + $ImBpp($imnum,$n) - $ImBpp($imnum,0))/(0.0 + $n)]
    set tempb $ImBpp($imnum,0)
    for {set i 1} {$i < $n} {incr i 1} {
      set tempb [expr 0.0 + $tempb + $bincr]
      puts $ifp "qfile $qfilename.$ImName($imnum).$i"
      flush $ifp
      while {[read $ifp 1] != { }} {};
      puts $ifp "bpp $tempb"
      flush $ifp
      while {[read $ifp 1] != { }} {};
      set qf [open $qfilename.$ImName($imnum).$i r]
      while {[gets $qf line] >= 0} {
        if {[string match "*Bpp =*" $line]} {
          scan $line "%s%s%lf" foo1 foo2 ImBpp($imnum,$i)
        }
        if {[string match "*PSNR =*" $line]} {
          scan $line "%s%s%lf" foo1 foo2 ImPsnr($imnum,$i)
        }
      }
      close $qf 
    }
  }

  puts $ifp quit
  catch "close $ifp" timestr

  scan $timestr "%s%lf%s%lf" foo1 foo2 foo3 t
  set Stats(rdoptTime) [expr $Stats(rdoptTime) + $t]

}



if {[catch {set ff [lsort [glob $srcdir/*]]}]} {
  puts "No images in directory"
  exit
}

set TotalNumPixels 0.0
set numimages 0

foreach im $ff {
  set fp [open "|ImIdentify $im" r]
  gets $fp id
  close $fp
  set imtail [file tail $im]
  if {[string match *P?M* $id]} {
    set ImName($numimages) $imtail
    scan $id "%s%s%s%s%s%d%s%d" foo1 colortag foo3 foo4 foo5 \
      ImWidth($numimages) foo6 ImHeight($numimages)
    if {[string compare $colortag PPM] == 0} {
      set ImPlanes($numimages) 3
    } else {
      set ImPlanes($numimages) 1
    }

    set ImPixels($numimages) [expr ((0.0+$ImWidth($numimages))* \
		 (0.0+$ImHeight($numimages)))]

    set TotalNumPixels [expr $TotalNumPixels + $ImPixels($numimages)] 
    incr numimages 1
  }
}


if {$numimages == 0} {
  puts "No images in directory"
  exit
} 

if {![file exists $targetdir]} { 
  exec mkdir $targetdir
}

if {$doavgsize == 1} { 
  for {set n 0} {$n < $numimages} {incr n 1} {
    set bppFactor($n) [expr (0.0 + $ImPixels($n))/(0.0 + $TotalNumPixels)]
    set qualFactor($n) [expr (1.0/(0.0 + $numimages))] 
  }
} else {
  for {set n 0} {$n < $numimages} {incr n 1} {
    set bppFactor($n) [expr (1.0/(0.0 + $numimages))] 
    set qualFactor($n) [expr (1.0/(0.0 + $numimages))] 
  }
}

set Stats(rdoptTime) 0.0
set Stats(combiningTime) 0.0
set Stats(compressTime) 0.0

if {$docorrect == 1} {
  if {$bppgiven == 0} {
    set cbpp $cbppdef
  } else {
    if {$totsizegiven == 1} {
      set cbpp [expr (0.0+($totsize*8.0))/$TotalNumPixels]
    } else {
      set cbpp $selbpp
    }
  }
}

puts "Running rdopt on images.."

for {set n 0} {$n < $numimages} {incr n 1} {
  RdOptImage $n 
}


puts "Combining rdopt results.."

set xyopt [open "|/usr/bin/time xyopt -xscale $bppscale -noprompt -v" r+]
puts $xyopt $numimages

for {set n 0} {$n < $numimages} {incr n 1} {
  puts $xyopt $numpoints
  for {set i 0} {$i < $numpoints} {incr i 1} {
    puts $xyopt "[expr $ImBpp($n,$i)*$bppFactor($n)] [expr $ImPsnr($n,$i)*$qualFactor($n)]"
  }
}

flush $xyopt

for {set n 0} {$n < $numimages} {incr n 1} {
  puts "    Incorporating $ImName($n)"
  gets $xyopt foo
}

# find target
if {$bppgiven == 0} {
  puts $xyopt "y $selqual"
} else {
  if {$totsizegiven == 0} {
    puts $xyopt "x $selbpp"
  } else {
    puts $xyopt "x [expr (0.0+$totsize)*8.0/$TotalNumPixels]"
  }
}
flush $xyopt

gets $xyopt ans
if {[string match "Infeas*" $ans]} {
  puts "Could not achieve target"
  ExitProc
}

scan $ans "%s%s%lf%s%s%lf" foo1 foo2 ansbpp foo3 foo4 anspsnr

gets $xyopt foo1
for {set n [expr $numimages - 1]} {$n >= 0} {incr n -1} {
  gets $xyopt ans
  scan $ans "%d" Choice($n)
}

catch "close $xyopt" timestr
scan $timestr "%s%lf%s%lf" foo1 foo2 foo3 Stats(combiningTime)

puts "Compressing.."

set ImRealTotalSize 0
set ImRealTotalQual 0.0
for {set n 0} {$n < $numimages} {incr n 1} {
  # compress
  puts "    Compressing $ImName($n)"
  if {$ImPlanes($n) == 1} {
    set fp [open "|/usr/bin/time cjpeg -optimize -dct float -grayscale \
                   -outfile $targetdir/$ImName($n)$compextn \
		   -qtables $qfilename.$ImName($n).$Choice($n) \
		   -thresh $qfilename.$ImName($n).$Choice($n) \
		   $srcdir/$ImName($n)" r]
  } else {
    set fp [open "|/usr/bin/time cjpeg -optimize -dct float \
                   -outfile $targetdir/$ImName($n)$compextn \
		   -qtables $qfilename.$ImName($n).$Choice($n) \
		   -thresh $qfilename.$ImName($n).$Choice($n) \
		   -qslots 0,1,2 \
		   $srcdir/$ImName($n)" r]
  } 
  catch "close $fp" timestr
  scan $timestr "%s%lf%s%lf" foo1 foo2 foo3 t
  set Stats(compressTime) [expr $Stats(compressTime) + $t]

  # get size
  set fp [open "|/bin/ls -l $targetdir/$ImName($n)$compextn" r]
  gets $fp ans
  catch "close $fp"
  scan $ans "%s%s%s%s%d" foo1 foo2 foo3 foo4 ImRealSize($n)
  set ImRealTotalSize [expr $ImRealTotalSize + $ImRealSize($n)]

  # get psnr
  set fp [open "|jpegqual $targetdir/$ImName($n)$compextn $srcdir/$ImName($n)" r]
  while {[gets $fp lans] >= 0} {set ans $lans};
  catch "close $fp"
  if {$ImPlanes($n) == 1} {
    scan $ans "%s%lf" foo1 ImRealPsnr($n)
  } else {
    scan $ans "%s%s%s%s%s%s%s%s%s%s%lf" foo1 foo2 foo3 \
                                        foo4 foo5 foo6 \
                                        foo7 foo8 foo9 \
                                        foo10 ImRealPsnr($n)
  }
  set ImRealTotalQual [expr $ImRealTotalQual + $ImRealPsnr($n)]
}

puts "Qclics on images in $srcdir: Results"
puts "------------------------------------"
puts "Target directory: $targetdir"
puts "Optimization parameters:"
puts "  RD-OPT method: $method"
puts "  BppScale: $bppscale ThreshSpan: $threshspan NumPoints: $numpoints"
if {$docorrect == 1} {
  puts "  Bpp correction was applied, using bpp of $cbpp as pivot"
}
puts "Objective function:"
if {$doavgsize == 1} {
  puts "  Averaging was done on sizes"
} else {
  puts "  Averaging was done on rates"
}
if {$bppgiven == 1} {
  if {$totsizegiven == 1} {
    puts "  Target total size was $totsize bytes"
  } else {
    puts "  Target total rate was $selbpp bits per pixel"
  }
} else {
  puts "  Target Psnr was $selqual dB"
}

puts "Constraints:"
puts "  BppMin: $bppmin BppMax: $bppmax"
puts "  PsnrMin: $psnrmin PsnrMax: $psnrmax"


puts [format "%-15s-%-15s-%-15s-%-15s" "---------------" \
                                       "---------------" \
                                       "---------------" \
                                       "---------------"]
puts [format "%-15s %-15s %-15s %-15s" "Image" \
		 "Rate (bpp)" "Size (bytes)" "Psnr (dB)"]
puts [format "%-15s %-7s %-7s %-7s %-7s %-7s %-7s" "" \
		 "Pred" "Actual" \
		 "Pred" "Actual" \
		 "Pred" "Actual"]
puts [format "%-15s-%-15s-%-15s-%-15s" "---------------" \
                                       "---------------" \
                                       "---------------" \
                                       "---------------"]
                 
for {set n 0} {$n < $numimages} {incr n 1} {
  puts [format "%-15s %-7.5lf %-7.5lf %-7d %-7d %-7.3lf %-7.3lf" \
     $ImName($n) \
     $ImBpp($n,$Choice($n)) \
     [expr (0.0+$ImRealSize($n))*8.0/$ImPixels($n)] \
     [expr round($ImBpp($n,$Choice($n))*$ImPixels($n)/8.0)] \
     $ImRealSize($n) \
     $ImPsnr($n,$Choice($n)) \
     $ImRealPsnr($n)]
}


puts [format "%-15s-%-15s-%-15s-%-15s" "---------------" \
                                       "---------------" \
                                       "---------------" \
                                       "---------------"]

puts [format "%-15s %-7.5lf %-7.5lf %-7d %-7d %-7.3lf %-7.3lf" \
     Overall \
     $ansbpp \
     [expr (0.0+$ImRealTotalSize)*8.0/$TotalNumPixels] \
     [expr round($ansbpp*$TotalNumPixels/8.0)] \
     $ImRealTotalSize \
     $anspsnr \
     [expr $ImRealTotalQual/$numimages]]

puts [format "%-15s-%-15s-%-15s-%-15s" "---------------" \
                                       "---------------" \
                                       "---------------" \
                                       "---------------"]

puts "Running Time:"
puts [format "%-10s %-20s %-15s %-15s" "Rdopt" "Combining images" "Compression" "Total"]
puts [format "%10.1lf%20.1lf%15.1lf%15.1lf" \
      $Stats(rdoptTime) \
      $Stats(combiningTime) \
      $Stats(compressTime) \
      [expr $Stats(rdoptTime)+$Stats(combiningTime)+$Stats(compressTime)]]

ExitProc



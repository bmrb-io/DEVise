set data style linespoints
set title "Devise with 150,000 Records"
set xlabel "version"
set offset 0.5, 0.5, 0, 0
set xtics 1,1

set logscale y
set ylabel "time (sec)"
set yrange [1:10000]
plot 'stats.150k' using "%*s%f%*f%*f%*f%f" title "exit", \
     'stats.150k' using "%*s%f%*f%*f%f" title "cache", \
     'stats.150k' using "%*s%f%*f%f" title "disp", \
     'stats.150k' using "%*s%f%f" title "index"
pause -1 "Press return to continue."
set terminal postscript eps
set output "times.150k.eps"
replot
set terminal x11

set logscale y
set yrange [1:350000]
set ylabel "seeks"
plot 'stats.150k' using "%*s%f%*f%*f%*f%*f%*f%f" title "fsr", \
     'stats.150k' using "%*s%f%*f%*f%*f%*f%*f%*f%f" title "bsr"
pause -1 "Press return to continue."
set terminal postscript eps
set output "calls.150k.eps"
replot
set terminal x11

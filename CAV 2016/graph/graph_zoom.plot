set terminal postscript eps
set output "zoomlines.eps"
set style line 1 lc rgb '#0060ad' lt 1 lw 2 pt 7 ps 1.5   # --- blue
set style line 2 lc rgb '#dd181f' lt 1 lw 2 pt 5 ps 1.5   # --- red
set xlabel "Size of the monoid"
set ylabel "Time (s)"
# set arrow from 3300,0 to 3300,500 nohead
plot "out_medium" using 2:3 title 'Acme++' with points ls 1,"out_medium" using 2:4 title 'AcmeML' with points ls 2

set terminal png
set output "ratios.png"
set xlabel "Size of the automaton"
set ylabel "Undecidable instances / Total instances"
set yrange [*:1]
plot "ratios.txt" u 1:2 w lp pt 0

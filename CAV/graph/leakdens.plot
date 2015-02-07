set xlabel "Size of the automaton"
set ylabel "Size of the monoid"

plot "<awk '{if($4==\"No\" && $3!=\"0\") print}' leakdens.txt" u 1:2 t "val?1 with leaks" w p pt 7,\
     "< awk '{if($4==\"Yes\") print}' leakdens.txt" u 1:2 t "val=1" w p pt 1, \
     "< awk '{if($4==\"No\" && $3==\"0\") print}' leakdens.txt" u 1:2 t "val<1 and leaktight" w p pt 2


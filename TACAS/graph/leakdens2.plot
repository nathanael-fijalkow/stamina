plot "<awk '{if($5==\"No\" && $4!=\"0\") print}' leakdens2.txt" u 1:6 t "val?1 with leaks" w p pt 7,\
     "< awk '{if($5==\"Yes\") print}' leakdens2.txt" u 1:6 t "val=1" w p pt 1, \
     "< awk '{if($5==\"No\" && $4==\"0\") print}' leakdens2.txt" u 1:6 t "val<1 and leaktight" w p pt 2


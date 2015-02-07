#!/usr/bin/awk -f
BEGIN {
    i = $1
    total = 0
    undec = 0
}{
    if(i!=$1) {
	print i,(undec/total)
	i=$1
	total = 0
	undec = 0
    }
    if($4=="No" && $3!="0")
	undec++
    total++
}
END {
    print i,(undec/total)
}

#set terminal postscript eps enhanced monochrome solid rounded
#set output "radius.eps"



set title "US immigration from Northern Europe\nPlot selected data columns as histogram of clustered boxes"
set auto x
set yrange [0:300000]
set style data histogram
set style histogram cluster gap 1
set style fill solid border -1
set boxwidth 0.9
set xtic rotate by -45
#set bmargin 10 
plot 'manyUsers.test' using ($12+$14+15):xtic(1) ti 'VL', '' u 16 ti col, '' u 13 ti col, '' u 14 ti col
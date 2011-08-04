# Gnuplot script file for plotting data in file "force.dat"
# This file is called   force.p
reset
#set key horizontal # synonym: horiz
set terminal postscript eps enhanced monochrome solid rounded
set output "querySize.eps"
#set   autoscale                        # scale axes automatically
set size 0.8,0.8			# scale whole graph, but not fonts
set border				# adds borders to graph
show border
#unset log                              # remove any log-scaling
unset label                            # remove any previous labels
set xtic auto                          # set xtics automatically
set ytic auto                          # set ytics automatically
set title "Cache Hitrate when increasing number of queries (cache 10% of map)"
set xlabel "Map Nodes" 25,0
set ylabel "Cache Hits" 0.5, 9
set key 500000,0.8
show xlabel
show ylabel
#set label "Yield Point" at 0.003,260
#set arrow from 0.0028,250 to 0.003,280
set xr [5:*]
#set yr [1:1100000]
set pointsize 1.3
set logscale x
set format x "10^{%L}" 

#set multiplot
#set origin 0.0,0.0
plot    "querySize-6-10.test" using 3:($14) title 'LRU - Imp' with linespoints pt 2 lw 2,\
"querySize-6-10.test" using 3:15 title 'OSC' with linespoints pt 6 lw 2,\
"querySize-6-10.test" using 3:($16) title 'FIFO' with linespoints pt 8 lw 2

#set origin 0.09,0.40
#set size 0.30,0.30
#set xr [0:11]
#set yr [0:1]
#unset title
#unset xlabel
#unset ylabel
#set ytic 0.2
#set xtic 2
#unset key
#unset label

#replot
#set nomultiplot
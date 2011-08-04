# Gnuplot script file for plotting data in file "force.dat"
# This file is called   force.p
reset
#set key horizontal # synonym: horiz
set terminal postscript eps enhanced monochrome solid rounded
set output "cacheTest.eps"
#set   autoscale                        # scale axes automatically
set size 0.8,0.8			# scale whole graph, but not fonts
set border				# adds borders to graph
show border
unset log                              # remove any log-scaling
unset label                            # remove any previous labels
set xtic auto                          # set xtics automatically
set ytic auto                          # set ytics automatically
set title "Cache Hitsrate when increasing cache size"
set xlabel "Cache Pct" 25,0
set ylabel "Cache Hitrate" 0.5, 9
set key 80,90
show xlabel
show ylabel
#set label "Yield Point" at 0.003,260
#set arrow from 0.0028,250 to 0.003,280
#set xr [0:13000]
set yr [0:100]
set pointsize 1.0

set multiplot
set origin 0.0,0.0
plot    "cacheTest-6-10.test" using 5:($14) title 'LRU - Imp' with linespoints pt 2 lw 2,\
"cacheTest-6-10.test" using 5:($15) title 'OSC' with linespoints pt 4 lw 2,\
"cacheTestNoHitScore-6-10.test" using 5:($15) title 'OSC - NodeScore' with linespoints pt 6 lw 2,\
"cacheTestNoNodeScore-6-10.test" using 5:($15) title 'OSC - HitScore' with linespoints pt 8 lw 2,\
"cacheTestNoNodeScore-6-10.test" using 5:($17) title 'FIFO' with linespoints pt 12 lw 2


set origin 0.09,0.40
set size 0.30,0.30
set xr [0:10]
set yr [0:1.1]
unset title
unset xlabel
unset ylabel
set ytic 0.2
set xtic 2
replot

set origin 0.09,0.13
set size 0.30,0.30
set xr [1:100]
set yr [-0.05:0.16]
set ytic 0.04
set xtic 20
set grid
plot "cacheTestNoNodeScore-6-10.test" using 5:($15-$16) title 'diff' with linespoints pt 14 lw 2

replot
set nomultiplot
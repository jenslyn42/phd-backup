# Gnuplot script file for plotting data in file "force.dat"
# This file is called   force.p
reset
#set key horizontal # synonym: horiz
set terminal postscript eps enhanced monochrome solid rounded
set output "rcache_plot37303-3200.eps"
#set   autoscale                        # scale axes automatically
set size 1.5,1.5			# scale whole graph, but not fonts
set border				# adds borders to graph
show border
unset log                              # remove any log-scaling
unset label                            # remove any previous labels
set log x
set xtic auto                          # set xtics automatically
set ytic auto                          # set ytics automatically
#set title "Cache Hitsrate when increasing cache size"
set xlabel "X"
set ylabel "Y"
set key left bottom
show xlabel
show ylabel
#set label "Yield Point" at 0.003,260
#set arrow from 0.0028,250 to 0.003,280
#set xr [1:10000]
#set yr [0:110]
set pointsize 1.0

plot    "rangeContent37303-3200.rcache" title 'Cache' with linespoints pt 2 lw 2

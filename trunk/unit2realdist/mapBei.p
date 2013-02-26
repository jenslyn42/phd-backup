# Gnuplot script file for plotting data in file "force.dat"
# This file is called   force.p
reset
#set key horizontal # synonym: horiz
set terminal postscript eps enhanced monochrome solid rounded
set output "mapBei.eps"
#set   autoscale                        # scale axes automatically
set size 2,2		# scale whole graph, but not fonts
set border				# adds borders to graph
show border
unset log                              # remove any log-scaling
unset label                            # remove any previous labels
#set log x
set xtic auto                          # set xtics automatically
set ytic auto                          # set ytics automatically
set grid xtics ytics
#set title "Cache Hitsrate when increasing cache size"
#set xlabel "Cache size"
#set ylabel "Time"
#set key left bottom
show xlabel
show ylabel
#set label "Yield Point" at 0.003,260
#set arrow from 0.0028,250 to 0.003,280
set xr [460330:460350]
set yr [4417750:4417770]
set pointsize 0.5

plot "beijingNEW.cnode" using 2:3 title 'bei' with points pt 7


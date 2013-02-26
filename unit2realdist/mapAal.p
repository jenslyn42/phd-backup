# Gnuplot script file for plotting data in file "force.dat"
# This file is called   force.p
reset
#set key horizontal # synonym: horiz
set terminal postscript eps enhanced monochrome solid rounded
set output "mapAal.eps"
#set   autoscale                        # scale axes automatically
set size 2,2		# scale whole graph, but not fonts
set border				# adds borders to graph
show border
unset log                              # remove any log-scaling
unset label                            # remove any previous labels
#set log x
#set xtic 2                          # set xtics automatically
#set ytic 2                          # set ytics automatically
#set grid xtics ytics
#set title "Cache Hitsrate when increasing cache size"
#set xlabel "Cache size"
#set ylabel "Time"
#set key left bottom
show xlabel
show ylabel
#set label "Yield Point" at 0.003,260
#set arrow from 0.0028,250 to 0.003,280
#set xr [558000:558050]
#set yr [6327600:6327650]
set pointsize 0.5

plot "aalborgNEW.cnode" using 2:3 title 'aal' with points pt 7


# Gnuplot script file for plotting data in file "force.dat"
# This file is called   force.p
reset
#set key horizontal # synonym: horiz
set terminal postscript eps enhanced monochrome solid rounded
set output "cRLSPVSP210000STATIC.eps"
#set   autoscale                        # scale axes automatically
set size 0.8,0.8			# scale whole graph, but not fonts
set border				# adds borders to graph
show border
unset log                              # remove any log-scaling
unset label                            # remove any previous labels
set xtic auto                          # set xtics automatically
set ytic auto                          # set ytics automatically
#set title "Cache Hitsrate when increasing cache size"
#set xlabel "Cache Pct" 25,0
#set ylabel "Cache Hitrate" 0.5, 9
#set key 80,90
#show xlabel
#show ylabel
#set label "Yield Point" at 0.003,260
#set arrow from 0.0028,250 to 0.003,280
#set xr [0:13000]
#set yr [0:100]
set pointsize 1.0

plot    "compRandLongStaticProbVsScoredPath210000STATIC.cache" using 1:2 with linespoints pt 2 lw 2

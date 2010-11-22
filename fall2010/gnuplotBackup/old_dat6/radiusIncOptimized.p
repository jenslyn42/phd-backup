# Gnuplot script file for plotting data in file "force.dat"
# This file is called   force.p
reset
set key horizontal # synonym: horiz
set terminal postscript eps enhanced monochrome solid rounded
set output "radiusIncOptimized.eps"
#set   autoscale                        # scale axes automatically
set size 0.6,0.6			# scale whole graph, but not fonts
#set border				# adds borders to graph
#show border
unset log                              # remove any log-scaling
unset label                            # remove any previous labels
set xtic auto                          # set xtics automatically
set ytic auto                          # set ytics automatically
set title "Granules sent for one user and timestamp"
set xlabel "Radius" 0,0.5
set ylabel "Granules" #2.0
set key 12800,70000
show xlabel
show ylabel
#set label "Yield Point" at 0.003,260
#set arrow from 0.0028,250 to 0.003,280
set xr [0:13000]
set logscale y
set format y "10^{%L}"
#set yr [1000:2200]
set pointsize 1.3

plot    "Radius.test" using 7:17 title 'Not Optimized' with linespoints ls 3,\
"RadiusRF_IU.test" using 7:17 title  'IU \& RF' with linespoints ls 6,\
"RadiusIncOptimized.test" using 7:17 title 'IU' with linespoints ls 8,\
"RadiusRF.test" using 7:17 title 'RF' with linespoints ls 4

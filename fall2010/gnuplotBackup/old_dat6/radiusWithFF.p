# Gnuplot script file for plotting data in file "force.dat"
# This file is called   force.p
reset
set key horizontal # synonym: horiz
#set terminal postscript eps enhanced monochrome solid rounded
#set output "radiusIncOptimized.eps"
#set   autoscale                        # scale axes automatically
set size 0.6,0.6			# scale whole graph, but not fonts
#set border				# adds borders to graph
#show border
unset log                              # remove any log-scaling
unset label                            # remove any previous labels
set xtic auto                          # set xtics automatically
set ytic auto                          # set ytics automatically
set title "Messages sent for one user and timestamp"
set xlabel "Radius" 0,0.5
set ylabel "Messages" 2.0
#set key 800,130
show xlabel
show ylabel
#set label "Yield Point" at 0.003,260
#set arrow from 0.0028,250 to 0.003,280
#set xr [0:5000]
#set yr [1000:2200]
set pointsize 1.3
set multiplot
set origin 0.0,0.0
plot    "Radius.test" using 7:($16+$19) title 'No optimization' with linespoints ls 1, \
"epsilonEffect.txt" using 6:2 title  'FriendLocator cells' with linespoints ls 6
#"RadiusIncOptimized.test" using 7:($16+$19) title 'Incremental Update' with linespoints ls 8,\
#"RadiusRF.test" using 7:($16+$19) title 'Roadnetwork Filter' with linespoints ls 4,\

set origin 0.075,0.18
set size 0.28,0.28
set xr [0:500]
set yr [0:4]
unset title
unset xlabel
unset ylabel
set ytic 40
set xtic 250
unset key
unset label

replot
set nomultiplot
# Gnuplot script file for plotting data in file "force.dat"
# This file is called   force.p
reset
set key horizontal # synonym: horiz
set terminal postscript eps enhanced monochrome solid rounded
set output "trajecAdjustTimeGraph.eps"
#set   autoscale                        # scale axes automatically
set size 0.5,0.5			# scale whole graph, but not fonts
#set border				# adds borders to graph
#show border
unset log                              # remove any log-scaling
unset label                            # remove any previous labels
set xtic auto                          # set xtics automatically
set ytic auto                          # set ytics automatically
#set title "Granules sent for one user and timestamp"
set xlabel "Time" 17,0
set ylabel "Dist." 0.5, 5.5
set key 17,38
show xlabel
show ylabel
#set label "Yield Point" at 0.003,260
#set arrow from 0.0028,250 to 0.003,280
#set xr [0:13000]
#set yr [1000:2200]
set pointsize 1.1

plot    "trajecAdjustTimeGraphData.test" using 1:2 title 'Time ajusted (A)' with linespoints pt 3 lw 4,\
"trajecAdjustTimeGraphData.test" using 3:4 title 'Not Time ajusted (B)' with linespoints pt 8 lw 4

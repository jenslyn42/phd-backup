# Gnuplot script file for plotting data in file "force.dat"
# This file is called   force.p
reset
#set key horizontal # synonym: horiz
set terminal postscript eps enhanced color solid rounded
set output "lmax.eps"
#set   autoscale                        # scale axes automatically
set size 0.6,0.6			# scale whole graph, but not fonts
#set border				# adds borders to graph
#show border
unset log                              # remove any log-scaling
unset label                            # remove any previous labels
set xtic auto                          # set xtics automatically
set ytic auto                          # set ytics automatically
set title "Granules sent for one user and timestamp"
set xlabel "L_{max}" 0,0.5
set ylabel "Granules" 2.0
#set key 20.5,1180
show xlabel
show ylabel
#set label "Yield Point" at 0.003,260
#set arrow from 0.0028,250 to 0.003,280
set xr [5:10]
#set yr [0:1500]
set pointsize 1.5

set multiplot
set origin 0.0,0.0

plot    "lmax.test" using 4:17 title 'No Optimization' with linespoints ls 2,\
"lmaxRF_IU.test" using 4:17 title 'IU \& RF' with linespoints ls 3,\
"lmaxIU.test" using 4:17 title 'IU' with linespoints ls 1,\
"lmaxRF.test" using 4:17 title 'RF' with linespoints




set origin 0.075,0.13
set size 0.35,0.32
set xr [3.8:9]
set yr [0:350]
unset title
unset xlabel
unset ylabel
set ytic 100
set xtic 1
unset key
unset label

replot
set nomultiplot
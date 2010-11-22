# Gnuplot script file for plotting data in file "force.dat"
# This file is called   force.p
reset
#set key horizontal # synonym: horiz
set terminal postscript eps enhanced color solid rounded
set output "radiusFFcompairMsg.eps"
set   autoscale                        # scale axes automatically
set size 0.6,0.6			# scale whole graph, but not fonts
#set border				# adds borders to graph
#show border
unset log                              # remove any log-scaling
unset label                            # remove any previous labels
set xtic auto                          # set xtics automatically
set ytic auto                          # set ytics automatically
set title "Messages sent for one user and timestamp"
set xlabel "Radius /& {/Symbol e}" 0,0.5
set ylabel "Messages" 2.0
set key 5900,6.3
show xlabel
show ylabel
#set label "Yield Point" at 0.003,260
#set arrow from 0.0028,250 to 0.003,280

set multiplot
set origin 0.0,0.0

set xr [0:13000]
#set yr [1000:2200]
set pointsize 1
plot    "RadiusFFcompair.test" using 7:($16+$18+$19) title 'VicinityLocator' with linespoints ls 1, \
"epsilonEffect.txt" using 6:2 title  'FriendLocator' with linespoints ls 3

set origin 0.295,0.10
set size 0.28,0.28
set xr [0:250]
#set yr [0:0.4]
unset title
unset xlabel
unset ylabel
set ytic 1
set xtic 50
unset key
unset label

replot
set nomultiplot
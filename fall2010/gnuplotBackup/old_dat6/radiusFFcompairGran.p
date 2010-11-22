# Gnuplot script file for plotting data in file "force.dat"
# This file is called   force.p
reset
set key horizontal # synonym: horiz
set terminal postscript eps enhanced color solid rounded
set output "radiusFFcompairGran.eps"
#set   autoscale                        # scale axes automatically
set size 0.6,0.6			# scale whole graph, but not fonts
#set border				# adds borders to graph
#show border
unset log                              # remove any log-scaling
unset label                            # remove any previous labels
set xtic auto                          # set xtics automatically
set ytic auto                          # set ytics automatically
set title "Granules sent for one user and timestamp"
set xlabel "Radius /& {/Symbol e}" 0,0.5
set ylabel "Granules" 2.0
#set key 800,130
show xlabel
show ylabel
#set label "Yield Point" at 0.003,260
#set arrow from 0.0028,250 to 0.003,280
set xr [0:13000]
#set yr [1000:2200]
set pointsize 1.3
set multiplot
set origin 0.0,0.0
plot    "RadiusFFcompairIU.test" using 7:17 title 'VicinityLocator w. IU' with linespoints ls 3,\
"RadiusFFcompair.test" using 7:17 title 'VicinityLocator' with linespoints ls 1, \
"FFclientEpsilon.txt" using 1:($2*4/2000000) title  'FriendLocator' with linespoints ls 2



set origin 0.075,0.18
set size 0.31,0.28
set xr [10:250]
set yr [0.2:16]
unset title
unset xlabel
unset ylabel
set ytic 4
set xtic 50
unset key
unset label

replot
set nomultiplot
# Gnuplot script file for plotting data in file "force.dat"
# This file is called   allMsgOneUser.p
reset
#set key horizontal # synonym: horiz
set terminal postscript eps enhanced monochrome solid rounded
set output "groupSize.eps"
set   autoscale                        # scale axes automatically
set size 0.65,0.65			# scale whole graph, but not fonts
set border				# adds borders to graph
show border
unset log                              # remove any log-scaling
unset label                            # remove any previous labels
set xtic auto				# set xtics automatically
set xtic 400                          
set ytic auto                          # set ytics automatically
set title "Proximity events, for one user and timestamp"
set xlabel "Number of users in the same group" #0,0.5
set ylabel "Proximity Events" #2.0
set key 1000,0.6
show xlabel
show ylabel
#set label "Yield Point" at 0.003,260
#set arrow from 0.0028,250 to 0.003,280
set xr [0:1050]
#set yr [0:2200]
set multiplot
set origin 0.0,0.0
plot "proxDymBase.txt" using 4:1 title 'FriendLocator' with linespoints ,\
"groupLmax5.test" using 3:19 title 'VicinityLocator,L_{max}=5' with linespoints,\
"groupLmax6.test" using 3:19 title 'VicinityLocator,L_{max}=6' with linespoints
#"proxDymBase.txt" using 4:2 title 'Basic -Min' with linespoints ls 5,\
#"proxDymBase.txt" using 4:3 title 'Basic -Max' with linespoints ls 8

set origin 0.075,0.25
set size 0.28,0.28
set xr [0:300]
set yr [0:0.4]
unset title
unset xlabel
unset ylabel
set ytic 0.3
set xtic 150
unset key
unset label

replot
set nomultiplot
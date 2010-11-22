# Gnuplot script file for plotting data in file "force.dat"
# This file is called   force.p
reset
set key horizontal # synonym: horiz
set terminal postscript eps enhanced monochrome solid rounded
set output "groupMembership.eps"
#set   autoscale                        # scale axes automatically
set size 0.6,0.6			# scale whole graph, but not fonts
#set border				# adds borders to graph
#show border
unset log                              # remove any log-scaling
unset label                            # remove any previous labels
set xtic auto                          # set xtics automatically
set ytic auto                          # set ytics automatically
set title "Messages sent \& recieved, for one timestamp"
set xlabel "User 0 group memberships" 0,0.5
set ylabel "Messages" 2.0
#set key 20.5,1180
show xlabel
show ylabel
#set label "Yield Point" at 0.003,260
#set arrow from 0.0028,250 to 0.003,280
#set xr [0:1000]
#set yr [1000:2200]
set pointsize 1
plot  "groupMembership.testu0" using 2:16 title 'VL' with linespoints ls 3, \
"groupDevideTest.txt" using 2:1 title 'FL' with linespoints ls 7

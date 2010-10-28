# Gnuplot script file for plotting data in file "force.dat"
# This file is called   force.p
reset
#set key horizontal # synonym: horiz
set terminal postscript eps enhanced monochrome solid rounded
set output "lzcsR500.eps"
set   autoscale                        # scale axes automatically
set size 0.60,0.60			# scale whole graph, but not fonts
set border				# adds borders to graph
show border
unset log                              # remove any log-scaling
unset label                            # remove any previous labels
set xtic auto				# set xtics automatically
#set xtic 400                          
set ytic auto 				# set ytics automatically
#set logscale y
#set format y "10^{%L}"
set logscale x
set format x "10^{%L}" 
                        
set title "Messages sent/received, for one user and timestamp"
set xlabel "Level Zero Cell Size" #0,0.5
set ylabel "Messages" #2.0
set key 12700,3.4
show xlabel
show ylabel
#set label "Yield Point" at 0.003,260
#set arrow from 0.0028,250 to 0.003,280
set xr [200:14000]
#set yr [0:22]
#set pointsize 0.8
plot   "lzcs50uR500.test" using 6:($16+$18+$19) title '50 users' with linespoints, \
"lzcs100uR500.test" using 6:($16+$18+$19) title '100 users' with linespoints, \
"lzcs500uR500.test" using 6:($16+$18+$19) title '500 users' with linespoints,\
"lzcs1000uR500.test" using 6:($16+$18+$19) title '1000 users' with linespoints

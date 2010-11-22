# Gnuplot script file for plotting data in file "force.dat"
# This file is called   force.p
reset
set key horizontal # synonym: horiz
set terminal postscript eps enhanced monochrome solid rounded
set output "lzcsR200u5-10.eps"
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
set key 12700,2.3
show xlabel
show ylabel
set label "HC_{10 users}" at 1100,1.05
set arrow from 201,1.1575 to 13850,1.1575 nohead
set label "HC_{5 users}" at 1100,1.35
set arrow from 201,1.26 to 13850,1.26 nohead
set xr [200:14000]
#set yr [0:22]
#set pointsize 0.8
plot   "lzcs5uR200.test" using 6:($16+$18+$19) title '5 VL users' with linespoints, \
"lzcs10uR200.test" using 6:($16+$18+$19) title '10 VL users' with linespoints ls 4, \
"FFdecreaseLvlZero.txt" using 1:7 title '5 FL users' with linespoints ls 6, \
"FFdecreaseLvlZero.txt" using 1:6 title '10 FL users' with linespoints ls 8
# Gnuplot script file for plotting data in file "force.dat"
# This file is called   force.p
reset
#set key horizontal # synonym: horiz
set terminal postscript eps enhanced monochrome #solid rounded
set output "cacheTest-G25_400.eps"
#set   autoscale                        # scale axes automatically
set size 1.5,1.5			# scale whole graph, but not fonts
set border				# adds borders to graph
show border
unset log                              # remove any log-scaling
unset label                            # remove any previous labels
set xtic auto                          # set xtics automatically
set ytic auto                          # set ytics automatically
set title "Cache Hitsrate when increasing cache size"
set xlabel "Cache Pct" 25,0
set ylabel "Cache Hitrate" 0.5, 9
set key 23,95
show xlabel
show ylabel
#set label "Yield Point" at 0.003,260
#set arrow from 0.0028,250 to 0.003,280
#set xr [0:13000]
set yr [0:100]
set pointsize 1.0

set style line 1 lt 1
set style line 2 lt 5
set style line 3 lt 8 
set style line 4 lt 11
set style line 5 lt 14


plot "cacheTest-G025.test" using 5:($14) title 'LRU - Imp(N/4)' with linespoints ls 1,\
"cacheTest-G025.test" using 5:($15) title 'OSC(N/4)' with linespoints ls 2,\
"cacheTest-G025.test" using 5:($16) title 'FIFO(N/4)' with linespoints ls 3,\
"cacheTestNoHitScore-G025.test" using 5:($15) title 'OSC - NodeScore(N/4)' with linespoints,\
"cacheTestNoNodeScore-G025.test" using 5:($15) title 'OSC - HitScore(N/4)' with linespoints,\
"cacheTest-G050.test" using 5:($14) title 'LRU - Imp(N/2)' with linespoints,\
"cacheTest-G050.test" using 5:($15) title 'OSC(N/2)' with linespoints,\
"cacheTest-G050.test" using 5:($16) title 'FIFO(N/2)' with linespoints,\
"cacheTestNoHitScore-G050.test" using 5:($15) title 'OSC - NodeScore(N/2)' with linespoints,\
"cacheTestNoNodeScore-G050.test" using 5:($15) title 'OSC - HitScore(N/2)' with linespoints,\
"cacheTest-G100.test" using 5:($14) title 'LRU - Imp(N)' with linespoints,\
"cacheTest-G100.test" using 5:($15) title 'OSC(N)' with linespoints,\
"cacheTest-G100.test" using 5:($16) title 'FIFO(N)' with linespoints,\
"cacheTestNoHitScore-G100.test" using 5:($15) title 'OSC - NodeScore(N)' with linespoints,\
"cacheTestNoNodeScore-G100.test" using 5:($15) title 'OSC - HitScore(N)' with linespoints,\
"cacheTest-G200.test" using 5:($14) title 'LRU - Imp(2N)' with linespoints,\
"cacheTest-G200.test" using 5:($15) title 'OSC(2N)' with linespoints,\
"cacheTest-G200.test" using 5:($16) title 'FIFO(2N)' with linespoints,\
"cacheTestNoHitScore-G200.test" using 5:($15) title 'OSC - NodeScore(2N)' with linespoints,\
"cacheTestNoNodeScore-G200.test" using 5:($15) title 'OSC - HitScore(2N)' with linespoints,\
"cacheTest-G400.test" using 5:($14) title 'LRU - Imp(4N)' with linespoints,\
"cacheTest-G400.test" using 5:($15) title 'OSC(4N)' with linespoints,\
"cacheTest-G400.test" using 5:($16) title 'FIFO(4N)' with linespoints,\
"cacheTestNoHitScore-G400.test" using 5:($15) title 'OSC - NodeScore(4N)' with linespoints,\
"cacheTestNoNodeScore-G400.test" using 5:($15) title 'OSC - HitScore(4N)' with linespoints
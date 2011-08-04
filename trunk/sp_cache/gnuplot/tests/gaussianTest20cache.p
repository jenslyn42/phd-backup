# Gnuplot script file for plotting data in file "force.dat"
# This file is called   force.p
reset
#set key horizontal # synonym: horiz
set terminal postscript eps enhanced monochrome solid rounded
set output "gaussianTest20pct.eps"
#set   autoscale                        # scale axes automatically
set size 0.8,0.8			# scale whole graph, but not fonts
set border				# adds borders to graph
show border
unset log                              # remove any log-scaling
unset label                            # remove any previous labels
set xtic auto                          # set xtics automatically
set ytic auto                         # set ytics automatically
set title "Cache Hitsrate when increasing Sigma (Gaussian Distribution, 20% Cache)"
set xlabel "Sigma" 25,0
set ylabel "Cache Hitrate" 0.5, 9
set key 3.8,23
show xlabel
show ylabel
#set label "Yield Point" at 0.003,260
#set arrow from 0.0028,250 to 0.003,280
#set xr [0:13000]
set yr [0:25]
set pointsize 1.5

plot "GaussianEffect-20pctCache.test" using ($12):($16) title 'LRU - Im' with linespoints pt 12,\
"GaussianEffect-20pctCache.test" using ($12):($17) title 'OSC' with linespoints,\
"GaussianEffect-20pctCache.test" using ($12):($18) title 'FIFO' with linespoints,\
"GaussianEffectNS-20pctCache.test" using ($12):($17) title 'OSC - NodeScore' with linespoints,\
"GaussianEffectHS-20pctCache.test" using ($12):($17) title 'OSC - HitScore' with linespoints pt 8
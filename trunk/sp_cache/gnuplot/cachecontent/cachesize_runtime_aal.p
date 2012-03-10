# Gnuplot script file for plotting data in file "force.dat"
# This file is called   force.p
reset
#set key horizontal # synonym: horiz
set terminal postscript eps enhanced monochrome solid rounded
set output "cachesize_runtime_aal.eps"
#set   autoscale                        # scale axes automatically
set size 0.5,0.5			# scale whole graph, but not fonts
set border				# adds borders to graph
show border
unset log                              # remove any log-scaling
unset label                            # remove any previous labels
set log x
set xtic auto                          # set xtics automatically
set ytic auto                          # set ytics automatically
#set title "Cache Hitsrate when increasing cache size"
set xlabel "Cache size"
set ylabel "Time"
set key left bottom
show xlabel
show ylabel
#set label "Yield Point" at 0.003,260
#set arrow from 0.0028,250 to 0.003,280
set xr [1:10000]
set yr [0:20]
set pointsize 1.0

plot    "v_cachesize_HQF_aal.test" using (($8/8)/1024):1 title 'HQF' with linespoints pt 2 lw 2,\
"v_cachesize_LRU_aal.test" using (($8/8)/1024):1 title 'LRU' with linespoints pt 4 lw 2,\
"v_cachesize_SPC_aal.test" using (($8/8)/1024):1 title 'SPC' with linespoints pt 8 lw 2,\
"v_cachesize_SPCSTAR_aal.test" using (($8/8)/1024):1 title 'SPC^*' with linespoints pt 14 lw 2

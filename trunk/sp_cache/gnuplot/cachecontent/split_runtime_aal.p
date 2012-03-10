# Gnuplot script file for plotting data in file "force.dat"
# This file is called   force.p
reset
#set key horizontal # synonym: horiz
set terminal postscript eps enhanced monochrome solid rounded
set output "split_runtime_aal.eps"
#set   autoscale                        # scale axes automatically
set size 0.5,0.5			# scale whole graph, but not fonts
set border				# adds borders to graph
show border
unset log                              # remove any log-scaling
unset label                            # remove any previous labels
set xtic auto                          # set xtics automatically
set ytic auto                          # set ytics automatically
#set title "Cache Hitsrate when increasing cache size"
set xlabel "kD-tree level"
set ylabel "Time"
set key right bottom
show xlabel
show ylabel
#set label "Yield Point" at 0.003,260
#set arrow from 0.0028,250 to 0.003,280
#set xr [0:13000]
set yr [0:31]
set pointsize 1.0


plot "v_split_SPC_aal.test" using 10:1 title 'SPC' with linespoints pt 8 lw 2,\
"v_split_SPCSTAR_aal.test" using 10:1 title 'SPC^*' with linespoints pt 14 lw 2

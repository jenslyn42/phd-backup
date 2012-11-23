# Gnuplot script file for plotting data in file "force.dat"
# This file is called   force.p
reset
#set key horizontal # synonym: horiz
set terminal postscript eps enhanced monochrome solid rounded
set output "split_diffnodes_bei_server.eps"
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
set ylabel "Visited Nodes Saving Ratio (%)"
set key right bottom
show xlabel
show ylabel
#set label "Yield Point" at 0.003,260
#set arrow from 0.0028,250 to 0.003,280
#set xr [6:16]
set yr [0:32]
set pointsize 1.0


plot "v_split_SPC_NONE_bei_server_diff.test" using 11:(100*($20-$6)/$20) title 'SPC' with linespoints pt 8 lw 2,\
"v_split_SPCSTAR_NONE_bei_server_diff.test" using 11:(100*($20-$6)/$20) title 'SPC^*' with linespoints pt 14 lw 2
# Gnuplot script file for plotting data in file "force.dat"
# This file is called   force.p
reset
#set key horizontal # synonym: horiz
set terminal postscript eps enhanced monochrome solid rounded
set output "cachesize_diffnodes_bei_server.eps"
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
set ylabel "Visited Nodes Saving Ratio (%)"
set key left top
show xlabel
show ylabel
#set label "Yield Point" at 0.003,260
#set arrow from 0.0028,250 to 0.003,280
set xr [1:10000]
#set yr [0:44000000]
set pointsize 1.0

plot    "v_cachesize_HQF_NONE_bei_server_diff.test" using (($9/8)/1024):(100*($20-$6)/$20) title 'HQF' with linespoints pt 2 lw 2,\
"v_cachesize_LRU_NONE_bei_server_diff.test" using (($9/8)/1024):(100*($20-$6)/$20) title 'LRU' with linespoints pt 4 lw 2,\
"v_cachesize_SPC_NONE_bei_server_diff.test" using (($9/8)/1024):(100*($20-$6)/$20) title 'SPC' with linespoints pt 8 lw 2,\
"v_cachesize_SPCSTAR_NONE_bei_server_diff.test" using (($9/8)/1024):(100*($20-$6)/$20) title 'SPC^*' with linespoints pt 14 lw 2

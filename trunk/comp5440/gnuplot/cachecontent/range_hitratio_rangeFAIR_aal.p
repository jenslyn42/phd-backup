# Gnuplot script file for plotting data in file "force.dat"
# This file is called   force.p
reset
#set key horizontal # synonym: horiz
set terminal postscript eps enhanced monochrome solid rounded
set output "range_hitratio_rangefair_aal.eps"
#set   autoscale                        # scale axes automatically
set size 0.4,0.4			# scale whole graph, but not fonts
set border				# adds borders to graph
show border
unset log                              # remove any log-scaling
unset label                            # remove any previous labels
set log x
set xtic auto                          # set xtics automatically
set ytic auto                          # set ytics automatically
#set title "Cache Hitsrate when increasing range size"
set xlabel "Range"
set ylabel "Hit ratio (%)"
set key left top
show xlabel
show ylabel
#set label "Yield Point" at 0.003,260
#set arrow from 0.0028,250 to 0.003,280
set xr [100:21000]
#set yr [0:100]
set pointsize 1.0

plot    "v_range_R2000_FAIR_HQF_aal.test" using ($16):(($2/($2+$3))*100) title 'HQF' with linespoints pt 2 lw 2,\
"v_range_R2000_FAIR_LRU_aal.test" using ($16):(($2/($2+$3))*100) title 'LRU' with linespoints pt 4 lw 2,\
"v_range_R2000_FAIR_SPC_aal.test" using ($16):(($2/($2+$3))*100) title 'SPC' with linespoints pt 8 lw 2

#"v_range_R2000_FAIR_SPCSTAR_aal.test" using ($16):(($2/($2+$3))*100) title 'SPC*' with linespoints pt 14 lw 2


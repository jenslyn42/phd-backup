# Gnuplot script file for plotting data in file "force.dat"
# This file is called   force.p
reset
#set key horizontal # synonym: horiz
set terminal postscript eps enhanced monochrome solid rounded
set output "cachesize_aal.eps"
#set   autoscale                        # scale axes automatically
set size 0.8,0.8			# scale whole graph, but not fonts
set border				# adds borders to graph
show border
unset log                              # remove any log-scaling
unset label                            # remove any previous labels
set log x
set xtic auto                          # set xtics automatically
set ytic auto                          # set ytics automatically
set title "Cache Hitsrate when increasing cache size"
set xlabel "Cache size"
set ylabel "Cache Hitrate"
set key left top
show xlabel
show ylabel
#set label "Yield Point" at 0.003,260
#set arrow from 0.0028,250 to 0.003,280
#set xr [0:13000]
set yr [0:100]
set pointsize 1.0

plot    "v_cachesize_HQF_Aal.test" using (($8/8)/1024):(($2/($2+$3))*100) title 'HQF' with linespoints pt 2 lw 2,\
"v_cachesize_LRU_Aal.test" using (($8/8)/1024):(($2/($2+$3))*100) title 'LRU' with linespoints pt 4 lw 2,\
"v_cachesize_ORACLE_Aal.test" using (($8/8)/1024):(($2/($2+$3))*100) title 'ORACLE' with linespoints pt 6 lw 2,\
"v_cachesize_SPC_Aal.test" using (($8/8)/1024):(($2/($2+$3))*100) title 'SPC' with linespoints pt 8 lw 2,\
"v_cachesize_SPCPLUS_Aal.test" using (($8/8)/1024):(($2/($2+$3))*100) title 'SPC^+' with linespoints pt 12 lw 2,\
"v_cachesize_SPCSTAR_Aal.test" using (($8/8)/1024):(($2/($2+$3))*100) title 'SPC^*' with linespoints pt 14 lw 2

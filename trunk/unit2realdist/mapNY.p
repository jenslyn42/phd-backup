# Gnuplot script file for plotting data in file "force.dat"
# This file is called   force.p
reset
#set key horizontal # synonym: horiz
set terminal postscript eps enhanced monochrome solid rounded
set output "mapNY.eps"
#set   autoscale                        # scale axes automatically
set size 2,2		# scale whole graph, but not fonts
set border				# adds borders to graph
show border
unset log                              # remove any log-scaling
unset label                            # remove any previous labels
#set log x
#set xtic 2                          # set xtics automatically
#set ytic 2                          # set ytics automatically
set grid xtics ytics
#set title "Cache Hitsrate when increasing cache size"
#set xlabel "Cache size"
#set ylabel "Time"
#set key left bottom
show xlabel
show ylabel
#set label "Yield Point" at 0.003,260
#set arrow from 0.0028,250 to 0.003,280
set xr [-74100000:-73900000]
set yr [40500000:40700000]
set pointsize 0.5
set size ratio -1

#set object 1 circle center first 539330.3125, 6303432, 0 size first 1600
#set object 1 back lw 1.0 fc  rgb "red" fs transparent solid 0.5 noborder
#set object 2 circle center first 532598.8125, 6300312, 0 size first 1600
#set object 2 back lw 1.0 fc  rgb "red" fs transparent solid 1.5 noborder
#set object 3 circle center first 562804.25, 6330826, 0 size first 1600
#set object 3 back lw 1.0 fc  rgb "cyan" fs transparent solid 0.5 noborder
#set object 4 circle center first 564231.3125, 6318503, 0 size first 1600
#set object 4 back lw 1.0 fc  rgb "green" fs transparent solid 0.5 noborder
#set object 5 circle center first 550922.75, 6349731, 0 size first 1600
#set object 5 back lw 1.0 fc  rgb "purple" fs transparent solid 0.5 noborder
#set object 6 circle center first 551363.0625, 6328788, 0 size first 1600
#set object 6 back lw 1.0 fc  rgb "green" fs transparent solid 0.5 noborder
#set object 7 circle center first 550534.875, 6305558, 0 size first 1600
#set object 7 back lw 1.0 fc  rgb "magenta" fs transparent solid 0.5 noborder
#set object 8 circle center first 542396.125, 6312403, 0 size first 1600
#set object 8 back lw 1.0 fc  rgb "blue" fs transparent solid 0.5 noborder
#set object 9 circle center first 561657.5, 6344880, 0 size first 1600
#set object 9 back lw 1.0 fc  rgb "green" fs transparent solid 0.5 noborder
#set object 10 circle center first 553555.375, 6328181, 0 size first 1600
#set object 10 back lw 1.0 fc  rgb "cyan" fs transparent solid 1.5 noborder


plot "USA-road-d.NY.cnode" using 2:3 title 'NY' with points pt 7


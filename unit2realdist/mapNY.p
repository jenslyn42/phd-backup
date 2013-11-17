# Gnuplot script file for plotting data in file "force.dat"
# This file is called   force.p
reset
#set key horizontal # synonym: horiz
set terminal postscript eps enhanced monochrome solid rounded
set output "mapNY.eps"
set   autoscale                        # scale axes automatically
set size 2,2		# scale whole graph, but not fonts
set border				# adds borders to graph
show border
unset log                              # remove any log-scaling
unset label                            # remove any previous labels
#set log x
#set xtic 2                          # set xtics automatically
#set ytic 2                          # set ytics automatically
set grid xtics ytics
set format x "%9.1f"
set format y "%9.1f"
#set title "Cache Hitsrate when increasing cache size"
#set xlabel "Cache size"
#set ylabel "Time"
#set key left bottom
show xlabel
show ylabel
#set label "Yield Point" at 0.003,260
#set arrow from 0.0028,250 to 0.003,280
#set xr [-73700000:-73500000]
#set yr [41100000:41300000]
set pointsize 0.5
set size ratio -1

set object 10 circle center first -73550000, 41100000, 0 size first 20000
set object 10 back lw 1.0 fc  rgb "green" fs transparent solid 1.5 noborder
set object 11 circle center first -74100000, 40350000, 0 size first 20000
set object 11 back lw 1.0 fc  rgb "red" fs transparent solid 1.5 noborder

#set object 10 circle center first -74455000, 40310000, 0 size first 6400
#set object 10 back lw 1.0 fc  rgb "cyan" fs transparent solid 1.5 noborder
#set object 11 circle center first -74480000, 40485000, 0 size first 6400
#set object 11 back lw 1.0 fc  rgb "green" fs transparent solid 1.5 noborder
#set object 12 circle center first -74134000, 40448000, 0 size first 6400
#set object 12 back lw 1.0 fc  rgb "green" fs transparent solid 1.5 noborder
#set object 13 circle center first -74047000, 40352000, 0 size first 6400
#set object 13 back lw 1.0 fc  rgb "green" fs transparent solid 1.5 noborder
#set object 14 circle center first -74460000, 40570000, 0 size first 6400
#set object 14 back lw 1.0 fc  rgb "green" fs transparent solid 1.5 noborder
#set object 15 circle center first -74415000, 40507000, 0 size first 6400
#set object 15 back lw 1.0 fc  rgb "green" fs transparent solid 1.5 noborder
#set object 16 circle center first -74137000, 40617000, 0 size first 6400
#set object 16 back lw 1.0 fc  rgb "green" fs transparent solid 1.5 noborder
#set object 17 circle center first -74250000, 40641000, 0 size first 6400
#set object 17 back lw 1.0 fc  rgb "green" fs transparent solid 1.5 noborder
#set object 18 circle center first -74070000, 40610000, 0 size first 6400
#set object 18 back lw 1.0 fc  rgb "green" fs transparent solid 1.5 noborder
#set object 19 circle center first -73807000, 40692000, 0 size first 6400
#set object 19 back lw 1.0 fc  rgb "green" fs transparent solid 1.5 noborder
#set object 20 circle center first -73555000, 40680000, 0 size first 6400
#set object 20 back lw 1.0 fc  rgb "green" fs transparent solid 1.5 noborder
#set object 21 circle center first -74480000, 40870000, 0 size first 6400
#set object 21 back lw 1.0 fc  rgb "green" fs transparent solid 1.5 noborder
#set object 22 circle center first -74183000, 40739000, 0 size first 6400
#set object 22 back lw 1.0 fc  rgb "green" fs transparent solid 1.5 noborder
#set object 23 circle center first -73810000, 40819000, 0 size first 6400
#set object 23 back lw 1.0 fc  rgb "green" fs transparent solid 1.5 noborder
#set object 24 circle center first -74005000, 40717000, 0 size first 6400
#set object 24 back lw 1.0 fc  rgb "green" fs transparent solid 1.5 noborder
#set object 25 circle center first -73610000, 40710000, 0 size first 6400
#set object 25 back lw 1.0 fc  rgb "green" fs transparent solid 1.5 noborder
#set object 26 circle center first -74169000, 40911000, 0 size first 6400
#set object 26 back lw 1.0 fc  rgb "green" fs transparent solid 1.5 noborder
#set object 27 circle center first -74000000, 40925500, 0 size first 6400
#set object 27 back lw 1.0 fc  rgb "green" fs transparent solid 1.5 noborder
#set object 28 circle center first -73788000, 40910700, 0 size first 6400
#set object 28 back lw 1.0 fc  rgb "green" fs transparent solid 1.5 noborder
#set object 29 circle center first -74027000, 41118000, 0 size first 6400
#set object 29 back lw 1.0 fc  rgb "green" fs transparent solid 1.5 noborder


#set object 1 circle center first -74387494, 40400007, 0 size first 16000
#set object 1 back lw 1.0 fc  rgb "red" fs transparent solid 0.5 noborder
#set object 2 circle center first 73869079, 40924098, 0 size first 16000
#set object 2 back lw 1.0 fc  rgb "red" fs transparent solid 1.5 noborder
#set object 3 circle center first -73672474, 40712902, 0 size first 16000
#set object 3 back lw 1.0 fc  rgb "cyan" fs transparent solid 0.5 noborder
#set object 4 circle center first -73580672, 40682301, 0 size first 16000
#set object 4 back lw 1.0 fc  rgb "green" fs transparent solid 0.5 noborder
#set object 5 circle center first -74068286, 41111995, 0 size first 16000
#set object 5 back lw 1.0 fc  rgb "purple" fs transparent solid 0.5 noborder
#set object 6 circle center first -73587053, 40828951, 0 size first 16000
#set object 6 back lw 1.0 fc  rgb "green" fs transparent solid 0.5 noborder
#set object 7 circle center first -74050084, 40955598, 0 size first 16000
#set object 7 back lw 1.0 fc  rgb "magenta" fs transparent solid 0.5 noborder
#set object 8 circle center first -74275892, 40944898, 0 size first 16000
#set object 8 back lw 1.0 fc  rgb "blue" fs transparent solid 0.5 noborder
#set object 9 circle center first -73626070, 41021397, 0 size first 16000
#set object 9 back lw 1.0 fc  rgb "green" fs transparent solid 0.5 noborder
#set object 10 circle center first -74241189, 40438307, 0 size first 16000
#set object 10 back lw 1.0 fc  rgb "cyan" fs transparent solid 1.5 noborder


plot "USA-road-d.NY.cnode" using 2:3 title 'NY' with points pt 7


set datafile separator " "
set terminal postscript enhanced color 
set output '| ps2pdf - output.pdf'

set xlabel 'distance' font "Times,30" offset 0,-1.5
set ylabel '# of edge' font "Times,30" offset -3,0
set xtics font "Times,30" offset 0,-1
set ytics font "Times,30" offset -.5,0
set autoscale
set logscale x

set pointsize 1 
set key spacing 3.5


binwidth=0.01
bin(x,width)=width*floor(x/width)

set boxwidth binwidth

plot 'histo.data' using (bin($1,binwidth)):(1.0) smooth freq with boxes title "histogram" 

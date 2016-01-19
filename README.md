# KaDraw
This is the open source graph drawing package KaDraw (Karlsruhe Graph Drawing).

Main project site: http://algo2.iti.kit.edu/kadraw/

Required Software Packages
=====

Before you can start you need to install the following software packages:

- Scons (http://www.scons.org/)
- Argtable (http://argtable.sourceforge.net/)
- Cairo (http://cairographics.org/)

Once you installed the packages, just type ./compile.sh.

Then try 

./deploy/kadraw --burn_image_to_disk --export_type=pdf --output_filename=my.pdf examples/delaunay_n16.graph 

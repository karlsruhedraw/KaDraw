# KaDraw
This is the open source graph drawing package KaDraw (Karlsruhe Graph Drawing).

Main project site: https://karlsruhedraw.github.io

Required Software Packages
=====

Before you can start you need to install the following software packages:

- CMake (http://www.cmake.org/)
- Cairo (http://cairographics.org/)

Once you installed the packages, just type ./compile.sh.

Then try 

./deploy/kadraw --burn_image_to_disk --export_type=pdf --output_filename=my.pdf examples/delaunay_n16.graph 

#!/bin/bash


scons program=drawit variant=optimized -j 4 -c 
scons program=graphchecker variant=optimized -j 4 -c

rm -rf deploy
rm -rf optimized

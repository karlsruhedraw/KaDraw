#!/bin/bash

rm -rf deploy

scons program=drawit variant=optimized -j 8 
scons program=drawdyn variant=optimized -j 8 
scons program=graphchecker variant=optimized -j 8
scons program=evaluator variant=optimized -j 8
scons program=draw_from_coordinates variant=optimized -j 8
scons program=graph_to_mtx variant=optimized -j 8

mkdir deploy
cp ./optimized/drawit deploy/
cp ./optimized/drawdyn deploy/
cp ./optimized/graphchecker deploy/
cp ./optimized/evaluator deploy/
cp ./optimized/draw_from_coordinates deploy/
cp ./optimized/graph_to_mtx deploy/

rm -rf ./optimized

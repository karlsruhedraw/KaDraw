#!/bin/bash

rm -rf deploy

mkdir build
cd build
cmake ..
make -j 4
cd ..

mkdir deploy
cp ./build/kadraw deploy/
cp ./build/graphchecker deploy/
cp ./build/evaluator deploy/
cp ./build/draw_from_coordinates deploy/

rm -rf ./build

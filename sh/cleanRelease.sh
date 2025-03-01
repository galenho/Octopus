#!/bin/sh

cd ..
cd engine/Release
make clean

cd ../..
cd Octopus/Release
make clean

cd ../../bin
rm Octopus
cd ../lib
rm libengine.a
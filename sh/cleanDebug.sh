#!/bin/sh

cd ..
cd engine/Debug
make clean

cd ../..
cd Octopus/Debug
make clean

cd ../../bin
rm OctopusD
cd ../lib
rm libengineD.a
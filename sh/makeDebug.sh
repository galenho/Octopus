#!/bin/sh

./automake

cd ..
cd engine/Debug
rm -rf engine_build.log
rm -rf libengineD.a
make all -j6 2>&1 | tee engine_build.log
cp libengineD.a ../../lib/linux64/libengineD.a

cd ../..
cd Octopus/Debug
rm -rf Octopus_build.log
rm -rf OctopusD
make all -j6 2>&1 | tee Octopus_build.log
cp OctopusD ../../bin/OctopusD

echo make Debug ok!

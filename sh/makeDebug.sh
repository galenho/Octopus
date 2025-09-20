#!/bin/sh

./automake

cd ..
cd Octopus/Debug
rm -rf Octopus_build.log
rm -rf OctopusD
make all -j6 2>&1 | tee Octopus_build.log
cp OctopusD ../../bin/OctopusD

echo make Debug ok!

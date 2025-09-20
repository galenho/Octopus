#!/bin/sh

./automake

cd ..
cd Octopus/Release
rm -rf Octopus_build.log
rm -rf Octopus
make all -j6 2>&1 | tee Octopus_build.log
cp Octopus ../../bin/Octopus

echo make Release ok!

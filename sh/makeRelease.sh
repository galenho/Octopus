#!/bin/sh

./automake

cd ..
cd engine/Release
rm -rf engine_build.log
rm -rf libengine.a
make all -j6 2>&1 | tee engine_build.log
cp libengine.a ../../lib/linux64/libengine.a

cd ../..
cd Octopus/Release
rm -rf Octopus_build.log
rm -rf Octopus
make all -j6 2>&1 | tee Octopus_build.log
cp Octopus ../../bin/Octopus

echo make Release ok!

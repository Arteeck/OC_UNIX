#!/bin/bash
make -s create_start_file
make -s task

./create_start_file
exitCode=$?
if [[ $exitCode -ne 0 ]]
then
	exit -1
fi

./task fileA fileB
exitCode=$?
if [[ $exitCode -ne 0 ]]
then
	exit -1
fi

gzip -kf fileA
gzip -kf fileB

gzip -cd fileB.gz | ./task fileC
exitCode=$?
if [[ $exitCode -ne 0 ]]
then
	exit -1
fi

./task fileA fileD -b 100
exitCode=$?
if [[ $exitCode -ne 0 ]]
then
	exit -1
fi

blockSize=$(stat fileA --printf=%B)
fileBlocks=$(stat fileA --printf=%b)
echo fileA physical size = $((blockSize*fileBlocks))
fileBlocks=$(stat fileB --printf=%b)
echo fileB physical size = $((blockSize*fileBlocks))
fileBlocks=$(stat fileA.gz --printf=%b)
echo fileA.gz physical size = $((blockSize*fileBlocks))
fileBlocks=$(stat fileB.gz --printf=%b)
echo fileB.gz physical size = $((blockSize*fileBlocks))
fileBlocks=$(stat fileC --printf=%b)
echo fileC physical size = $((blockSize*fileBlocks))
fileBlocks=$(stat fileD --printf=%b)
echo fileD physical size = $((blockSize*fileBlocks))

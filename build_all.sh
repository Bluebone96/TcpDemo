#!/bin/bash

path=`pwd`

cd ${path}/Proto
echo "start build protobuf"
sh build.sh
make clean && make

cd ${path}/Common
echo "start build Common" 
make clean && make

cd ${path}/Server
echo "start build Server"
make clean && make

cd ${path}/Client
echo "start build Client"
make clean && make

#!/bin/bash

echo "robot_run 0~100"
./robot_run 0 100  &

echo "robot_run 100~200"
./robot_run 100 200 &

echo "robot_run 200~300"
./robot_run 200 300 &  

echo "robot_run 300~400"
./robot_run 300 400 &

echo "robot_run 400~500"
./robot_run 400 500 &



#for i in $(seq 1 10); do
#{
##    echo "robot_run" ${(i-1)*50}"~"${i * 50}
#    ./robot_run (${i}-1)*50 ${i}*50
#}&
#done

wait

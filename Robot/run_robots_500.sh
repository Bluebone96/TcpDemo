#!/bin/bash

echo "robot_run 0~100"
#./robot_run 0 100 &
./robot_run 0 100  &

echo "robot_run 100~200"
./robot_run 100 200 &

echo "robot_run 200~300"
./robot_run 200 300 &  

echo "robot_run 300~400"
./robot_run 300 400 &

echo "robot_run 400~500"
./robot_run 400 500 && fg




#!/bin/bash

echo "start db_server"
./db_server > db.log &
sleep 0.1

echo "start game_server"
./game_server > game.log &
sleep 0.1

echo "start gate_server"
./gate_server > gate.log & 
sleep 0.1

echo "start login_server"
./login_server > login.log &


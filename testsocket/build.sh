#!/bin/bash

echo "start build server"
g++ -o server testserver.cpp ../Common/TcpSocket.h ../Common/TcpSocket.cpp

echo "start build client"
g++ -o client testclient.cpp ../Common/TcpSocket.h ../Common/TcpSocket.cpp


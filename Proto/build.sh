#!/bin/bash

path=.
for filename in `find $path -name "*.proto"`
do 
    echo "building $filename ..."
    protoc --proto_path=$path --csharp_out=. "$filename"
    protoc --proto_path=$path --cpp_out=. "$filename"
done

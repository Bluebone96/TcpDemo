#!/bin/bash

echo "game_server flamegraph start"

cd game_server
perf script -i perf.data > perf.perf
../../../FlameGraph/stackcollapse-perf.pl perf.perf > perf.folded
../../../FlameGraph/flamegraph.pl perf.folded > perf.svg


echo "gate_server flamegraph start"
cd ../gate_server
perf script -i perf.data > perf.perf
../../../FlameGraph/stackcollapse-perf.pl perf.perf > perf.folded
../../../FlameGraph/flamegraph.pl perf.folded > perf.svg

echo "end"
cd ..

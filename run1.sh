#!/bin/bash

g++ -O2 memcopy_benchmark.cpp -m64 -isystem benchmark/include -Lbenchmark/build/src -mavx -lbenchmark -pthread -o memcopy
sudo chrt --rr 99 ./memcopy --benchmark_out=memcopy_info.csv --benchmark_out_format=csv
#python cache_graph_maker.py cache_info.csv

#sudo cpupower frequency-set --governor performance
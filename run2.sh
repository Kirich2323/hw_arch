#!/bin/bash

g++ cache_benchmark.cpp -isystem benchmark/include -Lbenchmark/build/src -lbenchmark -pthread -o cache
sudo chrt --rr 99 ./cache --benchmark_out=cache_info.csv --benchmark_out_format=csv
python cache_graph_maker.py cache_info.csv
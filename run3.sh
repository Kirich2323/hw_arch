#!/bin/bash

g++ branch_prediction_benchmark.cpp  -O2 -isystem benchmark/include -Lbenchmark/build/src -lbenchmark -pthread -o branch_prediction
sudo chrt --rr 99 ./branch_prediction --benchmark_out=branch_prediction_info.csv --benchmark_out_format=csv
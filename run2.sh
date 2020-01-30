#!/bin/bash

g++ -O2 ./cache.cpp -o cache
sudo chrt --rr 99 ./cache > cache_info.txt
python3 graph_maker.py cache_info.txt
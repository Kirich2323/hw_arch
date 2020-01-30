#!/bin/bash

g++ -O2 ./branch_prediction.cpp -o branch_prediction
sudo chrt --rr 99 ./branch_prediction
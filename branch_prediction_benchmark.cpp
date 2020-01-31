#include <iostream>
#include <string>
#include <map>
#include <vector>
#include <fstream>
#include <chrono>
#include <cstring>
#include <random>
#include <cpuid.h>
#include <stdio.h>
#include<benchmark/benchmark.h>

//sse
#include <immintrin.h>

using namespace std;

typedef unsigned char uchar;
typedef unsigned long long ull;

const int KBYTE = 1 << 10;
const int MBYTE = 1 << 20;
const int GBYTE = 1 << 30;

int max_iter = 1e6;
const int is_jump_len = 1e6;
bool is_jump[is_jump_len];
volatile int dummy = 0;

static void BM_jump_always_true(benchmark::State& state) {
    
    for (int i = 0; i < is_jump_len; ++i) {
        is_jump[i] = true;
    }

    for (auto _: state) {
        for (int i = 0; i < max_iter; ++i) {
            if (is_jump[i % is_jump_len]) {
                benchmark::DoNotOptimize(dummy = 1 + 1);
            }
            else {
                benchmark::DoNotOptimize(dummy = 1 + 2);
            }
        }
    }
}

static void BM_jump_odd_even(benchmark::State& state) {
    for (int i = 0; i < is_jump_len; ++i) {
        is_jump[i] = i % 2;
    }

    for (auto _: state) {
        for (int i = 0; i < max_iter; ++i) {
            if (is_jump[i % is_jump_len]) {
                benchmark::DoNotOptimize(dummy = 1 + 1);
            }
            else {
                benchmark::DoNotOptimize(dummy = 1 + 2);
            }
        }
    }
}

static void BM_jump_random(benchmark::State& state) {
    for (int i = 0; i < is_jump_len; ++i) {
        is_jump[i] = random() % 2;
    }

    for (auto _: state) {
        for (int i = 0; i < max_iter; ++i) {
            if (is_jump[i % is_jump_len]) {
                benchmark::DoNotOptimize(dummy = 1 + 1);
            }
            else {
                benchmark::DoNotOptimize(dummy = 1 + 2);
            }
        }
    }
}


static void BM_find_seed(benchmark::State& state) {
    for (int i = 0; i < is_jump_len; ++i) {
        is_jump[i] = state.range(0) >> (i % 15) & 1;
    }

    for (auto _: state) {
        for (int i = 0; i < max_iter; ++i) {
            if (is_jump[i % is_jump_len]) {
                benchmark::DoNotOptimize(dummy = 1 + 1);
            }
            else {
                benchmark::DoNotOptimize(dummy = 1 + 2);
            }
        }
    }
}

static void BranchPredictionArguments(benchmark::internal::Benchmark* b) {
  for (int seed = 0; seed < 1 << 15; seed++)
      b->Arg(seed);
}

BENCHMARK(BM_jump_always_true)->Iterations(1000);
BENCHMARK(BM_jump_odd_even)->Iterations(1000);
BENCHMARK(BM_find_seed)->Iterations(1000)->Arg(21746);
BENCHMARK(BM_jump_random)->Iterations(1000);   
BENCHMARK_MAIN();
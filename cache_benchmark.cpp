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

using namespace std;

typedef unsigned char uchar;
typedef unsigned long long ull;

const int KBYTE = 1 << 10;
const int MBYTE = 1 << 20;
const int GBYTE = 1 << 30;
const int TIMES = 10;

// static void BM_StringCreation(benchmark::State& state) {
//   for (auto _ : state)
//     std::string empty_string;
// }
// // Register the function as a benchmark
// BENCHMARK(BM_StringCreation);

// // Define another benchmark
// static void BM_StringCopy(benchmark::State& state) {

//   std::string x = "hello";
//   for (auto _ : state)
//     std::string copy(x);
// }
// BENCHMARK(BM_StringCopy);

static void BM_MemoryAccess(benchmark::State& state) {
  //cout << state.range(0) << endl;
  const int block_size = state.range(0)/sizeof(ull);
  const int stride = state.range(1) / sizeof(ull);
  ull dummy = 0;
  vector<ull> arr(block_size);
  for (auto _: state) {
    // state.PauseTiming();
    // state.ResumeTiming();
    //for (int i = 0; i < TIMES; ++i) {
      //state.PauseTiming();
      for(int j = 0; j < block_size; j += stride) {
         //state.ResumeTiming();
         benchmark::DoNotOptimize(dummy = arr[j]);
         //dummy = arr[j];
         //state.PauseTiming();
      }
    // }
  }

  state.counters["avg. time"] = benchmark::Counter(max(block_size/stride, 1),  benchmark::Counter::kIsIterationInvariantRate | benchmark::Counter::kInvert);
}

static void CacheArguments(benchmark::internal::Benchmark* b) {
  for (int block_size = 1024 * KBYTE; block_size <= 128 * MBYTE; block_size <<= 1)
    for (int stride = sizeof(ull); stride <= 16 * MBYTE /*&& stride < block_size*/; stride <<= 1)
      b->Args({block_size, stride});
}

BENCHMARK(BM_MemoryAccess)->Apply(CacheArguments);

BENCHMARK_MAIN();
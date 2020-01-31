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

static void BM_copy_for(benchmark::State& state) {
  ull* source = new ull[state.range(0)/sizeof(ull)];
  ull* target = new ull[state.range(0)/sizeof(ull)];
  for (auto _: state) {
      for(int j = 0; j < state.range(0)/sizeof(ull); ++j) {
         //benchmark::DoNotOptimize(target[j] = source[j]);
         target[j] = source[j];
      }
  }
  delete source;
  delete target;
}

static void BM_copy_memcpy(benchmark::State& state) {
  ull* source = new ull[state.range(0)/sizeof(ull)];
  ull* target = new ull[state.range(0)/sizeof(ull)];
  for (auto _: state) {
      memcpy(target, source, state.range(0));
  }
  delete source;
  delete target;
}

static void BM_copy_movs(benchmark::State& state) {
  ull* source = new ull[state.range(0)/sizeof(ull)];
  ull* target = new ull[state.range(0)/sizeof(ull)];
  for (auto _: state) {
      asm volatile (
        "movq %0, %%rcx;"
        "movq %1, %%rsi;"
        "movq %2, %%rdi;"
        "rep movsq;"
        :
        : "r"(state.range(0)>>3), "r"(source), "r"(target)
        :"%ecx", "%rsi", "%rdi"
    );
  }
  delete source;
  delete target;
}

static void BM_copy_avx(benchmark::State& state) {
  ull* source = new ull[state.range(0)/sizeof(ull)];
  ull* target = new ull[state.range(0)/sizeof(ull)];
  for (auto _: state) {

      __m256i buffer;
      void* src = source;
      void* dst = target;
      size_t size = state.range(0)/sizeof(ull);
      while((((size_t)dst) & 31) != 0) {
          *(uchar*)dst = *(uchar*)src;
          ++src;
          ++dst;
          --size;
      }

      _mm_prefetch((const char*)(src), _MM_HINT_NTA);
      for (int i = 0; i < size; i += 256) {
          __m256i m0 = _mm256_loadu_si256(((const __m256i*)src) + 0);
          __m256i m1 = _mm256_loadu_si256(((const __m256i*)src) + 1);
          __m256i m2 = _mm256_loadu_si256(((const __m256i*)src) + 2);
          __m256i m3 = _mm256_loadu_si256(((const __m256i*)src) + 3);
          __m256i m4 = _mm256_loadu_si256(((const __m256i*)src) + 4);
          __m256i m5 = _mm256_loadu_si256(((const __m256i*)src) + 5);
          __m256i m6 = _mm256_loadu_si256(((const __m256i*)src) + 6);
          __m256i m7 = _mm256_loadu_si256(((const __m256i*)src) + 7);
          src += 256;
          _mm_prefetch((const char*)(src + 512), _MM_HINT_NTA);
          _mm256_stream_si256(((__m256i*)dst) + 0, m0);
          _mm256_stream_si256(((__m256i*)dst) + 1, m1);
          _mm256_stream_si256(((__m256i*)dst) + 2, m2);
          _mm256_stream_si256(((__m256i*)dst) + 3, m3);
          _mm256_stream_si256(((__m256i*)dst) + 4, m4);
          _mm256_stream_si256(((__m256i*)dst) + 5, m5);
          _mm256_stream_si256(((__m256i*)dst) + 6, m6);
          _mm256_stream_si256(((__m256i*)dst) + 7, m7);
          dst += 256;
      }
      _mm_sfence();

      //memcpy_tiny(dst, src, size);
      _mm256_zeroupper();
  }
  delete source;
  delete target;
}

static void MemCopyArguments(benchmark::internal::Benchmark* b) {
  for (int block_size = 16 * KBYTE; block_size <= 512 * MBYTE; block_size <<= 1)
      b->Arg(block_size);
}

BENCHMARK(BM_copy_avx)->Iterations(100)->Apply(MemCopyArguments);
BENCHMARK(BM_copy_movs)->Iterations(100)->Apply(MemCopyArguments);
BENCHMARK(BM_copy_for)->Iterations(100)->Apply(MemCopyArguments);
BENCHMARK(BM_copy_memcpy)->Iterations(100)->Apply(MemCopyArguments);

BENCHMARK_MAIN();
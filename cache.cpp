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

using namespace std;

typedef unsigned char uchar;

const int KBYTE = 1 << 10;
const int MBYTE = 1 << 20;
const int GBYTE = 1 << 30;

uchar *source;

void init() {
    for (int i = 0; i < GBYTE; i+=4) {
        *(long*)(source+i) = random();
    }
}

// static inline void native_cpuid(unsigned int *eax, unsigned int *ebx,
//                                 unsigned int *ecx, unsigned int *edx)
// {
//     //__cpuid(2, eax, ebx, ecx, edx);
//         /* ecx is often an input as well as an output. */
//         asm volatile("cpuid"
//             : "=a" (*eax),
//               "=b" (*ebx),
//               "=c" (*ecx),
//               "=d" (*edx)
//             : "0" (*eax), "2" (*ecx));
// }

void print_cache_information() {
    unsigned int cnt = 0;
    unsigned int max = 1;
    long int result = 0;
    bool no_level_2_or_3 = false;
    bool has_level_2 = false;

    while (cnt++ < max) {
        unsigned int eax=0;
        unsigned int ebx=1;
        unsigned int ecx;
        unsigned int edx;

        __cpuid(0, eax, ebx, ecx, edx);
  
        if (cnt == 1) {
            max = eax & 0xff;
            eax &= 0xffffff00;
            cout << "Max: " << max << endl;;
        }
  
        // /* Process the individual registers' value.  */
        // result = intel_check_word(name, eax, &has_level_2,
        //                            &no_level_2_or_3, cpu_features);
        // if (result != 0)
        //   return result;
  
        // result = intel_check_word (name, ebx, &has_level_2,
        //                            &no_level_2_or_3, cpu_features);
        // if (result != 0)
        //   return result;
  
        // result = intel_check_word (name, ecx, &has_level_2,
        //                            &no_level_2_or_3, cpu_features);
        // if (result != 0)
        //   return result;
  
        // result = intel_check_word (name, edx, &has_level_2,
        //                            &no_level_2_or_3, cpu_features);
        // if (result != 0)
        //   return result;
    }
  
    // if (name >= _SC_LEVEL2_CACHE_SIZE && name <= _SC_LEVEL3_CACHE_LINESIZE
    //     && no_level_2_or_3)
    //   return -1;

}

long long _cache_test_helper(int block_size, int stride, int many_times) {
    uchar dummy = 0;
    long long  c = (long long) many_times*(long long)(block_size / stride);
    chrono::nanoseconds duration(0);
    for (int times = 0; times < many_times; times++) {
        for (int i = 0; i < block_size; i = i + stride) {
            auto start = chrono::high_resolution_clock::now();
            dummy = source[i];
            auto stop = chrono::high_resolution_clock::now();
            duration += chrono::duration_cast<chrono::nanoseconds>(stop - start); 
        }
    }
    
    if (c == 0){
        return duration.count();
    }
    return duration.count() / (c);
}

void cache_test() {
    map<int, vector<long long>> results;
    int many_times = 100;
    int max_stride = 4 * MBYTE;
    for (int block_size = 16 * KBYTE; block_size <= 8 * MBYTE; block_size <<= 1) {

        results.insert(make_pair(block_size, vector<long long>()));

        for (int stride = 1; stride <= max_stride; stride <<= 1) {
                size_t duration = _cache_test_helper(block_size, stride, many_times);      
                results[block_size].push_back(duration);
        }
    }

    for (int stride = 1; stride <= max_stride; stride <<= 1) {
        cout << stride << " ";
    }

    cout << "\n";
    for (auto it: results) {
        cout << it.first << " ";
        for (int i = 0; i < it.second.size(); ++i) {
            cout << it.second[i] << " ";
        }
        cout << endl;
    }
}

int main(int argc, char* argv[]) {
    source = (uchar*)malloc(GBYTE);

    init();

    //print_cache_information();
    cache_test();

    return 0;
}

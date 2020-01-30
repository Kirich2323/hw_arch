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

//sse
 #include <immintrin.h>


using namespace std;

typedef unsigned char uchar;

const int KBYTE = 1 << 10;
const int MBYTE = 1 << 20;
const int GBYTE = 1 << 30;


uchar *source;
uchar *target;

void zero_array(void* p, size_t len) {
    memset(p, 0, len);
}

void init() {
    for (int i = 0; i < GBYTE; i+=4) {
        *(long*)(source+i) = random();
    }
    zero_array(target, GBYTE);
}

void verify(const string& fname) {
    int e = memcmp(source, target, GBYTE);
    if (e != 0) {
        cout << fname << ": bad copy --> " << e << "\n";  
        e = 30;
        cout << "source: ";
        for (int i = 5; i >= 0; --i) {
            if (e - i >= 0) {
                cout << (int)source[e-i] << ' ';
            }
        } 

        for (int i = 1; i <= 5; ++i) {
            if (e + i < GBYTE) {
                cout << (int)source[e+i] << ' ';
            }
        }
        cout << "\n";

        cout << "target: ";
        for (int i = 5; i >= 0; --i) {
            if (e - i >= 0) {
                cout << (int)target[e-i] << ' ';
            }
        } 

        for (int i = 1; i <= 5; ++i) {
            if (e + i < GBYTE) {
                cout << (int)target[e+i] << ' ';
            }
        }
        cout << "\n";
        //cout << (int)source[e] << "  !=  " << (int)target[e] << "\n";
    }
}

void clear() {
    zero_array(target, GBYTE);
}

void copy_for_loop() {
    for (int i = 0; i < GBYTE; i += 8) {
        *(unsigned long long *)(target + i) = *(unsigned long long*)(source + i);
    }
}

void copy_memcpy() {
    memcpy(target, source, GBYTE);
}

void copy_asm_movs() {
    asm volatile (
        "movl %0, %%ecx;"
        "movq %1, %%rsi;"
        "movq %2, %%rdi;"
        "rep movsq;"
        :
        : "r"(GBYTE>>3), "r"(source), "r"(target)
        :"%ecx", "%rsi", "%rdi"
    );
}

void copy_sse() {
    __m256i buffer;
    for (int i = 0; i < GBYTE; i += 32) {
        buffer = _mm256_loadu_si256((__m256i_u*)(source + i));
        _mm256_storeu_si256((__m256i_u*)(target + i), buffer);
    }
}

long long measure_function(string fname, void(*f)()) {
    auto start = chrono::high_resolution_clock::now();
    f();
    auto stop = chrono::high_resolution_clock::now();
    auto duration = chrono::duration_cast<chrono::milliseconds>(stop - start); 
    cout << fname << ": " << duration.count() << " ms." << endl;
    verify(fname);
    clear();
    return duration.count();
}

int main(int argc, char* argv[]) {
    source = (uchar*)malloc(GBYTE);
    target = (uchar*)malloc(GBYTE);

    //source = new uchar[GBYTE];
    //target = new uchar[GBYTE];

    init();

    measure_function("memcpy", copy_memcpy);
    measure_function("asm_movs", copy_asm_movs);
    measure_function("sse", copy_sse);
    measure_function("for", copy_for_loop);
    
    return 0;
}

#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <chrono>
#include <cstring>
#include <random>
#include <cpuid.h>
#include <stdio.h>

using namespace std;

typedef unsigned char uchar;

const int GIGABYTE = 1 << 30;

uchar *source;
uchar *target;

void zero_array(void* p, size_t len) {
    memset(p, 0, len);
}

void copy_for_loop() {
    for (int i = 0; i < GIGABYTE; ++i) {
        target[i] = source[i];
    }
}

void copy_memcpy() {
    memccpy(target, source, sizeof(uchar), GIGABYTE);
}

void copy_asm_movs() {
    __asm__(
        "movl $10, %eax;"
        "movl $20, %ebx;"
        "addl %ebx, %eax;"
    );
}

void measure_function(string fname, void(*f)()) {
    auto start = chrono::high_resolution_clock::now();
    f();
    auto stop = chrono::high_resolution_clock::now();
    auto duration = chrono::duration_cast<chrono::milliseconds>(stop - start); 
    cout << fname << ": " << duration.count() << " ms." << endl;
}


static inline void native_cpuid(unsigned int *eax, unsigned int *ebx,
                                unsigned int *ecx, unsigned int *edx)
{
        /* ecx is often an input as well as an output. */
        asm volatile("cpuid"
            : "=a" (*eax),
              "=b" (*ebx),
              "=c" (*ecx),
              "=d" (*edx)
            : "0" (*eax), "2" (*ecx));
}

int main(int argc, char* argv[]) {
    source = (uchar*)malloc(GIGABYTE);
    target = (uchar*)malloc(GIGABYTE);

    zero_array(source, GIGABYTE);
    zero_array(target, GIGABYTE);

    measure_function("for", copy_for_loop);
    measure_function("memcpy", copy_memcpy);
    measure_function("asm_movs", copy_asm_movs);


     unsigned eax, ebx, ecx, edx;

  eax = 1; /* processor info and feature bits */
  native_cpuid(&eax, &ebx, &ecx, &edx);

  printf("stepping %d\n", eax & 0xF);
  printf("model %d\n", (eax >> 4) & 0xF);
  printf("family %d\n", (eax >> 8) & 0xF);
  printf("processor type %d\n", (eax >> 12) & 0x3);
  printf("extended model %d\n", (eax >> 16) & 0xF);
  printf("extended family %d\n", (eax >> 20) & 0xFF);

  /* EDIT */
  eax = 3; /* processor serial number */
  native_cpuid(&eax, &ebx, &ecx, &edx);

  /** see the CPUID Wikipedia article on which models return the serial 
      number in which registers. The example here is for 
      Pentium III */
  printf("serial number 0x%08x%08x\n", edx, ecx);

    return 0;
}

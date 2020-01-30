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


int max_iter = 1e9;
const int is_jump_len = 1e8;
bool is_jump[is_jump_len];

volatile int a = 0;

void run_jumping(int m_iter=max_iter) {
    for (int i = 0; i < m_iter; ++i) {
        if (is_jump[i % is_jump_len]) {
            a = 1+1;
        }
        else {
            a = 1+2;
        }
    }
}

void clear_branch_predicion_table() {
    for (int i = 0; i < 100; ++i) {
        is_jump[i] = 0;
    }
    run_jumping(100);
}

long long measure_function(string fname, void(*f)(int a)) {
    auto start = chrono::high_resolution_clock::now();
    f(max_iter);
    auto stop = chrono::high_resolution_clock::now();
    auto duration = chrono::duration_cast<chrono::milliseconds>(stop - start); 
    cout << fname << ": " << duration.count() << " ms." << endl;
    clear_branch_predicion_table();
    return duration.count();
}

void branch_true() {
    for (int i = 0; i < is_jump_len; ++i) {
        is_jump[i] = true;
    }

    measure_function("true", run_jumping);
}

void branch_odd_even() {
    for (int i = 0; i < is_jump_len; ++i) {
        is_jump[i] = i % 2;
    }

    measure_function("odd/even", run_jumping);
}


void branch_even_odd() {
    for (int i = 0; i < is_jump_len; ++i) {
        is_jump[i] = (i+1) % 2;
    }

    measure_function("even/odd", run_jumping);
}

void branch_random() {
    for (int i = 0; i < is_jump_len; ++i) {
        is_jump[i] = random() % 2;
    }

    measure_function("random", run_jumping);
}

void branch_table() {
    vector<int> vals {1, 1, 0, 1, 1, 0, 0, 1};  
    for (int i = 0; i < is_jump_len; ++i) {
        is_jump[i] = vals[ i %vals.size() ];
    }

    measure_function("anti table", run_jumping);
}

int main(int argc, char* argv[]) {

    branch_true();
    branch_odd_even();
    branch_even_odd();
    //branch_random();
    branch_table();

    // if (a != 2) {
    //     cout << "keka";
    //     return 1;
    // }

    return 0;
}

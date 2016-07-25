//
// Created by root on 6/20/16.
//
#include "defs.h"
#include <iostream>
#include <immintrin.h>
#include <iomanip>
#include <glib.h>
#include <algorithm>
#include <iostream>
#include <string.h>
#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include <sys/time.h>
#include <sys/types.h>
#include <vector>
#include <algorithm>
#include <pthread.h>
#include <stdint.h>

using namespace std;
Strategy strategies[12] = {Strategy::NAIVE_METHOD_SMALL_N, Strategy::NAIVE_METHOD_LARGE_N, Strategy::SIMPLE_HASH, Strategy::CUCKOO_HASH, Strategy::POLYNOMIALS,
Strategy::BLOOM_FILTER, Strategy::BINARY_HASH, Strategy::POLYNOMIALS_SIMPLE_HASH, Strategy::BINARY_HASH_SIMPLE_HASH, Strategy::CUCKOO_HASH_POLYNOMIALS,
Strategy::CUCKOO_HASH_BLOOM_FILTER, Strategy::CUCKOO_HASH_BINARY_HASH};

void XOR(uint8_t *xoree1, uint8_t *xoree2, uint32_t size) {
    for (uint32_t i = 0; i < size; i++) {
        xoree1[i] = xoree1[i] ^ xoree2[i];
    }
};

void printHex(const uint8_t *arr, uint32_t size) {
    for(uint32_t k = 0; k < size; k++) {
        std::cout << setw(2) << setfill('0') << (hex) << (unsigned int) arr[k] << (dec);
    }
}

void printShares(const uint8_t *arr, uint32_t numOfShares, uint32_t maxSizeInBytes) {
    for (uint32_t i = 0; i < numOfShares; i++) {
        printHex(arr+i*maxSizeInBytes, maxSizeInBytes);
        std::cout << " ";
    }
    std::cout << std::endl;
}
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
Strategy strategies[13] = {Strategy::NAIVE_METHOD_SMALL_N, Strategy::NAIVE_METHOD_LARGE_N, Strategy::SIMPLE_HASH, Strategy::CUCKOO_HASH, Strategy::POLYNOMIALS,
Strategy::BLOOM_FILTER, Strategy::BINARY_HASH, Strategy::POLYNOMIALS_SIMPLE_HASH, Strategy::BINARY_HASH_SIMPLE_HASH, Strategy::CUCKOO_HASH_POLYNOMIALS,
Strategy::CUCKOO_HASH_BLOOM_FILTER, Strategy::CUCKOO_HASH_BINARY_HASH, Strategy::GAUSS_SIMPLE_HASH};

const std::string LOOPBACK_ADDRESS = "127.0.0.1";

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

bool isZero(uint8_t *arr, uint32_t size) {
    for (uint32_t i = 0; i < size; i++) {
        if (arr[i] != 0) {
            return false;
        }
    }
    return true;
}

uint32_t getStrategy(Strategy strategy) {
    switch (strategy) {
        case Strategy::NAIVE_METHOD_SMALL_N:
            return 0;
        case Strategy::NAIVE_METHOD_LARGE_N:
            return 1;
        case Strategy::SIMPLE_HASH:
            return 2;
        case Strategy::CUCKOO_HASH:
            return 3;
        case Strategy::POLYNOMIALS:
            return 4;
        case Strategy::BLOOM_FILTER:
            return 5;
        case Strategy::BINARY_HASH:
            return 6;
        case Strategy::POLYNOMIALS_SIMPLE_HASH:
            return 7;
        case Strategy::BINARY_HASH_SIMPLE_HASH:
            return 8;
        case Strategy::CUCKOO_HASH_POLYNOMIALS:
            return 9;
        case Strategy::CUCKOO_HASH_BLOOM_FILTER:
            return 10;
        case Strategy::CUCKOO_HASH_BINARY_HASH:
            return 11;
        case Strategy::GAUSS_SIMPLE_HASH:
            return 12;
        default:
            return 20;
    }
}

std::string getValFromConfig(ConfigFile &config, const char* section, const char* valueName) {
    std::string value = config.Value(section,valueName);
    size_t first = value.find_first_not_of(' ');
    return value.substr(first, value.length()-first);
}
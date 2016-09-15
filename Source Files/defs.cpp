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
Strategy strategies[8] = {Strategy::NAIVE_METHOD_SMALL_N, Strategy::NAIVE_METHOD_LARGE_N, Strategy::SIMPLE_HASH, Strategy::POLYNOMIALS,
Strategy::BLOOM_FILTER, Strategy::POLYNOMIALS_SIMPLE_HASH, Strategy::GAUSS_SIMPLE_HASH, Strategy::TWO_PARTY};

#ifndef DEBUG
null_out_stream cnul;
#endif

const std::string LOOPBACK_ADDRESS = "127.0.0.1";

void XOR(uint8_t *xoree1, uint8_t *xoree2, uint32_t size) {
    for (uint32_t i = 0; i < size; i++) {
        xoree1[i] = xoree1[i] ^ xoree2[i];
    }
};

void printHex(const uint8_t *arr, uint32_t size) {
    for(uint32_t k = 0; k < size; k++) {
        PRINT() << setw(2) << setfill('0') << (hex) << (unsigned int) arr[k] << (dec);
    }
}

void printShares(const uint8_t *arr, uint32_t numOfShares, uint32_t maxSizeInBytes) {
    for (uint32_t i = 0; i < numOfShares; i++) {
        printHex(arr+i*maxSizeInBytes, maxSizeInBytes);
        PRINT() << " ";
    }
    PRINT() << std::endl;
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
        case Strategy::POLYNOMIALS:
            return 3;
        case Strategy::BLOOM_FILTER:
            return 4;
        case Strategy::POLYNOMIALS_SIMPLE_HASH:
            return 5;
        case Strategy::GAUSS_SIMPLE_HASH:
            return 6;
        case Strategy::TWO_PARTY:
            return 7;
        default:
            return 20;
    }
}

std::string getValFromConfig(ConfigFile &config, const char* section, const char* valueName) {
    std::string value = config.Value(section,valueName);
    size_t first = value.find_first_not_of(' ');
    return value.substr(first, value.length()-first);
}
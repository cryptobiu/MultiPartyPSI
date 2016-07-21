//
// Created by root on 6/20/16.
//
#include "defs.h"

Strategy strategies[12] = {Strategy::NAIVE_METHOD_SMALL_N, Strategy::NAIVE_METHOD_LARGE_N, Strategy::SIMPLE_HASH, Strategy::CUCKOO_HASH, Strategy::POLYNOMIALS,
Strategy::BLOOM_FILTER, Strategy::BINARY_HASH, Strategy::POLYNOMIALS_SIMPLE_HASH, Strategy::BINARY_HASH_SIMPLE_HASH, Strategy::CUCKOO_HASH_POLYNOMIALS,
Strategy::CUCKOO_HASH_BLOOM_FILTER, Strategy::CUCKOO_HASH_BINARY_HASH};

void XOR(uint8_t *xoree1, uint8_t *xoree2, uint32_t size) {
    for (uint32_t i = 0; i < size; i++) {
        xoree1[i] = xoree1[i] ^ xoree2[i];
    }
};
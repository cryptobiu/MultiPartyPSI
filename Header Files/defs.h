//
// Created by root on 6/20/16.
//

#ifndef MULTIPARTYPSI_DEFS_H
#define MULTIPARTYPSI_DEFS_H

#include <openssl/rand.h>
#include <emmintrin.h>
#include <stdint.h>

typedef __m128i block;

void XOR(uint8_t *xoree1, uint8_t *xoree2, uint32_t size);
void printHex(const uint8_t *arr, uint32_t size);
void printShares(const uint8_t *arr, uint32_t numOfShares, uint32_t maxSizeInBytes);

#define PRINT_PARTY(partyId) std::cout << "Party " << partyId << " "

#define COPY_CTR(A) A(const A&)
#define ASSIGN_OP(A) A &operator=(const A&)


enum class Strategy : uint8_t {
    NAIVE_METHOD_SMALL_N = 0,
    NAIVE_METHOD_LARGE_N,
    SIMPLE_HASH,
    CUCKOO_HASH,
    POLYNOMIALS,
    BLOOM_FILTER,
    BINARY_HASH,
    POLYNOMIALS_SIMPLE_HASH,
    BINARY_HASH_SIMPLE_HASH,
    CUCKOO_HASH_POLYNOMIALS,
    CUCKOO_HASH_BLOOM_FILTER,
    CUCKOO_HASH_BINARY_HASH,
};

extern Strategy strategies[12];

#endif //MULTIPARTYPSI_DEFS_H



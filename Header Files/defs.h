//
// Created by root on 6/20/16.
//

#ifndef MULTIPARTYPSI_DEFS_H
#define MULTIPARTYPSI_DEFS_H

#include <openssl/rand.h>
#include <emmintrin.h>
#include <stdint.h>
#include <string>
#include <infra/ConfigFile.hpp>
#include <streambuf>
#include <ostream>

typedef __m128i block;

void XOR(uint8_t *xoree1, uint8_t *xoree2, uint32_t size);
void printHex(const uint8_t *arr, uint32_t size);
void printShares(const uint8_t *arr, uint32_t numOfShares, uint32_t maxSizeInBytes);
bool isZero(uint8_t *arr, uint32_t size);

#ifdef DEBUG
#define PRINT_PARTY(partyId) std::cout << "Party " << partyId << " "
#define PRINT() std::cout
#else
class null_out_buf : public std::streambuf {
    public:
        virtual std::streamsize xsputn (const char * s, std::streamsize n) {
            return n;
        }
        virtual int overflow (int c) {
            return 1;
        }
};

class null_out_stream : public std::ostream {
    public:
        null_out_stream() : std::ostream (&buf) {}
    private:
        null_out_buf buf;
};

extern null_out_stream cnul;       // My null stream.

#define PRINT_PARTY(partyId) cnul << "Party " << partyId << " "
#define PRINT() cnul
#endif

#define COPY_CTR(A) A(const A&)
#define ASSIGN_OP(A) A &operator=(const A&)

enum class ProgramType : uint32_t {
    OT_MPSI = 0,
    KISSNER_MPSI,
};

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
    GAUSS_SIMPLE_HASH,
};

uint32_t getStrategy(Strategy strategy);

extern Strategy strategies[13];

struct CuckooHashInfo {
    int32_t binIndex;
    int32_t tableIndex;
    uint32_t hashedBy;
};

struct BaseMPSIResult {
    double finalTime;
    uint32_t txFinal;
    uint32_t intersectionSize;
};

struct secParameters {
    uint32_t m_symSecParameter;
    uint32_t m_statSecParameter;
};

extern const std::string LOOPBACK_ADDRESS;

std::string getValFromConfig(ConfigFile &config, const char* section, const char* valueName);

#endif //MULTIPARTYPSI_DEFS_H


